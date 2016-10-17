#include "instruction.h"
#include <unistd.h>

int num_func;
int num_ins;

void LUI( ull ins ) {
    ull imm = ( ins >> 5 ) << 12;
    int rd = ins & ( ( 1 << 5 ) - 1 );
    reg[rd] = imm;
    if( ( reg[rd] >> 31 ) == 1 ) reg[rd] += 0xffffffff00000000;
    return ;
}

void AUIPC( ull ins ) {
    ull imm = ( ins >> 5 ) << 12;
    if( ( imm >> 31 ) == 1 ) imm += 0xffffffff00000000;
    int rd = ins & ( ( 1 << 5 ) - 1 );
    reg[rd] = imm + pc;
    return ;
}

void JAL( ull ins ) {
    ull imm = 0;
    int rd = ins & ( ( 1 << 5 ) - 1 );
    imm |= ( ins >> 24 ) << 20;
    imm |= ( ( ins >> 5 ) & ( ( 1 << 8 ) - 1 ) ) << 12;
    imm |= ( ( ins >> 13 ) & 1 ) << 11;
    imm |= ( ( ins >> 14 ) & ( ( 1 << 10 ) - 1 ) ) << 1;
    if( ( imm >> 20 ) == 1 ) imm += 0xffffffffffe00000;
    if( rd ) reg[rd] = pc + 4;
    pc += imm;
    if( imm == 0 ) { --num_func; return ; }
    if( rd == 1 ) num_func++;
    return ;
}

void JALR( ull ins ) {
    ull imm = ins >> 13;
    int rs1 = ( ins >> 8 ) & ( ( 1 << 5 ) - 1 );
    int rd = ins & ( ( 1 << 5 ) - 1 );
    if( ( imm >> 11 ) == 1 ) imm += 0xfffffffffffff000;
    if( rd ) reg[rd] = pc + 4;
    pc = ( imm + reg[rs1] ) & 0xfffffffffffffffe;
    if( rd == 1 ) num_func++;
    else if( rd == 0 && rs1 == 1 ) num_func--;
    return ;
}

void BEQ( int rs1, int rs2, ull imm ) {
    if( reg[rs1] == reg[rs2] ) pc += imm;
    else pc += 4;
    return ;
}

void BNE( int rs1, int rs2, ull imm ) {
    if( reg[rs1] != reg[rs2] ) pc += imm;
    else pc += 4;
    return ;
}

void BLT( int rs1, int rs2, ull imm ) {
    // take the branch if rs1 is less than rs2, using signed comparison
    if( ( long long )reg[rs1] < ( long long )reg[rs2] ) pc += imm;
    else pc += 4;
    return ;
}

void BGE( int rs1, int rs2, ull imm ) {
    // take the branch if rs1 is greater than or equal to rs2, using signed comparison
    if( ( long long )reg[rs1] >= ( long long )reg[rs2] ) pc += imm;
    else pc += 4;
    return ;
}

void BLTU( int rs1, int rs2, ull imm ) {
    // take the branch if rs1 is less than rs2, using unsigned comparison
    if( reg[rs1] < reg[rs2] ) pc += imm;
    else pc += 4;
    return ;
}

void BGEU( int rs1, int rs2, ull imm ) {
    // take the branch if rs1 is greater than or equal to rs2, using unsigned comparison
    if( reg[rs1] >= reg[rs2] ) pc += imm;
    else pc += 4;
    return ;
}

void process_branch( ull ins ) {
    ull imm = ( ins >> 24 ) << 12;
    imm |= ( ( ins >> 18 ) & ( ( 1 << 6 ) - 1 ) ) << 5;
    int rs2 = ( ins >> 13 ) & ( ( 1 << 5 ) - 1 );
    int rs1 = ( ins >> 8 ) & ( ( 1 << 5 ) - 1 );
    int funct3 = ( ins >> 5 ) & ( ( 1 << 3 ) - 1 );
    imm |= ( ins & 1 ) << 11;
    imm |= ( ( ins >> 1 ) & ( ( 1 << 4 ) - 1 ) ) << 1;
    if( ( imm >> 12 ) == 1 ) imm += 0xffffffffffffe000;
    switch( funct3 ) {
        case 0x0: BEQ( rs1, rs2, imm );  break;     /* BEQ */
        case 0x1: BNE( rs1, rs2, imm );  break;     /* BNE */
        case 0x4: BLT( rs1, rs2, imm );  break;     /* BLT */
        case 0x5: BGE( rs1, rs2, imm );  break;     /* BGE */
        case 0x6: BLTU( rs1, rs2, imm ); break;     /* BLTU */
        case 0x7: BGEU( rs1, rs2, imm ); break;     /* BGEU */
    }
    return ;
}

