#include<iostream>
#include<exception>
#include<conio.h>
#include"FileManager.h"
#include "Global.h"
#include "DBParser.h"
#include "FromQueryPlan.h"

using namespace std;

FileManager fileManager;
DBParser dbParser;

int main()
{
	char* inputText = NULL;
	char line[300];
	string command;
	for(;;)
	{
		cout<<"\nMODE:> "; cin>>command;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		if(command == "f")
		{
			inputText = fileManager.GetFileText("QUERY");
			if(inputText == NULL)
			{
				cout<<"Press Any Key to exit"<<endl;
				getch();
				exit(1);
			}
			cout<<"-----------File Content: "<<"-----------"<<endl<<inputText<<endl;

			cout<<"--------------------------"<<endl;
		}
		else if(command == "e")
		{
			exit(EXIT_SUCCESS);
		}
		else
		{
			cout<<"TSQL:> "; cin.getline(line, 255);
			//cin.ignore(numeric_limits<streamsize>::max(), '\n');
			inputText = line;
		}

		try
		{
			dbParser = DBParser(inputText);
			dbParser.ParseStatement();
			dbParser.ClearGlobalVariables();
		}
		catch(string str)
		{
			cout<<"ERROR!!! - "<<str<<endl;
			dbParser.ClearGlobalVariables();
		}
		catch(...)
		{
			cout<<"ERRROR!!! - Unhandled Exception Occurs"<<endl;
			dbParser.ClearGlobalVariables();
		}
	}
	//cout<<"Memory Contains"<<endl;
	//cout<<mem<<endl;
	//cout<<"Relation ABC Contains"<<endl;
	//cout<<*schema_manager.getRelation("ABC")<<endl<<endl;

	//cout<<"Relation Test Contains"<<endl;
	//cout<<*schema_manager.getRelation("TEST")<<endl<<endl;
	//Testing From Query Plan"
	getch();
}

////
//////#include<iostream>
//////#include<exception>
//////#include<conio.h>
//////#include"FileManager.h"
//////#include"LexAnalyzer.h"
//////
//////using namespace std;
//////int main()
//////{
//////	LexAnalyzer lexAn;
//////	FileManager fm;
//////	char* fileText = fm.GetFileText("QUERY");
//////	if(fileText == NULL)
//////	{
//////		cout<<"Press Any Key to exit"<<endl;
//////		getch();
//////		exit(1);
//////	}
//////	cout<<fileText<<endl;
//////	lexAn = LexAnalyzer(fileText);
//////	Token token;
//////	while(token.Type != EOF)
//////	{
//////		token = lexAn.GetNextToken();
//////		cout<<token.Type<<"    "<<token.Value<<endl;
//////	}
//////
//////	getch();
//////}

