#include "compile.h"

//关键字表
tableEntry keyword[35] = {
	"and",1, "array",2, "begin",3, "bool",4, "call",5,
	"case",6, "char",7, "constant",8, "dim",9, "do",10,
	"else",11, "end",12, "false",13, "for",14, "if",15,
	"input",16, "integer",17, "not",18, "of",19, "or",20,
	"output",21, "procedure",22, "program",23, "read",24, "real",25,
	"repeat",26, "set",27, "stop",28, "then",29, "to",30,
	"true",31, "until",32, "var",33, "while",34, "write",35
};
//判断是否为字母
bool is_alphabet(char ch)
{
	if((ch>=65 && ch<=90) || (ch>=97 && ch<=122))
		return true;
	else
		return false;
}
//判断是否为数字
bool is_digit(char ch)
{
	if(ch>=48 && ch<=57)
		return true;
	else
		return false;
}
//判断是否为单界符
bool is_character(char ch)
{
	switch(ch){
	case '+': return true;
	case '-': return true;
    case '*': return true;
	case '/': return true;
	case '=': return true;
	case '<': return true;
	case '>': return true;
	case '(': return true;
	case ')': return true;
	case '[': return true;
	case ']': return true;
	case ':': return true;
	case '.': return true;
	case ';': return true;
	case ',': return true;
	default: return false;
	}
}

//词法分析程序
void scanner(string buff, char*&token,int&next, int&code)
{
	int index = 0;
	char ch;
	ch = buff[next++]; 
	if(ch == '\0')
	{
		code =0;
		return;
	}
	while(ch == ' '||ch == '\t')
	{
		ch = buff[next++]; //读取下一个字符
	}
	if(is_alphabet(ch))
	{ //判断字母字符
		while(is_alphabet(ch) || is_digit(ch)) 
		{ //为字母字符或数字字符
			token[index++] = ch;
			ch = buff[next++]; //读取下一个字符
		}
		token[index++] = '\0';
		next = next-1;
		code =36;  //标识符
		for(int i=0;i<35;i++)
		{
			if(strcmp(token,keyword[i].word) == 0)
			{
				code = keyword[i].classcode; //关键字
				break;
			}
		}
	}
	else if(is_digit(ch))
	{ //判断数字字符
		while(is_digit(ch))
		{
			token[index++] =ch;
		    ch = buff[next++]; //读取下一个字符
			code =37; //整数
		}
		while(ch != ' ' && ch !='\0')
		{
			if(is_alphabet(ch))
			{
				token[index++] =ch;     
			}
			else if(is_character(ch))
			{
				break;
			}
			ch =buff[next++]; //读取下一个字符
			if(ch == ' '|| ch =='\0')
			{
				token[index++] ='\0';
				code = -1; //不合法的标识符
				break;
			}
		}
		token[index++] ='\0';
		next = next-1;
	}
	else if(ch == 39)
	{ //判断字符常数
		token[index++] = ch;
		ch = buff[next++];
		while(is_alphabet(ch) || is_digit(ch) || is_character(ch))
		{
			token[index++] = ch;
			ch = buff[next++];
		}
		if(ch == '\0')
		{
			code = -1;
			token[index++]  = '\0';
			next = next - 1; //回退一个字符
		}
		if(ch ==39 || ch ==' ')
		{
			if(ch == 39)
			{
				token[index++] =ch;
				token[index++] ='\0';
				code =38; //字符常数
			}
			else 
			{
				while(ch != '\0')
				{ //扫描一行字符串换行前是否有右单引号
					token[index++] = ch;
					ch =buff[next++];
					if(ch ==39)
					{
						token[index++] =ch;
						token[index++] ='\0';
						code =38; //字符常数
						break;
					}
					if(ch == '\0')
					{
						code = -1; //不合法的字符常数
						token[index++] ='\0'; //index指针回溯到断点的位置
						next = next - 1; //next指针回溯到断点的位置
						break;
					}
				}
			}
		}
	}
	else 
	{
		switch(ch)
		{
		case '(': {
			token[index++]=ch; token[index++]='\0'; code=39;
			break;
				  }
		case ')': {
			token[index++]=ch; token[index++]='\0'; code=40;
			break;
				  }
		case '*': {
			token[index++]=ch; 
			ch=buff[next++];
			if(ch =='/')
			{
				token[index++] =ch;
			    code =42;
			}
			else
			{
				code =41;
			    next =next-1; //回退一个字符
			}
			token[index++]='\0';
			break;
				  }
		case '+': {
			token[index++]=ch; token[index++]='\0'; code=43;
			break;
				  }
		case ',': {
			token[index++]=ch; token[index++]='\0'; code=44;
			break;
				  }
		case '-': {
			token[index++]=ch; token[index++]='\0'; code=45;
			break;
				  }
		case '.': {
			token[index++]=ch; 
			ch=buff[next++];
		    if(ch =='.')
			{
				token[index++] =ch;
			    code =47;
			}
			else
			{
				code =46;
			    next =next-1; //回退一个字符
		     }
			token[index++]='\0';
			break;
				  }
		case '/':{
			token[index++]=ch; 
			ch=buff[next++];
			if(ch =='*')
			{
				token[index++] =ch;
				int temp_index = index; //记录断点
				int temp_next = next; //记录断点
				ch = buff[next++];
				token[index++] = ch;
				while(ch !='\0')
				{ //扫描一行字符串换行前是否有 */号
					if(ch == '*')
					{ 
			            ch=buff[next++];
						token[index++] = ch;
			            if(ch =='/')
			            {
							code =49;
							index = temp_index; //回到断点处
							token[index++]='\0';
							next = next -2;
							break;
			            }
			            else
			            {
							code = -1;
							token[index++]='\0';
							next = next - 1;
							break;
			            }
					}
					ch = buff[next++];
					token[index++] = ch;
				}
				if( ch == '\0')
				{
					code = -1;
					next = next -1; //回退一个字符
				}
			}
			else
			{
				code =48;
			    next =next-1; //回退一个字符
				token[index++]='\0';
			}
			break;
				 }
		case ':':{
			token[index++]=ch; 
			ch=buff[next++];
		    if(ch =='=')
			{
				token[index++] =ch;
			    code =51;
			}
			else
			{
				code =50;
			    next =next-1; //回退一个字符
			}
			token[index++]='\0';
		    break;
				 }
	case ';': {
		token[index++]=ch; token[index++]='\0'; code=52;
		break;
			  }
	case '<': {
		token[index++]=ch; 
		ch=buff[next++];
		if(ch =='=')
		{
			token[index++] =ch;
			code =54;
		}
		else if(ch =='>')
		{
			token[index++] =ch;
			code =55;
		}
		else
		{
			code =53;
			next =next-1; //回退一个字符
		}
		token[index++]='\0';
		break;
	}
	case '=': {
		token[index++]=ch; token[index++]='\0'; code=56;
		break;
			  }
	case '>': {
		token[index++]=ch; 
		ch=buff[next++];
		if(ch =='=')
		{
			token[index++] =ch;
			code =58;
		}
		else
		{
			code =57;
			next =next-1; //回退一个字符
		}
		token[index++]='\0';
		break;
	}
	case '[': {
		token[index++]=ch; token[index++]='\0'; code=59;
		break;
			  }
	case ']': {
		token[index++]=ch; token[index++]='\0'; code=60;
		break;
			  }
	default: 
		token[index++] =ch;
		token[index++] ='\0';
		code =-1;
		break;
   }
   }
}
