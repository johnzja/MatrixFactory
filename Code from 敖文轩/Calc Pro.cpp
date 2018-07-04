#include <iostream>
#include <string>
#include <stdlib.h>
#include <cmath>
#include <windows.h>    //�������ؼ�
#define FB 0x0001 // text color contains blue.
#define FG 0x0002 // text color contains green.
#define FR 0x0004 // text color contains red.
#define FI 0x0008 // text color is intensified.
#define FW FR|FG|FB
#define MAX_INPUT 1000  //�����������ʶ������
#define MAX_VAR 100    //���������ͺ�����Ŀ
#define MAX_XS 9       //����Ա�����Ŀ
using namespace std;

//������
//3 + 4 * 2 / (1 - 5) ^ 2 ^ 3
//f(x) = x*f(x - 1)
//f(0) = 1
//f(20)
//g(x) = g(x - 1) + g(x - 2)
//g(1) = 1
//g(2) = 1
//g(10)

enum opatype {
	nul,num,alp,sym,fun,var,
	add1,mnu1,mlt1,div1,div2,pow1,lp,rp,mod1,equ1,dli
};
struct opa {
	string name="";
	opatype type=nul;  //��ʶ�������ݺ����
	int prev=-1; //���ȼ�,��ʼ��Ϊ-1
};
struct varlist {
	string name="";
	double val=0;
};
struct funclist {
	string name = "";
	opa def[MAX_INPUT];
	string vars[MAX_XS-1];  //��ຬ��MAX_XS������
	struct {
		struct {
			opa vars[MAX_XS-1];
			double val;
		} list[MAX_INPUT];  //����ֵ�洢
		int len;
	} known;
	int version = 0, len = 0, def_len = 0;//�������ָ���֮��ĺ���,������
};
struct {
	varlist list[MAX_VAR] = { {"pi",3.141592653589793},{"e",2.718281828} };
	int len=2;
} vars;
struct {
	funclist list[MAX_VAR];
	int len=0;
} funcs;
string parse(string, string *);  //�������벢���ؽ��
int stdlize(string, opa *);     //���ַ�������ת��Ϊopa���飬�������鳤��
double def_var(opa *, int);     //opa���鼰�䳤��
void def_func(opa *, int, int);//opa���鼰�䳤�ȡ����ںŵ�λ��
double compute(opa *, int, int);//opa���飬����㲿�ֵ������յ�
int getvar(string);//��ȡ����ֵ��û���򷵻�-1
int getfunc(string);
double fapply(int, double *, int *);//������ţ���ֵջ��ջָ��λ��
void re_var(opa *, int, string, double); //�滻opa��������Ϊstring�ı���Ϊ��ֵ
const string infuncs = ",sin,,,,cos,,,,tan,,,,cot,,,,csc,,,,sec,,,,ln,,,,,lg,,,,,exp,,,,ceil,,,floor,,gamma,,arcsin,arccos,arctan,C,,,,,,P,,,,,,abs,,,,";//���ú�����

void ex_d(double *a, double *b) { //��������double��ֵ
	double tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}

void ctext(int col) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), col);
}

void testt(opa input[],int len) {
	int i;
	cout << "-------------------------" << endl;
	for (i = 0; i < len; i++) {
		cout << input[i].name << "	" << input[i].type << endl;
	}
	cout << "-------------------------" << endl;
}

int main() {
	int in_count = 0;
	string input, output, re;
	system("title Calc Pro - Ao&color 07");
	for (;;) {
		ctext(FG | FI);
		cout << endl << "In[" << ++in_count << "]:= ";
		ctext(FW);
		cin >> input;
		re = parse(input, &output);
		if (re != "") {
			ctext(FG | FI);
			cout << "Out[" << in_count << "]= ";
			ctext(FW);
			cout << re;
		}
		cout << endl;
	}
	system("pause");
}

