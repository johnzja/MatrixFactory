#include "stdafx.h"
#include "ExpressionAnalyzer.h"

extern const char* _Object_Pointer_Corrupted;
extern const char* _Test_Debug_Error;
extern const char* _Operator_Not_Defined;
extern const char* _Convertion_To_Fraction_Failure;
extern const char* _Convertion_To_Op_Failure;
extern const char* _Syntax_Error;
extern const char* _Function_Unidentified;
extern const char* _Math_Error;
extern const char* _Brac_Error;
extern const char* _Matrix_Size_Error;
extern const char* _Invalid_Input;

extern const Int Int_one;
extern const fraction frc_zero;

#define DECIMAL_LENGTH 5
using namespace std;

vector<Math*> matArr;
vector<string> matNameArr;
vector<Function*> FuncArr;

Math* prev_ans;
bool(*Clear)(int AnalyzeResult, bool initialize);


Math* _calculate(Math* a, Math* b, optrs optr)// a & b should be deleted?!
{
	data_type aType = a->GetType();
	data_type bType = b->GetType();
	Math* ans = nullptr;
	Math* tmp = nullptr;

	try
	{
		switch (aType)
		{
		case EXC:throw Exceptions(_Object_Pointer_Corrupted);
		case FUNC:
			//Further code here. Function Applying& defining.
			break;


		case INT://a is an Int.
			if (bType == INT)//INT optr INT
			{
				switch (optr)
				{
				case add:
					ans = new Int((*dynamic_cast<Int*>(a)) + (*dynamic_cast<Int*>(b))); break;

				case subt:
					ans = new Int((*dynamic_cast<Int*>(a)) - (*dynamic_cast<Int*>(b))); break;

				case mult:
					ans = new Int((*dynamic_cast<Int*>(a)) * (*dynamic_cast<Int*>(b))); break;

				case  divide:
					ans = new fraction((*dynamic_cast<Int*>(a)), (*dynamic_cast<Int*>(b))); break;
				}
			}
			else if (bType == FRC)//INT optr FRC
			{
				tmp = new fraction(*dynamic_cast<Int*>(a));// now tmp is  fraction*.
				delete a;//Release the previous Int (converted into fraction.)
				a = tmp;
				switch (optr)
				{
				case add:
					ans = new fraction(*(dynamic_cast<fraction*>(a)) + *dynamic_cast<fraction*>(b)); break;

				case subt:
					ans = new fraction(*(dynamic_cast<fraction*>(a)) - *dynamic_cast<fraction*>(b)); break;

				case mult:
					ans = new fraction(*(dynamic_cast<fraction*>(a)) * (*dynamic_cast<fraction*>(b))); break;

				case divide:
					ans = new fraction(*(dynamic_cast<fraction*>(a)) / (*dynamic_cast<fraction*>(b))); break;
				}
			}
			else if (bType == DBL)//INT optr DBL
			{
				tmp = new Double(*dynamic_cast<Int*>(a), Int_one);
				delete a;
				a = tmp;

				switch (optr)
				{
				case add:
					ans = new Double(*dynamic_cast<Double*>(a) + *dynamic_cast<Double*>(b)); break;

				case subt:
					ans = new Double(*dynamic_cast<Double*>(a) - *dynamic_cast<Double*>(b)); break;

				case mult:
					ans = new Double(*dynamic_cast<Double*>(a) * *dynamic_cast<Double*>(b)); break;

				case divide:
					ans = new Double(*dynamic_cast<Double*>(a) / *dynamic_cast<Double*>(b)); break;

				case power:
					ans = new Double(pow(*dynamic_cast<Double*>(a), *dynamic_cast<Double*>(b))); break;
				}
			}
			else if (bType == MAT)//INT optr MAT
			{
				tmp = new fraction(*dynamic_cast<Int*>(a));// now tmp is  fraction*.
				delete a;//Release the previous Int (converted into fraction.)
				a = tmp;

				switch (optr)
				{
				case mult:
					ans = new Matrix(*dynamic_cast<fraction*>(a) * *dynamic_cast<Matrix*>(b)); break;

				case divide:
					ans = new Matrix(*dynamic_cast<fraction*>(a) * Ginverse(*dynamic_cast<Matrix*>(b))); break;
				}
			}
			break;

		case FRC:
			if (bType == FRC)//FRC optr FRC
			{
				switch (optr)
				{
				case add:
					ans = new fraction((*dynamic_cast<fraction*>(a)) + (*dynamic_cast<fraction*>(b))); break;

				case subt:
					ans = new fraction((*dynamic_cast<fraction*>(a)) - (*dynamic_cast<fraction*>(b))); break;

				case mult:
					ans = new fraction((*dynamic_cast<fraction*>(a)) * (*dynamic_cast<fraction*>(b))); break;

				case divide:
					ans = new fraction((*dynamic_cast<fraction*>(a)) / (*dynamic_cast<fraction*>(b))); break;

				case power:
					ans = new Double(pow(dynamic_cast<fraction*>(a)->GetValueD(), dynamic_cast<fraction*>(b)->GetValueD())); break;

				default:
					break;

				}
			}
			else if (bType == INT)//FRC optr INT
			{
				if (optr != power)
				{
					tmp = new fraction(*dynamic_cast<Int*>(b));//Convert into fraction.
					delete (Int*)b;
					b = tmp;

					switch (optr)
					{
					case add:
						ans = new fraction(*dynamic_cast<fraction*>(a) + *dynamic_cast<fraction*>(b)); break;

					case subt:
						ans = new fraction(*dynamic_cast<fraction*>(a) - *dynamic_cast<fraction*>(b)); break;

					case mult:
						ans = new fraction(*dynamic_cast<fraction*>(a) * *dynamic_cast<fraction*>(b)); break;

					case divide:
						ans = new fraction(*dynamic_cast<fraction*>(a) / *dynamic_cast<fraction*>(b)); break;

					default:
						break;

					}
				}
				else//optr==power
				{
					ans = new fraction(pow(*dynamic_cast<fraction*>(a), int(*(dynamic_cast<Int*>(b)))));
				}

			}
			else if (bType == DBL)//FRC optr DBL
			{
				tmp = new Double(dynamic_cast<fraction*>(a)->GetValueD());//Convert into Double.
				delete a;
				a = tmp;

				switch (optr)
				{
				case add:
					ans = new Double(*dynamic_cast<Double*>(a) + *dynamic_cast<Double*>(b)); break;

				case subt:
					ans = new Double(*dynamic_cast<Double*>(a) - *dynamic_cast<Double*>(b)); break;

				case mult:
					ans = new Double(*dynamic_cast<Double*>(a) * *dynamic_cast<Double*>(b)); break;

				case divide:
					ans = new Double(*dynamic_cast<Double*>(a) / *dynamic_cast<Double*>(b)); break;

				case power:
					ans = new Double(pow(*dynamic_cast<Double*>(a), *dynamic_cast<Double*>(b))); break;

				default:
					//Code here!
					break;
				}

			}
			else if (bType == MAT)//FRC optr MAT
			{
				switch (optr)
				{
				case mult:
					ans = new Matrix(*dynamic_cast<fraction*>(a) * *dynamic_cast<Matrix*>(b)); break;

				case divide:
					ans = new Matrix(*dynamic_cast<fraction*>(a) * Ginverse(*dynamic_cast<Matrix*>(b))); break;
				}
			}

			break;

		case DBL://DBL optr DBL
			if (bType == DBL)
			{
				switch (optr)
				{
				case add:
					ans = new Double(*dynamic_cast<Double*>(a) + *dynamic_cast<Double*>(b)); break;

				case subt:
					ans = new Double(*dynamic_cast<Double*>(a) - *dynamic_cast<Double*>(b)); break;

				case mult:
					ans = new Double(*dynamic_cast<Double*>(a) * *dynamic_cast<Double*>(b)); break;

				case divide:
					ans = new Double(*dynamic_cast<Double*>(a) / *dynamic_cast<Double*>(b)); break;

				case power:
					ans = new Double(pow(*dynamic_cast<Double*>(a), *dynamic_cast<Double*>(b))); break;

				default:

					break;
				}
			}
			else if (bType == INT)//DBL optr INT
			{
				if (optr != power)
				{
					tmp = new Double(*dynamic_cast<Int*>(b), Int_one);
					delete b;//Virtual Destructor called. No need to convert the type of pointer into Int* .
					b = tmp; // Replace b pointer.

					switch (optr)
					{
					case add:
						ans = new Double(*dynamic_cast<Double*>(a) + *dynamic_cast<Double*>(b)); break;

					case subt:
						ans = new Double(*dynamic_cast<Double*>(a) - *dynamic_cast<Double*>(b)); break;

					case mult:
						ans = new Double(*dynamic_cast<Double*>(a) * *dynamic_cast<Double*>(b)); break;

					case divide:
						ans = new Double(*dynamic_cast<Double*>(a) / *dynamic_cast<Double*>(b)); break;

					default:

						break;
					}
				}
				else//optr=power
				{
					ans = new Double(pow(*dynamic_cast<Double*>(a), int(*dynamic_cast<Int*>(b))));
				}
			}
			else if (bType == MAT)//DBL optr MAT
			{
				tmp = new fraction(*dynamic_cast<Double*>(a));
				delete a;
				a = tmp;

				switch (optr)
				{
				case mult:
					ans = new Matrix(*dynamic_cast<fraction*>(a) * *dynamic_cast<Matrix*>(b)); break;

				case divide:
					ans = new Matrix(*dynamic_cast<fraction*>(a) * Ginverse(*dynamic_cast<Matrix*>(b))); break;

				default:
					//Error code here.
					break;
				}
			}

			break;

		case MAT://Type of a is mat.
			if (bType == MAT)//MAT optr MAT
			{
				switch (optr)
				{
				case add:
					ans = new Matrix(*dynamic_cast<Matrix*>(a) + *dynamic_cast<Matrix*>(b)); break;

				case subt:
					ans = new Matrix(*dynamic_cast<Matrix*>(a) - *dynamic_cast<Matrix*>(b)); break;

				case mult:
					ans = new Matrix(*dynamic_cast<Matrix*>(a) * *dynamic_cast<Matrix*>(b)); break;

				case divide:
					ans = new Matrix(*dynamic_cast<Matrix*>(a) * Ginverse(*dynamic_cast<Matrix*>(b))); break;

				default:

					break;
				}
			}
			else if(bType==INT)//MAT optr INT
			{
				if (optr == power)
				{
					ans = new Matrix(pow(*dynamic_cast<Matrix*>(a), int(*dynamic_cast<Int*>(b))));
				}
			}

			break;

		default:
			throw Exceptions(_Test_Debug_Error);
		}

	}
	catch (Exceptions E)
	{
		delete a;
		delete b;
		throw;
	}
	catch (...)
	{
		delete a;
		delete b;
		throw;
		//Unknown Errors.
	}

	delete a;
	delete b;
	if (ans == nullptr)
	{
		throw Exceptions(_Operator_Not_Defined);
	}
	return ans;
}

