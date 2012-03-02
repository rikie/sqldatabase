#ifndef _SQLWORKER_H
#define _SQLWORKER_H
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include<iterator>
#include <iostream>
#include "Block.h"
#include "Config.h"
#include "Disk.h"
#include "Field.h"
#include "MainMemory.h"
#include "Relation.h"
#include "Schema.h"
#include "SchemaManager.h"
#include "Tuple.h"
#include "LogicalQueryPlan.h"

using namespace std;

//External global variables, define in Global.h and initialized when execution starts in DBManagerStart.cpp
extern MainMemory mem;
extern Disk disk;
extern SchemaManager schema_manager;

//SqlWorker directly works with memory

class SqlWorker
{
private:
	bool InitializeTuple(Tuple& tuple, vector<string> attributeList, vector<string> valueList);
	bool AppendTupleToRelation(Relation* relation_ptr, int memory_block_index, Tuple& tuple);

public:
	SqlWorker();
	//Returns true if successfully created else returns false
	bool CreateTable(string tableName, vector<string> attributeNameList, vector<enum FIELD_TYPE> dataTypeList);

	//Returns true if successfully inserted tuple in relation <tableName> else returns false
	bool InsertTuple(string tableName, vector<string> attributeList, vector<string> valueList);

	//Execute a logical plan and print the result
	bool ExcecuteLogicalPlan(LogicalQueryPlan*& lp);

	//Drop a table with name - tableName
	bool DropTable(string tableName);

	//DELETE from a table where some condition is true
	bool DeleteTable(string tableName, SearchConditionNode*& selOpn);

};

#endif