void LB( int rd, int rs1, ull imm ) {
    // loads a 8-bit value from memory and sign-extends this to 64 bits
    ull addr = reg[rs1] + imm;
    reg[rd] = 0;
    reg[rd] |= mem[addr];
    if( ( reg[rd] >> 7 ) == 1 ) reg[rd] += 0xfffffffffffffff0;
    return ;
}

void LH( int rd, int rs1, ull imm ) {
    // loads a 16-bit value from memory and sign-extends this to 64 bits
    ull addr = reg[rs1] + imm;
    reg[rd] = 0;
    for( int i = 1; i >= 0; --i ) {
        reg[rd] <<= 8;
        reg[rd] |= mem[addr + i];
    }
    if( ( reg[rd] >> 15 ) == 1 ) reg[rd] += 0xffffffffffffff00;
    return ;
}

void LW( int rd, int rs1, ull imm ) {
    // loads a 32-bit value from memory and sign-extends this to 64 bits
    ull addr = reg[rs1] + imm;
    reg[rd] = 0;
    for( int i = 3; i >= 0; --i ) {
        reg[rd] <<= 8;
        reg[rd] |= mem[addr + i];
    }
    if( ( reg[rd] >> 31 ) == 1 ) reg[rd] += 0xffffffff00000000;
    return ;
}

void LD( int rd, int rs1, ull imm ) {
    // loads a 64-bit value from memory
    ull addr = reg[rs1] + imm;
    reg[rd] = 0;
    for( int i = 7; i >= 0; --i ) {
        reg[rd] <<= 8;
        reg[rd] |= mem[addr + i];
    }
    return ;
}

void LBU( int rd, int rs1, ull imm ) {
    // loads a 8-bit value from memory and zero-extends this to 64 bits
    ull addr = reg[rs1] + imm;
    reg[rd] = 0;
    reg[rd] |= mem[addr];
    return ;
}

void LHU( int rd, int rs1, ull imm ) {
    // loads a 16-bit value from memory and zero-extends this to 64 bits
    ull addr = reg[rs1] + imm;
    reg[rd] = 0;
    for( int i = 1; i >= 0; --i ) {
        reg[rd] <<= 8;
        reg[rd] |= mem[addr + i];
    }
    return ;
}

void LWU( int rd, int rs1, ull imm ) {
    // loads a 32-bit value from memory and zero-extends this to 64 bits
    ull addr = reg[rs1] + imm;
    reg[rd] = 0;
    for( int i = 3; i >= 0; --i ) {
        reg[rd] <<= 8;
        reg[rd] |= mem[addr + i];
    }
    return ;
}

void process_load( ull ins ) {
    ull imm = ins >> 13;
    int rs1 = ( ins >> 8 ) & ( ( 1 << 5 ) - 1 );
    int funct3 = ( ins >> 5 ) & ( ( 1 << 3 ) - 1 );
    int rd = ins & ( ( 1 << 5 ) - 1 );
    if( ( imm >> 11 ) == 1 ) imm += 0xfffffffffffff000;
    switch( funct3 ) {
        case 0x0: LB( rd, rs1, imm );  break;       /* LB */
        case 0x1: LH( rd, rs1, imm );  break;       /* LH */
        case 0x2: LW( rd, rs1, imm );  break;       /* LW */
        case 0x3: LD( rd, rs1, imm );  break;       /* LD */
        case 0x4: LBU( rd, rs1, imm ); break;       /* LBU */
        case 0x5: LHU( rd, rs1, imm ); break;       /* LHU */
        case 0x6: LWU( rd, rs1, imm ); break;       /* LWU */
    }
    return ;
}

