/** @file

  Copyright (c) 2022, Peter Kirmeier <topeterk@users.noreply.github.com>. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __SHELL_DYN_LIST_PROTOCOLS_H__
#define __SHELL_DYN_LIST_PROTOCOLS_H__


/**
This is the shell command "lp" handler function. This function handles
the command when it is invoked in the shell.

@param[in]  This             The instance of the EFI_SHELL_DYNAMIC_COMMAND_PROTOCOL.
@param[in]  SystemTable      The pointer to the UEFI system table.
@param[in]  ShellParameters  The parameters associated with the command.
@param[in]  Shell            The instance of the shell protocol used in the context of processing this command.

@return  SHELL_SUCCESS            The operation was successful.
@return  SHELL_ABORTED            Operation aborted due to internal error.

**/

/**
This is the shell command "lp" help handler function. This
function returns the formatted help for the "lp" command.
The format matchs that in Appendix B of the revision 2.1 of the
UEFI Shell Specification.

@param[in]  This      The instance of the EFI_SHELL_DYNAMIC_COMMAND_PROTOCOL.
@param[in]  Language  The pointer to the language string to use.

@return  CHAR16*  Pool allocated help string, must be freed by caller.
**/
/*
CHAR16*
EFIAPI
EFIDynCmdProtocolLpGetHelp(
    IN EFI_SHELL_DYNAMIC_COMMAND_PROTOCOL  *This,
    IN CONST CHAR8                         *Language
    );
*/

EFI_GUID gEfiBlockIoProtocolGuid = {0x964e5b21, 0x6459, 0x11d2, {0x8e, 0x39, 0x0, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};
EFI_GUID gEfiPartitionInfoProtocolGuid = {0x8cf2f62c, 0xbc9b, 0x4821, {0x80, 0x8d, 0xec, 0x9e, 0xc4, 0x21, 0xa1, 0xa0}};
EFI_GUID gEfiBlockIo2ProtocolGuid = {0xa77b2472, 0xe282, 0x4e9f, {0xa2, 0x45, 0xc2, 0xc0, 0xe2, 0x7b, 0xbc, 0xc1}};
EFI_GUID gEfiDiskIoProtocolGuid = {0xce345171, 0xba0b, 0x11d2, {0x8e, 0x4f, 0x0, 0xa0, 0xc9, 0x69, 0x72, 0x3b}};
EFI_GUID gEfiDiskIo2ProtocolGuid = {0x151c8eae, 0x7f2c, 0x472c, 0x9e, 0x54, 0x98, 0x28, 0x19, 0x4f, 0x6a, 0x88};
EFI_GUID gEfiDevicePathProtocolGuid = {0x09576E91, 0x6D3F, 0x11D2, {0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B}};

#endif /* __SHELL_DYN_LIST_PROTOCOLS_H__ */
