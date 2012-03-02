#include <fstream>
#include "FileManager.h"


char* FileManager::GetFileText(char* filename)
	{
		ifstream in(filename); // input
		if(!in) 
		{
			cout << "Cannot open file -" << filename<<endl;
			return NULL;
		}			
		char line[255];
		char fullText[500000]="";
		while(in.getline(line,255))
		{
			//in.getline(line, 255); // delim defaults to '\n'
			//cout << line << endl;
			strcat(fullText,line);
			strcat(fullText,"\n");
		}
		return fullText;
	}


string FileManager::GetFileTextString(char* filename)
	{
		ifstream in(filename); // input
		if(!in) 
		{
			cout << "Cannot open file -" << filename<<endl;
			string s = "";
			return s;
		}			
		char line[255];
		string fullTextString ="";
		while(in.getline(line,255))
		{
			//in.getline(line, 255); // delim defaults to '\n'
			//cout << line << endl;
			fullTextString.append(line);
			fullTextString.append("\n");
		}
		return fullTextString;
	}