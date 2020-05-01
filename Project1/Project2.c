#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
	char inst[4];
	char full_bin[33];
	char op[7];
	char fn[7];
} BCode;

typedef struct {
	char b6_b10[6];
	char b11_b15[6];
	char b16_b20[6];
	char b21_b25[6];
} BParsed;

typedef struct {
	int num;
	char full_bin[33];
} Register;

typedef struct {
	long i;
	long address;
	char full_bin[33];
} Mem_4;

void initMem(Mem_4 m, long i);

void shell(char* c);
void readFile(FILE* f, BCode* li);

void inst_config(BCode* list, int len);
void inst_mem_config(BCode* list, int len, Mem_4* m);
void hex2bin( char h, char* dd, int r);

void alprint_master(BCode b, int i);
BParsed parse_inst_bin(BCode b);

void alprint_inst_3ar(BCode li, int i, const char* op, BParsed p);

void alprint_inst_3sh(BCode li, int i, const char* op, BParsed p);
void alprint_inst_3sl(BCode li, int i, const char* op, BParsed p);

void alprint_inst_imm(BCode li, int i, const char* op, BParsed p);
void alprint_inst_1imm(BCode li, int i, const char* op, BParsed p);

void alprint_unknown(BCode li, int i);

int charbin2int(char* c);
long charbin2long(char* c);
char* mygets(char* s);

int main() {
	char command[100];
	char* object;
	int i = 0;

	char init[33] = "00000000000000000000000000000000";
	Register reg[32];
	Register PC;
	PC.num = -1;
	strcpy(PC.full_bin, init);
	for (i; i < 32; i++) {
		strcpy(reg[i].full_bin, init);
	}

	while (1) {
		int list_len = 0;
		BCode* inst_list = (BCode*)malloc(sizeof(BCode) * list_len);
		Mem_4* memory = (Mem_4*)malloc(sizeof(Mem_4) * list_len);
		shell(command);

		if (strcmp(command, "exit") == 0) {
			free(inst_list);
			break;
		}

		else if (strlen(command) == 0)
			continue;

		else {
			object = strtok(command, " ");
			if (strcmp(object, "read") == 0) {
				if (object = strtok(NULL, " ")) {
					 char buffer[4];
					FILE* len = fopen(object, "rb");
					if (len == NULL)
						continue;

					while (fread(buffer, sizeof( char), 4, len) != 0)
						list_len++;
					fclose(len);

					FILE* f = fopen(object, "rb");
					inst_list = (BCode*)realloc(inst_list, sizeof(BCode) * list_len);
					int i = 0;

					readFile(f, inst_list);
					inst_config(inst_list, list_len);

					for (i = 0; i < list_len; i++)
						alprint_master(inst_list[i], i);

					continue;
				}
				else {
					printf("Please choose a file to read...\n");
					continue;
				}
			}
			else if (strcmp(object, "loadinst") == 0) {
				if (object = strtok(NULL, " ")) {
					int i = 0;
					 char buffer[4];
					FILE* len = fopen(object, "rb");
					if (len == NULL)
						continue;

					while (fread(buffer, sizeof( char), 4, len) != 0)
						list_len++;
					fclose(len);

					FILE* f = fopen(object, "rb");
					inst_list = (BCode*)realloc(inst_list, sizeof(BCode) * list_len);
					memory = (Mem_4*)realloc(memory, sizeof(Mem_4) * list_len);
					for (i; i < list_len; i++) {
						initMem(memory[i], (long)(i));
					}

					readFile(f, inst_list);
					inst_mem_config(inst_list, list_len, memory);

					




					continue;
				}
				else {
					printf("Please choose a file to read...\n");
					continue;
				}
			}
			else if (strcmp(object, "run") == 0) {

			}
			else if (strcmp(object, "register") == 0) {

			}
			else continue;
		}
	}

	return 0;
}

void initMem(Mem_4 m, long i) {
	strcpy(m.full_bin, "11111111");
	m.i = i;
	m.address = m.i * 4;
}

void shell(char* c) {
	printf("mips_sim> ");
	mygets(c);
	rewind(stdin);
}

void readFile(FILE* f, BCode* li) {
	int l = 0;
	int size = 32;
	 char buffer[4];

	while (fread(buffer, sizeof( char), 4, f) != 0) {
		for (int i = 0; i < 4; i++)
			li[l].inst[i] = buffer[i];
		l++;
	}
}

