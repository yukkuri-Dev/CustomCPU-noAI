#include <stdio.h>
#include <stdint.h>
#include <fileapi.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "vBus/vbus_root.h"      // ← こちらを追加
uint32_t CPU_GPR[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // General Purpose Registers
uint32_t CPU_PC = 0; // Program Counter
uint32_t CPU_SP = 0; // Stack Pointer
uint32_t CPU_FLAGS = 0b0000; // Status Flags//Carry,Zero,Sign,Overflow

//uint32_t RAM[524288];  // Simulated RAM
uint32_t *RAM = NULL;
size_t RAM_SIZE = 0;

int logo(){
 printf("Custom CPU Emulator v0.2.2-Switch\n");
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
            0x00000030,0x00000000,0x00000000,0x00000000,
            0x00001000,0x00000000,0x00000002,0x00000000,//8
            0x0FFFFFFF,0x00000004,0x00000001,0x00000000,
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
        CPU_FLAGS |= 0b0000; // Clear Flags
        CPU_GPR[4] = CPU_GPR[1] + CPU_GPR[2];
        if (CPU_GPR[4] < CPU_GPR[1]){
            CPU_FLAGS |= 0b0001; // Set Carry Flag
        }
        break;
    case 0x00000002: // SUB
        CPU_FLAGS |= 0b0000; // Clear Flags
        CPU_GPR[4] = CPU_GPR[1] - CPU_GPR[2];
        if (CPU_GPR[4] == 0){
            CPU_FLAGS |= 0b0010; // Set Zero Flag
        }
        break;
    case 0x00000003: //ADDC
        CPU_GPR[4] = CPU_GPR[1] + CPU_GPR[2] + (CPU_FLAGS & 0b0001);
        break;
    case 0x00000004: //SUBC
        CPU_GPR[4] = CPU_GPR[1] - CPU_GPR[2] - (CPU_FLAGS & 0b0001);
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
    case 0x00000022://MOVI
        if (CPU_GPR[1] == 0x00000000){
            printf("[!]MOVI instruction warning: destination register use R0\n");
        }
        CPU_GPR[CPU_GPR[1]] = CPU_GPR[2];
        break;
    case 0x00000030: // JMP
        {
        static int jmp_mode_printed = 0;
        if (CPU_GPR[2] == 0){
            if (!jmp_mode_printed) {
                printf("[i]JMP mode is Direct PC write\n");
                jmp_mode_printed = 1;
            }
            CPU_PC = CPU_GPR[1];
        }else if (CPU_GPR[2] == 1){
            if (!jmp_mode_printed) {
                printf("[i]JMP mode is PC Relative Jump\n");
                jmp_mode_printed = 1;
            }
            CPU_PC = CPU_GPR[1] * 4;
        }else {
            printf("[!]JMP instruction warning: Unknown mode %08X\n Emulator is supported 0(Direct PC write)or 1(PC Relative Jump)\n using Relative Jump Mode!\n", CPU_GPR[3]);
            CPU_PC = CPU_GPR[1] * 4;
        }
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
    case 0x00000100: //CMP
        CPU_FLAGS = 0; // Clear Flags
        if (CPU_GPR[1] == CPU_GPR[2]){
            CPU_FLAGS |= 0b0010; // Set Zero Flag
        }
        if ((int32_t)(CPU_GPR[1]) - (int32_t)(CPU_GPR[2]) < 0) {
            CPU_FLAGS |= 0b1000;
        }
        break;

    case 0x00001000: // IN
        {
            // IN uses register-based operands (like MOV/LOAD/STORE):
            // operand words are register indices, and we pass the register contents to vBus.
            uint32_t mode = CPU_GPR[CPU_GPR[1] & 0xF];
            uint32_t mount = CPU_GPR[CPU_GPR[2] & 0xF];
            uint32_t data = CPU_GPR[CPU_GPR[3] & 0xF];
            int res = vbus_root_main((uint16_t)mode, (uint16_t)mount, (uint16_t)data);
            switch (res) {
            case -1:
                printf("[!]vBus is not initialized.\n");
                break;
            case -100:
                printf("[i]vBus initialized successfully.\n");
                break;
            default:
                CPU_GPR[4] = res;
                break;
            }
        }
        break;
    case 0x00000200://OUT
        {
            uint32_t mode = CPU_GPR[CPU_GPR[1] & 0xF];
            uint32_t mount = CPU_GPR[CPU_GPR[2] & 0xF];
            uint32_t data = CPU_GPR[CPU_GPR[3] & 0xF];
            switch (vbus_root_out_main((uint16_t)mode, (uint16_t)mount, (uint16_t)data))
            {
            case -1:
                printf("[!]vBus is not initialized.\n");
                break;
            
            case -10 :
                printf("[!]vBus Write Error.\n");
                break;
            default:
                printf("[i]vBus OUT executed successfully.\n");
                break;
            }
        }
        break;
    case 0x00000400: // FUNC_CALL
        // スタックに現在のPCを保存
        int Stack_Address = RAM_SIZE - 16; // スタックの開始アドレス（RAMの最後から16ワード分をスタックに使用）
        if (CPU_SP >= RAM_SIZE - 16) {
            printf("[!]Stack Overflow detected! SP: %08X\n", CPU_SP);
            return -1;
        }
        RAM[Stack_Address + CPU_SP] = CPU_PC;
        CPU_SP += 1; // スタックポインタを更新
        CPU_PC = CPU_GPR[1]; // 関数のアドレスにジャンプ
        break;
    case 0x00000401: // FUNC_END
        // スタックからPCを復元
        if (CPU_SP <= 0) {
            printf("[!]Stack Underflow detected! SP: %08X\n", CPU_SP);
            return -1;
        }
        CPU_SP -= 1; // スタックポインタを更新
        int Stack_Address_End = RAM_SIZE - 16; // スタックの開始アドレス（RAMの最後から16ワード分をスタックに使用）
        CPU_PC = RAM[Stack_Address_End + CPU_SP]; // 保存されていたPCに戻る
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

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Custom CPU Emulator v0.2.2-Switch\n");
        printf("Usage: emulator <RAM size in bytes (e.g., 512k, 2m, 1048576b)>\n");
        return 1;
    }
    int Set_RAM_size = atoi(argv[1]);
    if (strstr(argv[1] , "k") != NULL || strstr(argv[1], "K") != NULL ){
        RAM_SIZE = Set_RAM_size * 1024;
    }else if (strstr(argv[1], "m") != NULL || strstr(argv[1], "M") != NULL ){
        RAM_SIZE = Set_RAM_size * 1024 * 1024;
    }else if (strstr(argv[1], "b") != NULL || strstr(argv[1], "B") != NULL ){
        RAM_SIZE = atoi(argv[1]);
    }else if (Set_RAM_size > 0 || Set_RAM_size == NULL){
        printf("Using default RAM size.\n");
        RAM_SIZE = 524288; // Default 2MB
    }else{
        printf("Invalid RAM size argument. Using default size.\n");
        return 1;
    }
    printf("RAMsize : %d bytes\n", RAM_SIZE * sizeof(uint32_t));
    RAM = (uint32_t *)malloc(RAM_SIZE * sizeof(uint32_t));
    if (RAM == NULL) {
        fprintf(stderr, "Failed to allocate memory for RAM\n");
        return 1;
    }
    clock_t start_time;
    start_time = clock();
    RAM_init();
    // Auto-initialize vBus so devices (fakevgq, counters) start and window appears
    // Initialize vBus devices early to ensure devices (fakevgq, counters) are ready
    vbus_list_init();
    VBUS_MODE = 1;
    while (1==1){     
        fetchRAM();
        if (Run() == -1) {
            break;
        }
        //printf("PC:%08X(%08X)|Reg0: %08X|Reg1: %08X|Reg2: %08X|Reg3: %08X|Reg4(Output): %08X\n", CPU_PC,CPU_PC / 4, CPU_GPR[0], CPU_GPR[1], CPU_GPR[2], CPU_GPR[3], CPU_GPR[4]);
    }
    free(RAM);
    printf("Emulator stopped.\n");
    printf("Total execution time: %.2f seconds\n", (clock() - start_time) / (double)CLOCKS_PER_SEC);
    return 0;
}