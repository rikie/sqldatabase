#include"LogicalQueryPlanGenerator.h"

LogicalQueryPlan* LogicalQueryPlanGenerator::
	CreateLogicalQueryPlan(vector<Column> projectionList, vector<string> fromList, SearchConditionNode*& searchCondition)
{

	//Ignore last parameter searchCondition for now, Just implementing a simple select operation for now

	ProductOperation productOperation(fromList); //Create a product operation
	LogicalQueryPlan *lpProduct = LogicalQueryPlan::CreateProductQueryPlan(productOperation);
	LogicalQueryPlan *lpSelect = NULL;
	if(searchCondition != NULL)
	{
		SelectOperation selectOperation(searchCondition);
		LogicalQueryPlan *lpSelect = LogicalQueryPlan::CreateSelectQueryPlan(selectOperation);
		lpSelect->AddLeftLogicalPlan(lpProduct);
	}

	ProjectOperation projectionOperation(projectionList);
	LogicalQueryPlan *lpProjection = LogicalQueryPlan::CreateProjectQueryPlan(projectionOperation);

	if(searchCondition != NULL)
	{
		lpProjection->AddLeftLogicalPlan(lpSelect);
	}
	else
	{
		//Make product logical plan an argument of Projection Logical Plan
		lpProjection->AddLeftLogicalPlan(lpProduct);
	}

	return lpProjection; // lpProjection is my final logical plan
}

LogicalQueryPlan* LogicalQueryPlanGenerator::
	CreateOptimizedLogicalQueryPlan(bool isDistinct, vector<Column> projectionList, vector<string> fromList, SearchConditionNode*& searchCondition, vector<Column> orderByList)
{

	//Ignore last parameter searchCondition for now, Just implementing a simple select operation for now
	LogicalQueryPlan* optimizedQueryPlan;
	//Project and Product froduct operations are must, selectcondition(the where clause) is optional
	ProductOperation productOperation(fromList); //Create a product operation
	ProjectOperation projectionOperation(projectionList);
	if(searchCondition != NULL)
	{
		SelectOperation selectOperation(searchCondition);
		//Case One when we have select condition
		optimizedQueryPlan = LogicalQueryPlan::CreateProjSelProdQueryPlan(isDistinct, projectionOperation, selectOperation, productOperation, orderByList); 
	}

	else
		optimizedQueryPlan = LogicalQueryPlan::CreateProjProdQueryPlan(isDistinct, projectionOperation, productOperation, orderByList); 
	return optimizedQueryPlan;
}