//Decision made: copy the previous MatrixFactory code.

///////////////////////////////////////////////////////////////////////////////

Math* DeepCopy(Math* src)
{
	Math* ans = nullptr;
	switch (src->GetType())
	{
	case INT:
		ans = new Int(*(dynamic_cast<Int*>(src))); break;

	case DBL:
		ans = new Double(*(dynamic_cast<Double*>(src))); break;

	case FRC:
		ans = new fraction(*(dynamic_cast<fraction*>(src))); break;

	case MAT:
		ans = new Matrix(*(dynamic_cast<Matrix*>(src))); break;

	default:

		//Further code here!
		break;
	}
	if (ans == nullptr)
	{
		throw Exceptions(_Test_Debug_Error);
	}
	else
	{
		return ans;
	}
}

void DeleteSpaces(string& expr)//Delete the spaces in an expression.
{
	unsigned int i = 0;
	while (i<expr.length())
	{
		if (expr[i] == ' ')
		{
			expr.erase(i, 1);
		}
		else
		{
			i++;
		}
	}
	return;
}

//Check if the brackets match.
bool BracMatch(const string& cmd)
{
	int L = cmd.length();
	int count = 0;
	for (register int i = 0;i < L;i++)
	{
		switch (cmd[i])
		{
		case '(':
			count++;
			break;
		case ')':
			count--;
			break;
		default:
			break;
		}
		if (count < 0)return false;

	}
	if (count != 0)return false;
	return true;
}

