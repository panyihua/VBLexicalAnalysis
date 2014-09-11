#include <iostream>
#include<iomanip>
#include<fstream>
#include<string>
#include<algorithm>
#include<set>
#include<sstream>
#include<map>
#include<cstring>
#include<vector>
#include<list>
#include"util.h"
using namespace std;


set<string> keywordSet;  //�ؼ���
set<string> typeSet; // ����
map<string,set<string>> propertyMap; // ���;��е�����

char bitmap[32];
char bitmapNum[32];

map<string,string> objectMap;  //����

void initBitmap(char * bitmap,char * ch)
{
	int i;
	memset(bitmap,0,sizeof(bitmap));
	for(i=0;i<strlen(ch);i++)
		bitmap[ch[i]>>3] |= 1<< (7 & ch[i]);
}

char checkBitmap(char * bitmap,char ch)
{
	return bitmap[ch>>3] & (1<<(ch & 7));

}

void init()
{
	///////////////////
	//��ʼ��λͼ�����ڼ��ָ��ַ�ch ������chNum
	//////////////////
	char* ch = "\t\n ,=><()+-*/%&|";
	char * chNum = "0123456789";

	initBitmap(bitmap,ch);
	initBitmap(bitmapNum,chNum);

	/////////////////
	//��ȡ�����ļ�����ʼ���ؼ����ֵ�
	////////////////
	char line[200];
	char * ptr;
	ifstream ifile("config.ini");
	int status  = 0;
	string s;
	string currentType = "";
	set<string> currentProperties;
	while(ifile.getline(line,sizeof(line)))
	{
		ptr = line;
		if(*line == 0)
			continue;
		if(*ptr == '$')
		{
			++ptr;
			if(strcmp(ptr,"keyword") == 0)
			{
				status = 1;
				continue;
			}

			if(strcmp(ptr,"type") == 0)
			{
				status = 2;
				continue;
			}
			if(strcmp(ptr,"object") == 0)
			{
				status = 3;
				continue;
			}
			status = 0;

		}
		switch(status)
		{
		case 1:
			stringlower(ptr);
			keywordSet.insert(ptr);
			break;
		case 2:
			if(*ptr == '*')
			{
				if(currentType != "")
				{
					propertyMap[currentType] = currentProperties;
				}

				++ptr;
				currentType.assign(ptr);
				currentProperties.clear();
				stringlower(currentType);
				typeSet.insert(currentType);
				break;
			}


			while(*ptr!=' ' && *ptr != '\0')  ///�߼���   ʹptr���ӣ��ᷢ����ȷ���쳣��
				++ptr;                  //++ptr������Ҫ��һ��������'\0'������ptr����ָ���ַ�����
			*ptr = '\0';
			s.assign(line);
			stringlower(s);
			currentProperties.insert(s);
			break;
		case 3:
			{
				char obj[50] ,ty[50];       //δ��ʼ����char* �����ַ���  ������
				sscanf(line,"%s%s",obj,ty);
				stringlower(obj);
				stringlower(ty);
				objectMap[string(obj)] = string(ty);
				break;
			}
		default:
			break;
		}
	}
	if(currentType != "")  //���� propertyMap�Ĵ��������� 
	{
		propertyMap[currentType] = currentProperties;
	}
	


}
enum wordType
{
	H_error = -1,
	H_finish = 0,
	H_identify = 1,
	H_dot = 2,
	H_string = 3,
	H_number = 4,
	H_annotation = 5,
	H_operation = 6,
	
};

