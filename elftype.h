#ifndef __ELFTYPE__
#define __ELFTYPE__

#include "type.h"

// Data representation
#define Elf64_Addr      ull
#define Elf64_Off       ull
#define Elf64_Half      us
#define Elf64_Word      ui
#define Elf64_Sword     ui
#define Elf64_Xword     ull
#define Elf64_Sxword    ull

// Special Section Indices
#define SHN_UNDEF       0
#define SHN_LOPROC      0xFF00
#define SHN_HIPROC      0xFF1F
#define SHN_LOOS        0xFF20
#define SHN_HIOS        0xFF3F
#define SHN_ABS         0xFFF1
#define SHN_COMMON      0xFFF2

// Section Types, sh_type
#define SHT_NULL        0
#define SHT_PROGBITS    1
#define SHT_SYMTAB      2
#define SHT_STRTAB      3
#define SHT_RELA        4
#define SHT_HASH        5
#define SHT_DYNAMIC     6
#define SHT_NOTE        7
#define SHT_NOBITS      8
#define SHT_REL         9
#define SHT_SHLIB       10
#define SHT_DYNSYM      11
#define SHT_LOOS        0x60000000
#define SHT_HIOS        0x6FFFFFFF
#define SHT_LOPROC      0x70000000
#define SHT_HIPROC      0x7FFFFFFF

// Section Attributes, sh_flags
#define SHF_WRITE       0x1
#define SHF_ALLOC       0x2
#define SHF_EXECINSTR   0x4
#define SHF_MASKOS      0x0F000000
#define SHF_MASKPROC    0xF0000000

#endif  // __ELFTYPE__
