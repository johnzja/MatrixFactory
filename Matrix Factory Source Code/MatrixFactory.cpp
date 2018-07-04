// ConsoleAppTest.cpp : 定义控制台应用程序的入口点。
//

//This program has 2473 lines of source code. Completed in about two weeks.
//Designed by John Zhu, Tsinghua University.
//User`s Brochure


#include "stdafx.h"
#include "ExpressionAnalyzer.h"
#define _AUTHOR_pINFO "This is a matrix calculator, which greatly boosts your study progress of linear algebra. \n(It can help you do heaps of annoying calculations! )\nAny bugs? Please contact the programmer. Tel: 15801916963 ; \nE-mail: johnzja@qq.com\nDon`t forget to submit the log file generated while the program corrupts!\n"
#define INVALID_MATNAME "Invalid Matrix Name!"
#define ISOLATOR "--------------------------------------"
#define HELP_INFO "******HELP********\nBasic calculation syntax: 3(1+2) ;-pi ;sin(e^pi) ;1-2*3+4/5-6;etc\nMatrix definition: define A ; redefine A ; A(B+C) ; det(A*B) ; inverse(A+B) ;A^(-1) ;etc.\n"
using namespace std;
//Declare the external functions & constants.
extern bool(*Clear)(int AnalyzeResult, bool initialize);
extern char* ValidFuncName[];
extern vector<fraction*> matArr;//Memory Tank
extern vector<string> matNameArr;

//Function definition
void defineMat(string& cmd,bool CheckValidity=true);
void redefineMat(string& cmd);
void help();
void init();
void ClearMemoryTank();
void ShowMemoryTank();

char Edition[] = "2.1.13";//Version information.
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
			fraction* ans;
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
			else if (cmd == "help" || cmd == "f1") { help();cout << ISOLATOR << endl;continue; }
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
			else if (cmd == "")cmd = last_cmd;

			ans = Calculate(cmd);
			ShowMatrix(ans);
			delete ans;
		}
		catch (char* A)
		{
			cerr<< A << endl;
			cmd.clear();
		}
		(*Clear)(0, true);
		last_cmd = cmd;
		cout << ISOLATOR<< endl;
	}
		
	return 0;
}

void init()
{
	cout << "*********************" << "Matrix Factory" << "*********************" << endl;
	cout << "Matrix Factory v" << Edition << endl;
	cout << "Supported functions:  ";
	for (register int i = 0;i < 17;i++)cout << ValidFuncName[i] << ",  ";
	cout << endl;
	cout << "Type help to get assistance." << endl;
	return;
}

void defineMat(string& cmd,bool CheckValidity)
{
	if (cmd.length() == 0)  throw INVALID_MATNAME;
	bool EqFlag = false;//define A=transposeB;
	string expr_after_eq;

	unsigned int L = cmd.length();//Save the cpu time.
	for (register unsigned int i = 0;i < L;i++)
	{
		int Analyze_Result = AnalyzeChar(cmd[i]);
		if (Analyze_Result == EQU_SIGN)
		{
			expr_after_eq = cmd.substr(i + 1, cmd.length() - 1 - i);
			cmd = cmd.substr(0, i);//Obtain the matrix name.
			EqFlag = true;
			break;
		}
		if (Analyze_Result != MATRIX) throw "Matrix name must be a capital letter!";
	}

	if (cmd.length() != 1)throw INVALID_MATNAME;
	if (CheckValidity)//Check collision.
	{
		for (register unsigned int i = 0;i < matNameArr.size();i++)//Check for collision.
		{
			if (cmd == matNameArr[i])
			{
				throw INVALID_MATNAME;
			}
		}
	}

	if (EqFlag)//case in which a sign of equation appeared.
	{
		fraction* result_equRight=Calculate(expr_after_eq);
		matArr.push_back(result_equRight);
		matNameArr.push_back(cmd);

		cout << cmd << ":=" << endl;
		ShowMatrix(result_equRight);
	}
	else
	{
		cout << "Please enter two positive integers. m×n Matrix." << endl;
		int m, n;
		cin >> m >> n;
		if (m <= 0 || n <= 0)throw "Invalid input!";

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
	cout << ISOLATOR<< endl;
}

void redefineMat(string& cmd)
{
	if (cmd.length() == 0)  throw INVALID_MATNAME;

	string cmd_prev_data = cmd;
	bool EqFlag = false;

	for (register unsigned int i = 0;i < cmd.length();i++)
	{
		int Analyze_Result = AnalyzeChar(cmd[i]);
		if (Analyze_Result == EQU_SIGN)//"="
		{
			cmd = cmd.substr(0, i);//Matrix name.
			EqFlag = true;
			break;
		}
		if (Analyze_Result != MATRIX) throw "Matrix name must be composed of capital letters!";
	}

	if (cmd.length() != 1)throw INVALID_MATNAME;

	int pointer = -1;
	for (register unsigned int i = 0;i <matNameArr.size();i++)
	{
		if (matNameArr[i] == cmd)pointer = i;
	}
	if (pointer == -1)
	{
		cmd.clear();
		throw UNKNOWN_MAT_ERROR;
	}
	else//delete matName [pointer] .
	{	//Redefine.
		defineMat(cmd_prev_data,false);

		vector<fraction*>::iterator it_mat = matArr.begin();
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
	cout << ISOLATOR << endl;
}

void ShowMemoryTank()
{
	if (matArr.size() != matNameArr.size())throw UNIDENT_ERROR;
	cout << "Memory Tank used: " << matArr.size() << endl;
	cout << ISOLATOR << endl;
	for (unsigned int i = 0;i < matArr.size(); i++)
	{
		cout << matNameArr[i] << "=" << endl;
		ShowMatrix(matArr[i]);
		cout << ISOLATOR << endl;
	}
	
	//return;
}