wordType analyseLineHead(char * &line,char * head)      //��line�зֽ��head
{
	int i=0,j=0;


	while(*line != '\0' && bitmap[*line >>3] & 1<<(*line&7))  //���˷ִʷ���    //����ʾ�������˵Ķ���������������
			++line;

	if(*line == '.')  //���⴦��.��
	{
		head[0] = '.';
		head[1] = '\0';
		++line;
		return H_dot;   //headΪ��.��
	}
	if(*line == '\'')   //����ַ���
	{
		return H_annotation;
	}

	if(bitmapNum[*line>>3] & 1<<(*line & 7))      //�������
	{
		++line;
		while(*line != '\0' && bitmapNum[*line>>3] & 1<<(*line & 7))
			++line;

		if(*line == '\0')
			return H_finish;

		return  H_number;
	}

	if(*line == '\"')   //����ַ���
	{
		++line;
		while(*line!='\0' && *line != '\"')
			++line;
	
		if(*line == '\0')
			return H_finish;

		++line;
		return H_string;  //headΪ�ַ���
		
	}

	i = 0;
	while(*line!='\0' && !checkBitmap(bitmap,*line) && *line != '.' && *line != '\"')
	{
		head[i++] = *line;
		++ line;
	}
	head[i] = '\0';

	if(i == 0)  //line�������
		return H_finish;
	else
		return H_identify;     //headΪ��ʶ��
}


int checkProperty(string& object,string& prpt)
{
	string type = objectMap[object];
	set<string> properties = propertyMap[type];
	if(properties.find(prpt) == properties.end())
		return 0;
	else
		return 1;
}

int checkType(string & type)
{
	if(typeSet.find(type) != typeSet.end())
		return 1;
	else
		return 0;
}



int checkWord(string word)
{
	if(keywordSet.find(word) != keywordSet.end())
		return 1;

	if(objectMap.find(word) != objectMap.end())
		return 1;

	if(typeSet.find(word) != typeSet.end())
		return 1;

	return 0;
}

void printWrongInfo(int row,int col,string & wrongReason)
{
	cout<<"��"<<row<<"��\t"<<"��"<<col<<"��:\t";
	cout<<wrongReason<<endl;
	wrongReason.clear();
}

void controllerScaner()
{
	int rowNum=0,colNum=0;
	ifstream fin;
	string controller;
	
	int status = 0;
	string word1,word2,word3;
	char line[1024];

	char *ptr,head[50];

	string wrongReason;
	stringstream ss;
	fin.open("test.OAF");
	cout<<"==========�ؼ����==========="<<endl;
	if (!fin)
		return ;
	while(!fin.eof() && status != -1000)
	{
		fin.getline(line,sizeof(line));
		switch(status)
		{
		case 0:
			colNum = 0;
			++rowNum;
			ptr = line;

			analyseLineHead(ptr,head);
			++colNum;
			word1.assign(head);
			if(word1.empty())
				break;
			stringlower(word1);
			if(word1 != "begin")   //
			{
				printWrongInfo(rowNum,colNum,wrongReason);
				return;
				break;
			}

			
			analyseLineHead(ptr,head);
			++colNum;
			stringlower(head);
			if(strcmp(head,"vb") != 0 || analyseLineHead(ptr,head) !=H_dot)
			{
				printWrongInfo(rowNum,colNum,wrongReason);
				return;
				break;
			}

			analyseLineHead(ptr,head);
			++colNum;
			word2.assign(head);
			stringlower(word2);
			if(typeSet.find(word2)== typeSet.end())  //
			{
				wrongReason ="�޴˿ؼ�����:  "+word2;
				printWrongInfo(rowNum,colNum,wrongReason);
				return;
				break;
			}
			controller = word2;

			
			analyseLineHead(ptr,head);
			word3.assign(head);
			++colNum;
			stringlower(word3);
			if(word3 == "")
			{
				printWrongInfo(rowNum,colNum,wrongReason);

				return;
				break;
			}
			if(objectMap.find(word3) != objectMap.end())
			{
				wrongReason = "�������ظ���  "+ word3;
				printWrongInfo(rowNum,colNum,wrongReason);

				return;
				break;
			}

			objectMap[word3] = word2;
			status = 1;
			break;


		case 1:
			{
				colNum = 0;
				
				++rowNum;
				ptr = line;


				if(analyseLineHead(ptr,head) == H_finish)
					break;
				word1.assign(head);
				++colNum;
				if(word1.empty())
					break;
				stringlower(word1);
				if(word1 == "end")
				{
					status = 0;
					break;
				}


				set<string> propertys = propertyMap[controller];
				if(propertys.find(word1) == propertys.end())
				{
					wrongReason = "�ؼ��޴�����:"+ word1;
					printWrongInfo(rowNum,colNum,wrongReason);
					break;
				}


				analyseLineHead(ptr,head);
				word3.assign(head);
				++colNum;
				if(word3 == "")
				{
					printWrongInfo(rowNum,colNum,wrongReason);
					break;
				}

				break;
			}

	
		
		}
	}

	fin.close();
}

