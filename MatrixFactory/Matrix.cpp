#include "stdafx.h"
#include "Math.h"

extern const fraction frc_zero;
extern const fraction frc_one;
extern const double precision;

extern const char* _Matrix_Math_Error;
extern const char* _Matrix_Size_Error;
extern const char* _Matrix_Pointer_Corrupted;

static void LineValidity(const Matrix* ptr_This, bool isRow, int lineA, int lineB = -1)
{
	if (lineB != -1)//Check B
	{
		if (isRow)//Row
		{
			if (lineB < 0 || lineB >= (ptr_This->GetRowCnt()))throw Exceptions(_Matrix_Size_Error);
		}
		else//Column
		{
			if (lineB < 0 || lineB >= (ptr_This->GetColCnt()))throw Exceptions(_Matrix_Size_Error);
		}
	}
	if (isRow)//isRow==true
	{

		if (lineA< 0 || lineA >= (ptr_This->GetRowCnt()))throw Exceptions(_Matrix_Size_Error);
	}
	else
	{
		if (lineA< 0 || lineA >= (ptr_This->GetColCnt()))throw Exceptions(_Matrix_Size_Error);
	}

	return;
}

bool Matrix::ValidityCheck()const
{
	if (column <= 0 || row <= 0 || ptr == nullptr)throw Exceptions(_Matrix_Size_Error);
	return true;
}

Matrix::Matrix(const Matrix& mat) :row(mat.row), column(mat.column)//the Copy Constructor includes simplification
{
	if (mat.row <= 0 || mat.column <= 0 || mat.ptr == nullptr)
	{
		row = 0;
		column = 0;
		ptr = nullptr;
		MatrixCount++;
		return;
	}

	ptr = new Math*[row];
	fraction* temp_ptr;

	for (int i = 0;i < row;i++)
	{
		ptr[i] = (Math*)new fraction[column];
		for (int j = 0;j < column;j++)
		{
			if ((temp_ptr = (dynamic_cast<fraction*>(mat.ptr[i]) + j)) == nullptr)throw Exceptions(_Matrix_Pointer_Corrupted);//Problems here.
			*(dynamic_cast<fraction*>(ptr[i]) + j) = simplify(*temp_ptr);
		}
	}
	MatrixCount++;
}

void Matrix::AbortPreciseCalculation()
{
	ValidityCheck();
	for (int i = 0;i < row;i++)
	{
		for (int j = 0;j < column;j++)
		{
			(*this)(i, j).AbortPreciseCalculation();
		}
	}
}

Matrix& Matrix::operator=(const Matrix& mat)
{
	if (mat.column <= 0 || mat.row <= 0 || mat.ptr == nullptr)
	{
		this->column = 0;
		this->row = 0;
		this->ptr = nullptr;
		return *this;
	}//a void matrix.

	if (this != &mat)//Two different matrices.
	{
		fraction* temp_ptr;
		if (row == mat.row && column == mat.column)//If the two matrices are of the same size, then copy the data directly.
		{
			for (int i = 0;i < row;i++)
			{
				for (int j = 0;j < column;j++)
				{
					if ((temp_ptr = (dynamic_cast<fraction*>(mat.ptr[i]) + j)) == nullptr)throw Exceptions(_Matrix_Pointer_Corrupted);
					*(dynamic_cast<fraction*>(ptr[i]) + j) = simplify(*temp_ptr);
				}
			}
		}
		else
		{
			if (this->ptr != nullptr)
			{
				for (int i = 0;i < row;i++)//delete the old contents.
				{
					delete[] dynamic_cast<fraction*>(ptr[i]);
				}
				delete[] ptr;
			}
			row = mat.row; //copy normal data members
			column = mat.column;

			ptr = new Math*[row];    //Reallocate.
			for (int i = 0;i < row;i++)
			{
				ptr[i] = (Math*)new fraction[column];
				for (int j = 0;j < column;j++)
				{
					if ((temp_ptr = (dynamic_cast<fraction*>(mat.ptr[i]) + j)) == nullptr)throw Exceptions(_Matrix_Pointer_Corrupted);//Problems here.
					*(dynamic_cast<fraction*>(ptr[i]) + j) = simplify(*temp_ptr);
				}
			}

		}
	}
	return *this;
}

