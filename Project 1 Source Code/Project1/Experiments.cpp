#include <iostream>
#include <cstdlib>
#include <vector>
#include <queue>
#include <list>
#include <fstream>
#include <limits>
#include <set>
#include <iomanip>
#include "blockcontroller.cpp"
#include "bplustree.cpp"
#include "chrono"
#include "ctime"

using namespace std;

class Experiments {
    private:
        vector<string> splitString(string s) {
            istringstream iss(s);
            vector<string> tokens;
            string tempWord;

            while (getline(iss, tempWord, '\t'))
                tokens.push_back(tempWord);

            return tokens;
        }
        string fileName;
        BlockController * blkController;
        BPlusTree * bTree;



    public:
        Experiments(string fileName, int blockSize);
        void buildBTree();
        void experiment1();
        void experiment2();
        void experiment3();
        void experiment4();
        void experiment5();
        void linearScan(int desiredVotes);
        void linearScanRange(int minVotes, int maxVotes);
        void removeByLinearScan(int votesToRemove);

};

//Constructor
Experiments::Experiments(string fileName, int blockSize){
    this->fileName = fileName;
    this->blkController = new BlockController(blockSize);
    this->bTree = new BPlusTree(blockSize);
}

void Experiments::linearScan(int desiredVotes) {
    int count = 0;
    int blockID = 1;
    double avgRatingTotal = 0;

    // Iterate through each block in the database
    for (blockID; blockID <= blkController->getNumOfBlocks(); blockID++) {
        Block block = blkController->getBlock(blockID);
        // Iterate through each record in the block
        for (int recordID = 1; recordID <= block.getNumRecords(); recordID++) {
            Record record = block.getRecord(recordID);

            // Check if the record meets the criteria (e.g., numVotes == desiredVotes)
            if (record.getNumVotes() == desiredVotes) {
                // Process the matching record (e.g., calculate average rating)
                avgRatingTotal += record.getAverageRating();
                count++;
            }
        }
    }

    // Calculate average of average ratings
    double averageRating = (count > 0) ? (avgRatingTotal / count) : 0;

    // Display results
    cout << "\n-- Brute Force Linear Scan Results -- " << endl;
    //cout << "Total number of records with numVotes equal to " << desiredVotes << ": " << count << endl;
    cout << "Average of average ratings of matching records: " << averageRating << endl;
    cout << "Number of data blocks accessed: " << blockID-1 << endl;
}

void Experiments::linearScanRange(int minVotes, int maxVotes) {
    int count = 0;
    double avgRatingTotal = 0;
    int blockID = 1;
    // Iterate through each block in the database
    for (blockID; blockID <= blkController->getNumOfBlocks(); blockID++) {
        Block block = blkController->getBlock(blockID);

        // Iterate through each record in the block
        for (int recordID = 1; recordID <= block.getNumRecords(); recordID++) {
            Record record = block.getRecord(recordID);

            // Check if the record's numVotes falls within the desired range
            int votes = record.getNumVotes();
            if (votes >= minVotes && votes <= maxVotes) {
                // Process the matching record
                avgRatingTotal += record.getAverageRating();
                count++;
            }
        }
    }

    // Calculate average of average ratings
    double averageRating = (count > 0) ? (avgRatingTotal / count) : 0;

    // Display results
    cout << "\n-- Brute Force Linear Scan Results -- " << endl;
    //cout << "\nTotal number of records with numVotes between " << minVotes << " and " << maxVotes << ": " << count << endl;
    cout << "Average of average ratings of matching records: " << averageRating << endl;
    cout << "Number of data blocks accessed: " << blockID-1<< endl;

}

void Experiments::removeByLinearScan(int votesToRemove) {
    // Start measuring time
    clock_t start = clock();

    int blocksAccessed = 0;
    int recordsRemoved = 0;

    // Assuming you have a way to iterate through all blocks
    for (int blockID = 1; blockID <= blkController->getNumOfBlocks(); ++blockID) {
        Block block = blkController->getBlock(blockID); // Assuming you can retrieve blocks like this
        blocksAccessed++;

        // Iterate through each record in the block
        for (int recordID = 0; recordID < block.getNumRecords(); ++recordID) {
            Record record = block.getRecord(recordID); // Assuming this is how you access records

            // Check if the record meets the criterion for removal
            if (record.getNumVotes() == votesToRemove) {
                recordsRemoved++;

            }
        }
    }


    // Output the results
    cout << "Removed " << recordsRemoved << " records with " << votesToRemove << " votes." << endl;
    cout << "Total data blocks accessed: " << blocksAccessed << endl;
}
void Experiments::buildBTree() {
    //read file
    ifstream dataFile(this->fileName);

    string dataLine;
    dataFile.ignore(numeric_limits<streamsize>::max(),'\n');
    int i = 0;

    while(getline(dataFile, dataLine)) {
        int key = stoi(splitString(dataLine)[2]);
        void * pointer = this->blkController->insert(dataLine);
        this->bTree->insert(key, pointer);
        i++;
    }
    dataFile.close();
}

void Experiments::experiment1(){
    //read the TSV file
    ifstream dataFile(this->fileName);
    string dataLine;

    // Writing to Block Controller to "write to disk"
    dataFile.ignore(numeric_limits<streamsize>::max(),'\n');
    while(getline(dataFile, dataLine)){
        this->blkController->insert(dataLine);
    }
    dataFile.close();
    //Retrieve the Block details from block controller
    int numberOfRecords = this->blkController->getNumOfRecords();
    int recordSize = this->blkController->getSingleRecordSize();
    int numberOfRecordsPerBlock = this->blkController->getRecordsPerBlock();
    int numberOfBlocks = this->blkController->getNumOfBlocks();

    cout << "Number of records: " << numberOfRecords << endl;
    cout << "Size of a record (in bytes): " << recordSize << endl;
    cout << "Number of records stored in a block: " << numberOfRecordsPerBlock << endl;
    cout << "Number of blocks for storing the data: " << numberOfBlocks << endl;
}



