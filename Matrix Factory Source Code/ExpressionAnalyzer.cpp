#include "stdafx.h"
#include "ExpressionAnalyzer.h"

#define DECIMAL_LENGTH 5
#define Int int
using namespace std;

vector<fraction*> matArr;
vector<string> matNameArr;

char* ValidFuncName[] = { "sin","cos","tan","log","ln","arcsin","arccos","arctan","sqrt","ans","inverse","det","transpose" ,"ginverse","gdet","geliminate","reciprocal"};
bool(*Clear)(int AnalyzeResult, bool initialize);

Matrix prev_ans_mat;
fraction prev_ans_frac;
bool is_prev_mat = false;

extern fraction zero;

void DeleteSpaces(string& expr)
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

int AnalyzeChar(char chr)
{
	switch (chr)
	{
	case '+':
	case '-':
		return SIGN;
		break;
	case '/':
	case '*':
		return OPTR;
		break;
	case '^':
		return OPTR;
		break;
	case '(':
		return BRAC_LEFT;
		break;
	case ')':
		return BRAC_RIGHT;
		break;
	case '=':
		return EQU_SIGN;
		break;
	default:
		break;
	}
	if ((chr >= 48 && chr <= 57) || chr == 46)//A number. Comma included.
	{
		return OPND;
	}
	else if (chr>=97 && chr<=122)//It is an alphabet.(chr >= 65 && chr <= 90)
	{
		return FUNC;
	}
	else if (chr >= 65 && chr <= 90)
	{
		return MATRIX;
	}
	else
	{
		return UNIDENT;
	}
}

bool Analyze_Change(int AnalyzeResult,bool initialize=false)
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

inline int Type(const string& str)
{
	return AnalyzeChar(str.back());
}