void SB( int rs1, int rs2, ull imm ) {
    // store 8-bit values from the low bits of register rs2 to memory
    ull addr = reg[rs1] + imm;
    mem[addr] = reg[rs2];
    return ;
}

void SH( int rs1, int rs2, ull imm ) {
    // store 16-bit values from the low bits of register rs2 to memory
    ull addr = reg[rs1] + imm;
    ull r = reg[rs2];
    for( int i = 0; i <= 1; ++i) {
        mem[addr + i] = r;
        r >>= 8;
    }
    return ;
}

void SW( int rs1, int rs2, ull imm ) {
    // store 32-bit values from the low bits of register rs2 to memory
    ull addr = reg[rs1] + imm;
    ull r = reg[rs2];
    for( int i = 0; i <= 3; ++i) {
        mem[addr + i] = r;
        r >>= 8;
    }
    return ;
}

void SD( int rs1, int rs2, ull imm ) {
    // store 64-bit values from the low bits of register rs2 to memory
    ull addr = reg[rs1] + imm;
    ull r = reg[rs2];
    for( int i = 0; i <= 7; ++i) {
        mem[addr + i] = r;
        r >>= 8;
    }
    return ;
}

void process_save( ull ins ) {
    ull imm = ( ins >> 18 ) << 5;
    int rs2 = ( ins >> 13 ) & ( ( 1 << 5 ) - 1 );
    int rs1 = ( ins >> 8 ) & ( ( 1 << 5 ) - 1 );
    int funct3 = ( ins >> 5 ) & ( ( 1 << 3 ) - 1 );
    imm |= ( ins & ( ( 1 << 5 ) - 1 ) );
    if( ( imm >> 11 ) == 1 ) imm += 0xfffffffffffff000;
    switch( funct3 ) {
        case 0x0: SB( rs1, rs2, imm ); break;       /* SB */
        case 0x1: SH( rs1, rs2, imm ); break;       /* SH */
        case 0x2: SW( rs1, rs2, imm ); break;       /* SW */
        case 0x3: SD( rs1, rs2, imm ); break;       /* SD */
    }
    return ;
}

void ADDI( int rd, int rs1, ull imm ) {
    // adds the sign-extended 12-bit immediate to register rs1
    // the result is simply the low 32-bits of the result
    reg[rd] = reg[rs1] + imm;
    return ;
}

void SLLI( int rd, int rs1, int shamt ) {
    // logical left shift
    reg[rd] = reg[rs1] << shamt;
    return ;
}

void SLLI64( int rd, int rs1, int shamt ) {
    shamt += 1 << 5;
    reg[rd] = reg[rs1] << shamt;
    return ;
}

void SLLI( int funct7, int rd, int rs1, int shamt ) {
    if( funct7 == 0x0 ) SLLI( rd, rs1, shamt );
    else SLLI64( rd, rs1, shamt );
    return ;
}

void SLTI( int rd, int rs1, ull imm ) {
    // writing 1 to rd if rs1 < imm(sign-extended), else 0 is written to rd
    // signed compares
    if( ( long long )reg[rs1] < ( long long )imm ) reg[rd] = 0xffffffffffffffff;
    else reg[rd] = 0;
    return ;
}

void SLTIU( int rd, int rs1, ull imm ) {
    // unsigned compares
    if( reg[rs1] < imm ) reg[rd] = 0xffffffffffffffff;
    else reg[rd] = 0;
    return ;
}

void XORI( int rd, int rs1, ull imm ) {
    reg[rd] = reg[rs1] ^ imm;
    return ;
}

void SRLI( int rd, int rs1, int shamt ) {
    // logical right shift
    reg[rd] = reg[rs1] >> shamt;
    return ;
}

void SRLI64( int rd, int rs1, int shamt ) {
    reg[rd] = reg[rs1] >> shamt;
    return ;
}

void SRAI( int rd, int rs1, int shamt ) {
    // arithmetic right shift
    reg[rd] = ( ( long long )reg[rs1] ) >> shamt;
    return ;
}

void SRAI64( int rd, int rs1, int shamt ) {
    reg[rd] = ( ( long long )reg[rs1] ) >> shamt;
    return ;
}

