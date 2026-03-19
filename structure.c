#include <stdio.h>
#include <stdint.h>

uint8_t mem[256] = {0};
uint8_t reg[4] = {0};   // R0, R1, R2, R3
uint8_t pc = 0;
uint8_t zf = 0;
uint8_t running = 1;
int ciclo = 0;

void fetch(uint8_t *op, uint8_t *a, uint8_t *b) {
    *op = mem[pc];
    *a  = mem[pc + 1];
    *b  = mem[pc + 2];
    pc += 3;
}

void decode_execute(uint8_t op, uint8_t a, uint8_t b) {
    switch (op) {
        case 0x01: // LOAD R, addr
            reg[a] = mem[b];
            break;

        case 0x02: // STORE R, addr
            mem[b] = reg[a];
            break;

        case 0x03: // ADD R1, R2
            reg[a] = reg[a] + reg[b];
            break;

        case 0x04: // SUB R1, R2
            reg[a] = reg[a] - reg[b];
            break;

        case 0x05: // MOV R, imm
            reg[a] = b;
            break;

        case 0x06: // CMP R1, R2
            zf = (reg[a] == reg[b]) ? 1 : 0;
            break;

        case 0x07: // JMP addr
            pc = a;
            break;

        case 0x08: // JZ addr
            if (zf) {
                pc = a;
            }
            break;

        case 0x09: // JNZ addr
            if (!zf) {
                pc = a;
            }
            break;

        case 0x0A: // HALT
            running = 0;
            break;

        default:
            printf("Opcode invalido: 0x%02X\n", op);
            running = 0;
            break;
    }
}

void format_instruction(uint8_t op, uint8_t a, uint8_t b, char *buffer) {
    switch (op) {
        case 0x01: sprintf(buffer, "LOAD R%d, [0x%02X]", a, b); break;
        case 0x02: sprintf(buffer, "STORE R%d, [0x%02X]", a, b); break;
        case 0x03: sprintf(buffer, "ADD R%d, R%d", a, b); break;
        case 0x04: sprintf(buffer, "SUB R%d, R%d", a, b); break;
        case 0x05: sprintf(buffer, "MOV R%d, %d", a, b); break;
        case 0x06: sprintf(buffer, "CMP R%d, R%d", a, b); break;
        case 0x07: sprintf(buffer, "JMP 0x%02X", a); break;
        case 0x08: sprintf(buffer, "JZ 0x%02X", a); break;
        case 0x09: sprintf(buffer, "JNZ 0x%02X", a); break;
        case 0x0A: sprintf(buffer, "HALT"); break;
        default:   sprintf(buffer, "???"); break;
    }
}

void trace(uint8_t op, uint8_t a, uint8_t b) {
    char instr[64];
    format_instruction(op, a, b, instr);

    printf(
        "Ciclo %3d: %-18s | "
        "R0=%3d R1=%3d R2=%3d R3=%3d | "
        "PC=0x%02X ZF=%d | "
        "MEM[0x10]=%3d MEM[0x20]=%3d MEM[0x21]=%3d MEM[0x23]=%3d MEM[0x24]=%3d\n",
        ciclo,
        instr,
        reg[0], reg[1], reg[2], reg[3],
        pc, zf,
        mem[0x10], mem[0x20], mem[0x21], mem[0x23], mem[0x24]
    );
}

