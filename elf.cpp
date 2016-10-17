#include "elf.h"

int tp1, tp2, tp3;
char section_name[32], cflags[5];

void get_section_name( const Elf64_Word &toff, const Elf64_Shdr &shdr, ifstream &fin ) {
    fin.seekg( shdr.sh_offset + toff, fin.beg );
    fin.read( section_name, sizeof( section_name ) );
    return ;
}

void print_elf_header( const Elf64_Ehdr &ehdr ) {
    puts( "ELF Header:" );
    printf( "  Magic:   " ); for( int i = 0; i < 16; i++ ) printf( "%02x ", ehdr.e_ident[i] ); puts( "" );
    printf( "  Class:                             %s\n", ehdr.e_ident[4] == 1 ? "ELF32" : "ELF64" );
    printf( "  Data:                              %s\n",
            ehdr.e_ident[5] == 1 ? "2's complement, little endian" : "2's complement, big endian" );
    printf( "  Version:                           1 (current)\n" );
    printf( "  OS/ABI:                            ");
    switch( ehdr.e_ident[7] ) {
        case 0: puts( "System V ABI" ); break;
        case 1: puts( "HP-UX operating system" ); break;
        case 255: puts( "Standalone (embedded) application" ); break;
    }
    printf( "  ABI Version:                       0\n" );
    printf( "  Type:                              " );
    switch( ehdr.e_type ) {
        case 0: puts( "No file type" ); break;
        case 1: puts( "Relocatable object file" ); break;
        case 2: puts( "Executable file" ); break;
        case 3: puts( "Shared object file" ); break;
        case 4: puts( "Core file" ); break;
        case 0xFE00: puts( "Environment-specific use" ); break;
        case 0xFEFF: puts( "" ); break;
        case 0xFF00: puts( "Processor-specific use" ); break;
        case 0xFFFF: puts( "" ); break;
    }
    printf( "  Machine:                           RISC-V\n" );
    printf( "  Version:                           1\n" );
    printf( "  Entry point address:               0x%llX\n", ehdr.e_entry );
    printf( "  Start of program headers:          %d (bytes into file)\n", ehdr.e_phoff );
    printf( "  Start of section headers:          %d (bytes into file)\n", ehdr.e_shoff );
    printf( "  Flags:                             0x%X\n", ehdr.e_flags );
    printf( "  Size of this header:               %d (bytes)\n", ehdr.e_ehsize );
    printf( "  Size of program headers:           %d (bytes)\n", ehdr.e_phentsize );
    printf( "  Number of program headers:         %d\n", ehdr.e_phnum );
    printf( "  Size of section headers:           %d (bytes)\n", ehdr.e_shentsize );
    printf( "  Number of section headers:         %d\n", ehdr.e_shnum );
    printf( "  Section header string table index: %d\n", ehdr.e_shstrndx );
    return ;
}

const char * get_shtype( Elf64_Word sh_type ) {
    switch( sh_type ) {
        case 0: return "NULL";
        case 1: return "PROGBITS";
        case 2: return "SYMTAB";
        case 3: return "STRTAB";
        case 4: return "RELA";
        case 5: return "HASH";
        case 6: return "DYNAMIC";
        case 7: return "NOTE";
        case 8: return "NOBITS";
        case 9: return "REL";
        case 10: return "SHLIB";
        case 11: return "DYNSYM";
        case 0x60000000: return "LOOS";
        case 0x6FFFFFFF: return "HIOS";
        case 0x70000000: return "LOPROC";
        case 0x7FFFFFFF: return "HIPROC";
    }
    return "";
}

void get_shflags( Elf64_Xword sh_flags ) {
    int cnt = 0;
    if( sh_flags & 0x01 ) cflags[cnt++] = 'W';
    if( sh_flags & 0x02 ) cflags[cnt++] = 'A';
    if( sh_flags & 0x04 ) cflags[cnt++] = 'X';
    cflags[cnt] = 0;
    return ;
}

