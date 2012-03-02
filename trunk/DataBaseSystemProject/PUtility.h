#ifndef P_UTILITY_H
#define P_UTILITY_H
#include<iostream>
#include<string>
#include <algorithm>
#include<deque>
#include<queue>
#include "Block.h"
#include "Config.h"
#include "Disk.h"
#include "Field.h"
#include "MainMemory.h"
#include "Tuple.h"
#include "Relation.h"
#include "Schema.h"
#include "SchemaManager.h"
#include "configDB.h"

using namespace std;

typedef pair<string, string> TableColumnPair;
typedef pair<FIELD_TYPE, Field> Value;

//compares tuple on the basis of each fields in the tuple
//used basically to sort a list(vector) of tuples
class CompareTuple
{
private:
	vector<string> fieldNames;
public:
	CompareTuple(vector<string> fn);
	bool operator()(Tuple& first, Tuple& second);
};

bool CompareTableSize(string rel1, string rel2);

//Main Memory Sorting
//In memory sorting operation based on tuples
bool MMSorting(int startMemIndex, int noOfblocks, vector<string> fieldNames);

//Compare two tuples
//Return true if equals else returns false
bool IsEqual(Tuple t1, Tuple t2);
bool IsEqual(Tuple t1, Tuple t2, vector<string> fnames);

//returns the first index of tuple to be searched for
//returns -1 if not found
int FindTuple(vector<Tuple> tuples, Tuple t);
int FindTuple(vector<Tuple> tuples, Tuple t, vector<string> fieldNames);


//Function to map the column values in a global space
void MapTupleValues(Tuple t);
void MapTupleValues(Tuple t, string tableName);
Tuple FindMinTuple(vector<Tuple> tuples, vector<string> fieldNames);

#endif