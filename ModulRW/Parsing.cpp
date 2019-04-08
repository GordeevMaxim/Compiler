#include "pch.h"
#include "ModulRW.h"
#include "Generation.h"
#include "Parsing.h"

extern unsigned Symbol;
extern char ch;
extern void Accept(unsigned Symbolexpected);
extern void NextSym();
extern char NextCh();
extern bool Belong(unsigned Symbolexpected, unsigned *set);
extern textposition token;

extern long int intConstant;
extern double realConstant;
extern unsigned charConstant;

extern FILE *file_program;
extern unsigned int *op_rel,
*st_express,
*begpart,
*st_typepart,
*st_varpart,
*st_procfuncpart,
*af_sameparam,
*st_all,
*blockfol,
*st_statpart,
*af_compstatement,
*idstarters,
*st_typ,
*m_scolon,
*af_headfunproc,
*af_blockprocfunc,
*rpar,
*m_fpar,
*st_statement,
*comp_stat,
*af_assignment,
*st_termfact,
*af_3const1,
*m_comma,
*op_mult,
*op_add,
*af_iftrue,
*af_for1,
*af_whilefor,
*idstarters,
*m_scolon,
*af_repeat,
*st_constant,
*st_startstatement,
*m_twopoint;

extern unsigned

*set_VARCONFUNCS,	/* доп. способы использования - VARS, CONSTS, FUNCS */
*set_VARS,			/* допустимый способ использования - VARS		*/
*set_TYPES, 		/* допустимый способ использования - TYPES 		*/
*set_CONSTS,		/* допустимый способ использования - CONSTS 		*/
*set_TYCON,			/* допустимые способы использования - TYPES,CONSTS	*/
*set_FUNCS,			/* допустимый способ использования - FUNCS              */
*set_PROCS,			/* допустимый способ использования - PROCS              */
*set_FUNPR,			/* допустимые способы использования - FUNCS,PROCS       */
*set_VARFUNPR;		/* допустимые способы использования - VARS,FUNCS,PROCS  */


/*----------------------------- Переменные для СЕМАНТИКИ -----------------------------*/
int myconst;
int kol;
int level;
SCOPE* localscope;
struct treenode *entry;
#define MAXCONST  	32767
TYPEREC *booltype, *realtype, *chartype, *inttype, *texttype, *niltype;
struct listrec *varlist; // адрес начала вспомогательного стека однотипных переменных
TYPEREC *vartype;//внешняя переменная vartype содержит адрес дескриптора для однотипных переменных*/
/*----------------------------- Конец переменные для СЕМАНТИКИ -----------------------------*/



/* присоединение новой константы к списку констант типа (например true, false) */
struct reestrconsts *newconst(char *addrname)
{
	struct reestrconsts *myconst = (struct reestrconsts *)malloc(sizeof(struct reestrconsts));
	myconst->addrconsts = addrname; /* присваиваем имя новой константы */
	myconst->next = NULL;			/* зануляем конец */
	return(myconst);
}

/*============================================= Работа с таблцей имён =============================================*/
#define MAXHASH  997			/* максимальное значение hash-функции */
#define MAXNUM   500			/* количество литер в строке для разрешения коллизий	*/

extern char name[80];
extern int lname;				/* это длина текущего имени в литерах */
unsigned hashresult;			/* результат hash-функции для текущего имени */
char *addrname;					/* адрес имени в таблице имен */

struct zveno     				/* строка для разрешения коллизий */
{
	int firstfree;				/* номер первой свободной позиции литеры в строке */
	char namelist[MAXNUM];		/* последовательность имен */
	struct zveno *next;			/* указатель на следующую строку с тем же значением hash-функции */
};

typedef struct zveno str;

static str *nametable[MAXHASH],	/* массив указателей на первые строки для разрешения коллизий соответственно текущему значению hash-функции */
*ptrstr; 			/* указатель на текущую строку для разрешения коллизий  */

static unsigned chcount,		/* счетчик литер текущего имени		*/
loccount;		/* счетчик литер в выделенной памяти	*/


//-----------------------------------------БЛОК (НАЧАЛО)-------------------------------------------------------//
/* Анализ конструкции "блок" */
void Block(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
	if (!Belong(Symbol, begpart))//labelsy,constsy,typesy,varsy,functionsy,proceduresy, beginsy, eolint
	{
		Error(18, token); /* ошибка в разделе описаний */
		Skipto2(begpart, followers);
	}
	if (Belong(Symbol, begpart))
	{
		SetDisjunct(st_varpart, followers, ptra);//pointc,endoffile,eolint + varsy,functionsy,proceduresy, beginsy, eolint
		TypePart(ptra);//описание типов
		SetDisjunct(st_procfuncpart, followers, ptra);//pointc,endoffile,eolint + functionsy,proceduresy, beginsy, eolint
		VarPart(ptra);//описание переменных
		SetDisjunct(st_statpart, followers, ptra);//pointc,endoffile,eolint + beginsy, eolint
		ProcFuncPart(ptra);//описание процедур и функций

		StartMainGeneration();

		BeginStatement(followers);//тело программы (или составной оператор)
		if (!Belong(Symbol, followers))
		{
			Error(6, token); /* заперещённый символ */
			Skipto1(followers);
		}
	}
}
//-----------------------------------------БЛОК (КОНЕЦ)--------------------------------------------------------//



//------------------------------------РАЗДЕЛ КОНСТАНТ(НАЧАЛО)---------------------------------------------------//
/* Анализ конструкции "константа" (число, символ) */
void Constant()
{

}
/* Анализ конструкции "целое без знака" */
void Un_Int()
{
	Accept(integerc);
}
/* Анализ конструкции "вещественное без знака" */
void Un_Real()
{
	Accept(realc);
}
/* Анализ конструкции "символ" */
void Const_Char()
{
	Accept(charc);
}
//------------------------------------РАЗДЕЛ КОНСТАНТ(КОНЕЦ)----------------------------------------------------//



//------------------------------------РАЗДЕЛ ТИПОВ(НАЧАЛО)------------------------------------------------------//

TYPEREC *typentry;						/* указатель на дескриптор типа */
NODE *identry;							/* указатель на Таблицу Имен вспомогательная */
struct reestrconsts *constlist = NULL;	/* указатель на элемен тсписка констант в дескрипторе перечисляемого типа */

////для ограниченного типа
//int numb1 = 0; //порядок const  в перечислимом типе. номер CONST..
//struct reestrconsts *head; // вспомогательная штучка для поиска const (ее номера)
//int numb2 = 0; //порядок const  в перечислимом типе номер ..CONST

/* Анализ конструкции "раздел типов" */
void TypePart(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
	if (!Belong(Symbol, st_typepart))
	{
		Error(1, token);
		Skipto2(st_typepart, followers);
	}
	if (Symbol == typesy)
	{
		NextSym();
		SetDisjunct(af_sameparam, followers, ptra);
		while (!Belong(Symbol, followers)) {
			One_Type(ptra);
			Accept(semicolonc);
		}
		if (!Belong(Symbol, followers))
		{
			Error(2, token); /* должно идти имя */
			Skipto1(followers);
		}
	}
}
/* Анализ конструкции "определение одного типа" */
void One_Type(unsigned *followers)
{
	TYPEREC *type1 = NULL;
	NODE *typ_node = NULL;
	if (!Belong(Symbol, idstarters))
	{
		Error(2, token);
		Skipto2(idstarters, followers);
	}
	if (Symbol == ident) {
		SearchInTable(name);
		Accept(ident);
		typ_node = NewIdent(hashresult, addrname, TYPES);//создаём новый тип
		Accept(equalc);
	}
	if (!Belong(Symbol, idstarters))
	{
		Skipto2(idstarters, followers);
	}
	typ_node->idtype = Type(followers);//делаем ссылку на один из базовых
}

