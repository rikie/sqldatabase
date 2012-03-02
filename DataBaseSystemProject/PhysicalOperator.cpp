#include"PhysicalOperator.h"

extern vector<string> GlobalFromTables;
extern map<TableColumnPair, Value> ColumnValueMap;

TableScan::TableScan(string rel, int startMemIndex, int size)
{
	relation = rel;
	startMemBlkIndex = startMemIndex;
	//endMemBlkIndex = startMemIndex + size -1;
	noOfBlocksToBeReadInCurrentPass = size;

	if(noOfBlocksToBeReadInCurrentPass < 1) throw exception("invalide agruments");
		
	relationPtr = schema_manager.getRelation(rel);
	if(relationPtr == NULL) 
		throw string("Error!!!");

	noOfBlocksInRelation = relationPtr->getNumOfBlocks();
	noOfTuplesInRelation = relationPtr->getNumOfTuples();

	noOfBlocksRemainingToScan = noOfBlocksInRelation;

	if(noOfBlocksRemainingToScan <= noOfBlocksToBeReadInCurrentPass)
		noOfBlocksToBeReadInCurrentPass = noOfBlocksRemainingToScan;

	currRelBlockIndex = 0;
	currTupleIndex = 0;
	currMemIndex = startMemIndex;

	bool success = relationPtr->getBlocks(currRelBlockIndex, startMemBlkIndex, noOfBlocksToBeReadInCurrentPass);
	if(success)
	{
		currentTuples = mem.getTuples(startMemBlkIndex, noOfBlocksToBeReadInCurrentPass);
	}
	else
	{
		throw string("Error In scanning");
	}

}

//Tuple TableScan::GetNextTuple()
//{
//	if(currentTuples.size() == 0)
//	{
//		currTupleIndex = 0;
//		currRelBlockIndex = currRelBlockIndex + noOfBlocksToBeReadInCurrentPass;
//		noOfBlocksRemainingToScan = noOfBlocksRemainingToScan - noOfBlocksToBeReadInCurrentPass;
//		if(noOfBlocksRemainingToScan <= noOfBlocksToBeReadInCurrentPass)
//			noOfBlocksToBeReadInCurrentPass = noOfBlocksRemainingToScan;
//		if(noOfBlocksRemainingToScan <= 0 )
//			return false;
//		for(int i = startMemBlkIndex; i<= endMemBlkIndex; i++)
//		{
//			Block* blkPtr = mem.getBlock(i);
//			blkPtr->clear();
//		}
//
//		bool success = relationPtr->getBlocks(currRelBlockIndex, startMemBlkIndex, noOfBlocksToBeReadInCurrentPass);
//		if(success == true)
//		{
//			currentTuples = mem.getTuples(startMemBlkIndex, noOfBlocksToBeReadInCurrentPass);
//			if(currentTuples.size() ==0)
//				return false;
//		}
//		else
//			throw string("Error in scanning table");
//	}
//
//	Tuple t =  currentTuples[currTupleIndex];
//	currTupleIndex ++;
//	return t;
//}

// Move the function below to some new file

void InitializedNewTupleType(Tuple& newTuple, Tuple originalTuple)
{
	Schema schemaNew = newTuple.getSchema();
	Schema schemaOriginal = originalTuple.getSchema();
	vector<string> fieldNames = schemaNew.getFieldNames();
	vector<string>::iterator iter;
	for(iter = fieldNames.begin(); iter != fieldNames.end(); iter++)
	{
		if(schemaOriginal.getFieldType(*iter) == STR20)
			newTuple.setField(*iter, *(originalTuple.getField(*iter).str));
		else
			newTuple.setField(*iter, originalTuple.getField(*iter).integer);
	}

}

//Move the function below to some helper file
Schema GetNewSchema(ProjectOperation projOperation, Relation* relationPtr)
{
	vector<string> newColumnNames;
	vector<FIELD_TYPE> newFieldTypes;
	vector<Column> columns = projOperation.GetColumnList();
	vector<Column>::iterator biter = columns.begin();
	vector<Column>::iterator eiter = columns.end();
	vector<Column>::iterator iter;
	Schema schema = relationPtr->getSchema();

	for(iter = biter; iter != eiter; iter++)
	{
		string columnName = iter->columnName;
		FIELD_TYPE fieldType = schema.getFieldType(columnName);
		if(fieldType!= INT && fieldType != STR20)
			throw string("Error!!!");
	
		newColumnNames.push_back(columnName);
		newFieldTypes.push_back(fieldType);
	}
	return Schema(newColumnNames, newFieldTypes);
}

Schema GetNewSchema(Relation*& relOne, Relation*& relTwo)
{
	string relOneName = relOne->getRelationName();
	string relTwoName = relTwo->getRelationName();
	Schema schemaOne = relOne->getSchema();
	Schema schemaTwo = relTwo->getSchema();
	vector<string> newColumnNames;
	vector<FIELD_TYPE> newFieldTypes;

	vector<string> relOneFieldNames = schemaOne.getFieldNames();
	vector<string>::iterator iter;
	for(iter = relOneFieldNames.begin(); iter != relOneFieldNames.end(); iter++)
	{
		string newn;
		if((*iter).find('.') == string::npos)
			newn = string(relOneName).append(".").append(*iter);
		else
			newn = *iter;

		FIELD_TYPE newft = schemaOne.getFieldType(*iter);

		newColumnNames.push_back(newn);
		newFieldTypes.push_back(newft);
	}

	vector<string> relTwoFieldNames = schemaTwo.getFieldNames();
	for(iter = relTwoFieldNames.begin(); iter != relTwoFieldNames.end(); iter++)
	{
		string newn;
		if((*iter).find('.') == string::npos)
			newn = string(relTwoName).append(".").append(*iter);
		else
			newn = *iter;

		FIELD_TYPE newft = schemaTwo.getFieldType(*iter);

		newColumnNames.push_back(newn);
		newFieldTypes.push_back(newft);
	}
	if(newColumnNames.size() > 8)
		throw string("Too Many Columns");

	return Schema(newColumnNames, newFieldTypes);
}



