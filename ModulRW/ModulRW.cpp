#include "pch.h"
#include "Parsing.h"

using namespace std;
/* Описание реализуемых в виде битовых строк множеств символов, 	*/
/* стартовых для различных обрабатываемых конструкций:			*/

unsigned

	*idstarters,		/* множество из одного стартового символа ident */
	*begpart,		/* стартовые символы функции block()		*/
	*rpar,			/* правая скобка 				*/
	*st_constpart,	/* стартовые символы функции constpart()	*/
	*st_typepart,		/* стартовые символы функции typepart()		*/
	*st_varpart,		/* стартовые символы функции varpart()		*/
	*st_procfuncpart,	/* стартовые символы функции procfuncpart()	*/
	*st_statpart,		/* стартовые символы функции statpart()		*/
	*st_constant,	 	/* стартовые символы функции constant()		*/
	*st_conaftersign,	/* стартовые символы конструкции константы, 	*/
	/* идущей после знака + или -					*/
	*st_typ,		/* стартовые символы функции typ()		*/
	*st_typafterpack,	/* стартовые символы конструкции описания типа 	*/
	/* после ключевого слова packedsy				*/
	*st_simpletype,	/* стартовые символы функции simpletype()	*/
	*st_fixpart,            /* стартовые символы функции fixpart()	и 	*/
	/* функции reestrfields()					*/
	*st_casefield,          /* стартовые символы функции casefield()	*/
	*st_statement,          /* стартовые символы конструкции <оператор>     */
	*st_startstatement,     /* стартовые символы оператора при нейтрализации*/
	*st_variant,            /* стартовые символы конструкции <вариант> в    */
		/* раздела компиляции выражений 				*/
	*st_express,		/* ... выражения 				*/
	*st_termfact;		/* ... слагаемого и множителя 			*/


	/* Описание реализуемых в виде битовых строк множеств символов, 	*/
	/* ожидаемых сразу после обработки различных конструкций:		*/

unsigned

	*blockfol,		/* ...после обработки блока основной программы  */
	*af_1constant,		/* ...после анализа константы при вызове функ-	*/
	/* ции constant() из функции constdeclaration(), а также после  */
	/* анализа конструкции объявления переменных при вызове функции */
	/* vardeclaration() из функции varpart()			*/
	*af_2constant,		/* ...после анализа константы при вызове функ-	*/
	/* ции constant() из функции casefield()			*/
	*af_3const1,		/* ...после анализа первой константы отрезка	*/
	/* при обработке оного в функции simpletype()			*/
	*af_4const2,		/* ...после анализа второй константы отрезка	*/
		/* при обработке оного в функции simpletype()			*/
	*af_simpletype,		/* ...после анализа конструкции типа индекса во */
		/* время обработки типа массива					*/
	*af_1typ,		/* ...после анализа конструкции описания типа	*/
		/* при вызове функции typ() из typedeclaration()		*/
	*af_2typ,		/* ...после анализа конструкции описания типа	*/
		/* при вызове функции typ() из fixpart()			*/
	*af_1reestrfields,	/* ...после анализа конструкции списка полей 	*/
		/* при вызове функции reestrfields() из функции complextype()	*/
	*af_2reestrfields,	/* ...после анализа конструкции списка полей,	*/
		/* вложенного внутри записи, т.е. при вызове reestrfields() из	*/
		/* casefield()							*/
	*af_fixpart,		/* ...после анализа фиксированной части списка 	*/
		/* полей 							*/
	*af_proclistparam,      /* ...после анализа списка параметров процедуры */
	*af_funclistparam,      /* ...после анализа списка параметров функции   */
	*af_blockprocfunc,      /* ...после анализа блока процедуры или функции */
	*af_sameparam,          /* ...после анализа однотипных параметров       */
	*af_factparam,          /* ...после анализа фактических параметров      */
		/* процедур и функций                                           */
	*af_oneparam,           /* ...после анализа параметра стандартных       */
		/* процедур и функций, имеющих один параметр                    */
	*af_writeparam,         /* ...после анализа параметра стандартных       */
		/* процедур write и writeln                                     */

	*af_assignment,         /* ...после анализа переменной в операторе      */
		/* присваивания							*/
	*af_compstatement,      /* ...после анализа оператора в составном оп-ре */
	*af_iftrue,             /* ...после анализа условного выражения в опера-*/
		/*торе if							*/
	*af_iffalse,            /* ...после анализа оператора ветви "истина" в  */
		/* операторе if							*/
	*af_whilefor,		/* ...после анализа условного выражения в опера-*/
		/* торе while и выражения-второй границы изменения параметра    */
		/* цикла в операторе for					*/
	*af_repeat,             /* ...после анализа оператора в теле цикла repeat*/
	*af_with,               /* ...после анализа  переменной  в   заголовке  */
		/* оператора with						*/
	*af_case1,              /* ...после анализа выбирающего выражения в case*/
	*af_case2,              /* ...после анализа варианта в операторе case   */
	*af_forassign,	        /* ...после анализа переменной в операторе for	*/
	*af_for1, 		/* ...после анализа выражения-первой границы из-*/
		/* менения параметра цикла в операторе for			*/
	*af_ident,		/* ...после идентификатора в "переменной"	*/
	*af_index,		/* ...после индекса при разборе массива		*/
	*af_set1,		/* ...после 1-го выражения в конструкторе множ. */
	*af_label,              /* ...после анализа метки в конструкции описания*/
	*af_label1,             /* ... после анализа конструкции описания меток */
	*af_goto;               /* ...после goto                                */

	/* Описание реализуемых в виде битовых строк множеств допустимых симво- */
	/* лов операций в разделе компиляции выражений 				*/

