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
#include "ShellDynListProtocols.h"
#include <Library/MemoryAllocationLib.h>

#include <Library/ShellLib.h>
/**
  GUID definitions
**/
STATIC CONST EFI_GUID mEFIDynListProtocolsHiiGuid = { 0X4AC75E15, 0X5DF5, 0X4F57,{ 0XAA, 0X08, 0X06, 0XD8, 0XB4, 0XB0, 0X5D, 0X1D } };



STATIC EFI_HANDLE mEFIDynListProtocolsHiiHandle;


EFI_STATUS
EFIAPI
EFIDynCmdProtocolLpHandler(EFI_HANDLE *InputHandle OPTIONAL)
{
    //EFI_SYSTEM_TABLE  *SystemTable;
    //EFI_BOOT_SERVICES *gBS = SystemTable->BootServices;
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

    if (InputHandle)
    {
        Print(L"EFI_HANDLE: %08X, value: %08X\n", InputHandle, *InputHandle);
        HandleBuffer = InputHandle;
        HandleCount = 1;
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
            DEBUG((EFI_D_ERROR, "LocateHandleBuffer failed %r\n", Status));
            return EFI_ABORTED;
        }
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
            Print(L"DevicePath is NULL\n");
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

BOOLEAN IsHexadecimal(CONST CHAR16 *Arg)
{
    // Check if the argument starts with '0x' or '0X'
    if ((Arg[0] == L'0') && ((Arg[1] == L'x') || (Arg[1] == L'X')))
    {
        Arg += 2; // Skip the '0x' or '0X'
    }

    // If the argument is now empty after '0x' or '0X', return FALSE
    if (*Arg == L'\0')
    {
        return FALSE;
    }

    // Iterate over each character in the argument
    while (*Arg != L'\0')
    {
        // Check if the character is between '0'-'9', 'A'-'F', or 'a'-'f'
        if (!(((*Arg >= L'0') && (*Arg <= L'9')) ||
              ((*Arg >= L'A') && (*Arg <= L'F')) ||
              ((*Arg >= L'a') && (*Arg <= L'f'))))
        {
            return FALSE;
        }
        Arg++;
    }
    return TRUE;
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
    UINTN ParamCount = 0;
    CHAR16 *ParamStr = NULL;
    UINT64 *ParamInt = AllocatePool(sizeof(UINT64));

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
            return Status;
        }

        ParamCount = ShellParameters->Argc;
        for (UINTN i = 1; i < ParamCount; i++)
        {
            ParamStr = ShellParameters->Argv[i];
            Print(L"ShellParameter arg [%d]: %s\n", i, ParamStr);
            Status = ShellConvertStringToUint64(ParamStr, ParamInt, FALSE, TRUE);
            if (!EFI_ERROR(Status))
            {
                Print(L"arg [%d] is vaild hex text, %08X\n", i, *ParamInt);
                Print(L"arg [%d] is vaild hex text, %d\n", i, *ParamInt);
                Status = EFIDynCmdProtocolLpHandler((EFI_HANDLE*)ParamInt);
                Print(L"EFIDynCmdProtocolLpHandler: %r\n", EFIDynCmdProtocolLpHandler);
            }
            else 
            {
                Print(L"arg [%d] is not vaild hex text, %r\n", i, Status);
            }
        }
    }
    /*
        Status = EFIDynCmdProtocolLpHandler(NULL);
        if (!EFI_ERROR (Status))
        {
            return EFI_LOAD_ERROR;
        }
    */
        return EFI_SUCCESS;
}