#include "pch.h"
#include "Parsing.h"

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
*st_startstatement;

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

//
// Анализ конструкции "раздел констант" */
//void ConstPart()
//{
//	if (Symbol == constsy)
//	{
//		NextSym();
//		do {
//			One_Const();
//			Accept(semicolonc);
//		} while (Symbol == ident);
//	}
//}
// Анализ конструкции "определение одной константы" */
//void One_Const()
//{
//	Accept(ident);
//	Accept(equalc);
//	Constant();
//}
/* Анализ конструкции "константа" (число, символ) */
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

// Анализ конструкции "имя константы" */
//void NameConstant()
//{
//	Accept(ident);
//}
////void String()
//////Анализ конструкции "строка"
////{
////	Accept(stringc);
////}

/* Анализ конструкции "символ" */
void Const_Char()
{
	Accept(charc);
}
//------------------------------------РАЗДЕЛ КОНСТАНТ(КОНЕЦ)----------------------------------------------------//



//------------------------------------РАЗДЕЛ ТИПОВ(НАЧАЛО)------------------------------------------------------//
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
	if (!Belong(Symbol, idstarters))
	{
		Error(2, token);
		Skipto2(idstarters, followers);
	}
	if (Symbol == ident) {
		NextSym();
		Accept(equalc);
	}
	if (!Belong(Symbol, idstarters))
	{
		Skipto2(idstarters, followers);
	}
	Type(followers);
}

//------------------ОПИСАНИЕ ТИПОВ ( ПЕРЕЧИСЛИМЫЙ, ОГРАНИЧЕННЫЙ,СТАНДАРТНЫЙ, СОСТАВНОЙ... )--------------------//
/* Анализ конструкции "тип" */
void Type(unsigned *followers)
{
	if (!Belong(Symbol, st_typ))
	{
		Error(10, token);
		Skipto2(st_typ, followers);
	}
	if (Belong(Symbol, st_typ))
	{
		if (Symbol == integerc || Symbol == plusc || Symbol == minusc || Symbol == charc || Symbol == leftparc || Symbol == ident)
			SimpleType(followers);//простой тип
		//Accept(semicolonc);
		//else
		//	if (Symbol == arraysy || Symbol == setsy || Symbol == filesy)
		//		CompoundType(followers);//составной тип
		//	else
		//		if (Symbol == arrowc)
		//			ReferenceType();//ссылочный тип
		if (!Belong(Symbol, followers))
		{
			Error(6, token);
			Skipto1(followers);
		}
	}
}
/* Анализ конструкции "простой тип" */
void SimpleType(unsigned *followers)
{
	switch (Symbol)
	{
	case leftparc: PerechislType(); break;//перечислимый тип
	case ident: NameType();break;//стандартный тип
	default://ограниченный тип
		if (Symbol == integerc || Symbol == plusc|| Symbol == minusc || Symbol == charc)
			LimitedType();
	}
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
// Анализ конструкции составной тип */
//void CompoundType(unsigned *followers)
//{
//	switch (Symbol)
//	{
//	case arraysy: //массив
//		ArrayType(followers);
//		break;
//	case setsy://множество
//		NextSym();
//		Accept(ofsy);
//		BazisType(followers);
//		break;
//	case filesy://файловый тип
//		NextSym();
//		Accept(ofsy);
//		Type(followers);
//		break;
//	}
//}
// Анализ конструкции тип элементов множества */
//void BazisType(unsigned *followers)
//{
//	SimpleType(followers);
//}
//Анализ конструкции ссылочный тип */
//void ReferenceType()
//{
//	NextSym();
//	NameType();
//}
// Анализ конструкции "регулярный тип" или массив */
//void ArrayType(unsigned *followers)
//{
//	Accept(arraysy);
//	Accept(lbracketc);
//	SimpleType(followers);
//	while (Symbol ==commac)
//	{
//		NextSym();
//		SimpleType(followers);
//	}
//	Accept(rbracketc);
//	Accept(ofsy);
//	Type(followers);
//}

//--------------------------------------------РАЗДЕЛ ТИПОВ( КОНЕЦ )---------------------------------------------//



//------------------------------------------РАЗДЕЛ ПЕРЕМЕННЫХ(НАЧАЛО)-------------------------------------------//
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
	unsigned ptra[SET_SIZE];
	int flag = 0;
	if (!Belong(Symbol, idstarters))
	{
		Error(2, token);
		Skipto2(idstarters, followers);
	}
	if (Symbol == ident) {
		NextSym();
		while (Symbol == commac)
		{			
			NextSym();
			Accept(ident);
		}
		Accept(colonc);
		Type(followers);
		if (!Belong(Symbol, followers))
		{
			Error(6, token);
			Skipto1(followers);
		}
	}
}