// This function fetch all the tuples from a relation(disk block) to memory block and
// then read those in a vector of tuples which it returns.
vector<Tuple> TableScan::FetchTuples(string tableName, int memBlockIndex)
{
	vector<Tuple> allTuples;
	Relation *relationPtr = schema_manager.getRelation(tableName);
	Block* blockPtr;
	if(relationPtr == NULL)
		throw string("Relation ").append(tableName).append(" doesn't exist");
	int noOfBlocks = relationPtr->getNumOfBlocks();
	blockPtr = mem.getBlock(memBlockIndex);
	if(blockPtr == NULL)
		throw string("Error!!!");

	for (int i = 0; i<noOfBlocks; i++)
	{
		relationPtr->getBlock(i, memBlockIndex);
		vector<Tuple> t;
		t = blockPtr->getTuples();
		allTuples.insert(allTuples.end(), t.begin(), t.end());
	}
    blockPtr->clear(); //clear the block
	return allTuples;
}

vector<Tuple> TableScan::FetchDistinctTuples(string tableName, int memBlockIndex)
{
	vector<Tuple> OutPutTuples;
	vector<Tuple> BufferTuple;
	Relation *relationPtr = schema_manager.getRelation(tableName);
	Block* blockPtr;
	if(relationPtr == NULL)
		throw string("Relation ").append(tableName).append(" doesn't exist");
	int noOfBlocks = relationPtr->getNumOfBlocks();
	if(noOfBlocks > NUM_OF_BLOCKS_IN_MEMORY - 1)
	{
		vector<Tuple> vt;
		SubListRelation slRelation(tableName, relationPtr->getSchema().getFieldNames());
		
		Tuple t = slRelation.FetchDisctinctTuple();
		while(!t.isNull())
		{
			//cout<<t<<endl;
			vt.push_back(t);
			t = slRelation.FetchDisctinctTuple();
		}
		return vt;
		// Proceed to Two pass distinct algorithm
	}

	//Continue with one pass algorithm
	blockPtr = mem.getBlock(memBlockIndex);
	if(blockPtr == NULL)
		throw string("Error!!!");
	vector<Tuple>::iterator iter;
	for (int i = 0; i<noOfBlocks; i++) //For all the blocks of Relation
	{
		relationPtr->getBlock(i, memBlockIndex);
		vector<Tuple> t;
		t = blockPtr->getTuples();
		for(iter = t.begin(); iter!=t.end(); iter++)
		{
			if(FindTuple(BufferTuple, *iter) == -1)
			{
				BufferTuple.push_back(*iter);
				OutPutTuples.push_back(*iter);
			}
		}
	}
    blockPtr->clear(); //clear the block
	return OutPutTuples;
}


vector<Tuple> TableScan::FetchTuples(bool distinct, string tableName, int memBlockIndex, vector<Column> ordBy)
{
	vector<Tuple> t;
	if(distinct)
	{
		t = FetchDistinctTuples(tableName, memBlockIndex);
	}
	else
	{
		t = FetchTuples(tableName, memBlockIndex);
	}

	if(ordBy.size() != 0)
	{
		t = OrderRelation(t, tableName, ordBy);
	}
	return t;
}


vector<Tuple> TableScan::OrderRelation(vector<Tuple> t, string tableName, vector<Column> ordBy)
{
		if(t.size()== 0) return t;
		
		vector<string> Col;
		vector<Column>::iterator iter;
		for(iter = ordBy.begin(); iter!=ordBy.end(); iter++)
			Col.push_back((*iter).columnName);
		int memBlks = t.size()/t[0].getTuplesPerBlock();
		if(memBlks<NUM_OF_BLOCKS_IN_MEMORY)
		{
			sort(t.begin(), t.end(), CompareTuple(Col));
		}
		else
		{
			Schema sc = schema_manager.getSchema(tableName);
			string tempName = Utility::RandomTableName();
			Relation* relPtr = schema_manager.createRelation(tempName, sc);
			PersistTuplesToDisk(relPtr, t, 0);
			//cout<<*relPtr;
			SubListRelation slr(tempName, Col);
			t.clear();
			Tuple tuple = slr.FetchOrderedTuple();
			while(!tuple.isNull())
			{
				t.push_back(tuple);
				cout<<tuple<<endl;
				tuple = slr.FetchOrderedTuple();
			}
		}
		return t;
}


vector<Tuple> TableScan::FetchTuples(ProjectOperation projection, SelectOperation &selOpn, string tableName, int memBlockIndex, string temporaryRelation)
{
	vector<Tuple> allTuples;
	//Get the relation information
	Relation *relationPtr = schema_manager.getRelation(tableName);
	if(relationPtr == NULL)
		throw string("Relation ").append(tableName).append("doesn't exist");
	Schema newSchema = GetNewSchema(projection, relationPtr);
	Relation* newRelationPtr = schema_manager.createRelation(temporaryRelation, newSchema); 
	Block* blockPtr;
	int noOfBlocks = relationPtr->getNumOfBlocks();
	blockPtr = mem.getBlock(memBlockIndex);
	if(blockPtr == NULL)
		throw string("Error!!!");
	for (int i = 0; i<noOfBlocks; i++)
	{
		relationPtr->getBlock(i, memBlockIndex);
		vector<Tuple> t;
		t = blockPtr->getTuples();
		vector<Tuple>::iterator iterTuple;
		for(iterTuple = t.begin(); iterTuple != t.end(); iterTuple++)
		{
			Tuple t = *iterTuple;
			if(selOpn.IsSearchCondition())
			{
				MapTupleValues(t, tableName);
				bool isValidTuple = selOpn.SearchConditionEvaluate();
				if(isValidTuple)
				{
					Tuple newTypetuple = newRelationPtr->createTuple();
					InitializedNewTupleType(newTypetuple, t);
					allTuples.push_back(newTypetuple);
				}
			}
			else
			{
				Tuple newTypetuple = newRelationPtr->createTuple();
				InitializedNewTupleType(newTypetuple, t);
				allTuples.push_back(newTypetuple);
			}
		} 
	}
    blockPtr->clear(); //clear the block
	return allTuples;
}

