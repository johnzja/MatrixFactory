
#include "stdafx.h"
#include "ExpressionAnalyzer.h"

//extern bool(*Clear)(int AnalyzeResult, bool initialize);
extern const char* _Invalid_Vector;
extern const char* _Vector_Dim_Umnatched;
extern const char* _Matrix_Size_Error;
extern const char* _Matrix_Math_Error;

extern const fraction frc_zero;




fraction InnerProduct(const Matrix &A, const Matrix &B) 
{
	if (A.GetColCnt() != 1 || B.GetColCnt() != 1) throw Exceptions(_Matrix_Size_Error);
	else if (A.GetRowCnt() != B.GetRowCnt()) throw Exceptions(_Matrix_Size_Error);
	Matrix C = Transpose(A)*B;
	return C(0, 0);
}

fraction norm(const Matrix& A)
{
	if (A.GetColCnt() != 1) throw Exceptions(_Matrix_Size_Error);
	return sqrt(InnerProduct(A, A).GetValue());
}

Matrix SubMatrix(const Matrix &A, int row_p, int col_p) 
{
	Matrix M((A.GetRowCnt() - row_p), (A.GetColCnt() - col_p));
	for (int i = 0; i < M.GetRowCnt(); i++) 
	{
		for (int j = 0; j < M.GetColCnt(); j++) 
		{
			M(i, j) = A(row_p + i, col_p + j);
		}
	}
	return M;
}

Matrix setZero(const Matrix &M) 
{
	Matrix result = M;
	for (int i = 0; i < M.GetRowCnt(); i++) 
	{
		for (int j = 0; j < M.GetColCnt(); j++) 
		{
			if (abs(M(i, j).GetValue()) < 1e-6) result(i, j) = frc_zero;
		}
	}
	return result;
}

bool isUpperTriangular(const Matrix& M) 
{
	if (M.GetRowCnt() != M.GetColCnt()) return false;
	for (int i = 0; i < M.GetColCnt(); i++)
	{
		for (int j = i + 1; j < M.GetRowCnt(); j++)
		{
			if (abs(M(j, i).GetValue()) > 1e-9) return false;
		}
	}
	return true;
}

int QR(Matrix& A, Matrix& Q, Matrix& R)
{
	if (A.rank() != A.GetColCnt()) throw Exceptions(_Matrix_Size_Error);
	if (A.GetColCnt() != Q.GetColCnt() || A.GetRowCnt() != Q.GetRowCnt()) throw Exceptions(_Matrix_Size_Error);
	if (A.GetColCnt() != R.GetColCnt() || A.GetColCnt() != R.GetRowCnt()) throw Exceptions(_Matrix_Size_Error);

	for (int i = 0; i < A.GetColCnt(); i++)
	{
		Matrix cur = Matrix(A.GetColumn(i));
		for (int j = 0; j < i; j++)
		{
			cur = cur - InnerProduct(Q.GetColumn(j), cur)*(Q.GetColumn(j));
		}
		cur = reciprocal(norm(cur)) * cur;
		for (int k = 0; k < Q.GetRowCnt(); k++)
		{
			Q(k, i) = cur(k, 0);
		}
	}
	R = Transpose(Q)*A;
	return 0;
}

int QR2(Matrix &A, Matrix& Q, Matrix& R) 
{
	if (Q.GetRowCnt() != A.GetRowCnt() || Q.GetRowCnt() != Q.GetRowCnt()) throw Exceptions(_Matrix_Size_Error);
	if (A.GetColCnt() != R.GetColCnt() || A.GetRowCnt() != R.GetRowCnt()) throw Exceptions(_Matrix_Size_Error);
	Q = Identity(A.GetRowCnt());
	R = A;
	for (int i = 0; i < A.GetRowCnt()-1; i++) {
		Matrix v = (SubMatrix(R, i, i).GetColumn(0));
		Matrix H(v.GetRowCnt(), v.GetRowCnt());
		Matrix e1 = Identity(v.GetRowCnt()).GetColumn(0);
		Matrix u = (v - (norm(v))*e1);
		if(norm(u) != (fraction)0) u = (reciprocal(norm(u)))*u;
		H = Identity(v.GetRowCnt()) - ((fraction)2*u*Transpose(u));
		if(i>0) H = Identity(i) % H;
		R = H * R;
		Q = Q * Transpose(H);
	}
	return 0;
}

