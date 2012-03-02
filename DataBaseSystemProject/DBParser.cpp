#include "DBParser.h"
ofstream logOut;
extern vector<string> GlobalFromTables;

//-----------Global Variable declaration for DBParser.cpp only-------------------

	//QUERY PLAN OBJECTS
	//LogicalQueryPlan *SelectQueryPlan = NULL;
	//LogicalQueryPlan *ProjectQueryPlan = NULL;
	//LogicalQueryPlan *ProductQueryPlan = NULL;
	LogicalQueryPlan *selectLQP;
	ExpressionNode* termExpression = NULL;
	ExpressionNode* factorExpression = NULL;
	ExpressionNode* expressionExpression = NULL;

	CompPredNode* comparisionPredicate = NULL;

	SearchConditionNode* booleanPrimarySearchCondition = NULL;
	SearchConditionNode* booleanFactorSearchCondition = NULL;
	SearchConditionNode* booleanTermSearchCondition = NULL;
	SearchConditionNode* searchCondition = NULL;
	//------------------
string tableName = "";
string attributeName = ""; //Set via InsertStatement, CreateStatement
enum FIELD_TYPE dataType;
string value;
Column column;

//dataTypeList and attibuteNameList together defines attribute-type-list
vector<enum FIELD_TYPE> dataTypeList;
vector<string> attributeNameList;
vector<string> attributeList; //only used in insert statement => insert-statement ::= INSERT INTO table-name(attribute-list) insert-tuples
vector<string> valueList; //only used in insert statement => insert-statement ::= INSERT INTO table-name(attribute-list) insert-tuples
							   //value-list ::= value | value, value-list
vector<Column> selectSublist;
vector<string> tableList;

vector<Column> orderByList;
bool isDistinct = false;

void DBParser::PrintDiskIOAndTimer()
{
	cout<<"-----------------------------------------"<<endl;
	cout<<"No. of Disk I/O: "<<disk.getDiskIOs()<<"   Timer Value: "<<disk.getDiskTimer()<<endl;
	cout<<"-----------------------------------------"<<endl;
}

void DBParser::ResetDiskIOAndTimer()
{
	//Reset the disk timer and i/o count after every operation
	disk.resetDiskIOs();
	disk.resetDiskTimer();
}

void DBParser::ClearGlobalVariables()
{
	tableName = "";
	attributeName = "";
	dataTypeList.clear();
	attributeNameList.clear();
	attributeList.clear();

	value = "";
	valueList.clear();
	column.columnName = ""; column.tableName = "";
	selectSublist.clear();
	tableList.clear();
	orderByList.clear();
	isDistinct = false;
	//Query Plan Pointers
	//if(SelectQueryPlan != NULL)
	// delete SelectQueryPlan;
	//SelectQueryPlan = NULL;

	//if(ProjectQueryPlan != NULL)
	//	delete ProjectQueryPlan;
	//ProjectQueryPlan = NULL;
	//if(ProductQueryPlan != NULL)
	//	delete ProductQueryPlan;
	//ProductQueryPlan = NULL;
	if(selectLQP != NULL)
		delete selectLQP;
	selectLQP = NULL;
	//if(searchCondition !=NULL)
	//	delete searchCondition;
	searchCondition = NULL;
	//if(termExpression  != NULL)
	//	delete termExpression ;
	termExpression = NULL;
	//if(factorExpression != NULL)
	//	delete factorExpression;
	factorExpression = NULL;
	//if(expressionExpression != NULL)
	//	delete expressionExpression;
	expressionExpression = NULL;
	//if(comparisionPredicate != NULL)
	//	delete comparisionPredicate;
	comparisionPredicate = NULL;
	//if(booleanPrimarySearchCondition != NULL)
	//	delete booleanPrimarySearchCondition;
	booleanPrimarySearchCondition = NULL;
	//if(booleanFactorSearchCondition != NULL)
	//	delete booleanFactorSearchCondition;
	booleanFactorSearchCondition = NULL;
	//if(booleanTermSearchCondition != NULL)
	//	delete booleanTermSearchCondition;
	booleanTermSearchCondition = NULL;

	GlobalFromTables.clear();

}

