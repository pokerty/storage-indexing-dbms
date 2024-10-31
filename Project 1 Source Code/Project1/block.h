#ifndef BLOCK_H
#define BLOCK_H 

#include <iostream>
#include <cstdlib>
#include <vector>
#include <queue>
#include <list>
#include "record.h"
using namespace std;

class Block {
    private:
        vector <Record> listOfRecords;
        int blockSize;
        int blockID;
        queue<int> deletedRecords;

    public:
        int insertRecord(Record record);
        void deleteRecord(int recordID);
        void printBlock();
        void printRecord(int recordID);
        Record getRecord(int recordID);

        //Don's new functions
        int getNumRecords() const; // Function to get the number of records in the block
        int getRecordSize() const; // Function to get the size of a single record
        int getMaxRecords() const; // Function to get the maximum number of records the block can hold

        Block(int blockSize, int blockID);
};

#endif // BLOCK_H