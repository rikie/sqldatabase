#include"Token.h"

Token::Token():Type(0)
{
	strcpy(Value,"");
}

Token::Token(int type, char value[])
{
	Type = type;
	strcpy(Value,value);
}