#ifndef _LOGICAL_QUERY_PLAN_H
#define _LOGICAL_QUERY_PLAN_H

#include<vector>
#include"Operation.h"
#include"PhysicalOperator.h"
#include"FromQueryPlan.h"
#include"Untility.h"
#include"ReturnRelation.h"

//Section 16.3
//From Parse tree to Logical query plan
//As we dont have any subquery implementation we only need simple logical query needs to be created

enum OperationType {SELECTION, PROJECTION, ORDERBY, CROSSPRODUCT, 
					O_PROJ_S_PROD, //Optimized (Single table)Product Followed by Project - A simple "select */a,b,c from table" case
					O_PROJ_S_PROD_SEL, //select */a,b,c from <tblename> where <condition>
					O_PROJ_M_PROD, // No Condition, More than one table involved
					O_PROJ_M_PROD_SEL //
					};
class LogicalQueryPlan
{
private:
	bool isDistinct;
	vector<Column> orderBy;
	ReturnRelation returnRelation;
	OperationType operationType;
	OrderByOperation orderByOperation;
	ProjectOperation projectOperation;
	SelectOperation selectOperation;
	ProductOperation productOperation;
	FromQueryPlan* fromQueryPlan;
	LogicalQueryPlan* leftLQP;
	LogicalQueryPlan* rightLQP;
public:
	LogicalQueryPlan();
	LogicalQueryPlan(LogicalQueryPlan*& left, LogicalQueryPlan *&right);
	void AddLeftLogicalPlan(LogicalQueryPlan*& lp);
	void AddRightLogicalPlan(LogicalQueryPlan*& rp);
	static LogicalQueryPlan* CreateSelectQueryPlan(SelectOperation selOperation);
	static LogicalQueryPlan* CreateProjectQueryPlan(ProjectOperation projectOperarion);
	static LogicalQueryPlan* CreateProductQueryPlan(ProductOperation prodOperation);
	static LogicalQueryPlan* CreateProjSelProdQueryPlan(bool isD, ProjectOperation projectOpn, SelectOperation& selOpn, ProductOperation prodOpn, vector<Column> orB);
	static LogicalQueryPlan* CreateProjProdQueryPlan(bool isD, ProjectOperation projectOpn, ProductOperation prodOpn, vector<Column> orB);

	LogicalQueryPlan* GetOptimizedQueryPlan();
	
	//Execute a logical query plan and returns the resulting table 
	ReturnRelation Execute();
	void operator delete(void *p);

}; 

#endif