void SR_I( int funct7, int rd, int rs1, int shamt ) {
    if( funct7 == 0x0 ) SRLI( rd, rs1, shamt );
    else if( funct7 == 0x01 ) SRLI64( rd, rs1, shamt | ( 1 << 5 ) );
    else if( funct7 == 0x20 ) SRAI( rd, rs1, shamt );
    else SRAI64( rd, rs1, shamt | ( 1 << 5 ) );
    return ;
}

void ORI( int rd, int rs1, ull imm ) {
    reg[rd] = reg[rs1] | imm;
    return ;
}

void ANDI( int rd, int rs1, ull imm ) {
    reg[rd] = reg[rs1] & imm;
    return ;
}

void process_op_imm( ull ins ) {
    ull  imm = ins >> 13;
    int  rs1 = ( ins >> 8 ) & ( ( 1 << 5 ) - 1 );
    int  funct3 = ( ins >> 5 ) & ( ( 1 << 3 ) - 1 );
    int  rd = ins & ( ( 1 << 5 ) - 1 );
    int  shamt = imm & ( ( 1 << 5 ) - 1 );
    int  funct7 = imm >> 5;
    if( ( imm >> 11 ) == 1 ) imm += 0xfffffffffffff000;
    switch( funct3 ) {
        case 0x0: ADDI( rd, rs1, imm );           break;    /* ADDI */
        case 0x1: SLLI( funct7, rd, rs1, shamt ); break;    /* SLLI SLLI64 */
        case 0x2: SLTI( rd, rs1, imm );           break;    /* SLTI */
        case 0x3: SLTIU( rd, rs1, imm );          break;    /* SLTIU */
        case 0x4: XORI( rd, rs1, imm );           break;    /* XORI */
        case 0x5: SR_I( funct7, rd, rs1, shamt ); break;    /* SRLI SRAI */
        case 0x6: ORI( rd, rs1, imm );            break;    /* ORI */
        case 0x7: ANDI( rd, rs1, imm );           break;    /* ANDI */
    }
    return ;
}

void ADD( int rd, int rs1, int rs2 ) {
    reg[rd] = reg[rs1] + reg[rs2];
    return ;
}

void SUB( int rd, int rs1, int rs2 ) {
    reg[rd] = reg[rs1] - reg[rs2];
    return ;
}

void ADD_SUB( int funct7, int rd, int rs1, int rs2 ) {
    if( funct7 == 0x0 ) ADD( rd, rs1, rs2 );
    else SUB( rd, rs1, rs2 );
    return ;
}

void SLL( int rd, int rs1, int rs2 ) {
    reg[rs1] <<= reg[rs2] & ( ( 1 << 6 ) - 1 );
    return ;
}

void SLT( int rd, int rs1, int rs2 ) {
    if( ( long long )reg[rs1] < ( long long )reg[rs2] ) reg[rd] = 0xffffffffffffffff;
    else reg[rd] = 0;
    return ;
}

void SLTU( int rd, int rs1, int rs2 ) {
    if( reg[rs1] < reg[rs2] ) reg[rd] = 0xffffffffffffffff;
    else reg[rd] = 0;
    return ;
}

void XOR( int rd, int rs1, int rs2 ) {
    reg[rd] = reg[rs1] ^ reg[rs2];
    return ;
}

void SRL( int rd, int rs1, int rs2 ) {
    reg[rs1] >>= reg[rs2] & ( ( 1 << 6 ) - 1 );
    return ;
}

void SRA( int rd, int rs1, int rs2 ) {
    reg[rs1] = ( long long )reg[rs1] >> reg[rs2] & ( ( 1 << 6 ) - 1 );
    return ;
}

void SR_( int funct7, int rd, int rs1, int rs2 ) {
    if( funct7 == 0x0 ) SRL( rd, rs1, rs2 );
    else SRA( rd, rs1, rs2 );
    return ;
}

void OR( int rd, int rs1, int rs2 ) {
    reg[rd] = reg[rs1] | reg[rs2];
    return ;
}

void AND( int rd, int rs1, int rs2 ) {
    reg[rd] = reg[rs1] & reg[rs2];
    return ;
}

