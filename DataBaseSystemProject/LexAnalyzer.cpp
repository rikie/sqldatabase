#include"lexAnalyzer.h"
Keywords LexAnalyzer::keyword;

//Just create an instance of Lexical Analyser
LexAnalyzer::LexAnalyzer()
{
	cout<<"Lexical Analyser Initialized"<<endl;
}

//Initialize Lexical Analyser with the text which needs to be parsed.
LexAnalyzer::LexAnalyzer(char* text)
{
	strcpy(textToParse, text);
	curr_char = textToParse;
	prev_char = curr_char;
	//cout<<"Lexical Analyser Initialised"<<endl;
}

char LexAnalyzer::GetNextCharacter()
{
	return *curr_char;
}

Token LexAnalyzer::GetNextToken()
{
	prev_char = curr_char; //previous char will have previous token start point, in case of look back
	Token token;
	//skip white space
	while(*curr_char == '\n' || *curr_char == ' ')
		*curr_char++;

	if(*curr_char == ',')
	{
		token = Token(COMMA,",");
		curr_char ++;
	}
	else if(*curr_char == '"')
	{
		int i = 0;
		curr_char++;
		while(*curr_char!='"')
		{
			token.Value[i] = *curr_char;
			i++; curr_char++;
		}
		token.Value[i] = '\0';
		//here curr_char points to '"' - Ignore this
		//check whether just after '"' there should be space
		curr_char++;
		if(*curr_char != ' ' && *curr_char != '\n' && *curr_char!=',' && *curr_char!= ')' && *curr_char!='\0')
			throw string("Incorrect Literal");
		token.Type = LITERAL;
	}
	else if(*curr_char == '/')
	{
		token = Token(DIVSIGN,"/");
		curr_char ++;
	}
	else if(*curr_char == '.')
	{
		token = Token(DOT,".");
		curr_char ++;
	}
	else if(*curr_char == ')')
	{
		token = Token(EPAREN,")");
		curr_char ++;
	}
	else if(*curr_char == ']')
	{
		token = Token(ESQPAREN,"]");
		curr_char ++;
	}
	else if(*curr_char == '='|| *curr_char == '<'|| *curr_char == '>')
	{
		token.Value[0] = *curr_char;
		token.Value[1] = '\0';
		token.Type = COMPOP;
		curr_char ++;
	}
	else if(*curr_char == '(')
	{
		token = Token(OPAREN,"(");
		curr_char ++;
	}
	else if(*curr_char == '[')
	{
		token = Token(OSQPAREN,"[");
		curr_char ++;
	}
	else if(*curr_char == '-')
	{
		token = Token(MINUSSIGN,"-");
		curr_char ++;
	}
	else if(*curr_char == '+')
	{
		token = Token(PLUSSIGN,"+");
		curr_char ++;
	}
	else if(*curr_char == ';')
	{
		token = Token(SEMICOLON,";");
		curr_char ++;
	}
	else if(*curr_char == '*')
	{
		token = Token(STARSIGN,"*");
		curr_char ++;
	}

	else if(isalnum(*curr_char))
	{
		int i=0;
		bool all_digit = true;
		bool not_an_identifier = false;
		if(isdigit(*curr_char))
			not_an_identifier = true; //if first digit is a number then it can not be an identifier, it has to ba a number
		while(isalnum(*curr_char))
		{
			if(isalpha(*curr_char))
				all_digit = false;
			token.Value[i] = *curr_char;
			curr_char++; i++;
		}
		token.Value[i] = '\0';
		int type;
		if(all_digit)
			token.Type = INTEGER;
		else if((type= keyword.KeywordToken(token.Value))!=0)
		{
			token.Type = type;
		}
		else if(type == 0)
		{
			if(!not_an_identifier)
				token.Type = IDENTIFIER;
			else
			{
				token.Type = UNDEFTOK;
				strcat(token.Value, " - Wrong Token, ERROR!!!");
			}
		}
	}
	else if(*curr_char == '\0')
	{ 
		//already defined as #define EOF(-1)
		token = Token(EOF, "End Of File");
	}
	else if(*curr_char!='\n' && *curr_char!=' ') //white spaces are valid but naything else are invalid
	{ 
		token.Type = UNDEFTOK;
		token.Value[0] = *curr_char;
		token.Value[1] = '\0';
		strcat(token.Value, " - Undef Token, ERROR!!!");
		curr_char++;
	}

	return token;
}

void LexAnalyzer::RestorePreviousToken()
{
	curr_char = prev_char;
}