#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

#include "main.c"

EFI_BOOT_SERVICES *BS;
EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *gopModeInfo;

static int setup_bs(void **state) {
    BS = malloc(sizeof(EFI_BOOT_SERVICES));
    BS->LocateProtocol = NULL;
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
    return 0;
}

static int teardown_bs(void **state) {
    free(BS);
    free(gop->Mode);
    free(gop);
    free(gopModeInfo);
    return 0;
}

VOID __wrap_InitializeLib(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE SystemTable) {
}

UINTN __wrap_Print(const CHAR16 *fmt, ...) {
    check_expected_ptr(fmt);
    return mock();
}

// This is always gop->SetMode.
UINT64 __wrap_efi_call2(void *func, UINT64 arg1, UINT64 arg2) {
    check_expected_ptr(func);
    check_expected(arg1);
    check_expected(arg2);
    return mock();
}

// This is always BS->LocateProtocol.
UINT64 __wrap_efi_call3(void *func, UINT64 arg1, UINT64 arg2, void **arg3) {
    check_expected_ptr(func);
    check_expected(arg1);
    check_expected(arg2);
    check_expected(arg3);
    EFI_GRAPHICS_OUTPUT_PROTOCOL **initGop = (EFI_GRAPHICS_OUTPUT_PROTOCOL **)arg3;
    *initGop = gop;
    return mock();
}

// This is always gop->QueryMode.
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

void test_unable_to_locate_gop(void **state) {
    (void)state;
    expect_value(__wrap_Print, fmt, L"Hello, world!\n");
    will_return(__wrap_Print, 0);
    // LocateProtocol
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    expect_value(__wrap_efi_call3, func, NULL);
    expect_memory(__wrap_efi_call3, arg1, &gopGuid, sizeof(EFI_GUID));
    expect_value(__wrap_efi_call3, arg2, NULL);
    expect_any(__wrap_efi_call3, arg3);
    will_return(__wrap_efi_call3, -1);
    // Print
    expect_value(__wrap_Print, fmt, L"Unable to locate GOP.\n");
    will_return(__wrap_Print, 0);
    assert_int_equal(efi_main(NULL, NULL), EFI_ABORTED);
}

void test_unable_to_get_native_mode(void **state) {
    (void)state;
    expect_value(__wrap_Print, fmt, L"Hello, world!\n");
    will_return(__wrap_Print, 0);
    // LocateProtocol
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    expect_value(__wrap_efi_call3, func, NULL);
    expect_memory(__wrap_efi_call3, arg1, &gopGuid, sizeof(EFI_GUID));
    expect_value(__wrap_efi_call3, arg2, NULL);
    expect_any(__wrap_efi_call3, arg3);
    will_return(__wrap_efi_call3, EFI_SUCCESS);
    // gop->QueryMode
    expect_value(__wrap_efi_call4, func, NULL);
    expect_memory(__wrap_efi_call4, arg1, gop, sizeof(EFI_GRAPHICS_OUTPUT_PROTOCOL));
    expect_value(__wrap_efi_call4, arg2, 0);
    expect_any(__wrap_efi_call4, arg3);
    expect_any(__wrap_efi_call4, arg4);
    will_return(__wrap_efi_call4, -1);
    // Print
    expect_value(__wrap_Print, fmt, L"Unable to get native mode.\n");
    will_return(__wrap_Print, 0);
    assert_int_equal(efi_main(NULL, NULL), EFI_ABORTED);
}

void test_printing_modes_and_fail_to_set_12(void **state) {
    (void)state;
    // Set up the gop modes info.
    gop->Mode = malloc(sizeof(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION));
    gop->Mode->Mode = 0;
    gop->Mode->MaxMode = 1;
    gop->Mode->FrameBufferBase = 0x12345678;
    gop->Mode->FrameBufferSize = 0x100000;
    // Print
    expect_value(__wrap_Print, fmt, L"Hello, world!\n");
    will_return(__wrap_Print, 0);
    // LocateProtocol
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    expect_value(__wrap_efi_call3, func, NULL);
    expect_memory(__wrap_efi_call3, arg1, &gopGuid, sizeof(EFI_GUID));
    expect_value(__wrap_efi_call3, arg2, NULL);
    expect_any(__wrap_efi_call3, arg3);
    will_return(__wrap_efi_call3, EFI_SUCCESS);
    // gop->QueryMode 1
    expect_value(__wrap_efi_call4, func, NULL);
    expect_memory(__wrap_efi_call4, arg1, gop, sizeof(EFI_GRAPHICS_OUTPUT_PROTOCOL));
    expect_value(__wrap_efi_call4, arg2, 0);
    expect_any(__wrap_efi_call4, arg3);
    expect_any(__wrap_efi_call4, arg4);
    will_return(__wrap_efi_call4, EFI_NOT_STARTED);
    // gop->SetMode 1
    expect_value(__wrap_efi_call2, func, NULL);
    expect_memory(__wrap_efi_call2, arg1, gop, sizeof(EFI_GRAPHICS_OUTPUT_PROTOCOL));
    expect_value(__wrap_efi_call2, arg2, 0);
    will_return(__wrap_efi_call2, EFI_SUCCESS);
    // gop->QueryMode 2
    expect_value(__wrap_efi_call4, func, NULL);
    expect_memory(__wrap_efi_call4, arg1, gop, sizeof(EFI_GRAPHICS_OUTPUT_PROTOCOL));
    expect_value(__wrap_efi_call4, arg2, 0);
    expect_any(__wrap_efi_call4, arg3);
    expect_any(__wrap_efi_call4, arg4);
    will_return(__wrap_efi_call4, EFI_SUCCESS);
    // gop->SetMode 2
    expect_value(__wrap_efi_call2, func, NULL);
    expect_memory(__wrap_efi_call2, arg1, gop, sizeof(EFI_GRAPHICS_OUTPUT_PROTOCOL));
    expect_value(__wrap_efi_call2, arg2, 12);
    will_return(__wrap_efi_call2, -1);
    // Print
    expect_value(__wrap_Print, fmt, L"mode %2d width %4d height %4d format %x%9s\n");
    will_return(__wrap_Print, 0);
    // Print
    expect_value(__wrap_Print, fmt, L"Unable to set mode %2d.\n");
    will_return(__wrap_Print, 0);
    assert_int_equal(efi_main(NULL, NULL), EFI_ABORTED);
}

