#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/ShellLib.h>
#include <Library/MemoryAllocationLib.h>

// UEFI Application Entry Point
EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status;
    EFI_SHELL_PARAMETERS_PROTOCOL *CommandLine = NULL;
    const LIST_ENTRY *RawValue = NULL;
    EFI_SHELL_PARAMETERS_PROTOCOL *ShellParameters = NULL;

    // Initialize the shell library
    Status = gBS->LocateProtocol(&gEfiShellParametersProtocolGuid, NULL, (VOID **)&ShellParameters);
    if (EFI_ERROR(Status))
    {
        Print(L"Failed to locate Shell Parameters Protocol\n");
        return Status;
    }

    // Get the command line passed to the application
    CommandLine = ShellParameters->CommandLine;

    // Print the original command line
    Print(L"Original Command Line: %s\n", CommandLine);

    // Example: Extract the raw value of the first argument
    Status = ShellCommandLineGetRawValue(CommandLine, &RawValue);
    if (EFI_ERROR(Status))
    {
        Print(L"Failed to retrieve raw value from command line\n");
        return Status;
    }

    // Print the raw value
    Print(L"Raw Value: %s\n", RawValue);

    // Clean up any allocated memory
    if (RawValue != NULL)
    {
        FreePool(RawValue);
    }

    return EFI_SUCCESS;
}
