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
*af_repeat;

//-----------------------------------------���� (������)-------------------------------------------------------//

/* ������ ����������� "����" */
void Block(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
	if (!Belong(Symbol, begpart))//labelsy,constsy,typesy,varsy,functionsy,proceduresy, beginsy, eolint
	{
		Error(18, token); /* ������ � ������� �������� */
		Skipto2(begpart, followers);
	}
	if (Belong(Symbol, begpart))
	{
		SetDisjunct(st_varpart, followers, ptra);//pointc,endoffile,eolint + varsy,functionsy,proceduresy, beginsy, eolint
		TypePart(ptra);//�������� �����

		SetDisjunct(st_procfuncpart, followers, ptra);//pointc,endoffile,eolint + functionsy,proceduresy, beginsy, eolint
		VarPart(ptra);//�������� ����������
		SetDisjunct(st_statpart, followers, ptra);//pointc,endoffile,eolint + beginsy, eolint
		ProcFuncPart(ptra);//�������� �������� � �������
		BeginStatement(followers);//���� ��������� (��� ��������� ��������)
		if (!Belong(Symbol, followers))
		{
			Error(6, token); /* ������������ ������ */
			Skipto1(followers);
		}
	}
}

//-----------------------------------------���� (�����)--------------------------------------------------------//



//------------------------------------������ ��������(������)---------------------------------------------------//

//
// ������ ����������� "������ ��������" */
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
// ������ ����������� "����������� ����� ���������" */
//void One_Const()
//{
//	Accept(ident);
//	Accept(equalc);
//	Constant();
//}
/* ������ ����������� "���������" (�����, ������) */
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
/* ������ ����������� "����� ��� �����" */
void Un_Int()
{
	Accept(integerc);
}
/* ������ ����������� "������������ ��� �����" */
void Un_Real()
{
	Accept(realc);
}

// ������ ����������� "��� ���������" */
//void NameConstant()
//{
//	Accept(ident);
//}
////void String()
//////������ ����������� "������"
////{
////	Accept(stringc);
////}

/* ������ ����������� "������" */
void Const_Char()
{
	Accept(charc);
}
//------------------------------------������ ��������(�����)----------------------------------------------------//



//------------------------------------������ �����(������)------------------------------------------------------//

/* ������ ����������� "������ �����" */
void TypePart(unsigned *followers)
{
	if (Symbol == typesy)
	{
		NextSym();
		do {
			One_Type(followers);
			Accept(semicolonc);
		} while (Symbol == ident);
	}
}
/* ������ ����������� "����������� ������ ����" */
void One_Type(unsigned *followers)
{
	Accept(ident);
	Accept(equalc);
	Type(followers);
}

//------------------�������� ����� ( ������������, ������������,�����������, ���������... )--------------------//

/* ������ ����������� "���" */
void Type(unsigned *followers)
{
	if (!Belong(Symbol, st_typ))
	{
		Error(18, token);
		Skipto2(st_typ, followers);
	}
	if (Belong(Symbol, st_typ))
	{
		if (Symbol == integerc || Symbol == plusc || Symbol == minusc || Symbol == charc || Symbol == leftparc || Symbol == ident)
			SimpleType(followers);//������� ���
		//else
		//	if (Symbol == arraysy || Symbol == setsy || Symbol == filesy)
		//		CompoundType(followers);//��������� ���
		//	else
		//		if (Symbol == arrowc)
		//			ReferenceType();//��������� ���
		if (!Belong(Symbol, followers))
		{
			Error(6, token);
			Skipto1(followers);
		}
	}
}
/* ������ ����������� "������� ���" */
void SimpleType(unsigned *followers)
{
	switch (Symbol)
	{
	case leftparc: PerechislType(); break;//������������ ���
	case ident: NameType();break;//����������� ���
	default://������������ ���
		if (Symbol == integerc || Symbol == plusc|| Symbol == minusc || Symbol == charc)
			LimitedType();
	}
}
/* ����������� ��� ��� ������ ����������� ��� ���� */
void NameType()
{
	Accept(ident);
	if (Symbol == twopointsc)
	{
		NextSym();
		Accept(ident);
	}
}

