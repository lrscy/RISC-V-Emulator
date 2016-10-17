#include "main.h"
#include "elf.h"
#include "instruction.h"

int bss, bss_size;

int main( int argc, char *argv[] ) {
    freopen( "out.txt", "w+", stdout );

    if( argc == 2 ) {
        analyse_elf( argv[1] );
        process_instructions();
    } else puts( "Usage: elf filename" );

    printf("\nRegister is:\n");
    for( int i = 0; i < 32; ++i ) printf( "%08x ", reg[i] ); puts( "" );
    printf("\nMemory(.bss) is:\n");
    for( int i = bss; i < bss + bss_size; ++i ) {
        if( i % 4 == 0 && i ) putchar( ' ' );
        if( i % 16 == 0 && i ) puts( "" );
        printf( "%02x", mem[i] );
    }
    return 0;
}
