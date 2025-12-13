#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    const char *mnemonic;
    uint32_t opcode;
} InstrTableEntry;


InstrTableEntry instr_table[] = {
    //CPU_Register Instructions
    {"R1",1},
    {"R2",2},
    {"R3",3},
    {"R4",4},
    {"R5",5},
    {"R6",6},
    {"R7",7},
    {"R8",8},
    {"R9",9},
    {"R10",10},
    {"R11",11},
    {"R12",12},
    {"R13",13},
    {"R14",14},
    {"R15",15},
    {"R0",0},
    {"IN_INIT", 0xFFFF},
    {"IN_READ", 0x0000},
    {"NULL", 0x00000000},
    //CPU Instructions

    {"ADD", 0x00000001},
    {"SUB", 0x00000002},
    {"ADDC", 0x00000003},
    {"SUBC", 0x00000004},
    {"MOV", 0x00000010},
    {"LOAD", 0x00000020},
    {"STORE", 0x00000021},
    {"MOVI", 0x00000022},
    {"JMP", 0x00000030},
    {"JZ", 0x00000031},
    {"AND", 0x00000040},
    {"OR", 0x00000041},
    {"XOR", 0x00000042},
    {"NOT", 0x00000045},
    {"SHL", 0x00000050},
    {"SHR", 0x00000051},
    {"ROL", 0x00000052},
    {"ROR", 0x00000053},
    {"CMP", 0x00000100},
    {"DEBUG_STOP", 0xFFFFFFFF},
    {"DEBUG_PRINT", 0x0FFFFFFF},
    {"NOP", 0x00000000},
    {"IN", 0x00001000},
    {"OUT", 0x00000200},
    {"FUNC_CALL",0x00000400},
    {"FUNC_END", 0x00000401},
    {"END_ASM", 0xDEAD0001},
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
        printf("CloverTech Assembler v0.1\n");
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
    // ファイルポインタを先頭に戻す
    fseek(inputFile, 0, SEEK_SET);
    printf("File pointer reset to beginning.\n");

    // ラベル情報を格納する構造体と配列
    typedef struct {
        char name[64];
        int address;
    } Label;

    Label labels[256];
    int label_count = 0;

    // 1st pass: ラベルを収集
    int current_address = 0;
    int line_num2 = 0;
    // Startラベルまでスキップ
    while (fgets(line, sizeof(line), inputFile)) {
        if (strcmp(line, search_str) == 0) {
            break;
        }
    }
    while (fgets(line, sizeof(line), inputFile)) {
        line_num2++;
        char *colon = strchr(line, ':');
        if (colon != NULL) {
            // ラベル名を抽出
            size_t len = colon - line;
            if (len < sizeof(labels[0].name)) {
                strncpy(labels[label_count].name, line, len);
                labels[label_count].name[len] = '\0';
                labels[label_count].address = current_address;
                printf("[!]Label found: %s at address %d (line %d)\n", labels[label_count].name, current_address, line_num2);
                label_count++;
            }
        } else if (line[0] != '\n' && line[0] != ';') {
            // 空行やコメントでなければ命令としてカウント
            current_address += 4 * sizeof(uint32_t); // 4ワード分進める
        }
    }
    fseek(inputFile, 0, SEEK_SET);

    //デコード開始行を表示
    printf("Decode start line: %d\n", line_num);
    // Startラベルまで読み飛ばす（ここでファイルポインタをStartの直後に合わせる）
    while (fgets(line, sizeof(line), inputFile)) {
        if (strcmp(line, search_str) == 0) {
            break;
        }
    }

    while(fgets(line, sizeof(line), inputFile)) {
        line_num++;
        //ここでアセンブル処理を行う
        printf("Assembling line %d: %s\n", line_num, line);
            char *mnemonic = strtok(line, " ,\n");
            if (mnemonic && strchr(mnemonic, ':')) {
                // ラベルなのでスキップ
                continue; // この行の命令処理を飛ばす
            }
            char *op1 = strtok(NULL, " ,\n");
            char *op2 = strtok(NULL, " ,\n");
            char *op3 = strtok(NULL, " ,\n");

            // オペランドがラベル名の場合、アドレスに変換
            for (int i = 0; i < label_count; ++i) {
                if (op1 && strcmp(op1, labels[i].name) == 0) op1 = (char *)labels[i].address;
                if (op2 && strcmp(op2, labels[i].name) == 0) op2 = (char *)labels[i].address;
                if (op3 && strcmp(op3, labels[i].name) == 0) op3 = (char *)labels[i].address;
            }
            printf("Mnemonic: %s, Operand1: %s, Operand2: %s,Operand3: %s\n", mnemonic, op1, op2,op3);
            uint32_t machine_code[4];
            machine_code[0] = get_opcode(mnemonic); // 命令種類

            if (mnemonic != NULL){// Mnemonic（命令）
                if(get_opcode(mnemonic) == 0xDEADDEAD){
                    printf("warning: mnemonic's %s is undefined in assembler\n", mnemonic);
                    machine_code[0] = (uint32_t)strtol(mnemonic, NULL, 0);
                }else if(get_opcode(mnemonic) == 0xDEAD0001){
                    break;
                }else{
                    machine_code[0] = get_opcode(mnemonic); // 命令種類
                }
            }else{
                printf("warning: mnemonic is NULL\n");
                machine_code[0] = 0x00000000; // 命令が無い場合は0に設定
            }
            printf("menemonic Done %08X\n", machine_code[0]);
            //op1
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