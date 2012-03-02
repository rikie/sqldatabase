#ifndef _EXPRESSION_NODE_H
#define _EXPRESSION_NODE_H

#include<string>
#include<cstdlib>
#include<map>

#include "Block.h"
#include "Config.h"
#include "Disk.h"
#include "Field.h"
#include "MainMemory.h"
#include "Relation.h"
#include "Schema.h"
#include "SchemaManager.h"
#include "Tuple.h"

typedef pair<string, string> TableColumnPair;
typedef pair<FIELD_TYPE, Field> Value;

using namespace std;
enum EXP_OPERATOR { E_PLUS_OPT = 1, E_MINUS_OPT, E_MUL_OPT, E_DIV_OPT, E_SELF_OPT};
enum VALUE_TYPE {INT_V, LITERAL_V, COL_NAME_V};

class ColumnMap
{
	map <string, int> columnMap; //map column name to integer map
};
class ExpressionNode
{
private:
	EXP_OPERATOR expOperator;
	VALUE_TYPE valueType;
	string value;
	ExpressionNode *leftExpressionNode;
	ExpressionNode *rightExpressionNode;
public:
	//static ColumnMap columnMap; 
	static map <string, int> columnMap; //map column name to integer map
	ExpressionNode();
	ExpressionNode(string value, VALUE_TYPE valueType);
	ExpressionNode(ExpressionNode*& leftExpressionNode, string expOpt, ExpressionNode*& rightExpressionNode);
	Value Evaluate();
	void operator delete(void *p);
};

#endif