#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define ADD  0x00000001
#define SUB  0x00000002
#define ADDC 0x00000003
#define SUBC 0x00000004
#define MOV  0x000000010
#define LOAD 0x00000020
#define STORE 0x00000021
#define MOVI 0x00000022
#define JMP  0x00000030
#define JZ   0x00000031
#define AND  0x00000040
#define OR   0x00000041
#define XOR  0x00000042
#define NOT  0x00000045
#define SHL  0x00000050
#define SHR  0x00000051
#define ROL  0x00000052
#define ROR  0x00000053
#define CMP  0x00000100
#define IN   0x00001000
#define DEBUG_STOP 0xFFFFFFFF
#define DEBUG_PRINT 0x0FFFFFFF
#define NOP  0x00000000
#define OUT  0x00000200
#define CPU_GPR_R1 1
#define CPU_GPR_R2 2
#define CPU_GPR_R3 3
#define CPU_GPR_R4 4
#define CPU_GPR_R5 5 
#define CPU_GPR_R6 6
#define CPU_GPR_R7 7
#define CPU_GPR_R8 8
#define CPU_GPR_R9 9 
#define CPU_GPR_R10 10
#define CPU_GPR_R11 11
#define CPU_GPR_R12 12
#define CPU_GPR_R13 13
#define CPU_GPR_R14 14
#define CPU_GPR_R15 15
#define CPU_GPR_R0 0
#define IN_INIT 0xFFFF
#define IN_READ 0x0000
typedef struct {
    const char *mnemonic;
    uint32_t opcode;
} InstrTableEntry;


InstrTableEntry instr_table[] = {
    //CPU_Register Instructions
    {"R1",CPU_GPR_R1},
    {"R2",CPU_GPR_R2},
    {"R3",CPU_GPR_R3},
    {"R4",CPU_GPR_R4},
    {"R5",CPU_GPR_R5},
    {"R6",CPU_GPR_R6},
    {"R7",CPU_GPR_R7},
    {"R8",CPU_GPR_R8},
    {"R9",CPU_GPR_R9},
    {"R10",CPU_GPR_R10},
    {"R11",CPU_GPR_R11},
    {"R12",CPU_GPR_R12},
    {"R13",CPU_GPR_R13},
    {"R14",CPU_GPR_R14},
    {"R15",CPU_GPR_R15},
    {"R0",CPU_GPR_R0},
    {"IN_INIT", IN_INIT},
    {"IN_READ", IN_READ},
    {"NULL", NOP},
    //CPU Instructions

    {"ADD", ADD},
    {"SUB", SUB},
    {"ADDC", ADDC},
    {"SUBC", SUBC},
    {"MOV", MOV},
    {"LOAD", LOAD},
    {"STORE", STORE},
    {"MOVI", MOVI},
    {"JMP", JMP},
    {"JZ", JZ},
    {"AND", AND},
    {"OR", OR},
    {"XOR", XOR},
    {"NOT", NOT},
    {"SHL", SHL},
    {"SHR", SHR},
    {"ROL", ROL},
    {"ROR", ROR},
    {"CMP", CMP},
    {"DEBUG_STOP", DEBUG_STOP},
    {"DEBUG_PRINT", DEBUG_PRINT},
    {"NOP", NOP},
    {"IN", IN},
    {"OUT", OUT},
    // 必要なら後ろにも追加
};
size_t instr_table_size = sizeof(instr_table)/sizeof(instr_table[0]);



uint32_t get_opcode(const char *mnemonic) {
    for (size_t i = 0; i < instr_table_size; ++i) {
        if (strcmp(instr_table[i].mnemonic, mnemonic) == 0) {
            return instr_table[i].opcode;
        }
    }
    // 未知命令ならNOPやエラー値を返す
    return 0xDEADDEAD; // 例：未知命令
}

