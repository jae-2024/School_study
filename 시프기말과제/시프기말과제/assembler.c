#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>

struct reg
{
	char reg_name[3];
	char reg_num[4];
} Reg[20];     // 레지스터에 대한 이름과 번호를 저장하는 구조체

struct ins
{
	char instruct[6];
	char dest[2];
	char sour[2];
	char word_type[2];
	char ins_code[3];
	char ins_len[2];
	char mod_reg[9];
} Instr[136];     // 각 인스트럭션의 정보를 보관하는 구조체

int MaxI;

struct symbol_tbl
{
	char symbol[10];
	char word_type[2];
	int lc;
	char data[10];
} Symbol[20];     // 심볼 테이블

int MaxS;

struct sentence
{
	char label[10];
	char _operator[10];
	char operand[3][10];
} Sen;

int LC;

// 화일에서 레지스터와 인스트럭션의 정보를 읽는다
void Initialize()
{
	int i = 0, j = 1;
	FILE* regi, * inst;
	regi = fopen("reg_tbl.txt", "r");
	inst = fopen("inst_tbl.txt", "r");
	while (!feof(regi))
	{
		fscanf(regi, "%s%s\n", Reg[i].reg_name, Reg[i].reg_num);
		i++;
	}     // 레지스터 테이블을 작성한다
	while (!feof(inst))
	{
		fscanf(inst, "%6s%2s%2s%4s%3s%2s%9s\n", Instr[j].instruct,
			Instr[j].dest, Instr[j].sour,
			Instr[j].word_type, Instr[j].ins_code,
			Instr[j].ins_len, Instr[j].mod_reg);
		j++;
	}     // 명령어 테이블을 작성한다.
	MaxI = j - 1;
	fclose(regi);
	fclose(inst);
}

int Analyze(char* operand)
{
	int i = 0;
	char* regist[] = { "AX","BX","CX","DX","AL","BL","CL","DL","AH","BH","CH","DH","SI",
		"DI","CS","DS","SS","ES","SP","BP",0x00};     // 레지스터의 이름을 저장

	if (operand[0] == '-' && isdigit(operand[1])) {
		return 0;  // 음수인 즉시값 처리
	}
	else if (isdigit(operand[0])) {
		return 0;  // 즉시값 처리
	}
	else {
		// 레지스터 검사
		while (regist[i] != 0x00) {
			if (!_stricmp(operand, regist[i])) {
				if (i < 4) return 1;  // 16비트 레지스터
				else return 2;  // 8비트 레지스터
			}
			else i++;
		}
		return 3;
	}
}
#define MAX_INS 13     // 명령어의 최대 개수를 지정
int Add_Chk(char* sen)     // 오퍼랜드의 어드레스 모드를 판정
{
	register int k = MaxI;
	int i = 0, j = 0, l = 0, wp = 0, wc = 0;
	char op[5][10], * opcode[] = { "mov", "cmp", "or", "and", "sub", "add", "inc", "jmp", "ja", "jb", "je", "int", "dec", "" };
	while (sen[wp] != '\n')
	{
		while (sen[wp] == ' ' || sen[wp] == '\t' || sen[wp] == ',') wp++;     // 공백, 탭, 콤마는 통과

		while (sen[wp] != ' ' && sen[wp] != '\t' && sen[wp] != '\n' && sen[wp] != ',')
		{
			*(op[j] + i) = sen[wp];
			i++;
			wp++;
		}
		*(op[j] + i) = '\0';
		i = 0;
		j++;
	}
	i = 0;

	if (j == 1) {	// L1: 처리
		strcpy(Sen.label, op[0]);
		return 0;
	}

	while (_stricmp(opcode[i], "")) {
		if (_stricmp(opcode[i], op[0]))i++;
		else
		{
			strcpy(Sen._operator, op[0]);
			for (l = 1; l < j; l++) strcpy(Sen.operand[l - 1], op[l]);
			break;
		}
	}

	if (i == MAX_INS)
	{
		strcpy(Sen.label, op[0]);
		strcpy(Sen._operator, op[1]);
		for (l = 2; l < j; l++) strcpy(Sen.operand[l - 2], op[l]);
	}     // 한 문장을 분석하여 레이블, 오퍼레이터와 오퍼랜드로 분류한다
	strcpy(Instr[0].instruct, op[0]);     // OP코드를 저장, bug????

	if (6 <= i <= 12) {
		k = 2;
		return k;
	}

	switch (Analyze(op[1]))
	{
	case 0: strcpy(Instr[0].dest, "i");
		break;
	case 1: strcpy(Instr[0].dest, "r");
		strcpy(Instr[0].word_type, "w");
		break;
	case 2: strcpy(Instr[0].dest, "r");
		strcpy(Instr[0].word_type, "b");
		break;
	case 3: strcpy(Instr[0].dest, "m");
		break;
	}
	switch (Analyze(op[2]))
	{
	case 0: strcpy(Instr[0].sour, "i");
		break;
	case 1: strcpy(Instr[0].sour, "r");
		strcpy(Instr[0].word_type, "w");
		break;
	case 2: strcpy(Instr[0].sour, "r");
		strcpy(Instr[0].word_type, "b");
		break;
	case 3: strcpy(Instr[0].sour, "m");
		break;
	}

	while (_stricmp(Instr[k].instruct, Instr[0].instruct)
		|| _stricmp(Instr[k].dest, Instr[0].dest)
		|| _stricmp(Instr[k].sour, Instr[0].sour)
		|| _stricmp(Instr[k].word_type, Instr[0].word_type))
		k--;
	return k; // k == 0 : symbol
}


int btoi(char* dig)
{
	register int i = 0, ret = 0;
	while (*(dig + i) != '\0')
	{
		if (*(dig + i) == '1') ret += pow((double)2, (double)strlen(dig + i) - 1);
		i++;
	}
	return ret;
}

void main()
{
	char buf[50];
	FILE* in;
	in = fopen("test1.asm", "r");
	Initialize();
	printf("\nPass1:\n");
	while (fgets(buf, sizeof(buf), in) != NULL) {
		// 빈 줄인지 확인
		if (buf[0] == '\n' || buf[0] == '\0') {
			printf("\n");
			continue;
		}
		PassI(buf);
	}

	rewind(in);
	LC = 0;

	printf("\nPass2:\n");

	while (fgets(buf, sizeof(buf), in) != NULL) {
		// 빈 줄인지 확인
		if (buf[0] == '\n' || buf[0] == '\0') {
			printf("\n");
			continue;
		}
		PassII(buf);
	}
	fclose(in);

	getchar();
}