unsigned

	*op_rel,		/* операции отношения над простыми выражениями	*/
	*op_add,		/* аддитивные операции над слагаемыми		*/
	*op_mult;		/* мультипликативные операции над множителями	*/

/* Описание реализуемых в виде битовых строк множеств способов исполь-	*/
/* зования идентификаторов:						*/

unsigned

	*set_VARCONFUNCS,	/* доп. способы использования - VARS, CONSTS, FUNCS */
	*set_VARS,	/* допустимый способ использования - VARS		*/
	*set_TYPES, 	/* допустимый способ использования - TYPES 		*/
	*set_CONSTS,	/* допустимый способ использования - CONSTS 		*/
	*set_TYCON,     /* допустимые способы использования - TYPES,CONSTS	*/
	*set_FUNCS,     /* допустимый способ использования - FUNCS              */
	*set_PROCS,     /* допустимый способ использования - PROCS              */
	*set_FUNPR,     /* допустимые способы использования - FUNCS,PROCS       */
	*set_VARFUNPR;  /* допустимые способы использования - VARS,FUNCS,PROCS  */

/* Описание реализуемого в виде битовой строки множества кодов типов,	*/
/* недопустимых для использования в том или ином контексте:		*/

unsigned *illegalcodes;
vector <ErrorTable> ErrTable(ErrMax);	// таблица ошибок из  файла с ошибками

unsigned i = 0,							// счётчик
ErrorOverflow = false,			// 
ErrInx = 0,						// счётчик ошибок
LastInLine;						// длина текущей строки
unsigned Symbol;						//код символа
char ch = '-';							// текущая литера
char line[90] = "";						// текущая строка
char **MsgErr = new char *[ErrCount];	//массив сообщений об ошибках
int flag;
bool err_203;
char name[80];							//лексема
int lname;								//длина лексемы
unsigned len_c;							//длина строковой констатнты
int nextDigit;							//распознаная цифра
long int intConstant;						//целая константа
int intE;								//показатель вещественного числа
double realConstant;					//вещественная константа
bool boolConstant;						//логическая константа
float mn;
FILE *file_program, 					// файл с текстом программы
*file_rezult_lex;					// файл результата работы лексического анализатора
ofstream file_listing;					// файл листинга программы


