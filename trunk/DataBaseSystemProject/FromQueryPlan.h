#ifndef _FROM_QUERY_PLAN_H
#define _FROM_QUERY_PLAN_H
#include<algorithm>
#include "Block.h"
#include "Config.h"
#include "Disk.h"
#include "Field.h"
#include "MainMemory.h"
#include "Relation.h"
#include "Schema.h"
#include "SchemaManager.h"
#include "Tuple.h"
#include "ReturnRelation.h"
#include"Operation.h"
#include"PhysicalOperator.h"
#include"Untility.h"
#include "PhysicalOperator.h"
#include"PUtility.h"

enum FRM_QUERY_IDENTIFICATION {LEAF = 1, NON_LEAF};
class FromQueryPlan
{
private:
	ReturnRelation returnRelation;
	FRM_QUERY_IDENTIFICATION type;
	FromQueryPlan* leftFromQueryPlan;
	FromQueryPlan* rightFromQueryPlan;
	static int persistToDB;
public:
	FromQueryPlan(ReturnRelation retRelation);
	FromQueryPlan(FromQueryPlan*& left, FromQueryPlan*& right);
	ReturnRelation Evaluate();
	static FromQueryPlan* CreateOptimizedFromQueryPlan(vector<string> fromList);
	void operator delete(void *p);
	static void SetNoOfPersistentCount(int count);
    static int GetNoOfPersistentCount();
};


#endif