fraction& Matrix::operator()(int i, int j)const
{
	LineValidity(this, true, i);
	LineValidity(this, false, j);
	return *(dynamic_cast<fraction*>(ptr[i]) + j);
}

ostream& operator<<(ostream& ostr, const Matrix& mat)//Need further investigation.
{
	try
	{
		int row = mat.GetRowCnt();
		int col = mat.GetColCnt();
		int space_resolved = 0;
		for (int i = 0;i < row;i++)
		{
			for (int j = 0;j < col;j++)
			{
				space_resolved = (space_resolved >= mat(i, j).GetLength()) ? space_resolved : mat(i, j).GetLength();
			}
		}
		space_resolved += 3;

		for (int i = 0;i < row;i++)
		{
			ostr << '[';
			for (int j = 0;j < col;j++)
			{
				ostr.width(space_resolved);
				fraction t = mat(i, j);
				if (t.GetApprox())
				{
					ostr << t.GetValue();
				}
				else
				{
					ostr << t;
				}
			}
			ostr << " ]" << endl;
		}
	}
	catch (...)
	{
		cout << "Exceptions caught in Matrix operator<<." << endl;
		throw;
	}
	return ostr;
}

Matrix operator+(const Matrix& mat1, const Matrix& mat2)
{
	mat1.ValidityCheck();
	mat2.ValidityCheck();
	if (mat1.column != mat2.column || mat1.row != mat2.row)throw Exceptions(_Matrix_Math_Error);

	Matrix ans(mat1.row, mat1.column);

	for (int i = 0;i < ans.row;i++)
	{
		for (int j = 0;j < ans.column;j++)
		{
			ans(i, j) = mat1(i, j) + mat2(i, j);
		}
	}
	return ans;
}

Matrix operator-(const Matrix& mat)
{
	mat.ValidityCheck();
	Matrix ans(mat.row, mat.column);
	for (int i = 0;i < mat.row;i++)
	{
		for (int j = 0;j < mat.column;j++)
		{
			ans(i, j) = -mat(i, j);
		}
	}
	return ans;
}

Matrix operator-(const Matrix& mat1, const Matrix& mat2)
{
	mat1.ValidityCheck();
	mat2.ValidityCheck();
	if (mat1.column != mat2.column || mat1.row != mat2.row)throw Exceptions(_Matrix_Math_Error);

	Matrix ans(mat1.row, mat1.column);

	for (int i = 0;i < ans.row;i++)
	{
		for (int j = 0;j < ans.column;j++)
		{
			ans(i, j) = mat1(i, j) - mat2(i, j);
		}
	}
	return ans;
}

bool operator==(const Matrix& mat1, const Matrix& mat2)
{
	mat1.ValidityCheck();
	mat2.ValidityCheck();
	if (mat1.row != mat2.row || mat1.column != mat2.column)return false;

	for (int i = 0;i < mat1.row;i++)
	{
		for (int j = 0;j < mat1.column;j++)
		{
			if (!(mat1(i, j) == mat2(i, j)))return false;
		}
	}
	return true;
}

bool operator!=(const Matrix& mat1, const Matrix& mat2)
{
	return !(mat1 == mat2);
}


Matrix reduce(const Matrix& mat, int DeleteCol, int DeleteRow)
{
	mat.ValidityCheck();
	LineValidity(&mat, true, DeleteRow);
	LineValidity(&mat, false, DeleteCol);

	Matrix ans(mat.row - 1, mat.column - 1);
	int i = 0;int j = 0;
	int a = 0;int b = 0;

	for (i = 0;i < mat.row;i++)
	{
		if (i == DeleteRow) continue;
		for (j = 0;j < mat.column;j++)
		{
			if (j == DeleteCol) continue;
			ans(a, b) = mat(i, j);
			b++;
		}
		b = 0;
		a++;
	}
	return ans;
}