void inst_config(BCode* li, int len) {
	for (int i = 0; i < len; i++) {
		char bin[33] = "";
		char dd[9] = { '\0' };
		for (int j = 0; j < 4; j++) {
			hex2bin(li[i].inst[j], dd, 0);
			strcat(bin, dd);
		}
		strcpy(li[i].full_bin, bin);
		int k;
		for (k = 0; k < 6; k++) {
			li[i].op[k] = li[i].full_bin[k];
			li[i].fn[k] = li[i].full_bin[k + 26];
		}
		li[i].op[k] = '\0';
		li[i].fn[k] = '\0';
	}
}

void inst_mem_config(BCode* li, int len, Mem_4* m) {
	for (int i = 0; i < len; i++) {
		char bin[33] = "";
		char dd[9] = { '\0' };
		for (int j = 0; j < 4; j++) {
			hex2bin(li[i].inst[j], dd, 0);
			strcat(bin, dd);
		}
		strcpy(li[i].full_bin, bin);
		int k;
		for (k = 0; k < 6; k++) {
			li[i].op[k] = li[i].full_bin[k];
			li[i].fn[k] = li[i].full_bin[k + 26];
		}
		li[i].op[k] = '\0';
		li[i].fn[k] = '\0';

		strcpy(m[i].full_bin, bin);
	}
}

void hex2bin( char h, char* dd, int r) {
	if (r == 0) {
		char bin[9] = { '\0' };
		hex2bin(h / 0x10, dd, 1);
		hex2bin(h % 0x10, dd, 1);
		strcat(bin, dd);
	}
	else if (r == 1) {
		switch (h - 0x00)
		{
		case 0x00:
			strcat(dd, "0000");
			break;
		case 0x01:
			strcat(dd, "0001");
			break;
		case 0x02:
			strcat(dd, "0010");
			break;
		case 0x03:
			strcat(dd, "0011");
			break;
		case 0x04:
			strcat(dd, "0100");
			break;
		case 0x05:
			strcat(dd, "0101");
			break;
		case 0x06:
			strcat(dd, "0110");
			break;
		case 0x07:
			strcat(dd, "0111");
			break;
		case 0x08:
			strcat(dd, "1000");
			break;
		case 0x09:
			strcat(dd, "1001");
			break;
		case 0x0A:
			strcat(dd, "1010");
			break;
		case 0x0B:
			strcat(dd, "1011");
			break;
		case 0x0C:
			strcat(dd, "1100");
			break;
		case 0x0D:
			strcat(dd, "1101");
			break;
		case 0x0E:
			strcat(dd, "1110");
			break;
		case 0x0F:
			strcat(dd, "1111");
			break;
		default:
			break;
		}
	}
}

void alprint_master(BCode b, int i) {
	BParsed bp = parse_inst_bin(b);
	if (!strcmp(b.op, "000000")) {
		if (!strcmp(b.fn, "101011")) // SLTU
			alprint_inst_3ar(b, i, "sltu", bp);
		else if (!strcmp(b.fn, "101010")) // SLT
			alprint_inst_3ar(b, i, "slt", bp);
		else if (!strcmp(b.fn, "100111")) // NOR
			alprint_inst_3ar(b, i, "nor", bp);
		else if (!strcmp(b.fn, "100110")) // XOR
			alprint_inst_3ar(b, i, "xor", bp);
		else if (!strcmp(b.fn, "100101")) // OR
			alprint_inst_3ar(b, i, "or", bp);
		else if (!strcmp(b.fn, "100100")) // AND
			alprint_inst_3ar(b, i, "and", bp);
		else if (!strcmp(b.fn, "100011")) // SUBU
			alprint_inst_3ar(b, i, "subu", bp);
		else if (!strcmp(b.fn, "100010")) // SUB
			alprint_inst_3ar(b, i, "sub", bp);
		else if (!strcmp(b.fn, "100001")) // ADDU
			alprint_inst_3ar(b, i, "addu", bp);
		else if (!strcmp(b.fn, "100000")) // ADD
			alprint_inst_3ar(b, i, "add", bp);

		else if (!strcmp(b.fn, "000111")) // SRAV
			alprint_inst_3sh(b, i, "srav", bp);
		else if (!strcmp(b.fn, "000110")) // SRLV
			alprint_inst_3sh(b, i, "srlv", bp);
		else if (!strcmp(b.fn, "000100")) // SLLV
			alprint_inst_3sh(b, i, "sllv", bp);
		else if (!strcmp(b.fn, "000010")) // SRL
			alprint_inst_3sl(b, i, "srl", bp);
		else if (!strcmp(b.fn, "000000")) // SLL
			alprint_inst_3sl(b, i, "sll", bp);
		else if (!strcmp(b.fn, "000011")) // SRA
			alprint_inst_3sl(b, i, "sll", bp);

		else
			alprint_unknown(b, i);
	}
	else if (!strcmp(b.op, "001000")) // ADDI
		alprint_inst_imm(b, i, "addi", bp);
	else if (!strcmp(b.op, "001001")) // ADDIU
		alprint_inst_imm(b, i, "addiu", bp);
	else if (!strcmp(b.op, "001010")) // SLTI
		alprint_inst_imm(b, i, "slti", bp);
	else if (!strcmp(b.op, "001011")) // SLTIU
		alprint_inst_imm(b, i, "sltiu", bp);
	else if (!strcmp(b.op, "001100")) // ANDI
		alprint_inst_imm(b, i, "addi", bp);
	else if (!strcmp(b.op, "001101")) // ORI
		alprint_inst_imm(b, i, "ori", bp);
	else if (!strcmp(b.op, "001110")) // XORI
		alprint_inst_imm(b, i, "xori", bp);
	else if (!strcmp(b.op, "001111")) // LUI
		alprint_inst_1imm(b, i, "lui", bp);

	else
		alprint_unknown(b, i);
}