//------------------ОПИСАНИЕ ТИПОВ ( ПЕРЕЧИСЛИМЫЙ, ОГРАНИЧЕННЫЙ,СТАНДАРТНЫЙ, СОСТАВНОЙ... )--------------------//
/* Анализ конструкции "тип" */
TYPEREC *Type(unsigned *followers)
{
	TYPEREC *tag = NULL;
	if (!Belong(Symbol, st_typ))
	{
		Error(10, token);
		Skipto2(st_typ, followers);
	}
	if (Belong(Symbol, st_typ))
	{
		if (Symbol == integerc || Symbol == plusc || Symbol == minusc || Symbol == charc || Symbol == leftparc || Symbol == ident)
			tag = SimpleType(followers);//простой тип
		if (!Belong(Symbol, followers))
		{
			Error(6, token);
			Skipto1(followers);
		}
	}
	return tag;
}
/* Анализ конструкции "простой тип" */
TYPEREC *SimpleType(unsigned *followers)
{
	TYPEREC *TypeTag = NULL;
	typentry = (TYPEREC *)malloc(sizeof(TYPEREC));
	switch (Symbol)
	{
	case leftparc: /* перечислимый тип */
		typentry = NewType(ENUMS); // "каркас" дескриптора типа
		TypeTag = PerechislType(followers);
		break;
	case ident: /* стандартный тип */
		//typentry = NewType(SCALARS); // "каркас" дескриптора типа
		TypeTag = NameType();
		break;
	default:/* ограниченный тип */
		if (Symbol == integerc || Symbol == plusc || Symbol == minusc || Symbol == charc)
		{
			typentry = NewType(LIMITEDS); // "каркас" дескриптора типа
			TypeTag = LimitedType(followers);
		}
	}
	return TypeTag;
}
/* стандартный тип или Анализ конструкции имя типа */
TYPEREC *NameType()
{
	SearchInTable(name);
	identry = SearchIdent(hashresult, addrname, set_TYPES);
	if (identry == NULL)
		identry = NewIdent(hashresult, addrname, TYPES);
	typentry = identry->idtype;//если не нашли идент (тип такой), то возвращаем нул
	Accept(ident);
	//if (Symbol == twopointsc)
	//{
	//	NextSym();
	//	Accept(ident);
	//}
	return typentry;
}
/* Анализ конструкции перечислимый тип */
TYPEREC *PerechislType(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
	int flag = 0;

	NextSym();
	if (!Belong(Symbol, idstarters))
	{
		Error(2, token);
		Skipto2(idstarters, followers);
	}
	if (Symbol == ident)
	{
		SearchInTable(name);
		identry = SearchIdent(hashresult, addrname, set_CONSTS);
		if (identry == NULL)
			identry = NewIdent(hashresult, addrname, CONSTS);				// создаем вершину в Таблице Идентиф для константы перечислимого типа
		identry->idtype = typentry;											//запоминаем указатель на дескриптор типа константы в Таблице Идентиф
		(identry->casenode).constvalue.enumval = addrname;					// запоминаем имя константы в Таблице Идентиф
		(typentry->casetype).firstconst = constlist = newconst(addrname);	// создаем первый елемент списка констант в дескрипторе перечисляемого типа
		Accept(ident);
		SetDisjunct(rpar, idstarters, ptra);
		while (Symbol == commac)
		{
			Accept(commac);
			if (Symbol != ident) {
				Accept(ident);
				Skipto2(ptra, followers);
			}
			if (Symbol == ident)
			{
				SearchInTable(name);
				identry = SearchIdent(hashresult, addrname, set_CONSTS);
				if (identry == NULL)
					identry = NewIdent(hashresult, addrname, CONSTS);	// создаем вершину в Таблице Идентиф для константы перечислимого типа
				identry->idtype = typentry;								//запоминаем указатель на дескриптор типа константы в Таблице Имен
				(identry->casenode).constvalue.enumval = addrname;		// запоминаем имя константы в Таблице Идент
				//создаем следующий элемент списка констант в дескрипторе перечисляемого типа
				constlist->next = newconst(addrname);
				constlist = constlist->next;
				Accept(ident);
			}
		}
		Accept(rightparc);
	}
	return typentry;
}
/* Анализ конструкции ограниченный тип */
TYPEREC *LimitedType(unsigned *followers)
{
	if (Symbol == plusc || Symbol == minusc)
		NextSym();
	switch (Symbol)
	{
	case integerc:
		typentry->casetype.basetype = inttype;
		(typentry->casetype).limtype.diapason.intptrs.min = intConstant;
		Un_Int();
		break;
	case realc:
		Un_Real();
		break;
	case charc:
		typentry->casetype.basetype = chartype;
		(typentry->casetype).limtype.diapason.enptrs.min = (char*)charConstant;
		Const_Char();
		break;
	}
	if (!Belong(Symbol, m_twopoint))
	{
		Error(74, token);
		Skipto2(m_twopoint, followers);
	}
	if (Belong(Symbol, m_twopoint))
	{
		Accept(twopointsc);
		if (Symbol == plusc || Symbol == minusc)
			NextSym();

		switch (Symbol)
		{
		case integerc:
			if (typentry->casetype.basetype == inttype)
			{
				(typentry->casetype).limtype.diapason.intptrs.max = intConstant;
				//проверка на границы
				if ((typentry->casetype).limtype.diapason.intptrs.min > (typentry->casetype).limtype.diapason.intptrs.max)
					Error(102, token);
			}
			else
				Error(182, token);
			Un_Int();
			break;
		case charc:
			if (typentry->casetype.basetype == chartype)
			{
				(typentry->casetype).limtype.diapason.enptrs.max = (char*)charConstant;
				//проверка на границы
				if ((typentry->casetype).limtype.diapason.enptrs.min > (typentry->casetype).limtype.diapason.enptrs.max)
					Error(102, token);
			}
			else
				Error(182, token);
			Const_Char();
			break;
		}
		return typentry;
	}
	else return NULL;
}

//--------------------------------------------РАЗДЕЛ ТИПОВ( КОНЕЦ )---------------------------------------------//



//------------------------------------------РАЗДЕЛ ПЕРЕМЕННЫХ(НАЧАЛО)-------------------------------------------//

/*создание элементов вспомогательного списка;
	в момент вызова этой функции информация о типеидентификатора еще не прочитанная анализатором*/
void NewVariable()
{
	struct listrec *listentry; //указатель на текущий элемент вспомогательного списка
	if (Symbol == ident)
	{
		kol++;
		listentry = (struct listrec*)malloc(sizeof(struct listrec)); // выделение памяти под новую константу данного типа
		listentry->id_r = NewIdent(hashresult, addrname, VARS); // занесение имени в список
		listentry->next = varlist; // присоединение к стеку
		varlist = listentry;
	}
}
/*присваивает значение полю idtype для вершин, адреса которых хранятся во всмопогательном стеке*/
void AddAttributes()
{
	struct listrec *listentry; // указатель на вершину вспомогательного списка
	struct listrec *oldentry; // указатель на вершину, предшествующую текущей во вспомогательном списке
	listentry = varlist;
	while (listentry != NULL)
	{
		if (listentry->id_r != NULL)//если переменная не пустая, т.е не была описана до этого
			listentry->id_r->idtype = vartype; // внешняя переменная vartype содержит адрес дескриптора для однотипных переменных
		oldentry = listentry;
		listentry = listentry->next;
		free((void*)oldentry);
	}
}
/* Анализ конструкции "раздел переменных" */
void VarPart(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
	if (!Belong(Symbol, st_varpart))
	{
		Error(18, token);
		Skipto2(st_varpart, followers);
	}
	if (Symbol == varsy)
	{
		StartVarGeneration();
		SetDisjunct(af_sameparam, followers, ptra);
		Accept(varsy);
		while (!Belong(Symbol, followers))
		{
			VarDeclaration(ptra);
			Accept(semicolonc);
		}
		if (!Belong(Symbol, followers))
		{
			Error(2, token); /* должно идти имя */
			Skipto1(followers);
		}
	}
}
/* Анализ конструкции "описание однотипных переменных" */
void VarDeclaration(unsigned *followers)
{
	//unsigned ptra[SET_SIZE];
	int flag = 0;
	if (!Belong(Symbol, idstarters))
	{
		Error(2, token);
		Skipto2(idstarters, followers);
	}
	if (Symbol == ident) {
		kol = 0;
		varlist = NULL;
		SearchInTable(name);
		NewVariable();
		Accept(ident);
		while (Symbol == commac)
		{
			Accept(commac);
			SearchInTable(name);
			NewVariable();
			Accept(ident);
		}
		Accept(colonc);
		vartype = Type(followers);
		AddAttributes();
		if (!Belong(Symbol, followers))
		{
			Error(6, token);
			Skipto1(followers);
		}
	}
}

