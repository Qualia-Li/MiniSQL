#include "stdafx.h"
#ifndef __commandcut__h__
#define __commandcut__h__

 

class Command
{
public:
	string  str[200];
	int n;
public:
	Command() : n(0){}

	bool getOneWord(string & src, string & des)
	{
		des.clear(); 
		unsigned int srcpos = 0, despos = 0;
		char temp = ' ';

		for (; srcpos<src.length(); srcpos++) {
			if (temp == ' ' || temp == '\t' || temp == 10 || temp == 13)
				temp = src[srcpos];
			else break;  
		}
		if (srcpos == src.length() && (temp == ' ' || temp == '\t' || temp == 10 || temp == 13))
			return false; 

		switch (temp)
		{
		case ',':
		case '(':
		case ')':
		case '*':
		case '=':
		case '\'':
			des += temp;
			src.erase(0, srcpos);
			break;
		case '<':
			des += temp;
			temp = src[srcpos++];
			if (temp == '=' || temp == '>')
			{
				des += temp;
				src.erase(0, srcpos); 
			}
			else
			{
				src.erase(0, srcpos - 1);
			}
			break;
		case '>':
			des += temp;
			temp = src[srcpos++];
			if (temp == '=')
			{
				des += temp;
				src.erase(0, srcpos);
			}
			else
			{
				src.erase(0, srcpos - 1);
			}
			break;
		default:
			do{
				des += temp;
				if (srcpos < src.length())
					temp = src[srcpos++];
				else  {
					src.erase(0, srcpos);
					des[despos++] = '\0';
					return true;
				}
			} while (temp != '*' && temp != ',' && temp != '(' && temp != ')'
				&& temp != ' ' && temp != '\t' && temp != '=' && temp != '>'
				&& temp != '<' && temp != '\'' && temp != 10 && temp != 13);
			src.erase(0, srcpos - 1);
		}
		return true;
	}
	void strcut(string & src)
	{
		string des;
		while (getOneWord(src, des))
		{
			str[n] = des;
			n++;
		}
	}

};

#endif