#include "stdafx.h"
#include "Math.h"


extern const char*   _Divide_By_Zero;
extern const char* _Negative_Base;


extern const double precision = 1e-10;
extern const fraction frc_zero = fraction();
extern const fraction frc_one = fraction(1,1);
extern const Int Int_one;

//Constructor
fraction::fraction(int a, int b) :value(0.0,PREC)//unsimplified. precise.  "value" not evaluated.
{
	if (b == 0)throw Exceptions(_Divide_By_Zero);
	isSimplified = false;
	isApprox = false;
	numerator = a;
	denominator = b;
}

fraction::fraction(Int a, Int b):value(0.0,PREC)//unsimplified. precise. "value" not evaluated.
{
	if (b.isZero())throw Exceptions(_Divide_By_Zero);
	isSimplified = false;
	isApprox = false;
	numerator = a;
	denominator = b;
	//Value not initialized.
}

fraction::fraction(double value) :value(value,14)//From double: precision<=14
{
	isApprox = true;
	isSimplified = true;
}

fraction::fraction(const Double& value):value(value)
{
	isApprox = true;
	isSimplified = true;
}


fraction::fraction(Int a):value(0.0,PREC),denominator(1),numerator(a)
{
	isSimplified = true;
	isApprox = false;
}

//Simplify a fraction
fraction simplify(const fraction& a)
{
	if (a.isSimplified == true || a.isApprox == true)
	{
		return a;//Do not need to simplify.
	}
	if (a.denominator.isZero())throw Exceptions(_Divide_By_Zero);

	fraction ans;
	Int i = abs(a.numerator);
	Int j = abs(a.denominator);

	if (i.isZero()) return frc_zero;
								
	Int d = gcd(i, j); //Assure that i,j !=0

	if ((a.numerator.sign ^ a.denominator.sign))//They are of different symbol.
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
	
	return ans;
}

double fraction::GetValue(void)const
{

	if (isApprox)
	{
		return double(value);
	}
	else
	{
	//	cout << numerator << "||"<<denominator << endl;
		return double(Double(numerator, denominator, PREC));
	}
}

Double fraction::GetValueD(void)const
{
	if (isApprox)
	{
		return value;
	}
	else
	{
		return Double(numerator, denominator, PREC);
	}
}

void fraction::SetValue(double val)
{
	value = val;
	isApprox = true;
	isSimplified = true;
}

void fraction::AbortPreciseCalculation()
{
	if (!isApprox)//precise
	{
		*this = simplify(*this);//Simplification.
		isApprox = true;
		value = Double(numerator, denominator);
	}
}

bool operator==(const fraction& a, const fraction & b)
{
	fraction c = simplify(a);
	fraction d = simplify(b);
	if (c.isApprox == false && d.isApprox == false) return c.denominator == d.denominator && c.numerator == d.numerator;
	else
	{
		return (a.value == b.value);
	}
}

bool operator!=(const fraction& a, const fraction& b)
{
	return !(a == b);
}

fraction operator+(const fraction& a, const fraction& b)
{

	if (!a.isApprox && !b.isApprox)
	{
		if (a.denominator.isZero() || b.denominator.isZero())throw Exceptions(_Divide_By_Zero);
		fraction ans = fraction(a.numerator*b.denominator + a.denominator*b.numerator, a.denominator*b.denominator);
		return simplify(ans);
	}
	else
	{
		return fraction(a.value + b.value);
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
		return fraction(-a.value);
	}
}
fraction operator-(const fraction& a, const fraction& b)
{
	if (a.isApprox == false && b.isApprox == false)
	{
		if (a.denominator.isZero() || b.denominator.isZero())throw Exceptions(_Divide_By_Zero);
		fraction ans = fraction(a.numerator*b.denominator - a.denominator*b.numerator, a.denominator*b.denominator);
		return simplify(ans);
	}
	else
	{
		return fraction(a.value - b.value);
	}
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
		return fraction(a.value*b.value);
	}
}


fraction reciprocal(const fraction& a)
{
	if (a.isApprox == false)
	{
		if (a.denominator.isZero() || a.numerator.isZero())throw Exceptions(_Divide_By_Zero);

		fraction ans(a.denominator, a.numerator);//The reciprocal.
		return simplify(ans);
	}
	else
	{
		if (a.value.isZero())throw Exceptions(_Divide_By_Zero);
		return fraction(Double((Int)1, 0) / a.value);
	}
}

fraction operator/(const fraction& a, const fraction& b)
{
	return a * reciprocal(b);
}

fraction& fraction::operator+=(const fraction& b)
{
	*this = (*this) + b;
	return *this;
}
fraction& fraction::operator-=(const fraction& b)
{
	*this = (*this) - b;
	return *this;
}
fraction& fraction::operator/=(const fraction& b)
{
	*this = (*this) / b;
	return *this;
}
fraction& fraction::operator*=(const fraction& b)
{
	*this = (*this) * b;
	return *this;
}

void displayFrac(const fraction& a, bool newline)
{
	if (!a.GetApprox())
	{
		if (a.denominator.isZero())throw Exceptions(_Divide_By_Zero);

		if (a.numerator.isZero())
		{
			cout << 0;
		}
		else
		{
			if (a.denominator==Int_one)
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
		cout << a.value;
	}

	if (newline)
		cout << endl;

}

//Fast power calculation.
fraction pow(const fraction& frc, const int& n)
{

	if (frc.GetValue() <= 0)throw Exceptions(_Negative_Base);

	if (n == 0)
	{
		return frc_one;
	}
	else if (n > 0)//Fast power Algorithm
	{
		fraction base = frc;
		fraction ans = frc_one;
		int _n = n;

		while (_n)
		{
			if (_n & 1)ans *= base;
			base *= base;
			_n >>= 1;
		}
		return ans;
	}
	else
	{
		if (frc == frc_zero)throw Exceptions(_Negative_Base);
		return pow(reciprocal(frc), -n);
	}

}

bool isInt(const fraction& frc)
{
	if (frc.isApprox == true)return false;
	fraction temp = simplify(frc);
	if (temp.denominator == Int_one)return true;
	else return false;
}


ostream& operator<<(ostream& ost, const fraction& frac)
{
	displayFrac(frac, false);
	return ost;
}
