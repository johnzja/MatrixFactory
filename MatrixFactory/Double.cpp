#include "stdafx.h"
#include "Math.h"

extern const Int Int_one = Int(1);

extern const char*   _Divide_By_Zero;
extern const char* _Invalid_Input;
extern const char* _Not_In_Conv_Radius;
extern const char* _Math_Error;
extern int str_cmp(char* a_start, char* b_start, int a_length, int b_length);
extern const Double Double_zero = Double((Int)0, Int_one);
extern const Double Double_one = Double(Int_one, Int_one);

int str_divide_givenlength(char* str1, char* str2, int length1, int length2, char*& quotient_ptr,int quotient_length_required)
//str1.length should be quotient_length_required + divisor_length+1. Ensure that the dividient is larger than the divisor!!!
{//example: 012441201 divided by 0037

	//max.quotient length=length(dividient)-length(divisor)+1.

	char* str1_tail = str1 + length1 - 1;
	char* str2_tail = str2 + length2 - 1;//tail pointers.
	char* subtStartingPoint = str1_tail - length2 + 1;

	int diff = length1 - length2;
	char i = 0;//i is the partial quotient.

	int quotient_exp = 0;
	int first_non_zero = true;

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

		quotient_ptr[quotient_length_required - 1] = i;

		if (first_non_zero&&i != 0)//Record the exp of the quotient.
		{
			quotient_exp = subtStartingPoint - str1;
			first_non_zero = false;
		}
		subtStartingPoint--;
		i = 0;
	} while (first_non_zero || --quotient_length_required);//loop quotient_length_required times.

	return quotient_exp;
}

inline int max(int a, int b)
{
	return (a > b) ? a : b;
}
inline int min(int a, int b)
{
	return (a < b) ? a : b;
}

//Constructor: assure that precision==val.rLength;
Double::Double(double x,int prec):precision(prec),val(0,prec)
{
	if (abs(x) < 1e-15)
	{
		exp = 0;
		return;
	}
	if (!(val.sign = (x > 0)))x = -x;

	exp = floor(log10(x));
	double base = pow(10, exp);

	for (int i = prec - 1;i >= 0;i--)//Extract each digit.
	{
		*(val.data_ptr + i) = (char(x / base));
		x -= base * val.data_ptr[i];
		base /= 10;
	}
	val.real_length();
}

Double::Double(const Int& a, int _exp,int prec):val(a),precision(prec),exp(_exp)//Never use this constructor!
{

}

Double::Double(const Int& a, const Int& b,int prec):precision(prec)//prec is the precision.
{
	//The number is stored in val.  scientific .
	if (b.isZero())throw Exceptions(_Divide_By_Zero);
	if (a.isZero())
	{
		val = Int(0, 1);
		exp = 0;
		return;
	}

	int dividend_length = prec + (b.data_tail_ptr - b.data_ptr) + 1;
	int a_rLength = a.data_tail_ptr - a.data_ptr + 1;
	char* dividend = (char*)calloc(dividend_length, 1);//copy the dividend.Zeros added.
	char* quot = (char*)calloc(prec, 1);

	//copy the data. Length calculation required.
	for (int j = dividend_length - 1,i=a_rLength-1;j >= 0;j--,i--)
	{
		*(dividend + j) = a.GetDigit(i);
	}

	//ok.
	exp = str_divide_givenlength(dividend, b.data_ptr, dividend_length, b.data_tail_ptr - b.data_ptr + 1, quot, prec) - (dividend_length - a_rLength);
	free(dividend);

	val = Int(quot, precision);//ensure that val.realLength=precision.
	val.sign = !(a.sign ^ b.sign);
}

ostream& operator<<(ostream& ost, const Double& db)
{
	if (db.val.sign == false)cout << '-';
	if (db.exp >= 0)//1.234,230000or so.
	{
		if (db.exp < db.precision-1)//decimal displayed.
		{
			int decimal_pos = 1 + db.exp;
			for (int i = 0;i < db.precision;i++)
			{
				if (i == decimal_pos)cout << '.';
				cout << char('0' + db.val.GetDigit(db.precision - 1 - i));
			}
		}
		else
		{
			// no decimal point. Zeros appended.
			int digits_cnt = db.exp + 1;
			for (int i = digits_cnt - 1;i >= 0;i--)
			{
				cout << char('0' + db.val.GetDigit(i - digits_cnt + db.precision));
			}
		}
	}
	else
	{
		cout << "0.";
		int num_of_zeors = -db.exp - 1;
		while (num_of_zeors--)cout << '0';
		for (char* ptr = db.val.data_tail_ptr;ptr >= db.val.data_ptr;ptr--)
		{
			cout << (char(*ptr + '0'));
		}
	}

	return ost;
}

