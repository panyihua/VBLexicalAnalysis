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


set<string> keywordSet;  //关键字
set<string> typeSet; // 类型
map<string,set<string>> propertyMap; // 类型具有的属性

char bitmap[32];
char bitmapNum[32];

map<string,string> objectMap;  //对象

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
	//初始化位图，用于检测分隔字符ch 与数字chNum
	//////////////////
	char* ch = "\t\n ,=><()+-*/%&|";
	char * chNum = "0123456789";

	initBitmap(bitmap,ch);
	initBitmap(bitmapNum,chNum);

	/////////////////
	//读取配置文件，初始化关键字字典
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


			while(*ptr!=' ' && *ptr != '\0')  ///逻辑错   使ptr增加，会发生不确定异常。
				++ptr;                  //++ptr，必须要有一个限制是'\0'，而且ptr必须指向字符串。
			*ptr = '\0';
			s.assign(line);
			stringlower(s);
			currentProperties.insert(s);
			break;
		case 3:
			{
				char obj[50] ,ty[50];       //未初始化的char* 不是字符串  ！！！
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
	if(currentType != "")  //保存 propertyMap的触发条件是 
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

wordType analyseLineHead(char * &line,char * head)      //从line中分解出head
{
	int i=0,j=0;


	while(*line != '\0' && bitmap[*line >>3] & 1<<(*line&7))  //过滤分词符号    //错误示范，过滤的动作让外面来处理
			++line;

	if(*line == '.')  //特殊处理‘.’
	{
		head[0] = '.';
		head[1] = '\0';
		++line;
		return H_dot;   //head为‘.’
	}
	if(*line == '\'')   //检测字符串
	{
		return H_annotation;
	}

	if(bitmapNum[*line>>3] & 1<<(*line & 7))      //检测数字
	{
		++line;
		while(*line != '\0' && bitmapNum[*line>>3] & 1<<(*line & 7))
			++line;

		if(*line == '\0')
			return H_finish;

		return  H_number;
	}

	if(*line == '\"')   //检测字符串
	{
		++line;
		while(*line!='\0' && *line != '\"')
			++line;
	
		if(*line == '\0')
			return H_finish;

		++line;
		return H_string;  //head为字符串
		
	}

	i = 0;
	while(*line!='\0' && !checkBitmap(bitmap,*line) && *line != '.' && *line != '\"')
	{
		head[i++] = *line;
		++ line;
	}
	head[i] = '\0';

	if(i == 0)  //line处理完毕
		return H_finish;
	else
		return H_identify;     //head为标识符
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
	cout<<"第"<<row<<"行\t"<<"第"<<col<<"列:\t";
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
	cout<<"==========控件检测==========="<<endl;
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
				wrongReason ="无此控件类型:  "+word2;
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
				wrongReason = "对象名重复：  "+ word3;
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
					wrongReason = "控件无此属性:"+ word1;
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

void funcScaner()  //包括了部分语义分析，模块层次划分不鲜明
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
	cout<<endl<<"=========子程序检测========="<<endl;
	string pre_head;
	set<string> currentObjectSet;   //细节处理：变量定义域
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
			word.assign(head);  //保存变量名
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
			word1.assign(head);   //word1保存变量类型
			++col;
			if(typeSet.find(word1) == typeSet.end())
			{
				
				wrongReason = "无此类型：" + word1;
				printWrongInfo(row,col,wrongReason);
				continue;
			}

			if(objectMap.find(word) != objectMap.end())
			{
				wrongReason = "变量重定义：" + word;
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
			}else    //加入函数定义
			{
				word.assign(head);
				stringlower(word);
				objectMap[word] = "function";

				//////细节处理：变量定义域
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
							if(s.empty()) // 函数返回值暂时不处理
								break;
							if(objectMap.find(s) != objectMap.end())
							{
								wrongReason = "变量重定义：" + s;
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
					wrongReason = "对象"+pre_head +"不具有属性："+word;
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
					wrongReason = "语法错误：" + word;
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
	while(*line!= '\0' && (*line ==' ' || *line == '\t')) //过滤空格
		++ line;

	if(*line == '\0')
	{
		return H_finish;
	}

	i = 0;
	if(checkBitmap(bitmapOperation,*line))  //操作符
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

	if(*line == '\'')   //注释
	{
		strcpy(head,line);
		*line = '\0';
		return H_annotation;
	}
	 i = 0;
	if(checkBitmap(bitmapNum,*line))      //检测数字
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
	if(*line == '\"')   //检测字符串
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

		return H_string;  //head为字符串
		
	}
	i = 0;
	int j;
	while(*line!='\0' && ! (j = checkBitmap(bitmap,*line)) && *line != '.' && *line != '\"')
	{
		head[i++] = *line;
		++ line;
	}
	head[i] = '\0';

	if(i == 0)  //line处理完毕
		return H_finish;
	else
		return H_identify;     //head为标识符
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
