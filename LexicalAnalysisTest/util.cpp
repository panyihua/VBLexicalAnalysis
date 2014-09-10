#include<ctype.h>
#include<cstring>
#include<string>
#include<algorithm>


#include <iostream>
using namespace std;

void stringlower(char * s)    //未初始化的字符串进来后，会出现异常，异常出现在：        前一个定义的变量  ！
{
	while(*s != '\0')
	{
		*s = tolower(*s);
		++s;
	}
}

void stringlower(std::string& s)
{
	std::transform(s.begin(),s.end(),s.begin(),tolower);
}





//1级 + - 
//2级 * / %
//3级 a b c d

//n级 )

int  compare(char a,char b)  // 返回1可以出栈
{
	if(a =='(')
		return 0;
	if(b == '+' || b == '-')
		return 1;

	if(b == '*' || b == '/' || b =='%')
		if(a == '+' || a == '-' || a == '(')
			return 0;
		else
			return 1;

	if(a == 'a' ||a == 'b' || a == 'c' || a == 'd')
		return 1;
	if(b == 'a' ||b == 'b' || b == 'c' || b == 'd')
		return 0;

	return 0;
}



//
char shortstr(char* &p,char* temp)    //错误的示范，没有遵循单一职责原则
{
	if(p[0] == temp[0] && p[1] == temp[1] && p[2] == temp[2])
	{
		p += 3;     ///X
		return 1;
	}
	return 0;
}
//sin -- a
//cos -- b
//tan -- c
//cot -- d
void preConvert(char * str)
{	stringlower(str);
	int i = 0;
	char * dest = str;
	while(*str)
	{
		if(shortstr(str,"sin"))
		{
			dest[i++] = 'a';
			continue;
		}
		if(shortstr(str,"cos"))
		{
			dest[i++] = 'b';
			continue;
		}
		if(shortstr(str,"tan"))
		{
			dest[i++] = 'c';
			continue;
		}
		if(shortstr(str,"cot"))
		{
			dest[i++] = 'd';
			continue;
		}
		dest[i++] = *(str++);

	}
	dest[i] = 0;
}


void infixToSuffix(char * infix,char * suffix)//读代码：有逻辑场景
{
	int top = 0;
	int stack[100];
	int i = 0;
	int j = 0;
	preConvert(infix);
	for(i = 0;i<strlen(infix);i++)
	{
		if(infix[i] == ' ')
			continue;
		if(infix[i] <='9' && infix[i]>= '0' || infix[i] == '.')   //数字直接入suffix
		{
			while(i<strlen(infix) && (infix[i] <='9' && infix[i]>= '0' || infix[i] == '.'))
			{
				suffix[j++] = infix[i++];
			}
			suffix[j++] = ' ';
			-- i;      //细节处理
			continue;
		}

		if(infix[i] == '(')    //括号入栈
		{
			stack[top++] = '(';
			continue;
		}
		if(infix[i] == ')')  //反括号优先级最大，直接出栈
		{
			while(top > 0 )
			{
				if(stack[top -1] == '(')
				{
					 -- top;
					 break;
				}
				suffix[j++] = stack[top-1];
				suffix[j++] = ' ';
				-- top;
			}
			continue;
		}

		//其他符号
		while(top>0 && compare(stack[top-1],infix[i]))  //比栈内符号优先级小的，使大优先级符号出栈并入suffix
		{
			suffix[j++] = stack[top-1];
			suffix[j++] = ' ';
			top --;
		}
		stack[top++] = infix[i];  //栈内符号优先级都比较小，将该符号入栈

	}
	while(top>0)   //
	{
		suffix[j++] = stack[top-- -1];
		suffix[j++] = ' ';
	}
	suffix[j] = '\0';
}
double calculateSuffix(char * str)
{
	double stack[100];
	int top = 0;
	double num = 0;
	char * word;
	word = strtok(str," ");
	while(word != NULL)
	{
		if(word[0] == ' ')
			continue;
		switch(word[0])
		{
		case '+':
			stack[top-2] += stack[top -1];
			-- top;
			break;
		case '-':
			stack[top-2] -= stack[top -1];
			-- top;
			break;
		case '*':
			stack[top-2] *= stack[top -1];
			-- top;
			break;
		case '/':
			stack[top-2] /= stack[top -1];
			-- top;
			break;
		case '%':
			stack[top-2] = (int)stack[top-1] % (int)stack[top -1] ;
			-- top;
			break;
		case 'a':
			stack[top - 1] = sin(stack[top -1]);
			break;
		case 'b':
			stack[top - 1] = cos(stack[top -1]);
			break;
		case 'c':
			stack[top - 1] = tan(stack[top -1]);
			break;
		case 'd':
			stack[top - 1] = tan(3.141592657589793/2 - stack[top-1]);
			break;
		default:
			stack[top ++] = atoi(word);
			break;
		}
		word = strtok(NULL," ");
	}
	return stack[0];

}


void main2()
{
	char ch[100],ch1[100];

	while(cin>>ch)
	{

		infixToSuffix(ch,ch1);
		cout<<ch1<<endl;
		cout<<calculateSuffix(ch1)<<endl;
	}
	system("pause");
}
