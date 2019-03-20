#include "pch.h"
#include "DECKEY.h"
#include <stdlib.h>
#include "ModulRW.h"
#include "DECW.h"
//#include "iodefs.h"
//#include "Types.h"
//#include "Typet.h"

extern unsigned Symbol;
extern char ch;
extern void Accept(unsigned Symbolexpected);
extern void NextSym();
extern char NextCh();
extern bool Belong(unsigned Symbolexpected, unsigned *set);
extern textposition token;

//-----------------------------------------œ–Œ“Œ“»œ€ ‘”Õ ÷»…(Õ¿◊¿ÀŒ)--------------------------------------------//
void Programme(unsigned *followers);
void Block(unsigned *followers);
void TypePart();
void Type(unsigned *followers);
void Constant();
void ArrayType(unsigned *followers);
void VarPart(unsigned *followers);
void VarDeclaration(unsigned *followers, unsigned *fol);
void ConstPart();
void ProcFuncPart(unsigned *followers);
void Statement(unsigned *followers);
void AssignStatement(unsigned *followers);
void SimpleType(unsigned *followers);
void LimitedType();
void CompoundType(unsigned *followers);
void ReferenceType();
void PerechislType();
void NameType();
void One_Type(unsigned *followers);
void Un_Int();
void BazisType(unsigned *followers);
void One_Const();
void Un_Real();
void NameConstant();
void DeclarationFun(unsigned *followers);
void DeclarationProc(unsigned *followers);
void DeclarationProcFun(unsigned *followers);
void FormalParameters(unsigned *followers, unsigned *fol);
void HeaderFun(unsigned *followers);
void HeaderProc(unsigned *followers);
//void Variable();
void BeginStatement(unsigned *followers);
void Factor(unsigned *followers);
void Term(unsigned *followers);
void SimpleExpression(unsigned *followers);
void Expression(unsigned *followers);
void NameFunc(unsigned *followers);
void CallProc(unsigned *followers);
void GivingStatement(unsigned *followers);
void CaseStatement(unsigned *followers);
void IfStatement(unsigned *followers);
void WhileStatement(unsigned *followers);
void ForStatement(unsigned *followers);
void RepeatStatement(unsigned *followers);
//-----------------------------------------œ–Œ“Œ“»œ€ ‘”Õ ÷»…( ŒÕ≈÷)---------------------------------------------//
