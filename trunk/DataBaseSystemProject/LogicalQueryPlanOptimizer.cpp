#include"LogicalQueryPlanOptimizer.h"

LogicalQueryPlan* LogicalQueryPlanOptimizer::OptimizeLogicalQueryPlan(LogicalQueryPlan*& lqp)
{
	return lqp->GetOptimizedQueryPlan();
}