void print_secton_headers( int num, const Elf64_Shdr *shdr, ifstream &fin ) {
    puts( "Section Headers:" );
    printf( "  [Nr] Name               Type            Address           Offset\n" );
    printf( "       Size               EntSize         Flags  Link  Info  Align\n" );
    for( int i = 0; i < num; ++i ) {
        get_section_name( shdr[i].sh_name, shdr[tp1], fin );
        get_shflags( shdr[i].sh_flags );
        printf( "  [%2d] %-16s  %-16s %016llx  %08x\n",
                i,
                section_name,
                get_shtype( shdr[i].sh_type ),
                shdr[i].sh_addr,
                shdr[i].sh_offset
                );
        printf( "       %016llx  %016llx %3s%8d%6d%6d\n",
                shdr[i].sh_size,
                shdr[i].sh_entsize,
                cflags,
                shdr[i].sh_link,
                shdr[i].sh_info,
                shdr[i].sh_addralign
                );
    }
    printf( "Key to Flags:\n\
  W (write), A (alloc), X (execute), M (merge), S (strings)\n\
  I (info), L (link order), G (group), T (TLS), E (exclude), x (unknown)\n\
  O (extra OS processing required) o (OS specific), p (processor specific)\n");
    printf( "\nThere are no section groups in this file.\n" );
    return ;
}

const char * get_ptype( Elf64_Word p_type ) {
    switch( p_type ) {
        case 0: return "NULL";
        case 1: return "LOAD";
        case 2: return "DYNAMIC";
        case 3: return "INTERP";
        case 4: return "NOTE";
        case 5: return "SHLIB";
        case 6: return "PHDR";
        case 0x60000000: return "LOOS";
        case 0x6FFFFFFF: return "HIOS";
        case 0x70000000: return "LOPROC";
        case 0x7FFFFFFF: return "HIPROC";
    }
    return "";
}

void get_pflags( Elf64_Xword p_flags ) {
    int cnt = 0;
    if( p_flags & 0x01 ) cflags[cnt++] = 'X';
    if( p_flags & 0x02 ) cflags[cnt++] = 'W';
    if( p_flags & 0x04 ) cflags[cnt++] = 'R';
    cflags[cnt] = 0;
    return ;
}

const char * get_stype( int s_type ) {
    switch( s_type ) {
        case 0: return "NOTYPE";
        case 1: return "OBJECT";
        case 2: return "FUNC";
        case 3: return "SECTION";
        case 4: return "FILE";
        case 10: return "LOOS";
        case 12: return "HIOS";
        case 13: return "LOPROC";
        case 15: return "HIPROC";
    }
    return "";
}

const char * get_sbinding( int s_binding ) {
    switch( s_binding ) {
        case 0: return "LOCAL";
        case 1: return "GLOBAL";
        case 2: return "WEAK";
        case 10: return "LOOS";
        case 12: return "HIOS";
        case 13: return "LOPROC";
        case 15: return "HIPROC";
    }
    return "";
}

void print_symtab( const Elf64_Shdr &shdr_sym, const Elf64_Shdr &shdr_str, ifstream &fin ) {
    Elf64_Sym tsym;
    Elf64_Off tpos = shdr_sym.sh_offset;
    char str[32];
    int num = shdr_sym.sh_size / sizeof( Elf64_Sym );
    printf( "Symbol table '.symtab' contains %d entries:\n", num );
    puts( "   Num:    Value          Size Type    Bind   Vis      Ndx Name" );
    for( int i = 0; i < num; ++i ) {
        fin.seekg( tpos + i * sizeof( Elf64_Sym ), fin.beg );
        fin.read( ( char * )&tsym, sizeof( Elf64_Sym ) );
        printf( "%6d: %016llx  %4lld %-7s %-6s %-7s  ",
                i,
                tsym.st_value,
                tsym.st_size,
                get_stype( tsym.st_info & ( ( 1 << 4 ) - 1 ) ),
                get_sbinding( tsym.st_info >> 4 ),
                "DEFAULT"
                );
        if( tsym.st_shndx == 0 ) printf( "UND " );
        else if( tsym.st_shndx == 65521 ) printf( "ABS " );
        else printf( "%3d ", tsym.st_shndx );
        fin.seekg( shdr_str.sh_offset + tsym.st_name, fin.beg );
        fin.read( str, sizeof( str ) );
        printf( "%s\n", str );
    }
    return ;
}

