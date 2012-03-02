#include "ComparisonPredicate.h"
typedef pair<string, string> TableColumnPair;
typedef pair<FIELD_TYPE, Field> Value;

CompPredNode::CompPredNode(bool val)
{
	value = val;
	compOpt = C_SELF_OPT;
	leftExpression = NULL;
	rightExpression = NULL;
}

CompPredNode::CompPredNode(ExpressionNode*& leftExp, string copt, ExpressionNode*& rightExp)
{
	if(copt == "=")
		compOpt = C_EQUAL_OPT;
	else if(copt == "<")
		compOpt = C_LESS_OPT;
	else
		compOpt = C_GREAT_OPT;
	leftExpression = leftExp;
	rightExpression = rightExp;
}

bool CompPredNode::Evaluate()
{
	FIELD_TYPE leftType;
	FIELD_TYPE rigthType;
	Value left;
	Value right;
	bool result;
	bool exception = false;
	switch(compOpt)
	{
	case C_LESS_OPT:
		left = leftExpression->Evaluate();
		right = rightExpression->Evaluate();
		if(left.first == INT && right.first == INT)
			result = (left.second.integer < right.second.integer);
		else if(left.first == STR20 && right.first == STR20)
		{
			if( (*(left.second.str)).compare(*(right.second.str)) <0)
				result = true;
			else
				result = false;
		}
		else
			exception = true;
		break;
	case C_GREAT_OPT:
		left = leftExpression->Evaluate();
		right = rightExpression->Evaluate();
		if(left.first == INT && right.first == INT)
			result = (left.second.integer > right.second.integer);
		else if(left.first == STR20 && right.first == STR20)
		{
			if( (*(left.second.str)).compare(*(right.second.str)) >0)
				result = true;
			else
				result = false;
		}
		else
			exception = true;
		break;
	case C_EQUAL_OPT:
		left = leftExpression->Evaluate();
		right = rightExpression->Evaluate();
		if(left.first == INT && right.first == INT)
			result = (left.second.integer == right.second.integer);
		else if(left.first == STR20 && right.first == STR20)
		{
			if( (*(left.second.str)).compare(*(right.second.str)) == 0)
				result = true;
			else
				result = false;
		}
		else
			exception = true;
		break;
	case C_SELF_OPT:
		result = value;
		break;
	}
	if(exception)
		throw string("Error!!! - Comparision Operation between Incompatible types");
	return result;
}

void CompPredNode::operator delete(void *p)
{
	if(p!=NULL)
	{
	if(((CompPredNode*)p)->rightExpression != NULL)
	{
		delete ((CompPredNode*)p)->rightExpression;
		((CompPredNode*)p)->rightExpression = NULL;
	}
	if(((CompPredNode*)p)->leftExpression != NULL)
	{
		delete ((CompPredNode*)p)->leftExpression;
		((CompPredNode*)p)->leftExpression = NULL;
	}

	free(p);
	p = NULL;
	}
}