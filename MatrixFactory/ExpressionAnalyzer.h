#pragma once
#include "Math.h"

enum optrs
{
	add = 0, subt, mult, divide, power, null
};

enum char_type
{
	sign=0, optr, brac_left, brac_right, equ_sign, opnd, func, matrix, unident
};

//OperatorName

#define PI 3.14159265358979323846264
#define EULER_NUM 2.7182818284590452353602874713526624977572470936999595749 



//Functions.
Math* _calculate(Math* a, Math* b, optrs optr);
Math* Calculate(string expr);
void ClearOpndStack(stack<Math*>* stkopnd);
char_type AnalyzeChar(char chr);

bool Analyze_Change(int AnalyzeResult, bool initialize);
Matrix InputMatrix(int row, int column);




