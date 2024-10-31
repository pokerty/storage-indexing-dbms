#ifndef DATABLKPTR_H
#define DATABLKPTR_H 
#include <vector>
using namespace std;

class DataBlkPtr {
    private:
    int maxSize;
    int numPtrs;
    vector<void*> ptrs;
    DataBlkPtr* next;
    friend class BPlusNode;

    public:
    DataBlkPtr(int maxSize);
    void insert(void* newPtr);
};


#endif // DATABLKPTR_H