char_type AnalyzeChar(char chr)
{
	switch (chr)
	{
	case '+':
	case '-':
		return sign;		break;
	case '/':
	case '*':
	case '^':
		return optr;		break;
	case '(':
		return brac_left;	break;

	case ')':
		return brac_right;
		break;
	case '=':
		return equ_sign;
		break;
	default:
		break;
	}
	if ((chr >= 48 && chr <= 57) || chr == 46)//A number. Comma included.
	{
		return opnd;
	}
	else if (chr >= 97 && chr <= 122)//It is an alphabet.(chr >= 65 && chr <= 90)
	{
		return func;
	}
	else if (chr >= 65 && chr <= 90)
	{
		return matrix;
	}
	else
	{
		return unident;
	}
}

bool Analyze_Change(int AnalyzeResult, bool initialize = false)
{
	static int prev_Result;//Initial value=0
	static bool FirstTime = true;

	if (initialize)
	{
		prev_Result = 0;
		FirstTime = true;
		return false;
	}

	if (FirstTime)
	{
		FirstTime = false;
		prev_Result = AnalyzeResult;
		return false;
	}

	if (prev_Result == AnalyzeResult)
	{
		prev_Result = AnalyzeResult;
		return false;
	}

	prev_Result = AnalyzeResult;
	return true;
}

