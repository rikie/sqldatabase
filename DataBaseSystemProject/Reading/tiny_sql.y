%{
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include <iostream>
#include "Block.h"
#include "Config.h"
#include "Disk.h"
#include "Field.h"
#include "MainMemory.h"
#include "Relation.h"
#include "Schema.h"
#include "SchemaManager.h"
#include "Tuple.h"

//declared for C++ compiler
extern "C"
{
        int yyparse(void);
        int yylex(void);
        int yywrap()
        {
                return 1;
        }
        void yyerror(const char *str)
        {
                fprintf(stderr,"error: %s\n",str);
        }
        
}
extern int yydebug;

//global variables
MainMemory mem;
Disk disk;
SchemaManager schema_manager(&mem,&disk);

vector<enum FIELD_TYPE> field_types;
vector<string> field_names;

//main function
main()
{
        //yydebug=1;
        yyparse();

}

%}

%token CREATE TABLE OPAREN EPAREN COLON NEWLINE

%union 
{
	int number;
	char *str;
}

%token <str> NAME
%type <number> data_type
%token <number> INTTOK
%token <number> STR20TOK

%%

statements:
	statement NEWLINE
	| statements statement NEWLINE
;

statement: create_table_statement
;

create_table_statement:
          CREATE TABLE NAME OPAREN attribute_type_list EPAREN
  {
    // Execute the physical plan of the CREATE TABLE statement
    // The following code is copied and paste from TestStorageManager.cpp

    // Use the two global vector variables field_names, field_types to create a schema
    Schema schema(field_names,field_types);
    string relation_name=$3;
    Relation* relation_ptr=schema_manager.createRelation(relation_name,schema);

    // Print the information about the Relation
    cout << "The table has name " << relation_ptr->getRelationName() << endl;
    cout << "The table has schema:" << endl;
    cout << relation_ptr->getSchema() << endl;
    cout << "The table currently have " << relation_ptr->getNumOfBlocks() << " blocks" << endl;
    cout << "The table currently have " << relation_ptr->getNumOfTuples() << " tuples" << endl;

    // clear the global variables
    field_names.clear();
    field_types.clear();
  }
;

attribute_type_list:
          NAME data_type
  {
    // stores the field name and the field type in two global vector variables field_names, field_types
    field_names.push_back($1);

    if ($2==INT) // Here INT is not a lex token. It is a field type defined in Field.h
      field_types.push_back(INT);
    else if ($2==STR20)
      field_types.push_back(STR20);
  }
        | NAME data_type COLON attribute_type_list
  {
    // stores the field name and the field type in two global vector variables field_names, field_types
    field_names.push_back($1);

    if ($2==INT) // Here INT is not a lex token. It is a field type defined in Field.h
      field_types.push_back(INT);
    else if ($2==STR20)
      field_types.push_back(STR20);
  }
;

data_type: INTTOK { $$=$1; } | STR20TOK { $$=$1; }
;