istream& operator>>(istream& ist, Double& db)
{
	string num;
	ist >> num;
	//Check validity.

	//Parse the string.
	string ans_abs_value;
	size_t L = num.length();
	int Int_exp = 0;

	//Check validity.

	for (size_t i = 0;i < L;i++)
	{
		char c = num[i];
		if (!(c == '.' || c == '-' || ((c <= '9') && (c >= '0'))))//Error.
		{
			throw Exceptions(_Invalid_Input);
		}
	}

	bool negativeFlag = false;
	bool decimalFlag = false;

	//look at the negative.
	size_t i, j;
	if (num[0] == '-')
	{
		negativeFlag = true;
		i = 1;
	}
	else
	{
		i = 0;
	}

	//Extract the number part.
	bool startExtFlag = false;
	for (j = i;j < L;j++)
	{
		if (startExtFlag || ((num[j] <= '9') && (num[j] >= '1')))
		{
			ans_abs_value += num[j];
			startExtFlag = true;
		}
		if (num[j] == '.' && !decimalFlag)
		{
			decimalFlag = true;
			if (!ans_abs_value.empty())ans_abs_value.pop_back();
			Int_exp = -(int)((L - 1) - j);
		}
		else if (num[j] == '.' && decimalFlag) throw Exceptions(_Invalid_Input);

	}

	//Check the string obtained.
	L = ans_abs_value.length();

	for (j = 0;j < L;j++)
		if (!(ans_abs_value[j] <= '9' && ans_abs_value[j] >= '0')) throw Exceptions(_Invalid_Input);
	//Convert the string into an Int, then a double.
	int prec = max(PREC, L);
	Int val = Int(0, prec);//Do not forget to realLength().
	val.data_tail_ptr = val.data_ptr + prec - 1;
	for (j = 0;j < L;j++)
	{
		*(val.data_ptr + prec - 1 - j) = ans_abs_value[j] - '0';//copy the data.
	}

	//calculate the exponent.
	int exp;
	if (L != 0)exp = Int_exp + L - 1;else exp = 0;
	if (negativeFlag&& L != 0)val.sign = false;
	db = Double(val, exp, prec);

	return ist;
}

Double::operator double() const
{
	if (!val.isZero())
	{
		int exp_s = exp;
		double ans = 0.0;//initial value=0.0
		for (int i = precision - 1;i >= 0;i--)
		{
			ans += val.data_ptr[i] * pow(10.0, exp_s);
			exp_s--;
		}
		if (val.sign)
		{
			return ans;
		}
		else
		{
			return -ans;
		}
	}
	else return 0;
}

bool Double::isZero()const
{
	return val.isZero();
}




int Double::GetExp(void) 
{
	return exp;
}

int Double::GetPrecision(void) 
{
	return precision;
}

int Double::GetLength()const
{
	if (val.isZero())return 1;
	else
	{
		if (exp >= 0)
			return exp + 1;
		else return 1;
	}
}

int abs_compare(const Double& a,const Double& b)
{
	bool isZa = a.isZero();
	bool isZb = b.isZero();
	if (!isZa && !isZb)
	{
		if (a.exp > b.exp)
		{
			return +1;
		}
		else if (a.exp < b.exp)
		{
			return -1;
		}
		else//exp is equal.
		{
			int L = min(a.precision, b.precision) - 1;
			char* ptr_a = a.val.data_tail_ptr;
			char* ptr_b = b.val.data_tail_ptr;
			for (;L >= 0;(L--, ptr_a--, ptr_b--))
			{
				if (*ptr_a > *ptr_b)
				{
					return +1;
				}
				else if (*(ptr_a) < *ptr_b)
				{
					return -1;
				}
			}
			return 0;
		}
	}
	else if (isZa && !isZb)
	{
		return -1;
	}
	else if (!isZa && isZb)
	{
		return +1;
	}
	else
	{
		return 0;
	}
}