inline char_type Type(const string& str)
{
	return AnalyzeChar(str.back());
}

static Math* Convert_to_Fraction(const string& str)///Fraction? Two ints. //This can be modified to Math*.
{

	int L = str.length();
	bool isNegative = false;
	bool isDecimal = false;
	int DecimalCount = 0;
	string Result_AbsValue;

	for (int i = 0;i < L;i++)
	{
		if (AnalyzeChar(str[i]) != opnd && AnalyzeChar(str[i]) != sign)//not sign nor operand.
		{
			throw Exceptions(_Convertion_To_Fraction_Failure);
		}
	
		if (str[i] == '+')continue;
		else
		{
			if (str[i] == '-') { isNegative = !isNegative; continue; }
			else
			{
				if (str[i] == '.')
				{
					isDecimal = true;
					DecimalCount++;
				}
			}
		}
		Result_AbsValue += str[i];
	}
	//Now start converting.
	if ((DecimalCount > 1) || (Result_AbsValue.length() == 0) || DecimalCount == L)//Three abnormal conditions.
		throw Exceptions(_Convertion_To_Fraction_Failure);


	if (!isDecimal)
	{
		if (isNegative)
		{
			return new Int(-Int(Result_AbsValue));
		}
		else
		{
			return new Int(Result_AbsValue);
		}
	}
	else//Decimal=true, using fraction.
	{
		fraction* ans;
		int Result_Length = Result_AbsValue.length();
		int Decimal_Position;

		Decimal_Position = Result_AbsValue.find('.');
		int Fractional_Part_Length = Result_Length - Decimal_Position - 1;//ok.
		Result_AbsValue.erase(Decimal_Position, 1);

		Int NUM(Result_AbsValue);
		Int DENOM = pow(Int(10), Fractional_Part_Length);

		if (Result_AbsValue.length() <= DECIMAL_LENGTH)
		{
			//Convert to fraction form.
			if (isNegative)
			{
				ans = new fraction(-NUM, DENOM);
			}
			else
			{
				ans = new fraction(NUM, DENOM);
			}
		}
		else
		{
			if (isNegative)
			{
				ans = new fraction(Double(-NUM, DENOM));
			}
			else
			{
				ans = new fraction(Double(NUM, DENOM));
			}
		}
		return ans;
	}
}

