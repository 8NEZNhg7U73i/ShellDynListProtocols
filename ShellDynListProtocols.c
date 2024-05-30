/** @file

  Copyright (c) 2022, Peter Kirmeier <topeterk@users.noreply.github.com>. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "ShellDynListProtocols.h"

/**
  GUID definitions
**/
STATIC CONST EFI_GUID mEFIDynListProtocolsHiiGuid = { 0X4AC75E15, 0X5DF5, 0X4F57,{ 0XAA, 0X08, 0X06, 0XD8, 0XB4, 0XB0, 0X5D, 0X1D } };



STATIC EFI_HANDLE mEFIDynListProtocolsHiiHandle;


EFI_STATUS
EFIAPI
EFIDynCmdProtocolLpHandler()
{
    //EFI_SYSTEM_TABLE  *SystemTable;
    //EFI_BOOT_SERVICES *gBS = SystemTable->BootServices;
    EFI_STATUS Status;
    UINTN HandleCount;
    EFI_HANDLE * pHandleBuffer;
    UINTN HandleIndex;
    UINTN ProtocolCount;
    EFI_GUID ** pProtocolBuffer;
    UINTN ProtocolIndex;
    EFI_DEVICE_PATH *DevicePath;
    CHAR16 *StrPath;

    // 1st get list of all handles
    Status = gBS->LocateHandleBuffer(
        AllHandles,
        NULL,
        NULL,
        &HandleCount,
        &pHandleBuffer);
    if (EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "LocateHandleBuffer failed %r\n", Status));
        return EFI_ABORTED;
    }

    // 2nd interate handles and get+print all protocols
    for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++)
    {
        Status = gBS->ProtocolsPerHandle(
            pHandleBuffer[HandleIndex],
            &pProtocolBuffer,
            &ProtocolCount);
        if (EFI_ERROR(Status))
        {
            DEBUG((EFI_D_ERROR, "ProtocolsPerHandle failed on handle #%d = 0x%x: %r\n", HandleIndex, pHandleBuffer[HandleIndex], Status));
            gBS->FreePool(pHandleBuffer);
            return EFI_ABORTED;
        }

        for (ProtocolIndex = 0; ProtocolIndex < ProtocolCount; ProtocolIndex++)
        {
            if (0 == ProtocolIndex)
                Print(L"Handle 0x%08X:   %g\n", pHandleBuffer[HandleIndex], pProtocolBuffer[ProtocolIndex]);
            else
                Print(L"                     %g\n", pProtocolBuffer[ProtocolIndex]);
            DevicePath = DevicePathFromHandle(pHandleBuffer[HandleIndex]);
            if (DevicePath == NULL)
            {
                ERR_PRINT(L"No path found\n");
            }
            StrPath = ConvertDevicePathToText(DevicePath, FALSE, FALSE);
            OUT_PRINT(StrPath);
            MemFree(StrPath);
        }

        gBS->FreePool(pProtocolBuffer);
    }

    gBS->FreePool(pHandleBuffer);

    return EFI_SUCCESS;
}


/**
  Main entry point of the dynamic EFI extension driver.

  @param[in]  ImageHandle   The firmware allocated handle for the present driver UEFI image.
  @param[in]  *SystemTable  A pointer to the EFI System table.

  @retval  EFI_SUCCESS           The driver was initialized.
  @retval  EFI_OUT_OF_RESOURCES  The "End of DXE" event could not be allocated or
                                 there was not enough memory in pool to install
                                 the EFI Dynamic Command protocol.
  @retval  EFI_LOAD_ERROR        Unable to add the HII package.

**/
EFI_STATUS
EFIAPI
EFIDynListProtocolsEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    //EFI_BOOT_SERVICES * gBS = SystemTable->BootServices;
    EFI_STATUS  Status;
    EFI_HANDLE  Handle;

    Handle = NULL;
    Status = EFIDynCmdProtocolLpHandler();
    if (!Status==0)
    {
        return EFI_LOAD_ERROR;
    }
    return EFI_SUCCESS;
}