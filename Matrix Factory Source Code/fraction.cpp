#include "stdafx.h"
#include "fraction.h"
using namespace std;


fraction zero(0);
fraction one(1);
//Constructor
fraction::fraction(double value) :value(value)
{
	isApprox = true;
	isSimplified = true;
}

fraction::fraction(int a, int b)//Constructor of a/b  (unsimplified)
{
	if (b == 0)throw MATH_ERROR;
	isSimplified = false;
	isApprox = false;
	numerator = a;
	denominator = b;
	value = static_cast<double>(a) / b;
}

fraction::fraction(int a)
{
	denominator = 1;
	numerator = a;
	isSimplified = true;
	isApprox = false;
	value = static_cast<double>(a);
}
//Return the greatest common factor.
inline int gcd(const int& a, const int& b)//It requires a>0 && b>0
{
	if (a <= 0 || b <= 0)throw MATH_ERROR;

	int i, j;
	if (a < b)
	{
		i = b;j = a;
	}
	else
	{
		i = a;j = b;
	}//Assure that i >= j

	int temp;

	while (i%j != 0)
	{
		temp = i%j;
		i = j;
		j = temp;
	}
	return j;

}
//Simplify a fraction
fraction simplify(const fraction& a)
{
	if (a.isSimplified == true || a.isApprox == true)
	{
		return a;
	}
	if (a.denominator == 0)throw MATH_ERROR;

	fraction ans;
	int i = abs(a.numerator);
	int j = abs(a.denominator);

	if (i == 0) return fraction::fraction(0);//Call the constructor to generate a zero.

											 //Assure that i,j !=0
	int d = gcd(i, j);

	if ((a.numerator > 0 && a.denominator < 0) || (a.numerator < 0 && a.denominator>0))//They are of different symbol.
	{
		ans.numerator = -i / d;
	}
	else
	{
		ans.numerator = i / d;
	}

	ans.denominator = j / d;
	ans.isSimplified = true;
	ans.isApprox = false;
	ans.value = static_cast<double>(ans.numerator) / ans.denominator;//If you set the num and denom directly, don`t forget to initialize the approximate
																	 //value.
	return ans;
}

double fraction::GetValue(void)const
{
	if (isApprox)
	{
		return value;
	}
	else
	{
		return (double)numerator / denominator;
	}
}

void fraction::SetValue(double val)
{
	numerator = 0;
	denominator = 1;
	value = val;
	isApprox = true;
	isSimplified = true;
}

fraction::operator int()const
{
	if (!isInt(*this))
	{
		return int(value);
	}
	else
	{
		return numerator;
	}
}


bool operator==(const fraction& a,const fraction & b)
{
	fraction c = simplify(a);
	fraction d = simplify(b);
	if (c.isApprox == false && d.isApprox == false) return c.denominator == d.denominator && c.numerator == d.numerator;
	else
	{
		return abs(c.value - d.value) < precision;
	}
}

bool isLessThanZero(const fraction& a)
{
	if (a.isApprox)
	{
		return (a.value < 0.0);
	}
	else//isApprox=false
	{
		fraction temp = simplify(a);
		return (temp.numerator < 0);
	}
}

bool operator<(const fraction& a, const fraction& b)
{
	return isLessThanZero(a - b);
}


fraction operator+(const fraction& a, const fraction& b)
{

	if (a.isApprox == false && b.isApprox == false)
	{	
		if (a.denominator == 0 || b.denominator == 0)throw MATH_ERROR;
		fraction ans(a.numerator*b.denominator + a.denominator*b.numerator, a.denominator*b.denominator);
		return simplify(ans);
	}
	else
	{
		fraction ans;
		ans.isApprox = true;
		ans.isSimplified = true;
		ans.value = a.value + b.value;
		return ans;
	}
}

fraction operator-(const fraction& a)
{
	if (a.isApprox == false)
	{
		fraction ans(-a.numerator, a.denominator);
		return simplify(ans);
	}
	else
	{
		fraction ans;
		ans.isApprox = true;
		ans.isSimplified = true;
		ans.value = -a.value;
		return ans;
	}
}

fraction operator-(const fraction& a, const fraction& b)
{
	return a + (-b);
}

fraction operator*(const fraction& a, const fraction& b)
{
	if (a.isApprox == false && b.isApprox == false)
	{
		fraction ans(a.numerator*b.numerator, a.denominator*b.denominator);
		return simplify(ans);
	}
	else
	{
		fraction ans;
		ans.isSimplified = true;
		ans.isApprox = true;
		ans.value = a.value * b.value;
		return ans;
	}
}

fraction reciprocal(const fraction& a)
{
	if (a.isApprox == false)
	{
		if (a.denominator == 0 || a.numerator == 0)throw MATH_ERROR;

		fraction ans(a.denominator, a.numerator);//The reciprocal.
		return simplify(ans);
	}
	else
	{
		fraction ans;
		ans.isApprox = true;
		ans.isSimplified = true;
		if (a.value == 0)throw MATH_ERROR;
		ans.value = 1 / a.value;
		return ans;
	}
}

fraction operator/(const fraction& a, const fraction& b)
{
	return a*reciprocal(b);
}

void displayFrac(const fraction& a, bool newline)
{
	if (a.isApprox == false)
	{
		if (a.denominator == 0)throw MATH_ERROR;
		simplify(a);

		if (a.numerator == 0)
		{
			cout << 0;
		}
		else
		{
			if (a.denominator == 1)
			{
				cout << a.numerator;
			}
			else
			{
				cout << a.numerator << "/" << a.denominator;
			}
		}
	}
	else
	{
		if (abs(a.value) < precision)cout << 0;
		else
		{
			cout << a.value;
		}
	}

	if (newline)
	{
		cout << endl;
	}

}

fraction pow(const fraction& frc, const int& n)
{
	if (n == 0)
	{
		if (frc.GetValue() <= 0)throw MATH_ERROR;
		return one;
	}
	else if (n > 0)
	{
		fraction temp = frc;
		for (int i = 1;i < n;i++)
		{
			temp = temp*frc;
		}
		return temp;
	}
	else
	{
		if (frc == zero)throw MATH_ERROR;
		return pow(reciprocal(frc), -n);
	}

}

bool isInt(const fraction& frc)
{
	if (frc.isApprox == true)return false;
	fraction temp = simplify(frc);
	if (temp.denominator == 1)return true;
	else return false;

}

ostream& operator<<(ostream& ost, const fraction& frac)
{
	displayFrac(frac, false);
	return ost;
}
