#include    <windows.h>
#include    "cpu.h"

//return instruction length
int WINAPI CpuDecodeAddressing(
    LPDECODE_STATUS_X86 lpStatus
    )
{
    BYTE Mod = lpStatus->lpInstruction[lpStatus->nInstructionSize] >> 6;
    BYTE RM = lpStatus->lpInstruction[lpStatus->nInstructionSize] & 7;

    switch (lpStatus->nCpuStatus)
    {
    case CPU_X86_16BITS:
        {
            lpStatus->nAddressingByte = 1;
            switch (Mod)
            {
            case 0:
                if (RM == 6)
                {
                    lpStatus->nDisplacement = 2;
                }
            case 1:
                lpStatus->nDisplacement = 1;
            case 2:
                lpStatus->nDisplacement = 2;
            }
        }
    case CPU_X86_32BITS:
    case CPU_X86_64BITS:
        {
            if (RM == 4 && Mod != 3)
            {
                lpStatus->nAddressingByte = 2;
            }
            else
            {
                lpStatus->nAddressingByte = 1;
            }
            switch (Mod)
            {
            case 0:
                if (RM == 5)
                {
                    lpStatus->nDisplacement = 4;
                }
            case 1:
                lpStatus->nDisplacement = 1;
            case 2:
                lpStatus->nDisplacement = 4;
            }
        }
    }

    lpStatus->nInstructionSize = lpStatus->nInstructionSize + lpStatus->nAddressingByte + lpStatus->nDisplacement;
    return 0;
}

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

    /*
    // 3. Opcode : 1 - , 2 - , or 3 - byte opcode
    {
        OPCODE_INFO_X86(*lpOpcodeMap)[256];
        if (lpInstruction[i] != 0x0F)   //One-byte Opcode
        {
            lpOpcodeMap = &OpcodeMap32[0];
        }
        else if (lpInstruction[i + 1] == 0x38)
        {
            lpOpcodeMap = &OpcodeMap32[2];    //Three-byte Opcode Map (First two bytes are 0F 38H)
            i = i + 2;
        }
        else if (lpInstruction[i + 1] == 0x3A)
        {
            lpOpcodeMap = &OpcodeMap32[3];    //Three-byte Opcode Map (First two bytes are 0F 3AH)
            i = i + 2;
        }
        else
        {
            lpOpcodeMap = &OpcodeMap32[1];    //Two-byte Opcode Map (First Byte is 0FH)
            i = i + 1;
        }
        Status.nInstructionSize = i;

        if ((*lpOpcodeMap[lpInstruction[i]]).Flags & OPCODE_X86_VAILD)  //this byte is a defined opcode
        {

        }
        else
        {
            return 0;
        }
    }
    */

    {
        LPOPCODE_X86 lpOpcode = OpcodeMap1[lpInstruction[i]];
        if (!lpOpcode->nOpcode)
        {
            return 0;
        }
        i = i + lpOpcode->nOpcode;
        Status.nInstructionSize = i;
        if (lpOpcode->fModRM)   // & OPCODE_X86_MODRM_EXIST)
        {
            CpuDecodeAddressing(&Status);
        }
        if (lpOpcode->fImmediate)
        {
            Status.nInstructionSize = Status.nInstructionSize - Status.nDisplacement;
            switch (lpOpcode->fImmediate)   // & OPCODE_X86_MODRM_EXIST)
            {
            case OPCODE_X86_IMMEDIATE_NONE:
                Status.nDisplacement = 0;
                break;
            case OPCODE_X86_IMMEDIATE_BYTE:
                Status.nDisplacement = 1;
                break;
            case OPCODE_X86_IMMEDIATE_WORD:
                Status.nDisplacement = 2;
                break;
            case OPCODE_X86_IMMEDIATE_DWORD:
                Status.nDisplacement = 4;
                break;
            case OPCODE_X86_IMMEDIATE_QWORD:
                Status.nDisplacement = 8;
                break;
            case OPCODE_X86_IMMEDIATE_DB_DW:
                Status.nDisplacement = 2;
                break;
            case OPCODE_X86_IMMEDIATE_DW_DD:
                Status.nDisplacement = 4;
                break;
            case OPCODE_X86_IMMEDIATE_DD_DQ:
                Status.nDisplacement = 4;
                break;
            case OPCODE_X86_IMMEDIATE_DW_DD_DQ:
                Status.nDisplacement = 4;
                break;
            case OPCODE_X86_IMMEDIATE_FARPTR:
                Status.nDisplacement = 6;
                break;
            case OPCODE_X86_IMMEDIATE_3_BYTE:
                Status.nDisplacement = 3;
                break;
            }
            Status.nInstructionSize = Status.nInstructionSize + Status.nDisplacement;
        }
    }

    return Status.nInstructionSize;
}