vector<Tuple> TableScan::FetchTuplesCorrectMistake(bool isDistinct, ProjectOperation projection, SelectOperation& selOpn, string tableName, int memBlockIndex, string temporaryRelation, vector<Column> OrderBy)
{	
		string TableNameInUse = tableName;
		vector<Tuple> OutPutTuples;
		vector<Tuple> BufferedTuples;
		//Get the relation information
		Relation *relationPtr = schema_manager.getRelation(tableName);
		Schema OriginalSchema = relationPtr->getSchema();
		if(relationPtr == NULL)
			throw string("Relation ").append(tableName).append("doesn't exist");
		Block* blockPtr;
		int noOfBlocks = relationPtr->getNumOfBlocks();
		blockPtr = mem.getBlock(memBlockIndex);
		if(blockPtr == NULL)
			throw string("Error!!!");
		for (int i = 0; i<noOfBlocks; i++)
		{
			relationPtr->getBlock(i, memBlockIndex);
			vector<Tuple> t;
			t = blockPtr->getTuples();
			vector<Tuple>::iterator iterTuple;
			for(iterTuple = t.begin(); iterTuple != t.end(); iterTuple++)
			{
				Tuple t = *iterTuple;
				if(selOpn.IsSearchCondition())
				{
					MapTupleValues(t, tableName);
					bool isValidTuple = selOpn.SearchConditionEvaluate();
					if(isValidTuple)
					{
						OutPutTuples.push_back(t);
					}
				}
				else
				{
					OutPutTuples.push_back(t);
				}
			} 
		}
	
		//now buffered tuples has all the selected tuples
		if(isDistinct)
		{
				int memBlkSize = OutPutTuples.size()/OutPutTuples[0].getTuplesPerBlock();
				if(memBlkSize < NUM_OF_BLOCKS_IN_MEMORY) //Pipelined
				{
					BufferedTuples.clear();
					vector<Tuple> buffTuple;
					vector<Tuple>::iterator iter1;
					for(iter1 = OutPutTuples.begin(); iter1 != OutPutTuples.end(); iter1++)
					{
						if(FindTuple(buffTuple, *iter1, projection.GetColumnListString()) == -1)
						{
							buffTuple.push_back(*iter1);
							BufferedTuples.push_back(*iter1);
						}
					}
					OutPutTuples.clear();
					OutPutTuples = BufferedTuples;
				}
				else
				{
					string Name = Utility::RandomTableName();
					TableNameInUse = Name;
					Relation* rel = schema_manager.createRelation(Name, OriginalSchema);
					PersistTuplesToDisk(rel, OutPutTuples, 0);
					SubListRelation sl(Name, projection.GetColumnListString());
					OutPutTuples.clear();
					Tuple t = sl.FetchDisctinctTuple();
					while(!t.isNull())
					{
						OutPutTuples.push_back(t);
						t = sl.FetchDisctinctTuple();
					}
				}
		}

		if(OrderBy.size() != 0)
		{
				int memBlkSize = OutPutTuples.size()/OutPutTuples[0].getTuplesPerBlock();
				vector<string> orByL;
				vector<Column>::iterator iter;
				for(iter = OrderBy.begin(); iter!=OrderBy.end(); iter++)
					orByL.push_back((*iter).columnName);
				if(memBlkSize < NUM_OF_BLOCKS_IN_MEMORY)
				{
					sort(OutPutTuples.begin(), OutPutTuples.end(), CompareTuple(orByL));
				}
				else
				{
					string Name = Utility::RandomTableName();
					TableNameInUse = Name;
					Relation* rel = schema_manager.createRelation(Name, OriginalSchema);
					PersistTuplesToDisk(rel, OutPutTuples, 0);
					SubListRelation sl(Name, orByL);
					OutPutTuples.clear();
					Tuple t = sl.FetchOrderedTuple();
					while(!t.isNull())
					{
						OutPutTuples.push_back(t);
						t = sl.FetchOrderedTuple();
					}
				}
		}
		//Now perform the projection
			int memBlkSize = OutPutTuples.size()/OutPutTuples[0].getTuplesPerBlock();
			Schema newSchema = GetNewSchema(projection, relationPtr);
			//string tbName = Utility::RandomTableName();
			Relation* newRelationPtr = schema_manager.createRelation(temporaryRelation, newSchema); 
			vector<Tuple>::iterator iter;
			BufferedTuples.clear();
			
			for(iter = OutPutTuples.begin(); iter != OutPutTuples.end(); iter++)
			{
				Tuple t = *iter;
				Tuple newTypetuple = newRelationPtr->createTuple();
				InitializedNewTupleType(newTypetuple, t);
				BufferedTuples.push_back(newTypetuple);
			} 
			
		//blockPtr->clear(); //clear the block
		return BufferedTuples;
}
//Replicating the code
//This code is redundant and can be refactored later
//Writing this just not to interfare with previous functionality
vector<Tuple> TableScan::FetchTuples(bool isDistinct, ProjectOperation projection, SelectOperation& selOpn, string tableName, int memBlockIndex, string temporaryRelation, vector<Column> ordBy)
{
	return FetchTuplesCorrectMistake(isDistinct, projection, selOpn, tableName, 0, temporaryRelation, ordBy);
	//if(isDistinct == false)
	//{
	//	return FetchTuples(projection, selOpn, tableName, memBlockIndex, temporaryRelation);
	//}
	//else
	//{
	//	vector<Tuple> OutPutTuples;
	//	vector<Tuple> BufferedTuples;
	//	//Get the relation information
	//	Relation *relationPtr = schema_manager.getRelation(tableName);
	//	if(relationPtr == NULL)
	//		throw string("Relation ").append(tableName).append("doesn't exist");
	//	Schema newSchema = GetNewSchema(projection, relationPtr);
	//	Relation* newRelationPtr = schema_manager.createRelation(temporaryRelation, newSchema); 
	//	Block* blockPtr;
	//	int noOfBlocks = relationPtr->getNumOfBlocks();
	//	if(noOfBlocks > NUM_OF_BLOCKS_IN_MEMORY - 1)
	//	{

	//		return FetchTuplesCorrectMistake(isDistinct, projection, selOpn, tableName, 0, temporaryRelation, ordBy);
	//		
	//	}
	//	blockPtr = mem.getBlock(memBlockIndex);
	//	if(blockPtr == NULL)
	//		throw string("Error!!!");
	//	for (int i = 0; i<noOfBlocks; i++)
	//	{
	//		relationPtr->getBlock(i, memBlockIndex);
	//		vector<Tuple> t;
	//		t = blockPtr->getTuples();
	//		vector<Tuple>::iterator iterTuple;
	//		for(iterTuple = t.begin(); iterTuple != t.end(); iterTuple++)
	//		{
	//			Tuple t = *iterTuple;
	//			if(selOpn.IsSearchCondition())
	//			{
	//				MapTupleValues(t, tableName);
	//				bool isValidTuple = selOpn.SearchConditionEvaluate();
	//				if(isValidTuple)
	//				{
	//					Tuple newTypetuple = newRelationPtr->createTuple();
	//					InitializedNewTupleType(newTypetuple, t);
	//					if(FindTuple(BufferedTuples, newTypetuple) == -1)
	//					{
	//						BufferedTuples.push_back(newTypetuple);
	//						OutPutTuples.push_back(newTypetuple);
	//					}
	//				}
	//			}
	//			else
	//			{
	//				Tuple newTypetuple = newRelationPtr->createTuple();
	//				InitializedNewTupleType(newTypetuple, t);
	//				if(FindTuple(BufferedTuples, newTypetuple) == -1)
	//				{
	//					BufferedTuples.push_back(newTypetuple);
	//					OutPutTuples.push_back(newTypetuple);
	//				}
	//			}
	//		} 
	//	}
	//	blockPtr->clear(); //clear the block
	//	return OutPutTuples;
	//}
}

