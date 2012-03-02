#ifndef _UTILITY_H
#define _UTILITY_H
#include<iostream>
#include<string>

using namespace std;

class Utility
{
	static int i ;
public:
	static string RandomTableName()
	{
		i = i+1;
		
		char buff[10];
		char* si = itoa(i, buff, 10);
		string str(si);
		str.append("TbleName");
		return str;
	}
};

#endif