Double abs_add(const Double& a, const Double& b)
{
	int max_prec = (a.precision > b.precision) ? a.precision : b.precision;
	int a_val_L = a.val.length;
	int b_val_L = b.val.length;
	//To analyze the range.
	int max_pow_10 = max(a.exp, b.exp);
	int min_pow_10 = min(a.exp - (a_val_L - 1), b.exp - (b_val_L - 1));

	int sum_length = max_pow_10 - min_pow_10 + 2;
	char* sum_ptr = (char*)calloc(sum_length, 1);


	for (int i = 0;i < sum_length;i++)//i==0: the min pow of 10 to the highest pow+1.
	{
		sum_ptr[i] = a.val.GetDigit(min_pow_10 + i - a.exp + a_val_L - 1) + b.val.GetDigit(min_pow_10 + i - b.exp + b_val_L - 1);

	}

	for (int i = 0;i < sum_length;i++)//Strange problems here!
	if (sum_ptr[i] > 9)
	{
		sum_ptr[i + 1] ++;
		sum_ptr[i] -= 10;
	}
	//Find the highest non-zero element and determine the exp of the new number.
	char* num_tail_ptr = sum_ptr + sum_length - 1;
	while ((*num_tail_ptr == 0) && (num_tail_ptr >= sum_ptr))//If not find, then return zero.
	{
		num_tail_ptr--;
	}

	int ans_exp = 0;
	if (num_tail_ptr != (sum_ptr - 1))//Normal condition.
	{
		ans_exp = min_pow_10 + (num_tail_ptr - sum_ptr);

		//Read ans.
		Int ans_val = Int(0, max_prec);//realLength=zero
		for (int i = max_prec - 1;i >= 0;i--)
		{
			*(ans_val.data_ptr + i) = *(num_tail_ptr--);
		}
		ans_val.real_length();
		free(sum_ptr);
		return Double(ans_val, ans_exp, max_prec);
	}
	else
	{
		free(sum_ptr);
		return Double_zero;
	}
}

Double abs_subtract(const Double& a, const Double& b)
{
	int max_prec = (a.precision > b.precision) ? a.precision : b.precision;
	int a_val_L = a.val.length;
	int b_val_L = b.val.length;
	//To analyze the range.
	int max_pow_10 = max(a.exp, b.exp);
	int min_pow_10 = min(a.exp - (a_val_L - 1), b.exp - (b_val_L - 1));

	int diff_length = max_pow_10 - min_pow_10 + 2;
	char* diff_ptr = (char*)calloc(diff_length, 1);

	for (int i = 0;i < diff_length;i++)//i==0: the min pow of 10 to the highest pow+1.
	{
		diff_ptr[i] = a.val.GetDigit(min_pow_10 + i - a.exp + a_val_L - 1) - b.val.GetDigit(min_pow_10 + i - b.exp + b_val_L - 1);
	}

	for (int i = 0;i < diff_length;i++)
	{
		if (diff_ptr[i] < 0)
		{
			diff_ptr[i + 1] -= 1;
			diff_ptr[i] += 10;
		}
	}

	//Find the highest non-zero element and determine the exp of the new number.
	char* num_tail_ptr = diff_ptr + diff_length - 1;
	while (*num_tail_ptr == 0)
	{
		num_tail_ptr--;
	}
	int ans_exp = min_pow_10 + (num_tail_ptr - diff_ptr);
	//Read ans.

	Int ans_val = Int(0, max_prec);//realLength=zero
	for (int i = max_prec - 1;(i >= 0 && num_tail_ptr >= diff_ptr);i--)
	{
		*(ans_val.data_ptr + i) = *(num_tail_ptr--);
	}

	free(diff_ptr);
	ans_val.real_length();
	return Double(ans_val, ans_exp, max_prec);
}

Double operator+(const Double& a, const Double& b)
{
	int cmp = abs_compare(a, b);
	int max_prec = max(a.precision, b.precision);

	if (a.val.sign)
	{
		if (b.val.sign)//two positive.
		{
			return abs_add(a, b);
		}
		else//a>0,b<0
		{
			if (cmp == 0)
			{
				return Double((Int)0, 0, max_prec);
			}
			else if (cmp > 0)
			{
				return abs_subtract(a, b);
			}
			else
			{
				return -(abs_subtract(b, a));
			}
		}
	}
	else//a<0
	{
		if (b.val.sign)//b>0
		{
			if (cmp == 0)
			{
				return Double((Int)0, 0, max_prec);
			}
			else if (cmp > 0)//|a|>|b|
			{
				return -(abs_subtract(a,b));
			}
			else
			{
				return abs_subtract(b, a);
			}
		}
		else
		{
			return -(abs_add(a, b));
		}
	}
}

Double operator-(const Double& a, const Double& b)
{
	int cmp = abs_compare(a, b);
	int max_prec = max(a.precision, b.precision);

	if (a.val.sign)
	{
		if (b.val.sign)//two positive.
		{
			if (cmp == 0)
			{
				return Double((Int)0, 0, max_prec);//Zeros can occur. But very rarely.
			}
			else if (cmp > 0)
			{
				return abs_subtract(a, b);
			}
			else
			{
				return -abs_subtract(b, a);
			}
		}
		else//a>0,b<0
		{
			return abs_add(a, b);
		}
	}
	else//a<0
	{
		if (b.val.sign)//b>0
		{
			return -abs_add(a, b);
		}
		else
		{
			if (cmp == 0)
			{
				return Double((Int)0, 0, max_prec);
			}
			else if (cmp > 0)//a.abs is bigger.
			{
				return -abs_subtract(a, b);
			}
			else
			{
				return abs_subtract(b, a);
			}
		}
	}

}