fraction Convert_to_Fraction(const string& str,bool& Error)
{
	int L = str.length();
	bool isNegative = false;
	bool isDecimal = false;
	int DecimalCount = 0;
	string Result_AbsValue;

	for (int i = 0;i < L;i++)
	{
		if (AnalyzeChar(str[i]) > SIGN)
		{
			Error = true;
			return zero;
		}
		if (str[i] == '+')continue;
		else
		{
			if (str[i] == '-') { isNegative=!isNegative; continue; }
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

	if ((DecimalCount > 1)||(Result_AbsValue.length() == 0)||DecimalCount==L)
	{
		Error = true;
		return zero;//SyntaxError
	}


	if (isDecimal == false)
	{
		
		fraction ans(stoi(Result_AbsValue));
		if (isNegative)
		{
			return -ans;
		}
		else
		{
			return ans;
		}
	}
	else
	{
		fraction ans;
		if (Result_AbsValue.length() <= DECIMAL_LENGTH)
		{
			//Convert to fraction form.
			int Result_Length = Result_AbsValue.length();
			int Decimal_Position;

			Decimal_Position = Result_AbsValue.find('.');
			int Fractional_Part_Length = Result_Length - Decimal_Position - 1;

			Result_AbsValue.erase(Decimal_Position,1);

			fraction NUM(stoi(Result_AbsValue));
			fraction DENOM = pow(fraction(10), Fractional_Part_Length);
			if (isNegative)
			{
				ans = -NUM / DENOM;
			}
			else
			{
				ans = NUM / DENOM;
			}

		}
		else
		{

			if (isNegative)
			{
				ans.SetValue(-stod(Result_AbsValue));
			}
			else
			{
				ans.SetValue(stod(Result_AbsValue));
			}
		}
		return ans;
	}
}

char Convert_to_Operator(const string& str, bool& Error)
{
	int L = str.length();
	bool isNegative = false;

	if (L == 0) { Error = true;return ADD; }
	if (L == 1)
	{
		switch (str[0])
		{
		case'+':
			return ADD;
			break;
		case'-':
			return SUBTRACT;
			break;
		case '*':
			return MULT;
			break;
		case '/':
			return DIV;
			break;
		case '^':
			return POW;
			break;
		default:
			Error = true;
			return ADD;
		}
	}
	else if (str == "^^")
	{
		return POW_EACH;
	}
	else//L>=2:only one circumstance.
	{
		int Analyze_Result = 0;
		for (int i = 0;i < L;i++)
		{
			Analyze_Result = AnalyzeChar(str[i]);
			if (Analyze_Result!=SIGN)
			{
				Error = true;
				return ADD;
			}
			else
			{
				if (str[i] == '-')isNegative = !isNegative;
			}
		}

		if (!isNegative)
		{
			return ADD;
		}
		else
		{
			return SUBTRACT;
		}

	}
}

int Priority(const char& intchr)
{
	switch (intchr)
	{
	case ADD:
	case SUBTRACT:
		return 1;
		break;
	case MULT:
	case DIV:
		return 2;
		break;
	case POW:
	case POW_EACH:
		return 3;
		break;
	default:
		return 0;
		break;
	}
}

void ClearOpndStack(stack<fraction*>* stkopnd)
{
	while (!stkopnd->empty())
	{
		delete stkopnd->top();
		stkopnd->pop();
	}

}

void CalculateOnce(stack<fraction*>* stkopnd, stack<char>* stkoptr)
{
	fraction* B_ptr = stkopnd->top();stkopnd->pop();//Ensure the uniqueness of the pointer. Don`t forget to delete it.
	if (stkopnd->empty())
	{
		char temp_optr = stkoptr->top();stkoptr->pop();
		switch (B_ptr->GetType())
		{
		case 0://fraction
			if (temp_optr == ADD) { stkopnd->push(B_ptr);return; }
			else if (temp_optr == SUBTRACT)
			{
				fraction* Negative_B_ptr = new fraction(-*B_ptr);
				delete B_ptr;
				stkopnd->push(Negative_B_ptr);
				return;
			}
			else
			{
				delete B_ptr;
				throw SYNTAX_ERROR;
			}
			break;
		case 1://Matrix
			if (temp_optr == ADD) { stkopnd->push(B_ptr);return; }//What does this line mean?
			else if (temp_optr == SUBTRACT)
			{
				Matrix* Negative_B_ptr = new Matrix(-*(B_ptr->GetThis()));
				delete B_ptr;
				stkopnd->push(Negative_B_ptr);
				return;
			}
			else
			{
				delete B_ptr;
				throw SYNTAX_ERROR;
			}

		default:
			delete B_ptr;
			throw UNIDENT_ERROR;
			break;
		}

	}
	int B_Type = B_ptr->GetType();
	fraction* A_ptr = stkopnd->top();stkopnd->pop();
	int A_Type = A_ptr->GetType();
	try
	{
		if (A_Type == 0 && B_Type == 0)//two fractions
		{
			fraction A = *A_ptr;
			fraction B = *B_ptr;
			fraction ans;
			switch (stkoptr->top())//Get the operator
			{
			case ADD:
				ans = A + B;
				break;
			case SUBTRACT:
				ans = A - B;
				break;
			case MULT:
				ans = A*B;
				break;
			case DIV:
				ans = A / B;
				break;
			case POW:
				if (!(A_ptr->GetApprox()) && isInt(*B_ptr))//precise.
				{
					ans = pow(*A_ptr, int(*B_ptr));
				}
				else
				{

					if (A.GetValue() <= 0)
					{
						throw MATH_ERROR;
					}
					ans.SetValue(pow(A.GetValue(), B.GetValue()));
				}

				break;
			default:
				throw MATH_ERROR;
				break;
			}
			stkopnd->push(new fraction(ans));
		}
		else if (A_Type == 1 && B_Type == 1)//Two matrices.
		{
			Matrix ans;
			switch (stkoptr->top())
			{
			case ADD:
				ans = *(A_ptr->GetThis()) + *(B_ptr->GetThis());
				break;
			case SUBTRACT:
				ans = *(A_ptr->GetThis()) - *(B_ptr->GetThis());
				break;
			case MULT:
				ans = *(A_ptr->GetThis()) * *(B_ptr->GetThis());
				break;
			case DIV:
				ans = *(A_ptr->GetThis()) * Ginverse(*(B_ptr->GetThis()));
				break;
			default:
				throw MATH_ERROR;
				break;
			}
			Matrix* ans_ptr = new Matrix(ans);
			stkopnd->push(ans_ptr);
		}
		else if (A_Type == 0 && B_Type == 1)//A:num;B:Matrix
		{
			switch (stkoptr->top())
			{
			case MULT:
				stkopnd->push(new Matrix((*A_ptr)* *(B_ptr->GetThis())));
				break;
			case DIV:
				stkopnd->push(new Matrix((*A_ptr) * Ginverse(*(B_ptr->GetThis()))));
				break;
			default:
				throw MATH_ERROR;
			}
		}
		else if (A_Type == 1 && B_Type == 0)
		{//switch.
			if (stkoptr->top() == POW)//A^2 , A^0 or A^(-1)
			{
				Matrix* A_This = A_ptr->GetThis();//Type:Matrix*
				Int B_num = int(*B_ptr);
				if (!(isInt(*B_ptr)) || B_num < -1 || B_num == 0) throw MATH_ERROR;
				if (B_num == -1)//A^(-1)
				{
					*A_This = Ginverse(*A_This);
				}
				else
				{
					Matrix temp = *(A_This);
					for (int i = 1;i < B_num;i++)//time Complexity=o(n)
					{
						*A_This = (*(A_This))* temp;
					}
				}

				delete B_ptr;
				stkopnd->push(A_This);//Matrix*->fraction*
				stkoptr->pop();
				return;//A_ptr can`t be deleted. Function result returned.
			}
			else if (stkoptr->top() == POW_EACH)//operator ^^
			{
				if (!isInt(*B_ptr)) { throw MATH_ERROR; }
				int pow_n = int(*B_ptr);

				Matrix* A_this = A_ptr->GetThis();//Get the matrix ptr.
				A_this->ValidityCheck();

				int i = A_this->GetRowCnt();
				int j = A_this->GetColCnt();
				fraction** A_data_ptr = A_this->GetPtr();

				for (int m = 0;m < i;m++)
				{
					for (int n = 0;n < j;n++)
					{
						A_data_ptr[m][n] = pow(A_data_ptr[m][n], pow_n);
					}
				}
				delete B_ptr;
				stkopnd->push(A_ptr);
				stkoptr->pop();
				return;

			}
			else
			{
				throw MATH_ERROR;
			}

		}
		else
		{
			throw MATH_ERROR;
		}
		stkoptr->pop();
		delete A_ptr;delete B_ptr;
	}
	catch (...)
	{
		delete A_ptr;delete B_ptr;
		throw;
	}
	return;
}

fraction* Func(const string& cmd, fraction* data, bool& Error)
{
	int data_type;
	if (data == nullptr)data_type = 0;else data_type= data->GetType();
	fraction* ans_ptr;//Answer pointer.
	double val;
	try
	{
		switch (data_type)
		{
		case 0://fraction
			val = data->GetValue();
			if (cmd == "sin") { ans_ptr=new fraction(sin(val)); }
			else if (cmd == "cos") { ans_ptr = new fraction(cos(val)); }
			else if (cmd == "tan") { ans_ptr = new fraction(tan(val)); }
			else if (cmd == "ln") { ans_ptr = new fraction(log(val)); }
			else if (cmd == "lg") { ans_ptr = new fraction(log(val) / log(10.0)); }
			else if (cmd == "arcsin") { ans_ptr = new fraction(asin(val)); }
			else if (cmd == "arccos") { ans_ptr = new fraction(acos(val)); }
			else if (cmd == "arctan") { ans_ptr = new fraction(atan(val)); }
			else if (cmd == "sqrt") { ans_ptr = new fraction(sqrt(val)); }
			//else if (cmd == "cbrt") { ans_ptr= fraction(cbrt(data->value)); }
			else if (cmd == "reciprocal") { ans_ptr = new fraction(reciprocal(*data)); }
			else if (cmd == "pi") { ans_ptr = new fraction(PI); }
			else if (cmd == "e") { ans_ptr = new fraction(EULER_NUM); }//consts

			else if (cmd == "ans")
			{
				if (is_prev_mat)ans_ptr = new Matrix(prev_ans_mat);
				else ans_ptr = new fraction(prev_ans_frac);
			}
			else
			{
				Error = true;
				delete data;
				return nullptr;//Will memory Leak? No!
			}
			delete data;
			break;

		case 1://Matrix
			if (cmd == "det")ans_ptr = new fraction(det(*(data->GetThis())));//data->GetThis() obtains a Matrix pointer.
			else if (cmd == "gdet")ans_ptr = new fraction(Gdet(*(data->GetThis())));
			else if (cmd == "inverse")ans_ptr = new Matrix(inverse(*(data->GetThis())));
			else if (cmd == "ginverse"||cmd=="reciprocal")ans_ptr = new Matrix(Ginverse(*(data->GetThis())));
			else if (cmd == "transpose"||cmd=="trn")ans_ptr = new Matrix(Transpose(*(data->GetThis())));
			else if (cmd == "geliminate")ans_ptr = new Matrix(GaussEliminate(*(data->GetThis())));
			else if (cmd == "r"||cmd=="rank")ans_ptr = new fraction((data->GetThis())->rank());
			else if (cmd == "rowsum")ans_ptr = new Matrix(RowSum(*(data->GetThis())));
			else if (cmd == "colsum")ans_ptr = new Matrix(ColSum(*(data->GetThis())));
			else if(cmd=="eigenequ")ans_ptr = new Matrix((data->GetThis())->EigenEqu());
			else if (cmd == "leftnullspace")ans_ptr = new Matrix(LeftNullSpace(*(data->GetThis())));
			else if (cmd == "nullspace")ans_ptr = new Matrix(NullSpace(*(data->GetThis())));

			//Customized functions

			else
			{
				Error = true;
				delete data;
				return nullptr;
			}
			delete data;
			break;
			//No default.
		}
		
	}catch (...)
		{
			delete data;
			throw MATH_ERROR;
		}
	return ans_ptr;
}

fraction* MatMap(const string& matName)//return pointer of the desired operand in matArr.
{
	for (register unsigned int i = 0;i < matNameArr.size();i++)
	{
		if (matNameArr[i] == matName)//Match.
		{
			if (matArr[i]->GetType() == 0)//a fraction
			{
				return new fraction(*matArr[i]);
			}
			else//a Matrix.
			{
				return new Matrix(*(matArr[i]->GetThis()));
			}
		}
		else if (matNameArr[i]+ 'T' == matName )
		{
			if (matArr[i]->GetType() == 0)
			{
				return new fraction(*matArr[i]);
			}
			else
			{
				return new Matrix(Transpose(*(matArr[i]->GetThis())));
			}
		}
	}
	throw UNKNOWN_MAT_ERROR;
}

fraction* Calculate(string expr)
{
	static int depth = 0;
	(*Clear)(0, true);//Initialize the character analyzer.
	if (depth ==0)
	{
		DeleteSpaces(expr);
		if (!BracMatch(expr)) { throw BRAC_ERROR; }
	}
	
	//Break the expression into small pieces.N,T,F.
	queue<string> command;
	bool Error = false;
	stack<fraction*> opnd;
	stack<char> optr;	//Variant Definition

	//Start Analyzing.
	int L = expr.length();if (L == 0) { throw SYNTAX_ERROR; }
	int AnalyzeResult;
	string Curr;
	bool BracFlag = false;
	int BracCount = 0;

	for (register int i = 0;i < L;i++)
	{
		AnalyzeResult = AnalyzeChar(expr[i]);
		if (AnalyzeResult == BRAC_LEFT)BracCount++;
		if (AnalyzeResult == BRAC_RIGHT)BracCount--;
		if (AnalyzeResult == BRAC_LEFT&& BracCount==1)
		{
			BracFlag = true;
			if(!Curr.empty())command.push(Curr);
			Curr = expr[i];//leftbracket
			continue;
		}
		else if (AnalyzeResult == BRAC_RIGHT&& BracCount==0)
		{
			BracFlag = false;
			Curr += expr[i];
			command.push(Curr);
			Curr.clear();
			(*Clear)(0, true);//Initialize.
			continue;
		}
		else if (Analyze_Change(AnalyzeResult)&& !BracFlag)
		{
			command.push(Curr);
			Curr = expr[i];//An operand is inputed.
		}
		else
		{
			Curr += expr[i];
		}
	}
	if(Curr!="")command.push(Curr);
	//-1^5
	int temp = Type(command.front());
	if(temp==SIGN)//-5+3
	{
		optr.push(Convert_to_Operator(command.front(),Error));
		command.pop();
		if (Error) throw SYNTAX_ERROR;
	}
	if (command.empty())throw SYNTAX_ERROR;

	//Now the expression is stored in queue command. 
	//Check started.
	string Opnd_Temp;
	string Optr_Temp;

	try
	{
		while (true)
		{
			Opnd_Temp = command.front();		command.pop();//Read an operand.
			fraction* Func_Result = nullptr;
			fraction* Result_ptr = nullptr;
			switch (Type(Opnd_Temp))
			{
			case FUNC://If it is a function,assure that the string which follows is an operand,or a matrix.
				if (Opnd_Temp != "pi"&& Opnd_Temp != "e"&& Opnd_Temp!="ans")// ! Consts
				{
					if (command.empty())throw SYNTAX_ERROR;
					string Temp_Data = command.front();command.pop();//The parameter of a function can be a bracket,a matrix,or ,simply, a number.
					int type_of_tempdata = Type(Temp_Data);
					if (type_of_tempdata == BRAC_RIGHT)
					{
						Temp_Data.pop_back();
						Temp_Data.erase(0, 1);//Delete the brackets.
						depth++;
						Result_ptr= Calculate(Temp_Data);
						depth--;
					}
					else if(type_of_tempdata ==OPND)
					{
						Result_ptr = new fraction(Convert_to_Fraction(Temp_Data, Error));
						if (Error)throw SYNTAX_ERROR;
					}
					else if (type_of_tempdata == MATRIX)//Maybe a real number.
					{//The matrix tank MatArr is not modified.
						Result_ptr = MatMap(Temp_Data);
					}
					else throw SYNTAX_ERROR;
					Func_Result = Func(Opnd_Temp, Result_ptr, Error);//Calculate the value of the function.
				}
				else
				{
					Func_Result = Func(Opnd_Temp, nullptr, Error);//pi & e. Assure that error is not provoked.
				}
				if (Error)throw UNIDENT_FUNC_ERROR;
				opnd.push(Func_Result);
				break;

			case BRAC_RIGHT://Complex Bracks
			{
				Opnd_Temp.pop_back();
				Opnd_Temp.erase(0, 1);
				depth++;
				fraction* Brac_Result_ptr= Calculate(Opnd_Temp);
				depth--;
				opnd.push(Brac_Result_ptr);//delete?No!
			}
			break;

			case OPND:
				opnd.push(new fraction(Convert_to_Fraction(Opnd_Temp, Error)));//delete?
				if (Error)throw SYNTAX_ERROR;
				break;
			case MATRIX:
				opnd.push( MatMap(Opnd_Temp));//Copy constructor called.
				break;
			default:
				throw SYNTAX_ERROR;
				break;
			}
			//There can be no command.
			if (command.empty())break;

			Optr_Temp = command.front();	//Read an operator. If it is a function...
			char ConvertedOptr;
			int PrevPriority = 0;
			if (!optr.empty())PrevPriority = Priority(optr.top());//The previous priority.
			switch (Type(Optr_Temp))
			{
			case FUNC://case 3/2sin3
			case BRAC_RIGHT: //case 3(1+2)
			case MATRIX:
				if (PrevPriority >= Priority(MULT))//case 3/2(1+2)
				{
					optr.push(MULT);//Of the first priority
					continue;//Dangerous Operation!
				}
				ConvertedOptr = MULT;
				break;

			case SIGN:
			case OPTR:
				ConvertedOptr = Convert_to_Operator(Optr_Temp, Error);
				command.pop();
				break;
			default:
				Error = true;
				break;
			}
			if (Error || command.empty())throw SYNTAX_ERROR;

			int CurrPriority = Priority(ConvertedOptr);//Priority of the current operator.
			if (PrevPriority > CurrPriority)
			{
				while (!optr.empty()) { CalculateOnce(&opnd, &optr); }
			}
			else
			{
				if (PrevPriority == CurrPriority)CalculateOnce(&opnd, &optr);
			}
			optr.push(ConvertedOptr);
		}

		while (!optr.empty())//Calculate the present expression
		{
			CalculateOnce(&opnd, &optr);
		}

	}
	catch (...)
	{
		ClearOpndStack(&opnd);
		throw;
	}

	fraction* ans_ptr = opnd.top();
	opnd.pop();

	if (depth == 0)
	{
		if (ans_ptr->GetType() == 0)
		{
			prev_ans_frac = *ans_ptr;
			is_prev_mat = false;
		}
		else
		{
			prev_ans_mat = *(ans_ptr->GetThis());
			is_prev_mat = true;
		}
	}

	if (!opnd.empty()){Analyze_Change(0,true);throw UNIDENT_ERROR;}
	if (!optr.empty()){Analyze_Change(0,true);throw UNIDENT_ERROR;}
	if (!command.empty()){Analyze_Change(0,true);throw UNIDENT_ERROR;}
	(*Clear)(0, true);//Reset the char analyzer
	
	return ans_ptr;
}