//------------------------------------------РАЗДЕЛ ПЕРЕМЕННЫХ(КОНЕЦ)--------------------------------------------//



//---------------------------------------РАЗДЕЛ ПРОЦЕДУР И ФУНКЦИЙ(НАЧАЛО)--------------------------------------//
NODE *myfunc; // указатель на Таблицу Имен вспомогательная
struct idparam * paramlist;

void NewParam(unsigned typeper)
{
	//struct idparam * head=(struct idparam *)malloc(sizeof(struct idparam *));
	while (kol != 0)
	{
		paramlist->typeparam = vartype;
		paramlist->mettransf = typeper;
		kol--;
		if (Symbol != rightparc || kol != 0)
			paramlist->linkparam = (struct idparam *)malloc(sizeof(struct idparam *));
		else
			paramlist->linkparam = NULL;
		paramlist = paramlist->linkparam;

	}
}
/* Анализ конструкции "раздел процедур и фукций" */
void ProcFuncPart(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
	if (!Belong(Symbol, st_procfuncpart))
	{
		Error(18, token);
		Skipto2(st_procfuncpart, followers);
	}
	if (Symbol == functionsy || Symbol == proceduresy)
	{
		SetDisjunct(m_scolon, followers, ptra);
		while (Symbol == functionsy || Symbol == proceduresy)
		{
			DeclarationProcFun(ptra);
			Accept(semicolonc);
		}
		if (!Belong(Symbol, followers))
		{
			Error(6, token);
			Skipto1(followers);
		}
	}
}
/* Анализ конструкции "описание процедуры или фукции" */
void DeclarationProcFun(unsigned *followers)
{
	switch (Symbol)
	{
	case proceduresy:
		DeclarationProc(followers);
		break;
		//case functionsy:
		//	DeclarationFun(followers);
		//	break;
	}
	if (!Belong(Symbol, followers))
	{
		Error(6, token);
		Skipto1(followers);
	}
}
/* Анализ конструкции "описание процедуры" */
void DeclarationProc(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
	SetDisjunct(blockfol, af_headfunproc, ptra);
	HeaderProc(ptra);
	Block(af_blockprocfunc);
	Close_Scope();
	level--;
}
/* Анализ конструкции "заголовок процедуры" */
void HeaderProc(unsigned *followers)
{
	unsigned ptra[SET_SIZE], dop[SET_SIZE];
	Accept(proceduresy);
	if (!Belong(Symbol, idstarters))
	{
		Error(2, token);
		Skipto2(idstarters, followers);
	}
	if (Symbol == ident)
	{
		SearchInTable(name);
		myfunc = (NODE *)malloc(sizeof(NODE*));
		myfunc = NewIdent(hashresult, addrname, FUNCS);
		myfunc->casenode.proc.level = level + 1;
		(myfunc->casenode.proc.param) = paramlist = (struct idparam *)malloc(sizeof(struct idparam *));
		//paramlist=(myfunc->casenode.proc.param);
		Open_Scope();
		level++;
		Accept(ident);
		if (Symbol == leftparc)
		{
			SetDisjunct(rpar, followers, ptra);
			SetDisjunct(af_sameparam, followers, dop);
			do
			{
				NextSym();
				FormalParameters(dop, ptra);
			} while (Symbol == semicolonc);
			Accept(rightparc);
			Accept(semicolonc);
		}
		else
			Accept(semicolonc);
		if (!Belong(Symbol, followers))
		{
			Error(6, token);
			Skipto1(followers);
		}
	}
}
/* Анализ конструкции "раздел фомальных параметорв" */
void FormalParameters(unsigned *followers, unsigned *fol)
{
	if (!Belong(Symbol, m_fpar))
	{
		Error(2, token);//
		Skipto2(m_fpar, followers);
	}
	if (Belong(Symbol, m_fpar))
	{
		switch (Symbol)
		{
		case ident:
			VarDeclaration(followers);
			NewParam(paramvar);
			break;
		case varsy:
			NextSym();
			VarDeclaration(followers);
			break;
		case functionsy:
			NextSym();
			VarDeclaration(followers);
			NewParam(paramvar);
			break;
		case proceduresy:
			NextSym();
			Accept(ident);
			while (Symbol == commac)
				Accept(ident);
			break;
		}
		if (!Belong(Symbol, followers))
		{
			Error(6, token);
			Skipto1(followers);
		}
	}
}
//---------------------------------------РАЗДЕЛ ПРОЦЕДУР И ФУНКЦИЙ(КОНЕЦ)---------------------------------------//



//--------------------------------------------ВЫРАЖЕНИЕ ( НАЧАЛО )----------------------------------------------//

//**************************************************************************************************************//
//-------------------------------------ФУНКЦИИ ДЛЯ СЕМАНТИКИ ( НАЧАЛО )-----------------------------------------//
//для simpleexpression
int Right_Sign(TYPEREC * type)
{//знак +/- может быть записан перед оператором, если он является числом(целым вещественным) или ошибка в операторе
	if (type == NULL || type == inttype || type == realtype
		|| (type->typecode == LIMITEDS && (type->casetype.limtype.basetype == inttype)))
		return (1);
	else
	{
		Error(211, token);//недопустимые типы операндов операции +/-
		return (0);
	}
}
TYPEREC *Comparing(TYPEREC * type1, TYPEREC * type2, unsigned oper)
{// проверка соответсвия типов в составном выражении
	if (type1 == NULL || type2 == NULL)
		return NULL;
	if (type1 == type2)
	{
		if (type1 == texttype)
		{
			Error(108, token);
		}
		return booltype;
	}
	else
		if (type1 == inttype || type1 == realtype || (type1->typecode == LIMITEDS && (type1->casetype.limtype.basetype == inttype)))
			if (type2 == inttype || type2 == realtype || (type2->typecode == LIMITEDS && (type2->casetype.limtype.basetype == inttype)))
				return booltype;
			else
			{
				Error(186, token);//145 107
				return NULL;//какой тип возвращать
			}
	// later/*  '<'   */, laterequal/*  '<='   */, greater/*  '>'   */, greaterequal/*  '>='   */,
	//equal/*  '='   */, latergreater/*  '<>'   */
}
TYPEREC *TestAdd(TYPEREC * type1, TYPEREC * type2, unsigned oper)
{//ПРОВЕРКА соответствия типов в слагаемом
	//plus, minus, orsy,
	if (type1 == NULL || type2 == NULL)
		return NULL;
	switch (oper)
	{
	case plusc:
	case minusc:
		if (type1 == type2)
		{
			if (type1 == texttype || type1 == chartype || type1->typecode == ENUMS)
			{
				Error(145, token);
				return NULL;
			}
			return type1;
		}
		else
		{
			if (type1 == realtype || type2 == realtype)
				if (type1 == inttype || type2 == inttype
					|| (type1->typecode == LIMITEDS && type1->casetype.limtype.basetype == inttype)
					|| (type2->typecode == LIMITEDS && type2->casetype.limtype.basetype == inttype))
					return realtype;
				else
				{
					Error(145, token);
					return NULL;
				}
			else
				if (type1 == inttype || type2 == inttype
					|| (type1->typecode == LIMITEDS && type1->casetype.limtype.basetype == inttype)
					|| (type2->typecode == LIMITEDS && type2->casetype.limtype.basetype == inttype))
					if (type1 == realtype || type2 == realtype)
						return realtype;
					else
					{
						Error(145, token);
						return NULL;
					}
				else
				{
					Error(145, token);
					return NULL;
				}
		}
		break;
	case orsy:
		if (type1 == type2 && type1 == booltype)
		{
			return type1;
		}
		else
		{
			Error(145, token);
			return NULL;
		}
		break;
	}
}
TYPEREC *Logical(TYPEREC *type)
{
	if (type == booltype || (type->casetype.limtype.basetype == booltype))
		return booltype;
	else
	{
		Error(145, token);
		return NULL;// может NULL?
	}
}
TYPEREC *TestMult(TYPEREC * type1, TYPEREC * type2, unsigned oper)
{//проверка соответсвия типов в множителе
	//star, slash, divsy, modsy, andsy,
	if (type1 == NULL || type2 == NULL)
		return NULL;
	switch (oper)
	{
	case divsy:
	case modsy:
		if (type1 == type2 && type1 == inttype)
			return type1;
		else
		{
			Error(145, token);
			return NULL;
		}
		break;
	case starc:
		switch (type1->typecode)
		{
		case SCALARS:
			if (type1 == inttype)
				if (type1 == type2 || (type2->typecode == LIMITEDS && type2->casetype.limtype.basetype == inttype))
					return type1;
				else
					if (type2 == realtype)
						return realtype;
					else
					{
						Error(145, token);
						return NULL;
					}
			else
				if (type1 == realtype)
					if (type1 == type2 || type2 == inttype
						|| (type2->typecode == LIMITEDS && type2->casetype.limtype.basetype == inttype))
						return realtype;
					else
					{
						Error(145, token);
						return NULL;
					}

				else
				{
					Error(145, token);
					return NULL;
				}
			break;
		case LIMITEDS:
			if (type1->casetype.limtype.basetype == inttype)
			{
				if (type2 == inttype || (type2->typecode == LIMITEDS && type2->casetype.limtype.basetype == inttype))
					return inttype;
				else
					if (type2 == realtype)
						return realtype;
					else
					{
						Error(145, token);
						return NULL;
					}

			}
			else
			{
				Error(145, token);
				return NULL;
			}
			break;
		default:
			Error(145, token);
			return NULL;
		}
		break;
	case andsy:
		if (type1 == type2 && type1 == booltype)
			return type1;
		else
		{
			Error(145, token);
			return NULL;
		}
		break;
	case slashc:
		if (type1 == realtype || type2 == realtype)
			if (type1 == inttype || type2 == inttype
				|| type1->casetype.limtype.basetype == inttype
				|| type2->casetype.limtype.basetype == inttype)
				return realtype;
			else
			{
				Error(145, token);
				return NULL;
			}
		else
			if (type1 == inttype || type2 == inttype
				|| type1->casetype.limtype.basetype == inttype
				|| type2->casetype.limtype.basetype == inttype)
				if (type1 == realtype || type2 == realtype)
					return realtype;
				else
				{
					Error(145, token);
					return NULL;
				}
			else
			{
				Error(145, token);
				return NULL;
			}
		break;
	}
}
//--------------------------------------ФУНКЦИИ ДЛЯ СЕМАНТИКИ ( КОНЕЦ )-----------------------------------------//
//**************************************************************************************************************//

