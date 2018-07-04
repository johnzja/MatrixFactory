//fraction.h
#pragma once
using namespace std;
const double precision = 1e-12;
#define MATH_ERROR "Math Error!"

#ifndef _FRACTION_H_
#define _FRACTION_H_

class Matrix;

struct fraction
{
public:
	fraction(int a, int b);
	fraction(int a);
	fraction(double value);
	fraction() //Default constructor:zero fraction
	{
		numerator = 0;
		denominator = 1;
		isSimplified = true;
		isApprox = false;
		value = 0.0;
	}

	virtual fraction** GetPtr() { return nullptr; }
	virtual int GetType()const { return 0; }
	virtual Matrix* GetThis() { return nullptr; }

	virtual ~fraction(){}

	double GetValue(void)const;

	friend fraction simplify(const fraction& a);
	friend bool operator==(const fraction& a, const fraction & b);
	friend bool isLessThanZero(const fraction& a);
	friend fraction operator+(const fraction& a, const fraction& b);
	friend fraction operator-(const fraction& a);
	friend fraction operator*(const fraction& a, const fraction& b);
	friend fraction reciprocal(const fraction& a);
	friend void displayFrac(const fraction& a, bool newline);
	friend bool isInt(const fraction& frc);
	friend fraction Convert_to_Fraction(const string& str, bool& Error);
	friend void CalculateOnce(stack<fraction*>* stkopnd, stack<char>* stkoptr);
	friend fraction* Func(const string& cmd, fraction* data, bool& Error);


	friend class Matrix;
	friend ostream& operator<<(ostream& out, const Matrix& mat);
	//friend Matrix inverse(const Matrix& mat);
	//friend void defineMat(string& cmd, bool CheckValidity);




private:

	int numerator;
	int denominator;
	double value;
	bool isApprox;
	bool isSimplified;
};

inline int gcd(const int& a, const int& b);
fraction simplify(const fraction& a);
bool operator==(const fraction& a,const fraction & b);

inline bool operator!=(const fraction& a, const fraction& b)
{
	return !(a == b);
}

bool isLessThanZero(const fraction& a);
bool operator<(const fraction& a, const fraction& b);

fraction operator+(const fraction& a, const fraction& b);
fraction operator-(const fraction& a);
fraction operator-(const fraction& a, const fraction& b);
fraction operator*(const fraction& a, const fraction& b);
fraction reciprocal(const fraction& a);
fraction operator/(const fraction& a, const fraction& b);
void displayFrac(const fraction& a, bool newline = false);
fraction pow(const fraction& frc, const int& n);
bool isInt(const fraction& frc);
ostream& operator<<(ostream& ost, const fraction& frac);


#endif