int main(int argc, char *argv[]) {
    printf("Argument count: %d\n", argc);
    printf("First argument: %s\n", argv[0]);
    printf("Second argument: %s\n", argv[1]);
    if (argc != 2) {
        printf("Usage: assembler <input.asm>\n");
        return 1;
    }

    FILE *inputFile = fopen(argv[1], "r");
    if (!inputFile) {
        printf("Failed to open input file");
        return 1;
    }
    char line[256]; // 1行を格納するバッファ
    const char *search_str = "Start:\n"; // 探したい文字列
    int line_num = 0;
    int flag_found = 0;
    while (fgets(line, sizeof(line), inputFile)) {
        line_num++;
        if (strcmp(line, search_str) == 0) {
            printf("Found at line %d: %s", line_num, line);
            flag_found = 1;
            break;
        }
    }
    if (flag_found == 0) {
        printf("Error: 'Start:' not found in the file.\n");
        return -1;
    }
    //デコード開始行を表示
    printf("Decode start line: %d\n", line_num+1);
    while(fgets(line, sizeof(line), inputFile)) {
        line_num++;
        //ここでアセンブル処理を行う
        printf("Assembling line %d: %s\n", line_num, line);
            char *mnemonic = strtok(line, " ,\n");
            char *op1 = strtok(NULL, " ,\n");
            char *op2 = strtok(NULL, " ,\n");
            char *op3 = strtok(NULL, " ,\n");
            printf("Mnemonic: %s, Operand1: %s, Operand2: %s,Operand3: %s\n", mnemonic, op1, op2,op3);
            uint32_t machine_code[4];
            machine_code[0] = get_opcode(mnemonic); // 命令種類

            if (mnemonic != NULL){// Mnemonic（命令）
                if(get_opcode(mnemonic) == 0xDEADDEAD){
                    machine_code[0] = (uint32_t)strtol(mnemonic, NULL, 0);
                }else{
                    machine_code[0] = get_opcode(mnemonic); // 命令種類
                }
            }else{
                machine_code[0] = 0x00000000; // 命令が無い場合は0に設定
            }
            printf("menemonic Done %08X\n", machine_code[0]);


            
            //machine_code[1] = atoi(&op1[1]);// R1（オペランド1）
            //machine_code[2] = atoi(&op2[1]);    
            if (op1 != NULL){// R2（オペランド2）
                if(get_opcode(op1) == 0xDEADDEAD){
                    machine_code[1] = (uint32_t)strtol(op1, NULL, 0);
                }else{
                    machine_code[1] = get_opcode(op1); // オペランド1
                }
            }else{
                machine_code[1] = 0x00000000; // オペランド1が無い場合は0に設定
            }
            printf("op1 Done %08X\n", machine_code[1]);
            //op2
            if(op2 != NULL){
                if(get_opcode(op2) == 0xDEADDEAD){
                    machine_code[2] = (uint32_t)strtol(op2, NULL, 0);
                }else{
                    machine_code[2] = get_opcode(op2); // オペランド2
                }
            }else{
                machine_code[2] = 0x00000000; // オペランド2が無い場合は0に設定
            }
            printf("op2 Done %08X\n", machine_code[2]);
            //op3
            if (op3 != NULL){
                if(get_opcode(op3) == 0xDEADDEAD){
                    machine_code[3] = (uint32_t)strtol(op3, NULL, 0);
                }else{
                    machine_code[3] = get_opcode(op3); // オペランド3
                }
            }else{
                machine_code[3] = 0x00000000; // オペランド3が無い場合は0に設定
            }
            

            printf("op3 Done %08X\n", machine_code[3]);
            printf("Machine code: %08X %08X %08X %08X\n", machine_code[0], machine_code[1], machine_code[2], machine_code[3]);
            //バイナリファイルに書き込み
            FILE *fp_out = fopen("game.bin", "ab");
            fwrite(machine_code, sizeof(uint32_t), 4, fp_out);
            fclose(fp_out);
            machine_code[0] =0;
            machine_code[1] =0;
            machine_code[2] =0;
            machine_code[3] =0;
    }   
    FILE *fp_out = fopen("game.bin", "ab");
    uint32_t STOP = 0xFFFFFFFF;
    fwrite(&STOP, sizeof(uint32_t), 1, fp_out);
}