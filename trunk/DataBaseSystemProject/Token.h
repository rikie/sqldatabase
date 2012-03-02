#ifndef _TOKEN_H
#define _TOKEN_H
#include<string>
using namespace std;

class Token
{
public:
	int Type;
	char Value[80];
	Token();
	Token(int type, char value[]);
};

#endif