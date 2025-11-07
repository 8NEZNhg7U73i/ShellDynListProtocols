/** @file

  Copyright (c) 2022, Peter Kirmeier <topeterk@users.noreply.github.com>. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ShellLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

EFI_STATUS
EFIAPI
EFIDynCmdProtocolLpHandler(IN EFI_HANDLE *InputHandleBuffer OPTIONAL, IN EFI_GUID **InputProtocolBuffer OPTIONAL, IN UINTN InputHandleCount, IN UINTN InputProtocolCount)
{
    EFI_STATUS Status;
    UINTN HandleCount;
    EFI_HANDLE * HandleBuffer;
    UINTN HandleIndex;
    UINTN ProtocolCount;
    EFI_GUID ** ProtocolBuffer;
    UINTN ProtocolIndex;
    EFI_DEVICE_PATH *DevicePath;
    CHAR16 *StrPath;
    EFI_OPEN_PROTOCOL_INFORMATION_ENTRY *OpenInfo;
    UINTN OpenInfoCount;
    UINTN OpenInfoIndex;

    if (InputHandleBuffer)
    {
    }

    // 2nd interate handles and get+print all protocols
    for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++)
    {
        Status = gBS->ProtocolsPerHandle(
            HandleBuffer[HandleIndex],
            &ProtocolBuffer,
            &ProtocolCount);
        if (EFI_ERROR(Status))
        {
            DEBUG((EFI_D_ERROR, "ProtocolsPerHandle failed on handle #%d = 0X%x: %r\n", HandleIndex, HandleBuffer[HandleIndex], Status));
            gBS->FreePool(HandleBuffer);
            return EFI_ABORTED;
        }

        DevicePath = DevicePathFromHandle(HandleBuffer[HandleIndex]);
        if (!(DevicePath == NULL))
        {
            StrPath = ConvertDevicePathToText(DevicePath, FALSE, FALSE);
            Print(L"DevicePath: %s\n", StrPath);
        }
        else
        {
        }

        for (ProtocolIndex = 0; ProtocolIndex < ProtocolCount; ProtocolIndex++)
        {
            Status = gBS->OpenProtocolInformation(HandleBuffer[HandleIndex], ProtocolBuffer[ProtocolIndex], &OpenInfo, &OpenInfoCount);
            if (!EFI_ERROR(Status))
            {
                if (0 == ProtocolIndex)
                {
                    Print(L"Handle 0X%08X: %g\n", HandleBuffer[HandleIndex], ProtocolBuffer[ProtocolIndex]);
                }
                else
                {
                    Print(L"                   %g\n", ProtocolBuffer[ProtocolIndex]);
                }
                for (OpenInfoIndex = 0; OpenInfoIndex < OpenInfoCount; OpenInfoIndex++)
                {
                    //Print(L"%p is the handle\n", HandleBuffer[HandleIndex]);
                    //Print(L"%g is the protocol GUID\n", ProtocolBuffer[ProtocolIndex]);
                    if (0 == OpenInfoIndex)
                    {
                    Print(L"                                                        Agent: 0X%08X , 0X%08X , 0X%02X, %d \n", OpenInfo[OpenInfoIndex].AgentHandle, OpenInfo[OpenInfoIndex].ControllerHandle, OpenInfo[OpenInfoIndex].Attributes, OpenInfo[OpenInfoIndex].OpenCount);
                    }
                    else
                    {
                    Print(L"                                                               0X%08X , 0X%08X , 0X%02X, %d \n", OpenInfo[OpenInfoIndex].AgentHandle, OpenInfo[OpenInfoIndex].ControllerHandle, OpenInfo[OpenInfoIndex].Attributes, OpenInfo[OpenInfoIndex].OpenCount);
                    }
/*
                    Print(L"                                                         0X%08X is the agent handle\n", OpenInfo[OpenInfoIndex].AgentHandle);
                    Print(L"                                                         0X%08X is the controller handle\n", OpenInfo[OpenInfoIndex].ControllerHandle);
                    Print(L"                                                         %d is the attributes\n", OpenInfo[OpenInfoIndex].Attributes);
                    Print(L"                                                         %d is the opencount\n", OpenInfo[OpenInfoIndex].OpenCount);
*/
                    // OpenInfo[OpenInfoIndex] is an agent that has opened a protocol
                    //
                }
            }
        }
        Print(L"\n");
    }
    gBS->FreePool(StrPath);
    gBS->FreePool(OpenInfo);
    gBS->FreePool(ProtocolBuffer);

    gBS->FreePool(HandleBuffer);

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
    EFI_STATUS  Status;

    {
    }
    return EFI_SUCCESS;
}