#include "stdafx.h"
#include "ExpressionAnalyzer.h"
#define INVALID_VECTOR "This is an invalid vector!"
#define VECTOR_DIMENSION_UNMATCHED "Vector Dimension Unmatched!"
int Matrix::MatrixCount = 0;
extern bool(*Clear)(int AnalyzeResult, bool initialize);

fraction InnerProduct(Matrix &A, Matrix &B) {
	if (A.GetColCnt() != 1 || B.GetColCnt() != 1) throw MAT_ERROR;
	else if (A.GetRowCnt() != B.GetRowCnt()) throw MAT_ERROR;
	Matrix C = Transpose(A)*B;
	return C.GetPtr()[0][0];
}
fraction norm(Matrix& A) {
	if (A.GetColCnt() != 1) throw MAT_ERROR;
	return sqrt(InnerProduct(A, A).GetValue());
}
Matrix DirectSum(Matrix &M1, Matrix&M2) {
	Matrix M(M1.GetRowCnt() + M2.GetRowCnt(), M1.GetColCnt() + M2.GetColCnt());
	for (int i = 0; i < M1.GetRowCnt(); i++) {
		for (int j = 0; j < M1.GetColCnt(); j++) {
			M.GetPtr()[i][j] = M1.GetPtr()[i][j];
		}
	}
	for (int i = 0; i < M2.GetRowCnt(); i++) {
		for (int j = 0; j < M2.GetColCnt(); j++) {
			M.GetPtr()[(M1.GetRowCnt()+i)][(M1.GetColCnt()+j)] = M2.GetPtr()[i][j];
		}
	}
	return M;
}
Matrix SubMatrix(Matrix &A, int row_p, int col_p) {
	Matrix M((A.GetRowCnt() - row_p), (A.GetColCnt() - col_p));
	for (int i = 0; i < M.GetRowCnt(); i++) {
		for (int j = 0; j < M.GetColCnt(); j++) {
			M.GetPtr()[i][j] = A.GetPtr()[row_p+i][col_p+j];
		}
	}
	return M;
}
bool isUpperTriangular(Matrix& M) {
	if (M.GetRowCnt() != M.GetColCnt()) return false;
	for (int i = 0; i < M.GetColCnt(); i++) {
		for (int j = i+1; j < M.GetRowCnt(); j++) {
			if (abs(M.GetPtr()[j][i].GetValue())>1e-6) return false;
		}
	}
	return true;
}
int QR(Matrix& A, Matrix& Q, Matrix& R) {
	if (A.rank() != A.GetColCnt()){
		return -1;
	}
	if (A.GetColCnt() != Q.GetColCnt() || A.GetRowCnt() != Q.GetRowCnt()) {
		return -1;
	}
	if (A.GetColCnt() != R.GetColCnt() || A.GetColCnt() != R.GetRowCnt()) {
		return -1;
	}
	for (int i = 0; i < A.GetColCnt(); i++) {
		Matrix cur = Matrix(A.GetColumn(i));
		for (int j = 0; j < i; j++) {
			cur = cur - InnerProduct(Q.GetColumn(j), cur)*(Q.GetColumn(j));
		}
		cur = reciprocal(norm(cur)) * cur;
		for (int k = 0; k < Q.GetRowCnt(); k++) {
			Q.GetPtr()[k][i] = cur.GetPtr()[k][0];
		}
	}
	R = Transpose(Q)*A;
	return 0;
}
int QR2(Matrix &A, Matrix& Q, Matrix& R) {
	Q = Identity(A.GetRowCnt());
	R = A;
	//cout << R;
	for (int i = 0; i < A.GetRowCnt()-1; i++) {
		Matrix v = (SubMatrix(R, i, i).GetColumn(0));
		//cout << v <<endl;
		Matrix H(v.GetRowCnt(), v.GetRowCnt());
		//cout << H << endl;
		Matrix e1 = Identity(v.GetRowCnt()).GetColumn(0);
		//cout << e1 << endl;
		Matrix u = (v - (norm(v))*e1);
		if(norm(u) != (fraction)0) u = (reciprocal(norm(u)))*u;
		//cout << u << endl;
		H = Identity(v.GetRowCnt()) - ((fraction)2*u*Transpose(u));
		//cout << H << endl;
		if(i>0) H = Identity(i) % H;
		//cout << H << endl;
		R = H * R;
		//cout << R << endl;
		Q = Q * Transpose(H);
	}
	return 0;
}
Matrix Eig(Matrix &A) {
	if (A.GetColCnt() != A.GetRowCnt()) throw MAT_ERROR;
	else if (A.GetRowCnt() == 0) throw MAT_ERROR;
	Matrix Q(A.GetRowCnt(), A.GetRowCnt());
	Matrix R(A.GetRowCnt(), A.GetColCnt());
	QR2(A, Q, R);
	while (!isUpperTriangular(R*Q)) {
		//cout << (R*Q);
		QR2(R*Q, Q, R);
	}
	//cout << (R*Q) << endl;
	Matrix result = Matrix(A.GetRowCnt(), 1);
	for (int i = 0; i < A.GetRowCnt(); i++) {
		result.GetPtr()[i][0] = (Q*R).GetPtr()[i][i];
	}
	return result;
}
void EigV(Matrix& A) {
	if (A.GetColCnt() != A.GetRowCnt()) throw MAT_ERROR;
	else if (A.GetRowCnt() == 0) throw MAT_ERROR;
	Matrix eigList = Eig(A);
	cout << eigList << endl;
	for (int i = 0; i < eigList.GetRowCnt(); i++) {
		Matrix C = A - (eigList.GetPtr()[i][0])*Identity(A.GetRowCnt());
		if (det(C) == (fraction)0) {
			cout << C << endl;
			cout << NullSpace(C) << endl;
		}
		else {
			C = adj(C);
			cout << C << endl;
			Matrix tempEigV = Identity(C.GetRowCnt());
			for (int i = 0; i < 100; i++) {
				tempEigV = C * tempEigV;
				for (int i = 0; i < tempEigV.GetRowCnt(); i++) tempEigV.ReplaceColumn(reciprocal(norm(tempEigV.GetColumn(i)))*tempEigV.GetColumn(i), i);
			}
			cout << tempEigV << endl;
		}
	}
}

int main() {
	Clear = Analyze_Change;
	int row, column;
	cout << "Please Enter the size of the square matrix you want to get eigenvalues and eigenvectors." << endl;
	cout << "Size: ";
	cin >> row;
	cout << "Input the matrix row by row: ";
	Matrix A = InputMatrix(row, row);
	EigV(A);
	/*Matrix Q = Matrix(row, row);
	Matrix R = Matrix(row, column);
	QR2(A, Q, R);
	cout << Q << endl;
	cout << R << endl;
	cout << isUpperTriangular(R) << endl;
	cout << (Q*R) << endl;*/
	return 0;
}