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

//-----------------------------------------œ–Œ“Œ“»œ€ ‘”Õ ÷»…(Õ¿◊¿ÀŒ)--------------------------------------------//
void Programme();
void Block();
void TypePart();
void Type();
void Constant();
void ArrayType();
void VarPart();
void VarDeclaration();
void ConstPart();
void ProcFuncPart();
void Statement();
void SimpleType();
void LimitedType();
void CompoundType();
void ReferenceType();
void PerechislType();
void NameType();
void One_Type();
void Un_Int();
void BazisType();
void One_Const();
void Un_Real();
void NameConstant();
void DeclarationFun();
void DeclarationProc();
void DeclarationProcFun();
void FormalParameters();
void HeaderFun();
void HeaderProc();
void Variable();
void BeginStatement();
void Factor();
void Term();
void SimpleExpression();
void Expression();
void NameFunc();
void CallProc();
void GivingStatement();
void CaseStatement();
void IfStatement();
void WhileStatement();
void ForStatement();
void RepeatStatement();
//-----------------------------------------œ–Œ“Œ“»œ€ ‘”Õ ÷»…( ŒÕ≈÷)---------------------------------------------//
