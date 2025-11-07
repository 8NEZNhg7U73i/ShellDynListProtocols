/** @file

  Copyright (c) 2022, Peter Kirmeier <topeterk@users.noreply.github.com>. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#pragma warning(disable: 4204)
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ShellLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "ShellDynListProtocols.h"

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
        Print(L"EFI_HANDLE: %p, value: %08X\n", InputHandleBuffer, InputHandleBuffer[0]);
        HandleBuffer = AllocateZeroPool(sizeof(EFI_HANDLE));
        if (!HandleBuffer)
        {
            Print(L"Can not allocate memory, %r\n", EFI_OUT_OF_RESOURCES);
            return EFI_OUT_OF_RESOURCES;
        }
        HandleBuffer = AllocateZeroPool(sizeof(EFI_HANDLE) * InputHandleCount);
        CopyMem(HandleBuffer, InputHandleBuffer, sizeof(EFI_HANDLE) * InputHandleCount);
        Print(L"EFI_HANDLE: %p, value: %d\n", HandleBuffer, HandleBuffer[0]);
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
        if (InputProtocolBuffer)
        {
            ProtocolBuffer = AllocateZeroPool(sizeof(EFI_GUID));
            if (!ProtocolBuffer)
            {
                Print(L"Can not allocate memory, %r\n", EFI_OUT_OF_RESOURCES);
                return EFI_OUT_OF_RESOURCES;
            }
            Print(L"InputProtocolBuffer: %g\n", (InputProtocolBuffer));
            Print(L"InputProtocolBuffer: %g\n", (*InputProtocolBuffer)[1]);
            Print(L"InputProtocolBuffer: %g\n", (*InputProtocolBuffer)[3]);
            /**
             * @var		mixed	*ProtocolBuffer
             *//**
             * @var		mixed	*InputProtocolBuffer
             *//**
             * @var		*	InputProtocolCount)
             */
            //ProtocolBuffer = AllocateZeroPool(sizeof(EFI_GUID) * InputProtocolCount);
            ProtocolBuffer = InputProtocolBuffer;
            CopyMem(ProtocolBuffer, InputProtocolBuffer, sizeof(EFI_GUID) * InputProtocolCount);
            Print(L"&InputProtocolBuffer: %p, InputProtocolBuffer: %g\n", &InputProtocolBuffer, (InputProtocolBuffer)[5]);
            Print(L"EFI_GUID: %p, value:%X\n", ProtocolBuffer, ProtocolBuffer);
            Print(L"EFI_GUID: %p, value:%g\n", ProtocolBuffer, ProtocolBuffer);
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
                gBS->FreePool(HandleBuffer);
                return EFI_ABORTED;
            }
        }

        for (ProtocolIndex = 0; ProtocolIndex < ProtocolCount; ProtocolIndex++)
        {
            Print(L"HandleBuffer[%d]: %X\n", HandleIndex, HandleBuffer[HandleIndex]);
            Print(L"ProtocolBuffer[%d]: %g\n", ProtocolIndex, ProtocolBuffer[ProtocolIndex]);
            Status = gBS->OpenProtocolInformation(HandleBuffer[HandleIndex], ProtocolBuffer[ProtocolIndex], &OpenInfo, &OpenInfoCount);
            if (!EFI_ERROR(Status))
            {
                if (0 == ProtocolIndex)
                {
                    Print(L"ProtocolIndex: %d\n", ProtocolIndex);
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
    UINTN ParamCount = 0;
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
            Print(L"ShellParameter arg [%d]: %s\n", i, ShellParameters->Argv[i]);
            Status = ShellConvertStringToUint64(ShellParameters->Argv[i], ParamInt, TRUE, TRUE);
            if (!EFI_ERROR(Status))
            {
                Print(L"arg %d is vaild hex text, %08X, Pointer location: %p, GUID: %g\n", i, *ParamInt, ParamInt, gEfiDevicePathProtocolGuid);
                EFI_GUID* test[6];
                test[0]=&gEfiPartitionInfoProtocolGuid;
                test[1]=&gEfiBlockIoProtocolGuid;
                test[2]=&gEfiDevicePathProtocolGuid;
                test[3]=&gEfiBlockIo2ProtocolGuid;
                test[4]=&gEfiDiskIoProtocolGuid;
                test[5]=&gEfiDiskIo2ProtocolGuid;
                Print(L"sizeof(test[]): %d\n",sizeof(test));
                Print(L"gEfiPartitionInfoProtocolGuid: %g\n", test[0]);
                Print(L"gEfiBlockIoProtocolGuid: %g\n", test[1]);
                EFI_GUID *test1 = (EFI_GUID *)test;
                Print(L"sizeof(test1[]): %d\n",sizeof(test1));
                Print(L"gEfiDevicePathProtocolGuid: %g\n", test1[2]);
                Print(L"gEfiBlockIo2ProtocolGuid: %g\n", test1[3]);
                EFI_GUID **test2 = (EFI_GUID **)&test1;
                Print(L"sizeof(test2[]): %d\n",sizeof(test2));
                Print(L"gEfiDiskIoProtocolGuid: %g\n", (*test2)[4]);
                Print(L"gEfiDiskIo2ProtocolGuid: %g\n", (*test2)[5]);
                Status = EFIDynCmdProtocolLpHandler((EFI_HANDLE *)ParamInt, (EFI_GUID **)test1, 1, 6);
            }
            else 
            {
                Print(L"arg [%d] is not vaild hex text, %r\n", i, Status);
            }
        }
        if (ParamCount == 1 )
        {
            EFIDynCmdProtocolLpHandler(NULL, NULL, 0, 0);
        }
    }
    /*
        Status = EFIDynCmdProtocolLpHandler(NULL);
        if (!EFI_ERROR (Status))
        {
            return EFI_LOAD_ERROR;
        }
    */
    FreePool(ParamInt);
    return EFI_SUCCESS;
}