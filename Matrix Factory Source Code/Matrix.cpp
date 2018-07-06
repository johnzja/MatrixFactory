#include "stdafx.h"
#include "Matrix.h"
#include "ExpressionAnalyzer.h"
using namespace std;

//consts
const int WIDTH =5;
extern fraction zero;
extern fraction one;


int GetNumLength(int& n)
{
	int i = 1;
	int absnum = abs(n);

	while ((absnum / 10) != 0)
	{
		absnum = absnum / 10;
		i++;
	}
	if (n < 0)i++;
	return i;

}

inline void displaydecimal(ostream& ost, double data, int width)
{
	ost.width(width);
	int prec = width - 1;

	if (-1 < data&& data < 1)prec--;
	if (data < 0)prec--;

	ost.precision(prec);
	ost << data;
	return;
}
//Private functions
void LineValidity(const Matrix* ptr_This, bool isRow , int lineA,int lineB = -1)
{
	if (lineB != -1)//Check B
	{
		if (isRow)//Row
		{
			if (lineB < 0 || lineB >= (ptr_This->GetRowCnt()))throw UNIDENT_ERROR;
		}
		else//Column
		{
			if (lineB < 0 || lineB >= (ptr_This->GetColCnt()))throw UNIDENT_ERROR;
		}
	}
	if (isRow)//isRow==true
	{

		if (lineA< 0 || lineA >= (ptr_This->GetRowCnt()))throw UNIDENT_ERROR;
	}
	else
	{
		if (lineA< 0 || lineA >= (ptr_This->GetColCnt()))throw UNIDENT_ERROR;
	}

	return;
}
//&&&&&&&&&&&&**********************&&&&&&&&&&&&&&&&&&&&
Matrix& Matrix::operator=(const Matrix& mat)
{
	if (mat.column <= 0|| mat.row<=0 || mat.ptr==nullptr)
	{
		this->column = 0;
		this->row = 0;
		this->ptr = nullptr;
		return *this;
	}//a void matrix.

	if (this != &mat)
	{
		if (row == mat.row && column == mat.column )//If the two matrices are of the same size, then copy the data directly.
		{
			for (int i = 0;i < row;i++)
			{
				for (int j = 0;j < column;j++)
				{
					ptr[i][j] = simplify(mat.ptr[i][j]);
				}
			}
		}
		else
		{
			if (this->ptr != nullptr)
			{
				for (int i = 0;i < row;i++)//删除原指针指向内容  
				{
					delete[] ptr[i];
				}
				delete[] ptr;
			}
			row = mat.row; //复制常规成员  
			column = mat.column;

			fraction**temp = new fraction*[row];     //复制指针指向的内容   
			for (int i = 0;i < row;i++)
			{
				temp[i] = new fraction[column];
				memcpy(temp[i], mat.ptr[i], column * sizeof(fraction));
			}
			
			ptr = temp;    //建立新指向  
		}
	}
	return *this;
}

Matrix InputMatrix(const int& row, const int& column)
{
	if (row <= 0 || column <= 0)throw MATH_ERROR;
	Matrix A(row, column);
	int i = 0;
	int  j = 0;
	string temp;
	fraction* Tmp_Calculation;

	for (i = 0;i < row;i++)
	{
		for (j = 0;j < column;j++)
		{
			cin >> temp;
			Tmp_Calculation=Calculate(temp);
			A.ptr[i][j] = *Tmp_Calculation;
			delete Tmp_Calculation;
		}
	}

	return A;

}