fraction det(const Matrix& mat)
{
	if (mat.column != mat.row)throw Exceptions(_Matrix_Size_Error);
	mat.ValidityCheck();

	switch (mat.row)
	{
	case 1:
		return mat(0, 0);break;
	case 2:
		return mat(0, 0)* mat(1, 1) - mat(0, 1) * mat(1, 0);		break;
	default:
		fraction sum = frc_zero;
		for (int i = 0;i < mat.row;i++)
		{
			if (i % 2 == 0)
			{
				sum += mat(0, i) * det(reduce(mat, i));
			}
			else
			{
				sum -= mat(0, i) * det(reduce(mat, i));
			}
		}
		return sum;
	}
}

Matrix inverse(const Matrix& mat)
{
	if (mat.column != mat.row)throw Exceptions(_Matrix_Size_Error);
	mat.ValidityCheck();
	fraction A = det(mat);	int n = mat.row;
	Matrix InverseMatrix(n, n);

	if (A == frc_zero)
		throw Exceptions(_Matrix_Math_Error);

	fraction B = reciprocal(A);

	if (mat.row == 1)
	{
		InverseMatrix(0, 0) = B;
		return InverseMatrix;
	}

	for (int i = 0;i < n;i++)
	{
		for (int j = 0;j < n;j++)
		{
			if ((i + j) % 2 == 0)
			{
				InverseMatrix(j, i) = B * det(reduce(mat, j, i));
			}
			else
			{
				InverseMatrix(j, i) = -B * det(reduce(mat, j, i));
			}
		}
	}
	return InverseMatrix;
}

Matrix adj(const Matrix& mat)
{
	if (mat.column != mat.row)throw Exceptions(_Matrix_Size_Error);
	mat.ValidityCheck();

	int n = mat.row;
	Matrix AdjugateMatrix(n, n);
	if (mat.row == 1)
	{
		AdjugateMatrix(0, 0) = frc_one;
		return AdjugateMatrix;
	}

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if ((i + j) % 2 == 0)
			{
				AdjugateMatrix(j, i) = det(reduce(mat, j, i));
			}
			else
			{
				AdjugateMatrix(j, i) = -det(reduce(mat, j, i));
			}
		}
	}

	return AdjugateMatrix;
}

Matrix operator*(const Matrix& mat1, const Matrix& mat2)
{
	mat1.ValidityCheck();
	mat2.ValidityCheck();
	if (mat1.column != mat2.row)throw Exceptions(_Matrix_Size_Error);

	Matrix ans(mat1.row, mat2.column);
	fraction temp = frc_zero;

	for (int i = 0;i < mat1.row;i++)
	{
		for (int j = 0;j < mat2.column;j++)
		{
			for (int k = 0;k < mat1.column;k++)
			{
				temp = temp + mat1(i, k) * mat2(k, j);
			}
			ans(i, j) = temp;
			temp = frc_zero;
		}
	}
	return ans;
}

Matrix operator*(const fraction& frc, const Matrix& mat)
{
	mat.ValidityCheck();
	Matrix ans(mat.row, mat.column);
	for (int i = 0;i < mat.row;i++)
	{
		for (int j = 0;j < mat.column;j++)
		{
			ans(i, j) = frc * mat(i, j);
		}
	}
	return ans;
}

Matrix operator%(const Matrix& mat1, const Matrix& mat2)
{
	Matrix M(mat1.row + mat2.row, mat1.column + mat2.column);
	for (int i = 0; i < mat1.row; i++)
	{
		for (int j = 0; j < mat1.column; j++)
		{
			M(i, j) = mat1(i, j);
		}
	}
	for (int i = 0; i < mat2.row; i++) 
	{
		for (int j = 0; j < mat2.column; j++) 
		{
			M(mat1.row + i, mat1.column + j) = mat2(i, j);
		}
	}
	return M;
}

