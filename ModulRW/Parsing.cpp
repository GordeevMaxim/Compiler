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

*set_VARCONFUNCS,	/* ���. ������� ������������� - VARS, CONSTS, FUNCS */
*set_VARS,			/* ���������� ������ ������������� - VARS		*/
*set_TYPES, 		/* ���������� ������ ������������� - TYPES 		*/
*set_CONSTS,		/* ���������� ������ ������������� - CONSTS 		*/
*set_TYCON,			/* ���������� ������� ������������� - TYPES,CONSTS	*/
*set_FUNCS,			/* ���������� ������ ������������� - FUNCS              */
*set_PROCS,			/* ���������� ������ ������������� - PROCS              */
*set_FUNPR,			/* ���������� ������� ������������� - FUNCS,PROCS       */
*set_VARFUNPR;		/* ���������� ������� ������������� - VARS,FUNCS,PROCS  */


/*----------------------------- ���������� ��� ��������� -----------------------------*/
int myconst;
int kol;
int level;
SCOPE* localscope;
struct treenode *entry;
#define MAXCONST  	32767
TYPEREC *booltype, *realtype, *chartype, *inttype, *texttype, *niltype;
struct listrec *varlist; // ����� ������ ���������������� ����� ���������� ����������
TYPEREC *vartype;//������� ���������� vartype �������� ����� ����������� ��� ���������� ����������*/
/*----------------------------- ����� ���������� ��� ��������� -----------------------------*/



/* ������������� ����� ��������� � ������ �������� ���� (�������� true, false) */
struct reestrconsts *newconst(char *addrname)
{
	struct reestrconsts *myconst = (struct reestrconsts *)malloc(sizeof(struct reestrconsts));
	myconst->addrconsts = addrname; /* ����������� ��� ����� ��������� */
	myconst->next = NULL;			/* �������� ����� */
	return(myconst);
}

/*============================================= ������ � ������� ��� =============================================*/
#define MAXHASH  997			/* ������������ �������� hash-������� */
#define MAXNUM   500			/* ���������� ����� � ������ ��� ���������� ��������	*/

extern char name[80];
extern int lname;				/* ��� ����� �������� ����� � ������� */
unsigned hashresult;			/* ��������� hash-������� ��� �������� ����� */
char *addrname;					/* ����� ����� � ������� ���� */

struct zveno     				/* ������ ��� ���������� �������� */
{
	int firstfree;				/* ����� ������ ��������� ������� ������ � ������ */
	char namelist[MAXNUM];		/* ������������������ ���� */
	struct zveno *next;			/* ��������� �� ��������� ������ � ��� �� ��������� hash-������� */
};

typedef struct zveno str;

static str *nametable[MAXHASH],	/* ������ ���������� �� ������ ������ ��� ���������� �������� �������������� �������� �������� hash-������� */
*ptrstr; 			/* ��������� �� ������� ������ ��� ���������� ��������  */

static unsigned chcount,		/* ������� ����� �������� �����		*/
loccount;		/* ������� ����� � ���������� ������	*/


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

		StartMainGeneration();

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
/* ������ ����������� "���������" (�����, ������) */
void Constant()
{

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
/* ������ ����������� "������" */
void Const_Char()
{
	Accept(charc);
}
//------------------------------------������ ��������(�����)----------------------------------------------------//



//------------------------------------������ �����(������)------------------------------------------------------//

TYPEREC *typentry;						/* ��������� �� ���������� ���� */
NODE *identry;							/* ��������� �� ������� ���� ��������������� */
struct reestrconsts *constlist = NULL;	/* ��������� �� ������ ������� �������� � ����������� �������������� ���� */

////��� ������������� ����
//int numb1 = 0; //������� const  � ������������ ����. ����� CONST..
//struct reestrconsts *head; // ��������������� ������ ��� ������ const (�� ������)
//int numb2 = 0; //������� const  � ������������ ���� ����� ..CONST

/* ������ ����������� "������ �����" */
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
			Error(2, token); /* ������ ���� ��� */
			Skipto1(followers);
		}
	}
}
/* ������ ����������� "����������� ������ ����" */
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
		typ_node = NewIdent(hashresult, addrname, TYPES);//������ ����� ���
		Accept(equalc);
	}
	if (!Belong(Symbol, idstarters))
	{
		Skipto2(idstarters, followers);
	}
	typ_node->idtype = Type(followers);//������ ������ �� ���� �� �������
}