void print_program_headers( int num, const Elf64_Phdr *phdr ) {
    puts( "Program Headers:" );
    printf( "  Type           Offset             VirtAddr           PhysAddr\n" );
    printf( "                 FileSiz            MemSiz              Flags  Align\n" );
    for( int i = 0; i < num; ++i ) {
        get_pflags( phdr[i].p_flags );
        printf( "  %-7s%8s0x%016llx 0x%016llx 0x%016llx\n",
                get_ptype( phdr[i].p_type ),
                "",
                phdr[i].p_offset,
                phdr[i].p_vaddr,
                phdr[i].p_paddr
                );
        printf( "%17s0x%016llx 0x%016llx  %-5s  %04llx\n",
                "",
                phdr[i].p_filesz,
                phdr[i].p_memsz,
                cflags,
                phdr[i].p_align
                );
    }
    return ;
}

void process_sections( Elf64_Shdr *shdr, int num, ifstream &fin ) {
    tp1 = -1;
    for( int i = 0; i < num; ++i ) {
        if( shdr[i].sh_type == SHT_PROGBITS && shdr[i].sh_flags == 0 ) continue;
        fin.seekg( shdr[i].sh_offset, fin.beg );
        int tnum = shdr[i].sh_size, pos = shdr[i].sh_addr;
        fin.read( ( char * )( mem + pos ), tnum );
        if( shdr[i].sh_type == SHT_STRTAB && shdr[i].sh_flags == 0 ) {
            if( tp1 == -1 ) tp1 = i;
            else tp2 = i;
        }
        if( shdr[i].sh_type == SHT_SYMTAB &&
                ( shdr[i].sh_flags == 0 || shdr[i].sh_flags == SHF_ALLOC ) ) tp3 = i;
        if( shdr[i].sh_type == SHT_NOBITS && shdr[i].sh_flags == ( SHF_ALLOC | SHF_WRITE ) ) {
            bss = shdr[i].sh_addr;
            bss_size = shdr[i].sh_size;
        }
    }
    return ;
}

void print_elf( const Elf64_Ehdr &ehdr, const Elf64_Shdr *shdr, const Elf64_Phdr *phdr, ifstream &fin ) {
    print_elf_header( ehdr ); puts( "" );
    print_secton_headers( ehdr.e_shnum, shdr, fin ); puts( "" );
    print_program_headers( ehdr.e_phnum, phdr ); puts( "" );
    print_symtab( shdr[tp3], shdr[tp2], fin );
    return ;
}

void analyse_elf( char filename[len_filename] ) {
    ifstream fin( filename, ios::binary | ios::in );
    Elf64_Ehdr ehdr;
    Elf64_Shdr shdr[num_section];
    Elf64_Phdr phdr[num_program];
    memset( &ehdr, 0, sizeof( ehdr ) );
    memset( shdr, 0, sizeof( shdr ) );
    fin.read( ( char * )&ehdr, sizeof( ehdr ) );
    pc = ehdr.e_entry;
    fin.seekg( ehdr.e_shoff, fin.beg );
    for( int i = 0; i < ehdr.e_shnum; ++i ) fin.read( ( char * )( shdr + i ), sizeof( Elf64_Shdr ) );
    fin.seekg( ehdr.e_phoff, fin.beg );
    for( int i = 0; i < ehdr.e_phnum; ++i ) fin.read( ( char * )( phdr + i ), sizeof( Elf64_Phdr ) );
    process_sections( shdr, ehdr.e_shnum, fin );
    reg[2] = 0xBFF0; /* SP */
    print_elf( ehdr, shdr, phdr, fin );
    fin.close();
    return ;
}
