#include "pch.h"
#include "DECKEY.h"
#include "DECW.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>

#define ErrMax 99						//максимальное  кол-во ошибок в программе
#define ErrCount 334					//кол-во ошибок, для кт есть описания

#define MAX_IDENT   12
#define bad_Symbol 1000
#define MAX_DL_IDENT 63
#define maxint 32767
#define MAX_DL_STRINGC 255
#define maxreal 1023.91938797167

struct key {
	unsigned CodeKey;
	char     NameKey[15];
};
static struct key  KeyWords[64] =
/* таблица ключевых слов и их кодов 		*/
{
		  { ident," "},
		  { dosy, "do"},
		  { ifsy, "if"},
		  { insy, "in"},
		  { ofsy, "of"},
		  { orsy, "or"},
		  { tosy, "to"},
		  { ident,"  "},
		  { andsy,"and"},
		  { divsy,"div"},
		  { endsy,"end"},
		  { forsy,"for"},
		  { modsy,"mod"},
		  { nilsy,"nil"},
		  { notsy,"not"},
		  { setsy,"set"},
		  { varsy,"var"},
		  { ident,"   "},
		  { casesy,"case"},
		  { elsesy,"else"},
		  { filesy,"file"},
		  { gotosy,"goto"},
		  { onlysy,"only"},
		  { thensy,"then"},
		  { typesy,"type"},
		  { unitsy,"unit"},
		  { usessy,"uses"},
		  { withsy,"with"},
		  { ident, "    "},
		  { arraysy,"array"},
		  { beginsy,"begin"},
		  { constsy,"const"},
		  { labelsy,"label"},
		  { untilsy,"until"},
		  { whilesy,"while"},
		  { ident,  "     "},
		  { exportsy,"export"},
		  { importsy,"import"},
		  { downtosy,"downto"},
		  { modulesy,"module"},
		  { packedsy,"packed"},
		  { recordsy,"record"},
		  { repeatsy,"repeat"},
		  { vectorsy,"vector"},
		  { stringsy,"string"},
		  { ident,   "      "},
		  { forwardsy,"forward"},
		  { processsy,"process"},
		  { programsy,"program"},
		  { segmentsy,"segment"},
		  { ident,    "       "},
		  { functionsy,"function"},
		  { separatesy,"separate"},
		  { ident,     "        "},
		  { interfacesy,"interface"},
		  { proceduresy,"procedure"},
		  { qualifiedsy,"qualified"},
		  { ident,      "         "},
		  { ident,      "          "},
		  { ident,      "           "},
		  { ident,      "            "},
		  { ident,      "             "},
		  { implementationsy,"implementation"},
		  { ident,        "              "}
};

/* массив номеров строк с кодом ident: 	*/
int short last[15] = { -1,0,7,17,28,35,45,50,53,57,58,59,60,61,63 };
using namespace std;

struct textposition
{
	unsigned linenumber = 0;				//номер строки
	unsigned charnumber = 0;				//номер позиции в строке
};

textposition token;
textposition positionnow;

struct ErrorTable
{
	textposition ErrPos;
	unsigned ErrCode;
};

vector <ErrorTable> ErrTable(ErrMax);	// таблица ошибок из  файла с ошибками

unsigned i = 0,							// счётчик
		ErrorOverflow = false,			// 
		ErrInx = 0,						// счётчик ошибок
		LastInLine;						// длина текущей строки
char ch = '-';							// текущая литера
char line[90] = "";						// текущая строка
char **MsgErr = new char *[ErrCount];	//массив сообщений об ошибках
int flag;
int flag_comment;
bool err_203;
char name[80];							//лексема
int lname;								//длина лексемы
unsigned Symbol,						//код символа
		 len_c;							//длина строковой констатнты
