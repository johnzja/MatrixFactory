#include "stdafx.h"
#include "Math.h"

extern const Int Int_one;

extern const char*   _Divide_By_Zero;
extern const char* _Math_Error_Gcd;
extern const char* _Negative_Base;
extern const char* _Type_Miss;
using namespace std;

Int::Int(int val, int _length):length(_length)
{
	data_ptr = (char*)calloc(length, 1);//Memory automatically initialized with value 0.
	data_tail_ptr = nullptr;

	if (val >= 0)
	{
		sign = true;
	}
	else
	{
		sign = false;
		val = -val;
	}

	char tmp;
	char* ptr_tmp = data_ptr;
	while (tmp = val % 10, val)
	{
		*(ptr_tmp++) = tmp;
		val /= 10;
	}

	real_length();//Initialize the tail.
}

int Int::real_length()//this length is normal.
{

	int index_tail = 0;
	for (int i = length - 1;i >= 0;i--)//Find the tail.
	{
		if (*(data_ptr + i) != 0)
		{
			index_tail = i;
			break;
		}
	}

	data_tail_ptr = data_ptr + index_tail;
	return index_tail + 1;
}

Int::Int(char* src, int length):length(length)
{
	data_ptr = src;
	data_tail_ptr = src + length - 1;
}

Int::Int(const string& str)//convert string into Int
{
	size_t L;
	length = L = str.length();
	data_ptr = (char*)calloc(L, 1);
	char* tmp_ptr = data_ptr;
	int i = L - 1;
	while (i >= 0 && str[i] >= '0' && str[i] <= '9')
	{
		*(tmp_ptr++) = str[i--] - '0';
	}
	real_length();
	sign = true;
}


int Int::GetLength()const
{
	return data_tail_ptr - data_ptr + 1;
}

int abs_compare(const Int& a, const Int& b)
{
	int diff = (a.data_tail_ptr - a.data_ptr) - (b.data_tail_ptr - b.data_ptr);//difference of real length

	if (diff > 0)
	{
		return +1;//a>b
	}
	else if (diff < 0)
	{
		return -1;
	}
	//diff==0.
	
	int n = a.data_tail_ptr - a.data_ptr;
	for (int i = n;i >= 0;i--)
	{
		if (*(a.data_ptr+i) > *(b.data_ptr+i))
		{
			return +1;
		}
		else if (*(a.data_ptr + i) < *(b.data_ptr + i))
		{
			return -1;
		}
	}
	return 0;//equal.
}

bool operator<(const Int& a, const Int& b)// suppose there is no negative zero.
{
	int abs_cmp_result;
	if (a.sign && b.sign)
	{
		switch (abs_cmp_result = abs_compare(a, b))
		{
		case +1:
			return false;
		case -1:
			return true;
		default:
			return false;
		}
	}
	else if (a.sign && !b.sign)//a>=0,b<0
	{
		return false;
	}
	else if (!a.sign && b.sign)//a<0, b>=0
	{
		return true;
	}
	else//a<0,b<0
	{
		switch (abs_cmp_result = abs_compare(a, b))
		{
		case +1:
			return true;
		case -1:
			return false;
		default:
			return false;
		}
	}
}

bool operator>(const Int& a, const Int& b)
{
	int abs_cmp_result;
	if (a.sign && b.sign)//a>=0,b>=0
	{
		switch (abs_cmp_result = abs_compare(a, b))
		{
		case +1:
			return true;
		case -1:
			return false;
		default:
			return false;
		}
	}
	else if (a.sign && !b.sign)//a>=0,b<0
	{
		return true;
	}
	else if (!a.sign && b.sign)//a<0, b>=0
	{
		return false;
	}
	else//a<0,b<0
	{
		switch (abs_cmp_result = abs_compare(a, b))
		{
		case +1:
			return false;
		case -1:
			return true;
		default:
			return false;
		}
	}

}

bool operator==(const Int& a, const Int& b)
{
	return a.sign == b.sign && abs_compare(a, b) == 0;
}

Int::~Int()
{
	free(data_ptr);
}

inline char Int::GetDigit(int n)const
{
	if (n < length&&n>=0) 
	{ 
		return data_ptr[n]; 
	}
	else
	{
		return 0;
	}
}

bool Int::isZero()const
{
	for (int i = 0;i < length;i++)
	{
		if (data_ptr[i] != 0)return false;
	}
	return true;
}

