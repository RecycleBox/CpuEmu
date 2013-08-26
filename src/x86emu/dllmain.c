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
    }

    return TRUE;
}