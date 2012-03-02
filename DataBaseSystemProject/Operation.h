#ifndef _OPERATION_H
#define _OPERATION_H
#include<vector>
#include<cstdlib>
#include "SearchCondition.h"
#include "configDB.h"


using namespace std;
struct Column
{
	string tableName;
	string columnName;
};

class ProductOperation
{
private:
	vector<string> fromList; //table names should be pushed here
public:
	ProductOperation();
	ProductOperation(vector<string> frmList);
	void AddTableNameInFromList(string tbleName);
	vector<string> GetFromList();
};
class ProjectOperation
{
private:
	vector<Column> columnList;
public:
	ProjectOperation();
	ProjectOperation(vector<Column> colList);
	void AddColumnToList(Column col);
	vector<Column> GetColumnList();
	vector<string> GetColumnListString();
	vector<string> GetColumnListStringWithTable();
};
class SelectOperation
{
private:
	SearchConditionNode* searchCondition;
public:	
	SelectOperation();
	SelectOperation(SearchConditionNode*& searchCond);
	bool SearchConditionEvaluate();
	bool IsSearchCondition();
	void operator delete(void *p);
};


class OrderByOperation
{
private:
	vector<Column> columnList;
public:
	OrderByOperation();
	OrderByOperation(vector<Column> colList);
	void AddColumnToList(Column col);
	vector<Column> GetColumnList();
};

#endif