/* ������ ����������� ������������ ��� */
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
/* ������ ����������� ������������ ��� */
void LimitedType()
{
	Constant();
	Accept(twopointsc);
	Constant();
}
// ������ ����������� ��������� ��� */
//void CompoundType(unsigned *followers)
//{
//	switch (Symbol)
//	{
//	case arraysy: //������
//		ArrayType(followers);
//		break;
//	case setsy://���������
//		NextSym();
//		Accept(ofsy);
//		BazisType(followers);
//		break;
//	case filesy://�������� ���
//		NextSym();
//		Accept(ofsy);
//		Type(followers);
//		break;
//	}
//}
// ������ ����������� ��� ��������� ��������� */
//void BazisType(unsigned *followers)
//{
//	SimpleType(followers);
//}
//������ ����������� ��������� ��� */
//void ReferenceType()
//{
//	NextSym();
//	NameType();
//}
// ������ ����������� "���������� ���" ��� ������ */
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

//--------------------------------------------������ �����( ����� )---------------------------------------------//



//------------------------------------------������ ����������(������)-------------------------------------------//
/* ������ ����������� "������ ����������" */
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
			VarDeclaration(ptra, followers);
			Accept(semicolonc);
		}
		if (!Belong(Symbol, followers))
		{
			Error(2, token); /* ������ ���� ��� */
			Skipto1(followers);
		}
	}
}
/* ������ ����������� "�������� ���������� ����������" */
void VarDeclaration(unsigned *followers, unsigned *fol)
{
	unsigned ptra[SET_SIZE], dop[SET_SIZE];
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

//------------------------------------------������ ����������(�����)--------------------------------------------//

//---------------------------------------������ �������� � �������(������)--------------------------------------//

/* ������ ����������� "������ �������� � ������" */
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
/* ������ ����������� "�������� ��������� ��� ������" */
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
/* ������ ����������� "�������� ���������" */
void DeclarationProc(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
	SetDisjunct(blockfol, af_headfunproc, ptra);
	HeaderProc(ptra);
	Block(af_blockprocfunc);
}
/* ������ ����������� "�������� �������" */
void DeclarationFun(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
	SetDisjunct(blockfol, af_headfunproc, ptra);
	HeaderFun(af_headfunproc);
	Block(ptra);
}
/* ������ ����������� "��������� ���������" */
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
/* ������ ����������� "��������� �������" */
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
/* ������ ����������� "������ ��������� ����������" */
void FormalParameters(unsigned *followers, unsigned *fol)
{
	if (!Belong(Symbol, m_fpar))
	{
		Error(20, token);//�� ���� ��� �������
		Skipto2(m_fpar, followers);
	}
	if (Belong(Symbol, m_fpar))
	{
		switch (Symbol)
		{
		case ident:
			VarDeclaration(followers, fol);
			break;
		case varsy:
			NextSym();
			VarDeclaration(followers, fol);
			break;
		case functionsy:
			NextSym();
			VarDeclaration(followers, fol);
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
//---------------------------------------������ �������� � �������(�����)---------------------------------------//

//--------------------------------------------��������� ( ������ )----------------------------------------------//
/* ������ ����������� "���������" */
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
/* ������ ����������� "������� ���������" */
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
/* ������ ����������� "���������" */
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
		SetDisjunct(followers, st_express, ptra);//��������� ������� ��������� � ��������� 
		//+��������� ������������
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
/* ������ ����������� "���������" */
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
		case leftparc://���� � ������� �.� ��������� ���������
			NextSym();
			SetDisjunct(followers, af_3const1, ptra);//af_3const1 = twopoints,comma,rbracket
			SetDisjunct(ptra, rpar, ptra);
			Expression(ptra);
			if (Symbol == rightparc)
				Accept(rightparc);
			else
				Skipto1(m_scolon);
			break;
		case notsy:// ��������� ����� ���� ����������
			NextSym();
			Factor(followers);
			break;
		case ident://��� ���������� ��� ����� �������
			Accept(ident);
			if (Symbol == leftparc)
			{
				SetDisjunct(followers, rpar, ptra);
				NextSym();
				NameFunc(ptra);
				Accept(rightparc);
			}
			break;
		case integerc://����� �����
			Accept(integerc);
			break;
		case realc://������������ �����
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
/* ������ ����������� ����������� ��������� ������� ( � ���������) */
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
			Expression(ptra);
		} while (Symbol == commac);
		if (!Belong(Symbol, followers))
		{
			Error(6, token);
			Skipto1(followers);
		}
	}
}
//---------------------------------------��������� ( ����� )----------------------------------------------------//



//-----------------------------------------�������� ���������� (������)-----------------------------------------//
/*������ ����������� "��������"*/
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
			//������ ����������� "���� � ������������"
			SetDisjunct(ptra, af_whilefor, ptra);
			WhileStatement(ptra);
			break;
		case repeatsy:
			//������ ����������� "���� � ������������"
			SetDisjunct(ptra, af_repeat, ptra);
			RepeatStatement(ptra);
			break;
		case forsy:
			//������ ����������� "���� � ����������"
			SetDisjunct(ptra, af_whilefor, ptra);
			SetDisjunct(ptra, af_for1, ptra);
			ForStatement(ptra);
			break;
		case ifsy:
			//������ ����������� "�������� ��������"
			SetDisjunct(ptra, af_iftrue, ptra);
			IfStatement(ptra);
			break;
		case casesy:
			CaseStatement(ptra);
			break;
		case beginsy:
			BeginStatement(ptra);
			break;
		case semicolonc:
			Accept(semicolonc);
			break;
		case ident:
			Accept(ident);
			if (Symbol == leftparc)
			{
				SetDisjunct(followers, rpar, ptra); // rightparc,eolint 
				CallProc(ptra);
				Accept(rightparc);
			}
			else
				if (Symbol == assignc)
					GivingStatement(followers);
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
 /* ������ ����������� "�������� ������������" */ 
void AssignStatement(unsigned *followers)
{
	Accept(ident);
	Accept(assignc);
	Expression(followers);
}
/* ������ ����������� "��������� ��������" */
void BeginStatement(unsigned *followers)
 {
	unsigned ptra[SET_SIZE];
	if (!Belong(Symbol, st_statpart))//st_statpart �������� beginsy
	{
		Error(17, token);
		Skipto2(st_statpart, followers);
	}
	if (Belong(Symbol, st_statpart))//st_statpart �������� beginsy
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
/* ������ ����������� "���� � ������������" */
void WhileStatement(unsigned *followers)
{	
	Accept(whilesy);
	Expression(followers);
	Accept(dosy);
	Statement(followers);
}
/* ������ ����������� "�������� ��������" */
void IfStatement(unsigned *followers)
{
	Accept(ifsy);
	Expression(followers);
	Accept(thensy);
	Statement(followers);
	if (Symbol == elsesy)
	{
		NextSym();
		Statement(followers);
	}
}
/* ������ ����������� "�������� ������ " */
void CaseStatement(unsigned *followers)
{
	Accept(casesy);
	Expression(followers);
	Accept(ofsy);
	while (Symbol == semicolonc)
	{
		NextSym();
	}
	Accept(endsy);
}
/* ������ ����������� "���� � ������������" */
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
/* ������ ����������� "���� � ����������" */
void ForStatement(unsigned *followers)
{
	Accept(forsy);
	Accept(ident);
	Accept(assignc);
	Expression(followers);
	if (Symbol == tosy || Symbol == downtosy)
		NextSym();
	Expression(followers);
	Accept(dosy);
	Statement(followers);
}
/* ������������� ������� ��������� */
void GivingStatement(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
	SetDisjunct(af_assignment, followers, ptra);//af_assignment �������� assign
	Accept(assignc);
	SetDisjunct(followers, comp_stat, ptra);
	Expression(ptra);
	if (!Belong(Symbol, followers))
	{
		Error(6, token);
		Skipto1(followers);
	}
}
/*������������� ������� ���������*/
void CallProc(unsigned *followers)
{
	NameFunc(followers);
}
//-----------------------------------------�������� ���������� (�����)-----------------------------------------//


void Programme(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
	if (!Belong(Symbol, st_all))  //st_all �������� programsy
	{
		Error(3, token); //��������� ����� PROGRAM
		Skipto2(st_all, followers);
	}
	if (Belong(Symbol, st_all))
	{
		Accept(programsy);
		if (!Belong(Symbol, idstarters))  //idstarters �������� idetn
		{
			Error(2, token); //������ ���� ���
			Skipto2(idstarters, followers);
		}
		Accept(ident);	
		if (!Belong(Symbol, m_scolon))  //idstarters �������� semilicon
		{
			Error(14, token); //������ ���� ;
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