//Basic operators.


Int Int::abs_add(const Int& b)const
{
	//Ignore the sign.
	int rLengthA = data_tail_ptr - data_ptr + 1;//'normal' lengthes.
	int rLengthB = b.data_tail_ptr - b.data_ptr + 1;
	int n = (rLengthA > rLengthB) ? rLengthA : rLengthB;//n is the maximum number of digits to be computed.
	n++;


	Int ans(0, n);//Initialize as zero.
	bool carry = false;
	char digitA, digitB;

	for (int i = 0;i < n;i++)
	{
		digitA = GetDigit(i);
		digitB = b.GetDigit(i);

		if (!carry)
		{
			ans.data_ptr[i] = (digitA + digitB) % 10;
			carry = ((digitA + digitB) > 9);
		}
		else
		{
			ans.data_ptr[i] = (digitA + digitB + 1) % 10;
			carry = ((digitA + digitB + 1) > 9);
		}

	}
	ans.real_length();//Redirect the tail ptr.
	ans.sign = true;
	return ans;
}

Int Int::abs_subtract(const Int& b)const
{

	int cmp = abs_compare(*this, b);
	if (cmp == 0)
	{
		return Int(0);
	}
	else if (cmp == 1)//a>b
	{
		Int ans(*this);//copy a;	
		int n = ans.data_tail_ptr - ans.data_ptr + 1;//get its real length. Then perform +.

		Int temp_b(0, n);
		for (int i = 0;i < n;i++)	temp_b.data_ptr[i] = 9 - b.GetDigit(i);

		//Perform +.
		bool carry = true;//there is an initial 1.
		char digitA, digitB;

		for (int i = 0;i < n;i++)
		{
			digitA = ans.GetDigit(i);
			digitB = temp_b.GetDigit(i);

			if (!carry)
			{
				ans.data_ptr[i] = (digitA + digitB) % 10;
				carry = ((digitA + digitB) > 9);
			}
			else
			{
				ans.data_ptr[i] = (digitA + digitB + 1) % 10;
				carry = ((digitA + digitB + 1) > 9);
			}

		}

		ans.real_length();//restore the tail ptr.
		ans.sign = true;
		return ans;
	}
	else//a<b
	{
		Int ans(b);//copy a;	
		int n = ans.data_tail_ptr - ans.data_ptr + 1;//get its real length. Then perform +.

		Int temp_b(0, n);
		for (int i = 0;i < n;i++)	temp_b.data_ptr[i] = 9 - GetDigit(i);

		//Perform +.
		bool carry = true;//there is an initial 1.
		char digitA, digitB;

		for (int i = 0;i < n;i++)
		{
			digitA = ans.GetDigit(i);
			digitB = temp_b.GetDigit(i);

			if (!carry)
			{
				ans.data_ptr[i] = (digitA + digitB) % 10;
				carry = ((digitA + digitB) > 9);
			}
			else
			{
				ans.data_ptr[i] = (digitA + digitB + 1) % 10;
				carry = ((digitA + digitB + 1) > 9);
			}

		}
		ans.sign = true;
		ans.real_length();//restore the tail ptr.
		return ans;
	}

}

Int Int::abs_mult(const Int& b)const
{
	//Ignore the sign.
	//Suppose that realLength(b) has been calculated.
	int LengthA = data_tail_ptr - data_ptr + 1;
	int LengthB = b.data_tail_ptr - b.data_ptr + 1;
	Int ans(0, LengthA + LengthB);
	for (int i = 0;i < LengthA;i++)
	{
		for (int j = 0;j < LengthB;j++)
		{
			ans.data_ptr[i + j] += data_ptr[i] * b.data_ptr[j];
			if (ans.data_ptr[i + j] > 9)
			{
				ans.data_ptr[i + j + 1] += ans.data_ptr[i + j] / 10;//Carry.
				ans.data_ptr[i + j] %= 10;
			}

		}
	}
	ans.real_length();
	ans.sign = true;
	return ans;
}

//Division preparation.

int str_cmp(char* a_start, char* b_start, int a_length, int b_length)//a and b are both positive. Length!=RealLength.
{
	char* a_end = a_start + a_length - 1;
	while (*(a_end) == 0)a_end--;
	char* b_end = b_start + b_length - 1;
	while (*(b_end) == 0)b_end--;

	//compare. Different size.
	a_length = a_end - a_start + 1;
	b_length = b_end - b_start + 1;
	if (a_length > b_length)return +1;
	if (a_length < b_length)return -1;

	for (int i = 0;i < a_length;i++)
	{
		if (*(a_end - i) < *(b_end - i))return -1;
		else if (*(a_end - i) > *(b_end - i))return +1;
	}
	return 0;
}

