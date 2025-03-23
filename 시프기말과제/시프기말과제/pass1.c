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
} Reg[20];     // �������Ϳ� ���� �̸��� ��ȣ�� �����ϴ� ����ü
struct ins
{
	char instruct[6];
	char dest[2];
	char sour[2];
	char word_type[2];
	char ins_code[3];
	char ins_len[2];
	char mod_reg[9];
} Instr[136];     // �� �ν�Ʈ������ ������ �����ϴ� ����ü

extern int MaxI;

struct symbol_tbl
{
	char symbol[10];
	char word_type[2];
	int lc;
	char data[10];
} Symbol[20];     // �ɺ� ���̺�

extern int MaxS;

struct sentence
{
	char label[10];
	char _operator[10];
	char operand[3][10];
} Sen;

extern int LC = 0;
extern int Add_Chk(char* );     // ���۷����� ��巹�� ��带 ����

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
		strcpy(Symbol[j].data, Sen.operand[0]);		//�ּҰ��� ��Ÿ������

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
			// ����� ���� ������ Ȯ��
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