void funcScaner()  //�����˲������������ģ���λ��ֲ�����
{
	ifstream fin;
	fin.open("test1.OAF");
	char line[1024];
	char head[50];
	int len;
	char * ptr;
	string wrongReason;
	string word,word1,word2;
	string keyhead;
	wordType wt;
	int row,col;
	row = 0;
	col = 0;
	cout<<endl<<"=========�ӳ�����========="<<endl;
	string pre_head;
	set<string> currentObjectSet;   //ϸ�ڴ�������������
	while(!fin.eof())
	{
		fin.getline(line,sizeof(line));
		++ row;
		col = 0;
		ptr = line;
		analyseLineHead(ptr,head);
		stringlower(head);
		++ col;
		keyhead.assign(head);
		
		if(keyhead == "dim")
		{
			++ col;
			if(analyseLineHead(ptr,head) == H_finish)
			{
				printWrongInfo(row,col,wrongReason);
				continue;
			}
			word.assign(head);  //���������
			stringlower(word);
			analyseLineHead(ptr,head);
			++ col;
			stringlower(head);
			if(strcmp(head,"as")!= 0)
			{
				printWrongInfo(row,col,wrongReason);
				continue;
			}

			
			analyseLineHead(ptr,head);
			stringlower(head);
			word1.assign(head);   //word1�����������
			++col;
			if(typeSet.find(word1) == typeSet.end())
			{
				
				wrongReason = "�޴����ͣ�" + word1;
				printWrongInfo(row,col,wrongReason);
				continue;
			}

			if(objectMap.find(word) != objectMap.end())
			{
				wrongReason = "�����ض��壺" + word;
				printWrongInfo(row,col,wrongReason);
				continue;

			}else
			{
				objectMap[word] = word1;
				currentObjectSet.insert(word);
			}

			continue;
		}

		if(keyhead == "private" || keyhead == "public")              //
		{
			
			analyseLineHead(ptr,head);
			stringlower(head);
			++col;
			if(strcmp(head,"sub")!= 0 && strcmp(head,"function")!= 0)
			{
				printWrongInfo(row,col,wrongReason);
				continue;

			}

			++col;
			if(analyseLineHead(ptr,head) == H_finish)     
			{
				printWrongInfo(row,col,wrongReason);
				continue;
			}else    //���뺯������
			{
				word.assign(head);
				stringlower(word);
				objectMap[word] = "function";

				//////ϸ�ڴ�������������
				set<string>::iterator it;
				for(it = currentObjectSet.begin();it != currentObjectSet.end();++it)
				{
					objectMap.erase(*it);
				}
				currentObjectSet.clear();
				/////

				string s1,s;
				while(analyseLineHead(ptr,head)!= H_finish)
				{
					++ col;
					stringlower(head);
					if(strcmp(head,"as") == 0)
					{
						analyseLineHead(ptr,head);
						s1.assign(head);
						stringlower(s1);
						++ col;
						if(!checkType(s1))
						{
							printWrongInfo(row,col,wrongReason);
							break;
						}else
						{
							if(s.empty()) // ��������ֵ��ʱ������
								break;
							if(objectMap.find(s) != objectMap.end())
							{
								wrongReason = "�����ض��壺" + s;
								printWrongInfo(row,col,wrongReason);
								break;
							}else
							{
								objectMap[s] = s1;
								currentObjectSet.insert(s);
								s.clear();
							}
						}

					}else
					{
						s.assign(head);
					}
				}
				continue;
			}
		}


		ptr = line;
		col = 0;
		while(1)
		{
			
			wt = analyseLineHead(ptr,head);
			word.assign(head);
			if(wt == H_finish || wt == H_annotation)
			{
				break;
			}
			if(wt == H_string || wt == H_number)
			{
				++col;
				pre_head = "";
				continue;
			}
			if(wt == H_dot)
			{
				analyseLineHead(ptr,head);
				word.assign(head);
				stringlower(word);
				++col;
				if(!checkProperty(pre_head,word))
				{
					wrongReason = "����"+pre_head +"���������ԣ�"+word;
					printWrongInfo(row,col,wrongReason);
					break;
				}
				continue;
			}
			if(wt == H_identify)
			{
				++col;
				stringlower(word);
				if(!checkWord(word))
				{
					wrongReason = "�﷨����" + word;
					printWrongInfo(row,col,wrongReason);
					break;
				}

				pre_head = word;
			}

			
		}
	}
	fin.close();

}