vector<Tuple> TableScan::FetchSelectedTuples(string tableName, int memBlockIndex, SelectOperation& selOp)
{
	vector<Tuple> selectedTuples;
	vector<Column> ordBy;

	vector<Tuple> allTuples = FetchTuples(false, tableName, memBlockIndex, ordBy);

	vector<Tuple>::iterator iter;

	for(iter = allTuples.begin(); iter!=allTuples.end(); iter++)
	{
		Tuple t = *iter;
		MapTupleValues(t, tableName);
		bool isValidTuple = selOp.SearchConditionEvaluate();
		if(isValidTuple)
			selectedTuples.push_back(t);
	}
	return selectedTuples;
}


vector<Tuple> TableScan::FetchSelectedTuples(bool isDistinct, string tableName, int memBlockIndex, SelectOperation& selOp, vector<Column> ordBy)
{
	string tempTable;
	Schema originalSchema = schema_manager.getSchema(tableName);
	vector<Tuple> outPutTuple =  FetchSelectedTuples(tableName, memBlockIndex, selOp);
	int memSize = 0;
	if(outPutTuple.size()!=0)
		memSize = outPutTuple.size()/outPutTuple[0].getTuplesPerBlock();
	if(isDistinct)
	{
		if(memSize > NUM_OF_BLOCKS_IN_MEMORY)
		{
			tempTable = Utility::RandomTableName();
			Relation* rel = schema_manager.createRelation(tempTable, originalSchema);
			PersistTuplesToDisk(rel, outPutTuple, 0);
			outPutTuple.clear();
			outPutTuple = FetchTuples(true, tempTable, 0, ordBy);
		}
		else
		{
			vector<Tuple> buffer;
			vector<Tuple> oBuffer;
			vector<Tuple>::iterator iter;
			for(iter = outPutTuple.begin(); iter!=outPutTuple.end(); iter++)
			{
				if(FindTuple(buffer, *iter) == -1)
				{
					buffer.push_back(*iter);
					oBuffer.push_back(*iter);
				}
			}
			outPutTuple.clear();
			outPutTuple = oBuffer;
		}

	}


	if(ordBy.size() > 0)
	{
		outPutTuple = OrderRelation(outPutTuple, tableName, ordBy);
	}
	
	return outPutTuple;
}

