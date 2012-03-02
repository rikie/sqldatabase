#ifndef _LOGICAL_QUERY_PLAN_OPTIMIZER_H
#define _LOGICAL_QUERY_PLAN_OPTIMIZER_H
#include<cstdlib>
#include "LogicalQueryPlan.h"

class LogicalQueryPlanOptimizer
{
public:
	static LogicalQueryPlan* OptimizeLogicalQueryPlan(LogicalQueryPlan*& lqp);
};

#endif