//------------------�������� ����� ( ������������, ������������,�����������, ���������... )--------------------//
/* ������ ����������� "���" */
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
			tag = SimpleType(followers);//������� ���
		if (!Belong(Symbol, followers))
		{
			Error(6, token);
			Skipto1(followers);
		}
	}
	return tag;
}
/* ������ ����������� "������� ���" */
TYPEREC *SimpleType(unsigned *followers)
{
	TYPEREC *TypeTag = NULL;
	typentry = (TYPEREC *)malloc(sizeof(TYPEREC));
	switch (Symbol)
	{
	case leftparc: /* ������������ ��� */
		typentry = NewType(ENUMS); // "������" ����������� ����
		TypeTag = PerechislType(followers);
		break;
	case ident: /* ����������� ��� */
		//typentry = NewType(SCALARS); // "������" ����������� ����
		TypeTag = NameType();
		break;
	default:/* ������������ ��� */
		if (Symbol == integerc || Symbol == plusc || Symbol == minusc || Symbol == charc)
		{
			typentry = NewType(LIMITEDS); // "������" ����������� ����
			TypeTag = LimitedType(followers);
		}
	}
	return TypeTag;
}
/* ����������� ��� ��� ������ ����������� ��� ���� */
TYPEREC *NameType()
{
	SearchInTable(name);
	identry = SearchIdent(hashresult, addrname, set_TYPES);
	if (identry == NULL)
		identry = NewIdent(hashresult, addrname, TYPES);
	typentry = identry->idtype;//���� �� ����� ����� (��� �����), �� ���������� ���
	Accept(ident);
	//if (Symbol == twopointsc)
	//{
	//	NextSym();
	//	Accept(ident);
	//}
	return typentry;
}
/* ������ ����������� ������������ ��� */
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
			identry = NewIdent(hashresult, addrname, CONSTS);				// ������� ������� � ������� ������� ��� ��������� ������������� ����
		identry->idtype = typentry;											//���������� ��������� �� ���������� ���� ��������� � ������� �������
		(identry->casenode).constvalue.enumval = addrname;					// ���������� ��� ��������� � ������� �������
		(typentry->casetype).firstconst = constlist = newconst(addrname);	// ������� ������ ������� ������ �������� � ����������� �������������� ����
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
					identry = NewIdent(hashresult, addrname, CONSTS);	// ������� ������� � ������� ������� ��� ��������� ������������� ����
				identry->idtype = typentry;								//���������� ��������� �� ���������� ���� ��������� � ������� ����
				(identry->casenode).constvalue.enumval = addrname;		// ���������� ��� ��������� � ������� �����
				//������� ��������� ������� ������ �������� � ����������� �������������� ����
				constlist->next = newconst(addrname);
				constlist = constlist->next;
				Accept(ident);
			}
		}
		Accept(rightparc);
	}
	return typentry;
}
/* ������ ����������� ������������ ��� */
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
				//�������� �� �������
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
				//�������� �� �������
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

//--------------------------------------------������ �����( ����� )---------------------------------------------//



//------------------------------------------������ ����������(������)-------------------------------------------//

/*�������� ��������� ���������������� ������;
	� ������ ������ ���� ������� ���������� � ������������������ ��� �� ����������� ������������*/