static optrs Convert_to_Operator(const string& str)
{
	int L = str.length();
	bool isNegative = false;

	if (L == 0)
	{
		throw Exceptions(_Convertion_To_Op_Failure);
	}
	if (L == 1)
	{
		switch (str[0])
		{
		case'+':
			return add;
			break;
		case'-':
			return subt;
			break;
		case '*':
			return mult;
			break;
		case '/':
			return divide;
			break;
		case '^':
			return power;
			break;
		default:
			throw Exceptions(_Convertion_To_Op_Failure);
		}
	}
	else//L>=2:only one circumstance.
	{
		int Analyze_Result = 0;
		for (int i = 0;i < L;i++)
		{
			Analyze_Result = AnalyzeChar(str[i]);
			if (Analyze_Result != sign)
			{
				throw Exceptions(_Convertion_To_Op_Failure);
			}
			else
			{
				if (str[i] == '-')isNegative = !isNegative;
			}
		}

		if (!isNegative)
		{
			return add;
		}
		else
		{
			return subt;
		}

	}
}

int Priority(optrs _optr)
{
	switch (_optr)
	{
	case add:
	case subt:
		return 1;
		break;
	case mult:
	case divide:
		return 2;
		break;
	case power:
		return 3;
		break;
	default:
		return 0;
		break;
	}
}

void ClearOpndStack(stack<Math*>* stkopnd)
{
	while (!stkopnd->empty())
	{
		delete stkopnd->top();
		stkopnd->pop();
	}
}

void CalculateOnce(stack<Math*>* stkopnd, stack<optrs>* stkoptr)
{
	Math* B_ptr = stkopnd->top();stkopnd->pop();//Ensure the uniqueness of the pointer. Don`t forget to delete it.
	if (stkopnd->empty())
	{
		optrs temp_optr = stkoptr->top();stkoptr->pop();
		switch (B_ptr->GetType())
		{
		case FRC://B is a fraction
			if (temp_optr == add) { stkopnd->push(B_ptr);return; }
			else if (temp_optr == subt)
			{
				fraction* Negative_B_ptr = new fraction(-*dynamic_cast<fraction*>(B_ptr));
				delete B_ptr;
				stkopnd->push(Negative_B_ptr);
				return;
			}
			else
			{
				delete B_ptr;
				throw Exceptions(_Syntax_Error);
			}
			break;

		case MAT://Matrix
			if (temp_optr == add) { stkopnd->push(B_ptr);return; }//What does this line mean?
			else if (temp_optr == subt)
			{
				Matrix* Negative_B_ptr = new Matrix(-*(dynamic_cast<Matrix*>(B_ptr)));
				delete B_ptr;
				stkopnd->push(Negative_B_ptr);
				return;
			}
			else
			{
				delete B_ptr;
				throw Exceptions(_Syntax_Error);
			}

		case INT:
			if (temp_optr == add) { stkopnd->push(B_ptr);return; }//What does this line mean?
			else if (temp_optr == subt)
			{
				Int* Negative_B_ptr = new Int(-*(dynamic_cast<Int*>(B_ptr)));
				delete B_ptr;
				stkopnd->push(Negative_B_ptr);
				return;
			}
			else
			{
				delete B_ptr;
				throw Exceptions(_Syntax_Error);
			}

		case DBL:
			if (temp_optr == add) { stkopnd->push(B_ptr);return; }//What does this line mean?
			else if (temp_optr == subt)
			{
				Double* Negative_B_ptr = new Double(-*(dynamic_cast<Double*>(B_ptr)));
				delete B_ptr;
				stkopnd->push(Negative_B_ptr);
				return;
			}
			else
			{
				delete B_ptr;
				throw Exceptions(_Syntax_Error);
			}

		default:
			delete B_ptr;
			throw Exceptions(_Operator_Not_Defined);
		}

	}
	data_type B_Type = B_ptr->GetType();
	Math* A_ptr = stkopnd->top();stkopnd->pop();
	data_type A_Type = A_ptr->GetType();
	//Two operands. One operator.
	optrs optr = stkoptr->top();//Get the operator.
	stkopnd->push(_calculate(A_ptr, B_ptr, optr));//Automatically apply the operation.
	stkoptr->pop();
	return;
}

