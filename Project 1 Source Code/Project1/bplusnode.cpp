#include "bplusnode.h"
#include <vector>
#include <iostream>

BPlusNode::BPlusNode(int n, bool leaf) {
    maxSize = n;
    keys = new int[maxSize];
    ptrs = new BPlusNode*[maxSize+1];
    isLeaf = leaf;
    numKeys = 0;
    for (int i = 0; i < maxSize+1; i++) {
        ptrs[i] = nullptr;
    }
    int maxDataPtrSize = (maxSize*12 + 8) / 8;
    for (int i = 0; i < maxSize; i++) {
        DataBlkPtr* ptr = new DataBlkPtr(maxDataPtrSize);
        dataPtrs.push_back(ptr);
    }
}

// Retrieve the DataBlkPtr at the given index and return results of the mapping table of keys to values
vector<pair<int, int> > BPlusNode::getDataBlkPtrs(int idx) {
    vector<pair<int, int> > result;
    DataBlkPtr* cur = dataPtrs[idx];
    while (cur != nullptr) {
        for (int j = 0; j < cur->numPtrs; j++) {
            pair<int, int> mappingTablePTR = *(pair<int, int> *)cur->ptrs[j];
            result.push_back(mappingTablePTR);
        }
        if (cur->numPtrs == cur->maxSize && cur->next != nullptr) {
            cur = cur->next;
        } else {
            break;
        }
    }
    
    return result;
}

void BPlusNode::printNode() {
    for (int i = 0; i < numKeys; i++) {
        printf("||%5d", keys[i]);
    }
    printf("||\n");
}
