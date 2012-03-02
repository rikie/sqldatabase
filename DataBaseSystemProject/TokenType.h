#ifndef _TOKTYPES_H
#define _TOKTYPES_H
#include<string>
#include<map>
using namespace std;
enum:int
{
	AND = 1,
	BY,
	COMPOP, //=|<|>
	COMMA,//,
	CREATE,
	DELETE,
	DISTINCT,
	DIVSIGN,// /
	DOT,// .
	DROP,
	ESQPAREN, //]
	EPAREN,//)
	FROM,
	IDENTIFIER,
	INSERT,
	INT_T,
	INTO,
	LITERAL,
	MINUSSIGN,//-
	NOT,
	NULL_T,
	INTEGER,
	OSQPAREN, //[
	OPAREN,//(
	OR,
	ORDER,
	PLUSSIGN,//+
	SELECT,
	SEMICOLON,//;
	STR20_T,
	STARSIGN, //*
	TABLE,
	UNDEFTOK,
	VALUES,
	WHERE
};
#endif


