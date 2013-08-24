#include <windows.h>
#include "cpu.h"

int WINAPI CpuDecode(
    LPBYTE lpInstruction,
    SIZE_T CpuStatus
    );

BOOL WINAPI DllMain(
    _In_    HINSTANCE hinstDLL,
    _In_    DWORD fdwReason,
    _In_    LPVOID lpvReserved
    )
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hinstDLL);
        CpuDecode((LPBYTE)"\x8B\xEC\x90\x90\x90\x90\x90", CPU_X86_32BITS);
    }

    return TRUE;
}