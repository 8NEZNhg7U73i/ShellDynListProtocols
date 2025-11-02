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
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ShellLib.h>
#include <Library/PrintLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Protocol/DevicePathFromText.h>
#include <Protocol/DevicePathToText.h>
#include <Protocol/DevicePathUtilities.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/ShellParameters.h>
#include <Protocol/PartitionInfo.h>
#include <Protocol/BlockIo.h>
/**
  GUID definitions
**/
STATIC CONST EFI_GUID mEFIDynListProtocolsHiiGuid = {0X4AC75E15, 0X5DF5, 0X4F57, {0XAA, 0X08, 0X06, 0XD8, 0XB4, 0XB0, 0X5D, 0X1D}};

STATIC EFI_HANDLE mEFIDynListProtocolsHiiHandle;

EFI_STATUS
EFIAPI
EFIDynCmdProtocolLpHandlerbyhandle(IN EFI_HANDLE InputHandle)
{
    // EFI_SYSTEM_TABLE  *SystemTable;
    // EFI_BOOT_SERVICES *gBS = SystemTable->BootServices;
    EFI_STATUS Status;
    UINTN ProtocolCount;
    EFI_GUID **ProtocolBuffer;
    UINTN ProtocolIndex;
    EFI_DEVICE_PATH *DevicePath;
    CHAR16 *StrPath;
    EFI_OPEN_PROTOCOL_INFORMATION_ENTRY *OpenInfo;
    UINTN OpenInfoCount;
    UINTN OpenInfoIndex;

    // 2nd interate handles and get+print all protocols
    Status = gBS->ProtocolsPerHandle(
        InputHandle,
        &ProtocolBuffer,
        &ProtocolCount);
    if (EFI_ERROR(Status))
    {
        DEBUG((EFI_D_ERROR, "ProtocolsPerHandle failed on handle #%d = 0X%x: %r\n", HandleIndex, InputHandle, Status));
        return EFI_ABORTED;
    }

    DevicePath = DevicePathFromHandle(InputHandle);
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
        Status = gBS->OpenProtocolInformation(InputHandle, ProtocolBuffer[ProtocolIndex], &OpenInfo, &OpenInfoCount);
        if (!EFI_ERROR(Status))
        {
            if (0 == ProtocolIndex)
            {
                Print(L"Handle 0X%08X: %g\n", InputHandle, ProtocolBuffer[ProtocolIndex]);
            }
            else
            {
                Print(L"                   %g\n", ProtocolBuffer[ProtocolIndex]);
            }
            for (OpenInfoIndex = 0; OpenInfoIndex < OpenInfoCount; OpenInfoIndex++)
            {
                // Print(L"%p is the handle\n", InputHandle);
                // Print(L"%g is the protocol GUID\n", ProtocolBuffer[ProtocolIndex]);
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

    gBS->FreePool(StrPath);
    gBS->FreePool(OpenInfo);
    gBS->FreePool(ProtocolBuffer);

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

BOOLEAN IsHexadecimal(CHAR16 *Arg)
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

EFI_STATUS
EFIAPI
EFIDynListProtocolsEntryPoint(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status;
    CHAR16 *ArgValue = AllocateZeroPool(sizeof(CHAR16));
    LIST_ENTRY *Package;
    UINTN Index = 1; // Start from index 1 to skip the command name itself

    Status = ShellInitialize();

    if (!EFI_ERROR(Status) && !(gEfiShellProtocol == NULL))
    {
        while (ArgValue)
        {
            FreePool(ArgValue);
            ShellCommandLineGetRawValue(Package, Index);
            Print(L"Argument %d: %08X\n", Index, ArgValue); // Print or process the argument value
            if (!EFI_ERROR(IsHexadecimal(ArgValue)))
            {
                EFIDynCmdProtocolLpHandlerbyhandle(ArgValue);
            }
            else
            {
                Print(L"skip args: %s", ArgValue);
            }
            Index++; // Move to the next argument
        }
    }
    return EFI_SUCCESS
}