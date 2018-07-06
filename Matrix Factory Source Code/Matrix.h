//Matrix.h
#pragma once
#include "fraction.h"
using namespace std;
#define MAT_ERROR "Invalid Matrix!"

#ifndef _MATRIX_H_
#define _MATRIX_H_

int GetNumLength(int& n);

/*    &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&    */
class Matrix;
struct SelectArray;
Matrix InputMatrix(const int& row, const int& column);
Matrix operator+(const Matrix& mat1, const Matrix& mat2);
Matrix operator-(const Matrix& mat);
Matrix operator-(const Matrix& mat1, const Matrix& mat2);
bool operator==(const Matrix& mat1, const Matrix& mat2);
ostream& operator<<(ostream& out, const Matrix& mat);
fraction det(const Matrix& mat);
Matrix reduce(const Matrix& mat, int DeleteCol, int DeleteRow = 0);
Matrix inverse(const Matrix& mat);
Matrix adj(const Matrix &mat);
Matrix operator*(const Matrix& mat1, const Matrix& mat2);
Matrix operator*(const fraction& frc, const Matrix& mat);
Matrix Transpose(const Matrix& mat);
Matrix GaussEliminate(const Matrix& mat,int* rankptr=nullptr,SelectArray** sarray=nullptr);
fraction Gdet(const Matrix& mat);
Matrix Ginverse(const Matrix& mat);
Matrix RowSum(const Matrix& mat);
Matrix ColSum(const Matrix& mat);
Matrix LeftNullSpace(const Matrix& mat);
Matrix NullSpace(const Matrix& mat);

//Additional functions
Matrix GenSumMat(int);
Matrix Identity(int);





void ShowMatrix(fraction*& ptr_data);

//Advanced functions


/*&&&&&&&&*************&&&&&&&&&---------------=============++++++++++++++&&&&&&*/
class Matrix:public fraction
{
	friend Matrix InputMatrix(const int& row, const int& column);
	friend Matrix operator+(const Matrix& mat1, const Matrix& mat2);
	friend Matrix operator-(const Matrix& mat);
	friend Matrix operator-(const Matrix& mat1, const Matrix& mat2);
	friend bool operator==(const Matrix& mat1, const Matrix& mat2);

	friend ostream& operator<<(ostream& out, const Matrix& mat);
	friend fraction det(const Matrix& mat);
	friend Matrix reduce(const Matrix& mat, int DeleteCol, int DeleteRow );
	friend Matrix inverse(const Matrix& mat);
	friend Matrix adj(const Matrix&mat);
	friend Matrix operator*(const Matrix& mat1, const Matrix& mat2);
	friend Matrix operator*(const fraction& frc, const Matrix& mat);
	friend Matrix Transpose(const Matrix& mat);
	friend Matrix GaussEliminate(const Matrix& mat,int* rankptr,SelectArray** sarray);
	friend fraction Gdet(const Matrix& mat);
	friend Matrix Ginverse(const Matrix& mat);
	friend Matrix LeftNullSpace(const Matrix& mat);

	friend Matrix RowSum(const Matrix& mat);
	friend Matrix ColSum(const Matrix& mat);

	friend static Matrix matmerge_row(const Matrix& mat1, const Matrix& mat2);




public:
		Matrix()//Standard blank matrix.The nullptr indicates that it is a blank matrix.
		{
			row = 0;
			column = 0;
			ptr = nullptr;
			MatrixCount++;
		}

		Matrix(int rowNum, int columnNum) :row(rowNum), column(columnNum)
		{
			if (row <= 0 || column <= 0)throw MATH_ERROR;
			ptr = new fraction*[row];
			for (int i = 1;i <= row;i++)
			{
				ptr[i - 1] = new fraction[column]();
			}
			MatrixCount++;
		}

		Matrix(const Matrix& mat) :row(mat.row), column(mat.column)//the Copy Constructor includes simplification
		{
			if (mat.row <= 0 || mat.column <= 0 || mat.ptr == nullptr)
			{
				row = 0;
				column = 0;
				ptr = nullptr;
				MatrixCount++;
				return;
			}

			ptr = new fraction*[row];

			for (int i = 0;i < row;i++)
			{
				ptr[i] = new fraction[column];
				for (int j = 0;j < column;j++)
				{
					ptr[i][j] = simplify(mat.ptr[i][j]);
				}
			}
			MatrixCount++;
		}

		Matrix& operator=(const Matrix& mat);

		bool ValidityCheck()const
		{
			if (column <= 0 ||row <= 0 ||ptr == nullptr)throw MAT_ERROR;
			return true;
		}

		void swap(int lineA, int lineB);
		void add(int lineA, int lineB, fraction rate);
		void mult(int line, fraction rate);

		void col_swap(int lineA, int lineB);
		void col_add(int lineA, int lineB, fraction rate);
		void col_mult(int line, fraction rate);

		int rank();
		Matrix EigenEqu();//return a 1xn matrix.
		
		virtual ~Matrix()
		{
			for (int i = 0;i < row;i++)
			{
				delete[] ptr[i];
			}
			delete[] ptr;
			MatrixCount--;
		}

		virtual fraction** GetPtr() { return ptr; }
		virtual int GetType() const { return 1; }
		virtual Matrix* GetThis() { return this; }

		Matrix GetRow(int Row);
		Matrix GetColumn(int Column);
		void ReplaceColumn(const Matrix& B, int pos_col);

		int GetRowCnt()const { return row; }
		int GetColCnt()const { return column; }
		static int GetMatrixCnt() {return MatrixCount;}

private:
	int row;
	int column;
	fraction** ptr;
	static int MatrixCount;
};

//SelectArray
struct SelectArray
{

	SelectArray(int ArrLength):ArrayLength(ArrLength)//ArrLength>=1
	{
		SArrayPtr = new bool[ArrayLength];
		//Initialize as false?
		return;
	}

	void initialize(bool initAsOne=true)
	{
		OverFlowFlag = false;
		SArrayPtr[0] = initAsOne;//InitialValue=0001;
		for (int i = 1;i < ArrayLength;i++)
		{
			SArrayPtr[i] = false;
		}
		return;
	}

	~SelectArray()
	{
		delete[] SArrayPtr;
	}

	

	void operator++(int)
	{
		for (int i = 0;i < ArrayLength;i++)
		{
			if (!SArrayPtr[i])
			{
				SArrayPtr[i] = true;
				return;
			}
			else
			{
				SArrayPtr[i] = false;
			}
		}
		OverFlowFlag = true;
		return;
	}

	int truecnt(void)const
	{
		int cnt = 0;
		for (int i = 0;i < ArrayLength;i++)
		{
			if (SArrayPtr[i])cnt++;
		}
		return cnt;
	}

	int falsecnt(void)const
	{
		return ArrayLength - truecnt();
	}


	bool OverFlowFlag;
	bool* SArrayPtr;
	int ArrayLength;
};


#endif