#include "SearchCondition.h"

SearchConditionNode::SearchConditionNode(CompPredNode*& val)
{
	searchCondOpt = SC_SELF_OPT;
	comPredNode = val;
	leftSearchCondNode = NULL;
	rightSearchCondNode = NULL;
}

SearchConditionNode::SearchConditionNode(SearchConditionNode*& leftCondNode, string condOperator, SearchConditionNode*& rightCondNode)
{
	if(condOperator == "AND")
		searchCondOpt = SC_AND_OPT;
	else if(condOperator == "OR")
		searchCondOpt = SC_OR_OPT;
	else if(condOperator == "NOT")
		searchCondOpt = SC_NOT_OPT;
	
	leftSearchCondNode = leftCondNode;
	rightSearchCondNode = rightCondNode;

}
bool SearchConditionNode::Evaluate()
{
	bool result;
	switch(searchCondOpt)
	{
	case SC_AND_OPT:
		result = (leftSearchCondNode->Evaluate() && rightSearchCondNode->Evaluate());
		break;
	case SC_OR_OPT:
		result = (leftSearchCondNode->Evaluate() || rightSearchCondNode->Evaluate());
		break;
	case SC_NOT_OPT:
		//leftSearchCondNode should be NULL in this case
		result = (!rightSearchCondNode->Evaluate());
		break;
	case SC_SELF_OPT:
		result = comPredNode->Evaluate();
		break;
	}
	return result;
}

void SearchConditionNode::operator delete(void *p)
{
	if(p!=NULL)
	{
	if(((SearchConditionNode*)p)->comPredNode != NULL)
		delete ((SearchConditionNode*)p)->comPredNode;

	if(((SearchConditionNode*)p)->leftSearchCondNode != NULL)
		delete ((SearchConditionNode*)p)->leftSearchCondNode;

	if(((SearchConditionNode*)p)->rightSearchCondNode != NULL)
		delete ((SearchConditionNode*)p)->rightSearchCondNode;
	free(p);
	}
}