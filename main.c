#include <efi.h>
#include <efilib.h>
#include <stdio.h>

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    InitializeLib(ImageHandle, SystemTable);
    Print(L"Hello, world!\n");
#ifdef GOP
    // Borrowed nearly exactly from https://wiki.osdev.org/GOP.
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    UINTN status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void **)&gop);
    if (EFI_ERROR(status)) {
        Print(L"Unable to locate GOP.\n");
        return EFI_ABORTED;
    }
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
    UINTN SizeOfInfo, numModes, nativeMode;
    status = uefi_call_wrapper(
        gop->QueryMode, 4, gop, gop->Mode == NULL ? 0 : gop->Mode->Mode, &SizeOfInfo, &info);
    // This is needed to get the current video mode.
    if (status == EFI_NOT_STARTED) {
        status = uefi_call_wrapper(gop->SetMode, 2, gop, 0);
    }
    if (EFI_ERROR(status)) {
        Print(L"Unable to get native mode.\n");
        return EFI_ABORTED;
    } else {
        nativeMode = gop->Mode->Mode;
        numModes = gop->Mode->MaxMode;
        for (UINTN i = 0; i < numModes; i++) {
            status = uefi_call_wrapper(gop->QueryMode, 4, gop, i, &SizeOfInfo, &info);
            Print(L"mode %2d width %4d height %4d format %x%9s\n",
                  i,
                  info->HorizontalResolution,
                  info->VerticalResolution,
                  info->PixelFormat,
                  i == nativeMode ? "(current)" : "");
        }
        UINTN mode = 12;
        status = uefi_call_wrapper(gop->SetMode, 2, gop, mode);
        if (EFI_ERROR(status)) {
            Print(L"Unable to set mode %2d.\n", mode);
            return EFI_ABORTED;
        } else {
            Print(L"Framebuffer address %x size %d, width %d height %d "
                  L"pixelsperline %d",
                  gop->Mode->FrameBufferBase,
                  gop->Mode->FrameBufferSize,
                  gop->Mode->Info->HorizontalResolution,
                  gop->Mode->Info->VerticalResolution,
                  gop->Mode->Info->PixelsPerScanLine);
        }
    }
#endif // GOP
    return EFI_SUCCESS;
}
