#include<ctype.h>
#include<cstring>
#include<string>
#include<algorithm>


#include <iostream>
using namespace std;

void stringlower(char * s)    //δ��ʼ�����ַ��������󣬻�����쳣���쳣�����ڣ�        ǰһ������ı���  ��
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





//1�� + - 
//2�� * / %
//3�� a b c d

//n�� )

int  compare(char a,char b)  // ����1���Գ�ջ
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
char shortstr(char* &p,char* temp)    //�����ʾ����û����ѭ��һְ��ԭ��
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


void infixToSuffix(char * infix,char * suffix)//�����룺���߼�����
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
		if(infix[i] <='9' && infix[i]>= '0' || infix[i] == '.')   //����ֱ����suffix
		{
			while(i<strlen(infix) && (infix[i] <='9' && infix[i]>= '0' || infix[i] == '.'))
			{
				suffix[j++] = infix[i++];
			}
			suffix[j++] = ' ';
			-- i;      //ϸ�ڴ���
			continue;
		}

		if(infix[i] == '(')    //������ջ
		{
			stack[top++] = '(';
			continue;
		}
		if(infix[i] == ')')  //���������ȼ����ֱ�ӳ�ջ
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

		//��������
		while(top>0 && compare(stack[top-1],infix[i]))  //��ջ�ڷ������ȼ�С�ģ�ʹ�����ȼ����ų�ջ����suffix
		{
			suffix[j++] = stack[top-1];
			suffix[j++] = ' ';
			top --;
		}
		stack[top++] = infix[i];  //ջ�ڷ������ȼ����Ƚ�С�����÷�����ջ

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
