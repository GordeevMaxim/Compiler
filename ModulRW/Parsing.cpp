#include "pch.h"
#include "Parsing.h"

extern unsigned int *op_rel,
					*st_express;

//-----------------------------------------БЛОК (НАЧАЛО)------------------------------------------------------//

/* Анализ конструкции "блок" */
void Block()
{
	ConstPart();//объявление констант
	TypePart();//описание типов
	VarPart();//описание переменных
	ProcFuncPart();//описание процедур и функций
	BeginStatement();//тело программы (или составной оператор)
}

//-----------------------------------------БЛОК (КОНЕЦ)--------------------------------------------------------//



//------------------------------------РАЗДЕЛ КОНСТАНТ(НАЧАЛО)---------------------------------------------------//

/* Анализ конструкции "раздел констант" */
void ConstPart()
{
	if (Symbol == constsy)
	{
		NextSym();
		do {
			One_Const();
			Accept(semicolonc);
		} while (Symbol == ident);
	}
}
/* Анализ конструкции "определение одной константы" */
void One_Const()
{
	Accept(ident);
	Accept(equalc);
	Constant();
}
/* Анализ конструкции "константа" (число, символ, слово ...) */
void Constant()
{
	switch (Symbol)
	{
	case integerc:
		Un_Int();
		break;
	case realc:
		Un_Real();
		break;
	case plusc:;
	case minusc:
		NextSym();
		Constant();
		break;
	case ident:
		NameConstant();
		break;
	}
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
/* Анализ конструкции "имя константы" */
void NameConstant()
{
	Accept(ident);
}
//void String()
////Анализ конструкции "строка"
//{
//	Accept(stringc);
//}
//void Const_Char()
////Анализ конструкции "символ"
//{
//	Accept(charc);
//}

//------------------------------------РАЗДЕЛ КОНСТАНТ(КОНЕЦ)----------------------------------------------------//



//------------------------------------РАЗДЕЛ ТИПОВ(НАЧАЛО)------------------------------------------------------//

/* Анализ конструкции "раздел типов" */
void TypePart()
{
	if (Symbol == typesy)
	{
		NextSym();
		do {
			One_Type();
			Accept(semicolonc);
		} while (Symbol == ident);
	}
}
/* Анализ конструкции "определение одного типа" */
void One_Type()
{
	Accept(ident);
	Accept(equalc);
	Type();
}

//------------------ОПИСАНИЕ ТИПОВ ( ПЕРЕЧИСЛИМЫЙ, ОГРАНИЧЕННЫЙ,СТАНДАРТНЫЙ, СОСТАВНОЙ... )--------------------//

/* Анализ конструкции "тип" */
void Type()
{
	if (Symbol == integerc || Symbol == plusc|| Symbol == minusc || Symbol == charc || Symbol == leftparc || Symbol == ident)
		SimpleType();//простой тип
	else
		if (Symbol == arraysy || Symbol == setsy || Symbol == filesy)
			CompoundType();//составной тип
		else
			if (Symbol == arrowc)
				ReferenceType();//ссылочный тип
}
/* Анализ конструкции "простой тип" */
void SimpleType()
{
	switch (Symbol)
	{
	case leftparc: PerechislType(); break;//перечислимый тип
	case ident: NameType();break;//стандартный тип  <<<для массива будет отслеживаться на этапе семантического Анализа>>>
	default://ограниченный тип
		if (Symbol == integerc || Symbol == plusc|| Symbol == minusc || Symbol == charc)
			LimitedType();
	}
}
/* Анализ конструкции перечислимый тип */
void PerechislType()
{
	NextSym();
	Accept(ident);
	while (Symbol ==commac)
	{
		NextSym();
		Accept(ident);
	}
	Accept(rightparc);
}
/* Анализ конструкции ограниченный тип */
void LimitedType()
{
	Constant();
	Accept(twopointsc);
	Constant();
}
/* стандартный тип или Анализ конструкции имя типа */
void NameType()
{
	Accept(ident);
	if (Symbol == twopointsc)
	{
		NextSym();
		Accept(ident);
	}
}
/* Анализ конструкции составной тип */
void CompoundType()
{
	switch (Symbol)
	{
	case arraysy: //массив
		ArrayType();
		break;
	case setsy://множество
		NextSym();
		Accept(ofsy);
		BazisType();
		break;
	case filesy://файловый тип
		NextSym();
		Accept(ofsy);
		Type();
		break;
	}
}
/* Анализ конструкции тип элементов множества */
void BazisType()
{
	SimpleType();
}
/* Анализ конструкции ссылочный тип */
void ReferenceType()
{
	NextSym();
	NameType();
}
/* Анализ конструкции "регулярный тип" или массив */
void ArrayType()
{
	Accept(arraysy);
	Accept(lbracketc);
	SimpleType();
	while (Symbol ==commac)
	{
		NextSym();
		SimpleType();
	}
	Accept(rbracketc);
	Accept(ofsy);
	Type();
}

//--------------------------------------------РАЗДЕЛ ТИПОВ( КОНЕЦ )---------------------------------------------//



//------------------------------------------РАЗДЕЛ ПЕРЕМЕННЫХ(НАЧАЛО)-------------------------------------------//
/* Анализ конструкции "раздел переменных" */
void VarPart()
{
	if (Symbol == varsy)
	{
		Accept(varsy);
		do {
			VarDeclaration();
			Accept(semicolonc);
		} while (Symbol == ident);
	}
}
/* Анализ конструкции "описание однотипных переменных" */
void VarDeclaration()
{
	Accept(ident);
	while (Symbol ==commac)
	{
		NextSym();
		Accept(ident);
	}
	Accept(colonc);
	Type();
}

//------------------------------------------РАЗДЕЛ ПЕРЕМЕННЫХ(КОНЕЦ)--------------------------------------------//

//---------------------------------------РАЗДЕЛ ПРОЦЕДУР И ФУНКЦИЙ(НАЧАЛО)--------------------------------------//

/* Анализ конструкции "раздел процедур и фукций" */
void ProcFuncPart()
{
	if (Symbol == functionsy || Symbol == proceduresy)
	{
		while (Symbol == functionsy || Symbol == proceduresy)
		{
			DeclarationProcFun();
			Accept(semicolonc);
		}
	}
}
/* Анализ конструкции "описание процедуры или фукции" */
void DeclarationProcFun()
//
{
	switch (Symbol)
	{
	case proceduresy:
		DeclarationProc();
		break;
	case functionsy:
		DeclarationFun();
		break;
	}
}
/* Анализ конструкции "описание процедуры" */
void DeclarationProc()
{
	HeaderProc();
	Block();
}
/* Анализ конструкции "описание функции" */
void DeclarationFun()
{
	HeaderFun();
	Block();
}
/* Анализ конструкции "заголовок процедуры" */
void HeaderProc()
{
	Accept(proceduresy);
	Accept(ident);
	if (Symbol == leftparc)
	{
		do
		{
			NextSym();
			FormalParameters();
		} while (Symbol == semicolonc);
		Accept(rightparc);
		Accept(semicolonc);
	}
	else
		Accept(semicolonc);
}
/* Анализ конструкции "заголовок функции" */
void HeaderFun()
{
	Accept(functionsy);
	Accept(ident);
	if (Symbol == leftparc)
	{
		//NextSym();
		do
		{
			//if (Symbol==semicolon) 
			NextSym();
			FormalParameters();
		} while (Symbol == semicolonc);
		Accept(rightparc);
	}
	Accept(colonc);
	NameType();
	Accept(semicolonc);
}
/* Анализ конструкции "раздел фомальных параметорв" */
void FormalParameters()
{
	switch (Symbol)
	{
	case ident:
		VarDeclaration();
		break;
	case varsy:
		NextSym();
		VarDeclaration();
		break;
	case functionsy:
		NextSym();
		VarDeclaration();
		break;
	case proceduresy:
		NextSym();
		Accept(ident);
		while (Symbol ==commac)
			Accept(ident);
		break;
	}
}
//---------------------------------------РАЗДЕЛ ПРОЦЕДУР И ФУНКЦИЙ(КОНЕЦ)---------------------------------------//

//--------------------------------------------ВЫРАЖЕНИЕ ( НАЧАЛО )----------------------------------------------//
/* Анализ конструкции "выражение" */
void Expression()
{
	SimpleExpression();
	if (Symbol == equalc || Symbol == laterc || Symbol == greaterc || Symbol == latergreaterc || Symbol == laterequalc || Symbol == greaterequalc)
	{
		NextSym();
		SimpleExpression();
	}
}
/* Анализ конструкции "простое выражение" */
void SimpleExpression()
{
	if (Symbol == plusc || Symbol == minusc)
		NextSym();
	Term();
	while (Symbol == plusc || Symbol == minusc || Symbol == orsy)
	{
		NextSym();
		Term();
	}
}
/* Анализ конструкции "слагаемое" */
void Term()
{
	Factor();
	while (Symbol == plusc || Symbol == minusc || Symbol == divsy || Symbol == modsy || Symbol == starc || Symbol == slashc)
	{
		NextSym();
		Factor();
	}
}
/* Анализ конструкции "множитель" */
void Factor()
{
	switch (Symbol)
	{
	case leftparc://если в скобках т.е составной множитель
		NextSym();
		Expression();
		Accept(rightparc);
		break;
	case notsy:// отрицание перед всем множителем
		NextSym();
		Factor();
		break;
	case ident://имя переменной или вызов функции
		Accept(ident);
		if (Symbol == leftparc)
		{
			NameFunc();
			Accept(rightparc);
		}
		break;
	case integerc://целое число
		Accept(integerc);
		break;
	case realc://вещественное число
		Accept(realc);
		break;
	}
}
/* Анализ конструкции фактические параметры функции ( и процедуры) */
void NameFunc()
{
	do{
		NextSym();
		Expression();
	} while (Symbol ==commac);
}
//---------------------------------------ВЫРАЖЕНИЕ ( КОНЕЦ )----------------------------------------------------//



//-----------------------------------------ОПИСАНИЕ ОПЕРАТОРОВ (НАЧАЛО)-----------------------------------------//
/*Анализ конструкции "оператор"*/
void Statement()
{
	switch (Symbol)
	{
	case whilesy:
		//Анализ конструкции "цикл с предусловием"
		WhileStatement();
		break;
	case repeatsy:
		//Анализ конструкции "цикл с постусловием"
		RepeatStatement();
		break;
	case forsy:
		//Анализ конструкции "цикл с параметром"
		ForStatement();
		break;
	case ifsy:
		//Анализ конструкции "условный оператор"
		IfStatement();
		break;
	case casesy:
		CaseStatement();
		break;
	case beginsy:
		BeginStatement();
		break;
	case semicolonc:
		Accept(semicolonc);
		break;
	case ident:
		Accept(ident);
		if (Symbol == leftparc)
		{
			CallProc();
			Accept(rightparc);
		}
		else
			if (Symbol == assignc)
				GivingStatement();
			break;
	case endsy:		break;
	}
}
 /* Анализ конструкции "оператор присваивания" */ 
void AssignStatement()
{
	Accept(ident);
	Accept(assignc);
	Expression();
}
/* Анализ конструкции "составной оператор" */
void BeginStatement()
 {
	 Accept(beginsy);
	 do  {
		 if (Symbol == semicolonc) Accept(semicolonc);
		 Statement();		 
	 } while (Symbol == semicolonc);
	 Accept(endsy);
 }
/* Анализ конструкции "цикл с предусловием" */
void WhileStatement()
{	
	Accept(whilesy);
	Expression();
	Accept(dosy);
	Statement();
}
/* Анализ конструкции "условный оператор" */
void IfStatement()
{
	Accept(ifsy);
	Expression();
	Accept(thensy);
	Statement();
	if (Symbol == elsesy)
	{
		NextSym();
		Statement();
	}
}
/* Анализ конструкции "оператор выбора " */
void CaseStatement()
{
	Accept(casesy);
	Expression();
	Accept(ofsy);
	while (Symbol == semicolonc)
	{
		NextSym();
	}
	Accept(endsy);
}
/* Анализ конструкции "цикл с постусловием" */
void RepeatStatement()
{
	Accept(repeatsy);
	Statement();
	while (Symbol == semicolonc)
	{
		NextSym();
		Statement();
	}
	Accept(untilsy);
	Expression();
}
/* Анализ конструкции "цикл с параметром" */
void ForStatement()
{
	Accept(forsy);
	Accept(ident);
	Accept(assignc);
	Expression();
	if (Symbol == tosy || Symbol == downtosy)
		NextSym();
	Expression();
	Accept(dosy);
	Statement();
}
/* Инициализация анализа выражения */
void GivingStatement()
{
	Accept(assignc);
	Expression();
}
/*Инициализация анализа процедуры*/
void CallProc()
{
	NameFunc();
}
//-----------------------------------------ОПИСАНИЕ ОПЕРАТОРОВ (КОНЕЦ)-----------------------------------------//



void Programme()
{
	Accept(programsy);
	Accept(ident);
	Accept(semicolonc);
	Block();
	Accept(pointc);
}