Tuple ConcatenateTuple(Tuple leftTuple, Tuple rightTuple, Relation*& newRelPtr, Relation*& leftRelPtr, Relation*& rightRelPtr)
{
	Schema leftSchema = leftTuple.getSchema();
	Schema rightSchema = rightTuple.getSchema();

	bool isLeftGenerated = false;
	bool isRightGenerated = false;

	//Check whether the column names in left or right schema has '.' character in it
	vector<string> leftFieldNames = leftSchema.getFieldNames();
	vector<string> rightFieldNames = rightSchema.getFieldNames();

	if(leftFieldNames[0].find('.') != string::npos)
		isLeftGenerated = true;
	if(rightFieldNames[0].find('.') != string::npos)
		isRightGenerated = true;

	Tuple newT = newRelPtr->createTuple();

	vector<string>::iterator iter;
	//Fill the left tuple values
	for(iter = leftFieldNames.begin(); iter!= leftFieldNames.end(); iter++)
	{
		//Column Name where tuple value needs to be fit in
		string colName;
		if(isLeftGenerated)
			colName = *iter;
		else
			colName = string(leftRelPtr->getRelationName()).append(".").append(*iter);

		if(leftSchema.getFieldType(*iter) == STR20)
		{
			string value = *(leftTuple.getField(*iter).str);
			newT.setField(colName, value);
		}
		else
		{
			int value = leftTuple.getField(*iter).integer;
			newT.setField(colName, value);
		}
	}

	//Fill the right tuple values
	for(iter = rightFieldNames.begin(); iter!= rightFieldNames.end(); iter++)
	{
		//Column Name where tuple value needs to be fit in
		string colName;
		if(isRightGenerated)
			colName = *iter;
		else
			colName = string(rightRelPtr->getRelationName()).append(".").append(*iter);

		if(rightSchema.getFieldType(*iter) == STR20)
		{
			string value = *(rightTuple.getField(*iter).str);
			newT.setField(colName, value);
		}
		else
		{
			int value = rightTuple.getField(*iter).integer;
			newT.setField(colName, value);
		}
	}

	return newT;
}

vector<Tuple> TableScan::FetchProduct(string table1, string table2, string tempTable, bool persisToDB)
{
	vector<Tuple> concatTuples;
	Relation* s_RelPtr = schema_manager.getRelation(table1);
	Relation* r_RelPtr = schema_manager.getRelation(table2);
	Relation *bulkRel,*singlefetchRel; 
	bool isOnePass = false;
	int s_size = s_RelPtr->getNumOfBlocks();
	int r_size = r_RelPtr->getNumOfBlocks();

	int small_size;
	int big_size;
	if(r_size<s_size)
	{
		bulkRel = r_RelPtr;
		singlefetchRel = s_RelPtr;
		small_size = r_size;
		big_size = s_size;
	}
	else
	{
		bulkRel = s_RelPtr;
		singlefetchRel = r_RelPtr;
		small_size = s_size;
		big_size = r_size;
	}

	Schema newSchema = GetNewSchema(bulkRel, singlefetchRel);
	Relation* newRelPtr = schema_manager.createRelation(tempTable, newSchema);
	bool lastPass = false;
	int startIndex = 0;
	while(1)
	{
		if(lastPass) break;
		
		int blocksToFetch;

		if(small_size<NUM_OF_BLOCKS_IN_MEMORY - 1 )
		{ 
			lastPass = true;
			blocksToFetch = small_size;
		}
		else
		   blocksToFetch = NUM_OF_BLOCKS_IN_MEMORY - 1;

		//1. Read the blocks of smaller relation
		bulkRel->getBlocks(startIndex, 1, blocksToFetch);
		//2. Reading all the tuples in the a vector - it will help in iteration
		vector<Tuple> smaller_rel_tuples = mem.getTuples(1, blocksToFetch);
		vector<Tuple>::iterator small_iter;
		int blocks_to_read = big_size; // No. of blocks in bigger relation
		int index = 0; // index for bigger relation
		while(blocks_to_read > 0) //while all blocks for the relation are not read
		{
			//Fetch a single block from the bigger relation in mem_index 0
			singlefetchRel->getBlock(index, 0); 
			vector<Tuple> bigger_rel_tuples = mem.getTuples(0, 1);
			//for each tuple in bigger_rel_tuples concatenate with all tuples in smaller_rel_tuples
			vector<Tuple>::iterator big_iter;
			for(big_iter = bigger_rel_tuples.begin(); big_iter!= bigger_rel_tuples.end(); big_iter++)
			{
				for(small_iter = smaller_rel_tuples.begin(); small_iter!= smaller_rel_tuples.end(); small_iter++)
				{
					
					Tuple t = ConcatenateTuple(*small_iter, *big_iter, newRelPtr, bulkRel, singlefetchRel); 
					concatTuples.push_back(t);
				}
			}
			blocks_to_read--;
			index++;
		}

		small_size = small_size - blocksToFetch;
		startIndex = startIndex + blocksToFetch;
	}

	if(persisToDB)
	{
		//cout<<"Write the Intermediate result to DB";
		PersistTuplesToDisk(newRelPtr, concatTuples, 0); //use memory index 0 to persist the tuples
	}
	return concatTuples;
}

void PersistTuplesToDisk(Relation*& relationPtr, vector<Tuple> tuples, int memoryIndex) 
{
	if(tuples.size() == 0) return;
	bool success = true;
	Block* blockPtr = mem.getBlock(memoryIndex);
	blockPtr->clear();
	Schema schema = relationPtr->getSchema();
	int noOfTuplesPerBlock = schema.getTuplesPerBlock();
	int relblockIndex = 0; //Where to insert the block in relation

	vector<Tuple>::iterator iter;
	for(iter = tuples.begin(); iter!=tuples.end(); iter++)
	{
		blockPtr->appendTuple(*iter);
		if(blockPtr->isFull())//if the block in full flush it to memory
		{
			relationPtr->setBlock(relblockIndex, memoryIndex);
			blockPtr->clear();
			relblockIndex++;
		}
	}

	//Flush out the last few tuples 
	if(!blockPtr->isEmpty())
	{
		relationPtr->setBlock(relblockIndex, memoryIndex);
	}

	//cout<<endl<<"Relation: "<<relationPtr->getRelationName()<<endl;
	//cout<<*relationPtr<<endl;
}

