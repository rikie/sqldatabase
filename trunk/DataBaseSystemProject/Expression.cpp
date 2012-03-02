#include"Expression.h"
#include "PhysicalOperator.h"
//ColumnMap ExpressionNode::columnMap;
map<string, int> ExpressionNode::columnMap;
typedef pair<string, string> TableColumnPair;
typedef pair<FIELD_TYPE, Field> Value;
extern map<TableColumnPair, Value> ColumnValueMap;


ExpressionNode::ExpressionNode()
{
	value = "";
	expOperator = E_SELF_OPT;
	leftExpressionNode = NULL;
	rightExpressionNode = NULL;
}

ExpressionNode::ExpressionNode(string val, VALUE_TYPE valType)
{
	value = val;
	valueType = valType;
	expOperator = E_SELF_OPT;
	leftExpressionNode = NULL;
	rightExpressionNode = NULL;
}

ExpressionNode::ExpressionNode(ExpressionNode*& leftExpNode, string expOpt, ExpressionNode*& rightExpNode)
{
	if(expOpt == "+")
		expOperator = E_PLUS_OPT;
	else if(expOpt == "-")
		expOperator = E_MINUS_OPT;
	else if(expOpt == "*")
		expOperator = E_MUL_OPT;
	else
		expOperator = E_DIV_OPT;

	leftExpressionNode = leftExpNode;
	rightExpressionNode = rightExpNode;
}

TableColumnPair GetTableColumn(string value)
{
	int pos = value.find_first_of('.');
	int length = value.length();
	string tnam = value.substr(0, pos);
	string colname = value.substr(pos+1, length - pos -1);
	if(tnam == NO_TABLE)
		tnam = TableScan::GetTableName(colname);
	return TableColumnPair(tnam, colname);
}

Value ExpressionNode::Evaluate()
{
	Value result;
	switch(expOperator)
	{
	case E_PLUS_OPT:
		result.second.integer = leftExpressionNode->Evaluate().second.integer + rightExpressionNode->Evaluate().second.integer;
		break;
	case E_MINUS_OPT:
		result.second.integer = leftExpressionNode->Evaluate().second.integer - rightExpressionNode->Evaluate().second.integer;
		break;
	case E_MUL_OPT:
		result.second.integer = leftExpressionNode->Evaluate().second.integer * rightExpressionNode->Evaluate().second.integer;
		break;
	case E_DIV_OPT:
		int rightValue;
		rightValue = rightExpressionNode->Evaluate().second.integer;
		if(0 == rightValue) //For you Mr. Compiler
		{
			throw string("Expression Evaluation Error: Can not convert STR20 to INT");
		}
		else
		{
			int leftValue = leftExpressionNode->Evaluate().second.integer;
			result.second.integer = leftValue/rightValue; // result will be rounded off to integer
		}
		break;
	case E_SELF_OPT:
		if(valueType == INT_V)
		{
			result.second.integer = atoi(value.c_str());
			result.first = INT;
		}
		else if(valueType == COL_NAME_V)
		{
			TableColumnPair key = GetTableColumn(value);
			result = ColumnValueMap[key]; //Initialize this in physical query plan before accessing it here
		}
		else	
		{
			result.second.str = &value;
			result.first = STR20;
		}
		break;
	default:
		throw string("Expression Evaluation Error: Unrecognized Operator");
	}
	return result;
}

void ExpressionNode::operator delete(void *p)
{
	if(p!=NULL)
	{
	if(((ExpressionNode*)p)->leftExpressionNode != NULL)
		delete ((ExpressionNode*)p)->leftExpressionNode;

	if(((ExpressionNode*)p)->rightExpressionNode != NULL)
		delete ((ExpressionNode*)p)->rightExpressionNode;
	free(p);
	}
}

