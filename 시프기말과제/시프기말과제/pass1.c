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

extern int MaxI;

struct symbol_tbl
{
	char symbol[10];
	char word_type[2];
	int lc;
	char data[10];
} Symbol[20];     // 심볼 테이블

extern int MaxS;

struct sentence
{
	char label[10];
	char _operator[10];
	char operand[3][10];
} Sen;

extern int LC = 0;
extern int Add_Chk(char* );     // 오퍼랜드의 어드레스 모드를 판정

void PassI(char* buf)
{
	int i;
	static int j = 0;

	i = Add_Chk(buf);

	if (i)
	{
		printf("%04X: %s", LC, buf);
		LC += atoi(Instr[i].ins_len);
	}
	else
	{
		if (!_stricmp(Sen._operator, "dw"))
			strcpy(Symbol[j].word_type, "w");
		else if (!_stricmp(Sen._operator, "db"))
			strcpy(Symbol[j].word_type, "b");

		strcpy(Symbol[j].symbol, Sen.label);
		strcpy(Symbol[j].data, Sen.operand[0]);		//주소값을 나타내야함

		if (*Symbol[j].word_type == 'w') {
			LC += 2;
		}
		else if (*Symbol[j].word_type == 'b') {
			LC += 1;
		}

		Symbol[j].lc = LC;
		printf("%04X: %s", LC, buf);
		FILE* file = fopen("symbol_table.txt", "a");
		if (file != NULL) {
			// 제대로 값이 들어갔는지 확인
			if (Symbol[j].symbol[0] != '\0') {
				fprintf(file, "Symbol: %s, word_type: %s  LC: %04X  value: %s\n",
					Symbol[j].symbol,
					Symbol[j].word_type,
					Symbol[j].lc,
					Symbol[j].data);
			}
			fclose(file);
		}
		j++;
	}
}