void NewVariable()
{
	struct listrec *listentry; //��������� �� ������� ������� ���������������� ������
	if (Symbol == ident)
	{
		kol++;
		listentry = (struct listrec*)malloc(sizeof(struct listrec)); // ��������� ������ ��� ����� ��������� ������� ����
		listentry->id_r = NewIdent(hashresult, addrname, VARS); // ��������� ����� � ������
		listentry->next = varlist; // ������������� � �����
		varlist = listentry;
	}
}
/*����������� �������� ���� idtype ��� ������, ������ ������� �������� �� ��������������� �����*/
void AddAttributes()
{
	struct listrec *listentry; // ��������� �� ������� ���������������� ������
	struct listrec *oldentry; // ��������� �� �������, �������������� ������� �� ��������������� ������
	listentry = varlist;
	while (listentry != NULL)
	{
		if (listentry->id_r != NULL)//���� ���������� �� ������, �.� �� ���� ������� �� �����
			listentry->id_r->idtype = vartype; // ������� ���������� vartype �������� ����� ����������� ��� ���������� ����������
		oldentry = listentry;
		listentry = listentry->next;
		free((void*)oldentry);
	}
}
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
			Error(2, token); /* ������ ���� ��� */
			Skipto1(followers);
		}
	}
}
/* ������ ����������� "�������� ���������� ����������" */
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

//------------------------------------------������ ����������(�����)--------------------------------------------//