/* Анализ конструкции "выражение" */
TYPEREC *Expression(unsigned *followers)
{
	unsigned ptra[SET_SIZE],
		operation;
	TYPEREC *ex1type = NULL,
		*ex2type;
	if (!Belong(Symbol, st_express))
	{
		Error(42, token);
		Skipto2(st_express, followers);
	}
	if (Belong(Symbol, st_express))
	{
		SetDisjunct(op_rel, followers, ptra);
		ex1type = SimpleExpression(ptra);
		if (Symbol == equalc || Symbol == laterc || Symbol == greaterc || Symbol == latergreaterc || Symbol == laterequalc || Symbol == greaterequalc)
		{
			//flag = 1;
			operation = Symbol;
			GeneralGeneration(Symbol);
			NextSym();
			ex2type = SimpleExpression(followers);
			//проверка совместимости типов простых выражений
			ex1type = Comparing(ex1type, ex2type, operation);
		}
		if (!Belong(Symbol, followers))
		{
			Error(6, token);
			Skipto1(followers);
		}
		return ex1type;
	}
	else return NULL;
}
/* Анализ конструкции "простое выражение" */
TYPEREC *SimpleExpression(unsigned *followers)
{
	unsigned ptra[SET_SIZE],
		sign = 0,
		operation;
	TYPEREC *ex1type = NULL,
		*ex2type;
	if (!Belong(Symbol, st_express))
	{
		Error(42, token);
		Skipto2(st_express, followers);
	}
	if (Belong(Symbol, st_express))
	{
		SetDisjunct(op_add, followers, ptra);
		if (Belong(Symbol, op_add))
		{
			//flag = 1;
			sign = 1;
			GeneralGeneration(Symbol);
			NextSym();
		}
		ex1type = Term(ptra);
		if (sign) Right_Sign(ex1type);
		while ((Symbol == plusc || Symbol == minusc || Symbol == orsy) && !feof(file_program))
		{
			//flag = 1;
			operation = Symbol;
			GeneralGeneration(Symbol);
			NextSym();
			ex2type = Term(ptra);
			ex1type = TestAdd(ex1type, ex2type, operation);
		}
		if (!Belong(Symbol, followers))
		{
			Error(6, token);
			Skipto1(followers);
		}
		return ex1type;
	}
	else return NULL;
}
/* Анализ конструкции "слагаемое" */
TYPEREC *Term(unsigned *followers)
{
	unsigned ptra[SET_SIZE],
		operation;
	TYPEREC *ex1type = NULL,
		*ex2type;
	if (!Belong(Symbol, st_termfact))
	{
		Error(42, token);
		Skipto2(st_termfact, followers);
	}
	if (Belong(Symbol, st_termfact))
	{
		SetDisjunct(followers, st_express, ptra);//начальные символы выражения с конечными 
		//+операиции мультипликат
		SetDisjunct(ptra, op_mult, ptra);
		ex1type = Factor(ptra);
		while (Belong(Symbol, op_mult) && !feof(file_program))
		{
			//flag = 1;
			operation = Symbol;
			GeneralGeneration(Symbol);
			NextSym();
			ex2type = Factor(ptra);
			//проверка совместимости типов множителей
			//ex1type = TestMult(ex1type, ex2type, operation);
		}
		if (!Belong(Symbol, followers))
		{
			Error(6, token);
			Skipto1(followers);
		}
		return ex1type;
	}
	else return NULL;
}
/* Анализ конструкции "множитель" */
TYPEREC *Factor(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
	TYPEREC *extype = NULL;
	NODE *node = NULL;
	if (!Belong(Symbol, st_termfact))
	{
		Error(42, token);
		Skipto2(st_termfact, followers);
	}
	if (Belong(Symbol, st_termfact))
	{
		switch (Symbol)
		{
		case leftparc://если в скобках т.е составной множитель
			myconst = 0;
			//flag = 1;
			GeneralGeneration(Symbol);
			NextSym();
			SetDisjunct(followers, af_3const1, ptra);//af_3const1 = twopoints,comma,rbracket
			SetDisjunct(ptra, rpar, ptra);
			extype = Expression(ptra);
			if (Symbol == rightparc)
				Accept(rightparc);
			else
				Skipto1(m_scolon);
			break;
		case notsy:// отрицание перед всем множителем
			myconst = 0;
			//flag = 1;
			NextSym();
			extype = Factor(followers);
			extype = Logical(extype);
			break;
		case ident://имя переменной 
			myconst = 0;
			SearchInTable(name);
			node = SearchIdent(hashresult, addrname, set_VARCONFUNCS);
			Accept(ident);
			if (node != NULL)
			{
				switch (node->usingclass)
				{
				case CONSTS:
					//flag = 1;
				case VARS:
					extype = node->idtype;
					break;
				default:
				{
					//flag = 1;
					extype = NULL;
				}
				}
			}
			else extype = NULL;
			/*if (Symbol == leftparc)
			{
				SetDisjunct(followers, rpar, ptra);
				GeneralGeneration(Symbol);
				NextSym();
				NameFunc(ptra);
				Accept(rightparc);
			}*/
			break;
		case integerc://целое число
			myconst = 1;
			//flag = 1;
			Accept(integerc);
			extype = inttype;
			break;
		case realc://вещественное число
			myconst = 0;
			//flag = 1;
			Accept(realc);
			extype = realtype;
			break;
		case charc://символ
			myconst = 0;
			//flag = 1;
			Accept(charc);
			extype = chartype;
			break;
		}
		if (!Belong(Symbol, followers))
		{
			Error(6, token);
			Skipto1(followers);
		}
		return extype;
	}
	else return NULL;
}
/* Анализ конструкции фактические параметры функции ( и процедуры) */
void NameFunc(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
	//Accept(leftparc);
	if (!Belong(Symbol, st_express))
	{
		Error(42, token);
		Skipto2(st_express, followers);
	}
	if (Belong(Symbol, st_express))
	{
		Accept(leftparc);
		SetDisjunct(followers, m_comma, ptra);
		Expression(ptra);
		while (Symbol == commac) {
			Accept(commac);
			Expression(ptra);
		}
		if (!Belong(Symbol, followers))
		{
			Error(6, token);
			Skipto1(followers);
		}
	}
}
//--------------------------------------------ВЫРАЖЕНИЕ ( КОНЕЦ )-----------------------------------------------//