//-----------end of global variable declaration---------------------



DBParser::DBParser():lexAnalyzer(), sqlWorker()
{
	cout<<"Parser Instantiated"<<endl;
}

DBParser::DBParser(char* text):lexAnalyzer(text)
{
	//cout<<"Parser Initialised"<<endl;
	if(!logOut.is_open())
	{
	  logOut.open("DB_LOG.txt");
	  	if (!logOut) 
		{
			cerr << "The file DB_LOG is not opened" << endl;
		}
	}
	else
	{
		logOut<<"\n\n-----------------------------------------------"<<endl;
		logOut<<"-----------------------------------------------\n\n"<<endl;
	}

}

void DBParser::ParseStatement()
{
	logOut<<"Parsing <Statement>"<<endl;
	//#######GRAMMAR######
	/*statement ::= create-table-statement | drop-table-statement | select-statement
	| delete-statement | insert-statement*/
	Token token;
	while(token.Type != EOF)
	{
		bool success = true;
		token = lexAnalyzer.GetNextToken();
		switch(token.Type)
		{
		case CREATE:
			logOut<<"Encounter Keyword: "<<token.Value<<endl;
			ParseCreateTableStatement();
			cout<<"Parsing of CREATE TABLE Statement is successfull"<<endl;
			//Now create table based on parsed values
			ResetDiskIOAndTimer();
			success = sqlWorker.CreateTable(tableName, attributeNameList, dataTypeList);
			PrintDiskIOAndTimer();
			ClearGlobalVariables();
			break;
		case DROP:
			logOut<<"Encounter Keyword: "<<token.Value<<endl;
			ParseDropTableStatement();
			cout<<"Parsing DROP Statement is successfull"<<endl;
			ResetDiskIOAndTimer();
			success = sqlWorker.DropTable(tableName);
			PrintDiskIOAndTimer();
			ClearGlobalVariables();
			break;
		case SELECT:
			logOut<<"Encounter Keyword: "<<token.Value<<endl;
			ParseSelectStatement();
			cout<<"Parsing SELECT Statement is successfull"<<endl;
			//-----------------------------
			//CreateQueryPlan for Select Operation
			GlobalFromTables = tableList;
			selectLQP = LQPGenerator::CreateOptimizedLogicalQueryPlan(isDistinct, selectSublist, tableList, searchCondition, orderByList);
			//-----------------------------
			//LogicalQueryPlan *optimizedSelectLQP;
			//optimizedSelectLQP = LQPOptimizer::OptimizeLogicalQueryPlan(selectLQP);
			ResetDiskIOAndTimer();
			success = sqlWorker.ExcecuteLogicalPlan(selectLQP);
			PrintDiskIOAndTimer();
			ClearGlobalVariables();
			break;
		case DELETE:
			logOut<<"Encounter Keyword: "<<token.Value<<endl;
			ParseDeleteStatement();
			cout<<"Parsing DELETE Statement is Successfull"<<endl;
			GlobalFromTables.push_back(tableName);
			ResetDiskIOAndTimer();
			success = sqlWorker.DeleteTable(tableName, searchCondition);
			PrintDiskIOAndTimer();
			ClearGlobalVariables();			
			break;
		case INSERT:
			logOut<<"Encounter Keyword: "<<token.Value<<endl;
			ParseInsertStatement();
			cout<<"Parsing of INSERT Statement is sucessfull"<<endl;
			//Now insert tuples in table based on parsed values
			success = sqlWorker.InsertTuple(tableName, attributeList, valueList);
			ClearGlobalVariables();
			break;
		case EOF:
			logOut<<"Encounter EOF"<<endl;
			break;
		default:
			logOut<<"Encounter Unrecognized Symbol: "<<token.Value<<endl;
			//string str = ;
			throw string("Wrong start of statement: ").append(token.Value);
		}
		if(success == false) //stop execution
				throw string(" - ");
	}
}

