#include<fstream>
#include<stdlib.h>
#include<stack>
#include<map>
#include "compile.h"
using namespace std;
//数据类型
string Types[] = { "integer", "bool", "char" }; 
map<string, Data*> mp; //定义一个映射存储变量及其类型
stack<string> number; //数据栈
stack<string> symbol; //符号栈
Token *tokenHead = NULL; //单词序列串的头指针
fourTuple *current = NULL, *fourtuple = NULL;
int nextstat = 0;
int temp = 1;
//产生单词二元式序列
void serialToken(char *filename,Token *& head)
{
	ifstream infile;
	string buff;
	Token *s=NULL, *p=NULL;
	infile.open(filename); //打开文件
	if(!infile)
	{
		cerr<<"File open failed!";
		getchar();
	 }
	while(getline(infile,buff))
	{	
			int code; //用于显示单词种别码
			int next =0; //用于缓冲区字符计数
			do
			{			
			    s = new Token;
				char *tok =new char[50];
				scanner(buff,tok,next,code); //逐行扫描分析单词
				if(code == 0)
					break;
				if(code == -1)
				{
					tok = (char *)"Invalid character or identifier!";
				}
				s->classcode = code;
				s->tok = tok;
				if(head == NULL)
					head = s;
				else
					p->next = s;
				p = s;
				delete []tok;
				tok = NULL;
		    }while(code != 0);	
	}
	p->next = NULL;
	delete s;
	infile.close();  //关闭文件 
}
//分析程序头部
bool Program(Token *&head)
{
	if(head->classcode == 23)
	{
		head = head->next;
		Token *p = head;
		head = head->next;
		if(head->classcode == 52)
		{
			emit("program",p->tok,"-","-");  
		}
		else
		{
			return false;
		}
		return true;	
	}
	else 
		return false;
}
//分析变量定义
bool Var(Token *&head)
{
	string *tok = NULL;
	Data *val = NULL;
	stack<string*> stk;
	int typeIndex = 0;
	if(head->classcode == 33)
	{
		head = head->next;
		while(1)
		{
			if(head->classcode != 36)
			{
				head = head->next;
				return false;
			}
			while(head->classcode == 36)
			{
				tok = new string(head->tok);
				stk.push(tok);
				head = head->next;
				if(head->classcode == 44)
				{
					head = head->next;
					continue;
				}
				if(head->classcode ==50)
					break;
				head = head->next;
				return false;
			}
			head = head->next;
			switch(head->classcode)
			{
			case 17: typeIndex = 0; break; //integer类型
			case 4: typeIndex = 1; break; //bool类型
			case 7: typeIndex = 2; break; //char类型
			default: {
				head = head->next;
				return false;
					 }
				break;
			}
			while(!stk.empty())
			{
				tok = new string;
				tok = stk.top();
				stk.pop();
				val = new Data;
				val->typeIndex = typeIndex;
				val->dataValue = Types[typeIndex];
				mp.insert(make_pair(*tok,val));
			}
			head = head->next;
			if(head->classcode == 52)
			{
				head = head->next;
				if(head->classcode ==3)
					return true;
				else if(head->classcode ==36)
					continue;
			}
			else
			{
				head= head->next;
				return false;
			}
		}
	}
	else
	{
		head = head->next;
		return false;
	}
}
//分析if语句
void If(Token *&head,int cstat)
{
	head = head->next;
	Chain *tc=NULL, *fc=NULL, *oc=NULL;
	while(1)
	{
		if(head->classcode == 36 || head->classcode == 37)
		{
			string x, op, y;
			x = head->tok;
			head = head->next;
			op = head->tok;
			head = head->next;
			y = head->tok;
			emit("j"+op,x,y,"");

			tc = new Chain;
			tc->next = NULL;
			tc->locate = current;
			emit("j","-","-","");

			fc = new Chain;
			fc->next = NULL;
			fc->locate = current;
			head = head->next;
			continue;
		}
		//then语句
		else if(head->classcode == 29)
		{
			back(tc,nextstat);
			head = head->next;
			if(head->classcode == 36) //声明语句
			{
				Statement(head);
			}
			if(head->classcode == 34) //while语句
			{
				While(head,cstat);
			}
			if(head->classcode == 15) //if语句
			{
				If(head,cstat);
			}
			if(head->classcode == 26) //repeat语句
			{
				Repeat(head,cstat); 
			}
			emit("j","-","-","");
			oc = new Chain;
			oc->next = NULL;
			oc->locate = current;
			continue;
		}
		//else语句
		else if(head->classcode == 11)
		{
			back(fc,nextstat);
			head = head->next;
			if(head->classcode == 36) //声明语句
			{
				Statement(head);
			}
			if(head->classcode == 34) //while语句
			{
				While(head,cstat);
			}
			if(head->classcode == 15) //if语句
			{
				If(head,cstat);
			}
			if(head->classcode == 26) //repeat语句
			{
				Repeat(head,cstat); 
			}
			/*emit("j","-","-","");
			Chain *temp = new Chain;
			temp->next = NULL;
			temp->locate = current;
			oc->next = temp;*/
			break;
		}
		else
		{
			cout<<"'if' sentence error!"<<endl;
			return;
		}
	}
	if(cstat == 0)
		back(oc,nextstat);
	else
		back(oc,cstat);
	return;
}
//分析repeat语句
void Repeat(Token *&head,int cstat)
{
	int currentstat = nextstat;
	Chain *c = new Chain;
	c->next = NULL;
	head = head->next;
	if(head->classcode == 36)
	{
		Sentence(head);
	}
	else
	{
		cout<<"'repeat' sentence error!"<<endl;
		return;
	}
	//until语句
	if(head->classcode == 32)
	{
		string x, op, y;
		head = head->next;
		x = head->tok;
		head = head->next;
		op = head->tok;
		head = head->next;
		y = head->tok;
		emit("j"+op,x,y,"");

		c->locate = current;
		char t[32];
		sprintf(t,"%d",currentstat);
		string s(t);
		emit("j","-","-",s);

		if(cstat == 0)
			back(c,nextstat);
		else
			back(c,cstat);
		head = head->next;
	}
}
//分析句子
void Sentence(Token *&head)
{
	if(head->classcode == 3) //复合语句
	{
		Complex(head);
	}
	if(head->classcode == 36) //赋值语句
	{
		Statement(head);
	}
	if(head->classcode == 34) //while语句
	{
		While(head,0);
	}
	if(head->classcode == 15) //if语句
	{
		If(head,0);
	}
	if(head->classcode == 26) //repeat语句
	{
		Repeat(head,0);
	}
	else
		return;
}
//产生四元式
void emit(string op,string arg1,string arg2, string result)
{
	fourTuple *f = new fourTuple; 
	f->op = op;
	f->arg1 = arg1;
	f->arg2 = arg2;
	f->result = result;
	f->next = NULL;
	f->stat = nextstat;
	if(fourtuple == NULL)
	{
		fourtuple = f;
		current = fourtuple;
	}
	else
	{
		current->next = f;
		current = f;
	}
	nextstat ++;
}
bool isExist(string keyName)
{
	return (mp.find(keyName) != mp.end());
}
//分析算术表达式
bool Calculate(Token *&head, string left)
{
	while(head->classcode !=52)
	{
		if(head->classcode == 37)
		{
			number.push(head->tok);
		}
		else if(head->classcode == 36)
		{
			if(!isExist(head->tok)) //变量未定义
			{
				cout<<"'"<<head->tok<<"'"<<"is not defined!"<<endl;
				head = head->next;
				return 0;
			}
			number.push(head->tok);
		}
		else if(head->classcode ==43) //加法运算
		{
			symbol.push("+");
		}
		else if(head->classcode ==45) //减法运算
		{
			symbol.push("-");
		}
		else if(head->classcode == 41) //乘法运算
		{
			string s = number.top();
			number.pop();
			char tc[32];
			sprintf(tc,"%d",temp);
			string t ="T"+string(tc);
			temp ++;
			head = head->next;
			emit("*",s,head->tok,t);
			number.push(t);
		}
		else if(head->classcode == 48) //除法运算
		{
			string s = number.top();
			number.pop();
			char tc[32];
			sprintf(tc,"%d",temp);
			string t ="T"+string(tc);
			temp ++;
			head = head->next;
			emit("/",s,head->tok,t);
			number.push(t);
		}
		else
			break;
		head = head->next;
		if(head->classcode == 46)
		{
			cout<<"Variable type not match with defined!"<<endl;
			return false;
		}
		if(head->classcode == 52)
			break;
	}
	while(!symbol.empty())
	{
		string x = number.top(); 
		number.pop();
		string y = number.top();
		number.pop();
		string op = symbol.top();
		symbol.pop();
		char tc[32];
		sprintf(tc,"%d",temp);
		string t = "T"+string(tc);
		temp ++;
		switch(op[0])
		{
		case '+':{
			emit("+",x,y,t);
			number.push(t);
			break;
				 }
		case '-':{
			emit("-",x,y,t);
			number.push(t);
			break;
				 }
		default: ;
		}
	}
	//插入赋值语句四元式
	if(symbol.empty())
	{
		string s = number.top();
		number.pop();
		emit(":=",s,"-",left);
		if(head->classcode == 52)
			head = head->next;
		return 1;
	}
	return 0;
}
//分析赋值语句
void Statement(Token *&head)
{
	if(!isExist(head->tok))
	{
		cout<<"'"<<head->tok<<"'"<<"is not defined!"<<endl;
		head = head->next;
		return;
	}
	while(head->classcode == 36 && isExist(head->tok))
	{
		string left = head->tok;
		head = head->next;
		if(head->classcode ==51)
		{
			head = head->next;
			if(!Calculate(head,left))
			{
				cout<<"Evaluation sentence error!"<<endl;
				head = head->next;
			}
		}
		else
		{
			cout<<"Evaluation sentence error!"<<endl;
			return;
		}
	}
}
//拉链回填
void back(Chain *&c, int stat)
{
	char tc[32];
	sprintf(tc,"%d",stat);
	string s(tc);
	while(c!=NULL)
	{
		c->locate->result = s;
		c = c->next;
	}
}
//分析while语句
void While(Token *&head, int cstat)
{
	head = head->next;
	int currentstat = nextstat;
	Chain *tru=NULL, *tc=NULL, *fal=NULL, *fc=NULL;
	while(1)
	{
		if(head->classcode == 36 || head->classcode == 37)
		{
			string x, op, y;
			x = head->tok;
			head = head->next;
			op = head->tok;
			head = head->next;
			y = head->tok;
			emit("j"+op,x,y,"");
			if(tc == NULL)
			{
				tru = new Chain;
				tru->next = NULL;
				tc = tru;
				tc->locate = current;
			}
			else
			{
				Chain *temp = new Chain;
				temp->next = NULL;
				temp->locate = current;
				tc->next = temp;
				tc = temp;
			}
			emit("j","-","-","");
			if(fc ==NULL)
			{
				fal = new Chain;
				fal->next = NULL;
				fc = fal;
				fc->locate = current;
			}
			else
			{
				Chain *temp = new Chain;
				temp->next = NULL;
				temp->locate = current;
				fc->next = temp;
				fc = temp;
			}
			head = head->next;
			continue;
		}
		//判断条件有and
		if(head->classcode == 1)
		{
			back(tru,nextstat);
			tc = tru;
			head = head->next;
			string x, op, y;
			x = head->tok;
			head = head->next;
			op = head->tok;
			head = head->next;
			y = head->tok;
			emit("j"+op,x,y,"");
			if(tc == NULL)
			{
				tru = new Chain;
				tru->next = NULL;
				tc = tru;
				tc->locate = current;
			}
			else
			{
				Chain *temp=new Chain;
				temp->next=NULL;
			    temp->locate=current;
			    tc->next=temp;
			    tc=temp;
			}
			emit("j","-","-","");
			if(fc ==NULL)
			{
				fal = new Chain;
				fal->next = NULL;
				fc = fal;
				fc->locate = current;
			}
			else
			{
				Chain *temp = new Chain;
				temp->next = NULL;
				temp->locate = current;
				fc->next = temp;
				fc = temp;
			}
			head = head->next;
			continue;
		}
		//判断条件中有or
		if(head->classcode == 20)
		{
			back(fal,nextstat);
			fc = fal;
			head = head->next;
			string x, op, y;
			x = head->tok;
			head = head->next;
			op = head->tok;
			head = head->next;
			y = head->tok;
			emit("j"+op,x,y,"");
			if(tc->locate == NULL)
			{
				tc->locate = current;
			}
			else
			{
				Chain *temp=new Chain;
				temp->next=NULL;
			    temp->locate=current;
			    tc->next=temp;
			    tc=temp;
			}
			emit("j","-","-","");
			if(fc->locate == NULL)
			{
				fc->locate = current;
			}
			else
			{
				Chain *temp = new Chain;
				temp->next = NULL;
				temp->locate = current;
				fc->next = temp;
				fc = temp;
			}
			head = head->next;
			continue;
		}
		//do语句
		if(head->classcode == 10)
		{
			back(tru,nextstat);
			head = head->next;
			if(head->classcode == 36) //声明语句
			{
				Statement(head);
			}
			if(head->classcode == 34) //while语句
			{
				While(head,currentstat);
			}
			if(head->classcode == 15) //if语句
			{
				If(head,currentstat);
			}
			if(head->classcode == 26) //repeat语句
			{
				Repeat(head,currentstat);
			}
			char tc[32];
			sprintf(tc,"%d",currentstat);
			string s(tc);
			emit("j","-","-",s);
			if(cstat == 0)
				back(fal,nextstat);
			else
				back(fal,cstat);
			return;
		}
	}
}
//分析复合语句
void Complex(Token *&head)
{
	if(head->classcode == 3)
	{
		head = head->next;
		while(1)
		{
			Sentence(head);
			if(head->classcode == 12) //遇到end关键字
				break;
		}
		head = head->next;
		if(head->classcode == 46) //遇到.结束符
		{
			if(head->next != NULL)
				head =head->next;
			return;
		}
	}
	else
		cout<<"Definition of complex sentence error!"<<endl;
}
int main()
{
	while(1)
	{
		cout<<"Input test file: ";
	    char filename[10];
	    cin>>filename;
		tokenHead = NULL;
		current = NULL;
		fourtuple = NULL;

		//词法分析得到单词序列
		serialToken(filename,tokenHead);
		//分析程序头部
		if(!Program(tokenHead))
		{
			cout<<"Definition of program error!"<<endl;
		}
		tokenHead = tokenHead->next;
		//分析变量定义
		if(!Var(tokenHead))
		{
			cout<<"Definition of variable type error!"<<endl;
		}
		//复合语句分析
		Complex(tokenHead);
		emit("sys","-","-","-");
		//输出四元式
		while(fourtuple)
		{
			cout << "(" << fourtuple->stat << ")";
			cout<<"("<<fourtuple->op<<","<<fourtuple->arg1<<","<<fourtuple->arg2<<","<<fourtuple->result<<")"<<endl;
			fourtuple = fourtuple->next;
		}
		tokenHead = NULL;
		current = NULL;
		fourtuple = NULL;
		nextstat = 0;
		temp = 1;
	}
	return 0;
}
