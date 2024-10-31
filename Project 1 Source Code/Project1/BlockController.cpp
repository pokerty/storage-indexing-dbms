#include <iostream>
#include <cstdlib>
#include <vector>
#include <queue>
#include <list>
#include <map>
#include <iterator>
#include "block.h"
#include "block.cpp"
#include "blockcontroller.h"

using namespace std;

BlockController::BlockController(int blockSize){
    this->blockSize = blockSize;
    listOfBlocks.push_back(Block(this->blockSize, listOfBlocks.size()+1));
}

void * BlockController::insert(string record){
    int tempRecordID;
    Record recObj(record);

    if (isEmptyBlocks.size() > 0){
        int index = isEmptyBlocks.front();
        tempRecordID = listOfBlocks[index].insertRecord(recObj); 
        //current block overflow 
        if (tempRecordID == -1){
            //current block no avail space for records, remove from isEmptyBlocks queue
            isEmptyBlocks.pop();
        //record successfully added into one of the middle blocks with space
        } else {
            isEmptyBlocks.pop();
            //insert the blockID and recordID where the record was inserted, into mapping table
            mappingTable.push_back(make_pair(index, tempRecordID));
            return &mappingTable.back();
        }
    //if no blocks in the middle have empty/available space
    } else {
        //add to back of list of blocks
        tempRecordID = listOfBlocks[listOfBlocks.size()-1].insertRecord(recObj);
        //block at the back has no avail space to insert record
        if (tempRecordID == -1){
            //create new block since current block overflow
            listOfBlocks.push_back(Block(this->blockSize, listOfBlocks.size()+1));
            //insert record into the back most block
            tempRecordID = listOfBlocks[listOfBlocks.size()-1].insertRecord(recObj);
        }

        mappingTable.push_back(make_pair(listOfBlocks.size(), tempRecordID));
    }

    return &mappingTable.back();
}


void BlockController::printBlockContent(int blockID){
    listOfBlocks[blockID-1].printBlock();
}

Record BlockController::getRecord(int blockID, int recordID){
    return listOfBlocks[blockID-1].getRecord(recordID);
}

void BlockController::deleteRecord(int blockID, int recordID){
    listOfBlocks[blockID].deleteRecord(recordID);
    isEmptyBlocks.push(blockID);

    mappingTable.remove(make_pair(blockID, recordID));
}

int BlockController::getNumOfBlocks(){
    return listOfBlocks.size();
}

float BlockController::getDatabaseSize(){
    float databaseSize = float(listOfBlocks.size() * this->blockSize) / float(1024*1024);
    return databaseSize;
}

Block BlockController::getBlock(int blockID){ 
        return listOfBlocks[blockID-1]; 
}

//Don's functions

int BlockController::getNumOfRecords() {
    int totalRecords = 0;
    for (const Block& block : listOfBlocks) {
        totalRecords += block.getNumRecords();
    }
    return totalRecords;
}

int BlockController::getSingleRecordSize() {
    // Assuming all records have the same size, so we can get the size from the first block
    return listOfBlocks[0].getRecordSize();
}

int BlockController::getRecordsPerBlock() {
    // Assuming all blocks have the same number of records, so we can get the count from the first block
    return listOfBlocks[0].getMaxRecords();
}