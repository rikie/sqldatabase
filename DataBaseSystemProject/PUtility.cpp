#include"PUtility.h"

//External global variables, define in Global.h and initialized when execution starts in DBManagerStart.cpp
extern MainMemory mem;
extern Disk disk;
extern SchemaManager schema_manager;
extern vector<string> GlobalFromTables;
extern map<TableColumnPair, Value> ColumnValueMap;



CompareTuple::CompareTuple(vector<string> fn)
{
	fieldNames = fn;
}

bool CompareTuple::operator()(Tuple& first, Tuple& second)
{
	if(first.isNull() && second.isNull())
		return false;
	if((!first.isNull()) && second.isNull())
		return true;
	if(first.isNull() && (!second.isNull()))
		return false;

	vector<string>::iterator beg = fieldNames.begin();
	vector<string>::iterator end = fieldNames.end();
	vector<string>::iterator i;
	Schema tupleSchemaOne = first.getSchema();
	Schema tupleSchemaTwo = second.getSchema();

//	cout<< "comparing "<<first<<"  "<<second<<endl;
	if(tupleSchemaOne != tupleSchemaTwo)
		throw string("Tuple Schema Comparision Error!!!! - In PUtility::ComapreTuple::Opeartor()");
		
	for(i = beg; i!=end; i++)
	{
		string colName = *i;
		FIELD_TYPE ftype = tupleSchemaOne.getFieldType(colName);
		if(ftype == INT)
		{
			int firstVal, secVal;
			firstVal = first.getField(colName).integer;
			secVal = second.getField(colName).integer;
			if(firstVal != secVal)
				return firstVal<secVal;
		}
		else //FIELD_TYPE == STR20
		{
			string firstVal, secVal;
			firstVal = *(first.getField(colName).str); 
			secVal =  *(second.getField(colName).str);
			if(firstVal.compare(secVal) < 0) return true;
			else if(firstVal.compare(secVal) > 0) return false;
			//else == 0, continue comparing next tuple
		}
	}
	return false;
}


//comparision operator defined to compare two tables based on table size
//Each table size is evaluated in terms of no of blocks and not the no
//of tuples
bool CompareTableSize(string relOne, string relTwo)
{
	int noOfBlocksOne = schema_manager.getRelation(relOne)->getNumOfBlocks();
	int noOfBlocksTwo = schema_manager.getRelation(relTwo)->getNumOfBlocks();
	return noOfBlocksOne < noOfBlocksTwo;
};

//Main Memory Sorting
//In memory sorting operation based on tuples
bool MMSorting(int startMemIndex, int noOfBlocks, vector<string> fieldNames)
{
	//total no of blocks to sort
	vector<Tuple> tuplesInMem = mem.getTuples(startMemIndex, noOfBlocks);
	sort(tuplesInMem.begin(), tuplesInMem.end(), CompareTuple(fieldNames));
	//clear out any data in the given range of memory block
	for(int i=startMemIndex; i< noOfBlocks; i++)
	{
		mem.getBlock(i)->clear();
	}
	bool success = mem.setTuples(startMemIndex, tuplesInMem);
	if(!success)
		cout<<"Error in MMSorting"<<endl;
	return success;
}

//Compare two tuples
//Return true if equals else returns false
bool IsEqual(Tuple t1, Tuple t2)
{
	if(t1.isNull() || t2.isNull()) return false;
	Schema st1 = t1.getSchema();
	Schema st2 = t2.getSchema();
	
	if(st1!=st2) return false;

	vector<string> fieldNames = st1.getFieldNames();

	vector<string>::iterator iter;

	for(iter = fieldNames.begin(); iter!=fieldNames.end(); iter++)
	{
		if(st1.getFieldType(*iter) == INT)
		{
			if(t1.getField(*iter).integer != t2.getField(*iter).integer)
				return false;
		}
		else
		{
			string val1 = *(t1.getField(*iter).str);
			string val2 = *(t2.getField(*iter).str);
			if(val1 != val2)
				return false;
		}
	}

	return true;
}

//Compare two tuples
//Return true if equals else returns false
bool IsEqual(Tuple t1, Tuple t2, vector<string> fnames)
{
	if(t1.isNull() || t2.isNull())
		return false;
	Schema st1 = t1.getSchema();
	Schema st2 = t2.getSchema();
	
	if(st1!=st2) return false;

	vector<string> fieldNames = st1.getFieldNames();

	vector<string>::iterator iter;

	for(iter = fnames.begin(); iter!=fnames.end(); iter++)
	{
		if(st1.getFieldType(*iter) == INT)
		{
			if(t1.getField(*iter).integer != t2.getField(*iter).integer)
				return false;
		}
		else
		{
			string val1 = *(t1.getField(*iter).str);
			string val2 = *(t2.getField(*iter).str);
			if(val1 != val2)
				return false;
		}
	}

	return true;
}

//returns the first index of tuple to be searched for
//returns -1 if not found
int FindTuple(vector<Tuple> tuples, Tuple t, vector<string> fieldNames)
{
	int index;
	for(index = 0; index<tuples.size(); index++)
	{
		if(IsEqual(tuples[index], t, fieldNames))
			return index;
	}
	return -1;
}

//returns the first index of tuple to be searched for
//returns -1 if not found
int FindTuple(vector<Tuple> tuples, Tuple t)
{
	int index;
	for(index = 0; index<tuples.size(); index++)
	{
		if(IsEqual(tuples[index], t))
			return index;
	}
	return -1;
}

void MapTupleValues(Tuple t)
{
	Schema tupleSchema = t.getSchema();
	vector<string> columnNames = tupleSchema.getFieldNames();
	vector<string>::iterator iter;
	for(iter = columnNames.begin(); iter!=columnNames.end(); iter++)
	{
		int dotPos = (*iter).find_first_of('.');
		int len = (*iter).length();
		if(dotPos == string::npos) throw string("Error in column Mapping");
		string tableName = (*iter).substr(0, dotPos);
		string colName = (*iter).substr(dotPos+1, len - dotPos - 1);
		FIELD_TYPE ft = tupleSchema.getFieldType(*iter);
		Field value = t.getField(*iter);
		TableColumnPair tcpair = TableColumnPair(tableName, colName);
		Value val = Value(ft, value);
		ColumnValueMap[tcpair] = val;
	}
}

void MapTupleValues(Tuple t, string tableName)
{
	Schema tupleSchema = t.getSchema();
	vector<string> columnNames = tupleSchema.getFieldNames();
	vector<string>::iterator iter;
	for(iter = columnNames.begin(); iter!=columnNames.end(); iter++)
	{
		FIELD_TYPE ft = tupleSchema.getFieldType(*iter);
		Field value = t.getField(*iter);
		TableColumnPair tcpair = TableColumnPair(tableName, *iter);
		TableColumnPair tcpair2 = TableColumnPair(NO_TABLE, *iter);
		Value val = Value(ft, value);

		ColumnValueMap[tcpair] = val;
		ColumnValueMap[tcpair2] = val;
	}
}


Tuple FindMinTuple(vector<Tuple> tuples, vector<string> fieldNames)
{
	vector<Tuple>::iterator iter;
	iter = min_element(tuples.begin(), tuples.end(), CompareTuple(fieldNames));
	return *iter;
}