//double output supproted.
ostream& operator<<(ostream& out, const Matrix& mat)//It`s possible to display a blank matrix.
{
	if (mat.row <= 0 || mat.column <= 0 || mat.ptr==nullptr)
	{
		throw MAT_ERROR;
		return out;
	}

	int i, j;

	int maxlenall = WIDTH;
	int maxlen = WIDTH;

	fraction** ptrt = new fraction*[mat.row];//Initialize the length memory.
	for (i = 0;i < mat.row;i++)
	{
		ptrt[i] = new fraction[mat.column];
	}

	bool isAllint = true;

	for (i = 0;i < mat.row;i++)
	{
		for (j = 0;j < mat.column;j++)
		{
			if (mat.ptr[i][j].denominator != 1||mat.ptr[i][j].isApprox==true)isAllint = false;
			if (mat.ptr[i][j].isApprox == false)//precise number(a fraction).
			{
				ptrt[i][j].numerator = GetNumLength(mat.ptr[i][j].numerator);
				ptrt[i][j].denominator = GetNumLength(mat.ptr[i][j].denominator);

				maxlen = (maxlen < ptrt[i][j].numerator + 2) ? (ptrt[i][j].numerator + 2) : maxlen;
				maxlenall = (maxlenall < ptrt[i][j].numerator + ptrt[i][j].denominator + 3) ? (ptrt[i][j].numerator + ptrt[i][j].denominator + 3) : maxlenall;
			}
		}
	}


	for (i = 0;i < mat.row;i++)
	{
		out << "[";
		for (j = 0;j < mat.column;j++)
		{
			if (mat.ptr[i][j].isApprox == false)
			{
				if (isAllint)
				{
					out.width(maxlen);
					out << mat.ptr[i][j].numerator;
				}
				else
				{
					if (mat.ptr[i][j].denominator != 1)
					{
						out.width(maxlenall - 1 - ptrt[i][j].denominator);
						out << mat.ptr[i][j].numerator;
						out << "/";
						out.width(ptrt[i][j].denominator);
						out << mat.ptr[i][j].denominator;
					}
					else
					{
						out.width(maxlenall);
						out << mat.ptr[i][j].numerator;
					}

				}
			}
			else
			{
				out << " ";
				displaydecimal(out, mat.ptr[i][j].value, maxlenall - 1);
			}
		}
		out << " ]" << endl;
	}

	for (i = 0;i < mat.row;i++)//Release the resources required above
	{
		delete[] ptrt[i];
	}
	delete[] ptrt;

	return out;
}

Matrix operator+(const Matrix& mat1, const Matrix& mat2)
{
	mat1.ValidityCheck();
	mat2.ValidityCheck();
	if (mat1.column != mat2.column || mat1.row != mat2.row)throw MATH_ERROR;
	
	Matrix ans(mat1.row, mat1.column);

	for (int i = 0;i < ans.row;i++)
	{
		for (int j = 0;j < ans.column;j++)
		{
			ans.ptr[i][j] = mat1.ptr[i][j] + mat2.ptr[i][j];
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
			ans.ptr[i][j] = -mat.ptr[i][j];
		}
	}
	return ans;
}

Matrix operator-(const Matrix& mat1, const Matrix& mat2)
{
	return mat1 + (-mat2);
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
			if (mat1.ptr[i][j] != mat2.ptr[i][j])return false;
		}
	}
	return true;

}

Matrix reduce(const Matrix& mat, int DeleteCol, int DeleteRow)
{
	mat.ValidityCheck();
	if(!(DeleteCol >= 0 && DeleteCol < mat.column)||
		!(DeleteRow >= 0 && DeleteRow < mat.row))throw MAT_ERROR;

	Matrix ans(mat.row - 1, mat.column - 1);
	int i = 0;int j = 0;
	int a = 0;int b = 0;

	for (i = 0;i < mat.row;i++)
	{
		if (i  == DeleteRow) continue;
		for (j = 0;j < mat.column;j++)
		{
			if (j  == DeleteCol) continue;
			ans.ptr[a][b] = mat.ptr[i][j];
			b++;
		}
		b = 0;
		a++;
	}

	return ans;
}

fraction det(const Matrix& mat)
{
	if (mat.column != mat.row)throw MATH_ERROR;
	mat.ValidityCheck();

	switch (mat.row)
	{
	case 1:
		return mat.ptr[0][0];
		break;
	case 2:
		return mat.ptr[0][0] * mat.ptr[1][1] - mat.ptr[0][1] * mat.ptr[1][0];
		break;

	default:
		fraction sum(0);
		for (int i = 0;i < mat.row;i++)
		{
			if (i % 2 == 0)
			{
				sum = mat.ptr[0][i] * det(reduce(mat, i)) + sum;
			}
			else
			{
				sum = sum - mat.ptr[0][i] * det(reduce(mat, i));
			}
		}
		return sum;
		break;

	}
}