void DBParser::ParseDeleteStatement()
{
	logOut<<"Parsing <DeleteStatement>"<<endl;
	//####GRAMMAR####
	//DELETE FROM table-name [WHERE search-condition]
	Token token = lexAnalyzer.GetNextToken();
	if(token.Type == FROM)
	{
		ParseTableName();
		logOut<<"Encounter Table Name: "<<tableName<<endl;
		token = lexAnalyzer.GetNextToken();
		if(token.Type == WHERE)
		{
			logOut<<"Encounter Keyword: "<<token.Value<<endl;
			ParseSearchCondition();
		}
		else
			lexAnalyzer.RestorePreviousToken();
	}
	else
		throw string("Expected Kyword FROM");
}

void DBParser::ParseCreateTableStatement()
{
	logOut<<"Parsing <CreateTableStatement>"<<endl;
	Token token;
	//create-table-statement ::= CREATE TABLE table-name(attribute-type-list)
	token = lexAnalyzer.GetNextToken();
	switch(token.Type)
	{
	case TABLE:
		logOut<<"Encounter Keyword: "<<token.Value<<endl;
		string relationName = ParseTableName();
		token = lexAnalyzer.GetNextToken();
		switch(token.Type)
		{
		case OPAREN:
			logOut<<"Encounter Syntax: "<<token.Value<<endl;
			ParseAttributeTypeList();
			token = lexAnalyzer.GetNextToken();
			if(token.Type == EPAREN)
			{
				logOut<<"Encounter Syntax: "<<token.Value<<endl;
				//Schema schema(field_names, field_types);
				//Relation* relation_ptr=schema_manager.createRelation(relationName,schema);
				// Print the information about the Relation
				//logOut << "The table has name " << relation_ptr->getRelationName() << endl;
				//logOut << "The table has schema:" << endl;
				//logOut << relation_ptr->getSchema() << endl;
				//logOut << "The table currently have " << relation_ptr->getNumOfBlocks() << " blocks" << endl;
				//logOut << "The table currently have " << relation_ptr->getNumOfTuples() << " tuples" << endl;
				// clear the global variables
				//field_names.clear();
				//field_types.clear();
				//logOut<<"Table Created Sucessfully"<<endl;
			}
			else
			{
				throw string(") Expected");
			}
			break;
		default:
			// clear the global variables
			ClearGlobalVariables();
			//field_names.clear();
			//field_types.clear();
			throw string("( expected");
		}
		break;
	}
}

string DBParser::ParseTableName()
{
	logOut<<"Parsing <TableName>"<<endl;
	//####GRAMMAR######
	//table-name ::= letter[digit | letter]*
	Token token;
	token = lexAnalyzer.GetNextToken();
	if(token.Type == IDENTIFIER)
	{
		//polulate table name here
		logOut<<"Encounter Table Name IDENTIFIER: " <<token.Value<<endl;
		tableName = token.Value;
		return token.Value;
	}
	else
	{
		throw string("Table name expected");
	}
}

void DBParser::ParseAttributeTypeList()
{
	logOut<<"Parsing <AttributeTypeList>"<<endl;
	//#####GRAMMAR#####
	//attribute-type-list ::= attribute-name data-type | attribute-name data-type, attribute-type-list
	Token token;
	ParseAttributeName();
	attributeNameList.push_back(attributeName);
	ParseDataType();
	dataTypeList.push_back(dataType);
	token = lexAnalyzer.GetNextToken();
	if(token.Type == COMMA)
	{
		logOut<<"Encounter Syntax: "<<token.Value<<endl;
		ParseAttributeTypeList();
	}
	else
		lexAnalyzer.RestorePreviousToken();
}

