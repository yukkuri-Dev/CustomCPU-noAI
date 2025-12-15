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
    {"CALL",0x00000400},
    {"RET", 0x00000401},
    {"FUNC_CALL",0x00000400},
    {"FUNC_END", 0x00000401},
    {"END_ASM", 0xDEAD0001},
    // 必要なら後ろにも追加
};
size_t instr_table_size = sizeof(instr_table)/sizeof(instr_table[0]);

// 追加: ラベル関連の型を main の前に移動（main 内で先に使っているため）
typedef struct {
    char name[64];
    int address;
} Label;

typedef struct {
    char name[128];
    char filename[256];
    int line_number;
} installed_label;

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
    if (argc != 2) {
        printf("CloverTech Assembler v0.1\n");
        printf("Usage: assembler <input.asm>\n");
        return 1;
    }

    printf("Second argument: %s\n", argv[1]);

    FILE *inputFile = fopen(argv[1], "r");
    if (!inputFile) {
        printf("Failed to open input file");
        return 1;
    }
    char line[256]; // 1行を格納するバッファ
    const char *search_str = "Start:\n"; // 探したい文字列
    const char *search_subroutine_install = "#install\n"; // 探したい文字列
    int line_num = 0;
    int flag_found = 0;
    while (fgets(line, sizeof(line), inputFile)) {
        line_num++;
        if (strcmp(line, search_str) == 0) {
            printf("Found at line %d: %s", line_num, line);
            flag_found = 1;
            break;
        }
        if (strncmp(line, search_subroutine_install, 8) == 0) {
            printf("Subroutine install directive found at line %d: %s", line_num, line);
            const char *line_ptr = line + 8; // "#install"の後ろから開始
            while (*line_ptr == ' ' || *line_ptr == '\t') {
                line_ptr++; // 空白とタブをスキップ
            }
            // 末尾の改行/スペースをトリム
            char filename[260];
            size_t len = strlen(line_ptr);
            while (len > 0 && (line_ptr[len-1] == '\n' || line_ptr[len-1] == '\r' || line_ptr[len-1] == ' ' || line_ptr[len-1] == '\t')) {
                len--;
            }
            if (len >= sizeof(filename)) len = sizeof(filename) - 1;
            memcpy(filename, line_ptr, len);
            filename[len] = '\0';

            FILE *INSTALL_SUBROUTINE_ENTRY = fopen(filename, "r");
            if (!INSTALL_SUBROUTINE_ENTRY) {
                printf("Failed to open subroutine install file: %s\n", filename);
                return 1;
            }
            printf("Subroutine install file opened successfully: %s\n", filename);
            while(fgets(line, sizeof(line), INSTALL_SUBROUTINE_ENTRY)) {
                if (line[0] == ';' || line[0] == '\n') {
                    continue; // コメント行と空行をスキップ
                }
                printf("Subroutine line: %s", line);
                // strtok を正しく使う（最初のトークン取得から）
                char *token = strtok(line, " ,\n");
                char *label_dec = NULL;
                char *address_dec = NULL;
                if (token != NULL) {
                    label_dec = token;
                    address_dec = strtok(NULL, " ,\n");
                }
                printf("Subroutine Label: %s, Address: %s\n", label_dec ? label_dec : "NULL", address_dec ? address_dec : "NULL");
                installed_label inst_lbl; // 変数名を型名と衝突しないよう変更
                if (label_dec != NULL && address_dec != NULL) {
                    strncpy(inst_lbl.name, label_dec, sizeof(inst_lbl.name)-1);
                    inst_lbl.name[sizeof(inst_lbl.name)-1] = '\0';
                    strncpy(inst_lbl.filename, filename, sizeof(inst_lbl.filename)-1);
                    inst_lbl.filename[sizeof(inst_lbl.filename)-1] = '\0';
                    inst_lbl.line_number = atoi(address_dec);
                    printf("Installed Subroutine: %s at address %d from file %s\n", inst_lbl.name, inst_lbl.line_number, inst_lbl.filename);
                } else {
                    printf("Warning: Invalid subroutine line format: %s", line);
                }
            }





            fclose(INSTALL_SUBROUTINE_ENTRY);
            
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
    Label labels[256];
    installed_label installed_labels[256];
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
        // Skip empty lines and comment lines during label scan
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\n' || *p == '\0' || *p == ';') {
            continue;
        }
        // Treat as a label only if ':' appears before any whitespace
        char *colon = strchr(p, ':');
        if (colon != NULL) {
            for (char *q = p; q < colon; ++q) {
                if (*q == ' ' || *q == '\t') {
                    colon = NULL;
                    break;
                }
            }
        }
        if (colon != NULL) {
            // ラベル名を抽出
            size_t len = colon - p;
            if (len < sizeof(labels[0].name)) {
                strncpy(labels[label_count].name, p, len);
                labels[label_count].name[len] = '\0';
                labels[label_count].address = current_address;
                printf("[!]Label found: %s at address %d (line %d)\n", labels[label_count].name, current_address, line_num2);
                label_count++;
            }
        } else if (line[0] != '\n' && line[0] != ';') {
            // 空行やコメントでなければ命令としてカウント
            // 1 instruction = 4 words, and PC is a word-index.
            current_address += 4;
        }

        

    } //fetching labels loop end
    fseek(inputFile, 0, SEEK_SET);

    //デコード開始行を表示
    printf("Decode start line: %d\n", line_num);
    // Startラベルまで読み飛ばす（ここでファイルポインタをStartの直後に合わせる）
    while (fgets(line, sizeof(line), inputFile)) {
        if (strcmp(line, search_str) == 0) {
            break;
        }
    }

    // Always recreate output so we don't append old programs.
    FILE *fp_out = fopen("game.bin", "wb");
    if (!fp_out) {
        printf("Failed to open output file game.bin\n");
        return 1;
    }

    while(fgets(line, sizeof(line), inputFile)) {
        line_num++;
        //ここでアセンブル処理を行う
        //printf("Assembling line %d: %s\n", line_num, line);
            char *mnemonic = strtok(line, " ,\n");
            if (mnemonic && strchr(mnemonic, ':')) {
                // ラベルなのでスキップ
                continue; // この行の命令処理を飛ばす
            }else if (mnemonic && strchr(mnemonic, ';')) {
                // コメントなのでスキップ
                continue; // この行の命令処理を飛ばす
            } else if (mnemonic == NULL) {
                // mnemonicがnullだったらスキップ
                continue; // この行の命令処理を飛ばす
            }
            char *op1 = strtok(NULL, " ,\n");
            char *op2 = strtok(NULL, " ,\n");
            char *op3 = strtok(NULL, " ,\n");
            //printf("Mnemonic: %s, Operand1: %s, Operand2: %s,Operand3: %s\n", mnemonic, op1, op2,op3);
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
            //printf("menemonic Done %08X\n", machine_code[0]);
            // operands (label > known token > immediate)
            const char *ops[3] = {op1, op2, op3};
            for (int op_idx = 0; op_idx < 3; ++op_idx) {
                const char *tok = ops[op_idx];
                uint32_t value = 0;
                if (tok == NULL) {
                    value = 0;
                } else {
                    int resolved = 0;
                    for (int i = 0; i < label_count; ++i) {
                        if (strcmp(tok, labels[i].name) == 0) {
                            value = (uint32_t)labels[i].address;
                            resolved = 1;
                            break;//ラベル格納
                        }
                    }
                    if (!resolved) {
                        uint32_t op = get_opcode(tok);
                        if (op != 0xDEADDEAD) {
                            value = op;
                        } else {
                            value = (uint32_t)strtol(tok, NULL, 0);
                        }
                    }
                }
                machine_code[1 + op_idx] = value;
            }
            //printf("op1 Done %08X,", machine_code[1]);
            //printf("op2 Done %08X,", machine_code[2]);
            //printf("op3 Done %08X\n", machine_code[3]);
            //printf("Machine code: %08X %08X %08X %08X\n", machine_code[0], machine_code[1], machine_code[2], machine_code[3]);
            //バイナリファイルに書き込み
            fwrite(machine_code, sizeof(uint32_t), 4, fp_out);
            machine_code[0] =0;
            machine_code[1] =0;
            machine_code[2] =0;
            machine_code[3] =0;
    }   
    uint32_t STOP = 0xFFFFFFFF;
    fwrite(&STOP, sizeof(uint32_t), 1, fp_out);

    fclose(fp_out);
    fclose(inputFile);
    char output_filename[260];
    snprintf(output_filename, sizeof(output_filename), "%s.list", argv[1]);
    FILE *subp_out = fopen(output_filename, "a");
    if (!subp_out) {
        printf("Failed to open output file %s.list\n", argv[1]);
        return 1;
    }
    fprintf(subp_out, ";This file is generated by CloverTech Assmbler.\n");
    fprintf(subp_out, ";This file is a list of labels and their addresses.\n");
    fprintf(subp_out, ";Format: Label:,Address\n");
    fprintf(subp_out, ";using for example, call subroutine with external program.\n");
    for (int i = 0; i < label_count; ++i) {
        fprintf(subp_out, "%s:,%d\n", labels[i].name, labels[i].address);
    }
    fclose(subp_out);
    return 0;

}