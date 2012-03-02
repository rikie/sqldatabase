#ifndef _LOGICAL_QUERY_PLAN_GENERATOR_H
#define _LOGICAL_QUERY_PLAN_GENERATOR_H
#include<cstdlib>
#include "LogicalQueryPlan.h"
#include "SearchCondition.h"
#include "Operation.h"


class LogicalQueryPlanGenerator
{
public:
	static LogicalQueryPlan* CreateLogicalQueryPlan(vector<Column> projectionList, vector<string> fromList, SearchConditionNode*& searchCondition);
	static LogicalQueryPlan* CreateOptimizedLogicalQueryPlan(bool isDistinct, vector<Column> projectionList, vector<string> fromList, SearchConditionNode*& searchCondition, vector<Column> orderByList);
	static LogicalQueryPlan* CreateLogicalQueryPlan(vector<Column> projectionList, vector<string> fromList);
};
#endif