int nextDigit;							//распознаная цифра
int intConstant;						//целая константа
int intE;								//показатель вещественного числа
double realConstant;					//вещественная константа
float mn;
FILE *file_program, 					// файл с текстом программы
	 *file_place_err,					// файл с ошибками
	 *file_rez_err,						// файл с выводом лексичских ошибок
	 *file_rezult_lex;					// файл результата работы лексического анализатора
ofstream file_listing;					// файл листинга программы

/*функция заполнения такблицы описаний ошибок*/
void MSGERR()
{
	for (int i = 0; i < ErrCount; i++)
	{
		MsgErr[i] = new char [75];
		MsgErr[i] = (char*)"Такого сообщения нет в файле данных !";
	}
	MsgErr[1] = (char*)"ошибка в простом типе";
	MsgErr[2] = (char*)"Такого сообщения нет в файле данных !""должно идти имя";
	MsgErr[3] = (char*)"должно быть служебное слово PROGRAM";
	MsgErr[4] = (char*)"должен идти символ  ')'";
	MsgErr[5] = (char*)"должен идти символ  ':'";
	MsgErr[6] = (char*)"запрещенный символ";
	MsgErr[7] = (char*)"ошибка в списке параметров";
	MsgErr[8] = (char*)"должно идти  OF";
	MsgErr[9] = (char*)"должен идти символ  '('";
	MsgErr[10] = (char*)"ошибка в типе";
	MsgErr[11] = (char*)"должен идти символ  '['";
	MsgErr[12] = (char*)"должен идти символ  ']'";
	MsgErr[13] = (char*)"должно идти слово  END";
	MsgErr[14] = (char*)"должен идти символ  ';'";
	MsgErr[15] = (char*)"должно идти целое";
	MsgErr[16] = (char*)"должен идти символ '='";
	MsgErr[17] = (char*)"должно идти слово  BEGIN";
	MsgErr[18] = (char*)"ошибка в разделе описаний";
	MsgErr[19] = (char*)"ошибка в списке полей";
	MsgErr[20] = (char*)"должен идти символ  ','";
	MsgErr[21] = (char*)"ошибка в переменной";
	MsgErr[50] = (char*)"ошибка в константе";
	MsgErr[51] = (char*)"должен идти символ  ':='";
	MsgErr[52] = (char*)"должно идти слово  THEN";
	MsgErr[53] = (char*)"должно идти слово  UNTIL";
	MsgErr[54] = (char*)"должно идти слово  DO";
	MsgErr[55] = (char*)"должно идти слово  TO  или  DOWNTO";
	MsgErr[56] = (char*)"должно идти слово  IF";
	MsgErr[61] = (char*)"должен идти символ  '.'";
	MsgErr[74] = (char*)"должен идти символ  '..'";
	MsgErr[75] = (char*)"ошибка в символьной константе";
	MsgErr[76] = (char*)"слишком длинная строковая константа";
	MsgErr[86] = (char*)"комментарий не закрыт";
	MsgErr[83] = (char*)"Должна идти символьная константа";
	MsgErr[100] = (char*)"использование имени не соответствует описанию";
	MsgErr[101] = (char*)"имя описано повторно";
	MsgErr[102] = (char*)"нижняя граница превосходит верхнюю";
	MsgErr[103] = (char*)"Имя не принадлежит соответствующему классу";
	MsgErr[104] = (char*)"имя не описано";
	MsgErr[105] = (char*)"недопустимое рекурсивное определение";
	MsgErr[108] = (char*)"файл здесь использовать нельзя";
	MsgErr[109] = (char*)"тип не должен быть  REAL";
	MsgErr[111] = (char*)"несовместимость с типом дискриминанта";
	MsgErr[112] = (char*)"недопустимый ограниченный тип";
	MsgErr[114] = (char*)"тип основания не должен быть  REAL  или  INTEGER";
	MsgErr[115] = (char*)"файл должен быть текстовым";
	MsgErr[116] = (char*)"ошибка в типе параметра стандартной процедуры";
	MsgErr[117] = (char*)"неподходящее опережающее описание";
	MsgErr[118] = (char*)"недопустимый тип пpизнака ваpиантной части записи";
	MsgErr[119] = (char*)"опережающее описание : повторение списка параметров не допускается";
	MsgErr[120] = (char*)"тип результата функции должен быть скалярным, ссылочным или ограниченным";
	MsgErr[121] = (char*)"параметр - значение не может быть файлом";
	MsgErr[122] = (char*)"опережающее описание функции : повторять тип результата нельзя";
	MsgErr[123] = (char*)"в описании функции пропущен тип результата";
	MsgErr[124] = (char*)"F - формат только для  REAL";
	MsgErr[125] = (char*)"ошибка в типе параметра стандартной функции";
	MsgErr[126] = (char*)"число параметров не согласуется с описанием";
	MsgErr[127] = (char*)"недопустимая подстановка параметров";
	MsgErr[128] = (char*)"тип результата функции не соответствует описанию";
	MsgErr[130] = (char*)"выражение не относится к множественному типу";
	MsgErr[135] = (char*)"тип операнда должен быть  BOOLEAN";
	MsgErr[137] = (char*)"недопустимые типы элементов множества";
	MsgErr[138] = (char*)"переменная не есть массив";
	MsgErr[139] = (char*)"тип индекса не соответствует описанию";
	MsgErr[140] = (char*)"переменная не есть запись";
	MsgErr[141] = (char*)"переменная должна быть файлом или ссылкой";
	MsgErr[142] = (char*)"недопустимая подстановка параметров";
	MsgErr[143] = (char*)"недопустимый тип параметра цикла";
	MsgErr[144] = (char*)"недопустимый тип выражения";
	MsgErr[145] = (char*)"конфликт типов";
	MsgErr[147] = (char*)"тип метки не совпадает с типом выбирающего выражения";
	MsgErr[149] = (char*)"тип индекса не может быть  REAL  или  INTEGER";
	MsgErr[152] = (char*)"в этой записи нет такого поля";
	MsgErr[156] = (char*)"метка варианта определяется несколько раз";
	MsgErr[165] = (char*)"метка определяется несколько раз";
	MsgErr[166] = (char*)"метка описывается несколько раз";
	MsgErr[167] = (char*)"неописанная метка";
	MsgErr[168] = (char*)"неопределенная метка";
	MsgErr[169] = (char*)"ошибка в основании множества(базовом типе)";
	MsgErr[170] = (char*)"тип не может быть упакован";
	MsgErr[177] = (char*)"здесь не допускается присваивание имени функции";
	MsgErr[182] = (char*)"типы не совместны";
	MsgErr[183] = (char*)"запрещенная в данном контексте операция";
	MsgErr[184] = (char*)"элемент этого типа не может иметь знак";
	MsgErr[186] = (char*)"несоответствие типов для операции отношения";
	MsgErr[189] = (char*)"конфликт типов параметров";
	MsgErr[190] = (char*)"повторное опережающее описание";
	MsgErr[191] = (char*)"ошибка в конструкторе множества";
	MsgErr[193] = (char*)"лишний индекс для доступа к элементу массива";
	MsgErr[194] = (char*)"указано слишком мало индексов для доступа к злементу массива";
	MsgErr[195] = (char*)"выбирающая константа вне границ описанного диапазона";
	MsgErr[196] = (char*)"недопустимый тип выбирающей константы";
	MsgErr[197] = (char*)"параметры процедуры(функции), являющейся параметром, д.б.пар. - значениями";
	MsgErr[198] = (char*)"несоответствие количества параметров параметра - процедуры(функции)";
	MsgErr[199] = (char*)"несоответствие типов параметров параметра - процедуры(функции)";
	MsgErr[200] = (char*)"тип парамера - функции не соответствует описанию";
	MsgErr[201] = (char*)"ошибка в вещественной константе : должна идти цифра";
	MsgErr[203] = (char*)"целая константа превышает предел";
	MsgErr[206] = (char*)"слишком маленькая вещественная константа";
	MsgErr[207] = (char*)"слишком большая вещественная константа";
	MsgErr[208] = (char*)"недопустимые типы операндов в операции IN";
	MsgErr[209] = (char*)"вторым операндом IN должно быть множество";
	MsgErr[210] = (char*)"операнды AND, NOT, OR должны быть булевыми";
	MsgErr[211] = (char*)"недопустимые типы операндов в операции + или -";
	MsgErr[212] = (char*)"операнды DIV и MOD должны быть целыми";
	MsgErr[213] = (char*)"недопустимые типы операндов в операции *";
	MsgErr[214] = (char*)"недопустимые типы операндов в операции /";
	MsgErr[215] = (char*)"первым операндом IN должно быть выражение базового типа множества";
	MsgErr[216] = (char*)"опережающее описание есть, полного нет";
	MsgErr[305] = (char*)"индексное значение выходит за границы";
	MsgErr[306] = (char*)"присваиваемое значение выходит за границы";
	MsgErr[307] = (char*)"выражение для элемента множества выходит за пределы";
	MsgErr[308] = (char*)"выражение выходит за допустимые пределы";
	MsgErr[309] = (char*)"должно идти слово EXPORT";
	MsgErr[310] = (char*)"должно идти слово IMPORT";
	MsgErr[311] = (char*)"должно идти слово INTERFACE";
	MsgErr[312] = (char*)"должно идти слово IMPLEMENTATION";
	MsgErr[313] = (char*)"должно идти слово MODULE";
	MsgErr[314] = (char*)"слишком много меток";
	MsgErr[322] = (char*)"Странный оператор";
	MsgErr[323] = (char*)"Плохие константы в ограниченном типе";
	MsgErr[324] = (char*)"Небывает такого типа";
	MsgErr[325] = (char*)"Неправильный вызов процедуры";
	MsgErr[326] = (char*)"Слишком мало параметров";
	MsgErr[327] = (char*)"Слишком много параметров";
	MsgErr[328] = (char*)"Несоответствие типов";
	MsgErr[329] = (char*)"Слишком много индексов";
	MsgErr[330] = (char*)"Слишком мало индексов";
	MsgErr[331] = (char*)"Должен идти тип";
	MsgErr[332] = (char*)"Переменная не является массивом";
	MsgErr[333] = (char*)"Неправильный способ использования идентификатора";
}
/*чтение новой строки*/
void ListThisLine(char *line)
{
	file_listing << setw(4) << setfill(' ') << positionnow.linenumber+1 << "  " <<line;
}
/*вывод ошибок для строки с номером i*/
void WriteErrorsLine(int i)
{
	file_listing << "**" << setw(2) << setfill('0') << ErrInx << "**";
	for (unsigned k = 1;k < ErrTable[i].ErrPos.charnumber;k++)
		file_listing << " ";
	file_listing << "^ ошибка код " << ErrTable[i].ErrCode << endl;
	if (ErrTable[i].ErrCode > 0 && ErrTable[i].ErrCode <= 334)
		file_listing << "****** " << MsgErr[ErrTable[i].ErrCode] << endl;
	else file_listing << "****** Расшифровка для данной ошибки не существует!" << endl;
}
/*функция вывода ошибок для строки current_str*/
void WriteErrorsListing(unsigned current_str)
{
	while (i < ErrTable.size() && ErrInx)
	{
		if (ErrTable[i].ErrPos.linenumber == current_str-1)
			WriteErrorsLine(i);
		i++;
	}
}
/*функция чтения новой строки из программы*/
void ReadNextLine()
{
	if (!feof(file_program))
	{
		fgets(line, 100, file_program);
		LastInLine = strlen(line);
	}
}
/*функция добавления ошибки в таблицу ошибок*/
void Error(unsigned errorcode, unsigned number_str, unsigned number_pos)
{
	if (ErrInx == ErrMax)
		ErrorOverflow = true;
	else
	{
		ErrTable[ErrInx].ErrCode = errorcode;
		ErrTable[ErrInx].ErrPos.charnumber = number_pos+1;
		ErrTable[ErrInx].ErrPos.linenumber = number_str;
		ErrInx++;
	}
}
/*функция чтения ошибок из файла с ошибками*/
void ReadErrors()
{
	/*зануляем значения*/
	for (int i = 0; i < ErrMax; i++)													 //обнуление таблицы ошибок
	{
		ErrTable[i].ErrCode = 0;
		ErrTable[i].ErrPos.charnumber = 0;
		ErrTable[i].ErrPos.linenumber = 0;
	}
	ErrInx = 0;
}
/*функция печати кода символа в файл*/
void PrintSym()
{
	fprintf(file_rezult_lex, "%u ", Symbol);
}
/*функция печати синтаксической ошибки в файл с ошибками*/
void PrintErrorSym(unsigned errorcode, textposition position)
{
	//flag_err = true;
	//PrintSym();
	//ErrInx++;
	Error(errorcode, position.linenumber, position.charnumber);
	fprintf(file_rez_err, "%d %d %d\n", position.linenumber, position.charnumber, errorcode);
}
/*функция чтения очередной литеры строки*/
char NextCh()
{
	i = 0;
	if (positionnow.charnumber == LastInLine)
	{
		ListThisLine(line);
		WriteErrorsListing(positionnow.linenumber + 1);
		ReadNextLine();
		positionnow.linenumber++;
		positionnow.charnumber = 0;
	}
	else positionnow.charnumber++;
	return ch = line[positionnow.charnumber];
}
/*функция пропускающая сам коментарий*/
void End_Comment()
{
	unsigned Prev_Symbol = Symbol;
	char prev_ch;
	while (Symbol != rcommentc && Symbol != frparc && !feof(file_program)) {
		prev_ch = ch;
		if (!feof(file_program))
		{
			if (NextCh() == ')' && prev_ch == '*' && Prev_Symbol == lcommentc) Symbol = rcommentc;
			if (ch == '}' && Prev_Symbol == flparc) Symbol = frparc;
		}
	}
	if (feof(file_program) && (Symbol != rcommentc || Symbol != frparc))
	{
		positionnow.charnumber = LastInLine-2;
		PrintErrorSym(86, positionnow);
		flag = 1;
		ch = prev_ch;
	}
	else
		NextCh();
}
/*определение числа*/
void DetermineTheNumber(int sign)
{
	/*
		Принцип работы алгоритма для чтения чсловых констант:
		1. Пока считываемая лексема является цифрой, "собираем" числовую константу поразрядно слева направо;
		2. Параллельно с процессом "сборки" числа проверяем на очередном шаге, не выходит ли константа за границы допустимого
		значения;
	*/
	err_203 = false;
	intConstant = 0;
	mn = 1;
	//ch = ch_first;
	while (ch >= '0' && ch <= '9')
	{
		nextDigit = ch - '0';
		if (intConstant < maxint / 10 || (intConstant == maxint / 10 && nextDigit <= maxint % 10))
		{
			intConstant = 10 * intConstant + nextDigit;
			NextCh();
		}
		else
		{
			/*целая константа превышает предел*/
			err_203 = true;
			while (isdigit(ch))
				NextCh();
		}
	}
	if ((ch == '.') && ((NextCh() >= '0') && (ch <= '9') || ch == ' '))
	{
		realConstant = (double)(intConstant);
		while ((ch >= '0') && (ch <= '9'))
		{
			nextDigit = ch - '0';
			mn *= 0.1;
			realConstant += nextDigit * mn;
			NextCh();
		}
		if (ch == 'e')
		{
			if ((NextCh() >= '0') && (ch <= '9'))
			{
				intE = 0;
				while ((ch >= '0') && (ch <= '9'))
				{
					nextDigit = ch - '0';
					intE = 10 * intE + nextDigit;
					NextCh();
				}
			}
			else {
				//positionnow.charnumber--;
				if (ch == '+' && ((NextCh() >= '0') && (ch <= '9')))
				{
					intE = 0;
					while ((ch >= '0') && (ch <= '9'))
					{
						nextDigit = ch - '0';
						intE = 10 * intE + nextDigit;
						NextCh();
					}
				}
				else {
					//positionnow.charnumber--;
					if ((ch == '-' && ((NextCh() >= '0') && (ch <= '9'))))
					{
						intE = 0;
						while ((ch >= '0') && (ch <= '9'))
						{
							nextDigit = ch - '0';
							intE = 10 * intE + nextDigit;
							NextCh();
						}
						intE *= -1;
					}
					else { positionnow.charnumber - 2; }
				}
			}
			realConstant *= powf(10, intE);
		}
		realConstant *= sign;
		Symbol = realc;
		if (realConstant > maxreal)
		{
			PrintErrorSym(207, token);
		}
		if (realConstant < -maxreal)
		{
			PrintErrorSym(206, token);
		}
	}
	else
	{
		if (err_203)
		{
			PrintErrorSym(203, token);
		}
		intConstant *= sign;
		Symbol = integerc;
	}
}
/*функция чтения очередной лексемы*/
void NextSym()
{
	int nextnextDigit = 0;
	int lname;
	int i;

	flag = 0;
	int flag_com = 0;
	while (ch == ' ')
		NextCh();
	token.linenumber = positionnow.linenumber;
	token.charnumber = positionnow.charnumber;

	switch (ch >= '0' && ch <= '9' ? 1 : ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) ? 2 : ch)
	{
		case 1:
			DetermineTheNumber(1);
			break;
		case 2:
			//ch = ch_first;
			lname = 0;
			while (((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || (ch == '_')) && lname < MAX_DL_IDENT)
			{
				name[lname++] = ch;
				NextCh();
			}
			name[lname] = '\0';
			if (lname < MAX_DL_IDENT)
			{
				strcpy_s(KeyWords[last[lname]].NameKey, name);
				i = last[lname - 1] + 1;
				while (strcmp(KeyWords[i].NameKey, name) != 0)
					i++;
				Symbol = KeyWords[i].CodeKey;
			}
			else
			{
				Symbol = ident;
				//PrintErrorSym(76, token);
				while (ch != ' ')
					NextCh();
			}
			break;

			/*
			Принцип работы алгоритма для чтения идентификаторов и ключевых слов:
			В массиве структур KeyWords хранится код ключевого слова или идентификатора и его эквивалент строкового типа.
			В массиве last хранятся индексы "пустых" позиций, в которые будут записываться очередные символы.
			1. Пока лексема является буквой, то "собираем" символ;
			2. Параллельно со "сборкой" символа сразу считаем его длину.
			3. В массив KeyWords вставляем считанный символ:
				3.1. Позиция берется из массива last;
				3.2. Из массива last позиция берется из элемента, номер которого равен длине считанного символа;
			4. Сравниваем строковое эквиваленты массива KeyWords и считанный символ, начиная с первого элемента блока
			(eyWords разделены на блоки по длине слов);
			5. При этом запоминаем индекс совпадения символов;
			6. Если этот индекс равен индексу из массива last, то выводим код ident, иначе код из массива KeyWords;
			*/
		case '\'':
			while (NextCh() != '\'' && ch != EOF)
				len_c++;
			if (len_c) {
				if (len_c > MAX_DL_STRINGC)
					PrintErrorSym(76, token);
				Symbol = stringc;
			}	
			else
				Symbol = charc;
			len_c = 0;
			NextCh();
			break;
		case ':':
			NextCh();
			if (ch == '=')
			{
				Symbol = assignc;
				NextCh();
			}
			else
				Symbol = colonc;
			break;
		case ';':
			Symbol = semicolonc;
			NextCh();
			break;
		case '<':
			NextCh();
			if (ch == '=')
			{
				Symbol = laterequalc;
				NextCh();
			}
			else
				if (ch == '>')
				{
					Symbol = latergreaterc;
					NextCh();
				}
				else
					Symbol = laterc;
			break;
		case '>':
			NextCh();
			if (ch == '=')
			{
				Symbol = greaterequalc;
				NextCh();
			}
			else
				Symbol = greaterc;
			break;
		case '+':
			Symbol = plusc;
			NextCh();
			break;
		case '-':
			if (NextCh() >= '0' && ch <= '9')
				DetermineTheNumber(-1);
			else {
				Symbol = minusc;
			}
			break;
		case '/':
			Symbol = slashc;
			NextCh();
			break;
		case  '=':
			Symbol = equalc;
			NextCh();
			break;
		case '*':
			NextCh();
			if (ch == ')')
			{
				Symbol = rcommentc;
				NextCh();
			}
			else
				Symbol = starc;
			break;
		case '(':
			NextCh();
			if (ch == '*')
			{
				Symbol = lcommentc;
				PrintSym();
				NextCh();
				End_Comment();
				//flag_com = 1;

			}
			else
				Symbol = leftparc;
			break;
		case ')':
			Symbol = rightparc;
			NextCh();
			break;
		case '{':
			Symbol = flparc;
			PrintSym();
			NextCh();
			End_Comment();
			//flag_com = 1;
			break;
		case '}':
			Symbol = frparc;
			NextCh();
			break;
		case '[':
			Symbol = lbracketc;
			NextCh();
			break;
		case ']':
			Symbol = rbracketc;
			NextCh();
			break;
		case '.':
			NextCh();
			if (ch == '.')
			{
				Symbol = twopointsc;
				NextCh();
			}
			else
				Symbol = pointc;
			break;
		case ',':
			Symbol = commac;
			NextCh();
			break;
		case '^':
			Symbol = arrowc;
			NextCh();
			break;
		//case '"':
		//	while (ch != '"' && ch != EOF)
		//	{
		//		NextCh();
		//	}
		//	Symbol = stringc;
		//	break;
		default: switch (ch)
		{
			case '\n':
				if (Symbol == integerc) {
					NextCh();
					NextCh();
				}
				else
					NextCh();
				fprintf(file_rezult_lex, "\n");
				flag = 1;
				break;
			case 0:
				NextCh();
				flag = 1;
				break;
			default:
				Symbol = bad_Symbol;
				PrintErrorSym(6, token);
				NextCh();
				break;
		}
	}
	if (flag != 1)
		PrintSym();

}

void main()
{
	setlocale(LC_ALL, "rus");
	fopen_s(&file_program, "exampl.txt", "r");
	fopen_s(&file_place_err, "exampl_err.txt", "r");
	fopen_s(&file_rez_err, "example_rez_err.txt", "w");
	fopen_s(&file_rezult_lex, "Rezult_Lex.txt", "w");
	file_listing.open("listing.txt");
	
	MSGERR();				
	ReadErrors();

	file_listing << "             Работает Pascal - компилятор\n";
	file_listing << "                   Листинг программы\n";
	ReadNextLine();
	ch = line[0];
	while ((token.charnumber != LastInLine) || !feof(file_program))
	{
		NextSym();
	}
	if (ErrorOverflow)
		file_listing << "\nКoмпиляция окончена: ошибок > " << ErrInx << "!" << endl;
	else
		file_listing << "\nКoмпиляция окончена: ошибок - " << ErrInx << "!" << endl;

	fclose(file_program);
	fclose(file_place_err);
	fclose(file_rez_err);
	fclose(file_rezult_lex);
	file_listing.close();
}