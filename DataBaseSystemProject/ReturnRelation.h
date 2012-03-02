#ifndef _RETURN_RELATION
#define _RETURN_RELATION
#include<vector>
#include<string>
#include"Tuple.h"
using namespace std;

struct ReturnRelation
{
	string relationName;
	bool markedForDeletion;
	vector<Tuple> tuples;
};

#endif