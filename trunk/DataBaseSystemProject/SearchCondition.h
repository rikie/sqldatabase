#ifndef _SEARCH_CONDITION_H
#define _SEARCH_CONDITION_H
#include<cstdlib>
#include<string>
#include "ComparisonPredicate.h"
enum SearchConditionOperator {SC_AND_OPT = 1, SC_OR_OPT, SC_NOT_OPT, SC_SELF_OPT};

class SearchConditionNode
{

private:
	SearchConditionOperator searchCondOpt;
	CompPredNode* comPredNode;
	SearchConditionNode* leftSearchCondNode;
	SearchConditionNode* rightSearchCondNode;
public:
	SearchConditionNode(SearchConditionNode*& leftCondNode, string condOperator, SearchConditionNode*& rightCondNode);
	SearchConditionNode(CompPredNode*& value);
	bool Evaluate();
	void operator delete(void *p);
};
#endif