map <string, unsigned> KeyWords =
/* таблица ключевых слов и их кодов 		*/
{
	{ "do", dosy},
	{ "if", ifsy},
	{ "in", insy},
	{ "of", ofsy},
	{ "or", orsy},
	{ "to", tosy},
	{ "and", andsy},
	{ "div", divsy},
	{ "end", endsy},
	{ "for", forsy},
	{ "mod" ,modsy},
	{ "nil", nilsy},
	{ "not", notsy},
	{ "set", setsy},
	{ "var", varsy},
	{ "case", casesy},
	{ "else", elsesy},
	{ "file", filesy},
	{ "goto", gotosy},
	{ "only", onlysy},
	{ "then", thensy},
	{ "type", typesy},
	{ "unit", unitsy},
	{ "uses", usessy},
	{ "with", withsy},
	{ "array", arraysy},
	{ "begin", beginsy},
	{ "const", constsy},
	{ "label", labelsy},
	{ "until", untilsy},
	{ "while", whilesy},
	{ "export", exportsy},
	{ "import", importsy},
	{ "downto", downtosy},
	{ "module", modulesy},
	{ "packed", packedsy},
	{ "record", recordsy},
	{ "repeat", repeatsy},
	{ "vector", vectorsy},
	{ "string", stringsy},
	{ "forward", forwardsy},
	{ "process", processsy},
	{ "program", programsy},
	{ "segment", segmentsy},
	{ "function", functionsy},
	{ "separate", separatesy},
	{ "interface", interfacesy},
	{ "procedure", proceduresy},
	{ "qualified", qualifiedsy},
	{ "implementation", implementationsy},
};

