#include "DECKEY.h"
#include "DECW.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include "string"
#include <map>
#include <vector>

#define ErrMax 99						//максимальное  кол-во ошибок в программе
#define ErrCount 334					//кол-во ошибок, для кт есть описания

#define MAX_IDENT   12
#define bad_Symbol 1000
#define MAX_DL_IDENT 63
#define maxint 2147483647
#define MAX_DL_STRINGC 255
#define maxreal 2147483647.0
#define WORDLENGTH 16	/* длина минимальной адресуемой информационной 	*/
#define PROGS 300		/* ПРОГРАММА */ 
#define TYPES 301		/* ТИП */
#define CONSTS 302		/* КОНСТАНТА */ 
#define VARS 303		/* ПЕРЕМЕННАЯ */ 
#define PROCS 304		/* ПРОЦЕДУРА */ 
#define FUNCS 305		/* ФУНКЦИЯ */
#define SCALARS 401		/* cтандартный скалярный тип */
#define LIMITEDS 402	/* ограниченный тип */
#define ENUMS 403		/* перечислимый тип */
#define ARRAYS 404		/* регулярный тип (массив) */
#define REFERENCES 405	/* ссылочный тип */
#define SETS 406		/* множественный тип */
#define FILES 407		/* файловый тип */
#define RECORDS 408		/* комбинированный тип (запись)*/

#define SET_SIZE   8

using namespace std;

struct key {
	unsigned CodeKey;
	char     NameKey[15];
};

struct textposition
{
	unsigned linenumber = 0;				//номер строки
	unsigned charnumber = 0;				//номер позиции в строке
};

struct ErrorTable
{
	textposition ErrPos;
	unsigned ErrCode;
};

/*функция заполнения такблицы описаний ошибок*/
void MSGERR();
/*чтение новой строки*/
void ListThisLine(char *line);
/*вывод ошибок для строки с номером i*/
void WriteErrorsLine(int i);
/*функция вывода ошибок для строки current_str*/
void WriteErrorsListing(unsigned current_str);
/*функция чтения новой строки из программы*/
void ReadNextLine();
/*функция добавления ошибки в таблицу ошибок*/
void Error(unsigned errorcode, unsigned number_str, unsigned number_pos);
/*функция чтения ошибок из файла с ошибками*/
void ReadErrors();
/*функция печати кода символа в файл*/
void PrintSym();
/*функция печати синтаксической ошибки в файл с ошибками*/
void PrintErrorSym(unsigned errorcode, textposition position);
/*функция чтения очередной литеры строки*/
char NextCh();
/*функция пропускающая сам коментарий*/
void End_Comment();
/*определение числа*/
void DetermineTheNumber(int sign);
/*функция чтения очередной лексемы*/
void NextSym();
/*поиск указанного элемента в множестве.*/
bool Belong(unsigned element, unsigned *set);
/* проверяет, совпадает ли сканируемый символ с ожидаемым */
void Accept(unsigned symbolexpected);
/*-------------------- C O N V E R T _ T O _ B I T S -------------------*/
unsigned *Convert_To_Bits(unsigned *intstr);
/*	Функция SetDisjunct. Осуществляет дизъюнкцию множеств set1 и set2. Результатом становится множество set3.*/
void SetDisjunct(unsigned set1[], unsigned set2[], unsigned set3[]);
/* пропуск символов, пока не встречен символ, принадлежащий данному множеству*/
void Skipto1(unsigned *set);
/* пропуск символов, пока не встречен символ, принадлежащий одному из данных множеств 		        */
void Skipto2(unsigned *set1, unsigned *set2);