int str_divide(char* str1, char* str2,int length1,int length2,char*quotient_ptr)//str1.length>=str2.length, with length1>length2. str1& str2 are head ptrs.
{//example: 012441201 divided by 0037
	
	char* str1_tail = str1 + length1 - 1;
	char* str2_tail = str2 + length2 - 1;//tail pointers.
	char* subtStartingPoint = str1_tail - length2 + 1;

	int diff = length1 - length2;

	char i = 0;

	do
	{
		while (str_cmp(subtStartingPoint, str2, str1_tail - subtStartingPoint + 1, length2) >= 0)//bigger.
		{
			//subtract.
			for (int i = 0;i < length2;i++)
			{
				*(subtStartingPoint + i) -= *(str2 + i);
			}
			//Normalize.
			for (char* ptr = subtStartingPoint;ptr <= str1_tail;ptr++)
			{
				if (*(ptr) < 0)
				{
					*ptr += 10;
					*(ptr + 1) -= 1;
				}
			}
			i++;
		}
		quotient_ptr[subtStartingPoint - str1] = i;
		subtStartingPoint--;
		i = 0;
	} while (subtStartingPoint >= str1);
	return diff + 1;
}

Int Int::abs_divide(const Int& b, Int& res)const
{
	//Abs value!
	if (b.isZero())throw Exceptions(_Divide_By_Zero);//Math error.
	Int a = *this;
	int lengthA = a.data_tail_ptr - a.data_ptr + 1;
	int lengthB = b.data_tail_ptr - b.data_ptr + 1;
	if (lengthA < lengthB)
	{
		res = *this;
		return Int(0, 1);
	}
	//lengthA>=lengthB
	
	Int Quotient(0, lengthA - lengthB + 1);
	
	str_divide(a.data_ptr, b.data_ptr, lengthA, lengthB, Quotient.data_ptr);
	Quotient.real_length();//Relocate the tail.
	a.real_length();
	res = a;
	return Quotient;
}

Int operator+(const Int& a,const Int& b)
{
	int cmp = abs_compare(a, b);

	if (a.sign)
	{
		if (b.sign)//two positive.
		{
			return a.abs_add(b);
		}
		else//a>0,b<0
		{
			if (cmp == 0)
			{
				return Int(0, 1);
			}
			else if (cmp > 0)
			{
				return a.abs_subtract(b);
			}
			else
			{
				return -(b.abs_subtract(a));
			}
		}
	}
	else//a<0
	{
		if (b.sign)//b>0
		{
			if (cmp == 0)
			{
				return Int(0, 1);
			}
			else if (cmp > 0)//|a|>|b|
			{
				return -(a.abs_subtract(b));
			}
			else
			{
				return b.abs_subtract(a);
			}
		}
		else
		{
			return -(a.abs_add(b));
		}
	}
}

Int operator-(const Int& a, const Int& b)
{
	Int negativeB = -b;
	return a + negativeB;
}

Int& operator+=(Int& a,const Int& b)
{
	//Using this operator can speed up the calculation process.
	a = a + b;
	return a;
}

Int operator*(const Int& a, const Int& b)
{
	//Code here.
	Int ans = a.abs_mult(b);
	ans.sign = !(a.sign ^ b.sign);
	//Real Length has been calculated.
	if (ans.isZero())ans.sign = true;
	return ans;
}

Int operator/(const Int& a, const Int& b)//  a/b
{
	//There could be exceptions.
	if (b.isZero())throw Exceptions(_Divide_By_Zero);//Math error.
	Int tmp = a;
	int lengthA = tmp.data_tail_ptr - tmp.data_ptr + 1;
	int lengthB = b.data_tail_ptr - b.data_ptr + 1;
	if (lengthA < lengthB)
	{
		return Int(0, 1);
	}
	//lengthA>=lengthB

	Int Quotient(0, lengthA - lengthB + 1);

	str_divide(tmp.data_ptr, b.data_ptr, lengthA, lengthB, Quotient.data_ptr);
	Quotient.real_length();//Relocate the tail.

	Quotient.sign = !(a.sign ^ b.sign);
	if (Quotient.isZero())Quotient.sign = true;
	return Quotient;
}