void DBParser::ParseAttributeName()
{
	logOut<<"Parsing <AttributeName>"<<endl;
	//#####GRAMMAR#####
	//attribute-name ::= letter[digit | letter]
	Token token;
	token = lexAnalyzer.GetNextToken();
	if(token.Type == IDENTIFIER)
	{
		//field_names.push_back(token.Value);
		attributeName = token.Value;
		logOut<<"Encounter IDENTIFIER: "<<token.Value<<endl;
	}
	else
	{
		throw string("Identifier(Attribute Name) Expected");
	}
}

void DBParser::ParseDataType()
{
	logOut<<"Parsing <DataType>"<<endl;
	
	//#####GRAMMAR#####
	//data-type ::= INT | STR20
	Token token;
	token = lexAnalyzer.GetNextToken();
	if(token.Type == INT_T)
	{
		logOut<<"Encounter DataType: "<< token.Value<<endl;
		dataType = INT;
	}
	else if(token.Type == STR20_T)
	{
		logOut<<"Encounter DataType: "<< token.Value<<endl;
		dataType = STR20;
	}
	else
	{
		throw string("Data Type Expected");
	}
}

void DBParser::ShowMemoryDump()
{
	
//	logOut<<"The memory contains " << mem.getMemorySize() << " blocks" << endl;
	//logOut << mem << endl << endl;
}

void DBParser::ParseDropTableStatement()
{
	logOut<<"Parsing <DropTableStatement>"<<endl;
	//#####GRAMMAR#####
	//drop-table-statement ::= DROP TABLE table-name
	Token token = lexAnalyzer.GetNextToken();
	if(token.Type== TABLE)
	{
		logOut<<"Encounter Keyword: "<<token.Value<<endl;
		ParseTableName();
		logOut<<"Table needs to be droped: "<<tableName<<endl;
	}
	else
	{
		throw string("Expected Keyword TABLE");
	}
		
}

void DBParser::ParseInsertStatement()
{
	logOut<<"Parsing <InsertStatement>"<<endl;	
	//####GRAMMAR#####
	//INSERT INTO table-name(attribute-list) insert-tuples
	Token token;
	token = lexAnalyzer.GetNextToken();
	if(token.Type == INTO)
	{
		logOut<<"Encounter Keyword: "<<token.Value<<endl;
		string tableName = ParseTableName();
		token = lexAnalyzer.GetNextToken();
		if(token.Type == OPAREN)
		{
			logOut<<"Encounter Syntax: "<<token.Value<<endl;
			ParseAttributeList();
			token = lexAnalyzer.GetNextToken();
			if(token.Type == EPAREN)
			{
				logOut<<"Encounter Syntax: "<<token.Value<<endl;
				ParseInsertTuples();
			}
			else
				throw string("Expected Syntax )");
		}
		else
			throw string("Expected syntax (");
	}
	else
	{
		throw string("Expected keyword INTO");
	}
}

void DBParser::ParseAttributeList()
{
	logOut<<"Parsing <AttributeList>"<<endl;
	//####GRAMMAR####
	//attribute-list ::= attribute-name | attribute-name, attribute-list
	ParseAttributeName();
	attributeList.push_back(attributeName);
	Token token = lexAnalyzer.GetNextToken();
	if(token.Type == COMMA)
	{
		logOut<<"Encounter Syntax: "<<token.Value<<endl;
		ParseAttributeList();
	}
	else
		lexAnalyzer.RestorePreviousToken();
	
}

