#include "FromQueryPlan.h"	
#include "ReturnRelation.h"
//External global variables, define in Global.h and initialized when execution starts in DBManagerStart.cpp
extern MainMemory mem;
extern Disk disk;
extern SchemaManager schema_manager;
int FromQueryPlan::persistToDB = 0;

ReturnRelation ExecuteProductOperation(ReturnRelation s, ReturnRelation r);
FromQueryPlan::FromQueryPlan(ReturnRelation retRelation)
{
	returnRelation = retRelation;
	type = LEAF;
	leftFromQueryPlan = NULL;
	rightFromQueryPlan = NULL;
}
FromQueryPlan::FromQueryPlan(FromQueryPlan*& left, FromQueryPlan*& right)
{
	leftFromQueryPlan = left;
	rightFromQueryPlan = right;
	type = NON_LEAF;
}
ReturnRelation 
	 FromQueryPlan::Evaluate()
{
	ReturnRelation retValue;
	switch(type)
	{
	case NON_LEAF:
		bool persist;
		persist = false;
		retValue = ExecuteProductOperation(leftFromQueryPlan->Evaluate(),
										rightFromQueryPlan->Evaluate());
		returnRelation = retValue;
		persistToDB--;
		break;
	case LEAF:
		retValue = returnRelation;
		break;
	}
	return retValue;
}
	
void FromQueryPlan::operator delete(void *p)
{
}

ReturnRelation ExecuteProductOperation(ReturnRelation s, ReturnRelation r)
{
    bool persist = false;
	if(FromQueryPlan::GetNoOfPersistentCount()>0)
		persist = true;
	ReturnRelation retRelation;
	string tempTable = Utility::RandomTableName();
	vector<Tuple> tuples = TableScan::FetchProduct(s.relationName, r.relationName, tempTable, persist);
	retRelation.relationName = tempTable;
	retRelation.markedForDeletion = true;
	retRelation.tuples = tuples;
	return retRelation;
}

FromQueryPlan* fromQueryPlan = NULL;
FromQueryPlan* childPlan = NULL;
vector<string> globalTabList;

void ParseTableVal()
{
	string tab = globalTabList.back();
	//cout<<"Parsing table: "<<tab<<endl;
	ReturnRelation ret;
	ret.markedForDeletion = false;
	ret.relationName = tab;
	childPlan = new FromQueryPlan(ret);
	globalTabList.pop_back();
}
void ParseFromList()
{
	ParseTableVal();
	fromQueryPlan = childPlan;
	if(globalTabList.size() != 0)
	{
		FromQueryPlan* left = fromQueryPlan;
		ParseFromList();
		FromQueryPlan* right = fromQueryPlan;
		fromQueryPlan = new FromQueryPlan(left, right);
	}
}
FromQueryPlan* FromQueryPlan::CreateOptimizedFromQueryPlan(vector<string> frmList)
{
	FromQueryPlan* toReturn;
	vector<string> fromList = frmList;
	sort(fromList.begin(), fromList.end(), CompareTableSize);
	globalTabList = fromList;
	ParseFromList();
	toReturn = fromQueryPlan;
	fromQueryPlan = NULL;
	globalTabList.clear();
	return toReturn;
}

void FromQueryPlan::SetNoOfPersistentCount(int count)
{
	persistToDB = count;
}

int FromQueryPlan::GetNoOfPersistentCount()
{
	return persistToDB;
}