char bitmapOperation[32];

wordType getWord(char * &line,char* head)
{
	char * opch = ",=><()+-*/%&|.";
	initBitmap(bitmapOperation,opch);
	int i=0;
	while(*line!= '\0' && (*line ==' ' || *line == '\t')) //���˿ո�
		++ line;

	if(*line == '\0')
	{
		return H_finish;
	}

	i = 0;
	if(checkBitmap(bitmapOperation,*line))  //������
	{
		head[i++] = *line;
		++line;
		switch (head[0])
		{
		case '>':
		case '<':
			if(*line == '=')
			{
				head[i++] = *line;
				++line;
			}
			break;
		case '+':
			if(*line == '+')
			{
				head[i++] = *line;
				++line;
			}
			break;
		case '-':
			if(*line == '-')
			{
				head[i++] = *line;
				++line;
			}
			break;
		}

		head[i] = '\0';
		return H_operation;
	}

	if(*line == '\'')   //ע��
	{
		strcpy(head,line);
		*line = '\0';
		return H_annotation;
	}
	 i = 0;
	if(checkBitmap(bitmapNum,*line))      //�������
	{
		head[i++] = *line;
		++line;
		while(checkBitmap(bitmapNum,*line))
		{
			head[i++] = *line;
			++line;
		}
		head[i] = '\0';
		return  H_number;
	}

	i = 0;
	if(*line == '\"')   //����ַ���
	{
		head[i++] = *line;
		++line;
		while(*line!='\0' && *line != '\"')
		{
			head[i++] = *line;
			++line;
		}
		head[i++] = '\"';
		head[i] = '\0';
		if(*line == '\"')
			++line;

		return H_string;  //headΪ�ַ���
		
	}
	i = 0;
	int j;
	while(*line!='\0' && ! (j = checkBitmap(bitmap,*line)) && *line != '.' && *line != '\"')
	{
		head[i++] = *line;
		++ line;
	}
	head[i] = '\0';

	if(i == 0)  //line�������
		return H_finish;
	else
		return H_identify;     //headΪ��ʶ��
}

struct Lexical
{
	string word;
	wordType type;
};

vector<Lexical> lexicals;

void lexicalAnalyse()
{
	char line[500];
	char head[100];
	char * pline;
	int i;
	ifstream fin("test1.OAF");
	
	wordType wt;
	Lexical lexical;
	while(fin.getline(line,sizeof(line)))
	{
		pline = line;
		while(1)
		{
			wt = getWord(pline,head);
			if(wt == H_finish)
				break;
			lexical.type = wt;
			lexical.word.assign(head);
			lexicals.push_back(lexical);
		}
	}
	for(i = 0; i < lexicals.size(); i++)
	{
		cout<<left<<setw(30)<<lexicals[i].word<<lexicals[i].type<<endl;
	}

	fin.close();
}

void main()
{
	/*init();
	controllerScaner();
	funcScaner();*/
	/*char in[50] = "2+(   3*(23421 -32)+1)";
	char out[50];
	infixToSuffix(out,in);
	cout<<out<<endl;*/
	/*lexicalAnalyse();*/
	vector<string*> vec;
	string *s2 = new string("aaa");
	vec.push_back(s2);
	(*s2)[0] = 'q';
	system("pause");
}