Matrix pow(const Matrix& mat, int n)
{
	mat.ValidityCheck();
	if (mat.GetRowCnt() != mat.GetColCnt())throw Exceptions(_Matrix_Size_Error);
	if (n == 0)
	{
		return Identity(mat.GetRowCnt());
	}
	else if (n > 0)//Fast power Algorithm
	{
		Matrix  base = mat;
		Matrix ans = Identity(mat.GetRowCnt());
		int _n = n;

		while (_n)
		{
			if (_n & 1)ans = ans * base;
			base = base * base;
			_n >>= 1;
		}
		return ans;
	}
	else
	{
		return pow(Ginverse(mat), -n);
	}

}

Matrix Transpose(const Matrix& mat)
{
	Matrix ans(mat.column, mat.row);
	mat.ValidityCheck();
	for (int i = 0;i < mat.row;i++)
	{
		for (int j = 0;j < mat.column;j++)
		{
			ans(j, i) = mat(i, j);
		}
	}
	return ans;
}

void Matrix::swap(int lineA, int lineB)
{
	LineValidity(this, true, lineA, lineB);
	Math* temp = ptr[lineB];//Swap the pointers.
	ptr[lineB] = ptr[lineA];
	ptr[lineA] = temp;
	return;
}

void Matrix::add(int lineA, int lineB, const fraction& rate)
{
	LineValidity(this, true, lineA, lineB);
	for (int j = 0;j < column;j++)
	{
		this->operator()(lineB, j) = rate * this->operator()(lineA, j) + this->operator()(lineB, j);
	}
	return;
}

void Matrix::mult(int line, const fraction& rate)
{
	LineValidity(this, true, line);
	for (int j = 0;j < column;j++)
	{
		(this->operator()(line, j)) *= rate;
	}
	return;
}

void Matrix::col_swap(int lineA, int lineB)//swap two columns.
{
	LineValidity(this, false, lineA, lineB);
	fraction temp;//Initialize as zero.
	for (int i = 0;i < row; i++)
	{
		temp = this->operator()(i, lineA);
		this->operator()(i, lineA) = this->operator()(i, lineB);
		this->operator()(i, lineB) = temp;
	}
	return;
}

void Matrix::col_add(int lineA, int lineB, const fraction& rate)
{
	LineValidity(this, false, lineA, lineB);
	for (int i = 0;i < row;i++)
	{
		this->operator()(i, lineB) += this->operator()(i, lineA)* rate;
	}
	return;
}

void Matrix::col_mult(int line, const fraction& rate)
{
	LineValidity(this, false, line);
	for (int i = 0;i < row;i++)
	{
		this->operator()(i, line) *= rate;
	}
	return;
}

//Matrix GetCol/GetRow

Matrix Matrix::GetRow(int Row)const
{
	LineValidity(this, true, Row);//Check if the variable Row is legal.

	Matrix ans(1, column);
	for (int j = 0;j < column;j++)
	{
		ans.operator()(0, j) = this->operator()(Row, j);	//Copy the data.
	}
	return ans;
}

Matrix Matrix::GetColumn(int Column)const 
{
	LineValidity(this, false, Column);//Check if the variable Row is legal.

	Matrix ans(row, 1);
	for (int i = 0;i < row;i++)
	{
		ans.operator()(i, 0) = this->operator()(i, Column);	//Copy the data.
	}
	return ans;
}