Matrix inverse(const Matrix& mat)
{
	if (mat.column != mat.row)throw MATH_ERROR;
	mat.ValidityCheck();
	fraction A = det(mat);	int n = mat.row;
	Matrix InverseMatrix(n, n);

	if (A == zero)
	{
		throw MATH_ERROR;
		return Matrix();
	}

	fraction B = reciprocal(A);

	if (mat.row == 1)
	{
		InverseMatrix.ptr[0][0] = B;
		return InverseMatrix;
	}

	for (int i = 0;i < n;i++)
	{
		for (int j = 0;j < n;j++)
		{
			if ((i + j) % 2 == 0)
			{
				InverseMatrix.ptr[j][i] = B*det(reduce(mat, j, i));
			}
			else
			{
				InverseMatrix.ptr[j][i] = -B*det(reduce(mat, j, i));
			}
		}
	}

	return InverseMatrix;
}

Matrix adj(const Matrix& mat)
{
	if (mat.column != mat.row)throw MATH_ERROR;
	mat.ValidityCheck();
	int n = mat.row;
	Matrix AdjugateMatrix(n, n);
	if (mat.row == 1)
	{
		AdjugateMatrix.ptr[0][0] = 1;
		return AdjugateMatrix;
	}

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if ((i + j) % 2 == 0)
			{
				AdjugateMatrix.ptr[j][i] = det(reduce(mat, j, i));
			}
			else
			{
				AdjugateMatrix.ptr[j][i] = -det(reduce(mat, j, i));
			}
		}
	}

	return AdjugateMatrix;
}

Matrix operator*(const Matrix& mat1, const Matrix& mat2)
{
	mat1.ValidityCheck();
	mat2.ValidityCheck();
	if (mat1.column != mat2.row)throw MATH_ERROR;

	Matrix ans(mat1.row, mat2.column);
	fraction temp = zero;
	for (int i = 0;i < mat1.row;i++)
	{
		for (int j = 0;j < mat2.column;j++)
		{
			for (int k = 0;k < mat1.column;k++)
			{
				temp = temp + mat1.ptr[i][k] * mat2.ptr[k][j];
			}
			ans.ptr[i][j] = temp;
			temp = zero;
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
			ans.ptr[i][j] = frc*mat.ptr[i][j];
		}
	}
	return ans;
}

Matrix Transpose(const Matrix& mat)
{
	Matrix ans(mat.column, mat.row);
	mat.ValidityCheck();
	for (int i = 0;i < mat.row;i++)
	{
		for (int j = 0;j < mat.column;j++)
		{
			ans.ptr[j][i] = mat.ptr[i][j];
		}
	}
	return ans;
}

void Matrix::swap(int lineA, int lineB)
{
	LineValidity(this, true, lineA, lineB);
	fraction* temp = ptr[lineB];
	ptr[lineB] = ptr[lineA];
	ptr[lineA] = temp;
	return;
}

void Matrix::add(int lineA, int lineB, fraction rate)
{
	LineValidity(this, true, lineA, lineB);
	for (int j= 0;j < column;j++)
	{
		ptr[lineB][j] = rate*ptr[lineA][j] + ptr[lineB][j];
	}
	return;
}

void Matrix::mult(int line, fraction rate)
{
	LineValidity(this, true, line);
	for (int j = 0;j < column;j++)
	{
		ptr[line][j] = rate*ptr[line][j];
	}
	return;
}

void Matrix::col_swap(int lineA, int lineB)
{
	LineValidity(this, false, lineA, lineB);
	fraction temp;
	for (int i = 0;i < row; i++)
	{
		temp = ptr[i][lineA];
		ptr[i][lineA] = ptr[i][lineB];
		ptr[i][lineB] = temp;
	}
	return;
}

void Matrix::col_add(int lineA, int lineB, fraction rate)
{
	LineValidity(this, false, lineA, lineB);
	for (int i = 0;i < row;i++)
	{
		ptr[i][lineB] = ptr[i][lineB] + ptr[i][lineA] * rate;
	}
	return;
}

void Matrix::col_mult(int line, fraction rate)
{
	LineValidity(this, false, line);
	for (int i = 0;i < row;i++)
	{
		ptr[i][line] = rate*ptr[i][line];
	}
	return;
}

//Matrix GetCol/GetRow

