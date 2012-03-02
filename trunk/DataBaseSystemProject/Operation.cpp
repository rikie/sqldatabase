#include "Operation.h"
#include "PhysicalOperator.h"

SelectOperation::SelectOperation()
{
	searchCondition = NULL;
}


SelectOperation::SelectOperation(SearchConditionNode*& sc)
{
	searchCondition = sc;
}

bool SelectOperation::IsSearchCondition()
{
	if(searchCondition == NULL)
		return false;
	return true;
}

bool SelectOperation::SearchConditionEvaluate()
{
	return searchCondition->Evaluate();
}


void SelectOperation::operator delete(void *p)
{
	if(p!=NULL)
	{
	if(((SelectOperation*)p)->searchCondition != NULL)
		delete ((SelectOperation*)p)->searchCondition;
	free(p);
	}
}

ProductOperation::ProductOperation(){}
ProductOperation::ProductOperation(vector<string> frmList)
{
	fromList = frmList;
}

vector<string> ProductOperation::GetFromList()
{
	return fromList;
}
void ProductOperation::AddTableNameInFromList(string tbleName)
{
	fromList.push_back(tbleName);
}


ProjectOperation::ProjectOperation(){}
ProjectOperation::ProjectOperation(vector<Column> colList)
{
	columnList = colList;
}
void ProjectOperation::AddColumnToList(Column col)
{
	columnList.push_back(col);
}

vector<Column> ProjectOperation::GetColumnList()
{
	return columnList;
}
vector<string> ProjectOperation::GetColumnListString()
{
	vector<string> columnListstr;
	vector<Column>::iterator iter;
	for(iter = columnList.begin(); iter!=columnList.end(); iter++)
		columnListstr.push_back((*iter).columnName);
	return columnListstr;
}

vector<string> ProjectOperation::GetColumnListStringWithTable()
{
	vector<string> columnListstr;
	vector<Column>::iterator iter;
	for(iter = columnList.begin(); iter!=columnList.end(); iter++)
	{
		if((*iter).tableName == NO_TABLE)
		{
			string tabName = TableScan::GetTableName((*iter).columnName);
			columnListstr.push_back(string(tabName).append(".").append(string((*iter).columnName)));
		}
		else
			columnListstr.push_back(string((*iter).tableName).append(".").append((*iter).columnName));
	}
	return columnListstr;
}


OrderByOperation::OrderByOperation(){}
OrderByOperation::OrderByOperation(vector<Column> colList)
{
	columnList = colList;
}
void OrderByOperation::AddColumnToList(Column col)
{
	columnList.push_back(col);
}

vector<Column> OrderByOperation::GetColumnList()
{
	return columnList;
}