void process_op( ull ins ) {
    int  funct7 = ins >> 18;
    int  rs2 = ( ins >> 13 ) & ( ( 1 << 5 ) - 1 );
    int  rs1 = ( ins >> 8 ) & ( ( 1 << 5 ) - 1 );
    int  funct3 = ( ins >> 5 ) & ( ( 1 << 3 ) - 1 );
    int  rd = ins & ( ( 1 << 5 ) - 1 );
    switch( funct3 ) {
        case 0x0: ADD_SUB( funct7, rd, rs1, rs2 ); break;   /* ADD SUB */
        case 0x1: SLL( rd, rs1, rs2 );             break;   /* SLL */
        case 0x2: SLT( rd, rs1, rs2 );             break;   /* SLT */
        case 0x3: SLTU( rd, rs1, rs2 );            break;   /* SLTU */
        case 0x4: XOR( rd, rs1, rs2 );             break;   /* XOR */
        case 0x5: SR_( funct7, rd, rs1, rs2 );     break;   /* SRLI SRAI */
        case 0x6: OR( rd, rs1, rs2 );              break;   /* OR */
        case 0x7: AND( rd, rs1, rs2 );             break;   /* AND */
    }
    return ;
}

void ADDIW( int rd, int rs1, ull imm ) {
    if( ( imm >> 11 ) == 1 ) imm += 0xfffffffffffff000;
    reg[rd] = reg[rs1] + imm;
    if( ( reg[rd] >> 31 ) == 1 ) reg[rd] += 0xffffffff00000000;
    return ;
}

void SLLIW( int rd, int rs1, int shamt ) {
    reg[rs1] <<= shamt;
    if( ( reg[rd] >> 31 ) == 1 ) reg[rd] += 0xffffffff00000000;
    return ;
}

void SRLIW( int rd, int rs1, int shamt ) {
    reg[rs1] >>= shamt;
    if( ( reg[rd] >> 31 ) == 1 ) reg[rd] += 0xffffffff00000000;
    return ;
}

void SRAIW( int rd, int rs1, int shamt ) {
    reg[rs1] = ( long long )reg[rs1] >> shamt;
    if( ( reg[rd] >> 31 ) == 1 ) reg[rd] += 0xffffffff00000000;
    return ;
}

void SR_IW( int funct7, int rd, int rs1, int shamt ) {
    if( funct7 == 0x0 ) SRLIW( rd, rs1, shamt );
    else SRAIW( rd, rs1, shamt );
    return ;
}

void process_op_imm64( ull ins ) {
    ull imm = ins >> 13;
    int funct7 = imm >> 5;
    int shamt = imm & ( ( 1 << 5 ) - 1 );
    int rs1 = ( ins >> 8 ) & ( ( 1 << 5 ) - 1 );
    int funct3 = ( ins >> 5 ) & ( ( 1 << 3 ) - 1 );
    int rd = ins & ( ( 1 << 5 ) - 1 );
    switch( funct3 ) {
        case 0x0: ADDIW( rd, rs1, imm );           break;   /* ADDIW */
        case 0x1: SLLIW( rd, rs1, shamt );         break;   /* SLLIW */
        case 0x5: SR_IW( funct7, rd, rs1, shamt ); break;   /* SRLIW SRAIW */
    }
    return ;
}

void ADDW( int rd, int rs1, int rs2 ) {
    reg[rd] = reg[rs1] + reg[rs2];
    if( ( reg[rd] >> 31 ) == 1 ) reg[rd] += 0xffffffff00000000;
    return ;
}

void SUBW( int rd, int rs1, int rs2 ) {
    reg[rd] = reg[rs1] - reg[rs2];
    if( ( reg[rd] >> 31 ) == 1 ) reg[rd] += 0xffffffff00000000;
    return ;
}

void ADD_SUBW( int funct7, int rd, int rs1, int rs2 ) {
    if( funct7 == 0x0 ) ADDW( rd, rs1, rs2 );
    else SUBW( rd, rs1, rs2 );
    return ;
}

void SLLW( int rd, int rs1, int rs2 ) {
    reg[rs1] <<= reg[rs2] & ( ( 1 << 5 ) - 1 );
    return ;
}

void SRLW( int rd, int rs1, int rs2 ) {
    reg[rs1] >>= reg[rs2] & ( ( 1 << 5 ) - 1 );
    return ;
}

