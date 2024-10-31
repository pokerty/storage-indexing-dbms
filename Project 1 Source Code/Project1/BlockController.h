#ifndef BLOCKCONTROLLER_H
#define BLOCKCONTROLLER_H

#include <vector>
#include <queue>
#include <list>
#include <map>
#include <iterator>
#include "block.h"

using namespace std;

class BlockController {
    private:
        int blockSize;
        vector<Block> listOfBlocks;
        list<pair<int, int> > mappingTable;
        queue<int> isEmptyBlocks;

    public:
        void printBlockContent(int blockID);
        Record getRecord(int blockID, int recordID);
        int getNumOfBlocks();
        float getDatabaseSize();
        Block getBlock(int blockID);
        void * insert(string recordString);
        void deleteRecord(int blockID, int recordID);

        //Don's new functions
        int getNumOfRecords();
        int getSingleRecordSize();
        int getRecordsPerBlock();

        //declare constructor
        BlockController(int blockSize);

};


#endif // BLOCKCONTROLLER_H