Matrix GaussEliminate(const Matrix& mat, int* rankptr, SelectArray** sarray)
{
	mat.ValidityCheck();
	Matrix ans(mat);
	int i = 0;

	bool NeedToSelect = false;//SelectArray.
	if (sarray != nullptr)
	{
		*sarray = new SelectArray(mat.column);
		(*sarray)->initialize(false);//initialize as all false.
		NeedToSelect = true;
	}

	for (int j = 0;j < mat.column;j++)// Go through all the column vectors.
	{
		int temp_maxNumPos = -1;
		double temp_maxNumAbs = 0.0;
		double tmp;
		for (int i_rec = i;i_rec< mat.row;i_rec++)//Find the main number.From line i to the end.
		{
			if ((tmp = abs(ans(i_rec, j).GetValue())) > temp_maxNumAbs)
			{
				temp_maxNumAbs = tmp;
				temp_maxNumPos = i_rec;
			}
		}
		if (temp_maxNumAbs < precision || temp_maxNumPos == -1)//Zero.
		{
			if (NeedToSelect)(*sarray)->SArrayPtr[j] = true;//It is not a pivot column. Label it as true.
			continue;
		}

		//Swap the main number to the nth row.
		ans.swap(i, temp_maxNumPos);
		//Reduce it to 1 by division.
		ans.mult(i, reciprocal(ans(i, j)));
		//Eliminate.
		for (int i_rec = 0;i_rec < mat.row;i_rec++)
		{
			if (ans(i_rec,j) != frc_zero && i_rec != i)
			{
				ans.add(i, i_rec, -ans(i_rec, j));
			}
		}

		i++;
		if (i == mat.row)break;
	}

	if (rankptr != nullptr)
	{
		*rankptr = i;
	}
	return ans;
}

static Matrix matmerge_row(const Matrix& mat1, const Matrix& mat2)
{
#ifdef DEBUGGING
	assert(mat1.row == mat2.row);
	mat1.ValidityCheck();
	mat2.ValidityCheck();
#endif
	Matrix ans(mat1.row, mat1.column + mat2.column);
	for (int i = 0;i < mat1.row;i++)
	{
		for (int j = 0;j < ans.column;j++)
		{
			if (j < mat1.column)
			{
				ans(i, j) = mat1(i, j);
			}
			else
			{
				ans(i, j) = mat2(i, j - mat1.column);
			}
		}
	}
	return ans;
}

Matrix LeftNullSpace(const Matrix& mat)
{
	int n = mat.row;
	int rank;
	GaussEliminate(mat, &rank);
	if (rank == mat.row)return Matrix(1, mat.row);//A zero matrix.
	Matrix merged_row = matmerge_row(mat, Identity(n));
	Matrix U = GaussEliminate(merged_row);
	Matrix E(n, n);//EA=U


	for (int i = 0;i < n;i++)
	{
		for (int j = 0;j < n;j++)
		{
			E(i, j) = U(i, j + mat.column);
		}
	}

	//Extract the left nullspace matrix.
	Matrix LeftNullSpace(mat.row - rank, mat.row);
	for (int i = rank;i < mat.row;i++)
	{
		for (int j = 0;j < n;j++)
			LeftNullSpace(i - rank, j) = E(i, j);
	}
	return LeftNullSpace;
}

Matrix NullSpace(const Matrix& mat)
{
	return Transpose(LeftNullSpace(Transpose(mat)));
}


//Gauss det Algorithm
fraction Gdet(const Matrix& mat)
{
	if (mat.column != mat.row)throw Exceptions(_Matrix_Size_Error);
	mat.ValidityCheck();

	Matrix ans(mat);
	int i = 0;
	int swapcnt = 0;
	for (int j = 0;j < mat.column;j++)
	{
		int temp_maxNumPos = -1;
		double temp_maxNumAbs = 0.0;
		double temp;
		for (int i_rec = i;i_rec < mat.row;i_rec++)//Find the main number.From line i to the end.
		{
			if ((temp = abs(ans(i_rec, j).GetValue())) > temp_maxNumAbs)
			{
				temp_maxNumAbs = temp;
				temp_maxNumPos = i_rec;
			}
		}
		if (temp_maxNumAbs < precision || temp_maxNumPos == -1)//Zero.
		{
			return frc_zero;
		}

		//Swap the main number to the nth row.
		if (i != temp_maxNumPos)
		{
			ans.swap(i, temp_maxNumPos);
			swapcnt++;
		}

		fraction tmp_reci = reciprocal(ans(i, j));
		//Eliminate.
		for (int i_rec = i + 1;i_rec < mat.row;i_rec++)//Triangularize.
		{
			if (ans(i_rec, j) != frc_zero)
			{
				ans.add(i, i_rec, -ans(i_rec, j) * tmp_reci);
			}
		}
		i++;
	}

	fraction detValue = frc_one;

	for (int i = 0;i < mat.row;i++)
	{
		detValue *= ans(i, i);
	}

	if (swapcnt % 2 == 0)
	{
		return detValue;
	}
	else
	{
		return -detValue;
	}
}

