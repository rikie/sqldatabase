#include "SqlWorker.h"

SqlWorker::SqlWorker()
{
	//cout<<"SqlWorker Initialized"<<endl;
}

bool SqlWorker::CreateTable(string tableName, vector<string> attributeNameList, vector<enum FIELD_TYPE> dataTypeList)
{
	//1. Create schema : Schema schema(field_names,field_types);
	Schema schema(attributeNameList,dataTypeList);

	//2. Create a relation with the created schema through the schema manager
	Relation* relation_ptr=schema_manager.createRelation(tableName,schema);
	if(relation_ptr == NULL)
	{
		cout<<"Table Creation FAILED!!!!"<<endl;
		return false;
	}
	cout<<"Table: "<<tableName<<" created sucessfully"<<endl;
	return true;
}

bool SqlWorker::InitializeTuple(Tuple& tuple, vector<string> attributeList, vector<string> valueList)
{
	bool success = true;
	vector<string>::iterator attIter;
	vector<string>::iterator valueIter;
	Schema tupleSchema = tuple.getSchema();
	int i = 0;
	for(attIter = attributeList.begin(), valueIter = valueList.begin(); attIter != attributeList.end(); attIter++, valueIter++)
	{
		if(tupleSchema.getFieldType(*attIter) == STR20)
		{
			string value = *valueIter;
			if(*valueIter ==  "NULL")
				value = "";
			success = tuple.setField(*attIter, value);
		}
		else
		{
			int value;
			if(*valueIter == "NULL")
			{
				success = tuple.setField(*attIter, 0);
			}
			else 
			{
				bool alldigit = true;
				for(int i=0; i<(*valueIter).length(); i++)
					if(!isdigit((*valueIter).at(i)))
						alldigit = false;

				if(alldigit)
				{
					value = atoi((*valueIter).c_str());
					success = tuple.setField(*attIter, value);
				}
				else
					success = tuple.setField(*attIter, *valueIter);
			}
			
		}
		if(!success) 
			return success;
		i++;
	}
	return success;
}

bool SqlWorker::InsertTuple(string tableName, vector<string> attributeList, vector<string> valueList)
{
	bool success = true;
	//Note: tuple should not exceed on block of memory i.e 8 fields
	//Perform following steps
	//1. Get Relation from schema using <tableName>
	Relation* relation = schema_manager.getRelation(tableName);
	if(relation == NULL) return false;
	//2. Create an empty tuple in that relation and get a reference to that tuple
	Tuple tuple = relation->createTuple();
	//3. Set the fields in the tuples using <attributeList> and <valueList>
	success = InitializeTuple(tuple, attributeList, valueList);
	if(!success) return success;
	//4. Setup a block in memory (usually the 0th block)
	//5. Clear this memory block
	//6. Set or append tuple to this block
	//Above three steps are performed be the function call below
	success = AppendTupleToRelation(relation, 0, tuple);
	cout<<"One row is Inserted in "<<tableName<<endl;
	return success;
}

//TA's code
// An example procedure of appending a tuple to the end of a relation
// using memory block "memory_block_index" as output buffer
bool SqlWorker::AppendTupleToRelation(Relation* relation_ptr, int memory_block_index, Tuple& tuple) 
{
	bool success = true;
	Block* block_ptr;
	if (relation_ptr->getNumOfBlocks()==0) 
	{
		//fout << "The relation is empty" << endl;
		//fout << "Get the handle to the memory block " << memory_block_index << " and clear it" << endl;
		block_ptr=mem.getBlock(memory_block_index);
		block_ptr->clear(); //clear the block
		success = block_ptr->appendTuple(tuple); // append the tuple
		if(!success) return success;
		//fout << "Write to the first block of the relation" << endl;
		success = relation_ptr->setBlock(relation_ptr->getNumOfBlocks(),memory_block_index);
		if(!success) return success;
	} 
	else 
	{
		//fout << "Read the last block of the relation into memory block 5:" << endl;
		success = relation_ptr->getBlock(relation_ptr->getNumOfBlocks()-1,memory_block_index);
		if(!success) return success;
		block_ptr=mem.getBlock(memory_block_index);

		if (block_ptr->isFull()) 
		{
			//fout << "(The block is full: Clear the memory block and append the tuple)" << endl;
			block_ptr->clear(); //clear the block
			success = block_ptr->appendTuple(tuple); // append the tuple
			if(!success) return success;
			//fout << "Write to a new block at the end of the relation" << endl;
			success = relation_ptr->setBlock(relation_ptr->getNumOfBlocks(),memory_block_index); //write back to the relation
			if(!success) return success;
		} 
		else 
		{
			//fout << "(The block is not full: Append it directly)" << endl;
			block_ptr->appendTuple(tuple); // append the tuple
			//fout << "Write to the last block of the relation" << endl;
			relation_ptr->setBlock(relation_ptr->getNumOfBlocks()-1,memory_block_index); //write back to the relation
		}
	}  
	return success;
}

