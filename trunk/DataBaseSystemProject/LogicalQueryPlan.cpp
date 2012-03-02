#include"LogicalQueryPlan.h"

LogicalQueryPlan::LogicalQueryPlan()
{
	leftLQP = NULL;
	rightLQP = NULL;
	fromQueryPlan = NULL;
	isDistinct = false;
}

LogicalQueryPlan::LogicalQueryPlan(LogicalQueryPlan*& left, LogicalQueryPlan *&right)
{
	leftLQP = left;
	rightLQP = right;
}

void LogicalQueryPlan::AddLeftLogicalPlan(LogicalQueryPlan*& lp)
{
	leftLQP = lp;
}
void LogicalQueryPlan::AddRightLogicalPlan(LogicalQueryPlan*& rp)
{
	rightLQP = rp;
}

LogicalQueryPlan* LogicalQueryPlan::CreateSelectQueryPlan(SelectOperation selOperation)
{
	LogicalQueryPlan* lp = new LogicalQueryPlan();
	lp->operationType = SELECTION;
	lp->selectOperation = selOperation;
	return lp;
}

LogicalQueryPlan* LogicalQueryPlan::CreateProjectQueryPlan(ProjectOperation projOperation)
{
	LogicalQueryPlan* lp = new LogicalQueryPlan();
	lp->operationType = PROJECTION;
	lp->projectOperation = projOperation;
	return lp;
}

LogicalQueryPlan* LogicalQueryPlan::CreateProductQueryPlan(ProductOperation prodOperation)
{
	LogicalQueryPlan* lp = new LogicalQueryPlan();
	lp->operationType = CROSSPRODUCT;
	lp->productOperation = prodOperation;
	return lp;
}

LogicalQueryPlan* LogicalQueryPlan::CreateProjSelProdQueryPlan(bool isD, ProjectOperation projectOpn, SelectOperation &selOpn, ProductOperation prodOpn, vector<Column> orB)
{
	LogicalQueryPlan* lp = new LogicalQueryPlan();
	lp->operationType = O_PROJ_S_PROD_SEL;
	lp->projectOperation = projectOpn;
	lp->selectOperation = selOpn;
	lp->productOperation = prodOpn;
	lp->isDistinct = isD;
	lp->orderBy = orB;
	vector<string> fromList = prodOpn.GetFromList();
	if(fromList.size() >1)
	{
		FromQueryPlan::SetNoOfPersistentCount(fromList.size() - 2);
		FromQueryPlan* fromQueryPlan = FromQueryPlan::CreateOptimizedFromQueryPlan(fromList);
		lp->fromQueryPlan = fromQueryPlan;
		lp->operationType = O_PROJ_M_PROD;
	}
	return lp;

}

LogicalQueryPlan* LogicalQueryPlan::CreateProjProdQueryPlan(bool isD, ProjectOperation projectOpn, ProductOperation prodOpn, vector<Column> orB)
{
	LogicalQueryPlan* lp = new LogicalQueryPlan();
	//vector<Column> columns = projectOpn.GetColumnList();
	vector<string> tablenames = prodOpn.GetFromList();
	lp->isDistinct = isD;
	lp->orderBy = orB;
	if(tablenames.size() == 1)
	{
		//No Optimization required
		lp->productOperation = prodOpn;
		lp->projectOperation = projectOpn;
		lp->operationType = O_PROJ_S_PROD;
	}
	else
	{
		//From Query Optimized for table size
		FromQueryPlan* fromQueryPlan = FromQueryPlan::CreateOptimizedFromQueryPlan(tablenames);
		FromQueryPlan::SetNoOfPersistentCount(tablenames.size() - 2); //No of temporary tables needs to get persisted in DataBase
		lp->productOperation = prodOpn;
		lp->projectOperation = projectOpn;
		lp->fromQueryPlan = fromQueryPlan;
		lp->operationType = O_PROJ_M_PROD; 
	}
	return lp;
}