void DBParser::ParseInsertTuples()
{
	logOut<<"Parsing <InsertTuples>"<<endl;
	//####GRAMMAR####
	//insert-tuples ::= VALUES (value-list) | select-statement
	Token token = lexAnalyzer.GetNextToken();
	if(token.Type == VALUES)
	{
		logOut<<"Encounter Keyword: "<<token.Value<<endl;
		token = lexAnalyzer.GetNextToken();
		if(token.Type == OPAREN)
		{
			logOut<<"Encounter Syntax: "<<token.Value<<endl;
			ParseValueList();
			token = lexAnalyzer.GetNextToken();
			if(token.Type == EPAREN)
			{
				logOut<<"Encounter Syntax: "<<token.Value<<endl;
				return;
			}
			else
				throw string("Expected syntax )");
		}
		else
			throw string("Expected Syntax (");
	}
	else if(token.Type == SELECT)
	{
		logOut<<"Encounter Keyword: "<<token.Value<<endl;
		ParseSelectStatement();
	}
	else
		throw string("Expected Keyword VALUES");
}

void DBParser::ParseValueList()
{
	logOut<<"Parsing <ValueList>"<<endl;
	//####GRAMMAR#####
	//value-list ::= value | value, value-list
	ParseValue();
	valueList.push_back(value);
	Token token = lexAnalyzer.GetNextToken();
	if(token.Type == COMMA)
	{
		logOut<<"Encounter Syntax: "<<token.Value<<endl;
		ParseValueList();
	}
	else
		lexAnalyzer.RestorePreviousToken();
}

void DBParser::ParseValue()
{
	logOut<<"Parsing <Value>"<<endl;
	//####GRAMMAR#####
	//value ::= literal | integer | NULL
	Token token = lexAnalyzer.GetNextToken();
	switch(token.Type)
	{
	case LITERAL:
		logOut<<"Encounter LITERAL: "<<token.Value<<endl;
		value =  token.Value;
		break;
	case INTEGER:
		logOut<<"Encounter INTEGER: "<<token.Value<<endl;
		value =  token.Value;
		break;
	case NULL_T:
		logOut<<"Encounter Keyword: "<<token.Value<<endl;
		//If NULL setting the values to default as suggested by TA
		value =  token.Value;
		break;
	default:
		throw string("Expected LITERAL or INTEGER or NULL");
	}
}

void DBParser::ParseSelectStatement()
{
	logOut<<"Parsing <SelectStatement>"<<endl;
	//####GRAMMAR#####
	//select-statement ::= SELECT [DISTINCT] select-list
	//FROM table-list
	//[WHERE search-condition]
	//[ORDER BY colume-name]

	Token token = lexAnalyzer.GetNextToken();
	if(token.Type == DISTINCT)
	{
		logOut<<"Encounter Keyword: "<<token.Value<<endl;
		isDistinct = true;
		ParseSelectList();
	}
	else
	{
		lexAnalyzer.RestorePreviousToken();
		ParseSelectList();
	}

	token = lexAnalyzer.GetNextToken();
	if(token.Type == FROM)
	{
		logOut<<"Encounter Keyword: "<<token.Value<<endl;
		ParseTableList();
		token = lexAnalyzer.GetNextToken();
		if(token.Type == WHERE)
		{
			logOut<<"Encounter Keyword: "<<token.Value<<endl;
			ParseSearchCondition();
		}
		else
			lexAnalyzer.RestorePreviousToken();

		token = lexAnalyzer.GetNextToken();
		if(token.Type == ORDER)
		{
			logOut<<"Encounter Keyword: "<<token.Value<<endl;			
			token = lexAnalyzer.GetNextToken();
			if(token.Type == BY)
			{
				logOut<<"Encounter Keyword: "<<token.Value<<endl;
				ParseColumnName();
				orderByList.push_back(column);
			}
			else
				throw string("Expected Keyword BY");
		}
		else
			lexAnalyzer.RestorePreviousToken();
	}
	else
		throw string("Expected Keyword FROM");
}