BParsed parse_inst_bin(BCode b) {
	BParsed bp;
	for (int i = 0; i < 5; i++) {
		bp.b6_b10[i] = b.full_bin[i + 6];
		bp.b11_b15[i] = b.full_bin[i + 11];
		bp.b16_b20[i] = b.full_bin[i + 16];
		bp.b21_b25[i] = b.full_bin[i + 21];
	}
	bp.b6_b10[5] = '\0';
	bp.b11_b15[5] = '\0';
	bp.b16_b20[5] = '\0';
	bp.b21_b25[5] = '\0';
	bp.b21_b25[5] = '\0';
	// printf("===%s %s %s %s %s %s===\n", b.op, bp.b6_b10, bp.b11_b15, bp.b16_b20, bp.b21_b25, b.fn);
	return bp;
}

void alprint_inst_3ar(BCode li, int i, const char* op, BParsed p) {
	printf("inst %d: %02x%02x%02x%02x %s $%d, $%d, $%d\n",
		i, li.inst[0], li.inst[1], li.inst[2], li.inst[3], op,
		charbin2int(p.b16_b20), charbin2int(p.b6_b10), charbin2int(p.b11_b15));
}

void alprint_inst_3sh(BCode li, int i, const char* op, BParsed p) {
	printf("inst %d: %02x%02x%02x%02x %s $%d, $%d, $%d\n",
		i, li.inst[0], li.inst[1], li.inst[2], li.inst[3], op,
		charbin2int(p.b16_b20), charbin2int(p.b11_b15), charbin2int(p.b6_b10));
}
void alprint_inst_3sl(BCode li, int i, const char* op, BParsed p) {
	printf("inst %d: %02x%02x%02x%02x %s $%d, $%d, %d\n",
		i, li.inst[0], li.inst[1], li.inst[2], li.inst[3], op,
		charbin2int(p.b16_b20), charbin2int(p.b11_b15), charbin2int(p.b21_b25));
}
void alprint_inst_md(BCode li, int i, const char* op, BParsed p) {
	printf("inst %d: %02x%02x%02x%02x %s $%d, $%d\n",
		i, li.inst[0], li.inst[1], li.inst[2], li.inst[3], op,
		charbin2int(p.b6_b10), charbin2int(p.b11_b15));
}
void alprint_inst_1rs(BCode li, int i, const char* op, BParsed p) {
	printf("inst %d: %02x%02x%02x%02x %s $%d\n",
		i, li.inst[0], li.inst[1], li.inst[2], li.inst[3], op,
		charbin2int(p.b6_b10));
}
void alprint_inst_mf(BCode li, int i, const char* op, BParsed p) {
	printf("inst %d: %02x%02x%02x%02x %s $%d\n",
		i, li.inst[0], li.inst[1], li.inst[2], li.inst[3], op,
		charbin2int(p.b16_b20));
}
void alprint_inst_sys(BCode li, int i, const char* op) {
	printf("inst %d: %02x%02x%02x%02x %s\n",
		i, li.inst[0], li.inst[1], li.inst[2], li.inst[3], op);
}
void alprint_inst_j(BCode li, int i, const char* op, BParsed p) {
	char targ[27] = "";
	strcat(targ, p.b6_b10);
	strcat(targ, p.b11_b15);
	strcat(targ, p.b16_b20);
	strcat(targ, p.b21_b25);
	strcat(targ, li.fn);
	printf("inst %d: %02x%02x%02x%02x %s %ld\n",
		i, li.inst[0], li.inst[1], li.inst[2], li.inst[3], op, charbin2long(targ));
}
void alprint_inst_2off(BCode li, int i, const char* op, BParsed p) {
	char targ[17] = "";
	strcat(targ, p.b16_b20);
	strcat(targ, p.b21_b25);
	strcat(targ, li.fn);
	printf("inst %d: %02x%02x%02x%02x %s $%d, $%d, %ld\n",
		i, li.inst[0], li.inst[1], li.inst[2], li.inst[3], op,
		charbin2int(p.b6_b10), charbin2int(p.b11_b15), charbin2long(targ));
}
void alprint_inst_imm(BCode li, int i, const char* op, BParsed p) {
	char targ[17] = "";
	strcat(targ, p.b16_b20);
	strcat(targ, p.b21_b25);
	strcat(targ, li.fn);
	printf("inst %d: %02x%02x%02x%02x %s $%d, $%d, %ld\n",
		i, li.inst[0], li.inst[1], li.inst[2], li.inst[3], op,
		charbin2int(p.b11_b15), charbin2int(p.b6_b10), charbin2long(targ));
}
void alprint_inst_1imm(BCode li, int i, const char* op, BParsed p) {
	char targ[17] = "";
	strcat(targ, p.b16_b20);
	strcat(targ, p.b21_b25);
	strcat(targ, li.fn);
	printf("inst %d: %02x%02x%02x%02x %s $%d, %ld\n",
		i, li.inst[0], li.inst[1], li.inst[2], li.inst[3], op,
		charbin2int(p.b11_b15), charbin2long(targ));
}
void alprint_inst_1off(BCode li, int i, const char* op, BParsed p) {
	char targ[17] = "";
	strcat(targ, p.b16_b20);
	strcat(targ, p.b21_b25);
	strcat(targ, li.fn);
	printf("inst %d: %02x%02x%02x%02x %s $%d, %ld($%d)\n",
		i, li.inst[0], li.inst[1], li.inst[2], li.inst[3], op,
		charbin2int(p.b11_b15), charbin2long(targ), charbin2int(p.b6_b10));
}
void alprint_unknown(BCode li, int i) {
	printf("inst %d: %02x%02x%02x%02x unknown instruction\n",
		i, li.inst[0], li.inst[1], li.inst[2], li.inst[3]);
}

