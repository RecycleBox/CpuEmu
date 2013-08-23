#ifndef _CPUEMU_X86EMU_X86_H_
#define _CPUEMU_X86EMU_X86_H_

#define CPU_X86_16BITS          0x01
#define CPU_X86_32BITS          0x02
#define CPU_X86_64BITS          0x04

typedef struct
{
    LPBYTE  lpInstruction;
    SIZE_T  nInstructionSize;
    SIZE_T  nCpuStatus;
    SIZE_T  nLegacyPrefix;
    SIZE_T  nExtendedPrefix;
    SIZE_T  nOpcode;
    SIZE_T  nAddressingByte;
    SIZE_T  nDisplacement;
    SIZE_T  nImmediate;
} DECODE_STATUS_X86, *LPDECODE_STATUS_X86;

typedef struct
{
    LPBYTE  lpInstruction;
    SIZE_T  nInstructionSize;
    SIZE_T  nCpuStatus;
    SIZE_T  nLegacyPrefix;
    SIZE_T  nExtendedPrefix;
    SIZE_T  nOpcode;
    SIZE_T  nAddressingByte;
    SIZE_T  nDisplacement;
    SIZE_T  nImmediate;
} ADDRESSING_INFO_X86, *LPADDRESSING_INFO_X86;

typedef int (WINAPI *LPOPCODE)(LPDECODE_STATUS_X86 lpStatus);

extern  BYTE LegacyPrefixMap[256];
extern  LPOPCODE OpcodeMap[4][256];

#endif  /* _CPUEMU_X86EMU_X86_H_ */