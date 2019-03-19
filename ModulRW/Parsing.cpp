#include "pch.h"
#include "Parsing.h"

extern unsigned int *op_rel,
					*st_express;

//-----------------------------------------���� (������)------------------------------------------------------//

/* ������ ����������� "����" */
void Block()
{
	ConstPart();//���������� ��������
	TypePart();//�������� �����
	VarPart();//�������� ����������
	ProcFuncPart();//�������� �������� � �������
	BeginStatement();//���� ��������� (��� ��������� ��������)
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
			One_Type();
			Accept(semicolonc);
		} while (Symbol == ident);
	}
}
/* ������ ����������� "����������� ������ ����" */
void One_Type()
{
	Accept(ident);
	Accept(equalc);
	Type();
}

//------------------�������� ����� ( ������������, ������������,�����������, ���������... )--------------------//

/* ������ ����������� "���" */
void Type()
{
	if (Symbol == integerc || Symbol == plusc|| Symbol == minusc || Symbol == charc || Symbol == leftparc || Symbol == ident)
		SimpleType();//������� ���
	else
		if (Symbol == arraysy || Symbol == setsy || Symbol == filesy)
			CompoundType();//��������� ���
		else
			if (Symbol == arrowc)
				ReferenceType();//��������� ���
}
/* ������ ����������� "������� ���" */
void SimpleType()
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
void CompoundType()
{
	switch (Symbol)
	{
	case arraysy: //������
		ArrayType();
		break;
	case setsy://���������
		NextSym();
		Accept(ofsy);
		BazisType();
		break;
	case filesy://�������� ���
		NextSym();
		Accept(ofsy);
		Type();
		break;
	}
}
/* ������ ����������� ��� ��������� ��������� */
void BazisType()
{
	SimpleType();
}
/* ������ ����������� ��������� ��� */
void ReferenceType()
{
	NextSym();
	NameType();
}
/* ������ ����������� "���������� ���" ��� ������ */
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

//--------------------------------------------������ �����( ����� )---------------------------------------------//



//------------------------------------------������ ����������(������)-------------------------------------------//
/* ������ ����������� "������ ����������" */
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
/* ������ ����������� "�������� ���������� ����������" */
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

//------------------------------------------������ ����������(�����)--------------------------------------------//

//---------------------------------------������ �������� � �������(������)--------------------------------------//

/* ������ ����������� "������ �������� � ������" */
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
/* ������ ����������� "�������� ��������� ��� ������" */
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
/* ������ ����������� "�������� ���������" */
void DeclarationProc()
{
	HeaderProc();
	Block();
}
/* ������ ����������� "�������� �������" */
void DeclarationFun()
{
	HeaderFun();
	Block();
}
/* ������ ����������� "��������� ���������" */
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
/* ������ ����������� "��������� �������" */
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
/* ������ ����������� "������ ��������� ����������" */
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
//---------------------------------------������ �������� � �������(�����)---------------------------------------//

//--------------------------------------------��������� ( ������ )----------------------------------------------//
/* ������ ����������� "���������" */
void Expression()
{
	SimpleExpression();
	if (Symbol == equalc || Symbol == laterc || Symbol == greaterc || Symbol == latergreaterc || Symbol == laterequalc || Symbol == greaterequalc)
	{
		NextSym();
		SimpleExpression();
	}
}
/* ������ ����������� "������� ���������" */
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
/* ������ ����������� "���������" */
void Term()
{
	Factor();
	while (Symbol == plusc || Symbol == minusc || Symbol == divsy || Symbol == modsy || Symbol == starc || Symbol == slashc)
	{
		NextSym();
		Factor();
	}
}
/* ������ ����������� "���������" */
void Factor()
{
	switch (Symbol)
	{
	case leftparc://���� � ������� �.� ��������� ���������
		NextSym();
		Expression();
		Accept(rightparc);
		break;
	case notsy:// ��������� ����� ���� ����������
		NextSym();
		Factor();
		break;
	case ident://��� ���������� ��� ����� �������
		Accept(ident);
		if (Symbol == leftparc)
		{
			NameFunc();
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
}
/* ������ ����������� ����������� ��������� ������� ( � ���������) */
void NameFunc()
{
	do{
		NextSym();
		Expression();
	} while (Symbol ==commac);
}
//---------------------------------------��������� ( ����� )----------------------------------------------------//



//-----------------------------------------�������� ���������� (������)-----------------------------------------//
/*������ ����������� "��������"*/
void Statement()
{
	switch (Symbol)
	{
	case whilesy:
		//������ ����������� "���� � ������������"
		WhileStatement();
		break;
	case repeatsy:
		//������ ����������� "���� � ������������"
		RepeatStatement();
		break;
	case forsy:
		//������ ����������� "���� � ����������"
		ForStatement();
		break;
	case ifsy:
		//������ ����������� "�������� ��������"
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
 /* ������ ����������� "�������� ������������" */ 
void AssignStatement()
{
	Accept(ident);
	Accept(assignc);
	Expression();
}
/* ������ ����������� "��������� ��������" */
void BeginStatement()
 {
	 Accept(beginsy);
	 do  {
		 if (Symbol == semicolonc) Accept(semicolonc);
		 Statement();		 
	 } while (Symbol == semicolonc);
	 Accept(endsy);
 }
/* ������ ����������� "���� � ������������" */
void WhileStatement()
{	
	Accept(whilesy);
	Expression();
	Accept(dosy);
	Statement();
}
/* ������ ����������� "�������� ��������" */
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
/* ������ ����������� "�������� ������ " */
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
/* ������ ����������� "���� � ������������" */
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
/* ������ ����������� "���� � ����������" */
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
/* ������������� ������� ��������� */
void GivingStatement()
{
	Accept(assignc);
	Expression();
}
/*������������� ������� ���������*/
void CallProc()
{
	NameFunc();
}
//-----------------------------------------�������� ���������� (�����)-----------------------------------------//



void Programme()
{
	Accept(programsy);
	Accept(ident);
	Accept(semicolonc);
	Block();
	Accept(pointc);
}