Matrix Eig(Matrix &A) 
{
	if (A.GetColCnt() != A.GetRowCnt()) throw Exceptions(_Matrix_Size_Error);
	else if (A.GetRowCnt() == 0) throw Exceptions(_Matrix_Size_Error);
	Matrix Q(A.GetRowCnt(), A.GetRowCnt());
	Matrix R(A.GetRowCnt(), A.GetColCnt());
	QR2(A, Q, R);
	int iteration = 0;
	while (!isUpperTriangular(R*Q) && iteration < 5000) 
	{
		QR2(R*Q, Q, R);
		iteration++;
	}
	QR2(setZero(R*Q), Q, R);
	Matrix result = Matrix(A.GetRowCnt(), 1);

	for (int i = 0; i < A.GetRowCnt(); i++) 
	{
		result(i, 0) = (Q*R)(i, i);
	}
	return result;
}

int DiagonalizeRealSymmetricMatrix(Matrix &A, Matrix&Q, Matrix&D)
{
	if (A != Transpose(A)) throw Exceptions(_Matrix_Math_Error);
	if (D.GetRowCnt() != D.GetColCnt() || D.GetRowCnt() != A.GetRowCnt()) throw Exceptions(_Matrix_Size_Error);
	Q = Identity(A.GetRowCnt());
	Matrix Qtmp(A.GetRowCnt(), A.GetRowCnt());
	QR2(A, Qtmp, D);
	int iteration = 0;
	while (!isUpperTriangular(D*Qtmp) && iteration < 5000)
	{
		Q = Q * Qtmp;
		QR2(D*Qtmp, Qtmp, D);
		iteration++;
	}
	D = Qtmp * D;
	for (int i = 0; i < D.GetRowCnt(); i++)
	{
		for (int j = 0; j < D.GetColCnt(); j++)
		{
			if (i != j) D(i, j) = frc_zero;
		}
	}
	cout << Qtmp << endl;
	cout << D << endl;
	cout << Q << endl;

	return 0;
}

void EigV(Matrix& A)
{
	if (A.GetColCnt() != A.GetRowCnt()) throw Exceptions(_Matrix_Size_Error);
	else if (A.GetRowCnt() == 0) throw Exceptions(_Matrix_Size_Error);
	Matrix eigList = Eig(A);
	cout << eigList << endl;
	for (int i = 0; i < eigList.GetRowCnt(); i++) 
	{
		Matrix C = A - (eigList(i, 0))*Identity(A.GetRowCnt());
		if ((det(C).GetValue()) == 0.0) 
		{
			cout << C << endl;
			cout << NullSpace(C) << endl;
		}
		else 
		{
			C = adj(C);
			C = (fraction)(pow(abs(det(C).GetValue()), (double)(-1.0 / C.GetRowCnt()))) * C;
			cout << C << endl;
			cout << det(C) << endl;
			Matrix tempEigV = Identity(C.GetRowCnt());
			for (int i = 0; i < 10; i++) 
			{
				tempEigV = C * tempEigV;
				for (int i = 0; i < tempEigV.GetRowCnt(); i++) 
				{
					if (norm(tempEigV.GetColumn(i)).GetValue() >= 1e-12) 
					{
						tempEigV.ReplaceColumn(reciprocal(norm(tempEigV.GetColumn(i)))*tempEigV.GetColumn(i), i);
					}
				}

			}
			cout << tempEigV << endl;
		}
	}
}

/*
int main() 
{
	Clear = Analyze_Change;
	int row, column;
	cout << "Please Enter the size of the square matrix you want to get eigenvalues and eigenvectors." << endl;
	cout << "Size: ";
	cin >> row;
	cout << "Input the matrix row by row: ";
	Matrix A = InputMatrix(row, row);
	cout << Eig(A);
	//EigV(A);
	Matrix Q = Matrix(row, row);
	Matrix D = Matrix(row, row);
	DiagonalizeRealSymmetricMatrix(A, Q, D);
	cout << Q << endl;
	cout << D << endl;
	cout << Q*D*inverse(Q) << endl;
 	return 0;
}
*/