string parse(string input, string *output) {
	opa input_p[MAX_INPUT];
	int input_p_len, i, ematch=0, pmatch=0,eptr;
	//ע��len��ֵΪ���ȣ�Ҫ-1
	input_p_len=stdlize(input, input_p);
	for (i = 0; i < input_p_len; i++) {
		if (input_p[i].type == alp) {
			if (input_p[i + 1].name == "(") {
				input_p[i].type = fun;
				input_p[i].prev = 9;
			}
			else {
				input_p[i].type = var;
				//if (ematch == 0) vars[(int)vars[0].val++].name = input_p[i].name;
			} ///////////////////????????????????????
		}
		else {
			if (input_p[i].type == sym) {
				switch (input_p[i].name[0]) {
				case '+': input_p[i].type = add1; input_p[i].prev = 5; break;
				case '-': input_p[i].type = mnu1; input_p[i].prev = 5; break;
				case '*': input_p[i].type = mlt1; input_p[i].prev = 6; break;
				case '/': input_p[i].type = div1; input_p[i].prev = 6; break;
				case '\\': input_p[i].type = div2; input_p[i].prev = 6; break;
				case '%': input_p[i].type = mod1; input_p[i].prev = 6; break;
				case '^': input_p[i].type = pow1; input_p[i].prev = 7; break;
				case '(': input_p[i].type = lp; input_p[i].prev = 0; pmatch++; break;
				case ')': input_p[i].type = rp; input_p[i].prev = 0; pmatch--; break;
				case '=': input_p[i].type = equ1; ematch += 1;
					if (ematch == 1) eptr = i; break;
				case ',': input_p[i].type = dli; input_p[i].prev = 0; break;
				}
			}
		}
		if (pmatch < 0) {
			ctext(FR | FI);
			cout << "Error:Unmatched parathesis!" << endl;
			ctext(FW);
			return "$Aborted";
		}
	}
	if (pmatch != 0) {
		ctext(FR | FI);
		cout << "Error:Unmatched parathesis!" << endl;
		ctext(FW);
		return "$Aborted";
	}
	if (ematch > 1) {
		ctext(FR | FI);
		cout << "Error:" << ematch << " ='s are found!" << endl;
		ctext(FW);
		return "$Aborted";
	}
	if (ematch == 1) {
		if (input_p[0].type == var) {
			return to_string(def_var(input_p, input_p_len));
		}
		else {
			def_func(input_p,input_p_len,eptr);
			return "";
		}
	}
	return to_string(compute(input_p,0,input_p_len-1));
}

int stdlize(string input, opa *outp) {
	int i,outptr=0,s;
	opa last;
	for (i = 0; i < (int)input.size(); i++) {
		opa now;
		now.name = input[i];
		//���type
		if (((s = (char)now.name[0]) <= '9'&&s >= '0') || s == '.') {
			now.type = num;
			now.prev = -2;  //�����壬����-1����
		}
		else if ((s <= 'Z'&&s >= 'A') || (s <= 'z'&&s >= 'a')) {
			now.type = alp;
			now.prev = -2;
		}
		if (now.type == nul) now.type = sym;

		//�Ա�ǰ��type������ ����/push
		if (last.type == nul || now.type == last.type) {
			if (now.type == sym&&last.type != nul) outptr++;
		}			
		else {
			outptr++;
		}
		outp[outptr].name += now.name;
		outp[outptr].type = now.type;
		outp[outptr].prev = now.prev;
		last = now;
	}
	return ++outptr;
}

double def_var(opa *input, int len) {
	int index;
	if (len <= 2) {
		ctext(FR | FI);
		cout << "Error:No expression for [" << input[0].name << "]!" << endl;
		ctext(FW);
		return 0;
	}
	else {
		if ((index = getvar(input[0].name)) == -1) {
			vars.list[vars.len].name = input[0].name;
			vars.list[vars.len].val = compute(input, 2, len - 1);
			vars.len++;
			cout << "[" << input[0].name << "] Defined." << endl;
		}
		else {
			vars.list[index].val= compute(input, 2, len - 1);
			cout << "[" << input[0].name << "] Overwritten." << endl;
		}
		return vars.list[vars.len - 1].val;
	}
		
}

