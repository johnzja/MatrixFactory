// ConsoleAppTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ExpressionAnalyzer.h"
#include "Output.h"
#define _AUTHOR_pINFO "This is a matrix calculator, which greatly boosts your study progress of linear algebra. \n(It can help you do heaps of annoying calculations! )\n\n"
//#define ISOLATOR "--------------------------------------"
#define HELP_INFO "******HELP********\nBasic calculation syntax: 3(1+2) ;-pi ;sin(e^pi) ;1-2*3+4/5-6;etc\nMatrix definition: define A ; redefine A ; A(B+C) ; det(A*B) ; inverse(A+B) ;A^(-1) ;etc.\n"
using namespace std;
//Declare the external functions & constants.
extern bool(*Clear)(int AnalyzeResult, bool initialize);
extern vector<Math*> matArr;//Memory Tank
extern vector<string> matNameArr;
extern const char* _Invalid_MatName;
extern const char* _Syntax_Error;
extern const char* _Unknown_Mat_Error;
extern const char* _Test_Debug_Error;

//Function definition
void defineMat(string& cmd,bool CheckValidity=true);
void redefineMat(string& cmd);
void help();
void ClearMemoryTank();
void ShowMemoryTank();
void Show_Lines();

char Edition[] = "3.0.4";//Version information.
int Matrix::MatrixCount = 0;//Initialize static member in class Matrix.


int main() 
{
	string cmd;//command.
	string last_cmd;//previous command.
	Clear = Analyze_Change;//Initialize function pointer.
	init();	//display some basic information about this application.

	while (true)
	{
		try 
		{
			Math* ans;
			getline(cin, cmd);
			//CheckMatDefinition.
			int pos = cmd.find("define ");
			if (pos != string::npos&&pos == 0)
			{
				cmd.erase(0, 7);//delete "define " from the cmd string.
				defineMat(cmd);
				continue;
			}

			pos = cmd.find("redefine ");
			if (pos != string::npos&& pos == 0)
			{
				cmd.erase(0, 9);//delete "redefine ".
				redefineMat(cmd);
				continue;
			}
			else if (cmd == "exit" || cmd == "terminate" || cmd == "abort")
			{
				ClearMemoryTank();
				return 0;
			}
			else if (cmd == "help" || cmd == "f1") { help();isolate();continue; }
			else if (cmd == "cls")
			{
				system("cls");
				init();
				continue;
			}
			else if (cmd == "clrmem"||cmd=="clearmem")
			{
				ClearMemoryTank();
				continue;
			}
			else if (cmd == "showmem") { ShowMemoryTank();continue; }
			else if (cmd == "showlines") { Show_Lines();continue; }
			else if (cmd == "")cmd = last_cmd;

			ans = Calculate(cmd);
			ans->print();
			delete ans;
		}
		catch (Exceptions A)
		{
			A.print();
			cmd.clear();
		}
		(*Clear)(0, true);
		last_cmd = cmd;
		isolate();
	}
		
	return 0;
}


void defineMat(string& cmd,bool CheckValidity)
{
	if (cmd.length() == 0)  throw Exceptions(_Syntax_Error);
	bool EqFlag = false;//define A=transposeB;
	string expr_after_eq;

	size_t L = cmd.length();//Save the cpu time.
	for (size_t i = 0;i < L;i++)
	{
		char_type Analyze_Result = AnalyzeChar(cmd[i]);
		if (Analyze_Result == equ_sign)
		{
			expr_after_eq = cmd.substr(i + 1, cmd.length() - 1 - i);
			cmd = cmd.substr(0, i);//Obtain the matrix name.
			EqFlag = true;
			break;
		}
		if (Analyze_Result != matrix) throw Exceptions(_Invalid_MatName);
	}

	if (cmd.length() != 1)throw Exceptions(_Syntax_Error);
	if (CheckValidity)//Check collision.
	{
		for (size_t i = 0;i < matNameArr.size();i++)//Check for collision.
		{
			if (cmd == matNameArr[i])
			{
				throw Exceptions(_Invalid_MatName);
			}
		}
	}

	if (EqFlag)//case in which a sign of equation appeared.
	{
		Math* result_equRight=Calculate(expr_after_eq);
		matArr.push_back(result_equRight);
		matNameArr.push_back(cmd);

		cout << cmd << ":=" << endl;
		result_equRight->print();
	}
	else
	{
		cout << "Please enter two positive integers. m×n Matrix." << endl;
		int m, n;
		cin >> m >> n;
		if (m <= 0 || n <= 0)throw Exceptions(_Invalid_MatName);

		cout << "Please Enter the " << m << " by " << n << " Matrix." << endl;
		//Store the matrix.
		matArr.push_back(new Matrix(InputMatrix(m, n)));
		matNameArr.push_back(cmd);

		//Some strange problems.
		string rubbish;
		getline(cin, rubbish);
		cmd.clear();
	}

	cout << "Definition Complete!" << endl;
	isolate();
}

void redefineMat(string& cmd)
{
	if (cmd.length() == 0)  throw Exceptions(_Invalid_MatName);

	string cmd_prev_data = cmd;
	bool EqFlag = false;

	for (register unsigned int i = 0;i < cmd.length();i++)
	{
		char_type Analyze_Result = AnalyzeChar(cmd[i]);
		if (Analyze_Result == equ_sign)//"="
		{
			cmd = cmd.substr(0, i);//Matrix name.
			EqFlag = true;
			break;
		}
		if (Analyze_Result != matrix) throw Exceptions(_Invalid_MatName);
	}

	if (cmd.length() != 1)throw Exceptions(_Invalid_MatName);

	int pointer = -1;
	for (size_t i = 0;i <matNameArr.size();i++)
	{
		if (matNameArr[i] == cmd)pointer = i;
	}
	if (pointer == -1)
	{
		cmd.clear();
		throw Exceptions(_Unknown_Mat_Error);
	}
	else//delete matName [pointer] .
	{	//Redefine.
		defineMat(cmd_prev_data,false);

		vector<Math*>::iterator it_mat = matArr.begin();
		vector<string>::iterator it_str = matNameArr.begin();

		for (register int i = 0;i < pointer ;i++)
		{
			++it_mat;
			++it_str;
		}		
		delete matArr[pointer];
		matArr.erase(it_mat);
		matNameArr.erase(it_str);
	}

	cmd.clear();
	return;
}

void help(void)
{
	cout << HELP_INFO<< _AUTHOR_pINFO;
}

void ClearMemoryTank()
{
	while (!matArr.empty())
	{//delete.
		delete matArr.back();

		//pop.
		matArr.pop_back();
		matNameArr.pop_back();
	}
	cout << "ClearOperation successfully done." << endl;
	isolate();
}

void ShowMemoryTank()
{
	if (matArr.size() != matNameArr.size())throw Exceptions(_Test_Debug_Error);
	cout << "Memory Tank used: " << matArr.size() << endl;
	cout << ISOLATOR << endl;
	for (unsigned int i = 0;i < matArr.size(); i++)
	{
		cout << matNameArr[i] << "=" << endl;
		matArr[i]->print();
		cout << endl;
		isolate();
	}
}
//Test

void Show_Lines()
{
	cout << 5049 << endl;
	isolate();
}

