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
*af_whilefor;

//-----------------------------------------���� (������)-------------------------------------------------------//

/* ������ ����������� "����" */
void Block(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
	if (!Belong(Symbol, begpart))
	{
		Error(18, token); /* ������ � ������� �������� */
		Skipto2(begpart, followers);
	}
	if (Belong(Symbol, begpart))
	{
		SetDisjunct(st_typepart,followers,ptra);
		ConstPart();//���������� ��������

		SetDisjunct(st_varpart, followers, ptra);
		TypePart();//�������� �����

		SetDisjunct(st_procfuncpart, followers, ptra);
		VarPart(ptra);//�������� ����������
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

/* ������ ����������� "������ ��������" */
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
/* ������ ����������� "����������� ����� ���������" */
void One_Const()
{
	Accept(ident);
	Accept(equalc);
	Constant();
}
/* ������ ����������� "���������" (�����, ������, ����� ...) */
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
/* ������ ����������� "��� ���������" */
void NameConstant()
{
	Accept(ident);
}
//void String()
////������ ����������� "������"
//{
//	Accept(stringc);
//}
//void Const_Char()
////������ ����������� "������"
//{
//	Accept(charc);
//}

//------------------------------------������ ��������(�����)----------------------------------------------------//



//------------------------------------������ �����(������)------------------------------------------------------//

/* ������ ����������� "������ �����" */
void TypePart()
{
	if (Symbol == typesy)
	{
		NextSym();
		do {
			//One_Type();
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
		else
			if (Symbol == arraysy || Symbol == setsy || Symbol == filesy)
				CompoundType(followers);//��������� ���
			else
				if (Symbol == arrowc)
					ReferenceType();//��������� ���
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
	case ident: NameType();break;//����������� ���  <<<��� ������� ����� ������������� �� ����� �������������� �������>>>
	default://������������ ���
		if (Symbol == integerc || Symbol == plusc|| Symbol == minusc || Symbol == charc)
			LimitedType();
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
/* ������ ����������� ��������� ��� */
void CompoundType(unsigned *followers)
{
	switch (Symbol)
	{
	case arraysy: //������
		ArrayType(followers);
		break;
	case setsy://���������
		NextSym();
		Accept(ofsy);
		BazisType(followers);
		break;
	case filesy://�������� ���
		NextSym();
		Accept(ofsy);
		Type(followers);
		break;
	}
}
/* ������ ����������� ��� ��������� ��������� */
void BazisType(unsigned *followers)
{
	SimpleType(followers);
}
/* ������ ����������� ��������� ��� */
void ReferenceType()
{
	NextSym();
	NameType();
}
/* ������ ����������� "���������� ���" ��� ������ */
void ArrayType(unsigned *followers)
{
	Accept(arraysy);
	Accept(lbracketc);
	SimpleType(followers);
	while (Symbol ==commac)
	{
		NextSym();
		SimpleType(followers);
	}
	Accept(rbracketc);
	Accept(ofsy);
	Type(followers);
}

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
		do {
			VarDeclaration(ptra, followers);
			Accept(semicolonc);
		} while (Symbol == ident);
		if (!Belong(Symbol, followers))
		{
			Error(6, token); /* ������������ ������ */
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
			if (Symbol == commac) flag = 1;
			Accept(ident);
			if (!Belong(Symbol, ptra) && Symbol != commac || flag)
			{
				Skipto2(ptra, fol);
				if (Symbol == ident) Accept(ident);
				flag = 0;
			}
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
		Error(23, token);
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
		Error(23, token);
		Skipto2(st_express, followers);
	}
	if (Belong(Symbol, st_express))
	{
		SetDisjunct(op_add, followers, ptra);
		if (Belong(Symbol, op_add))
			NextSym();
		Term(ptra);
		while (Symbol == plusc || Symbol == minusc || Symbol == orsy)
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
		Error(23, token);
		Skipto2(st_termfact, followers);
	}
	if (Belong(Symbol, st_termfact))
	{
		SetDisjunct(followers, st_express, ptra);//��������� ������� ��������� � ��������� 
		//+��������� ������������
		SetDisjunct(ptra, op_mult, ptra);
		Factor(ptra);
		while (Belong(Symbol, op_mult))
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
		Error(23, token);
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
			Accept(rightparc);
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
		Error(23, token);
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
		Error(24, token);
		Skipto2(st_statement, followers);
	}
	if (Belong(Symbol, st_statement))//intc,endsy,elsesy,untilsy,ident,beginsy,ifsy,
	//whilesy,repeatsy,forsy,casesy,withsy,semicolon,gotosy,eolint
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
			BeginStatement(followers);
			break;
		case semicolonc:
			Accept(semicolonc);
			break;
		case ident:
			Accept(ident);
			if (Symbol == leftparc)
			{
				SetDisjunct(followers, rpar, ptra);
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
		SetDisjunct(followers, af_compstatement, ptra); //af_compstatement �������� semicolon, endsy
		do {
			if (Symbol == semicolonc) Accept(semicolonc);
			Statement(ptra);
		} while (Symbol == semicolonc);
		Accept(endsy);
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
	Statement(followers);
	while (Symbol == semicolonc)
	{
		NextSym();
		Statement(followers);
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
		Accept(ident);
		Accept(semicolonc);
		Block(blockfol);
		Accept(pointc);
		//while (!feof(file_program)) NextCh();
		//if (!Belong(Symbol, followers))
		//{
		//	Error(6, token);
		//	Skipto1(followers);
		//}
	}
}