void test_printing_modes_and_set_12(void **state) {
    (void)state;
    // Set up the gop modes info.
    gop->Mode = malloc(sizeof(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION));
    gop->Mode->Mode = 0;
    gop->Mode->MaxMode = 1;
    gop->Mode->FrameBufferBase = 0x12345678;
    gop->Mode->FrameBufferSize = 0x100000;
    gop->Mode->Info = gopModeInfo;
    // Print
    expect_value(__wrap_Print, fmt, L"Hello, world!\n");
    will_return(__wrap_Print, 0);
    // LocateProtocol
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    expect_value(__wrap_efi_call3, func, NULL);
    expect_memory(__wrap_efi_call3, arg1, &gopGuid, sizeof(EFI_GUID));
    expect_value(__wrap_efi_call3, arg2, NULL);
    expect_any(__wrap_efi_call3, arg3);
    will_return(__wrap_efi_call3, EFI_SUCCESS);
    // gop->QueryMode 1
    expect_value(__wrap_efi_call4, func, NULL);
    expect_memory(__wrap_efi_call4, arg1, gop, sizeof(EFI_GRAPHICS_OUTPUT_PROTOCOL));
    expect_value(__wrap_efi_call4, arg2, 0);
    expect_any(__wrap_efi_call4, arg3);
    expect_any(__wrap_efi_call4, arg4);
    will_return(__wrap_efi_call4, EFI_NOT_STARTED);
    // gop->SetMode 1
    expect_value(__wrap_efi_call2, func, NULL);
    expect_memory(__wrap_efi_call2, arg1, gop, sizeof(EFI_GRAPHICS_OUTPUT_PROTOCOL));
    expect_value(__wrap_efi_call2, arg2, 0);
    will_return(__wrap_efi_call2, EFI_SUCCESS);
    // gop->QueryMode 2
    expect_value(__wrap_efi_call4, func, NULL);
    expect_memory(__wrap_efi_call4, arg1, gop, sizeof(EFI_GRAPHICS_OUTPUT_PROTOCOL));
    expect_value(__wrap_efi_call4, arg2, 0);
    expect_any(__wrap_efi_call4, arg3);
    expect_any(__wrap_efi_call4, arg4);
    will_return(__wrap_efi_call4, EFI_SUCCESS);
    // Print
    expect_value(__wrap_Print, fmt, L"mode %2d width %4d height %4d format %x%9s\n");
    will_return(__wrap_Print, 0);
    // gop->SetMode 2
    expect_value(__wrap_efi_call2, func, NULL);
    expect_memory(__wrap_efi_call2, arg1, gop, sizeof(EFI_GRAPHICS_OUTPUT_PROTOCOL));
    expect_value(__wrap_efi_call2, arg2, 12);
    will_return(__wrap_efi_call2, EFI_SUCCESS);
    // Print
    expect_value(__wrap_Print,
                 fmt,
                 L"Framebuffer address %x size %d, width %d height %d "
                 L"pixelsperline %d");
    will_return(__wrap_Print, 0);
    assert_int_equal(efi_main(NULL, NULL), EFI_SUCCESS);
}

const struct CMUnitTest hello_world_efi_tests[] = {
    cmocka_unit_test_setup_teardown(test_unable_to_locate_gop, setup_bs, teardown_bs),
    cmocka_unit_test_setup_teardown(test_unable_to_get_native_mode, setup_bs, teardown_bs),
    cmocka_unit_test_setup_teardown(test_printing_modes_and_fail_to_set_12, setup_bs, teardown_bs),
    cmocka_unit_test_setup_teardown(test_printing_modes_and_set_12, setup_bs, teardown_bs),
};

int main(void) {
    return cmocka_run_group_tests(hello_world_efi_tests, NULL, NULL);
}