void Experiments::experiment2() {
    cout << "The parameter n of B+ tree: " << this->bTree->getMaxNodeSize() << endl;
    cout << "The number of nodes in B+ Tree: " << this->bTree->getNumNodes() << endl;
    cout << "The height (level) of B+ tree: " << this->bTree->getHeight() << endl;
    cout << '\n' << "The content of root node (only keys are shown): ";
    bTree->getRoot()->printNode();
    cout << endl;
}

void Experiments::experiment3() {
    cout << "(Only report the first 5 index nodes / data block)\n" << endl;

    // Start measuring time
    clock_t start = clock();

    cout << "- - - - - Content of top 5 index nodes accessed - - - - - " <<  endl;
    vector <pair<int, int> > queryResult;
    queryResult = this->bTree->query(500);

    // Stop measuring time after query.
    clock_t end = clock();
    double duration = ((double)(end - start) / CLOCKS_PER_SEC) * 1000;

     cout << "\n" << "- - - - - Content of top 5 data blocks accessed - - - - - " << endl;
    // store unique block IDs accessed
    set <int> numBlocks;
    double avgRatingTotal = 0;
    int i = 0;
    for (auto result : queryResult) {
        if (i < 5) {
            // printing out all top 5 contents/records in the data block accessed
            cout << "\n >> Found in blockID: " << result.first << " and recordID: " << result.second << endl;
            printf("tconst    |  averageRating  |  numVotes\n");
            blkController->printBlockContent(result.first);
            i++;
        }

        numBlocks.insert(result.first);
        // getting the total average ratings of records with numVotes=500
        avgRatingTotal = avgRatingTotal + (blkController->getBlock(result.first).getRecord(result.second).getAverageRating());

    }

    // total no. of unique data blocks accessed
    cout << "\nTotal number of data blocks accessed: " << numBlocks.size() << endl;
    // average of average ratings of all the records where numVotes=500
    cout << "Average of \"averageRatings\" of all the records where \"numVotes\" equal 500: " << avgRatingTotal/queryResult.size() << endl;
    // Running time of the retrieval process
    cout << "Running time of the retrieval process: " << duration << " milliseconds" << endl;


    // Function to calculate brute-force linear scan method
    // Start measuring time
    start = clock();
    linearScan(500);
    // Stop measuring time after query.
    end = clock();
    duration = ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
    cout << "Running time of the retrieval process: " << duration << " milliseconds" << endl;

}

void Experiments::experiment4() {
    cout << "(Only report the first 5 index nodes / data block)\n" << endl;

    // Start measuring time
    clock_t start = clock();

    cout << "- - - - - Content of top 5 index nodes accessed - - - - - " <<  endl;
    vector <pair<int, int> > queryResult;
    queryResult = this->bTree->queryRange(30000, 40000);

    // Stop measuring time after query.
    clock_t end = clock();
    double duration = ((double)(end - start) / CLOCKS_PER_SEC) * 1000;

    cout << "\n" << "- - - - - Content of top 5 data blocks accessed - - - - - " << endl;
    // store unique block IDs accessed
    set <int> numBlocks;
    float avgRatingTotal = 0;
    int i = 0;
    for (auto result : queryResult) {
        // print only the top 5 nodes content

        if (i < 5){
            // printing out all contents/records in the data block accessed
            cout << "\n >> Found in blockID: " << result.first << " and recordID: " << result.second << endl;
            printf("tconst    |  averageRating  |  numVotes\n");
            blkController->printBlockContent(result.first);
            i++;
        }

        numBlocks.insert(result.first);

        // getting the total average ratings of records with numVotes=500
        avgRatingTotal = avgRatingTotal + (blkController->getBlock(result.first).getRecord(result.second).getAverageRating());
    }
    // total no. of unique data blocks accessed
    cout << "\nTotal number of data blocks accessed: " << numBlocks.size() << endl;

    // average of average ratings of all the records where numVotes=500
    cout << "Average of \"averageRatings\" of the records: " << avgRatingTotal/queryResult.size() << endl;

    // Running time of the retrieval process
    cout << "Running time of the retrieval process: " << duration << " milliseconds" << endl;

    // Function to calculate brute-force linear scan method
    // Start measuring time
    start = clock();
    linearScanRange(30000, 40000);
    end = clock();
    duration = ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
    cout << "Running time of the retrieval process: " << duration << " milliseconds" << endl;

}

void Experiments::experiment5() {
    cout << "\n" << "/// Starting Experiment 5: Delete movies with \"numVotes\" equal 1000 " << endl;
    int countDelete = 0;

    // Start measuring time
    clock_t start = clock();

    countDelete = this->bTree->remove(1000);
    // Stop measuring time after query.
    clock_t end = clock();
    double duration = ((double)(end - start) / CLOCKS_PER_SEC) * 1000;

    cout << "Total number of times that a node is deleted: "<< countDelete << endl;
    cout << "The number of nodes in updated B+ tree: " << this->bTree->getNumNodes() << endl;
    cout << "The height (level) of updated B+ tree: " << this->bTree->getHeight() << endl;
    cout << "Running time of the removal process: " << duration << " milliseconds" << endl;

    bTree->getRoot()->printNode();
    cout << endl;
    bTree->print1Child();
    cout << endl;

    // Start measuring time
    start = clock();
    removeByLinearScan(1000);
    // Stop measuring time after query.
    end = clock();
    duration = ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
    cout << "Running time of the retrieval process: " << duration << " milliseconds" << endl;

}
