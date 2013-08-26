#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include "../x86emu/cpu.h"
extern "C"
{
    int WINAPI CpuDecode(
        LPBYTE lpInstruction,
        SIZE_T CpuStatus
        );
};
#pragma comment(lib, "x86emu.lib")

int _tmain(int argc, _TCHAR* argv [])
{
    int i = CpuDecode((LPBYTE)"\x8B\xEC\x90\x90\x90\x90\x90", CPU_X86_32BITS);
    printf("%i", i);
    return 0;
}