Schema GetNewSchema(ProjectOperation projectOpn, ProductOperation prodOpn, Relation*& relPtrOld)
{
	vector<string> tables = prodOpn.GetFromList();
	vector<Column> columns = projectOpn.GetColumnList();

	vector<Schema> schemas;

	vector<string> newColumnNames;
	vector<FIELD_TYPE> newFieldTypes;

	vector<Column>::iterator iter;
	Schema schema = relPtrOld->getSchema();

	vector<string>::iterator tabIter;
	for(tabIter = tables.begin(); tabIter != tables.end(); tabIter++)
	{
		schemas.push_back(schema_manager.getSchema(*tabIter));
	}
	vector<Schema>::iterator schemaIter;

	vector<Column>::iterator biter = columns.begin();
	vector<Column>::iterator eiter = columns.end();
	
	for(iter = biter; iter != eiter; iter++)
	{
		string columnName = iter->columnName;
		string tableName = iter->tableName;
		if(tableName == NO_TABLE) //if table name is not give
		{
			int noOfTab = 0;
			tabIter = tables.begin();
			schemaIter = schemas.begin();
			while(tabIter != tables.end())
			{
				if((*schemaIter).fieldNameExists(columnName))
				{
					tableName = *tabIter;
					noOfTab++;
				}
				tabIter++;
				schemaIter++;
			}
			if(noOfTab != 1)
				throw string("Multiple or No Relation(Schema) Match for Column ").append(string(columnName));
		}
		string colName; // Concatenate it with table name and column name
		colName = tableName;
		colName.append(".");
		colName.append(columnName);
		FIELD_TYPE fieldType = schema.getFieldType(colName);
		if(fieldType!= INT && fieldType != STR20)
			throw string("Error!!!");
	
		newColumnNames.push_back(colName);
		newFieldTypes.push_back(fieldType);
	}
	return Schema(newColumnNames, newFieldTypes);
}

ReturnRelation TableScan::ApplyProjectOperation(ProjectOperation projectOpn, ProductOperation prodOpn,
					ReturnRelation retRelation, bool persist)
{
	ReturnRelation returnRelation;
	vector<Tuple> tuples = retRelation.tuples;
	Relation *relPtrOld = schema_manager.getRelation(retRelation.relationName);
	string tempTable = Utility::RandomTableName();
	returnRelation.relationName = tempTable;
	returnRelation.markedForDeletion = true;
	Schema newSchema = GetNewSchema(projectOpn, prodOpn, relPtrOld);
	Relation* newRelationPtr = schema_manager.createRelation(tempTable, newSchema);

	vector<Tuple>::iterator tupleIter;

	for(tupleIter = tuples.begin(); tupleIter != tuples.end(); tupleIter++)
	{
		Tuple t = newRelationPtr->createTuple();
		InitializedNewTupleType(t, *tupleIter);
		returnRelation.tuples.push_back(t);
	}

	return returnRelation; 
}

string TableScan::GetTableName(string columnName)
{
	if(GlobalFromTables.size() == 0)
		throw string("No table availables to verify the columns against");

	vector<string>::iterator tabIter;

	vector<Schema> schemas;

	int count = 0;
	string tableName;
	for(tabIter = GlobalFromTables.begin(); tabIter != GlobalFromTables.end(); tabIter++)
	{
		Schema s = schema_manager.getSchema(*tabIter);
		if(s.fieldNameExists(columnName))
		{
			tableName = *tabIter;
			count++;
		}
	}
	if(count == 1)
		return tableName;
	else
		throw string("Multiple or No table matches to column name :").append(string(columnName)); 

}

vector<Tuple> TableScan::ApplySelectCondition(SelectOperation& selectOperation, ReturnRelation retRelation)
{
	vector<Tuple> selectedTuples;
	vector<Tuple> tuples = retRelation.tuples;

	vector<Tuple>::iterator tupleIter;

	for(tupleIter = tuples.begin(); tupleIter != tuples.end(); tupleIter++)
	{
		Tuple t = *tupleIter;
		MapTupleValues(t);
		bool isValidTuple = selectOperation.SearchConditionEvaluate();
		if(isValidTuple)
			selectedTuples.push_back(t);
	}
	return selectedTuples; 
}

SubListRelation::SubListRelation(string relName, vector<string> col)
{
	relationName = relName;
	noOfSublists = 0;
	memQVecSet = false;
	relationPtr = schema_manager.getRelation(relName);
	relSchema = relationPtr->getSchema();
	if(col.size() == 0)
		columns = relSchema.getFieldNames();
	else
		columns = col;
	noOfBlocks = relationPtr->getNumOfBlocks();
	
	int remBlocksToRead = noOfBlocks;
	int indexToRead = 0;
	while(remBlocksToRead > 0)
	{
		int mainMemIndex = 0;
		if(remBlocksToRead > NUM_OF_BLOCKS_IN_MEMORY)
		{
			//Scan the relation from start index to startIndex + NUM_OF_BLOCKS_IN_MEMORY - 1
			relationPtr->getBlocks(indexToRead, 0, NUM_OF_BLOCKS_IN_MEMORY);
			MMSorting(0, NUM_OF_BLOCKS_IN_MEMORY, columns);
			relationPtr->setBlocks(indexToRead, 0, NUM_OF_BLOCKS_IN_MEMORY);
//			cout<<*relationPtr;
			sublitsIndex.push_back(indexToRead);
			noOfSublists++;
			indexToRead = indexToRead + NUM_OF_BLOCKS_IN_MEMORY;
			remBlocksToRead = remBlocksToRead - NUM_OF_BLOCKS_IN_MEMORY;
		}
		else
		{
			relationPtr->getBlocks(indexToRead, 0, remBlocksToRead);
			MMSorting(0, remBlocksToRead, columns);
			relationPtr->setBlocks(indexToRead, 0, remBlocksToRead);
			sublitsIndex.push_back(indexToRead);
			noOfSublists++;
			indexToRead = indexToRead + remBlocksToRead;
			noOflastListBlocks = remBlocksToRead;
			remBlocksToRead = 0; //terminating condition
		}
	}
}

