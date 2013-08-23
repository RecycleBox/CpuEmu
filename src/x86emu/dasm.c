#include    <windows.h>
#include    "cpu.h"

//return instruction length
int WINAPI CpuDecode(
    LPBYTE lpInstruction,
    SIZE_T CpuStatus
    )
{
    // The Intel 386 processor sets a limit of 15 bytes on instruction length.
    int i = 0;
    DECODE_STATUS_X86 Status;
    RtlZeroMemory(&Status, sizeof(Status));
    Status.lpInstruction = lpInstruction;
    Status.nCpuStatus = CpuStatus;

    // 1. Legacy Instruction Prefixes
    for (; LegacyPrefixMap[lpInstruction[i]]; Status.nLegacyPrefix++, i++);

    // 2. REX prefix : 1 byte (64-Bit : 40 - 4F)
    //    VEX prefix : 2 byte (64-Bit : C5 ????????) (32-Bit : C5 11??????)
    //                 3 byte (64-Bit : C4 ???????? ????????) (32-Bit : C4 11?????? ????????)
    if (CpuStatus == CPU_X86_64BITS)
    {
        if ((lpInstruction[i] >= 0x40) && (lpInstruction[i] <= 0x4F))
        {
            Status.nExtendedPrefix = 1;
        }
        else if (lpInstruction[i] == 0xC5)
        {
            Status.nExtendedPrefix = 2;
        }
        else if (lpInstruction[i] == 0xC4)
        {
            Status.nExtendedPrefix = 3;
        }
    }
    else
    {
        if ((lpInstruction[i] == 0xC5) && ((lpInstruction[i] & 0xC0) == 0xC0))
        {
            Status.nExtendedPrefix = 2;
        }
        else if ((lpInstruction[i] == 0xC4) && ((lpInstruction[i] & 0xC0) == 0xC0))
        {
            Status.nExtendedPrefix = 3;
        }
    }
    i = i + Status.nExtendedPrefix;

    // 3. Opcode : 1 - , 2 - , or 3 - byte opcode
    {
        LPOPCODE(*lpOpcodeMap)[256];
        if (lpInstruction[i] != 0x0F)   //One-byte Opcode
        {
            lpOpcodeMap = &OpcodeMap[0];
        }
        else if (lpInstruction[i + 1] == 0x38)
        {
            lpOpcodeMap = &OpcodeMap[2];    //Three-byte Opcode Map (First two bytes are 0F 38H)
            i = i + 2;
        }
        else if (lpInstruction[i + 1] == 0x3A)
        {
            lpOpcodeMap = &OpcodeMap[3];    //Three-byte Opcode Map (First two bytes are 0F 3AH)
            i = i + 2;
        }
        else
        {
            lpOpcodeMap = &OpcodeMap[1];    //Two-byte Opcode Map (First Byte is 0FH)
            i = i + 1;
        }
        Status.nInstructionSize = i;

        if (*lpOpcodeMap[lpInstruction[i]])  //this byte is a defined opcode
        {
            (*lpOpcodeMap[lpInstruction[i]])(&Status);
        }
        else
        {
            return 0;
        }
    }

    return Status.nInstructionSize;
}