void DBParser::ParseSelectList()
{
	logOut<<"Parsing <SelectList>"<<endl;
	//####GRAMMAR####
	//select-list ::= * | select-sublist
	Token token = lexAnalyzer.GetNextToken();
	if(token.Type == STARSIGN)
	{
		column.columnName = token.Value;
		column.tableName = NO_TABLE;
		selectSublist.push_back(column);
		logOut<<"Encounter Syntax: "<<token.Value<<endl;
	}
	else
	{
		lexAnalyzer.RestorePreviousToken();
		ParseSelectSublist();
	}
}

void DBParser::ParseSelectSublist()
{
	logOut<<"Parsing <ParseSelectSublist>"<<endl;
	//####GRAMMAR####
	//select-sublist ::= column-name | column-name, select-sublist
	ParseColumnName();
	selectSublist.push_back(column);
	Token token = lexAnalyzer.GetNextToken();
	if(token.Type == COMMA)
	{
		logOut<<"Encounter Syntax: "<<token.Value<<endl;
		ParseSelectSublist();
	}
	else
		lexAnalyzer.RestorePreviousToken();
}

void DBParser::ParseTableList()
{
	logOut<<"Parsing <TableList>"<<endl;
	//####GRAMMAR####
	//table-list ::= table-name | table-name, table-list
	ParseTableName();
	tableList.push_back(tableName);
	Token token = lexAnalyzer.GetNextToken();
	if(token.Type == COMMA)
	{
		logOut<<"Encounter Syntax: "<<token.Value<<endl;
		ParseTableList();
	}
	else
		lexAnalyzer.RestorePreviousToken();
}

void DBParser::ParseColumnName()
{
	logOut<<"Parsing <ColumnName>"<<endl;
	//####GRAMMAR#####
	//column-name ::= [table-name.]attribute-name
	//Here we are not taking conventional way of coding as the recusion starts on left which could be optional
	Token token;
	string columnName;
	string tableName1;// = ParseTableName();
	//----------------
	//logOut<<"Parsing <TableName.Column Name>"<<endl;
	//####GRAMMAR######
	//table-name ::= letter[digit | letter]*
	Token token1;
	token1 = lexAnalyzer.GetNextToken();
	if(token1.Type == IDENTIFIER)
	{
		//polulate table name here
		logOut<<"Encounter Table Name IDENTIFIER: " <<token.Value<<endl;
		tableName1 = token1.Value;
	}
	else
	{
		throw string("Table/Column name expected");
	}
	//----------------
	token = lexAnalyzer.GetNextToken();// Read '.'
	if(token.Type == DOT)
	{
		token = lexAnalyzer.GetNextToken(); //we are not calling procedure ParseAttributeName here
		if(token.Type == IDENTIFIER)
		{
			columnName = token.Value;
			logOut<<"Encounter Column Name: "<<tableName1<<"."<<columnName<<endl;
		}
		else
			throw ("Expected Column name after DOT(.)");

	}
	else
	{
		//If no DOT operator then the Identifier fetched in the first place is basically column name and not table name.
		columnName = tableName1;
		tableName1 = NO_TABLE; 
		logOut<<"Encounter ColumnName: "<<columnName<<endl;
		lexAnalyzer.RestorePreviousToken(); //Go back to previous token as there were no DOT to read.
	}

	logOut<<tableName1<<"."<<columnName<<endl;
	column.tableName = tableName1;
	column.columnName = columnName;
}