//-----------------------------------------ОПИСАНИЕ ОПЕРАТОРОВ (НАЧАЛО)-----------------------------------------//

//===================================================Cемантика==================================================//
bool Сompatible(TYPEREC* type1, TYPEREC * type2)
{//проверка соответсвия типов в операторе присваивания
	bool flag = true;
	if (type1 != NULL && type2 != NULL)
	{
		if (type1 != type2)
		{
			switch (type1->typecode)
			{
			case SCALARS:
				if (type1 == inttype)
				{
					if (type2->typecode != LIMITEDS)
					{
						Error(222, token);
						flag = false;
					}
					else
						if (type2->casetype.limtype.basetype != inttype)
						{
							Error(222, token);
							flag = false;
						}
				}
				else
					if (type1 != realtype)
						Error(222, token);
					else
						if (type2 == inttype || type2 == realtype)
							;
						else
							if (type2->typecode == LIMITEDS)
							{
								if (type2->casetype.basetype != inttype)
								{
									Error(222, token);
									flag = false;
								}
							}
							else
							{
								Error(144, token);
								flag = false;
							}
				break;
			case LIMITEDS:
				if (type1->casetype.limtype.basetype != type2)
				{
					if (type2->typecode != LIMITEDS)
						Error(221, token);
					else
						if (type1->casetype.limtype.basetype != type2->casetype.limtype.basetype)
							Error(220, token);
						else
							if (type1->casetype.limtype.basetype->typecode == ENUMS)
							{
								if (type1->casetype.limtype.diapason.enptrs.max < type2->casetype.limtype.diapason.enptrs.min ||
									type2->casetype.limtype.diapason.enptrs.max < type1->casetype.limtype.diapason.enptrs.min)
									Error(222, token);//написать функцию сравнения констант
							}
							else
								if (type1->casetype.limtype.diapason.intptrs.max < type2->casetype.limtype.diapason.intptrs.min ||
									type2->casetype.limtype.diapason.intptrs.max < type1->casetype.limtype.diapason.intptrs.min)
									Error(222, token);
				}
				else
					if (myconst)
					{//сравнение костанты с минимум и максимумом огр типа
						if (type1->casetype.limtype.basetype == inttype)
						{
							if (type1->casetype.limtype.diapason.intptrs.max<intConstant || type1->casetype.limtype.diapason.intptrs.min>intConstant)
								Error(222, token);
						}
						else
							if (type1->casetype.limtype.diapason.intptrs.max<charConstant || type1->casetype.limtype.diapason.intptrs.min>charConstant)
								Error(222, token);
					}
				break;
			case ENUMS:
				if (type2->typecode != LIMITEDS)
					Error(145, token);
				break;
			default: Error(186, token);
			}
		}
	}
	return flag;
}
//=============================================Конец Cемантики==================================================//

/* Анализ конструкции "оператор" */
void Statement(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
	if (!Belong(Symbol, st_statement))
	{
		Error(113, token);
		Skipto2(st_statement, followers);
	}
	if (Belong(Symbol, st_statement))//integerc,endsy,elsesy,untilsy,ident,beginsy,ifsy,whilesy,repeatsy,forsy,casesy,withsy,semicolon,gotosy,eolint
	{
		SetDisjunct(followers, st_statement, ptra);

		switch (Symbol)
		{
		case whilesy:
			//Анализ конструкции "цикл с предусловием"
			SetDisjunct(ptra, af_whilefor, ptra);
			WhileStatement(ptra);
			break;
		case repeatsy:
			//Анализ конструкции "цикл с постусловием"
			SetDisjunct(ptra, af_repeat, ptra);
			RepeatStatement(ptra);
			break;
		case forsy:
			//Анализ конструкции "цикл с параметром"
			SetDisjunct(ptra, af_whilefor, ptra);
			SetDisjunct(ptra, af_for1, ptra);
			ForStatement(ptra);
			break;
		case ifsy:
			//Анализ конструкции "условный оператор"
			SetDisjunct(ptra, af_iftrue, ptra);
			IfStatement(ptra);
			break;
		case beginsy:
			BeginStatement(ptra);
			break;
		case semicolonc:
			//Accept(semicolonc);
			break;
		case ident:
			GivingStatement(followers);
			//Accept(ident);
			//switch (Symbol)
			//{
			//case leftparc:
			//	//Анализ конструкции "вызов процедуры"
			//	SetDisjunct(followers, rpar, ptra); // rightparc,eolint 
			//	CallProc(ptra);
			//	Accept(rightparc);
			//	break;
			//case  assignc:
			//	//Анализ конструкции ""
			//	GivingStatement(followers);
			//	break;
			//case  semicolonc:
			//	break;
			//default:
			//	if (!Belong(Symbol, af_assignment))
			//	{
			//		Error(6, token);
			//		Skipto1(af_assignment);
			//	}
			//	GivingStatement(followers);
			//	break;
			//}
			break;
		case endsy:		break;
		}
		if (!Belong(Symbol, followers))
		{
			Error(6, token);
			Skipto1(followers);
		}
	}
}
/* Анализ конструкции "составной оператор" */
void BeginStatement(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
	if (!Belong(Symbol, st_statpart))//st_statpart содержит beginsy
	{
		Error(17, token);
		Skipto2(st_statpart, followers);
	}
	if (Belong(Symbol, st_statpart))//st_statpart содержит beginsy
	{
		Accept(beginsy);
		SetDisjunct(followers, st_statement, ptra); //semicolon, endsy
		//SetDisjunct(followers, idstarters, ptra);
		while (Symbol != endsy && !feof(file_program)) {
			Statement(ptra);
			Accept(semicolonc);
		}
		Accept(endsy);
		if (!Belong(Symbol, followers))
		{
			Error(61, token);
			Skipto1(followers);
		}
	}
}
/* Анализ конструкции "цикл с предусловием" */
void WhileStatement(unsigned *followers)
{
	TYPEREC *exptype = NULL;
	Accept(whilesy);
	if (Belong(Symbol, st_express))
	{
		exptype = Expression(followers);
		if (!Сompatible(exptype, booltype))
			Error(135, token); /* тип операнда должен быть boolean */
		if (Symbol == dosy)
			Accept(dosy);
		else {
			Accept(dosy);
			Skipto1(st_startstatement);
		}
	}
	Statement(followers);
}
/* Анализ конструкции "условный оператор" */
void IfStatement(unsigned *followers)
{
	TYPEREC *exptype = NULL;
	Accept(ifsy);
	if (Belong(Symbol, st_express))
	{
		exptype = Expression(followers);
		if (!Сompatible(exptype, booltype))
			Error(135, token); /* тип операнда должен быть boolean */
		if (Symbol == thensy)
			NextSym();
		else {
			Accept(thensy);
			Skipto1(st_startstatement);
		}
	}
	Statement(followers);
	if (Symbol == elsesy)
	{
		Accept(elsesy);
		Statement(followers);
	}
}
/* Анализ конструкции "цикл с постусловием" */
void RepeatStatement(unsigned *followers)
{
	TYPEREC *exptype = NULL;
	Accept(repeatsy);
	while (Symbol != untilsy && !feof(file_program))
	{
		Statement(followers);
		Accept(semicolonc);
	}
	Accept(untilsy);
	exptype = Expression(followers);
	if (!Сompatible(exptype, booltype))
		Error(135, token); /* тип операнда должен быть boolean */
}
/* Анализ конструкции "цикл с параметром" */
void ForStatement(unsigned *followers)
{
	unsigned ptra[SET_SIZE],
		codevar;	/* код типа переменной */
	TYPEREC *exptype;	/* указатель на дескриптор типа выражения*/
	TYPEREC *vartype;	/* указатель на дискриптор типа параметра цикла*/
	Accept(forsy);
	if (Symbol == ident)
	{
		SearchInTable(name);
		identry = SearchIdent(hashresult, addrname, set_VARS);
		if (identry == NULL)
			identry = NewIdent(hashresult, addrname, VARS);
		vartype = identry->idtype;
		SaveParameter();
		if (vartype != NULL)
		{
			codevar = vartype->typecode;
			if (codevar != SCALARS && codevar != ENUMS && codevar != LIMITEDS || vartype == realtype)
				Error(143, token);
		}
		Accept(ident);
		if (Symbol == assignc)
		{
			Accept(assignc);
			if (Belong(Symbol, st_express))
			{
				exptype = Expression(followers);
				if (!Сompatible(vartype, exptype))
					Error(145, token);
				if (Belong(Symbol, af_for1))// to или downto
				{
					if (Symbol == tosy)
						Accept(tosy);
					else Accept(downtosy);
					if (Belong(Symbol, st_express))
					{
						exptype = Expression(followers);
						if (!Сompatible(vartype, exptype))
							Error(145, token);
						if (Symbol == dosy)
							Accept(dosy);
						else {
							Accept(dosy);
							Skipto1(st_startstatement);
						}
					}
					else
					{
						Error(42, token);
						Skipto1(st_startstatement);
					}
				}
				else
				{
					Accept(tosy);
					Skipto1(st_startstatement);
				}
			}
			else {
				Error(42, token);
				Skipto1(st_startstatement);
			}
		}
		else {
			Accept(assignc);
			Skipto1(st_startstatement);
		}
	}
	else
	{
		Error(2, token);
		Skipto1(st_startstatement);
	}
	Statement(followers);
}
/* Инициализация анализа выражения */
void GivingStatement(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
	TYPEREC * ex1type = NULL,
		*ex2type = NULL;
	NODE * node;
	myconst = 0;
	SetDisjunct(af_assignment, followers, ptra);//af_assignment содержит assign
	SearchInTable(name);
	node = SearchIdent(hashresult, addrname, set_VARFUNPR);
	Accept(ident);
	if (node != NULL)
	{
		switch (node->usingclass)
		{
		case VARS:
			ex1type = node->idtype;
			break;
		case PROCS:
			break;
		default:
			ex1type = NULL;
		}
	}
	else ex1type = NULL;//StandartFunc(foolowers)
	switch (Symbol)
	{
	case leftparc:
		//Анализ конструкции "вызов процедуры"
		SetDisjunct(followers, rpar, ptra); // rightparc,eolint 
		CallProc(ptra);
		Accept(rightparc);
		break;
	case  semicolonc:
		break;
	default:
		if (!Belong(Symbol, af_assignment))
		{
			Error(6, token);
			Skipto1(af_assignment);
		}
		//Анализ конструкции ""
		Accept(assignc);
		SetDisjunct(followers, comp_stat, ptra);
		ex2type = Expression(ptra);
		Сompatible(ex1type, ex2type);
		if (!Belong(Symbol, followers))
		{
			Error(6, token);
			Skipto1(followers);
		}
		break;
	}
}
/*Инициализация анализа процедуры*/
void CallProc(unsigned *followers)
{
	NameFunc(followers);
}
//-----------------------------------------ОПИСАНИЕ ОПЕРАТОРОВ (КОНЕЦ)-----------------------------------------//

