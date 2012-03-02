#ifndef _FILE_MANAGER_H
#define _FILE_MANAGER_H

#include<iostream>
#include<string>
#include<fstream>
using namespace std;

class FileManager
{
private:
	char filename[100];

public:
	char* GetFileText(char* filename);
	string GetFileTextString(char* filename); //Works on string - No limit on file size
};

#endif