void DBParser::ParseFactor()
{
	string colName;
	logOut<<"Parsing<Factor>"<<endl;
	//####GRAMMAR#####
	//factor ::= column-name | literal | integer | ( expression )
	Token token = lexAnalyzer.GetNextToken();
	switch(token.Type)
	{
	case IDENTIFIER: //Column Name
		lexAnalyzer.RestorePreviousToken();
		ParseColumnName();
		colName = column.tableName;
		colName.append(".");
		colName.append(column.columnName);
		factorExpression = new ExpressionNode(colName, COL_NAME_V);
		break;
	case LITERAL:
		logOut<<"Encounter LITERAL: "<<token.Value<<endl;
		logOut<< "Literal: "<<token.Value<<endl;
		factorExpression = new ExpressionNode(token.Value, LITERAL_V);
		break;
	case INTEGER:
		logOut<<"Encounter INTEGER: "<<token.Value<<endl;
		logOut<< "Integer: "<<token.Value<<endl;
		factorExpression = new ExpressionNode(token.Value, INT_V);
		break;
	case OPAREN:
		logOut<<"Encounter Syntax: "<<token.Value<<endl;		
		ParseExpression();
		token = lexAnalyzer.GetNextToken();
		if(token.Type == EPAREN)
		{
			logOut<<"Encounter Syntax: "<<token.Value<<endl;	
			factorExpression = expressionExpression;
		}
		else
			throw string("Expected Syntax )");
		break;
	default:
		throw string("Expected IDENTIFIER, LITERAL, INTEGER or (EXPRESSION)");
	}

}

void DBParser::ParseTerm()
{
	logOut<<"Parsing <Term>"<<endl;
	//####GRAMMAR#####
	//term ::= factor | factor * term | factor / term
	ParseFactor();
	termExpression = factorExpression;
	ExpressionNode * leftFactorNode = factorExpression;
	Token token = lexAnalyzer.GetNextToken();
	if(token.Type == STARSIGN)
	{
		logOut<<"Encounter Operator: "<<token.Value<<endl;
		ExpressionNode* left = termExpression;
		ParseTerm();
		ExpressionNode* right = termExpression;
		termExpression = new ExpressionNode(left, token.Value, right);
	}
	else if(token.Type == DIVSIGN)
	{
		logOut<<"Encounter Operator: "<<token.Value<<endl;
		ExpressionNode* left = termExpression;
		ParseTerm();
		ExpressionNode* right = termExpression;
		termExpression = new ExpressionNode(left, token.Value, right);
	}
	else
		lexAnalyzer.RestorePreviousToken();

}

void DBParser::ParseExpression()
{
	logOut<<"Parsing <Expression>"<<endl;
	//####GRAMMAR####
	//expression ::= term | term + expression | term - expression
	ParseTerm();
	expressionExpression = termExpression;
	Token token = lexAnalyzer.GetNextToken();
	if(token.Type == PLUSSIGN)
	{
		logOut<<"Encounter Operator: "<<token.Value<<endl;
		ExpressionNode* left = expressionExpression;
		ParseExpression();
		ExpressionNode* right = expressionExpression;
		expressionExpression = new ExpressionNode(left, token.Value, right);
	}
	else if(token.Type == MINUSSIGN)
	{
		logOut<<"Encounter Operator: "<<token.Value<<endl;
		ExpressionNode* left = expressionExpression;
		ParseExpression();
		ExpressionNode* right = expressionExpression;
		expressionExpression = new ExpressionNode(left, token.Value, right);
	}
	else
		lexAnalyzer.RestorePreviousToken();
}

void DBParser::ParseBooleanFactor()
{
	logOut<<"Parsing <BooleanFactor>"<<endl;
	//####GRAMMAR####
	//boolean-factor ::= [NOT] boolean-primary
	//boolean-factor ::= [NOT] (boolean-primary)
	//changeing this grammar a bit
	Token token = lexAnalyzer.GetNextToken();
	if(token.Type == NOT)
	{
		logOut<<"Encounter Keyword: "<<token.Value<<endl;
		ParseBooleanPrimary();
		SearchConditionNode* nullNode = NULL;
		booleanFactorSearchCondition = new SearchConditionNode(nullNode, token.Value, booleanPrimarySearchCondition);
	}
	else
	{
		lexAnalyzer.RestorePreviousToken();
		ParseBooleanPrimary();
		booleanFactorSearchCondition = booleanPrimarySearchCondition;
	}
}
//OLD GRAMMAR
//void DBParser::ParseBooleanPrimary()
//{
//	logOut<<"Parsing <BooleanPrimary>"<<endl;
//	//####GRAMMAR####
//	//boolean-primary ::= comparison-predicate |( search-condition )
//	Token token = lexAnalyzer.GetNextToken();
//	if(token.Type == OPAREN)
//	{
//		logOut<<"Encounter Syntax: "<<token.Value<<endl;
//		ParseSearchCondition();
//		token = lexAnalyzer.GetNextToken();
//		if(token.Type == EPAREN)
//		{
//			logOut<<"Encounter Syntax: "<<token.Value<<endl;
//		}
//		else
//			throw string("Expected Syntax )");
//	}
//	else
//	{
//		lexAnalyzer.RestorePreviousToken();
//		ParseComparisionPredicate();
//	}
//}