void load_program(void) {

    mem[0x10] = 5;   // N = 5C (program counter)

    mem[0x00] = 0x07; mem[0x01] = 0x30; mem[0x02] = 0x00; // JMP 0x30

    mem[0x30] = 0x05; mem[0x31] = 0x01; mem[0x32] = 0x01; // MOV R1, 1
    mem[0x33] = 0x05; mem[0x34] = 0x02; mem[0x35] = 0x00; // MOV R2, 0
    mem[0x36] = 0x01; mem[0x37] = 0x03; mem[0x38] = 0x10; // LOAD R3, [0x10]
    mem[0x39] = 0x02; mem[0x3A] = 0x03; mem[0x3B] = 0x21; // STORE R3, [0x21]
    mem[0x3C] = 0x05; mem[0x3D] = 0x00; mem[0x3E] = 0x01; // MOV R0, 1
    mem[0x3F] = 0x02; mem[0x40] = 0x00; mem[0x41] = 0x20; // STORE R0, [0x20]

    mem[0x42] = 0x01; mem[0x43] = 0x03; mem[0x44] = 0x21; // LOAD R3, [0x21]
    mem[0x45] = 0x06; mem[0x46] = 0x03; mem[0x47] = 0x02; // CMP R3, R2
    mem[0x48] = 0x08; mem[0x49] = 0x96; mem[0x4A] = 0x00; // JZ 0x96
    mem[0x4B] = 0x06; mem[0x4C] = 0x03; mem[0x4D] = 0x01; // CMP R3, R1
    mem[0x4E] = 0x08; mem[0x4F] = 0x96; mem[0x50] = 0x00; // JZ 0x96

    mem[0x51] = 0x01; mem[0x52] = 0x00; mem[0x53] = 0x20; // LOAD R0, [0x20]
    mem[0x54] = 0x02; mem[0x55] = 0x00; mem[0x56] = 0x22; // STORE R0, [0x22]
    mem[0x57] = 0x01; mem[0x58] = 0x03; mem[0x59] = 0x21; // LOAD R3, [0x21]
    mem[0x5A] = 0x02; mem[0x5B] = 0x03; mem[0x5C] = 0x23; // STORE R3, [0x23]
    mem[0x5D] = 0x05; mem[0x5E] = 0x00; mem[0x5F] = 0x00; // MOV R0, 0
    mem[0x60] = 0x02; mem[0x61] = 0x00; mem[0x62] = 0x24; // STORE R0, [0x24]

    mem[0x63] = 0x01; mem[0x64] = 0x03; mem[0x65] = 0x23; // LOAD R3, [0x23]
    mem[0x66] = 0x06; mem[0x67] = 0x03; mem[0x68] = 0x02; // CMP R3, R2
    mem[0x69] = 0x08; mem[0x6A] = 0x84; mem[0x6B] = 0x00; // JZ 0x84

    mem[0x6C] = 0x01; mem[0x6D] = 0x00; mem[0x6E] = 0x24; // LOAD R0, [0x24]
    mem[0x6F] = 0x01; mem[0x70] = 0x03; mem[0x71] = 0x22; // LOAD R3, [0x22]
    mem[0x72] = 0x03; mem[0x73] = 0x00; mem[0x74] = 0x03; // ADD R0, R3
    mem[0x75] = 0x02; mem[0x76] = 0x00; mem[0x77] = 0x24; // STORE R0, [0x24]

    mem[0x78] = 0x01; mem[0x79] = 0x03; mem[0x7A] = 0x23; // LOAD R3, [0x23]
    mem[0x7B] = 0x04; mem[0x7C] = 0x03; mem[0x7D] = 0x01; // SUB R3, R1
    mem[0x7E] = 0x02; mem[0x7F] = 0x03; mem[0x80] = 0x23; // STORE R3, [0x23]
    mem[0x81] = 0x07; mem[0x82] = 0x63; mem[0x83] = 0x00; // JMP 0x63

    mem[0x84] = 0x01; mem[0x85] = 0x00; mem[0x86] = 0x24; // LOAD R0, [0x24]
    mem[0x87] = 0x02; mem[0x88] = 0x00; mem[0x89] = 0x20; // STORE R0, [0x20]

    mem[0x8A] = 0x01; mem[0x8B] = 0x03; mem[0x8C] = 0x21; // LOAD R3, [0x21]
    mem[0x8D] = 0x04; mem[0x8E] = 0x03; mem[0x8F] = 0x01; // SUB R3, R1
    mem[0x90] = 0x02; mem[0x91] = 0x03; mem[0x92] = 0x21; // STORE R3, [0x21]
    mem[0x93] = 0x07; mem[0x94] = 0x42; mem[0x95] = 0x00; // JMP 0x42

    mem[0x96] = 0x0A; mem[0x97] = 0x00; mem[0x98] = 0x00; // HALT
}

int main(void) {
    load_program();

    while (running && pc < 256) {
        uint8_t op, a, b;
        ciclo++;
        fetch(&op, &a, &b);
        decode_execute(op, a, b);
        trace(op, a, b);
    }

    printf("\nResultado final: MEM[0x20] = %d\n", mem[0x20]);
    return 0;
}