void ConsoleOutput(ReturnRelation retRelation)
{
	cout<<endl;
	string relation = retRelation.relationName;
	Schema schema = schema_manager.getSchema(relation);
	vector<string> field_names = schema.getFieldNames();
	copy(field_names.begin(),field_names.end(),ostream_iterator<string,char>(cout,"\t"));
	cout<<endl;
	cout<<" - - - - - - - - - - - - - - - - - - - - - - - - -  "<<endl;
	vector<Tuple>::iterator iter;
	for(iter = retRelation.tuples.begin(); iter != retRelation.tuples.end(); iter++)
	cout<<*iter<<endl;
}

bool SqlWorker::ExcecuteLogicalPlan(LogicalQueryPlan*& lp)
{
	ReturnRelation retR = lp->Execute();
	ConsoleOutput(retR);
	if(retR.markedForDeletion)
		schema_manager.deleteRelation(retR.relationName);
	return true;
}

bool SqlWorker::DropTable(string tableName)
{
	Relation* rel = schema_manager.getRelation(tableName);
	if(rel == NULL)
		return false;
	int noOfBlocks = rel->getNumOfBlocks();
	if(noOfBlocks !=0)
	{
		//Delete all the blocks from the relation
		rel->deleteBlocks(0);
	}

	bool success =  schema_manager.deleteRelation(tableName);
	if(success) 
		cout<<"Table: "<<tableName<<" Dropped successfully"<<endl;
	return success;
}


bool SqlWorker::DeleteTable(string tableName, SearchConditionNode*& searchCondition)
{
	//1. Get all the tuples from the relation in a vector, RelVector
	//2. Get the no of Blocks for the relation
	//3. Iterate on each vector in RelVector and validate against search condition
	//4. Maintain a new vector NewRelVector, copy only those tuples to this which didn't get validated in step 3
	//5. Copy the tuples form NewRelVector to relation and keep counting the no of blocks
	//6. Evaluate the no of blocks to be nullified in the end and nullified those by copying null block
	//7. Test the result by printing relation for various boundary conditions
	//Note: this strategy is not good but maintained to avoid (holes) in Realtion
	//Will lead to all the equal to 2*(no of blocks) in relation worst case, even if no of tuple modified could only be one.

	int relIndex = 0;
	vector<int> affectedBlockIndexes;
	vector<Tuple>::iterator iter;
	Relation *relPtr = schema_manager.getRelation(tableName);
	int noOfBlocksInRelation = relPtr->getNumOfBlocks();
	int noOfTuples = relPtr->getNumOfTuples();
	if(noOfBlocksInRelation == 0) return true;

	if(searchCondition == NULL)
	{
		if(noOfTuples>0)
			relPtr->deleteBlocks(0);
		cout<<"No of Tuples Deleted = "<<noOfTuples<<endl;
		return true;
	}
	Block* blkPtr = mem.getBlock(0);
	bool pushToFinalSave = false;
	vector<Tuple> finallySavedTuples;
	for(relIndex = 0; relIndex < noOfBlocksInRelation; relIndex++)
	{
		relPtr->getBlock(relIndex, 0);
		vector<Tuple> tuples = blkPtr->getTuples();
		vector<Tuple> tuplesToSave;
		for(iter = tuples.begin(); iter!= tuples.end(); iter++)
		{
			MapTupleValues(*iter, tableName);
			if(!searchCondition->Evaluate())
			{
				tuplesToSave.push_back(*iter);
			}
		}
		if(tuples.size() != tuplesToSave.size()) //Means some tuples got deleted becoz they have fullfilled search condition
		{
			affectedBlockIndexes.push_back(relIndex);
			//As soon as get one block to edit start saving it to final saved buffer
			pushToFinalSave = true;
		}
		if(pushToFinalSave)
		{
			finallySavedTuples.insert(finallySavedTuples.end(), tuplesToSave.begin(), tuplesToSave.end());
		}
	}

	//----------------------------------------------
	int strtBlkIndex = affectedBlockIndexes.front();
	//Now persist tuple to disk (Relation, block startinf from strtBlkIndex)

	if(finallySavedTuples.size() == 0) 
	{ 
		if(noOfTuples>0)
			relPtr->deleteBlocks(0);
		return true;
	}
	bool success = true;
	Block* blockPtr = mem.getBlock(1);  //use memory index one, just taken arbitrarily
	blockPtr->clear();
	Schema schema = relPtr->getSchema();
	int noOfTuplesPerBlock = schema.getTuplesPerBlock();
	int relblockIndex = 0; //Where to insert the block in relation

	for(iter = finallySavedTuples.begin(); iter!=finallySavedTuples.end(); iter++)
	{
		blockPtr->appendTuple(*iter);
		if(blockPtr->isFull())//if the block in full flush it to memory
		{
			relPtr->setBlock(strtBlkIndex, 1);
			blockPtr->clear();
			strtBlkIndex++;
		}
	}

	//Flush out the last few tuples 
	if(!blockPtr->isEmpty())
	{
		relPtr->setBlock(strtBlkIndex, 1);
	}

	relPtr->deleteBlocks(strtBlkIndex+1); //delete is there is any remaining block in relation after
										  //strtBlkIndex, which is by this time pointing to the last
										  //valid block in relation

	//cout<<*relPtr;
}