Double operator*(const Double& a, const Double& b)
{
	int max_prec = max(a.precision, b.precision);
	Int product = a.val.abs_mult(b.val);
	//Extract numbers.
	int product_rLength = product.data_tail_ptr - product.data_ptr + 1;

	Int extracted_product(0, max_prec);

	for (int i = 0;i<max_prec;i++)
	{
		extracted_product.data_ptr[max_prec - 1 - i] = product.GetDigit(product_rLength - i - 1);
	}
	extracted_product.real_length();
	if (!extracted_product.isZero())
	{
		extracted_product.sign = !(a.val.sign ^ b.val.sign);
	}
	else
	{
		extracted_product.sign = true;
	}

	return Double(extracted_product, product_rLength + 1 - a.precision - b.precision + a.exp + b.exp, max_prec);
}

Double operator-(const Double& a)
{
	Double ans = a;//prec copied.
	if (!ans.val.isZero())ans.val.sign = !ans.val.sign;
	return ans;
}

Double operator/(const Double& a, const Double& b)
{
	int max_prec = (a.precision > b.precision) ? a.precision : b.precision;
	Double ans(a.val, b.val, max_prec);//Int part. Division carried out automatically.
	ans.exp = ans.exp - a.precision + b.precision + a.exp - b.exp;
	return ans;
}

//Comparative operators

bool operator==(const Double& a, const Double& b)
{
	if (a.exp != b.exp || a.precision != b.precision)
	{
		return false;
	}
	else
	{
		if (a.val == b.val)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool operator<(const Double& a, const Double& b)
{
	int abs_cmp_result;
	if (a.val.sign && b.val.sign)
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
	else if (a.val.sign && !b.val.sign)//a>=0,b<0
	{
		return false;
	}
	else if (!a.val.sign && b.val.sign)//a<0, b>=0
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

bool operator>(const Double& a, const Double& b)
{
	{
		int abs_cmp_result;
		if (a.val.sign && b.val.sign)//a>=0,b>=0
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
		else if (a.val.sign && !b.val.sign)//a>=0,b<0
		{
			return true;
		}
		else if (!a.val.sign && b.val.sign)//a<0, b>=0
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
}

//Some Mathmatical Functions.

Double pow(const Double& x, int y)
{
	if (x == Double_zero && y <= 0) throw Exceptions(_Math_Error);
	else if (y == 0) return Double_one;
	else if (y == 1) return x;
	else if (x == Double_zero) return Double_zero; 
	else if (x == Double_one) return Double_one; 
	else if (y > 0) 
	{
		Double result = Double_one; 

		while (y > 0) 
		{
			result = result * x;
			y--;
		}
		return result;
	}
	else 
	{
		Double result = Double_one;
		while (y < 0) 
		{
			result = result/x;
			y++;
		}
		return result;
	}
}

Double abs(const Double& x) 
{
	return (x > Double_zero) ? x : (-x);
}

Double exponent(const Double& x) 
{
	Double result = Double_one; 
	Double term = Double_one; 
	int i = 1;
	Double prec = pow(Double(Int(10)), (int)-30);
	while (abs(term) > prec)
	{
		term = (term * x) / (Double(Int(i)));
		result = result + term;
		i++;
	}
	return result;
}

static Double lnop(const Double& x)
{
	if (x > Double_one || x < -Double_one) throw Exceptions(_Not_In_Conv_Radius);
	Double result = x;
	Double term = x;
	int i = 2;
	Double sz = pow(Double(Int(10)), -30);
	while (abs(term) > sz)
	{
		term = term * (-x)*(Double(Int(i - 1)) / Double(Int(i)));
		result = result + term;
		i++;
	}
	return result;
}

Double ln(const Double& x)
{
	if (x.val.sign == false || x.val.isZero()) throw Exceptions(_Math_Error);
	Double u = (x - Double_one) / (x + Double_one);
	return (lnop(u) - lnop(-u));
}

Double pow(const Double& x, const Double& y) 
{
	return exponent(ln(x)*y);
}

Double ArcTan(const Double& x)
{
	Double ans = x;
	Double mul = x * x;
	Double base = x * mul;

	for (int i = 1;i <= 200;i++)//100 terms of series.
	{
		if (i % 2)
		{
			ans = ans - Double((Int)1, (Int)(2 * i + 1)) * base;
		}
		else
		{
			ans = ans + Double((Int)1, (Int)(2 * i + 1)) * base;
		}
		base = base * mul;
	}

	return ans;
}

Double sqrt(const Double& x,bool HighPrec)
{
	if (HighPrec)
	{
		return pow(x, Double(Int_one, (Int)2));
	}
	else
	{
		return pow(double(x), 0.5);
	}
}
