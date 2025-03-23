#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#define LSB(value) ((unsigned char)((value) & 0xFF))
#define MSB(value) ((unsigned char)(((value) >> 8) & 0xFF))

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

void ReplaceSymbolWithLC(char* buf, const char* symbol, int lc) {
	char temp[256];
	char* pos;

	// ���ۿ��� �ɺ��� ã�Ƽ� �����ϴ� �Լ�
	pos = strstr(buf, symbol);
	if (pos) {
		// �ɺ��� �߰ߵǸ� ���ο� LC�� ��ü
		snprintf(temp, sizeof(temp), "%.*s[%04X%s]", (int)(pos - buf), buf, lc, pos + strlen(symbol));
		strcpy(buf, temp);
	}
}

extern int LC;
extern int Add_Chk(char* sen);     // ���۷����� ��巹�� ��带 ����
extern int btoi(char* dig);

void PassII(char* buf)
{
	int i, j = 0, k = 0;

	i = Add_Chk(buf);
	// '\n' ����
	size_t len = strlen(buf);
	if (len > 0 && buf[len - 1] == '\n') {
		buf[len - 1] = '\0';
	}

	if (i)
	{
		printf("%04x:", LC);

		if (strcmp(Instr[i].ins_code, "00")) {
			printf("%3s", Instr[i].ins_code);
		}
		strcpy(Instr[0].mod_reg, Instr[i].mod_reg);

		if (!_stricmp(Instr[i].dest, "r"))
		{
			while (_stricmp(Reg[j].reg_name, Sen.operand[0]))
				j++;
			strncpy(strchr(Instr[0].mod_reg, '?'), Reg[j].reg_num, 3);
		}		// ?�� ó�� ������ ��ġ�� ��ȯ�ؼ� ����ġ�� reg_num�� �ִ� 3���ڸ� ����
		j = 0;

		if (!_stricmp(Instr[i].sour, "r"))
		{
			while (_stricmp(Reg[j].reg_name, Sen.operand[1]))
				j++;
			strncpy(strchr(Instr[0].mod_reg, '?'), Reg[j].reg_num, 3);
		}

		if (_stricmp(Instr[i].dest, "m") && _stricmp(Instr[i].sour, "m")) {
			printf(" %02X", btoi(Instr[0].mod_reg));

			// ��: ��ð��� ��ɾ �߰�
			if (!_stricmp(Instr[i].sour, "i")) {  // dest�� Immediate(��ð�)���� Ȯ��
				int immediate_value = atoi(Sen.operand[1], NULL, 10); // ��ð� ��������
				if (!_stricmp(Instr[i].word_type, "b")) {
					printf(" %02X", LSB(immediate_value));
				}
				else {
					printf(" %02X%02X", LSB(immediate_value), MSB(immediate_value));
				}
			}
			printf("  %s", buf);
			printf("	mod_reg: %s\n", Instr[0].mod_reg);
		}

		else {
			if (!_stricmp(Instr[i].dest, "m"))
				while (_stricmp(Symbol[k].symbol, Sen.operand[0]))k++;
			//�̺κп��� mov AX, [BX]ó���� �ȵ� ���ڵ尡 ����
			else if (!_stricmp(Instr[i].sour, "m"))
				while (_stricmp(Symbol[k].symbol, Sen.operand[1]))k++;

			// buf �ȿ��� Symbol[k].symbol�� Symbol[k].lc�� �ٲ㼭 ���
			ReplaceSymbolWithLC(buf, Symbol[k].symbol, Symbol[k].lc);
			
			int data_value = Symbol[k].lc;  // Symbol[k].data �� ������ ��ȯ

			printf(" %02X%02X	%s", LSB(data_value), MSB(data_value), buf);
			printf("	mod_reg: %s\n", Instr[0].mod_reg);
		}
		LC += atoi(Instr[i].ins_len);
	}

	else
	{
		k = 0;
		while (_stricmp(Symbol[k].symbol, Sen.label)) {
			k++;
		}

		int data_value = atoi(Symbol[k].data);  // Symbol[k].data �� ������ ��ȯ

		if (!_stricmp(Symbol[k].word_type, "w"))
			printf("%04X:  %02X%02X %15s", LC, LSB(data_value), MSB(data_value), buf);
		if (!_stricmp(Symbol[k].word_type, "b"))
			printf("%04X:  %02X%02X %15s", LC, LSB(data_value), MSB(data_value), buf);
		if (*Symbol[k].word_type == 'w')  LC += 2;
		else if (*Symbol[k].word_type == 'b') LC += 1;
	}
}