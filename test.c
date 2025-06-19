#include <setjmp.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <cmocka.h>

#include "main.c"

EFI_BOOT_SERVICES *BS;
EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

static int setup_bs(void **state) {
    BS = malloc(sizeof(EFI_BOOT_SERVICES));
    BS->LocateProtocol = NULL;
    gop = malloc(sizeof(EFI_GRAPHICS_OUTPUT_PROTOCOL));
    gop->QueryMode = NULL;
    gop->Mode = NULL;
    return 0;
}

static int teardown_bs(void **state) {
    free(BS);
    free(gop);
    return 0;
}

VOID __wrap_InitializeLib(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE SystemTable) {
}

UINTN __wrap_Print(const CHAR16 *fmt, ...) {
    check_expected_ptr(fmt);
    return mock();
}

UINT64 __wrap_efi_call2(void *func, UINT64 arg1, UINT64 arg2) {
    check_expected_ptr(func);
    check_expected(arg1);
    check_expected(arg2);
    return mock();
}

UINT64 __wrap_efi_call3(void *func, UINT64 arg1, UINT64 arg2, UINT64 arg3) {
    check_expected_ptr(func);
    check_expected(arg1);
    check_expected(arg2);
    check_expected(arg3);
    EFI_GRAPHICS_OUTPUT_PROTOCOL *arg3_ptr = mock_ptr_type(EFI_GRAPHICS_OUTPUT_PROTOCOL *);
    arg3 = (UINT64)gop;
    printf("arg3_ptr = %p\n", (void *)arg3);
    arg3 = cast_ptr_to_largest_integral_type(arg3_ptr);
    printf("arg3 = 0x%x\n", (unsigned int)arg3);
    return mock();
}

UINT64 __wrap_efi_call4(void *func, UINT64 arg1, UINT64 arg2, UINT64 arg3, UINT64 arg4) {
    check_expected_ptr(func);
    check_expected(arg1);
    check_expected(arg2);
    check_expected(arg3);
    check_expected(arg4);
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
    will_return(__wrap_efi_call3, cast_ptr_to_largest_integral_type(gop));
    will_return(__wrap_efi_call3, -1);
    // Print
    expect_value(__wrap_Print, fmt, L"Unable to locate GOP\n");
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
    printf("gop: %p\n", gop);
    will_return(__wrap_efi_call3, cast_ptr_to_largest_integral_type(gop));
    will_return(__wrap_efi_call3, 0);
    // gop->QueryMode
    expect_value(__wrap_efi_call4, func, NULL);
    expect_memory(__wrap_efi_call4, arg1, &gop, sizeof(EFI_GRAPHICS_OUTPUT_PROTOCOL));
    expect_value(__wrap_efi_call4, arg2, 0);
    expect_any(__wrap_efi_call4, arg3);
    expect_any(__wrap_efi_call4, arg4);
    will_return(__wrap_efi_call4, -1);
    // Print
    expect_value(__wrap_Print, fmt, "Unable to get native mode\n");
    will_return(__wrap_Print, 0);
    assert_int_equal(efi_main(NULL, NULL), EFI_ABORTED);
}

const struct CMUnitTest hello_world_efi_tests[] = {
    cmocka_unit_test_setup_teardown(test_unable_to_locate_gop, setup_bs, teardown_bs),
    cmocka_unit_test_setup_teardown(test_unable_to_get_native_mode, setup_bs, teardown_bs),
};

int main(void) {
    return cmocka_run_group_tests(hello_world_efi_tests, NULL, NULL);
}
