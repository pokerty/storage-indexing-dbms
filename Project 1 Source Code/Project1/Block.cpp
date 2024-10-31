#include "record.h"
#include "record.cpp"
#include "block.h"
#include <iostream>
#include <cstdlib>
#include <vector>
#include <queue>
#include <list>


using namespace std;

Block::Block(int blockSize, int blockID){
    vector<Record> listOfRecords;
    this->blockSize = blockSize;
    this->blockID = blockID;
}


void Block::deleteRecord(int recordID){
    listOfRecords[recordID-1].setIsDeleted();
    deletedRecords.push(recordID);
}

void Block::printRecord(int recordID){
    cout << listOfRecords[recordID-1].toString() << endl;

}

int Block::insertRecord(Record record){
    if (deletedRecords.size() > 0){
        int index = deletedRecords.front();
        deletedRecords.pop();
        listOfRecords[index] = record; 
        return (index);
    } else if ((this->blockSize) - (listOfRecords.size() * record.getRecordBytes()) < record.getRecordBytes()){
        return -1;
    } else {
        listOfRecords.push_back(record);
        return ((listOfRecords.size()));
    }
}


void Block::printBlock(){
    for (int i = 0; i < listOfRecords.size(); i++){
        string recordString = listOfRecords[i].toString();
        cout << recordString << endl;
    }
}


Record Block::getRecord(int recordID){
    return listOfRecords[recordID-1];
}
 
// Don's new functions
int Block::getNumRecords() const {
    return listOfRecords.size();
}

int Block::getRecordSize() const {
    // Assuming all records have the same size
    if (!listOfRecords.empty()) {
        return listOfRecords[0].getRecordBytes();
    }
    return 0;
}

int Block::getMaxRecords() const {
    // Assuming the maximum number of records is determined by the block size and record size
    if (getRecordSize() > 0) {
        return blockSize / getRecordSize();
    }
    return 0;
}