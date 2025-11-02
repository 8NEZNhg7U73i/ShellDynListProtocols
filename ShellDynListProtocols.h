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

#include <Uefi.h>

#include <Protocol/ShellDynamicCommand.h>

#include <Library/UefiLib.h> // Print()
#include <Library/HiiLib.h>
#include <Library/DebugLib.h>

#define OUT_PRINT(format, ...) ShellPrintExMod(-1, -1, format, ##__VA_ARGS__)
#define ERR_PRINT(format, ...) ShellPrintExMod(-1, -1, L"%E" format L"%N", ##__VA_ARGS__)

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

extern EFI_BOOT_SERVICES *gBS;
extern EFI_SYSTEM_TABLE *gST;

EFI_STATUS
EFIAPI
ShellPrintExMod(
  IN INT32                Col OPTIONAL,
  IN INT32                Row OPTIONAL,
  IN CONST CHAR16         *Format,
  ...
  )
{
  VA_LIST           Marker;
  EFI_STATUS        RetVal;
  if (Format == NULL) {
    return (EFI_INVALID_PARAMETER);
  }
  VA_START (Marker, Format);
  RetVal = InternalShellPrintWorkerMod(Col, Row, Format, Marker);
  VA_END(Marker);
  return(RetVal);
}

#endif /* __SHELL_DYN_LIST_PROTOCOLS_H__ */
