#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

// Suppress gnu-efi's jmp_buf typedef so cmocka's setjmp dependency wins.
#define GNU_EFI_SETJMP_H
#include "main.c"

EFI_BOOT_SERVICES *BS;
EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *gopModeInfo;
static SIMPLE_INPUT_INTERFACE fakeConIn;
static SIMPLE_TEXT_OUTPUT_INTERFACE fakeConOut;
static EFI_SYSTEM_TABLE fakeSystemTable;

static int setup_bs(void **state) {
    (void)state;
    BS = malloc(sizeof(EFI_BOOT_SERVICES));
    BS->LocateProtocol = NULL;
    BS->WaitForEvent = NULL;
    gop = malloc(sizeof(EFI_GRAPHICS_OUTPUT_PROTOCOL));
    gop->QueryMode = NULL;
    gop->Mode = NULL;
    gopModeInfo = malloc(sizeof(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION));
    gopModeInfo->Version = 0;
    gopModeInfo->HorizontalResolution = 800;
    gopModeInfo->VerticalResolution = 600;
    gopModeInfo->PixelFormat = PixelFormatMax;
    gopModeInfo->PixelInformation.RedMask = 0;
    gopModeInfo->PixelInformation.GreenMask = 0;
    gopModeInfo->PixelInformation.BlueMask = 0;
    gopModeInfo->PixelInformation.ReservedMask = 0;
    gopModeInfo->PixelsPerScanLine = 800;
    fakeConIn.Reset = NULL;
    fakeConIn.ReadKeyStroke = NULL;
    fakeConIn.WaitForKey = NULL;
    fakeConOut.ClearScreen = NULL;
    fakeSystemTable.ConIn = &fakeConIn;
    fakeSystemTable.ConOut = &fakeConOut;
    return 0;
}

static int teardown_bs(void **state) {
    (void)state;
    free(BS);
    free(gop->Mode);
    free(gop);
    free(gopModeInfo);
    return 0;
}

VOID __wrap_InitializeLib(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE SystemTable) {
    (void)ImageHandle;
    (void)SystemTable;
}

UINTN __wrap_Print(const CHAR16 *fmt, ...) {
    check_expected_ptr(fmt);
    return mock();
}

// ConOut->ClearScreen.
UINT64 __wrap_efi_call1(void *func, UINT64 arg1) {
    check_expected_ptr(func);
    check_expected(arg1);
    return mock();
}

// gop->SetMode and ConIn->ReadKeyStroke.
UINT64 __wrap_efi_call2(void *func, UINT64 arg1, UINT64 arg2) {
    check_expected_ptr(func);
    check_expected(arg1);
    check_expected(arg2);
    return mock();
}

// BS->LocateProtocol and BS->WaitForEvent.
UINT64 __wrap_efi_call3(void *func, UINT64 arg1, UINT64 arg2, void **arg3) {
    check_expected_ptr(func);
    check_expected(arg1);
    check_expected(arg2);
    check_expected(arg3);
    EFI_GRAPHICS_OUTPUT_PROTOCOL **initGop = (EFI_GRAPHICS_OUTPUT_PROTOCOL **)arg3;
    *initGop = gop;
    return mock();
}

// gop->QueryMode.
UINT64 __wrap_efi_call4(void *func, void *arg1, UINT64 arg2, void **arg3, void **arg4) {
    check_expected_ptr(func);
    check_expected_ptr(arg1);
    check_expected(arg2);
    check_expected(arg3);
    check_expected(arg4);
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION **info = (EFI_GRAPHICS_OUTPUT_MODE_INFORMATION **)arg4;
    *info = gopModeInfo;
    return mock();
}

static void expect_initial_prompt_and_pause(void) {
    expect_value(__wrap_Print, fmt, L"Hello, world!\n");
    will_return(__wrap_Print, 0);
    expect_value(__wrap_Print, fmt, L"Press any key to show GOP modes.\n");
    will_return(__wrap_Print, 0);
    // BS->WaitForEvent.
    expect_value(__wrap_efi_call3, func, NULL);
    expect_value(__wrap_efi_call3, arg1, 1);
    expect_value(__wrap_efi_call3, arg2, (UINT64)&fakeConIn.WaitForKey);
    expect_any(__wrap_efi_call3, arg3);
    will_return(__wrap_efi_call3, EFI_SUCCESS);
    // ConIn->ReadKeyStroke.
    expect_value(__wrap_efi_call2, func, NULL);
    expect_value(__wrap_efi_call2, arg1, (UINT64)&fakeConIn);
    expect_any(__wrap_efi_call2, arg2);
    will_return(__wrap_efi_call2, EFI_SUCCESS);
}

static void expect_locate_protocol(UINT64 returnValue) {
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    expect_value(__wrap_efi_call3, func, NULL);
    expect_memory(__wrap_efi_call3, arg1, &gopGuid, sizeof(EFI_GUID));
    expect_value(__wrap_efi_call3, arg2, NULL);
    expect_any(__wrap_efi_call3, arg3);
    will_return(__wrap_efi_call3, returnValue);
}

