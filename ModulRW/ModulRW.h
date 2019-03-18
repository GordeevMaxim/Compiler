#include "DECKEY.h"
#include "DECW.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include "string"
#include <map>
#include <vector>

#define ErrMax 99						//������������  ���-�� ������ � ���������
#define ErrCount 334					//���-�� ������, ��� �� ���� ��������

#define MAX_IDENT   12
#define bad_Symbol 1000
#define MAX_DL_IDENT 63
#define maxint 2147483647
#define MAX_DL_STRINGC 255
#define maxreal 2147483647.0
#define WORDLENGTH 16	/* ����� ����������� ���������� �������������� 	*/
#define PROGS 300		/* ��������� */ 
#define TYPES 301		/* ��� */
#define CONSTS 302		/* ��������� */ 
#define VARS 303		/* ���������� */ 
#define PROCS 304		/* ��������� */ 
#define FUNCS 305		/* ������� */
#define SCALARS 401		/* c���������� ��������� ��� */
#define LIMITEDS 402	/* ������������ ��� */
#define ENUMS 403		/* ������������ ��� */
#define ARRAYS 404		/* ���������� ��� (������) */
#define REFERENCES 405	/* ��������� ��� */
#define SETS 406		/* ������������� ��� */
#define FILES 407		/* �������� ��� */
#define RECORDS 408		/* ��������������� ��� (������)*/

#define SET_SIZE   8

using namespace std;

struct key {
	unsigned CodeKey;
	char     NameKey[15];
};

struct textposition
{
	unsigned linenumber = 0;				//����� ������
	unsigned charnumber = 0;				//����� ������� � ������
};

struct ErrorTable
{
	textposition ErrPos;
	unsigned ErrCode;
};

/*������� ���������� �������� �������� ������*/
void MSGERR();
/*������ ����� ������*/
void ListThisLine(char *line);
/*����� ������ ��� ������ � ������� i*/
void WriteErrorsLine(int i);
/*������� ������ ������ ��� ������ current_str*/
void WriteErrorsListing(unsigned current_str);
/*������� ������ ����� ������ �� ���������*/
void ReadNextLine();
/*������� ���������� ������ � ������� ������*/
void Error(unsigned errorcode, unsigned number_str, unsigned number_pos);
/*������� ������ ������ �� ����� � ��������*/
void ReadErrors();
/*������� ������ ���� ������� � ����*/
void PrintSym();
/*������� ������ �������������� ������ � ���� � ��������*/
void PrintErrorSym(unsigned errorcode, textposition position);
/*������� ������ ��������� ������ ������*/
char NextCh();
/*������� ������������ ��� ����������*/
void End_Comment();
/*����������� �����*/
void DetermineTheNumber(int sign);
/*������� ������ ��������� �������*/
void NextSym();
/*����� ���������� �������� � ���������.*/
bool Belong(unsigned element, unsigned *set);
/* ���������, ��������� �� ����������� ������ � ��������� */
void Accept(unsigned symbolexpected);
/*-------------------- C O N V E R T _ T O _ B I T S -------------------*/
unsigned *Convert_To_Bits(unsigned *intstr);
/*	������� SetDisjunct. ������������ ���������� �������� set1 � set2. ����������� ���������� ��������� set3.*/
void SetDisjunct(unsigned set1[], unsigned set2[], unsigned set3[]);
/* ������� ��������, ���� �� �������� ������, ������������� ������� ���������*/
void Skipto1(unsigned *set);
/* ������� ��������, ���� �� �������� ������, ������������� ������ �� ������ �������� 		        */
void Skipto2(unsigned *set1, unsigned *set2);