void SRAW( int rd, int rs1, int rs2 ) {
    reg[rs1] = ( long long )reg[rs1] >> ( reg[rs2] & ( ( 1 << 5 ) - 1 ) );
    return ;
}

void SR_W( int funct7, int rd, int rs1, int rs2 ) {
    if( funct7 == 0x0 ) SRLW( rd, rs1, rs2 );
    else SRAW( rd, rs1, rs2 );
    return ;
}

void read() {
    return ;
}

void write() {
    char *buf = ( char * )malloc( sizeof( char ) * reg[12] );
    for( int i = 0; i <= reg[12]; i++) {
        buf[reg[12] - 1 - i] = mem[reg[11] + i];
    }
    reg[10] = _write( reg[10], buf, reg[12] );
    return ;
}

void exit() {
    //_exit(0);
    return ;
}

void time() {
    return ;
}

void SCALL( ull ins ) {
    if( reg[17] == 63 ) read();
    if( reg[17] == 64 ) write();
    if( reg[17] == 93 ) exit();
    if( reg[17] == 169 ) time();
    return ;
}

void process_op64( ull ins ) {
    int funct7 = ins >> 18;
    int rs2 = ( ins >> 13 ) & ( ( 1 << 5 ) - 1 );
    int rs1 = ( ins >> 8 ) & ( ( 1 << 5 ) - 1 );
    int funct3 = ( ins >> 5 ) & ( ( 1 << 3 ) - 1 );
    int rd = ins & ( ( 1 << 5 ) - 1 );
    switch( funct3 ) {
        case 0x0: ADD_SUBW( funct7, rd, rs1, rs2 ); break;  /* ADDW SUBW */
        case 0x1: SLLW( rd, rs1, rs2 );             break;  /* SLLW */
        case 0x5: SR_W( funct7, rd, rs1, rs2 );     break;  /* SRLW SRAW */
    }
    return ;
}

void process_instruction( ull ins ) {
    ull opcode = ins & ( ( 1 << 7 ) - 1 );
    ins >>= 7;

    switch( opcode ) {
        case 0x37: LUI( ins );              pc += 4; break;     /* LUI */
        case 0x17: AUIPC( ins );            pc += 4; break;     /* AUIPC */
        case 0x6F: JAL( ins );                       break;     /* JAL */
        case 0x67: JALR( ins );                      break;     /* JALR */
        case 0x63: process_branch( ins );            break;     /* BEQ BNE BLT BGE BLTU BGEU */
        case 0x03: process_load( ins );     pc += 4; break;     /* LB LH LW LBU LHU LWU LD */
        case 0x23: process_save( ins );     pc += 4; break;     /* SB SH SW SD */
        case 0x13: process_op_imm( ins );   pc += 4; break;     /* ADDI SLTI SLTIU XORI ORI AND SLLI SRLI SRAI */
        case 0x33: process_op( ins );       pc += 4; break;     /* ADD SUB SLL SLT SLTU XOR SRL SRA OR AND */
//        case 0x0F: process_syn( ins );     pc += 4;  break;     /* FENCE FENCE.I */
//        case 0x73: process_sys( ins );     pc += 4;  break;     /* SCALL SBREAK RDCYCLE RDCYCLEN RDTIME RDTIMEH RDINSTRET RDINSTRETH*/
        case 0x1B: process_op_imm64( ins ); pc += 4; break;     /* ADDIW SLLIW SRLIW SRAIW */
        case 0x3B: process_op64( ins );     pc += 4; break;     /* ADDW SUBW SLLW SRLW SRAW */
        case 0x73: SCALL( ins );            pc += 4; break;     /* SCALL */
    }
    return ;
}

void process_instructions() {
    num_func = 1;
    num_ins = 0;
    while( num_func ) {
    //for( int j = 0; j < 50; j++ ) {
        ull ins = 0;
        for( int i = 3; i >= 0; --i ) {
            ins <<= 8;
            ins |= mem[pc + i];
        }
        num_ins++;
        //puts( "" );
        //printf( " pc is: %08x\n", pc );
        //printf( "ins is: %08x\n", ins );
        //int old = num_func;
        process_instruction( ins );
        //if( num_func != old ) printf( "num_func = %d\n", num_func);
    }
    printf("\nThe number of instruction is:%d\n", num_ins);
    return ;
}