//NEW GRAMMAR
void DBParser::ParseBooleanPrimary()
{
	logOut<<"Parsing <BooleanPrimary>"<<endl;
	//####GRAMMAR####
	//boolean-primary ::= comparison-predicate |"[" search-condition "]"
	Token token = lexAnalyzer.GetNextToken();
	if(token.Type == OSQPAREN)
	{
		logOut<<"Encounter Syntax: "<<token.Value<<endl;
		ParseSearchCondition();
		token = lexAnalyzer.GetNextToken();
		if(token.Type == ESQPAREN)
		{
			logOut<<"Encounter Syntax: "<<token.Value<<endl;
			booleanPrimarySearchCondition = searchCondition;
		}
		else
			throw string("Expected Syntax )");
	}
	else
	{
		lexAnalyzer.RestorePreviousToken();
		ParseComparisionPredicate();
		booleanPrimarySearchCondition = new SearchConditionNode(comparisionPredicate);
	}
}

void DBParser::ParseComparisionPredicate()
{
	logOut<<"Parsing <ComparisionPredicate>"<<endl;
	//####GRAMMAR####
	//comparison-predicate ::= expression comp-op expression
	ParseExpression();
	ExpressionNode* left = expressionExpression;
	Token token = lexAnalyzer.GetNextToken();
	if(token.Type == COMPOP)
	{
		logOut<<"Encounter COMPOP Syntax: "<<token.Value<<endl;
		ParseExpression();
		ExpressionNode* right = expressionExpression;
		comparisionPredicate = new CompPredNode(left, token.Value, right);
	}
	else
		throw ("Expected Comparision Operator =,< or >");
}

void DBParser::ParseSearchCondition()
{
	logOut<<"Parsing <SearchCondition>"<<endl;
	//####GRAMAR####
	//search-condition ::= boolean-term | boolean-term OR search-condition
	ParseBooleanTerm();
	searchCondition = booleanTermSearchCondition;
	Token token = lexAnalyzer.GetNextToken();
	if(token.Type == OR)
	{
		logOut<<"Encounter Keyword: OR"<<endl;
		SearchConditionNode* left = searchCondition;
		ParseSearchCondition();
		SearchConditionNode* right = searchCondition;
		searchCondition = new SearchConditionNode(left, token.Value, right);
	}
	else
		lexAnalyzer.RestorePreviousToken();
}

void DBParser::ParseBooleanTerm()
{
	logOut<<"Parsing <BooleanTerm>"<<endl;
	//####GRAMMAR####
	//boolean-term ::= boolean-factor | boolean-factor AND boolean-term
	ParseBooleanFactor();
	booleanTermSearchCondition = booleanFactorSearchCondition;
	Token token = lexAnalyzer.GetNextToken();
	if(token.Type == AND)
	{
		logOut<<"Encounter Keyword: AND"<<endl;
		SearchConditionNode* left = booleanTermSearchCondition;
		ParseBooleanTerm();
		SearchConditionNode* right = booleanTermSearchCondition;
		booleanTermSearchCondition = new SearchConditionNode(left, token.Value, right);
	}
	else
		lexAnalyzer.RestorePreviousToken();
}