textposition token;
textposition positionnow;

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
/*функция зануления таблицы ошибок*/
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
	Error(errorcode, position.linenumber, position.charnumber);
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
			{
				nextDigit = ch - '0';
				intConstant = 10 * intConstant + nextDigit;
				NextCh();
			}
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
		Symbol = realc;//
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

	flag = 0;
	while (ch == ' ')
		NextCh();
	if (ch == '\n')
	{
		NextCh(); NextCh();
		fprintf(file_rezult_lex, "\n");
	}
	token.linenumber = positionnow.linenumber;
	token.charnumber = positionnow.charnumber;

	switch (ch >= '0' && ch <= '9' ? 1 : ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) ? 2 : ch)
	{
		case 1:
			DetermineTheNumber(1);
			break;
		case 2:
			lname = 0;
			while (((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || (ch == '_')) && lname < MAX_DL_IDENT)
			{
				name[lname++] = ch;
				NextCh();
			}
			name[lname] = '\0';
			if (lname < MAX_DL_IDENT)
			{
				if (!strcmp(name, "TRUE"))
				{
					Symbol = TRUE;
					boolConstant = true;
				}
				else if (!strcmp(name, "FALSE"))
				{
					Symbol = FALSE;
					boolConstant = false;
				}
				else
				{
					if (KeyWords.count(name))
						Symbol = KeyWords.find(name)->second;
					else
						Symbol = ident;
				}
			}
			else
			{
				Symbol = ident;
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
			//if (NextCh() >= '0' && ch <= '9')
				//DetermineTheNumber(-1);
			//else {
				Symbol = minusc;
			//}
			NextCh();
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
		default: switch (ch)
		{
			case '\n':
				if (Symbol == integerc) {
					NextCh();
					NextCh();
				}
				else
					NextCh();
				//fprintf(file_rezult_lex, "\n");
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
/*---------------------------- B E L O N G -----------------------------*/
/*	Функция belong. Осуществляет поиск указанного элемента в множестве.
	Возвращает истину в случае наличия элемента, иначе ложь.
*/
bool Belong(unsigned element, unsigned *set)	/* номер элемента, который ищем, множество, в котором ищем элемент */
{
	unsigned WordNumber,						/* номер слова строки, в к-м может быть элемент */
	BitNumber;									/* номер бита, соответствующего элементу */
	WordNumber = element / WORDLENGTH;
	BitNumber = element % WORDLENGTH;
	return(set[WordNumber] & (1 << (WORDLENGTH - 1 - BitNumber)));
}
/*----------------------------- A C C E P T ----------------------------*/
void Accept(unsigned symbolexpected)	/* код ожидаемого символа*/
{
	if (Symbol == symbolexpected) NextSym();
	else
	{
		Error(symbolexpected, token.linenumber, token.charnumber);
		//fprintf(t, " Обнаружена ошибка %d (%d)", symbolexpected, Symbol);
	}
}
/*-------------------- C O N V E R T _ T O _ B I T S -------------------*/
unsigned *Convert_To_Bits(unsigned *intstr)
				/* переводит одномерный массив с базо-	*/
				/* вым типом 0..127 в 128-битовую строку, со-	*/
				/* стояние каждого бита в которой отражает от-	*/
				/* сутствие или наличие в исходном массиве эле-	*/
				/* мента, численно равного номеру этого бита	*/
					/* используется для представления в	*/
				/* виде битовых строк стартовых символов раз-	*/
				/* личных языковых конструкций паскаля, симво-	*/
				/* лов, ожидаемых сразу после обработки тех или */
				/* иных конструкций, а также множеств допусти-	*/
				/* мых способов использования идентификатора и 	*/
				/* прочих неблаговидных вещей			*/
					/* аргумент - адрес массива, под-	*/
				/* лежащего конвертированию			*/
				/* ( на "Эльбрусе" - intstr[4] )		*/
					/* возвращает адрес другого массива, -	*/
				/* массива целых чисел с суммарной длиной эле-	*/
				/* ментов в 128 бит, который и именуем 128-би-	*/
				/* товой строкой				*/
{
	unsigned     
		*set,
		*str,		/* вспомогательный указатель на исходный массив */
		wordnum,	/* номер слова - составляющего битовой строки	*/
		bitnum;		/* номер бита в слове, содержащемся в строке	*/

	str = intstr;
	set = (unsigned*)calloc(8, sizeof(unsigned));
	while (*str != eolint)	/* пока не начался...			*/
	{
		wordnum = *str / WORDLENGTH;	/* вычисление номера слова, в котором находится бит с номером *str			*/
		bitnum = *str % WORDLENGTH;	/* вычисление номера бита в	*/
			/* слове, в котором он находится			*/
		set[wordnum] |= (1 << (WORDLENGTH - 1 - bitnum));
		str++;
	}
	return((unsigned*)set);
}
/*----------------------- S e t D i s j u n c t ------------------------*/
/*	Функция SetDisjunct. Осуществляет дизъюнкцию множеств set1 и set2. Результатом становится множество set3.*/
void SetDisjunct(unsigned set1[], unsigned set2[], unsigned set3[])
{
	unsigned i;	/* параметр цикла */
	for (i = 0; i < SET_SIZE; ++i) set3[i] = set1[i] | set2[i];
}
/*---------------------------- S K I P T O 1 ---------------------------*/
/* пропуск символов, пока не встречен символ, принадлежащий данному множеству*/
void		 Skipto1(unsigned *set)/* указатель на данное 	*/
{
	while ((!Belong(Symbol, set)) && (Symbol != endoffile)) NextSym();
}
/*---------------------------- S K I P T O 2 ---------------------------*/
/* пропуск символов, пока не встречен символ, принадлежащий одному из данных множеств 		        */
void		 Skipto2(unsigned *set1, unsigned *set2)
{
	while ((!Belong(Symbol, set1)) && (!Belong(Symbol, set2)) && (Symbol != endoffile))
		NextSym();
}



void main()
{
	setlocale(LC_ALL, "rus");

/* Множества КОДОВ символов, стартовых для различных обрабатываемых 	*/
/* конструкций:								*/

	unsigned

		codes_idstart[] = { ident,eolint },	/* стартовые символы некоторых 	*/
				/* обрабатываемых конструкций 				*/
		codes_block[] = { labelsy,constsy,typesy,varsy,functionsy,
			proceduresy,beginsy,eolint },	/* стартовые символы разделов	*/
				/* описаний и раздела операторов			*/
		codes_rightpar[] = { rightparc,eolint },
		/* правая скобка 					*/
		codes_constant[] = { plusc,minusc,charc,stringc,ident,integerc,realc,
			eolint },	/* стартовые символы конструкции constant       */
		codes_typ[] = { packedsy,arrowc,arraysy,filesy,setsy,recordsy,
			plusc,minusc,ident,leftparc,integerc,charc,stringc,eolint },
		/* стартовые символы конструкции описания типа;	*/
	/* {plus,minus,ident,leftpar,intc,charc,stringc,eolint}-*/
	/* стартовые символы конструкции описания простого типа */
		codes_fixpart[] = { ident,semicolonc,casesy,endsy,eolint },
		/* стартовые символы конструкции списка полей и кон-	*/
		/* струкции фиксированной части	записи			*/
		codes_casefield[] = { realc,endsy,semicolonc,plusc,minusc,charc,
			stringc,ident,integerc,eolint },
		/* стартовые символы конструкции варианта       	*/
		codes_statement[] = { integerc,endsy,elsesy,untilsy,ident,beginsy,ifsy,
			whilesy,repeatsy,forsy,casesy,withsy,semicolonc,gotosy,eolint },
		/* стартовые символы конструкции <оператор>             */
		/* (при нейтрализации ошибок используются символы:      */
		/* {beginsy,ifsy,whilesy,repeatsy,forsy,casesy,withsy,  */
		/* semicolon,gotosy,eolint} )                           */
		codes_express[] = { plusc, minusc, leftparc, lbracketc, notsy, ident,
			integerc, realc, charc, stringc, nilsy, eolint },
		/* стартовые символы выражения и простого выражения 	*/
		codes_termfact[] = { ident, leftparc, lbracketc, notsy, integerc, realc,
			charc, stringc, nilsy, eolint };
	/* стартовые символы слагаемого и множителя 		*/


/* Множества кодов символов, ожидаемых после обработки различных кон-	*/
/* струкций:								*/

	unsigned

		acodes_block[] = { pointc,endoffile,eolint },	/* сим-	*/
				/* волы, следующие за конструкцией блока в основной 	*/
				/* программе			 			*/
		acodes_simpletype[] = { commac,rbracketc,eolint }, 	/* сим- */
				/* волы, ожидаемые сразу после вызова simpletype() во 	*/
				/* время анализа типа "массив" 				*/
		acodes_fixpart[] = { casesy,rightparc,endsy,eolint },	/* символы, 	*/
				/* ожидаемые сразу после анализа конструкции fixpart;	*/
				/* {rightpar,endsy,eolint} - символы, ожидаемые сразу 	*/
				/* после анализа списка полей при вызове reestrfields() */
				/* из casefield() ( а при вызове из complextype() ожи-	*/
				/* даемый символ только endsy )				*/
		acodes_typ[] = { endsy,rightparc,semicolonc,eolint },	/* сим-	*/
				/* волы, ожидаемые сразу после анализа конструкции typ 	*/
				/* при вызове функции typ() из fixpart()		*/
		acodes_2constant[] = { commac,colonc,eolint }, 	/* символы, ожидаемые 	*/
				/* сразу после анализа константы при вызове constant()	*/
				/* из casefield() и variant()				*/
		acodes_3const[] = { twopointsc,commac,rbracketc,eolint },	/* коды */
				/* символов, ожидаемых сразу после анализа константы 	*/
		acodes_listparam[] = { colonc,semicolonc,forwardsy,constsy,varsy,eolint },
		/* символы, ожидаемые сразу после списка параметров     */
		/* ( символы functionsy,proceduresy,beginsy уже есть в  */
		/*   followers)                                         */
		acodes_factparam[] = { commac,rightparc,eolint },
		/* символы, ожидаемые сразу после разбора фактических   */
		/* параметров процедур и функций                        */
		acodes_assign[] = { assignc,eolint },
		/* символ, ожидаемый сразу после переменной в операторе */
		/* присваивания и в операторе for                       */
		acodes_compcase[] = { semicolonc,endsy,eolint },
		/* символы, ожидаемые сразу после оператора в составном */
		/* операторе и после варианта в операторе варианта      */
		acodes_iftrue[] = { thensy,eolint },
		/* символ, ожидаемый сразу после условного выражения в  */
		/* операторе if						*/
		acodes_iffalse[] = { elsesy,eolint },
		/*символ, ожидаемый сразу после оператора ветви "истина"*/
		/* в операторе if					*/
		acodes_wiwifor[] = { commac,dosy,eolint },
		/* символы, ожидаемые сразу после переменной в заголовке*/
		/* оператора with;  { dosy,eolint} - символ, ожидаемый  */
		/* сразу после условного выражения в операторе while и  */
		/* сразу после выражения-второй границы изменения пара- */
		/* метра цикла в операторе for				*/
		acodes_repeat[] = { untilsy, semicolonc, eolint },
		/* cимволs, ожидаемые сразу после оператора в теле      */
		/* оператора repeat					*/
		acodes_case1[] = { ofsy,eolint },
		/* символ, ожидаемый сразу после выбирающего выражения  */
		/* в операторе case					*/
		acodes_for1[] = { tosy,downtosy,eolint },
		/* символы, ожидаемые сразу после выражения-первой гра- */
		/* ницы изменения пераметра цикла в операторе for       */
		acodes_ident[] = { lbracketc, arrowc, pointc, eolint },
		/* ... после идентификатора в переменной 		*/
		acodes_index[] = { rbracketc, commac, eolint },
		/* ... после индексного выражения при разборе массива	*/
		acodes_set1[] = { rbracketc, twopointsc, commac, eolint };
	/* ... после 1-го выражения в конструкторе множества 	*/

/* Множества кодов операций в разделе компиляции выражений: 		*/

	unsigned

		codes_rel[] = { laterc, laterequalc, greaterc, greaterequalc,
			equalc, latergreaterc, insy, eolint },
		/* операции отношения 					*/
		codes_add[] = { plusc, minusc, orsy, eolint },
		/* аддитивные операции 					*/
		codes_mult[] = { starc, slashc, divsy, modsy, andsy, eolint };
	/* мультипликативные операции				*/

/* Множества кодов допустимых способов использования идентификаторов:	*/

	unsigned

		codes_VARCONFUNCS[] = { VARS,FUNCS,CONSTS,eolint },
		codes_VARS[] = { VARS,eolint },
		codes_CONSTS[] = { CONSTS,eolint },
		codes_TYPES[] = { TYPES,eolint },
		codes_TYCON[] = { TYPES,CONSTS,eolint },
		codes_FUNCS[] = { FUNCS,eolint },
		codes_VARFUNPR[] = { VARS,FUNCS,PROCS,eolint };



	/* Множества кодов типов, недопустимых для использования в том или ином */
	/* контексте:								*/

	unsigned codes_illegal[] = { REFERENCES,RECORDS,SETS,FILES,ARRAYS,eolint };


	illegalcodes = Convert_To_Bits(codes_illegal);
	idstarters = Convert_To_Bits(codes_idstart);
	begpart = Convert_To_Bits(codes_block);
	st_constpart = Convert_To_Bits(codes_block + 1);
	st_typepart = Convert_To_Bits(codes_block + 2);
	st_varpart = Convert_To_Bits(codes_block + 3);
	st_procfuncpart = Convert_To_Bits(codes_block + 4);
	st_statpart = Convert_To_Bits(codes_block + 6);
	st_constant = Convert_To_Bits(codes_constant);
	st_conaftersign = Convert_To_Bits(codes_constant + 4);
	st_typ = Convert_To_Bits(codes_typ);
	st_typafterpack = Convert_To_Bits(codes_typ + 1);
	st_simpletype = Convert_To_Bits(codes_typ + 6);
	st_fixpart = Convert_To_Bits(codes_fixpart);
	st_casefield = Convert_To_Bits(codes_casefield + 1);
	st_startstatement = Convert_To_Bits(codes_statement + 5);
	st_statement = Convert_To_Bits(codes_statement);
	st_variant = Convert_To_Bits(codes_casefield);

	st_express = Convert_To_Bits(codes_express);
	st_termfact = Convert_To_Bits(codes_termfact);

	blockfol = Convert_To_Bits(acodes_block);
	rpar = Convert_To_Bits(codes_rightpar);
	af_1constant = Convert_To_Bits(acodes_typ + 2);
	af_2constant = Convert_To_Bits(acodes_2constant);
	af_3const1 = Convert_To_Bits(acodes_3const);
	af_4const2 = Convert_To_Bits(acodes_3const + 1);
	af_simpletype = Convert_To_Bits(acodes_simpletype);
	af_1typ = Convert_To_Bits(acodes_typ + 1);
	af_2typ = Convert_To_Bits(acodes_typ);
	af_1reestrfields = Convert_To_Bits(acodes_fixpart + 2);
	af_2reestrfields = Convert_To_Bits(acodes_fixpart + 1);
	af_fixpart = Convert_To_Bits(acodes_fixpart);
	af_funclistparam = Convert_To_Bits(acodes_listparam);
	af_proclistparam = Convert_To_Bits(acodes_listparam + 1);
	af_blockprocfunc = Convert_To_Bits(acodes_typ + 2);
	af_sameparam = Convert_To_Bits(acodes_typ + 1);
	af_factparam = Convert_To_Bits(acodes_factparam);
	af_oneparam = Convert_To_Bits(acodes_factparam + 1);
	af_writeparam = Convert_To_Bits(acodes_2constant + 1);

	af_assignment = Convert_To_Bits(acodes_assign);
	af_compstatement = Convert_To_Bits(acodes_compcase);
	af_iftrue = Convert_To_Bits(acodes_iftrue);
	af_iffalse = Convert_To_Bits(acodes_iffalse);
	af_whilefor = Convert_To_Bits(acodes_wiwifor + 1);
	af_repeat = Convert_To_Bits(acodes_repeat);
	af_with = Convert_To_Bits(acodes_wiwifor);
	af_case1 = Convert_To_Bits(acodes_case1);
	af_case2 = Convert_To_Bits(acodes_compcase);
	af_for1 = Convert_To_Bits(acodes_for1);
	af_forassign = Convert_To_Bits(acodes_assign);

	af_set1 = Convert_To_Bits(acodes_set1);
	af_ident = Convert_To_Bits(acodes_ident);
	af_index = Convert_To_Bits(acodes_index);
	af_goto = Convert_To_Bits(codes_casefield + 9);
	af_label = Convert_To_Bits(acodes_index + 1);
	af_label1 = Convert_To_Bits(codes_block + 1);

	op_rel = Convert_To_Bits(codes_rel);
	op_add = Convert_To_Bits(codes_add);
	op_mult = Convert_To_Bits(codes_mult);

	set_VARCONFUNCS = Convert_To_Bits(codes_VARCONFUNCS);
	set_VARS = Convert_To_Bits(codes_VARS);
	set_TYPES = Convert_To_Bits(codes_TYPES);
	set_CONSTS = Convert_To_Bits(codes_CONSTS);
	set_TYCON = Convert_To_Bits(codes_TYCON);
	set_FUNCS = Convert_To_Bits(codes_FUNCS);
	set_FUNPR = Convert_To_Bits(codes_VARFUNPR + 1);
	set_PROCS = Convert_To_Bits(codes_VARFUNPR + 2);
	set_VARFUNPR = Convert_To_Bits(codes_VARFUNPR);


	fopen_s(&file_program, "exampl.txt", "r");
	fopen_s(&file_rezult_lex, "Rezult_Lex.txt", "w");
	file_listing.open("listing.txt");
	
	MSGERR();				
	ReadErrors();

	file_listing << "             Работает Pascal - компилятор\n";
	file_listing << "                   Листинг программы\n";
	ReadNextLine();
	ch = line[0];
	NextSym();
	Programme();
	if (ErrorOverflow)
		file_listing << "\nКoмпиляция окончена: ошибок > " << ErrInx << "!" << endl;
	else
		file_listing << "\nКoмпиляция окончена: ошибок - " << ErrInx << "!" << endl;

	fclose(file_program);
	fclose(file_rezult_lex);
	file_listing.close();
}