long charbin2long(char* c) {
	//printf("========%s=======\n", c);
	long s = -1 * (c[0] - '0');
	int i = 0;
	while (c[i + 1] != '\0') {
		if (c[i + 1] == '1')
			c[i] = c[i + 1] + s;
		else
			c[i] = c[i + 1] - s;
		i++;
	}
	c[i] = '\0';
	int d = strlen(c);
	long raw = atol(c);
	long res = 0;

	while (i > 0) {
		if (c[i - 1] == '1')
			res += (long)(pow(2.0, d - i));
		i--;
	}
	if (!s)
		return res;
	else
		return s * (res + 1);
}

int charbin2int(char* c) {
	int raw = atol(c);
	int i = 0;
	int d = strlen(c);
	int res = 0;
	int n = 0;

	for (i = 0; i < d; i++) {
		n = pow(10.0, d - i - 1);
		if (raw / n) {
			res += pow(2.0, d - i - 1);
			raw = raw % n;
		}
	}
	return res;
}


char* mygets(char* s) {
	char buffer = '0';
	int i = 0;

	do {
		buffer = (char)getchar();
		if (buffer != '\n') {
			s[i] = buffer;
			i++;
		}
		else {
			s[i] = '\0';
			break;
		}

	} while (1);
	rewind(stdin);

	return s;
}