//---------------------------------------������ �������� � �������(������)--------------------------------------//
NODE *myfunc; // ��������� �� ������� ���� ���������������
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
/* ������ ����������� "�������� ���������" */
void DeclarationProc(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
	SetDisjunct(blockfol, af_headfunproc, ptra);
	HeaderProc(ptra);
	Block(af_blockprocfunc);
	Close_Scope();
	level--;
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
/* ������ ����������� "������ ��������� ����������" */
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
//---------------------------------------������ �������� � �������(�����)---------------------------------------//



//--------------------------------------------��������� ( ������ )----------------------------------------------//

//**************************************************************************************************************//
//-------------------------------------������� ��� ��������� ( ������ )-----------------------------------------//
//��� simpleexpression
int Right_Sign(TYPEREC * type)
{//���� +/- ����� ���� ������� ����� ����������, ���� �� �������� ������(����� ������������) ��� ������ � ���������
	if (type == NULL || type == inttype || type == realtype
		|| (type->typecode == LIMITEDS && (type->casetype.limtype.basetype == inttype)))
		return (1);
	else
	{
		Error(211, token);//������������ ���� ��������� �������� +/-
		return (0);
	}
}
TYPEREC *Comparing(TYPEREC * type1, TYPEREC * type2, unsigned oper)
{// �������� ����������� ����� � ��������� ���������
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
				return NULL;//����� ��� ����������
			}
	// later/*  '<'   */, laterequal/*  '<='   */, greater/*  '>'   */, greaterequal/*  '>='   */,
	//equal/*  '='   */, latergreater/*  '<>'   */
}
TYPEREC *TestAdd(TYPEREC * type1, TYPEREC * type2, unsigned oper)
{//�������� ������������ ����� � ���������
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
		return NULL;// ����� NULL?
	}
}
TYPEREC *TestMult(TYPEREC * type1, TYPEREC * type2, unsigned oper)
{//�������� ����������� ����� � ���������
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
//--------------------------------------������� ��� ��������� ( ����� )-----------------------------------------//
//**************************************************************************************************************//

/* ������ ����������� "���������" */
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
			//�������� ������������� ����� ������� ���������
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
/* ������ ����������� "������� ���������" */
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
/* ������ ����������� "���������" */
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
		SetDisjunct(followers, st_express, ptra);//��������� ������� ��������� � ��������� 
		//+��������� ������������
		SetDisjunct(ptra, op_mult, ptra);
		ex1type = Factor(ptra);
		while (Belong(Symbol, op_mult) && !feof(file_program))
		{
			//flag = 1;
			operation = Symbol;
			GeneralGeneration(Symbol);
			NextSym();
			ex2type = Factor(ptra);
			//�������� ������������� ����� ����������
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
/* ������ ����������� "���������" */
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
		case leftparc://���� � ������� �.� ��������� ���������
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
		case notsy:// ��������� ����� ���� ����������
			myconst = 0;
			//flag = 1;
			NextSym();
			extype = Factor(followers);
			extype = Logical(extype);
			break;
		case ident://��� ���������� 
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
		case integerc://����� �����
			myconst = 1;
			//flag = 1;
			Accept(integerc);
			extype = inttype;
			break;
		case realc://������������ �����
			myconst = 0;
			//flag = 1;
			Accept(realc);
			extype = realtype;
			break;
		case charc://������
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
/* ������ ����������� ����������� ��������� ������� ( � ���������) */
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
//--------------------------------------------��������� ( ����� )-----------------------------------------------//



//-----------------------------------------�������� ���������� (������)-----------------------------------------//

//===================================================C��������==================================================//
bool �ompatible(TYPEREC* type1, TYPEREC * type2)
{//�������� ����������� ����� � ��������� ������������
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
									Error(222, token);//�������� ������� ��������� ��������
							}
							else
								if (type1->casetype.limtype.diapason.intptrs.max < type2->casetype.limtype.diapason.intptrs.min ||
									type2->casetype.limtype.diapason.intptrs.max < type1->casetype.limtype.diapason.intptrs.min)
									Error(222, token);
				}
				else
					if (myconst)
					{//��������� �������� � ������� � ���������� ��� ����
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
//=============================================����� C��������==================================================//

/* ������ ����������� "��������" */
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
			//	//������ ����������� "����� ���������"
			//	SetDisjunct(followers, rpar, ptra); // rightparc,eolint 
			//	CallProc(ptra);
			//	Accept(rightparc);
			//	break;
			//case  assignc:
			//	//������ ����������� ""
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
	TYPEREC *exptype = NULL;
	Accept(whilesy);
	if (Belong(Symbol, st_express))
	{
		exptype = Expression(followers);
		if (!�ompatible(exptype, booltype))
			Error(135, token); /* ��� �������� ������ ���� boolean */
		if (Symbol == dosy)
			Accept(dosy);
		else {
			Accept(dosy);
			Skipto1(st_startstatement);
		}
	}
	Statement(followers);
}
/* ������ ����������� "�������� ��������" */
void IfStatement(unsigned *followers)
{
	TYPEREC *exptype = NULL;
	Accept(ifsy);
	if (Belong(Symbol, st_express))
	{
		exptype = Expression(followers);
		if (!�ompatible(exptype, booltype))
			Error(135, token); /* ��� �������� ������ ���� boolean */
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
/* ������ ����������� "���� � ������������" */
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
	if (!�ompatible(exptype, booltype))
		Error(135, token); /* ��� �������� ������ ���� boolean */
}
/* ������ ����������� "���� � ����������" */
void ForStatement(unsigned *followers)
{
	unsigned ptra[SET_SIZE],
		codevar;	/* ��� ���� ���������� */
	TYPEREC *exptype;	/* ��������� �� ���������� ���� ���������*/
	TYPEREC *vartype;	/* ��������� �� ���������� ���� ��������� �����*/
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
				if (!�ompatible(vartype, exptype))
					Error(145, token);
				if (Belong(Symbol, af_for1))// to ��� downto
				{
					if (Symbol == tosy)
						Accept(tosy);
					else Accept(downtosy);
					if (Belong(Symbol, st_express))
					{
						exptype = Expression(followers);
						if (!�ompatible(vartype, exptype))
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
/* ������������� ������� ��������� */
void GivingStatement(unsigned *followers)
{
	unsigned ptra[SET_SIZE];
	TYPEREC * ex1type = NULL,
		*ex2type = NULL;
	NODE * node;
	myconst = 0;
	SetDisjunct(af_assignment, followers, ptra);//af_assignment �������� assign
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
		//������ ����������� "����� ���������"
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
		//������ ����������� ""
		Accept(assignc);
		SetDisjunct(followers, comp_stat, ptra);
		ex2type = Expression(ptra);
		�ompatible(ex1type, ex2type);
		if (!Belong(Symbol, followers))
		{
			Error(6, token);
			Skipto1(followers);
		}
		break;
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
	/*�������� ��������� ������� ��������*/
	localscope = NULL;//���������� ���� ���� (localscope - ����������, ����������� ��������� ����� �������� ��������)

	Open_Scope(); //�������� �������� ����� ��� ��������� ������� ��������
	//boolean
	level = 0;
	booltype = NewType(ENUMS); // ������� ��� boolean, booltype - ������� ���������� �������� ����� ����������� ����
	SearchInTable((char*)"FALSE"); // ��������� � ������� ���� "false" I ����, ���������� ��������, ������� ����. �� �����)
	entry = NewIdent(hashresult, addrname, CONSTS); // NewIdent ��������� ������� � ���������� �� ��� ������, � ������ ���������� ������� ���������� NULL � ������ ������
	entry->idtype = booltype;
	//�������� ������ �������� � ����������� �������������� ���� (� ������ ������ boolean)
	(booltype->casetype).firstconst = newconst(addrname);
	SearchInTable((char*)"TRUE"); // ��������� � ������� ���� "true" I ����, ���������� ��������, ������� ����. �� �����)
	entry = NewIdent(hashresult, addrname, CONSTS); // NewIdent ��������� ������� � ���������� �� ��� ������, � ������ ���������� ������� ���������� NULL � ������ ������
	entry->idtype = booltype;
	(booltype->casetype).firstconst = newconst(addrname);

	//real
	realtype = NewType(SCALARS);
	//�har
	chartype = NewType(SCALARS);
	//integer
	inttype = NewType(SCALARS);
	//files
	texttype = NewType(FILES);
	(texttype->casetype).basetype = chartype;

	//������� � ������� ���� ��������� ����������� ��������������
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

	Open_Scope();//�������� ������� ����� ��� �������� ��������
	level++;
	if (!Belong(Symbol, st_all))  //st_all �������� programsy
	{
		Error(3, token); //��������� ����� PROGRAM
		Skipto2(st_all, followers);
	}
	if (Belong(Symbol, st_all))
	{
		Accept(programsy);
		//SetDisjunct(st_startstatement, st_all, ptra);
		if (!Belong(Symbol, idstarters))  //idstarters �������� idetn
		{
			Error(2, token); //������ ���� ���
			Skipto2(followers, st_startstatement);
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
		Close_Scope(); Close_Scope();
		StopGeneration();
		while (!feof(file_program)) NextCh();
	}
}

/* ��������� ������������� ��������� ��������������. ���� ������������� � �������� �������� ������������� �����
   ��� ������� � ������, �� ��������� - NULL, ����� - ������ �� ������� ������, ��������������� ����� �������������� */
NODE *NewIdent(unsigned hashfunc, char *addrname, int classused)
// hashfunc - ��������� ���-�������
// addrname - ������ �� ��� ��������������
// classused - ������ ������������� ��������������
{
	// C ������� node1 ���� ����� ��� ������� ������ ����
	// ���� �������� �� ��� ���, ���� node1 �� ����� NULL
	NODE *node1 = localscope->firstlocal;
	// node2 - ��������������� ���������.
	// �������� ������� ����� ���� � �������
	NODE *node2 = NULL;
	// ���������� d ����������, ����� �� ���������� node2
	// (right ��� left) ��������� �� node1
	// ���� d=1, �� ������ ���������, ���� d=-1, �� ����� ���������,
	// ���� d=0, �� ���� � ����� ������ ��� ���� ������
	int d = 2;
	/* ���� ����� ��� ������� ������ ����, ��� ����
	   � ����� ����� �� ������*/
	while (node1 != NULL && d != 0)
	{
		/* ���� �� ����� ���� � ����� ����� �� ������ */
		if (node1->idname == addrname)
			d = 0;
		else
		{
			node2 = node1;
			/* ������������� ���� ������� �� �������� ���-�������,
			   ����� - �� ������ ������ � ������ ��������������*/
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
	if (d == 0)	// ������������� � ������� ������ ��������� � ���,
				// ������� �� �������� �������� � ������
	{
		// ������ - ��� ������� ��������
		Error(101, token);
		// ���������� NULL
		return NULL;
	}
	else	// ����� ����� ��� ������� ����� �������
	{
		// ������� ����� �������
		node1 = (NODE*)malloc(sizeof(NODE));
		node1->hashvalue = hashfunc;
		node1->idname = addrname;
		node1->usingclass = classused;
		node1->left = node1->right = NULL;
		if (node2 == NULL)	// ��������� ��������� ��� �� ������ ������� ������
		{
			/* � ������ ��� ��� ������, ������ ��������
			   ������ ������� � ������ */
			localscope->firstlocal = node1;
		}
		else // ����� �� �������� ������ ������
		{
			if (d == 1)	// �������� ������� �� ������ ���������
				node2->right = node1;
			else	// �������� ������� �� ����� ���������
				node2->left = node1;
		}
		return node1;
	}
}
/* ��������� ����������� ��������� �������������� - ����� ���������������� ��� ������������� ���������.
   ��������� ������� - ��������� �� ������� ������, ��������������� ������� �������������� */
NODE *SearchIdent(unsigned hashfunc, char *addrname, unsigned *setofclass)
// hashfunc - �������� hash-������� ��� ��������������
// addrname - ������ �� ��� ��������������
// setofclass - ��������� ��������� �������� ������������� ��������������
{
	int usingtype;						/* ������� */
	SCOPE* templocalscope = localscope; /* ��� ������������ �� ����� �������� �������� */
	NODE *node1;						/* C ������� node1 �������� ����� ������ ��� ������� */
	NODE *node2 = NULL;					/* ���������� ��������� �� �������, ��� ������� ����� ����� �������� ���� � ������ ��� ����� �������������,
										� ������ ���������� ����� � ������� */
	NODE *topnode = NULL;	// ������������ ��������� �� �������, ��� ������� 
					// ����� ����� �������� ���� � ������ ��� ����� �������������
					// (������ ��� ������� ������� ��������)
	int topnodeinited = 0;	// ��������������� ���������� ����������; 
							// ����������, ���� ������������������� topnode ��� ���
	int foundname = 0;	// ���������� ����������; ����������, ����� ��
						// �� ������������� � ������ ��� ������ ��� ���

	while (templocalscope != NULL)	// �� ��� ���, ���� �� �� ����� 
									// �� ����� ����� �������� ��������
	{
		// ��������� �� ������� ������� ���������������
		node1 = templocalscope->firstlocal;
		while (node1 != NULL)	// �� ��� ���, ���� �� �� �����							
							// �� ����� ������� ���������������
							// (���� �� ������ ������ ����, 
							// �� ������������� ������ �� �������)
		{
			if (node1->idname == addrname)	// ����� ���� � ������ ��� ������
			{
				if (Belong(node1->usingclass, setofclass))
				{	// ����� ���� � ������ 
					// ��� �������� �������������
					if (foundname&&localscope != templocalscope)	Error(333, token);
					return node1;	// ����� ������ ��� ���� (���������� ���)
				}
				else	// ��������� ���� �� ����� �������������� � ������ ���������
				{
					foundname = 1;
					node2 = node1;	// ������������� ��������� 
					node1 = node1->left;	// ���������� ����� �����
				};
			}
			else	// �� ����� ���� � ������ ��� ������ (���� ������)
			{
				// ������������� ��������� node2 �� ������� �������
				node2 = node1;
				/*  ���� ����������� ������� �� �������� ���-�������,
					����� - �� ������ ������ � ������ ��������������	*/
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
		/*	���� �� ������ � ��� ����� ����, �� ��� ��������, ��� �� �� �����
			������ ��� ���� � ������� �������� templocalscape
			(����� �� �� ����� �� �������, ��������� ������ ����)	*/
			// ��������� � ������� ��������, ���������� ������ ��� ���������������
		templocalscope = templocalscope->enclosingscope;
		if (!topnodeinited)	// ���� �� ��� �� ����� ������� ��� ������� �����
							// (�� ����������� ������ ��� ������� ������� ��������)
		{//���������� ��������� � ��������������� ��� ��������!!!
			// ���������� ��������� �������
			topnode = node2;
			// ��������, ��� �� ���������������� topnode
			topnodeinited = 1;
		};
	};
	/*	���� �� ������ � ��� ����� ����, �� ��� ��������, ��� �� �� �����
		������ ��� ���� ������ �� ���� �������� ��������
		(����� �� �� ����� �� �������, ��������� ������ ����)	*/
	if (foundname)	// ���� �� ����� ������������� � ������ ������
		Error(103, token);	// ������: ��� �� ����������� ���������������� ������!
	else
	{
		Error(104, token);	// ������: ��� �� �������!
//else
// �� ���� �������� �������������

		// ���� ������ ������ �������������
		// ������������� � ��������� ���������
		// (��� ����� ������ � ������)

		// ��������� node1 ��� �������� ������ ����
		node1 = (NODE*)malloc(sizeof(NODE));
		// �������������� ���� ������ ����
		node1->hashvalue = hashfunc;
		node1->idname = addrname;
		node1->idtype = NULL;	// ������� ��� ������ �������
		node1->left = node1->right = NULL;
		// ��������� ��������� ������� � ������� ���������������
		if (topnode == NULL)	// ���� � ������ ������� �������� 
							// ��� ��� ���������������
		{
			// ��������� ���� ������� ���������� �������
			localscope->firstlocal = node1;
			topnode = node1;
		}
		else	// �������������� � ������ ������� �������� ���� 
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
		//*set_VARCONFUNCS,	/* ���. ������� ������������� - VARS, CONSTS, FUNCS */=
		//*set_VARS,	/* ���������� ������ ������������� - VARS		*/=
		//*set_TYPES, 	/* ���������� ������ ������������� - TYPES 		*/=
		//*set_CONSTS,	/* ���������� ������ ������������� - CONSTS 		*/=
		//*set_TYCON,     /* ���������� ������� ������������� - TYPES,CONSTS	*/=
		//*set_FUNCS,     /* ���������� ������ ������������� - FUNCS              */
		//*set_PROCS,     /* ���������� ������ ������������� - PROCS              */
		//*set_FUNPR,     /* ���������� ������� ������������� - FUNCS,PROCS       */=
		//*set_VARFUNPR;  /* ���������� ������� ������������� - VARS,FUNCS,PROCS  */=	
		if (setofclass == set_VARS) //����������
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
		// ���������� ��������� ����������� ���� � ������ ����
	}
	return topnode;
}
/*�������� � ������������� ������ ����������� ���� � �������(������� �� ���� ����)  �����*/
TYPEREC *NewType(int tcode/*��� ����*/) {
	struct typerec *newd;									/* ��������� �� ���������� ���� */
	newd = (struct typerec*)malloc(sizeof(struct typerec)); /* ��������� ������ ��� �������� � ������ ������������ ���� */
	newd->typecode = tcode;
	newd->next = localscope->typechain;						/* �������� �� ������������� �������� ����� � ������ ������������ */
	switch (newd->typecode)									/* ��������� ����������� � ����������� �� ��� ���� */
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
	localscope->typechain = newd; // ������������� ��������� �������� �������� (� ������������� � ���� ������� �����) � ������� ��������
	return(newd);
}
/*�������� �������� ����� ��� ������� ������� ��������*/
void Open_Scope() {
	SCOPE *newscope; // ��������� �� ����� ������� �����
	newscope = (SCOPE*)malloc(sizeof(SCOPE));// ��������� ������ ��� ������� �����
	newscope->firstlocal = NULL; // ��������� ��������� �� ������� ���������������
	newscope->typechain = NULL;// ��������� ��������� �� ������� ����� ������� ��������
	newscope->labelpointer = NULL; // �������� ��������� �� ������� ����� ������� ��������
	newscope->enclosingscope = localscope; //������������ �������� �������� � �����. (�� ������ �������� ��������� ������� �������� localscope=NULL)
	localscope = newscope; // ��������� ������� �����
}
/*�������� ������� ��������������� ������� ������� �������� */
void Dispose_Ids(struct treenode *firstlocal) {//�������� 
	if (firstlocal != NULL) {
		Dispose_Ids(firstlocal->left);
		Dispose_Ids(firstlocal->right);
		free(firstlocal);
	}
}
/* �������� ������� ����� ������� ������� ��������*/
void Dispose_Types(TYPEREC *typechain) {
	TYPEREC* old;
	while (typechain != NULL) {
		old = typechain;
		typechain = typechain->next;
		free(old);
	}
}
/* �������� ������� ������� �������� */
void Close_Scope()
{
	SCOPE *oldscope;							/* ��������� �� ��������� ������ ����� */
	oldscope = localscope;
	localscope = localscope->enclosingscope;	/* ������������ ������� ����� �� ��������� ������� ����� */
	//������� ����� ���������� ��������
	Dispose_Ids(oldscope->firstlocal);			/* �������� �� */
	Dispose_Types(oldscope->typechain);			/* �������� �� */
	free(oldscope);								/* ������������� �������� ������� */
}

/*============================================= ������ � �������� �̨� =============================================*/
/*���-������� ��� ���������� �������� � ������� ��� */
unsigned HashFunction(char *name)
{
	int i = 0, result = 0;
	while (name[i] != 0) result += name[i++] | i;
	return(abs(result % 997));
}
/* ����� �������� ����� � ������� ���� */
void SearchInTable(char *name)/* �������� �������� ����� */
/* ���������� - ������� ����������:
 addrname - ����� �������� ����� � ������� ����,
 hashresult - ��������� hash-������� �� �������� �����	*/
{
	int ind;	/* ���� ind ����� 0, ������, ������� ��� �� ��������� � ��������� ������ � ������ */
	hashresult = HashFunction(name);
	if (nametable[hashresult] == NULL) 	/* ���������� ������ ������     */
						/* ��� ��������� �������� 	*/
	{
		ptrstr = (str*)malloc(sizeof(str)); /* ��������� ������� ������ */
					   /* ��� ��������� str            */
		nametable[hashresult] = ptrstr;
		(*ptrstr).firstfree = 0;
		(*ptrstr).next = NULL;
		WriteName(name);/* ��������� �������� ����� � ������� ����      */
		return;			/* ���������� ����� ����� � �������     */
	}
	else {
		ptrstr = nametable[hashresult];   /* ��������; ���� ��� � ������� */
		loccount = 0;
	search:
		do {/* ��������� ��������� ���������� ��� ���������        */
			/* �������� ����� � ��������� ������ � �������:         */
			ind = 1;chcount = 0;
			addrname = &((*ptrstr).namelist[loccount]);
			/* (����� ��������������� �� ������ ������ �����)       */
			while ((addrname[chcount] != '\0') && (ind == 1)
				&& (lname >= chcount))
			{
				if (addrname[chcount] != name[chcount])
					ind = 0; /* ����� �� ���������		*/
				else { loccount++;chcount++; }
			}
			if (ind == 1)
			{
				return;
			}		/* ��� ������� ������� � ���������� ����� � ������:				*/
			while ((*ptrstr).namelist[loccount++] != '\0');
		} while (loccount < (*ptrstr).firstfree); /* ���� �� �������� ������ ��������� ������
													(� ������, ����� �� ����� ����� MAXNUM, ��� ������ � ������ */
		if ((*ptrstr).next != NULL) /* ���� ���������� ��������� ������ � ��� �� ��������� hash-�������    */
		{
			ptrstr = (*ptrstr).next; /* ...�� ��������� �� ���       */
			loccount = 0;
			goto search;            /* ...� ���������� �����        */
		}
		else    /* ����� �������, ��� �� �������			*/
		{
			if (lname < MAXNUM - (*ptrstr).firstfree)
				/* (���� ���� ��� ��������� � ������� ���� ������...)   */
			{
				WriteName(name);/* ...�� �� ��� ���� � �������!*/
				return;
			}
			else {/* ����� ��� ������ ����� ���, �������� ����� ������ */
				(*ptrstr).next = (str*)malloc(sizeof(str));
				ptrstr = (*ptrstr).next;
				(*ptrstr).next = NULL;
				(*ptrstr).firstfree = 0;
				WriteName(name);/* ������� ��� � ����� ������	*/
				return;
			}
		}
	}
}
/* ��������� �������� ����� � ������� ���� */
void WriteName(char name[])	/* ��������� ���	*/
/* ���������� �� SearchInTable(), ��� ������ ��������� ����� �� ��� */
{
	addrname = &((*ptrstr).namelist[(*ptrstr).firstfree]);
	/* ���������� ����� ���������� ����� 				*/
	for (chcount = 0, loccount = (*ptrstr).firstfree;name[chcount] != '\0';
		chcount++, loccount++)
	{
		(*ptrstr).namelist[loccount] = name[chcount];
	}
	(*ptrstr).namelist[loccount++] = '\0';
	(*ptrstr).firstfree = loccount;
}
/*========================================== ����� ������ � �������� �̨� ==========================================*/