void def_func(opa *input, int len, int eptr) {
	int i = 0, index = -1;
	//����Ƿ�����
	if (getfunc(input[0].name) < -1) {
		ctext(FR | FG | FI);
		cout << "Warning:[" << input[0].name << "] Defined Inside. No overwriting." << endl;
		ctext(FW | FI);
		cout << "Reference:List of inside functions:" << infuncs << endl;
		ctext(FW);
		return;
	}
	while ((i += 2) < eptr - 1) if (input[i].type != var) index = 0;//index����Ƿ��Ƕ��峣��,i����������
	if (i >= 2 * MAX_XS) {
		ctext(FR | FI);
		cout << "Function Error:Too many varibles." << endl;
		ctext(FW);
		return;
	}
	if (index == 0) {
		if ((index = getfunc(input[0].name)) == -1) {
			for (i = 2; i < eptr - 1; i += 2) {
				funcs.list[funcs.len].known.list[funcs.list[funcs.len].known.len].vars[i / 2 - 1] = input[i];
			}  //�������ı����б�洢����
			funcs.list[funcs.len].known.list[funcs.list[funcs.len].known.len].val = compute(input, eptr + 1, len - 1);
			funcs.list[funcs.len].name = input[0].name;
			funcs.list[funcs.len].len = i / 2 - 1;
			funcs.list[funcs.len].version = 1;
			funcs.list[funcs.len].known.len++;
			funcs.len++;
			cout << "Function [" << input[0].name << "] Partly Defined." << endl;
		}
		else {
			for (i = 2; i < eptr - 1; i += 2) {
				funcs.list[index].known.list[funcs.list[index].known.len].vars[i / 2 - 1] = input[i];
			}  //�������ı����б�洢����
			funcs.list[index].known.list[funcs.list[index].known.len].val = compute(input, eptr + 1, len - 1);
			funcs.list[index].known.len++;
			cout << "Function [" << input[0].name << "] Partly Defined." << endl;
		}
	}
	else {
		if ((index = getfunc(input[0].name)) == -1 ) {
			for (i = 2; i < eptr - 1; i += 2) {
				funcs.list[funcs.len].vars[i / 2 - 1] = input[i].name;
			}  //�������ı����б�洢����
			funcs.list[funcs.len].name = input[0].name;
			funcs.list[funcs.len].len = i / 2 - 1;
			funcs.list[funcs.len].version = 1;
			for (i = eptr + 1; i < len; i++) {
				funcs.list[funcs.len].def[i - eptr - 1] = input[i];
			}
			funcs.list[funcs.len].def_len = i-eptr-1;
			funcs.len++;
			cout << "Function [" << input[0].name << "] Defined." << endl;
		}
		else {
			for (i = 2; i < eptr - 1; i += 2) {
				funcs.list[index].vars[i / 2 - 1] = input[i].name;
			}  //�������ı����б�洢����
			funcs.list[index].len = i / 2 - 1;
			funcs.list[index].version++;
			for (i = eptr + 1; i < len; i++) {
				funcs.list[index].def[i - eptr - 1] = input[i];
			}
			funcs.list[index].def_len = i-eptr-1;
			cout << "Function [" << input[0].name << "] Redefined." << endl;
		}
	}
}

