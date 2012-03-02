#ifndef _LEX_ANALYZER_H
#define _LEX_ANALYZER_H

#include <fstream>
#include<iostream>
#include"Token.h"
#include"Keywords.h"
using namespace std;


class LexAnalyzer
{
private: 
	//Our lexAnalyzer could parse atmost 2000 character in a single query
	char textToParse[500000];

	//A pointer points to the current char to parse
	char* curr_char;
	char* prev_char;
	static Keywords keyword;

public:
	LexAnalyzer();
	LexAnalyzer(char* text);
	Token GetNextToken();
	char GetNextCharacter();
	void RestorePreviousToken();
};


#endif