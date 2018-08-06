#ifndef COMPILE_H
#define COMPILE_H

#include<iostream>
#include<cstring>
#include<string>
using namespace std;
//�ؼ���
struct tableEntry
{
	char word[10];
	int classcode;
};
//����
struct Token
{
	int classcode;
	string tok;
	Token *next;
};
//��Ԫʽ
struct fourTuple
{
	string op;
	string arg1, arg2;
	string result;
	int stat;
	fourTuple *next;
};
//����
struct Data
{
	int typeIndex;
	string dataValue;
};
//������
struct Chain
{
	fourTuple *locate;
	Chain *next;
};

bool is_alphabet(char ch);
bool is_digit(char ch);
bool is_character(char ch);
void scanner(string buff, char*&token,int&next,int&code);
void serialToken(char *filename, Token *& head);
void emit(string op,string arg1,string arg2, string result);
bool Program(Token *&head);
bool Var(Token *&head);
void If(Token *&head,int cstat);
void Repeat(Token *&head,int cstat);
void Sentence(Token *&head);
bool isExist(string keyName);
bool Calculate(Token *&head, string left);
void Statement(Token *&head);
void back(Chain *&c, int stat);
void While(Token *&head, int cstat);
void Complex(Token *&head);

#endif