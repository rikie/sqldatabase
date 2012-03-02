#ifndef _DBPARSER_H
#define _DBPARSER_H
#include<iostream>
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "Block.h"
#include "Config.h"
#include "Disk.h"
#include "Field.h"
#include "MainMemory.h"
#include "Relation.h"
#include "Schema.h"
#include "SchemaManager.h"
#include "Tuple.h"

#include "LexAnalyzer.h"
#include "SqlWorker.h"
#include "LogicalQueryPlan.h"
#include "Operation.h"
#include "LogicalQueryPlanGenerator.h"
#include "LogicalQueryPlanOptimizer.h"
#include "configDB.h"

//Starts the table name with an integer if not exists, because there can't be any table in system with name 0NA

class DBParser
{
private:
	LexAnalyzer lexAnalyzer;
	SqlWorker sqlWorker;
	void ParseCreateTableStatement();
	string ParseTableName();
	void ParseAttributeTypeList();
	void ParseAttributeName();
	void ParseDataType();
	void ParseDropTableStatement();
	void ParseInsertStatement();
	void ParseInsertTuples();
	void ParseAttributeList();
	void ParseValueList();
	void ParseValue();
	void ParseSelectStatement();
	void ParseSelectList();
	void ParseSelectSublist();
	void ParseTableList();
	void ParseDeleteStatement();
	void ParseSearchCondition();
	void ParseBooleanTerm();
	void ParseBooleanFactor();
	void ParseBooleanPrimary();
	void ParseComparisionPredicate();
	void ParseExpression();
	void ParseCompOp();
	void ParseTerm();
	void ParseFactor();
	void ParseColumnName();


	void PrintDiskIOAndTimer();
	void ResetDiskIOAndTimer();
public:
	DBParser();
	void ClearGlobalVariables();
	DBParser(char *text);
	DBParser(string text);
	void ParseStatement();
	void ShowMemoryDump();
};

typedef LogicalQueryPlanGenerator LQPGenerator;
typedef LogicalQueryPlanOptimizer LQPOptimizer;

#endif