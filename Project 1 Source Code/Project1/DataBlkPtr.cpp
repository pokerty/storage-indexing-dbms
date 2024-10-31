#include "datablkptr.h"
#include <vector>
using namespace std;

DataBlkPtr::DataBlkPtr(int size) {
    maxSize = size;
    numPtrs = 0;
    next = nullptr;
}

void DataBlkPtr::insert(void* newPtr) {
    if (numPtrs < maxSize) {
        ptrs.push_back(newPtr);
        numPtrs ++;
        return;
    } else if (next == nullptr) {
        next = new DataBlkPtr(maxSize);
        next->insert(newPtr);
    } else{
        next->insert(newPtr);
    }
}
