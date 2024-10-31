#ifndef BPLUSTREE_H
#define BPLUSTREE_H
#include "bplusnode.h" 
#include <vector>

class BPlusTree {
    private:
    BPlusNode* root;
    int blockSize;  // Size of a block, i.e. max size of a node
    int numNodes;
    int height;
    int maxDataBlkPtrSize;
    int maxNodeSize;  // Max number of keys in a node


    public:
    BPlusTree(int blkSize);
    int getMaxNodeSize();
    int getNumNodes();
    int getHeight();
    BPlusNode* getRoot();

    void insert(int val, void* ptrToBlk);
    void insertChildNode(int key, BPlusNode* newNode, vector<BPlusNode*> path);
    vector<pair<int, int> > query(int val);  // Return a list of data block pointers
    vector<pair<int, int> > queryRange(int lower, int upper); // Inclusive of lower and upper range
    BPlusNode* findParent(BPlusNode* cur, BPlusNode* child);
    int remove(int val);
    int removeInNode(int val, BPlusNode* cur, BPlusNode* child);
    void printTree();
    void print1Child();
};

#endif // BPLUSTREE_H