//------------------------------------------РАЗДЕЛ ПЕРЕМЕННЫХ(КОНЕЦ)--------------------------------------------//



//---------------------------------------РАЗДЕЛ ПРОЦЕДУР И ФУНКЦИЙ(НАЧАЛО)--------------------------------------//
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
	case functionsy:
		DeclarationFun(followers);
		break;
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
}
/* Анализ конструкции "описание функции" */
void DeclarationFun(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
	SetDisjunct(blockfol, af_headfunproc, ptra);
	HeaderFun(af_headfunproc);
	Block(ptra);
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
		Accept(ident);
		if (Symbol == leftparc)
		{
			SetDisjunct(rpar, followers, ptra);
			SetDisjunct(af_sameparam, followers, dop);
			do
			{
				NextSym();
				FormalParameters(dop,ptra);
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
/* Анализ конструкции "заголовок функции" */
void HeaderFun(unsigned *followers)
{
	unsigned ptra[SET_SIZE], dop[SET_SIZE];
	Accept(functionsy);
	if (!Belong(Symbol, idstarters))
	{
		Error(2, token);
		Skipto2(idstarters, followers);
	}
	if (Symbol == ident)
	{
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
		}
		Accept(colonc);
		NameType();//NameType(dop);
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
			break;
		case varsy:
			NextSym();
			VarDeclaration(followers);
			break;
		case functionsy:
			NextSym();
			VarDeclaration(followers);
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
/* Анализ конструкции "выражение" */
void Expression(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
	if (!Belong(Symbol, st_express))
	{
		Error(42, token);
		Skipto2(st_express, followers);
	}
	if (Belong(Symbol, st_express))
	{
		SetDisjunct(op_rel, followers, ptra);
		SimpleExpression(ptra);
		if (Symbol == equalc || Symbol == laterc || Symbol == greaterc || Symbol == latergreaterc || Symbol == laterequalc || Symbol == greaterequalc)
		{
			NextSym();
			SimpleExpression(followers);
		}
		if (!Belong(Symbol, followers))
		{
			Error(6, token);
			Skipto1(followers);
		}
	}
}
/* Анализ конструкции "простое выражение" */
void SimpleExpression(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
	if (!Belong(Symbol, st_express))
	{
		Error(42, token);
		Skipto2(st_express, followers);
	}
	if (Belong(Symbol, st_express))
	{
		SetDisjunct(op_add, followers, ptra);
		if (Belong(Symbol, op_add))
			NextSym();
		Term(ptra);
		while ((Symbol == plusc || Symbol == minusc || Symbol == orsy) && !feof(file_program))
		{
			NextSym();
			Term(ptra);
		}
		if (!Belong(Symbol, followers))
		{
			Error(6, token);
			Skipto1(followers);
		}
	}
}
/* Анализ конструкции "слагаемое" */
void Term(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
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
		Factor(ptra);
		while (Belong(Symbol, op_mult) && !feof(file_program))
		{
			NextSym();
			Factor(ptra);
		}
		if (!Belong(Symbol, followers))
		{
			Error(6, token);
			Skipto1(followers);
		}
	}
}
/* Анализ конструкции "множитель" */
void Factor(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
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
			NextSym();
			SetDisjunct(followers, af_3const1, ptra);//af_3const1 = twopoints,comma,rbracket
			SetDisjunct(ptra, rpar, ptra);
			Expression(ptra);
			if (Symbol == rightparc)
				Accept(rightparc);
			else
				Skipto1(m_scolon);
			break;
		case notsy:// отрицание перед всем множителем
			NextSym();
			Factor(followers);
			break;
		case ident://имя переменной или вызов функции
			Accept(ident);
			if (Symbol == leftparc)
			{
				SetDisjunct(followers, rpar, ptra);
				NextSym();
				NameFunc(ptra);
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
		if (!Belong(Symbol, followers))
		{
			Error(6, token);
			Skipto1(followers);
		}
	}
}
/* Анализ конструкции фактические параметры функции ( и процедуры) */
void NameFunc(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
	if (!Belong(Symbol, st_express))
	{
		Error(42, token);
		Skipto2(st_express, followers);
	}
	if (Belong(Symbol, st_express))
	{
		SetDisjunct(followers, m_comma, ptra);
		do {
			NextSym();
			/*if (Symbol == ident) */
				Expression(ptra);
		} while (Symbol == commac);
		if (!Belong(Symbol, followers))
		{
			Error(6, token);
			Skipto1(followers);
		}
	}
}
//---------------------------------------ВЫРАЖЕНИЕ ( КОНЕЦ )----------------------------------------------------//



//-----------------------------------------ОПИСАНИЕ ОПЕРАТОРОВ (НАЧАЛО)-----------------------------------------//
/*Анализ конструкции "оператор"*/
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
		//case casesy:
		//	CaseStatement(ptra);
		//	break;
		case beginsy:
			BeginStatement(ptra);
			break;
		case semicolonc:
			//Accept(semicolonc);
			break;
		case ident:
			Accept(ident);
			switch (Symbol)
			{
			case leftparc:
				//Анализ конструкции "вызов процедуры"
				SetDisjunct(followers, rpar, ptra); // rightparc,eolint 
				CallProc(ptra);
				Accept(rightparc);
				break;
			case  assignc:
				//Анализ конструкции ""
				GivingStatement(followers);
				break;
			case  semicolonc:
				break;
			default:
				if (!Belong(Symbol, af_assignment))
				{
					Error(6, token);
					Skipto1(af_assignment);
				}
				GivingStatement(followers);
				break;
			}
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
 /* Анализ конструкции "оператор присваивания" */ 
void AssignStatement(unsigned *followers)
{
	Accept(ident);
	Accept(assignc);
	Expression(followers);
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
	Accept(whilesy);
	if (Belong(Symbol, st_express))
	{
		Expression(followers);
		if (Symbol == dosy)
			NextSym();
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
	Accept(ifsy);
	if (Belong(Symbol, st_express))
	{
		Expression(followers);
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
		NextSym();
		Statement(followers);
	}
}

// Анализ конструкции "оператор выбора " */
//void CaseStatement(unsigned *followers)
//{
//	Accept(casesy);
//	Expression(followers);
//	Accept(ofsy);
//	while (Symbol == semicolonc)
//	{
//		NextSym();
//	}
//	Accept(endsy);
//}

/* Анализ конструкции "цикл с постусловием" */
void RepeatStatement(unsigned *followers)
{
	Accept(repeatsy);
	while (Symbol != untilsy && !feof(file_program))
	{
		Statement(followers);
		Accept(semicolonc);
	}
	Accept(untilsy);
	Expression(followers);
}
/* Анализ конструкции "цикл с параметром" */
void ForStatement(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
	Accept(forsy);
	if (Symbol == ident)
	{
		NextSym();
		if (Symbol == assignc)
		{
			NextSym();
			if (Belong(Symbol, st_express))
			{
				Expression(followers);
				if (Belong(Symbol, af_for1))// to или downto
				{
					NextSym();
					if (Belong(Symbol, st_express))
					{
						Expression(followers);
						if (Symbol == dosy)
							NextSym();
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
	//SetDisjunct(af_assignment, followers, ptra);//af_assignment содержит assign
	Accept(assignc);
	SetDisjunct(followers, comp_stat, ptra);
	Expression(ptra);
	if (!Belong(Symbol, followers))
	{
		Error(6, token);
		Skipto1(followers);
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
	if (!Belong(Symbol, st_all))  //st_all содержит programsy
	{
		Error(3, token); //ожидается слово PROGRAM
		Skipto2(st_all, followers);
	}
	if (Belong(Symbol, st_all))
	{
		Accept(programsy);
		if (!Belong(Symbol, idstarters))  //idstarters содержит idetn
		{
			Error(2, token); //должно идти имя
			Skipto2(idstarters, followers);
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
		while (!feof(file_program)) NextCh();
		//if (!Belong(Symbol, followers))
		//{
		//	Error(6, token);
		//	Skipto1(followers);
		//}
	}
}