Math* Func(const string& cmd, Math* data, bool& Error)
{

	data_type data_type;
	if (data == nullptr)data_type = NULLS;else data_type = data->GetType();
	Math* ans_ptr = nullptr;//Answer pointer.
	fraction* _tmp = nullptr;
	double val = 0.0;
	try
	{
		switch (data_type)
		{
		case INT:
			_tmp = new fraction(*dynamic_cast<Int*>(data));
			ans_ptr = Func(cmd, _tmp, Error);
			//Do not need to delete _tmp;
			break;

		case FRC:
			val = (dynamic_cast<fraction*>(data))->GetValue();
			if (cmd == "sin") { ans_ptr = new fraction(sin(val)); }
			else if (cmd == "cos") { ans_ptr = new fraction(cos(val)); }
			else if (cmd == "tan") { ans_ptr = new fraction(tan(val)); }
			else if (cmd == "ln") { ans_ptr = new fraction(ln(dynamic_cast<fraction*>(data)->GetValueD())); }
			else if (cmd == "lg") { ans_ptr = new fraction(log(val) / log(10.0)); }
			else if (cmd == "arcsin") { ans_ptr = new fraction(asin(val)); }
			else if (cmd == "arccos") { ans_ptr = new fraction(acos(val)); }
			else if (cmd == "arctan") { ans_ptr = new fraction(atan(val)); }
			else if (cmd == "sqrt") { ans_ptr = new fraction(pow(dynamic_cast<fraction*>(data)->GetValueD(), Double(Int_one, (Int)2))); }
			//else if (cmd == "cbrt") { ans_ptr= fraction(cbrt(data->value)); }
			else if (cmd == "reciprocal") { ans_ptr = new fraction(reciprocal(*dynamic_cast<fraction*>(data))); }
			else if (cmd == "n")//Numerical calculation initialized.
			{
				((fraction*)(ans_ptr = new fraction(*dynamic_cast<fraction*>(data))))->AbortPreciseCalculation();
			}
			else
			{
				Error = true;
				delete data;
				return nullptr;//Will memory Leak? No!
			}

			delete data;//delete a null pointer is valid.
			break;

		case MAT://Matrix
			if (cmd == "det")ans_ptr = new fraction(det(*(dynamic_cast<Matrix*>(data))));
			else if (cmd == "gdet")ans_ptr = new fraction(Gdet(*(dynamic_cast<Matrix*>(data))));
			else if (cmd == "inverse")ans_ptr = new Matrix(inverse(*(dynamic_cast<Matrix*>(data))));
			else if (cmd == "ginverse" || cmd == "reciprocal")ans_ptr = new Matrix(Ginverse(*(dynamic_cast<Matrix*>(data))));
			else if (cmd == "transpose" || cmd == "trn")ans_ptr = new Matrix(Transpose(*(dynamic_cast<Matrix*>(data))));
			else if (cmd == "geliminate")ans_ptr = new Matrix(GaussEliminate(*(dynamic_cast<Matrix*>(data))));
			else if (cmd == "r" || cmd == "rank")ans_ptr = new fraction((dynamic_cast<Matrix*>(data))->rank());
			else if (cmd == "eigenequ")ans_ptr = new Matrix((dynamic_cast<Matrix*>(data))->EigenEqu());
			else if (cmd == "leftnullspace")ans_ptr = new Matrix(LeftNullSpace(*(dynamic_cast<Matrix*>(data))));
			else if (cmd == "nullspace")ans_ptr = new Matrix(NullSpace(*(dynamic_cast<Matrix*>(data))));
			else if (cmd == "n")((Matrix*)(ans_ptr = new Matrix(*dynamic_cast<Matrix*>(data))))->AbortPreciseCalculation();

			//Customized functions
			else
			{
				Error = true;
				delete data;
				return nullptr;
			}
			delete data;
			break;


		case NULLS:

			if (cmd == "pi") { ans_ptr = new fraction(PI); }
			else if (cmd == "e") { ans_ptr = new fraction(EULER_NUM); }//consts
			else if (cmd == "ans")
			{
				return DeepCopy(prev_ans);
			}
			break;
		}
	}
	catch (...)
	{
		delete data;
		throw Exceptions(_Math_Error);
	}
	if (ans_ptr == nullptr)throw Exceptions(_Function_Unidentified);
	return ans_ptr;
}