double compute(opa *input, int start, int end) {
	opa sysms[100], result[200], ex_lp;//��ȡһ��(��ʵ��
	int i, result_len = 0, sysms_len = 0, last_lp = 0;
	opatype t;

	double numstack[100], a1, a2, re;
	int nsptr = 0,index;//numstackָ�룬var���
	/*     Part 1    �㷨����
			num/varֱ������result��
			func��������prefix��ֱ������sysms��
			lpѹ��sysms����¼λ�ã��ȴ�rp������
			dliʹ�����ŵȼۣ��� "," -> "),(" ��
			��ͨ�����:���ϵ������ڵ��ڣ��ҽ�ϵ�������(ֻ��pow)

			����sysmsʣ���ȫ������
	*/
	for (i = start; i <= end; i++) {
		if ((t = input[i].type) ==num||t==var) {
			result[result_len++] = input[i];
		}
		else {
			if (t == lp) {
				ex_lp = sysms[last_lp = sysms_len++] = input[i];//��¼��һ��(��λ��
			}
			else {
				if (t == fun) {
					sysms[sysms_len++] = input[i];
				}
				else {
					if (t ==rp || t ==dli) {
						for (sysms_len--; sysms[sysms_len].type != lp;) result[result_len++] = sysms[sysms_len--];
						//��sysms��һֱ������resultֱ������(��������(
						if (t == dli) sysms[sysms_len++] = ex_lp;
					}
					else {
						if (input[i].prev == -1) {
							ctext(FR | FI);
							cout << "Error:Undefined syntax - [" << input[i].name << "]" << endl;
							ctext(FW);
							return 0;
						}
						if (sysms_len > 0) {
/*							while (sysms[--sysms_len].prev >= input[i].prev) {
								if (!(input[i].type == pow1&&sysms[sysms_len].prev == input[i].prev)) {
									result[result_len++] = sysms[sysms_len];
								}
								else {
									sysms[sysms_len++] = input[i];
								}
							}
*/
							if (input[i].type == pow1&&sysms[sysms_len-1].prev == input[i].prev) {
								while (sysms[--sysms_len].prev > input[i].prev) {
									result[result_len++] = sysms[sysms_len];
								}
							}
							else {
								while (sysms[--sysms_len].prev >= input[i].prev) {
									result[result_len++] = sysms[sysms_len];
								}
							}
							sysms_len++;
						}
						sysms[sysms_len++] = input[i];
					}
				}
			}
		}
	}
	while (--sysms_len>=0) result[result_len++] = sysms[sysms_len];
	//testt(result, result_len);///////////////////////////////////////////////////////
	
	/*     Part 2    �㷨����
			����ֱ����ջ
			func����fapply������
			��ͨ����ֱ�Ӽ���	
	*/
	for (i = 0; i < result_len; i++) {
		if ((t = result[i].type) == num) {
			numstack[nsptr++] = stod(result[i].name);
		}
		else {
			if (t == var) {
				if ((index = getvar(result[i].name)) != -1) {
					numstack[nsptr++] = vars.list[index].val;
				}
				else {
					ctext(FR | FG | FI);
					cout << "Warning:[" << result[i].name << "] Undefined" << endl;
					ctext(FW);
					numstack[nsptr++] = 0;
				}
			}
			else {
				if (t == fun) {
					if ((index = getfunc(result[i].name)) == -1) {
						ctext(FR | FI);
						cout << "Error:Function [" << result[i].name << "] Undefined!" << endl;
						ctext(FW);
						return 0;
					}
					else {
						if (index < -1) {
							re = fapply(index, numstack, &nsptr);
							numstack[nsptr++] = re;
						}
						else {
							if (nsptr < funcs.list[index].len) {
								ctext(FR | FI);
								cout << "Function error : [" << funcs.list[index].name << "] needs " << funcs.list[index].len << " arguments!" << endl;
								ctext(FW);
								return 0;
							}
							re = fapply(index, numstack, &nsptr);
							numstack[nsptr++] = re;
						}
					}
				}
				else {
					if (nsptr < 2) {
						ctext(FR | FI);
						cout << "Function error : [" << result[i].name << "] needs 2 arguments!" << endl;
						ctext(FW);
						return 0;
					}
					a2 = numstack[--nsptr];
					a1 = numstack[--nsptr];
					switch (result[i].type) {
					case add1:re = a1 + a2; break;
					case mnu1:re = a1 - a2; break;
					case mlt1:re = a1 * a2; break;
					case div1:re = a1 / a2; break;
					case div2:re = (int)(a1 / a2); break;
					case pow1:re = pow(a1, a2); break;
					case mod1:re = fmodf((float)a1, (float)a2); break;
					}
					numstack[nsptr++] = re;
				}
			}
		}
	}
	if (nsptr == 1) {
		return numstack[0];
	}
	else {
		ctext(FR | FI);
		cout << "Invalid Input!" <<nsptr<< endl;
		ctext(FW);
		return 0;
	}
	return 0;
}

int getvar(string name) {
	int i;
	for (i = 0; i < vars.len; i++) {
		if (vars.list[i].name == name) {
			return i;
		}
	}
	return -1;
}

int getfunc(string name) {
	int i;
	// ",sin,,,,cos,,,,tan,,,,cot,,,,csc,,,,sec,,,,ln,,,,,lg,,,,,exp,,,,ceil,,,floor,,gamma,,arcsin,arccos,arctan,C,,,,,,P,,,,,,"
	if ((i = infuncs.find("," + name + ",")) != string::npos) return -i - 7;
	for (i = 0; i < funcs.len; i++) {
		if (funcs.list[i].name == name) {
			return i;
		}
	}
	return -1;
}