static void expect_query_mode(UINT64 mode, UINT64 returnValue) {
    expect_value(__wrap_efi_call4, func, NULL);
    expect_memory(__wrap_efi_call4, arg1, gop, sizeof(EFI_GRAPHICS_OUTPUT_PROTOCOL));
    expect_value(__wrap_efi_call4, arg2, mode);
    expect_any(__wrap_efi_call4, arg3);
    expect_any(__wrap_efi_call4, arg4);
    will_return(__wrap_efi_call4, returnValue);
}

static void expect_set_mode(UINT64 mode, UINT64 returnValue) {
    expect_value(__wrap_efi_call2, func, NULL);
    expect_memory(__wrap_efi_call2, arg1, gop, sizeof(EFI_GRAPHICS_OUTPUT_PROTOCOL));
    expect_value(__wrap_efi_call2, arg2, mode);
    will_return(__wrap_efi_call2, returnValue);
}

static void expect_exit_pause_and_clear(void) {
    expect_value(__wrap_Print, fmt, L"\nPress any key to exit.\n");
    will_return(__wrap_Print, 0);
    // BS->WaitForEvent.
    expect_value(__wrap_efi_call3, func, NULL);
    expect_value(__wrap_efi_call3, arg1, 1);
    expect_value(__wrap_efi_call3, arg2, (UINT64)&fakeConIn.WaitForKey);
    expect_any(__wrap_efi_call3, arg3);
    will_return(__wrap_efi_call3, EFI_SUCCESS);
    // ConIn->ReadKeyStroke.
    expect_value(__wrap_efi_call2, func, NULL);
    expect_value(__wrap_efi_call2, arg1, (UINT64)&fakeConIn);
    expect_any(__wrap_efi_call2, arg2);
    will_return(__wrap_efi_call2, EFI_SUCCESS);
    // ConOut->ClearScreen.
    expect_value(__wrap_efi_call1, func, NULL);
    expect_value(__wrap_efi_call1, arg1, (UINT64)&fakeConOut);
    will_return(__wrap_efi_call1, EFI_SUCCESS);
}

void test_unable_to_locate_gop(void **state) {
    (void)state;
    expect_initial_prompt_and_pause();
    expect_locate_protocol(EFI_ABORTED);
    expect_value(__wrap_Print, fmt, L"Unable to locate GOP.\n");
    will_return(__wrap_Print, 0);
    assert_int_equal(efi_main(NULL, &fakeSystemTable), EFI_ABORTED);
}

void test_unable_to_get_native_mode(void **state) {
    (void)state;
    expect_initial_prompt_and_pause();
    expect_locate_protocol(EFI_SUCCESS);
    expect_query_mode(0, EFI_ABORTED);
    expect_value(__wrap_Print, fmt, L"Unable to get native mode.\n");
    will_return(__wrap_Print, 0);
    assert_int_equal(efi_main(NULL, &fakeSystemTable), EFI_ABORTED);
}

void test_unable_to_get_native_mode_after_set_mode_fails(void **state) {
    (void)state;
    expect_initial_prompt_and_pause();
    expect_locate_protocol(EFI_SUCCESS);
    expect_query_mode(0, EFI_NOT_STARTED);
    expect_set_mode(0, EFI_ABORTED);
    expect_value(__wrap_Print, fmt, L"Unable to get native mode.\n");
    will_return(__wrap_Print, 0);
    assert_int_equal(efi_main(NULL, &fakeSystemTable), EFI_ABORTED);
}

void test_full_success_path(void **state) {
    (void)state;
    gop->Mode = malloc(sizeof(EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE));
    gop->Mode->Mode = 0;
    gop->Mode->MaxMode = 1;
    gop->Mode->FrameBufferBase = 0x12345678;
    gop->Mode->FrameBufferSize = 0x100000;
    gop->Mode->Info = gopModeInfo;
    expect_initial_prompt_and_pause();
    expect_locate_protocol(EFI_SUCCESS);
    expect_query_mode(0, EFI_NOT_STARTED);
    expect_set_mode(0, EFI_SUCCESS);
    expect_query_mode(0, EFI_SUCCESS);
    expect_value(__wrap_Print, fmt, L"mode %2d width %4d height %4d format %x%9s\n");
    will_return(__wrap_Print, 0);
    expect_value(
        __wrap_Print, fmt, L"Framebuffer address %x size %d, width %d height %d pixelsperline %d");
    will_return(__wrap_Print, 0);
    expect_exit_pause_and_clear();
    assert_int_equal(efi_main(NULL, &fakeSystemTable), EFI_SUCCESS);
}

const struct CMUnitTest hello_world_efi_tests[] = {
    cmocka_unit_test_setup_teardown(test_unable_to_locate_gop, setup_bs, teardown_bs),
    cmocka_unit_test_setup_teardown(test_unable_to_get_native_mode, setup_bs, teardown_bs),
    cmocka_unit_test_setup_teardown(
        test_unable_to_get_native_mode_after_set_mode_fails, setup_bs, teardown_bs),
    cmocka_unit_test_setup_teardown(test_full_success_path, setup_bs, teardown_bs),
};

int main(void) {
    return cmocka_run_group_tests(hello_world_efi_tests, NULL, NULL);
}
