#pragma once
#include "fraction.h"
#include "Matrix.h"
using namespace std;
//ExpressionAnalyzer.h

#ifndef _EXPR_ANALYZER_H_
#define _EXPR_ANALYZER_H_
//TypeName

#define OPND 0//operand.
#define SIGN 1//+-
#define OPTR 2//*/^
#define FUNC 3//sin,cos
#define UNIDENT 4
#define BRAC_LEFT 5
#define BRAC_RIGHT 6
#define MATRIX 7
#define EQU_SIGN 8
#define UNIDENT_ERROR "Unidentified Syntax!"
#define SYNTAX_ERROR "Syntax Error!"
#define BRAC_ERROR "Brackets don`t match!"
#define UNIDENT_FUNC_ERROR "Unidentified function Error!"
#define UNKNOWN_MAT_ERROR "Undefined Matrix!"

//OperatorName
#define ADD 0
#define SUBTRACT 1
#define MULT 2
#define DIV 3
#define POW 4//^
#define POW_EACH 5//^^


#define PI 3.14159265358979323846264
#define EULER_NUM 2.7182818284590452353602874713526624977572470936999595749 
//Calculator code here!
void DeleteSpaces(string& expr);
int AnalyzeChar(char chr);
bool Analyze_Change(int AnalyzeResult, bool initialize);
fraction* Calculate(string expr);


#endif