void Programme(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
	/*СОЗДАНИЕ ФИКТИВНОЙ ОБЛАСТИ ДЕЙСТВИЯ*/
	localscope = NULL;//изначально стек пуст (localscope - переменная, указывающая навершину стека областей действия)

	Open_Scope(); //создание элемента стека для фиктивной области действия
	//boolean
	level = 0;
	booltype = NewType(ENUMS); // создаем тип boolean, booltype - внешняя переменная содержит адрес дескриптора типа
	SearchInTable((char*)"FALSE"); // занесение в Таблицу Имен "false" I курс, разрешение коллизий, таблица имен. не помню)
	entry = NewIdent(hashresult, addrname, CONSTS); // NewIdent заполняет вершину и возвращает на нее ссылку, в случае повторения вершины возвращает NULL и выдает ошибку
	entry->idtype = booltype;
	//создание списка констант в дескриптора перечисляемого типа (в данном случае boolean)
	(booltype->casetype).firstconst = newconst(addrname);
	SearchInTable((char*)"TRUE"); // занесение в Таблицу Имен "true" I курс, разрешение коллизий, таблица имен. не помню)
	entry = NewIdent(hashresult, addrname, CONSTS); // NewIdent заполняет вершину и возвращает на нее ссылку, в случае повторения вершины возвращает NULL и выдает ошибку
	entry->idtype = booltype;
	(booltype->casetype).firstconst = newconst(addrname);

	//real
	realtype = NewType(SCALARS);
	//сhar
	chartype = NewType(SCALARS);
	//integer
	inttype = NewType(SCALARS);
	//files
	texttype = NewType(FILES);
	(texttype->casetype).basetype = chartype;

	//заносим в таблицу имен остальные стандартные идентификаторы
	SearchInTable((char*)"integer");
	entry = NewIdent(hashresult, addrname, TYPES);
	entry->idtype = inttype;
	SearchInTable((char*)"maxint");
	entry = NewIdent(hashresult, addrname, CONSTS);
	(entry->casenode).constvalue.intval = MAXCONST;
	entry->idtype = inttype;
	SearchInTable((char*)"boolean");
	entry = NewIdent(hashresult, addrname, TYPES);
	entry->idtype = booltype;
	SearchInTable((char*)"text");
	entry = NewIdent(hashresult, addrname, TYPES);
	entry->idtype = texttype;
	SearchInTable((char*)"real");
	entry = NewIdent(hashresult, addrname, TYPES);
	entry->idtype = realtype;
	SearchInTable((char*)"char");
	entry = NewIdent(hashresult, addrname, TYPES);
	entry->idtype = chartype;

	SearchInTable((char*)"ReadLn");
	entry = NewIdent(hashresult, addrname, PROCS);
	entry->idtype = NULL;
	SearchInTable((char*)"Read");
	entry = NewIdent(hashresult, addrname, PROCS);
	entry->idtype = NULL;
	SearchInTable((char*)"WriteLn");
	entry = NewIdent(hashresult, addrname, PROCS);
	entry->idtype = NULL;
	SearchInTable((char*)"Write");
	entry = NewIdent(hashresult, addrname, PROCS);
	entry->idtype = NULL;

	Open_Scope();//создание области стека для основной програмы
	level++;
	if (!Belong(Symbol, st_all))  //st_all содержит programsy
	{
		Error(3, token); //ожидается слово PROGRAM
		Skipto2(st_all, followers);
	}
	if (Belong(Symbol, st_all))
	{
		Accept(programsy);
		//SetDisjunct(st_startstatement, st_all, ptra);
		if (!Belong(Symbol, idstarters))  //idstarters содержит idetn
		{
			Error(2, token); //должно идти имя
			Skipto2(followers, st_startstatement);
		}
		Accept(ident);
		if (!Belong(Symbol, m_scolon))  //idstarters содержит semilicon
		{
			Error(14, token); //должно идти ;
			Skipto2(m_scolon, followers);
		}
		Accept(semicolonc);
		Block(blockfol);// pointc,endoffile,eolint
		Accept(pointc);
		Close_Scope(); Close_Scope();
		StopGeneration();
		while (!feof(file_program)) NextCh();
	}
}

/* Обработка определяющего вхождения идентификатора. Если идентификатор с заданным способом использования ранее
   был включен в дерево, то результат - NULL, иначе - ссылка на вершину дерева, соответствующую этому идентификатору */
