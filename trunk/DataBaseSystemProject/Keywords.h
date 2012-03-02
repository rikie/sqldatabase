#ifndef _KEYWORDS_H
#define _KEYWORDS_H

#include<string>
#include"TokenType.h"
class Keywords
{

public:
	Keywords()
	{
		fillMap();
	}
	int KeywordToken(string value)
	{
		return keywordMap[value];
	}
private : 
	map<string, int> keywordMap;
	void fillMap()
	{
		keywordMap["AND"] = AND; keywordMap["CREATE"] = CREATE; keywordMap["DISTINCT"] = DISTINCT; 
		keywordMap["BY"] = BY; keywordMap["DELETE"] = DELETE; keywordMap["DROP"] = DROP; keywordMap["FROM"] = FROM; 
		keywordMap["INSERT"]=INSERT; keywordMap["INT"]=INT_T; keywordMap["INTO"]=INTO; keywordMap["NULL"] = NULL_T;
		keywordMap["NOT"]=NOT; keywordMap["OR"]=OR; keywordMap["ORDER"]=ORDER;
		keywordMap["SELECT"]=SELECT; keywordMap["STR20"]=STR20_T; keywordMap["TABLE"]=TABLE;
		keywordMap["VALUES"]=VALUES; keywordMap["WHERE"]=WHERE;

	}
};

#endif