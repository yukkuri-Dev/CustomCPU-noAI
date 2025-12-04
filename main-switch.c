#include <stdio.h>
#include <stdint.h>
#include <fileapi.h>
#include <time.h>
#include <string.h>
uint32_t CPU_GPR[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // General Purpose Registers
uint32_t CPU_PC = 0; // Program Counter
uint32_t CPU_SP = 0; // Stack Pointer
uint32_t CPU_FLAGS = 0; // Status Flags
uint32_t RAM[524288];  // Simulated RAM
#define RAM_SIZE 524288

int logo(){
 printf("Custom CPU Emulator v0.2.1-Switch\n");
 printf("\n");
 printf("      _     _            _                       ____  _   _       _____ \n");
 printf("  ___| |__ | | ___  _ __(_)_ __   ___     __   _|  _ \\| | | |     | ____|_ __ ___  _   _\n");
 printf(" / __| '_ \\| |/ _ \\| '__| | '_ \\ / _ \\____\\ \\ / / |_) | | | |_____|  _| | '_ ` _ \\| | | |\n");
 printf("| (__| | | | | (_) | |  | | | | |  __/_____\\ V /|  __/| |_| |_____| |___| | | | | | |_| |\n");
 printf(" \\___|_|_|_|_|\\___/|_|  |_|_| |_|\\___|      \\_/ |_|    \\___/      |_____|_| |_| |_|\\__,_|\n");


 
 printf("Developed by CloverTech1105^^\n");
 return 0;
}

int RAM_init() {
    logo();
    // RAM をゼロクリア
    for(int i = 0; i < RAM_SIZE; i++) {
        RAM[i] = 0;
    }

    // RAM ファイルを読み込む
    FILE *fp = fopen("game.bin", "rb");
    if(fp) {
        size_t bytes_read = fread(RAM, 1, RAM_SIZE * sizeof(uint32_t), fp);
        // 修正: ファイルサイズが1バイト以上あればOK
        if(bytes_read > 0) {
            printf("[i] Loaded %zu bytes from game.bin\n", bytes_read);
        } else {
            printf("[!] Warning: game.bin is empty\n");
        }
        fclose(fp);
    } else {
        perror("Failed to open RAM file, using default init data");

        // 初期データをコピー（テスト用）
        uint32_t init_data[4*19] = {
            0x00000053,0xD000FF0D,0x00000001,0x00000000,
            0x0FFFFFFF,0x00000004,0x0000000E,0x00000002,//8
            0x00000052,0xD000FF0D,0x00000001,0x00000000,
            0x0FFFFFFF,0x00000004,0x00000000,0x00000000,//10
            0xFFFFFFFF,0x0FFFFFFF,0x00000000,0x00000000
        };
        for(int i = 0; i < 4*19; i++) {
            RAM[i] = init_data[i];
        }
    }

    return 0;
}

int fetchRAM(){
    CPU_GPR[0] = RAM[CPU_PC];
    CPU_GPR[1] = RAM[CPU_PC + 1];
    CPU_GPR[2] = RAM[CPU_PC + 2];
    CPU_GPR[3] = RAM[CPU_PC + 3];
    CPU_PC += 4;
    return 0;
}
int Run() {
    switch (CPU_GPR[0])
    {
    case 0x00000001: // ADD
        CPU_GPR[4] = CPU_GPR[1] + CPU_GPR[2];
        break;
    case 0x00000002: // SUB
        CPU_GPR[4] = CPU_GPR[1] - CPU_GPR[2];
        break;
    case 0x00000010: // MOV
        if (CPU_GPR[1] == 0x00000000){
            printf("[!]MOV instruction warning: destination register use R0\n");
        }
        CPU_GPR[CPU_GPR[1]] = CPU_GPR[CPU_GPR[2]];
        break;
    case 0x00000020: //LOAD
        if (CPU_GPR[1] == 0x00000000){
            printf("[!]LOAD instruction warning: destination register use R0\n");
        }
        CPU_GPR[CPU_GPR[1]] = RAM[CPU_GPR[2]];
        break;
    case 0x00000021: //STORE
        if (CPU_GPR[1] == 0x00000000){
            printf("[!]STORE instruction warning: destination register use R0\n");
        }
        RAM[CPU_GPR[2]] = CPU_GPR[CPU_GPR[1]];
        break;
    case 0x00000030: // JMP
        if (CPU_GPR[3] == 0){
            printf("[i]JMP mode is Direct PC write\n");
            CPU_PC = CPU_GPR[1];
        }else if (CPU_GPR[3] == 1){
            printf("[i]JMP mode is PC Relative Jump\n");
            CPU_PC = CPU_GPR[1] / 4;
        }else {
            printf("[!]JMP instruction warning: Unknown mode %08X\n Emulator is supported 0(Direct PC write)or 1(PC Relative Jump)\n using Relative Jump Mode!\n", CPU_GPR[3]);
            CPU_PC = CPU_GPR[1] / 4;
        }
        break;
    case 0x00000031: // JZ
        if(CPU_GPR[3] == 0){
            printf("[i]JZ mode is Direct PC write\n");
            if(CPU_GPR[2] == 0){
                CPU_PC = CPU_GPR[1];
            }
        }else if(CPU_GPR[3] == 1){
            printf("[i]JZ mode is PC Relative Jump\n");
            if(CPU_GPR[2] == 0){
                CPU_PC = CPU_GPR[1] / 4;
            }
        }else {
            printf("[!]JZ instruction warning: Unknown mode %08X\n Emulator is supported 0(Direct PC write)or 1(PC Relative Jump)\n using Relative Jump Mode!\n", CPU_GPR[3]);
            if(CPU_GPR[2] == 0){
                CPU_PC = CPU_GPR[1] / 4;
            }
        }
        break;
    case 0x00000040: // AND
        CPU_GPR[4] = CPU_GPR[1] & CPU_GPR[2];
        break;
    case 0x00000041: // OR
        CPU_GPR[4] = CPU_GPR[1] | CPU_GPR[2];
        break;
    case 0x00000042: // XOR
        CPU_GPR[4] = CPU_GPR[1] ^ CPU_GPR[2];
        break;
    case 0x00000045: // Not
        CPU_GPR[4] = ~CPU_GPR[1];
        break;
    case 0x00000050: // SHL
        CPU_GPR[4] = CPU_GPR[1] << CPU_GPR[2];
        break;
    case 0x00000051: // SHR
        CPU_GPR[4] = CPU_GPR[1] >> CPU_GPR[2];
        break;
    case 0x00000052:// ROL
        CPU_GPR[4] = (CPU_GPR[1] << CPU_GPR[2]) | (CPU_GPR[1] >> (32 - CPU_GPR[2]));
        break;
    case 0x00000053:// ROR
        CPU_GPR[4] = (CPU_GPR[1] >> CPU_GPR[2]) | (CPU_GPR[1] << (32 - CPU_GPR[2]));
        break;
    case 0xFFFFFFFF: // エミュレーター用強制停止命令
        printf("[!]Emulator Debugging instruction!:PC is %08X\nHow many times?:%08X\n", CPU_PC, CPU_PC / 4);
        printf("====[What's happen?]====\n");
        printf("Emulator Debugging instruction!\n");
        printf("So, Emulator stopped.\n");
        printf("OK?\n");
        return -1;
    case 0x0FFFFFFF: //DEBUG PRINT
        printf("[DEBUG PRINT] Reg[%08X]: %08X\n", CPU_GPR[1], CPU_GPR[CPU_GPR[1]]);
        break;
    case 0x00000000: // NOP
        printf("[i]NOP instruction executed.\n");
        break;
    default:
        printf("[!]Unknown instruction: %08X\n", CPU_GPR[0]);
        printf("====[What's happen?]====\n");
        printf("Emulator encountered an unknown instruction at PC:%08X\n", CPU_PC - 4);
        printf("Emulator is supported instructions below:\n");
        printf("ADD(0x00000001), SUB(0x00000002),\nMOV(0x00000010), LOAD(0x00000020),\n STORE(0x00000021), JMP(0x00000030),\n JZ(0x00000031), DEBUG_STOP(0xFFFFFFFF)\n");
        printf("NOP(0x00000000), DEBUG_PRINT(0x0FFFFFFF)\n");
        printf("SHL(0x00000050), SHR(0x00000051)\n");
        printf("But,The command you are trying to execute is %08X\n", CPU_GPR[0]);
        printf("Unknown instruction encountered!\n");
        printf("So, Emulator stopped.\n");
        return -1;
    }
    return 0;
}

int main() {
    clock_t start_time;
    start_time = clock();
    RAM_init();
    while (1==1){     
        fetchRAM();
        if (Run() == -1) {
            break;
        }
        printf("PC:%08X(%08X)|Reg0: %08X|Reg1: %08X|Reg2: %08X|Reg3: %08X|Reg4(Output): %08X\n", CPU_PC,CPU_PC / 4, CPU_GPR[0], CPU_GPR[1], CPU_GPR[2], CPU_GPR[3], CPU_GPR[4]);
    }
    printf("Emulator stopped.\n");
    printf("Total execution time: %.2f seconds\n", (clock() - start_time) / (double)CLOCKS_PER_SEC);
    return 0;
}