Tuple SubListRelation::FetchDisctinctTuple()
{
	if(memQVecSet== false)
		//Set all the memQs
	{
		memQueueVector = vector<deque<Tuple>>(noOfSublists); //Maintains the queue of tuple for each sublist in Main Memory
		//cout<<mem<<endl;
		//cout<<*relationPtr<<endl;
		for(int i=0; i<noOfSublists; i++)
		{
			relativeIndexMapForBlock_i[i] = 0; //For each sublist maintains the relative location where we need to fetch next
			int index = sublitsIndex[i] + relativeIndexMapForBlock_i[i]; //Absolute location of block in Relation from where to fetch next
			relationPtr->getBlock(index, i); //Fetch from the absolute location in memmory index identified by i
			relativeIndexMapForBlock_i[i] = relativeIndexMapForBlock_i[i] + 1;//Increasing the next relative location
			Block* blkPtr = mem.getBlock(i);
			//cout<<mem<<endl;
			vector<Tuple> tuples = blkPtr->getTuples();
			vector<Tuple>::iterator iter;
			for(iter = tuples.begin(); iter != tuples.end(); iter++)
				memQueueVector[i].push_back(*iter);
		}
		memQVecSet = true;
	}

	Tuple nullt = relationPtr->createTuple();
	nullt.null();
	//Create a temporary tuple vector
	vector<Tuple> tupleToCompare;
	for(int i=0; i<noOfSublists; i++)
	{
		tupleToCompare.push_back(nullt);
	}

	for(int i=0; i<noOfSublists; i++)
	{
		bool validTuple = true;
		if(memQueueVector[i].empty())
		{
			//If Queue is empty now
			//Then try to fetch the new block
			//Find absolute index of block to fetch
			if(i == noOfSublists - 1) //if i is the lat sublist
			{
				if(relativeIndexMapForBlock_i[i] == noOflastListBlocks)
					tupleToCompare[i].null();
				validTuple = false;
			}
			else if(relativeIndexMapForBlock_i[i] == NUM_OF_BLOCKS_IN_MEMORY)
			{
				tupleToCompare[i].null();
				validTuple = false;
			}
			else
			{
				int index = sublitsIndex[i] + relativeIndexMapForBlock_i[i];
				relativeIndexMapForBlock_i[i] = relativeIndexMapForBlock_i[i] + 1;
				relationPtr->getBlock(index, i);
				Block* blkPtr = mem.getBlock(i);
				vector<Tuple> tuples = blkPtr->getTuples();
				vector<Tuple>::iterator iter;
				for(iter = tuples.begin(); iter != tuples.end(); iter++)
					memQueueVector[i].push_back(*iter);
				validTuple = true;
			}
		}

		if(validTuple)
		{
			tupleToCompare[i] = memQueueVector[i].front();
			//memQueueVector[i].pop_front();
		}

	}

	Tuple t = FindMinTuple(tupleToCompare, columns);
	if(!t.isNull())
	{
		for(int i=0; i<noOfSublists; i++)
		{
			 PopAll(i, t);
		}
	}
	return t;
}


bool SubListRelation::FetchQueue(int i)
{
	if(i == noOfSublists - 1) //if i is the last sublist
	{
		if(relativeIndexMapForBlock_i[i] == noOflastListBlocks)
	    	return false;
	}
	else if(relativeIndexMapForBlock_i[i] == NUM_OF_BLOCKS_IN_MEMORY)
	{
		return false;
	}
	int index = sublitsIndex[i] + relativeIndexMapForBlock_i[i];
	relativeIndexMapForBlock_i[i] = relativeIndexMapForBlock_i[i] + 1;
	relationPtr->getBlock(index, i);
	Block* blkPtr = mem.getBlock(i);
	vector<Tuple> tuples = blkPtr->getTuples();
	vector<Tuple>::iterator iter;
	for(iter = tuples.begin(); iter != tuples.end(); iter++)
		memQueueVector[i].push_back(*iter);
}

void SubListRelation::PopAll(int i, Tuple t)
{
   for(;;)
   {
	   if(memQueueVector[i].empty())
	   {
		   bool fetched = FetchQueue(i);
		   if(fetched == false)
			   return;
	   }
	   while(!memQueueVector[i].empty())
	   {
		   Tuple t2 = memQueueVector[i].front();
		   if(IsEqual(t, t2, columns))
		   {
			   memQueueVector[i].pop_front();
		   }
		   else
			   return;
	   }
   }
   cout<<"Infinite"<<endl;
}

void SubListRelation::Pop(int i)
{
	 memQueueVector[i].pop_front();
	 if(memQueueVector[i].empty())
	 {
		 bool fetched = FetchQueue(i);
		 if(fetched == false)
			  return;
	 }
}