Int operator%(const Int& a, const Int& b)
{
	if (b.isZero())throw Exceptions(_Divide_By_Zero);//Math error.
	Int ans = a;
	int lengthA = ans.data_tail_ptr - ans.data_ptr + 1;
	int lengthB = b.data_tail_ptr - b.data_ptr + 1;
	if (lengthA < lengthB)
	{
		return ans;
	}
	//lengthA>=lengthB

	Int Quotient(0, lengthA - lengthB + 1);
	str_divide(ans.data_ptr, b.data_ptr, lengthA, lengthB, Quotient.data_ptr);
	ans.real_length();

	if (ans.isZero())ans.sign = true;
	return ans;//Quotient discarded.
}

Int gcd(const Int& a, const Int& b)//It requires a>0 && b>0
{

	if (a.sign == false || b.sign == false || a.isZero() || b.isZero())throw Exceptions(_Math_Error_Gcd);

	Int i, j;
	if (abs_compare(a,b)==-1)
	{
		i = b;j = a;
	}
	else
	{
		i = a;j = b;
	}//Assure that i >= j

	Int temp;

	while (!(temp = i % j).isZero())
	{
		i = j;
		j = temp;
	}
	return j;
}


ostream& operator<<(ostream& ostr,const Int& a)
{
	if (!a.sign)cout << '-';
	for (char* ptr = a.data_tail_ptr;ptr >= a.data_ptr;ptr--)
	{
		cout << (char(*ptr + '0'));
	}
	return ostr;
}

Int::operator int()const//<=10^9
{
	int ans = 0;
	int l = 1;
	for (int i = 0;i < 9;i++)
	{
		ans += l * GetDigit(i);//Avoid length>=9.
		l *= 10;
	}
	if (sign)
	{
		return ans;
	}
	else
	{
		return -ans;
	}
}

Int::Int(const Int& src)//copy constructor. Needn`t allocate memory every time.
{
	int realL = src.data_tail_ptr - src.data_ptr + 1;
	data_ptr = (char*)malloc(realL);
	memcpy(data_ptr, src.data_ptr, realL);


	length = realL;
	data_tail_ptr = data_ptr + (src.data_tail_ptr - src.data_ptr);
	sign = src.sign;
}

Int& Int::operator=(const Int& src)
{
	if (this != &src)//copy the data.
	{
		int realL = src.data_tail_ptr - src.data_ptr + 1;
		if (length >= realL)
		{
			//Clear.
			memset(data_ptr, 0, length);
			memcpy(data_ptr, src.data_ptr, realL);
		}
		else//space is insufficient.
		{
			//Then copy new data.
			data_ptr = (char*)realloc(data_ptr, realL);
			memcpy(data_ptr, src.data_ptr, realL);
			length = realL;
		}

		data_tail_ptr = data_ptr + (src.data_tail_ptr - src.data_ptr);
		sign = src.sign;
	}
	return *this;
}

Int Int::operator-()const
{
	Int ans(*this);
	ans.sign = !ans.sign;
	return ans;
}

Int abs(const Int& a)
{
	Int ans = a;
	ans.sign = true;
	return ans;
}

Int pow(const Int& a, int b)
{
	if (b == 0)
	{
		return Int_one;
	}
	else if (b > 0)//Fast power Algorithm
	{
		Int base = a;
		Int ans = Int_one;
		int _n = b;

		while (_n)
		{
			if (_n & 1)ans = base * ans;
			base = base * base;
			_n >>= 1;
		}
		return ans;
	}
	else
	{
		//Throw exceptions.
		throw Exceptions(_Type_Miss);
	}

}



//Exceptions.
Exceptions::Exceptions(const char* _error_info)
{
	strcpy_s(errinfo, _error_info);
}

//Function//
Math* Function::apply(Math* x)
{
	Math* result = nullptr;
	switch (x->GetType())
	{
	case DBL:
		result = new Double(((Double(*)(const Double&))funcptr)(*dynamic_cast<Double*>(x))); break;

	case FRC:
		result = new fraction(((fraction(*)(const fraction&))funcptr)(*dynamic_cast<fraction*>(x))); break;

	case MAT:
		result = new Matrix(((Matrix(*)(const Matrix&))funcptr)(*dynamic_cast<Matrix*>(x))); break;



		break;
	}
	return result;

}