Matrix Matrix::GetRow(int Row)
{
	LineValidity(this, true, Row);//Check if the variable Row is legal.

	Matrix ans(1, column);
	for (int j = 0;j < column;j++)
	{
		ans.ptr[0][j] = ptr[Row][j];	//Copy the data.
	}

	return ans;
}

Matrix Matrix::GetColumn(int Column)
{
	LineValidity(this,false,Column);//Check if the variable Row is legal.

	Matrix ans(row, 1);
	for (int i = 0;i < row;i++)
	{
		ans.ptr[i][0] = ptr[i][Column];
	}

	return ans;
}

Matrix GaussEliminate(const Matrix& mat,int* rankptr,SelectArray** sarray)
{
	mat.ValidityCheck();
	Matrix ans(mat);
	int i = 0;

	bool NeedToSelect = false;//Select what?
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
		for (int i_rec = i;i_rec< mat.row;i_rec++)//Find the main number.From line i to the end.
		{
			if (abs(ans.ptr[i_rec][j].GetValue()) > temp_maxNumAbs)
			{
				temp_maxNumAbs = abs(ans.ptr[i_rec][j].GetValue());
				temp_maxNumPos = i_rec;
			}
		}
		if (temp_maxNumAbs < precision || temp_maxNumPos == -1)//Zero.
		{
			if(NeedToSelect)(*sarray)->SArrayPtr[j] = true;//It is not a pivot column. Label it as true.
			continue;
		}
		
		//Swap the main number to the nth row.
		ans.swap(i, temp_maxNumPos);
		//Reduce it to 1 by division.
		ans.mult(i, reciprocal(ans.ptr[i][j]));
		//Eliminate.
		for (int i_rec = 0;i_rec < mat.row;i_rec++)
		{
			if (ans.ptr[i_rec][j]!=zero&&  i_rec!=i)
			{
				ans.add(i, i_rec, -ans.ptr[i_rec][j]);
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
				ans.ptr[i][j] = mat1.ptr[i][j];
			}
			else
			{
				ans.ptr[i][j] = mat2.ptr[i][j-mat1.column];
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
	Matrix U= GaussEliminate(merged_row);
	Matrix E(n,n);//EA=U
	

	for (int i = 0;i < n;i++)
	{
		for (int j = 0;j < n;j++)
		{
			E.ptr[i][j] = U.ptr[i][j + mat.column];
		}
	}

	//Extract the left nullspace matrix.
	Matrix LeftNullSpace(mat.row - rank, mat.row);
	for (int i = rank;i < mat.row;i++)
	{
		memcpy(LeftNullSpace.ptr[i - rank], E.ptr[i], sizeof(fraction)*n);
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
	if (mat.column != mat.row)throw MATH_ERROR;
	mat.ValidityCheck();

	Matrix ans(mat);
	int i = 0;
	int swapcnt = 0;
	for (int j = 0;j < mat.column;j++)
	{
		int temp_maxNumPos = -1;
		double temp_maxNumAbs = 0.0;
		for (int i_rec = i;i_rec< mat.row;i_rec++)//Find the main number.From line i to the end.
		{
			if (abs(ans.ptr[i_rec][j].GetValue()) > temp_maxNumAbs)
			{
				temp_maxNumAbs = abs(ans.ptr[i_rec][j].GetValue());
				temp_maxNumPos = i_rec;
			}
		}
		if (temp_maxNumAbs < precision || temp_maxNumPos == -1)//Zero.
		{
			return zero;
			continue;
		}

		//Swap the main number to the nth row.
		if (i != temp_maxNumPos)
		{
			ans.swap(i, temp_maxNumPos);
			swapcnt++;
		}
		
		fraction tmp_reci=reciprocal(ans.ptr[i][j]);
		//Eliminate.
		for (int i_rec = i+1;i_rec < mat.row;i_rec++)//Triangularize.
		{
			if ( ans.ptr[i_rec][j]!=zero )
			{
				ans.add(i, i_rec, -ans.ptr[i_rec][j]*tmp_reci);
			}
		}
		i++;
	}

	fraction detValue = one;

	for (int i = 0;i < mat.row;i++)
	{
		detValue = detValue*ans.ptr[i][i];
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
	if (mat.column != mat.row)throw MATH_ERROR;
	mat.ValidityCheck();

	//Matrix Definition.
	Matrix ans(mat);
	Matrix Identity(mat.row, mat.column);

	//Initialize identity matrix.
	for (int k = 0;k < mat.row;k++)
	{
		Identity.ptr[k][k] = one;
	}

	int i = 0;
	for (int j = 0;j < mat.column;j++)
	{
		int temp_maxNumPos = -1;
		double temp_maxNumAbs = 0.0;
		for (int i_rec = i;i_rec< mat.row;i_rec++)//Find the main number.From line i to the end.
		{
			if (abs(ans.ptr[i_rec][j].GetValue()) > temp_maxNumAbs)
			{
				temp_maxNumAbs = abs(ans.ptr[i_rec][j].GetValue());
				temp_maxNumPos = i_rec;
			}
		}
		if (temp_maxNumAbs < precision || temp_maxNumPos == -1)//Zero detValue.
		{
			throw MATH_ERROR;
		}

		//Swap the main number to the i-th row.
		if (temp_maxNumPos != i)
		{
			ans.swap(i, temp_maxNumPos);
			Identity.swap(i, temp_maxNumPos);
		}

		//Reduce it to 1 by division.
		fraction tmp_reci = reciprocal(ans.ptr[i][j]);
		ans.mult(i, tmp_reci);
		Identity.mult(i, tmp_reci);
		
		//Eliminate.
		for (int i_rec = 0;i_rec < mat.row;i_rec++)
		{
			if (!( ans.ptr[i_rec][j]==zero ) && i_rec != i)
			{
				fraction negative_ans = -ans.ptr[i_rec][j];
				ans.add(i, i_rec, negative_ans);
				Identity.add(i, i_rec, negative_ans);//The same operation.
			}
		}

		i++;
	}

	return Identity;
}

int Matrix::rank()
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
	if (row != B.row)throw MATH_ERROR;
	if (B.column != 1)throw MATH_ERROR;

	for (int i = 0;i < row;i++)
	{
		ptr[i][pos_col] = B.ptr[i][0];
	}
	return;
}

Matrix Matrix::EigenEqu()
{
	//Check.
	ValidityCheck();
	if (row != column)throw MATH_ERROR;

	//int n=column;
	Matrix eigen_ans(1,column+1);
	SelectArray s_array(column);
	s_array.initialize();
	eigen_ans.ptr[0][0] = one;

	Matrix temp;
	while (!s_array.OverFlowFlag)
	{
		temp = *this;
		for (int i = row-1;i >=0;i--)
		{
			if (!(s_array.SArrayPtr[i] ))
			{
				temp = reduce(temp, i, i);
			}
		}
		eigen_ans.ptr[0][temp.row] = eigen_ans.ptr[0][temp.row] + Gdet(temp);
		s_array++;
	}
	
	for (int i = 0;i < column+1;i++)
	{
		if (i % 2 != 0)eigen_ans.ptr[0][i] = -eigen_ans.ptr[0][i];
	}

	return eigen_ans;
}

Matrix RowSum(const Matrix& mat)
{
	mat.ValidityCheck();
	Matrix ans( mat.row,1);

	for (int i = 0;i < mat.row;i++)
	{
		for (int j = 0;j < mat.column;j++)
		{
			ans.ptr[i][0] = ans.ptr[i][0] + mat.ptr[i][j];
		}
	}

	return ans;

}

Matrix ColSum(const Matrix& mat)
{
	mat.ValidityCheck();
	Matrix ans(1, mat.column);
	for (int j = 0;j < mat.column;j++)
	{
		for (int i = 0;i < mat.row;i++)
		{
			ans.ptr[0][j] = ans.ptr[0][j] + mat.ptr[i][j];
		}
	}
	return ans;
}

void ShowMatrix(fraction*& ptr_data)
{
	if (ptr_data->GetType() == 0)// a fraction
	{
		displayFrac(*ptr_data, true);
	}
	else//a matrix.
	{
		cout << *(ptr_data->GetThis());
	}
	return;
}

Matrix Identity(int n)
{
	if (n <= 0)throw MATH_ERROR;
	Matrix ans(n,n);
	fraction** mat_ptr = ans.GetPtr();
	for (int i = 0;i < n;i++)
	{
		mat_ptr[i][i] = one;
	}
	return ans;
}

//DIY functions/Special operations.

