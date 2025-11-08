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

    if (InputHandleBuffer && InputHandleCount > 0)
    {
        HandleBuffer = AllocateZeroPool(sizeof(EFI_HANDLE) * InputHandleCount);
        if (!HandleBuffer)
        {
            Print(L"Can not allocate memory, %r\n", EFI_OUT_OF_RESOURCES);
            return EFI_OUT_OF_RESOURCES;
        }
        CopyMem(HandleBuffer, InputHandleBuffer, sizeof(EFI_HANDLE) * InputHandleCount);
        HandleCount = InputHandleCount;
    }
    else
    {
        // 1st get list of all handles
        Status = gBS->LocateHandleBuffer(
            AllHandles,
            NULL,
            NULL,
            &HandleCount,
            &HandleBuffer);

        if (EFI_ERROR(Status))
        {
            Print(L"LocateHandleBuffer failed %r\n", Status);
            return EFI_ABORTED;
        }
    }
    // 2nd interate handles and get+print all protocols
    for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++)
    {
        if (InputProtocolBuffer && InputProtocolCount > 0)
        {
            ProtocolBuffer = AllocateZeroPool(sizeof(EFI_GUID));
            if (!ProtocolBuffer)
            {
                Print(L"Can not allocate memory, %r\n", EFI_OUT_OF_RESOURCES);
                return EFI_OUT_OF_RESOURCES;
            }
            ProtocolBuffer = AllocateZeroPool(sizeof(EFI_GUID) * InputProtocolCount);
            CopyMem(ProtocolBuffer, InputProtocolBuffer, sizeof(EFI_GUID) * InputProtocolCount);
            ProtocolCount = InputProtocolCount;
        }
        else
        {
            Status = gBS->ProtocolsPerHandle(
                HandleBuffer[HandleIndex],
                &ProtocolBuffer,
                &ProtocolCount);
            if (EFI_ERROR(Status))
            {
                Print(L"ProtocolsPerHandle failed on handle #%d = 0X%x: %r\n", HandleIndex, HandleBuffer[HandleIndex], Status);
                ProtocolCount=0;
            }
        }

        for (ProtocolIndex = 0; ProtocolIndex < ProtocolCount; ProtocolIndex++)
        {
            Status = gBS->OpenProtocolInformation(HandleBuffer[HandleIndex], ProtocolBuffer[ProtocolIndex], &OpenInfo, &OpenInfoCount);
            if (!EFI_ERROR(Status))
            {
                if (ProtocolIndex == 0)
                {
                    DevicePath = DevicePathFromHandle(HandleBuffer[HandleIndex]);
                    if (!(DevicePath == NULL))
                    {
                        StrPath = ConvertDevicePathToText(DevicePath, FALSE, FALSE);
                        Print(L"DevicePath: %s\n", StrPath);
                    }
                    else
                    {
                        Print(L"DevicePath is NULL\n");
                    }
                    Print(L"|  Handle  |           Protocol GUID            | |   Agent  | |Controller| |Attr| |OpenCount|  \n");
                    Print(L" 0X%08X %g\n", HandleBuffer[HandleIndex], ProtocolBuffer[ProtocolIndex]);
                }
                else
                {
                    Print(L"              %g\n", ProtocolBuffer[ProtocolIndex]);
                }
                for (OpenInfoIndex = 0; OpenInfoIndex < OpenInfoCount; OpenInfoIndex++)
                {
                    if (OpenInfoIndex == 0)
                    {
                        Print(L"                                                     0X%08X   0X%08X   0X%02X   %d \n", OpenInfo[OpenInfoIndex].AgentHandle, OpenInfo[OpenInfoIndex].ControllerHandle, OpenInfo[OpenInfoIndex].Attributes, OpenInfo[OpenInfoIndex].OpenCount);
                    }
                    else
                    {
                    Print(L"                                                     0X%08X   0X%08X   0X%02X   %d \n", OpenInfo[OpenInfoIndex].AgentHandle, OpenInfo[OpenInfoIndex].ControllerHandle, OpenInfo[OpenInfoIndex].Attributes, OpenInfo[OpenInfoIndex].OpenCount);
                    }
                }
            }
            else
            {
                Print(L"Handle: %X, Guid: %g, gBS->OpenProtocolInformation error: %r\n", HandleBuffer[HandleIndex], ProtocolBuffer[ProtocolIndex], Status);
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
    EFI_SHELL_PARAMETERS_PROTOCOL *ShellParameters;
    UINTN ParamCount;
    UINTN ParamIndex;
    UINT64 *ParamInt=AllocateZeroPool(sizeof(UINT64));
    EFI_HANDLE *Handle;

    Status = ShellInitialize();

    if (!EFI_ERROR(Status) && gEfiShellProtocol)
    {
        Status = gBS->OpenProtocol(
            ImageHandle,
            &gEfiShellParametersProtocolGuid,
            (VOID **)&ShellParameters,
            ImageHandle,
            NULL,
            EFI_OPEN_PROTOCOL_GET_PROTOCOL);

        if (EFI_ERROR(Status))
        {
            Print(L"Open EFI_SHELL_PARAMETERS_PROTOCOL protocol failed: %r\n", Status);
            ParamCount = 1;
            EFIDynCmdProtocolLpHandler(NULL, NULL, 0, 0);
        }
        else
        {
            ParamCount = ShellParameters->Argc;
            Handle = AllocateZeroPool(sizeof(EFI_HANDLE) * ParamCount);
        }

        for (ParamIndex = 1; ParamIndex < ParamCount; ParamIndex++)
        {
            Status = ShellConvertStringToUint64(ShellParameters->Argv[ParamIndex], ParamInt, TRUE, TRUE);
            if (!EFI_ERROR(Status))
            {
                Print(L"arg [%d] is vaild hex text, %08X\n", ParamIndex, *ParamInt);
                Handle[ParamIndex - 1] = *(EFI_HANDLE *)ParamInt;
            }
            else 
            {
                Print(L"arg [%d] is not vaild hex text, %r\n", ParamIndex, Status);
            }
        }

        if (ParamCount == 1)
        {
            EFIDynCmdProtocolLpHandler(NULL, NULL, 0, 0);
        }
        else
        {
            EFIDynCmdProtocolLpHandler((EFI_HANDLE *)Handle, NULL, ParamCount - 1, 0);
        }
    }
    FreePool(ParamInt);
    return EFI_SUCCESS;
}