double fapply(int index, double stack[], int *ptr) {
	//��stack���������Һ������ڵ�ǰ����
	int num_len, i, j;
	double *numm, re, re2;
	opa *expre;
	//����Ƿ����ú���
	if (index <-1) {
		index /= -7;
		re = stack[--*ptr];
		// ",sin,,,,cos,,,,tan,,,,cot,,,,csc,,,,sec,,,,ln,,,,,lg,,,,,exp,,,,ceil,,,floor,,gamma,,arcsin,arccos,arctan,C,,,,,,P,,,,,,abs,,,,"
		switch (index) {
		case 1:re = sin(re); break;
		case 2:re = cos(re); break;
		case 3:re = tan(re); break;
		case 4:re = 1/tan(re); break;
		case 5:re = 1/sin(re); break;
		case 6:re = 1/cos(re); break;
		case 7:re = log(re); break;
		case 8:re = log10(re); break;
		case 9:re = exp(re); break;
		case 10:re = ceil(re); break;
		case 11:re = floor(re); break;
		case 12:re = tgamma(re); break;
		case 13:re = asin(re); break;
		case 14:re = acos(re); break;
		case 15:re = atan(re); break;
		case 16:re2 = stack[--*ptr]; re = tgamma(re2 + 1) / tgamma(re2 - re + 1) / tgamma(re + 1); break;
		case 17:re2 = stack[--*ptr]; re = tgamma(re2 + 1) / tgamma(re2 - re + 1); break;
		case 18:re = abs(re); break;
		}
		return re;
	}
	num_len = funcs.list[index].len;
	numm = new double[num_len - 1];
	expre = new opa[funcs.list[index].def_len];
	for (i = 1; i <= num_len; i++) {
		numm[num_len - i] = stack[--*ptr];
	}
	//�����֪��
	for (i = 0; i < funcs.list[index].known.len; i++) {
		re = 0;
		for (j = 0; j < num_len; j++) {
			if (funcs.list[index].known.list[i].vars[j].type != var &&
			 stod(funcs.list[index].known.list[i].vars[j].name) != numm[j]) {
				re = -1;
			}
		}
		if (re == 0) {
			return funcs.list[index].known.list[i].val;
		}
	}
	//����֪��ֱ�Ӽ��㲢��ӵ���֪
	/*for (i = 0; i < num_len; i++) {
		if ((j = getvar(funcs.list[index].vars[i])) == -1) {
			vars.list[vars.len] = { funcs.list[index].vars[i],0 };
			ex_d(&vars.list[vars.len].val, &numm[i]);
			vars.len++;
		}
		else {
			ex_d(&vars.list[j].val, &numm[i]);
		}
	}
	re = compute(funcs.list[index].def, 0, funcs.list[index].def_len - 1);
	for (i = 0; i < num_len; i++) {
		ex_d(&numm[i], &vars.list[getvar(funcs.list[index].vars[i])].val);
		funcs.list[index].known.list[funcs.list[index].known.len].vars[i] = { to_string(numm[i]),num,-1 };
	}*/
	for (i = 0; i < funcs.list[index].def_len; i++) {
		expre[i] = funcs.list[index].def[i];
	}
	for (i = 0; i < num_len; i++) {
		re_var(expre, funcs.list[index].def_len, funcs.list[index].vars[i], numm[i]);
	}
	/*cout << "sdf";
	testt(expre, funcs.list[index].def_len);*/
	re = compute(expre, 0, funcs.list[index].def_len - 1);
	for (i = 0; i < num_len; i++) {
		funcs.list[index].known.list[funcs.list[index].known.len].vars[i] = { to_string(numm[i]),num,-2 };
	}
	funcs.list[index].known.list[funcs.list[index].known.len].val = re;
	funcs.list[index].known.len++;
	funcs.list[index].known.len %= MAX_INPUT; //ѭ�����ô洢�ռ�
	return re;
}

void re_var(opa *input, int len, string var, double val) {
	for (; --len >= 0;) {
		if (input[len].name == var) {
			input[len] = { to_string(val),num,-2 };
		}
	 }
}