Tuple SubListRelation::FetchOrderedTuple()
{
	if(memQVecSet== false)
		//Set all the memQs
	{
		memQueueVector = vector<deque<Tuple>>(noOfSublists); //Maintains the queue of tuple for each sublist in Main Memory
		//cout<<mem<<endl;
		//cout<<*relationPtr<<endl;
		for(int i=0; i<noOfSublists; i++)
		{
			relativeIndexMapForBlock_i[i] = 0; //For each sublist maintains the relative location where we need to fetch next
			int index = sublitsIndex[i] + relativeIndexMapForBlock_i[i]; //Absolute location of block in Relation from where to fetch next
			relationPtr->getBlock(index, i); //Fetch from the absolute location in memmory index identified by i
			relativeIndexMapForBlock_i[i] = relativeIndexMapForBlock_i[i] + 1;//Increasing the next relative location
			Block* blkPtr = mem.getBlock(i);
			//cout<<mem<<endl;
			vector<Tuple> tuples = blkPtr->getTuples();
			vector<Tuple>::iterator iter;
			for(iter = tuples.begin(); iter != tuples.end(); iter++)
				memQueueVector[i].push_back(*iter);
		}
		memQVecSet = true;
	}

	Tuple nullt = relationPtr->createTuple();
	nullt.null();
	//Create a temporary tuple vector
	vector<Tuple> tupleToCompare;
	for(int i=0; i<noOfSublists; i++)
	{
		tupleToCompare.push_back(nullt);
	}

	for(int i=0; i<noOfSublists; i++)
	{
		bool validTuple = true;
		if(memQueueVector[i].empty())
		{
			//If Queue is empty now
			//Then try to fetch the new block
			//Find absolute index of block to fetch
			if(i == noOfSublists - 1) //if i is the lat sublist
			{
				if(relativeIndexMapForBlock_i[i] == noOflastListBlocks)
					tupleToCompare[i].null();
				validTuple = false;
			}
			else if(relativeIndexMapForBlock_i[i] == NUM_OF_BLOCKS_IN_MEMORY)
			{
				tupleToCompare[i].null();
				validTuple = false;
			}
			else
			{
				int index = sublitsIndex[i] + relativeIndexMapForBlock_i[i];
				relativeIndexMapForBlock_i[i] = relativeIndexMapForBlock_i[i] + 1;
				relationPtr->getBlock(index, i);
				Block* blkPtr = mem.getBlock(i);
				vector<Tuple> tuples = blkPtr->getTuples();
				vector<Tuple>::iterator iter;
				for(iter = tuples.begin(); iter != tuples.end(); iter++)
					memQueueVector[i].push_back(*iter);
				validTuple = true;
			}
		}

		if(validTuple)
		{
			tupleToCompare[i] = memQueueVector[i].front();
			//memQueueVector[i].pop_front();
		}

	}

	Tuple t = FindMinTuple(tupleToCompare, columns);
	if(!t.isNull())
	{
		int pos = FindTuple(tupleToCompare, t, columns);
		if(pos != -1)
			Pop(pos);
	}
	
	return t;
}



vector<Tuple> TableScan::FetchTuplesDistinctOrdered(vector<Tuple> Tuples, bool isDistinct, ProjectOperation projection, string tableName, int memBlockIndex, vector<Column> OrderBy)
{

		string TableNameInUse = tableName;
		vector<Tuple> OutPutTuples = Tuples;
		vector<Tuple> BufferedTuples;
		//Get the relation information
		Relation *relationPtr = schema_manager.getRelation(tableName);
		Schema OriginalSchema = relationPtr->getSchema();
		if(relationPtr == NULL)
			throw string("Relation ").append(tableName).append("doesn't exist");
		bool allSelection = false;
		vector<string>fieldNames;
		if(projection.GetColumnList().front().columnName == "*")
		{
			allSelection = true;
		}
		if(allSelection)
		{
			fieldNames = OriginalSchema.getFieldNames();
		}
		else
		{
			fieldNames = projection.GetColumnListStringWithTable();
		}
		vector<string> ColumnList;
		//if(allSelection)
		//	ColumnList = OriginalSchema.getFieldNames();
		//else
		int noOfBlocks = relationPtr->getNumOfBlocks();
	
		//now buffered tuples has all the selected tuples
		if(isDistinct)
		{
				int memBlkSize = OutPutTuples.size()/OutPutTuples[0].getTuplesPerBlock();
				if(memBlkSize < NUM_OF_BLOCKS_IN_MEMORY) //Pipelined
				{
					BufferedTuples.clear();
					vector<Tuple> buffTuple;
					vector<Tuple>::iterator iter1;
					for(iter1 = OutPutTuples.begin(); iter1 != OutPutTuples.end(); iter1++)
					{
						if(allSelection)
						{
							if(FindTuple(buffTuple, *iter1) == -1)
							{
								buffTuple.push_back(*iter1);
								BufferedTuples.push_back(*iter1);
							}
						}
						else
						{
							if(FindTuple(buffTuple, *iter1, projection.GetColumnListStringWithTable()) == -1)
							{
								buffTuple.push_back(*iter1);
								BufferedTuples.push_back(*iter1);
							}
						}
	
					}
					OutPutTuples.clear();
					OutPutTuples = BufferedTuples;
				}
				else
				{
					string Name = Utility::RandomTableName();
					TableNameInUse = Name;
					Relation* rel = schema_manager.createRelation(Name, OriginalSchema);
					
					PersistTuplesToDisk(rel, OutPutTuples, 0);
					//cout<<*rel<<endl;
					
					SubListRelation sl(Name, fieldNames);
					
					OutPutTuples.clear();
					Tuple t = sl. FetchDisctinctTuple();
					while(!t.isNull())
					{
						OutPutTuples.push_back(t);
						t = sl.FetchDisctinctTuple();
					}
				}
		}

		if(OrderBy.size() != 0)
		{
				int memBlkSize = OutPutTuples.size()/OutPutTuples[0].getTuplesPerBlock();
				vector<string> orByL;
				vector<Column>::iterator iter;
				for(iter = OrderBy.begin(); iter!=OrderBy.end(); iter++)
				{
					string tabName = (*iter).tableName;
					if((*iter).tableName == NO_TABLE)
						tabName = TableScan::GetTableName((*iter).columnName);
					orByL.push_back(string(tabName).append(".").append((*iter).columnName));
				}
				if(memBlkSize < NUM_OF_BLOCKS_IN_MEMORY)
				{

					sort(OutPutTuples.begin(), OutPutTuples.end(), CompareTuple(orByL));
					
				}
				else
				{
					string Name = Utility::RandomTableName();
					TableNameInUse = Name;
					Relation* rel = schema_manager.createRelation(Name, OriginalSchema);
					PersistTuplesToDisk(rel, OutPutTuples, 0);
					SubListRelation sl(Name, orByL);
					OutPutTuples.clear();
					Tuple t = sl.FetchOrderedTuple();
					int i = 0;
					while(!t.isNull())
					{
						OutPutTuples.push_back(t);
						t = sl.FetchOrderedTuple();
						
					}
				}
		}
		
		return OutPutTuples;

}