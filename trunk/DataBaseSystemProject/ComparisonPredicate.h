#ifndef _COMPARISION_P_H
#define _COMPARISION_P_H
#include<cstdlib>
#include "Expression.h"
enum COMP_OPT {C_LESS_OPT, C_GREAT_OPT, C_EQUAL_OPT, C_SELF_OPT};

class CompPredNode
{
private:
	bool value;
	COMP_OPT compOpt;
	ExpressionNode* leftExpression;
	ExpressionNode* rightExpression;
public:
	CompPredNode(bool val);
	CompPredNode(ExpressionNode*& leftExp, string copt, ExpressionNode*& rightExp);
	bool Evaluate();
	void operator delete(void *p);
};
#endif