LogicalQueryPlan* LogicalQueryPlan::GetOptimizedQueryPlan()
	{
		LogicalQueryPlan* optPlan = new LogicalQueryPlan();
		switch(operationType)
		{
		case PROJECTION:
			optPlan->projectOperation = projectOperation;

			if(leftLQP != NULL) //For projection operation it must be one
			{
				LogicalQueryPlan*& arglp = leftLQP;
				if(arglp->operationType == CROSSPRODUCT)
				{
					vector<string> tableNames = arglp->productOperation.GetFromList();
					if(tableNames.size() == 1)
					{
						//A simple case of one table select
						//Create a single logical plan by merging product and project operation
						optPlan->productOperation = arglp->productOperation;
						optPlan->operationType = O_PROJ_S_PROD;

					}
				}
				else if(arglp->operationType == SELECTION)
				{
					
					SelectOperation& selOp = arglp->selectOperation;
					optPlan->selectOperation = selOp;
					if(arglp->leftLQP != NULL)
					{
						LogicalQueryPlan*& arglp2 = arglp->leftLQP;
						if(arglp2->operationType == CROSSPRODUCT)
						{
							vector<string> tableNames = arglp2->productOperation.GetFromList();
							if(tableNames.size() == 1)
							{
								//A simple case of one table select
								//Create a single logical plan by merging product and project operation
								optPlan->productOperation = arglp2->productOperation;
								optPlan->operationType = O_PROJ_S_PROD;
							}
						}
						else
							throw exception ("Select Argument must have a product");

					}
					else
					{
						throw string("Exception - Invalid Argument for Select Operation");
					}
				}
			}
			else
				throw string("Exception - Invalid Argument for Projection Operation");
			break;
		}
		return optPlan;
	}

ReturnRelation LogicalQueryPlan::Execute()
{
	ReturnRelation retRelation;
	switch(operationType)
	{
	case O_PROJ_S_PROD: //Single product... only one table
		if(projectOperation.GetColumnList().front().columnName == "*")
		{
			string relation = productOperation.GetFromList().front();
			vector<Tuple> tuples = TableScan::FetchTuples(isDistinct, relation, 0, orderBy);
			retRelation.markedForDeletion = false; //it is permanent table, do not delete it after printing its information
			retRelation.relationName = relation;
			retRelation.tuples = tuples;
		}
		else
		{
			//If project list has few columns
			string relation = productOperation.GetFromList().front();
			string tempTable = Utility::RandomTableName();
			vector<Tuple> tuples = TableScan::FetchTuples(isDistinct, projectOperation, selectOperation, relation, 0, tempTable, orderBy);
			retRelation.relationName = tempTable; //delete this relation after print
			retRelation.markedForDeletion = true;
			retRelation.tuples = tuples;
		}
		break;
	case O_PROJ_S_PROD_SEL: //Single product.. only one table
		if(projectOperation.GetColumnList().front().columnName == "*")
		{
			string relation = productOperation.GetFromList().front();
			vector<Tuple> tuples = TableScan::FetchSelectedTuples(isDistinct, relation, 0, selectOperation, orderBy);
			retRelation.markedForDeletion = false; //it is permanent table, do not delete it after printing its information
			retRelation.relationName = relation;
			retRelation.tuples = tuples;
		}
		else
		{
			//If project list has few columns
			string relation = productOperation.GetFromList().front();
			string tempTable = Utility::RandomTableName();
			vector<Tuple> tuples = TableScan::FetchTuples(isDistinct, projectOperation, selectOperation, relation, 0, tempTable, orderBy);
			retRelation.relationName = tempTable; //delete this relation after print
			retRelation.markedForDeletion = true;
			retRelation.tuples = tuples;
		}
		break;
	case O_PROJ_M_PROD:
		returnRelation = fromQueryPlan->Evaluate();
		bool persist = false;
		if(selectOperation.IsSearchCondition())
			returnRelation.tuples = TableScan::ApplySelectCondition(selectOperation, returnRelation);
		returnRelation.tuples = TableScan::FetchTuplesDistinctOrdered(returnRelation.tuples, isDistinct, projectOperation, returnRelation.relationName, 0, orderBy);
		if(projectOperation.GetColumnList().front().columnName != "*")
			returnRelation = TableScan::ApplyProjectOperation(projectOperation, productOperation, returnRelation, persist);
		retRelation = returnRelation;
		break;

	}

	return retRelation;
}

void LogicalQueryPlan::operator delete(void *p)
{
	if(p!=NULL)
	{
			if(((LogicalQueryPlan*)p)->leftLQP != NULL)
		delete ((LogicalQueryPlan*)p)->leftLQP;

	if(((LogicalQueryPlan*)p)->rightLQP != NULL)
		delete ((LogicalQueryPlan*)p)->rightLQP;
	free(p);
	}
}