NODE *NewIdent(unsigned hashfunc, char *addrname, int classused)
// hashfunc - результат хэш-функции
// addrname - ссылка на имя идентификатора
// classused - способ использования идентификатора
{
	// C помощью node1 ищем место для вставки нового узла
	// Цикл работает до тех пор, пока node1 не равно NULL
	NODE *node1 = localscope->firstlocal;
	// node2 - вспомогательный указатель.
	// Помогает связать новый узел с деревом
	NODE *node2 = NULL;
	// Переменная d показывает, какой из указателей node2
	// (right или left) указывает на node1
	// Если d=1, то правый указатель, если d=-1, то левый указатель,
	// если d=0, то узел с таким именем уже есть дереве
	int d = 2;
	/* Ищем место для вставки нового узла, или узел
	   с точно таким же именем*/
	while (node1 != NULL && d != 0)
	{
		/* Если мы нашли узел с точно таким же именем */
		if (node1->idname == addrname)
			d = 0;
		else
		{
			node2 = node1;
			/* Упорядочиваем узлы сначала по значению хэш-функции,
			   затем - по адресу строки с именем идентификатора*/
			if (hashfunc > node1->hashvalue)
			{
				node1 = node1->right;
				d = 1;
			}
			else
			{
				if (hashfunc == node1->hashvalue)
				{
					if (addrname > node1->idname)
					{
						node1 = node1->right;
						d = 1;
					}
					else
					{
						node1 = node1->left;
						d = -1;
					}
				}
				else
				{
					node1 = node1->left;
					d = -1;
				}
			}
		}
	}
	if (d == 0)	// Идентификатор в вершине дерева совпадает с тем,
				// который мы пытались вставить в дерево
	{
		// Ошибка - имя описано повторно
		Error(101, token);
		// Возвращаем NULL
		return NULL;
	}
	else	// Нашли место для вставки новой вершины
	{
		// Создаем новую вершину
		node1 = (NODE*)malloc(sizeof(NODE));
		node1->hashvalue = hashfunc;
		node1->idname = addrname;
		node1->usingclass = classused;
		node1->left = node1->right = NULL;
		if (node2 == NULL)	// Произошла остановка уже на первой вершине дерева
		{
			/* В дереве еще нет вершин, значит включаем
			   первую вершину в дерево */
			localscope->firstlocal = node1;
		}
		else // Дошли до конечных ветвей дерева
		{
			if (d == 1)	// Включаем вершину на правый указатель
				node2->right = node1;
			else	// Включаем вершину на левый указатель
				node2->left = node1;
		}
		return node1;
	}
}
/* Обработка прикладного вхождения идентификатора - поиск соответствующего ему определяющего вхождения.
   Результат функции - указатель на вершину дерева, соответсвующего данному идентификатору */
NODE *SearchIdent(unsigned hashfunc, char *addrname, unsigned *setofclass)
// hashfunc - значение hash-функции для идентификатора
// addrname - ссылка на имя идентификатора
// setofclass - множество возможных способов использования идентификатора
{
	int usingtype;						/* Счетчик */
	SCOPE* templocalscope = localscope; /* Для передвижения по стеку областей действия */
	NODE *node1;						/* C помощью node1 пытаемся найти нужную нам вершину */
	NODE *node2 = NULL;					/* Изменяемый указатель на вершину, под которой нужно будет вставить узлы с нужным нам типом использования,
										в случае отсутствия таких в таблице */
	NODE *topnode = NULL;	// Неизменяемый указатель на вершину, под которой 
					// нужно будет вставить узлы с нужным нам типом использования
					// (только для внешней области действия)
	int topnodeinited = 0;	// Вспомогательная логическая переменная; 
							// показывает, была проинициализирована topnode или нет
	int foundname = 0;	// Логическая переменная; показывает, нашли ли
						// мы идентификатор с нужным нам именем или нет

	while (templocalscope != NULL)	// До тех пор, пока мы не дошли 
									// до конца стека областей действия
	{
		// Переходим на вершину таблицы идентификаторов
		node1 = templocalscope->firstlocal;
		while (node1 != NULL)	// До тех пор, пока мы не дошли							
							// до конца таблицы идентификаторов
							// (если мы найдем нужный узел, 
							// то автоматически выйдем из функции)
		{
			if (node1->idname == addrname)	// Нашли узел с нужным нам именем
			{
				if (Belong(node1->usingclass, setofclass))
				{	// Нашли узел с нужным 
					// нам способом использования
					if (foundname&&localscope != templocalscope)	Error(333, token);
					return node1;	// Нашли нужный нам узел (возвращаем его)
				}
				else	// Найденный узел не может использоваться в нужном контексте
				{
					foundname = 1;
					node2 = node1;	// Устанавливаем указатель 
					node1 = node1->left;	// Продолжаем поиск слева
				};
			}
			else	// Не нашли узел с нужным нам именем (ищем дальше)
			{
				// Устанавливаем указатель node2 на текущую вершину
				node2 = node1;
				/*  Узлы упорядочены сначала по значению хэш-функции,
					затем - по адресу строки с именем идентификатора	*/
				if (hashfunc > node1->hashvalue)
					node1 = node1->right;
				else
				{
					if (hashfunc == node1->hashvalue)
					{
						if (addrname > node1->idname)
							node1 = node1->right;
						else
							node1 = node1->left;
					}
					else
						node1 = node1->left;
				};
			};
		};
		/*	Если мы пришли в эту точку кода, то это означает, что мы не нашли
			нужный нам узел в области действия templocalscape
			(иначе бы мы вышли из функции, возвратив нужный узел)	*/
			// Переходим в область действия, объемлющую только что рассматриваемую
		templocalscope = templocalscope->enclosingscope;
		if (!topnodeinited)	// Если мы еще не нашли вершину для вставки новой
							// (мы рассмотрели только что внешнюю область действия)
		{//Необходимо вставлять в рассматриваемую обл действия!!!
			// Запоминаем найденную вершину
			topnode = node2;
			// Отмечаем, что мы инициализировали topnode
			topnodeinited = 1;
		};
	};
	/*	Если мы пришли в эту точку кода, то это означает, что мы не нашли
		нужный нам узел вообще во всех областях действия
		(иначе бы мы вышли из функции, возвратив нужный узел)	*/
	if (foundname)	// Если мы нашли итендификатор с нужным именем
		Error(103, token);	// Ошибка: Имя не принадлежит соответствующему классу!
	else
	{
		Error(104, token);	// Ошибка: Имя не описано!
//else
// по всем способам использования

		// Если данный способ использования
		// предполагался в контексте программы
		// (его нужно внести в дерево)

		// Применяем node1 для создания нового узла
		node1 = (NODE*)malloc(sizeof(NODE));
		// Инициализируем поля нового узла
		node1->hashvalue = hashfunc;
		node1->idname = addrname;
		node1->idtype = NULL;	// базовый тип задаем нулевым
		node1->left = node1->right = NULL;
		// Вставляем созданную вершину в таблицу идентификаторов
		if (topnode == NULL)	// Если в данной области действия 
							// еще нет идентификаторов
		{
			// Созданная нами вершина становится верхней
			localscope->firstlocal = node1;
			topnode = node1;
		}
		else	// Идентификаторы в данной области действия есть 
		{
			if (hashfunc > topnode->hashvalue)
				topnode->right = node1;
			else
			{
				if (hashfunc == topnode->hashvalue)
				{
					if (addrname > topnode->idname)
						topnode->right = node1;
					else
						topnode->left = node1;
				}
				else
					topnode->left = node1;
			};
			topnode = node1;
		};
		//*set_VARCONFUNCS,	/* доп. способы использования - VARS, CONSTS, FUNCS */=
		//*set_VARS,	/* допустимый способ использования - VARS		*/=
		//*set_TYPES, 	/* допустимый способ использования - TYPES 		*/=
		//*set_CONSTS,	/* допустимый способ использования - CONSTS 		*/=
		//*set_TYCON,     /* допустимые способы использования - TYPES,CONSTS	*/=
		//*set_FUNCS,     /* допустимый способ использования - FUNCS              */
		//*set_PROCS,     /* допустимый способ использования - PROCS              */
		//*set_FUNPR,     /* допустимые способы использования - FUNCS,PROCS       */=
		//*set_VARFUNPR;  /* допустимые способы использования - VARS,FUNCS,PROCS  */=	
		if (setofclass == set_VARS) //однозначно
			topnode->usingclass = VARS;
		if (setofclass == set_TYPES)
			topnode->usingclass = TYPES;
		if (setofclass == set_VARCONFUNCS)
			topnode->usingclass = VARS;
		if (setofclass == set_TYCON)
			topnode->usingclass = TYPES;
		if (setofclass == set_CONSTS)
			topnode->usingclass = CONSTS;
		if (setofclass == set_VARFUNPR)
			topnode->usingclass = VARS;
		if (setofclass == set_FUNPR)
			topnode->usingclass = PROCS;
		if (setofclass == set_FUNCS)
			topnode->usingclass = FUNCS;
		if (setofclass == set_PROCS)
			topnode->usingclass = PROCS;
		// Возвращаем последний вставленный нами в дерево узел
	}
	return topnode;
}
/*СОЗДАНИЕ И ПРИСОЕДЕНЕНИЕ НОВОГО ДЕСКРИПТОРА ТИПА К ТАБЛИЦЕ(таблица по сути стек)  ТИПОВ*/
TYPEREC *NewType(int tcode/*код типа*/) {
	struct typerec *newd;									/* указатель на дескриптор типа */
	newd = (struct typerec*)malloc(sizeof(struct typerec)); /* выделение памяти под верщинку в списке дескрипторов типа */
	newd->typecode = tcode;
	newd->next = localscope->typechain;						/* действия по присоединения элемента стека к другим дескрипторам */
	switch (newd->typecode)									/* заполение дескриптора в зависимости от его типа */
	{
	case LIMITEDS:
		newd->casetype.basetype = NULL;
	case SCALARS:
		break;
	case ENUMS:
		newd->casetype.firstconst = NULL;
		break;
	case SETS:
	case FILES:
	case REFERENCES:
		newd->casetype.basetype = NULL;
		break;
	}
	localscope->typechain = newd; // присоединение созданого элемента элемента (а следовательно и всей таблицы типов) к области действия
	return(newd);
}
/*СОЗДАНИЕ ЭЛЕМЕНТА СТЕКА ДЛЯ ТЕКУЩЕЙ ОБЛАСТИ ДЕЙСТВИЯ*/
void Open_Scope() {
	SCOPE *newscope; // указатель на новый элемент стека
	newscope = (SCOPE*)malloc(sizeof(SCOPE));// выделение памяти под элемент стека
	newscope->firstlocal = NULL; // обнуление указателя на Таблицу Идентификаторов
	newscope->typechain = NULL;// обнуление указателя на Таблицу Типов области действия
	newscope->labelpointer = NULL; // обнулние указателя на Таблицу Меток области действия
	newscope->enclosingscope = localscope; //прикрепление текущего элемента к стеку. (на момент создания фиктивной области действия localscope=NULL)
	localscope = newscope; // изменение вершины стека
}
/*УДАЛЕНИЕ Таблицы Идентификаторов ТЕКУЩЕЙ ОБЛАСТИ ДЕЙСТВИЯ */
void Dispose_Ids(struct treenode *firstlocal) {//удаление 
	if (firstlocal != NULL) {
		Dispose_Ids(firstlocal->left);
		Dispose_Ids(firstlocal->right);
		free(firstlocal);
	}
}
/* УДАЛЕНИЕ Таблицы Типов ТЕКУЩЕЙ ОБЛАСТИ ДЕЙСТВИЯ*/
void Dispose_Types(TYPEREC *typechain) {
	TYPEREC* old;
	while (typechain != NULL) {
		old = typechain;
		typechain = typechain->next;
		free(old);
	}
}
/* УДАЛЕНИЕ ТЕКУЩЕЙ ОБЛАСТИ ДЕЙСТВИЯ */
void Close_Scope()
{
	SCOPE *oldscope;							/* указатель на удаляемый элмент стека */
	oldscope = localscope;
	localscope = localscope->enclosingscope;	/* переключение вершины стека на следующий элемент стека */
	//очистка полей удаляемого элемента
	Dispose_Ids(oldscope->firstlocal);			/* удаление ТИ */
	Dispose_Types(oldscope->typechain);			/* удаление ТТ */
	free(oldscope);								/* окончательное удаление вершины */
}