Math* MatMap(const string& matName)//return pointer of the desired operand in matArr.
{
	for (size_t i = 0;i < matNameArr.size();i++)
	{
		if (matNameArr[i] == matName)//Match.
		{
			if (matArr[i]->GetType() == FRC)//a fraction
			{
				return new fraction(*dynamic_cast<fraction*>(matArr[i]));
			}
			else if(matArr[i]->GetType()==MAT)
			{
				return new Matrix(*dynamic_cast<Matrix*>(matArr[i]));
			}
			else
			{
				throw Exceptions(_Function_Unidentified);
			}
		}
		else if (matNameArr[i] + 'T' == matName)
		{
			if (matArr[i]->GetType() == FRC)
			{
				return new fraction(*dynamic_cast<fraction*>(matArr[i]));
			}
			else if (matArr[i]->GetType() == MAT)
			{
				return new Matrix(Transpose(*(dynamic_cast<Matrix*>(matArr[i]))));
			}
			else
			{
				throw Exceptions(_Function_Unidentified);
			}
		}
	}
	throw Exceptions(_Function_Unidentified);
}

//The main func.
Math* Calculate(string expr)
{
	static int depth = 0;
	(*Clear)(0, true);//Initialize the character analyzer.
	if (depth == 0)
	{
		DeleteSpaces(expr);
		if (!BracMatch(expr)) { throw Exceptions(_Brac_Error); }
	}
	//Break the expression into small pieces.N,T,F.
	queue<string> command;
	bool Error = false;
	stack<Math*> stkopnd;
	stack<optrs> stkoptr;

	//Start Analyzing.
	int L = expr.length();if (L == 0) { throw Exceptions(_Syntax_Error); }
	char_type AnalyzeResult;
	string Curr;
	bool BracFlag = false;
	int BracCount = 0;

	for (int i = 0;i < L;i++)
	{
		AnalyzeResult = AnalyzeChar(expr[i]);
		if (AnalyzeResult == brac_left)BracCount++;
		if (AnalyzeResult == brac_right)BracCount--;
		if (AnalyzeResult == brac_left && BracCount == 1)
		{
			BracFlag = true;
			if (!Curr.empty())command.push(Curr);
			Curr = expr[i];//leftbracket
			continue;
		}
		else if (AnalyzeResult == brac_right && BracCount == 0)
		{
			BracFlag = false;
			Curr += expr[i];
			command.push(Curr);
			Curr.clear();
			(*Clear)(0, true);//Initialize.
			continue;
		}
		else if (Analyze_Change(AnalyzeResult) && !BracFlag)
		{
			command.push(Curr);
			Curr = expr[i];//An operand is inputed.
		}
		else
		{
			Curr += expr[i];
		}
	}
	if (Curr != "")command.push(Curr);
	//-1^5
	char_type  temp = Type(command.front());
	if (temp == sign)//-5+3
	{
		stkoptr.push(Convert_to_Operator(command.front()));
		command.pop();
	}
	if (command.empty())throw Exceptions(_Syntax_Error);

	//Now the expression is stored in queue command. 
	//Check started.
	string Opnd_Temp;
	string Optr_Temp;

	try
	{
		while (true)
		{
			Opnd_Temp = command.front();		command.pop();//Read an operand.
			Math* Func_Result = nullptr;
			Math* Result_ptr = nullptr;
			switch (Type(Opnd_Temp))
			{
			case func://If it is a function,assure that the string which follows is an operand,or a matrix.
				if (Opnd_Temp != "pi"&& Opnd_Temp != "e"&& Opnd_Temp != "ans")// ! Consts, ie. real function.
				{
					if (command.empty())throw Exceptions(_Syntax_Error);
					string Temp_Data = command.front();command.pop();//The parameter of a function can be a bracket,a matrix,or ,simply, a number.
					char_type type_of_tempdata = Type(Temp_Data);
					if (type_of_tempdata == brac_right)
					{
						Temp_Data.pop_back();
						Temp_Data.erase(0, 1);//Delete the brackets.
						depth++;
						Result_ptr = Calculate(Temp_Data);
						depth--;
					}
					else if (type_of_tempdata == opnd)//func applied to an operand.
					{
						Result_ptr = Convert_to_Fraction(Temp_Data);
					}
					else if (type_of_tempdata == matrix)//Maybe a real number.
					{//The matrix tank MatArr is not modified.
						Result_ptr = MatMap(Temp_Data);
					}
					else throw Exceptions(_Syntax_Error);
					Func_Result = Func(Opnd_Temp, Result_ptr, Error);//Calculate the value of the function.
				}
				else
				{
					Func_Result = Func(Opnd_Temp, nullptr, Error);//pi & e. Assure that error is not provoked.
				}
				if (Error)throw Exceptions(_Function_Unidentified);
				stkopnd.push(Func_Result);
				break;

			case brac_right://Complex Bracks
			{
				Opnd_Temp.pop_back();
				Opnd_Temp.erase(0, 1);
				depth++;
				Math* Brac_Result_ptr = Calculate(Opnd_Temp);
				depth--;
				stkopnd.push(Brac_Result_ptr);//delete?No!
			}
			break;

			case opnd:
				stkopnd.push(Convert_to_Fraction(Opnd_Temp));
				break;
			case matrix:
				stkopnd.push(MatMap(Opnd_Temp));//Copy constructor called.
				break;
			default:
				throw Exceptions(_Syntax_Error);
			}
			//There can be no command.
			if (command.empty())break;

			Optr_Temp = command.front();	//Read an operator. If it is a function...
			optrs ConvertedOptr;
			int PrevPriority = 0;
			if (!stkoptr.empty())PrevPriority = Priority(stkoptr.top());//The previous priority.
			switch (Type(Optr_Temp))
			{
			case func://case 3/2sin3
			case brac_right: //case 3(1+2)
			case matrix:
				if (PrevPriority >= Priority(mult))//case 3/2(1+2)
				{
					stkoptr.push(mult);//Of the first priority
					continue;//Dangerous Operation!
				}
				ConvertedOptr = mult;
				break;

			case sign:
			case optr:
				ConvertedOptr = Convert_to_Operator(Optr_Temp);
				command.pop();
				break;
			default:
				Error = true;
				break;
			}
			if (Error || command.empty())throw Exceptions(_Syntax_Error);

			int CurrPriority = Priority(ConvertedOptr);//Priority of the current operator.
			if (PrevPriority > CurrPriority)
			{
				while (!stkoptr.empty()) { CalculateOnce(&stkopnd, &stkoptr); }
			}
			else
			{
				if (PrevPriority == CurrPriority)CalculateOnce(&stkopnd, &stkoptr);
			}
			stkoptr.push(ConvertedOptr);
		}

		while (!stkoptr.empty())//Calculate the present expression
		{
			CalculateOnce(&stkopnd, &stkoptr);
		}

	}
	catch (...)
	{
		ClearOpndStack(&stkopnd);
		throw;
	}

	Math* ans_ptr = stkopnd.top();//Extract the answer pointer.
	stkopnd.pop();

	if (depth == 0)
	{
		if (prev_ans != nullptr)delete prev_ans;//Without this line, there`ll be memory leakage.
		prev_ans = DeepCopy(ans_ptr);
	}

	if (!stkopnd.empty()) { Analyze_Change(0, true);throw Exceptions(_Test_Debug_Error); }
	if (!stkoptr.empty()) { Analyze_Change(0, true);throw Exceptions(_Test_Debug_Error); }
	if (!command.empty()) { Analyze_Change(0, true);throw Exceptions(_Test_Debug_Error); }
	(*Clear)(0, true);//Reset the char analyzer

	return ans_ptr;
}


Matrix InputMatrix(int row, int column)
{
	if (row <= 0 || column <= 0)throw Exceptions(_Matrix_Size_Error);
	Matrix A(row, column);
	int i = 0;
	int  j = 0;
	string temp;
	Math* Tmp_Calculation;
	fraction* Tmp_ans;

	for (i = 0;i < row;i++)
	{
		for (j = 0;j < column;j++)
		{
			cin >> temp;
			Tmp_Calculation = Calculate(temp);
			switch (Tmp_Calculation->GetType())
			{
			case INT:
				Tmp_ans = new fraction(*dynamic_cast<Int*>(Tmp_Calculation));
				delete Tmp_Calculation;
				break;

			case FRC:
				Tmp_ans = dynamic_cast<fraction*>(Tmp_Calculation);
				break;

			default:
				throw Exceptions(_Invalid_Input);
			}

			A(i, j) = *Tmp_ans;
			delete Tmp_ans;
		}
	}
	return A;
}

