#ifndef BPLUSNODE_H
#define BPLUSNODE_H 
#include <vector>
#include "datablkptr.h"
using namespace std;

// A tree node in B+ tree
class BPlusNode {
    private:
    int maxSize;  // Max number of keys
    BPlusNode** ptrs;  // Pointers to child nodes
    int* keys;  // Stored keys
    bool isLeaf;
    int numKeys; // Current number of keys
    // To-do: Use data block
    vector<DataBlkPtr*> dataPtrs;  // Vector for lead node to store pointers to data blocks
    friend class BPlusTree;

    public:
    BPlusNode(int n, bool leaf);
    void printNode();
    vector<pair<int, int> > getDataBlkPtrs(int idx);
};





#endif // BPLUSNODE_H