/*============================================= РАБОТА С ТАБЛИЦЕЙ ИМЁН =============================================*/
/*Хеш-функция для разрешения коллизий в таблице имён */
unsigned HashFunction(char *name)
{
	int i = 0, result = 0;
	while (name[i] != 0) result += name[i++] | i;
	return(abs(result % 997));
}
/* Поиск текущего имени в таблице имен */
void SearchInTable(char *name)/* значение текущего имени */
/* результаты - внешние переменные:
 addrname - адрес текущего имени в таблице имен,
 hashresult - результат hash-функции от текущего имени	*/
{
	int ind;	/* если ind равен 0, значит, текущее имя не совпадает с очередным именем в строке */
	hashresult = HashFunction(name);
	if (nametable[hashresult] == NULL) 	/* размещение первой строки     */
						/* для обработки коллизий 	*/
	{
		ptrstr = (str*)malloc(sizeof(str)); /* выделение участка памяти */
					   /* под структуру str            */
		nametable[hashresult] = ptrstr;
		(*ptrstr).firstfree = 0;
		(*ptrstr).next = NULL;
		WriteName(name);/* занесение текущего имени в таблицу имен      */
		return;			/* возвращаем адрес имени в таблице     */
	}
	else {
		ptrstr = nametable[hashresult];   /* коллизия; ищем имя в таблице */
		loccount = 0;
	search:
		do {/* установка начальных параметров для сравнения        */
			/* искомого имени с очередным именем в таблице:         */
			ind = 1;chcount = 0;
			addrname = &((*ptrstr).namelist[loccount]);
			/* (адрес устанавливается по первой литере имени)       */
			while ((addrname[chcount] != '\0') && (ind == 1)
				&& (lname >= chcount))
			{
				if (addrname[chcount] != name[chcount])
					ind = 0; /* имена не совпадают		*/
				else { loccount++;chcount++; }
			}
			if (ind == 1)
			{
				return;
			}		/* имя найдено переход к следующему имени в строке:				*/
			while ((*ptrstr).namelist[loccount++] != '\0');
		} while (loccount < (*ptrstr).firstfree); /* пока не встретим первую свободную ячейку
													(в случае, когда ее номер равен MAXNUM, все ячейки в строке */
		if ((*ptrstr).next != NULL) /* если существует следующая строка с тем же значением hash-функции    */
		{
			ptrstr = (*ptrstr).next; /* ...то переходим на нее       */
			loccount = 0;
			goto search;            /* ...и продолжаем поиск        */
		}
		else    /* поиск окончен, имя не найдено			*/
		{
			if (lname < MAXNUM - (*ptrstr).firstfree)
				/* (если наше имя вместится в остаток этой строки...)   */
			{
				WriteName(name);/* ...то мы его туда и запишем!*/
				return;
			}
			else {/* места для записи имени нет, выделяем новую строку */
				(*ptrstr).next = (str*)malloc(sizeof(str));
				ptrstr = (*ptrstr).next;
				(*ptrstr).next = NULL;
				(*ptrstr).firstfree = 0;
				WriteName(name);/* заносим имя в новую строку	*/
				return;
			}
		}
	}
}
/* Занесение текущего имени в таблицу имен */
void WriteName(char name[])	/* заносимое имя	*/
/* вызывается из SearchInTable(), все прочие аргументы берет из нее */
{
	addrname = &((*ptrstr).namelist[(*ptrstr).firstfree]);
	/* установили адрес заносимого имени 				*/
	for (chcount = 0, loccount = (*ptrstr).firstfree;name[chcount] != '\0';
		chcount++, loccount++)
	{
		(*ptrstr).namelist[loccount] = name[chcount];
	}
	(*ptrstr).namelist[loccount++] = '\0';
	(*ptrstr).firstfree = loccount;
}
/*========================================== КОНЕЦ РАБОТА С ТАБЛИЦЕЙ ИМЁН ==========================================*/