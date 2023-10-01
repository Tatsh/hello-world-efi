#include <setjmp.h>
#include <stdarg.h>
#include <stdlib.h>

#include <cmocka.h>

#include "main.c"

EFI_BOOT_SERVICES *BS;

int setup_bs() {
    BS = malloc(sizeof(EFI_BOOT_SERVICES));
    BS->LocateProtocol = NULL;
    return 0;
}

int teardown_bs() {
    free(BS);
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
    // check_expected(arg1);
    check_expected(arg2);
    // check_expected(arg3);
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

    // EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    // EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    expect_value(__wrap_efi_call3, func, NULL);
    // expect_value(__wrap_efi_call3, arg1, &gopGuid);
    expect_value(__wrap_efi_call3, arg2, NULL);
    // expect_value(__wrap_efi_call3, arg3, (void **)&gop);
    will_return(__wrap_efi_call3, -1);

    expect_value(__wrap_Print, fmt, L"Unable to locate GOP\n");
    will_return(__wrap_Print, 0);

    assert_int_equal(efi_main(NULL, NULL), EFI_ABORTED | 0x8000000000000000);
}

const struct CMUnitTest hello_world_efi_tests[] = {
    cmocka_unit_test_setup_teardown(test_unable_to_locate_gop, setup_bs, teardown_bs),
    // cmocka_unit_test(test_graphics_fail),
    // cmocka_unit_test(test_graphics),
};

int main(void) {
    return cmocka_run_group_tests(hello_world_efi_tests, NULL, NULL);
}
