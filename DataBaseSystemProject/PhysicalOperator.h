#ifndef _PHYSICAL_OPERATOR_H
#define _PHYSICAL_OPERATOR_H
#include <string>
#include<iostream>
#include<queue>
#include "Block.h"
#include "Config.h"
#include "Disk.h"
#include "Field.h"
#include "MainMemory.h"
#include "Relation.h"
#include "Tuple.h"
#include "Schema.h"
#include "SchemaManager.h"
#include "Operation.h"
#include "configDB.h"
#include "ReturnRelation.h"
#include "Untility.h"
#include "PUtility.h"

using namespace std;
//External global variables, define in Global.h and initialized when execution starts in DBManagerStart.cpp
extern MainMemory mem;
extern Disk disk;
extern SchemaManager schema_manager;
class TableScan
{
private:
	string relation;
	int startMemBlkIndex;
	int endMemBlkIndex;
	int noOfBlocksToBeReadInCurrentPass;
	int noOfBlocksInRelation;
	int noOfTuplesInRelation;
	int noOfBlocksRemainingToScan;
	Relation* relationPtr;
	int currRelBlockIndex;
	int currTupleIndex;
	int currMemIndex;
	vector<Tuple> currentTuples;
public:
	TableScan(string rel, int startMemIndex, int endMemIndex);
	// Single table scan
	// This function fetch all the tuples from a relation(disk block) to memory block and
	// then read those in a vector of tuples and return.
	static vector<Tuple> FetchTuples(string tableName, int memBlockIndex);
	static vector<Tuple> FetchTuples(bool distinct, string tableName, int memBlockIndex, vector<Column> ordBy);
	static vector<Tuple> FetchDistinctTuples(string tableName, int memBlockIndex);
	
	// Single table scan
	// This function fetch all the tuples from a relation(disk block) to memory block and
	// then read those in a vector of tuples and return only yhose which satifies condition.
	static vector<Tuple> FetchSelectedTuples(string tableName, int memBlockIndex, SelectOperation& selOp);
	static vector<Tuple> FetchSelectedTuples(bool isDistinct, string tableName, int memBlockIndex, SelectOperation& selOp, vector<Column> ordBy);

	//New tuple will be returned to t, returns false if no new tuple remains to scan.
	//Throws string exception in case of any scanning problem
	bool GetNextTuple(Tuple& t);

	// Single table scan
	// This function fetch all the tuples, in which each tuple will have specified columns mentioned
	// in ProjectOperation input parameter
	static vector<Tuple> FetchTuples(ProjectOperation projection, SelectOperation& selOpn, string tableName, int memBlockIndex, string temporaryRelation);

	static vector<Tuple> FetchTuples(bool isDistinct, ProjectOperation projection, SelectOperation& selOpn, string tableName, int memBlockIndex, string temporaryRelation, vector<Column> ordBy);

	static vector<Tuple> FetchProduct(string table1, string table2, string tempTable, bool persistToDB);
	
	static ReturnRelation ApplyProjectOperation(ProjectOperation projetOpn, ProductOperation prodOpn,
					ReturnRelation retRelation, bool persist);

	static string GetTableName(string columnName);
	static vector<Tuple> ApplySelectCondition(SelectOperation& selectOperation, ReturnRelation retRelation);
	static vector<Tuple> FetchTuplesCorrectMistake(bool isDistinct, ProjectOperation projection, SelectOperation& selOpn, string tableName, int memBlockIndex, string temporaryRelation, vector<Column> orderBy);
	static vector<Tuple> OrderRelation(vector<Tuple> t, string tableName, vector<Column> ordBy);
	static vector<Tuple> FetchTuplesDistinctOrdered(vector<Tuple> Tuples, bool isDistinct, ProjectOperation projection, string tableName, int memBlockIndex, vector<Column> OrderBy);
};

class SubListRelation
{
private:
	bool memQVecSet;
	int noOflastListBlocks;
	void PopAll(int i, Tuple t);
	void Pop(int i);
	bool FetchQueue(int i);
public:
	Relation* relationPtr;
	string relationName;
	int noOfBlocks;
	Schema relSchema;
	int noOfSublists;
	vector<string> columns;
	vector<int> sublitsIndex;
	vector<deque<Tuple>> memQueueVector;
	map<int, int> relativeIndexMapForBlock_i;
	SubListRelation(string relName, vector<string> columns);
	Tuple FetchDisctinctTuple();
	Tuple FetchOrderedTuple();
};

void PersistTuplesToDisk(Relation*& relationPtr, vector<Tuple> tuples, int memoryIndex);
#endif