//Gauss inverse Algorithm
Matrix Ginverse(const Matrix& mat)
{
	if (mat.column != mat.row)throw Exceptions(_Matrix_Size_Error);
	mat.ValidityCheck();

	//Matrix Definition.
	Matrix ans(mat);
	Matrix E = Identity(mat.row);

	int i = 0;
	for (int j = 0;j < mat.column;j++)
	{
		int temp_maxNumPos = -1;
		double temp_maxNumAbs = 0.0;
		double temp;
		for (int i_rec = i;i_rec < mat.row;i_rec++)//Find the main number.From line i to the end.
		{
			if ((temp = abs(ans(i_rec, j).GetValue())) > temp_maxNumAbs)
			{
				temp_maxNumAbs = temp;
				temp_maxNumPos = i_rec;
			}
		}
		if (temp_maxNumAbs < precision || temp_maxNumPos == -1)//Zero detValue.
		{
			throw Exceptions(_Matrix_Math_Error);
		}

		//Swap the main number to the i-th row.
		if (temp_maxNumPos != i)
		{
			ans.swap(i, temp_maxNumPos);
			E.swap(i, temp_maxNumPos);
		}

		//Reduce it to 1 by division.
		fraction tmp_reci = reciprocal(ans(i, j));
		ans.mult(i, tmp_reci);
		E.mult(i, tmp_reci);

		//Eliminate.
		for (int i_rec = 0;i_rec < mat.row;i_rec++)
		{
			if (!(ans(i_rec, j) == frc_zero) && i_rec != i)
			{
				fraction negative_ans = -ans(i_rec, j);
				ans.add(i, i_rec, negative_ans);
				E.add(i, i_rec, negative_ans);//The same operation.
			}
		}

		i++;
	}

	return E;
}

int Matrix::rank()const
{
	//it is unnecessary to check matrix validity.
	int rank_value;
	Matrix temp = GaussEliminate(*this, &rank_value);
	return rank_value;
}

void Matrix::ReplaceColumn(const Matrix& B, int pos_col)
{
	LineValidity(this, false, pos_col);
	//Check if B is the required shape.
	if (row != B.row)throw Exceptions(_Matrix_Size_Error);
	if (B.column != 1)throw Exceptions(_Matrix_Size_Error);

	for (int i = 0;i < row;i++)
		this->operator()(i, pos_col) = B.operator()(i, 0);
	return;
}

Matrix Matrix::EigenEqu()
{
	//Check.
	ValidityCheck();
	if (row != column)throw Exceptions(_Matrix_Size_Error);

	//int n=column;
	Matrix eigen_ans(1, column + 1);
	SelectArray s_array(column);
	s_array.initialize();
	eigen_ans(0, 0) = frc_one;

	Matrix temp;
	while (!s_array.OverFlowFlag)
	{
		temp = *this;
		for (int i = row - 1;i >= 0;i--)
		{
			if (!(s_array.SArrayPtr[i]))
			{
				temp = reduce(temp, i, i);
			}
		}
		eigen_ans(0, temp.row) += Gdet(temp);
		s_array++;
	}

	for (int i = 0;i < column + 1;i++)
	{
		if (i % 2 != 0)eigen_ans(0, i) = -eigen_ans(0, i);
	}
	return eigen_ans;
}

Matrix Identity(int n)
{
	if (n <= 0)throw Exceptions(_Matrix_Size_Error);
	Matrix ans(n, n);
	for (int i = 0;i < n;i++)
	{
		ans(i, i) = frc_one;
	}
	return ans;
}