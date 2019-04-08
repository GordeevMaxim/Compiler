#include "pch.h"
#include "DECKEY.h"
#include "DECW.h"
#include <stdlib.h>


//======================================��������� ������� ���������(������)======================================//
void Programme(unsigned *followers);
void Block(unsigned *followers);
void TypePart(unsigned *followers);
TYPEREC *Type(unsigned *followers);
TYPEREC *SimpleType(unsigned *followers);
TYPEREC *LimitedType(unsigned *followers);
TYPEREC *PerechislType(unsigned *followers);
TYPEREC *NameType();
void Constant();
void VarPart(unsigned *followers);
void VarDeclaration(unsigned *followers);
void ProcFuncPart(unsigned *followers);
void Statement(unsigned *followers);
void One_Type(unsigned *followers);
void Un_Int();
void Un_Real();
void Const_Char();
void DeclarationProc(unsigned *followers);
void DeclarationProcFun(unsigned *followers);
void FormalParameters(unsigned *followers, unsigned *fol);
void HeaderProc(unsigned *followers);
void BeginStatement(unsigned *followers);
TYPEREC *Factor(unsigned *followers);
TYPEREC *Term(unsigned *followers);
TYPEREC *SimpleExpression(unsigned *followers);
TYPEREC *Expression(unsigned *followers);

bool �ompatible(TYPEREC* type1, TYPEREC * type2);
void NameFunc(unsigned *followers);
void CallProc(unsigned *followers);
void GivingStatement(unsigned *followers);
void IfStatement(unsigned *followers);
void WhileStatement(unsigned *followers);
void ForStatement(unsigned *followers);
void RepeatStatement(unsigned *followers);
/*======================================��������� ������� ���������(�����)======================================*/



/*=====================================��������� ������� ���������(������)======================================*/
NODE *NewIdent(unsigned hashfunc, char *addrname, int classused);
NODE *SearchIdent(unsigned hashfunc, char *addrname, unsigned *setofclass);
TYPEREC *NewType(int tcode/*��� ����*/);
void Open_Scope();
void Dispose_Ids(struct treenode *firstlocal);
void Dispose_Types(TYPEREC *typechain);
void Close_Scope();
/*======================================��������� ������� ���������(�����)======================================*/



/*=========================================��������� ������� ��(������)=========================================*/
unsigned HashFunction(char *name);
/* ����� �������� ����� � ������� ���� */
void SearchInTable(char *name);
/* ��������� �������� ����� � ������� ���� */
void WriteName(char name[]);
/*==========================================��������� ������� ��(�����)=========================================*/