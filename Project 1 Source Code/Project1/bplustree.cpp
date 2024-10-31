#include "bplustree.h"
#include "bplusnode.cpp" 
#include "datablkptr.cpp"
#include <vector>
#include <iostream>
#include <queue>

BPlusTree::BPlusTree(int blkSize) {
    blockSize = blkSize;
    // Assume each pointer takes 8 bytes and each integer key takes 4 bytes
    maxNodeSize = (blkSize - 8) / (4 + 8);
    root = nullptr;
    numNodes = 0;
    height = 0;
    maxDataBlkPtrSize = blockSize / 8;
}


int BPlusTree::getMaxNodeSize() {
    return this->maxNodeSize;
}

int BPlusTree::getNumNodes() {
    return this->numNodes;
}

int BPlusTree::getHeight() {
    return this->height;
}

BPlusNode* BPlusTree::getRoot() {
    return this->root;
}

void BPlusTree::insert(int val, void* ptrToBlk) {
    if (root == nullptr) {
        root = new BPlusNode(maxNodeSize, true);
        root->keys[0] = val;
        root->isLeaf = true;
        root->numKeys = 1;
        root->dataPtrs[0]->insert(ptrToBlk);
        numNodes ++;
        height = 1;
        return;
    }

    BPlusNode* cur = root;
    vector<BPlusNode*> path;
    // Locate the leaf node
    while (cur->isLeaf == false) {
        path.push_back(cur);
        for (int i = 0; i < cur->numKeys; i++) {
            if (val < cur->keys[i]) {
                cur = cur->ptrs[i];
                break;
            }
            if (i == cur->numKeys - 1) {
                cur = cur->ptrs[i+1];
                break;
            }
        }
    }

    // If the leaf node has space
    if (cur->numKeys < cur->maxSize) {
        int i = 0;
        while (val > cur->keys[i] && i < cur->numKeys) {
            i++;
        }

        // If the key already exists
        // TODO
        if (i < cur->numKeys && val == cur->keys[i]) {
            cur->dataPtrs[i]->insert(ptrToBlk);
            return;
        }

        // Shift the exisitng keys and pointers
        for (int j = cur->numKeys; j > i; j--) {
            cur->keys[j] = cur->keys[j-1];
            cur->dataPtrs[j] = cur->dataPtrs[j-1];
        }
        // Insert the new key
        cur->keys[i] = val;
        cur->numKeys += 1;
        cur->dataPtrs[i] = new DataBlkPtr(maxDataBlkPtrSize);
        cur->dataPtrs[i]->insert(ptrToBlk);
        return;
    }

    // If the leaf node needs to be split
    BPlusNode* newNode = new BPlusNode(maxNodeSize, true);
    numNodes++;
    int i = 0, j;
    int tempKey;
    DataBlkPtr* tempDataPtr;
    while (val > cur->keys[i] && i < maxNodeSize) {
        i++;
    }

    // If the key already exists
    if (i < cur->numKeys && val == cur->keys[i]) {
        cur->dataPtrs[i]->insert(ptrToBlk);
        return;
    }

    if (i == maxNodeSize) {
        tempKey = val;
        tempDataPtr = new DataBlkPtr(maxDataBlkPtrSize);
        tempDataPtr->insert(ptrToBlk);
    } else {
        tempKey = cur->keys[maxNodeSize-1];
        tempDataPtr = cur->dataPtrs[maxNodeSize-1];
        for (j = maxNodeSize-1; j > i; j--) {
            cur->keys[j] = cur->keys[j-1];
            cur->dataPtrs[j] = cur->dataPtrs[j-1];
        }
        cur->keys[i] = val;
        cur->dataPtrs[i] = new DataBlkPtr(maxDataBlkPtrSize);
        cur->dataPtrs[i]->insert(ptrToBlk);
    }
    
    int num_keys_new = (maxNodeSize + 1) / 2;
    int num_keys_old = maxNodeSize + 1 - num_keys_new;
    cur->numKeys = num_keys_old;
    newNode->numKeys = num_keys_new;
    for (j = num_keys_old; j < maxNodeSize; j++) {
        newNode->keys[j-num_keys_old] = cur->keys[j];
        newNode->dataPtrs[j-num_keys_old] = cur->dataPtrs[j];
    }
    newNode->keys[num_keys_new-1] = tempKey;
    newNode->dataPtrs[num_keys_new-1] = tempDataPtr;
    // Link the new node to the current node
    newNode->ptrs[maxNodeSize] = cur->ptrs[maxNodeSize];
    cur->ptrs[maxNodeSize] = newNode;
    // If the current node is the root, a new root is created
    if (cur == root) {
        BPlusNode* newRoot = new BPlusNode(maxNodeSize, false);
        numNodes ++;
        newRoot->keys[0] = newNode->keys[0];
        newRoot->ptrs[0] = cur;
        newRoot->ptrs[1] = newNode;
        newRoot->numKeys = 1;
        root = newRoot;
        height ++;
    } else {
        // Insert the new leaf node into the B+ tree
        insertChildNode(newNode->keys[0], newNode, path);
    }
}

// Add a child node into the parent node and inccur recursion when necessary
void BPlusTree::insertChildNode(int lowerBound, BPlusNode* newNode, vector<BPlusNode*> path) {
    BPlusNode* cur = path.back();
    path.pop_back();
    // If the parent node is not full
    if (cur->numKeys < maxNodeSize) {
        int i = 0;
        while (lowerBound > cur->keys[i] && i < cur->numKeys) {
            i++;
        }
        for (int j = cur->numKeys; j > i; j--) {
            cur->keys[j] = cur->keys[j-1];
        }
        for (int j = cur->numKeys + 1; j > i; j--) {
            cur->ptrs[j] = cur->ptrs[j-1];
        }
        cur->keys[i] = lowerBound;
        cur->numKeys += 1;
        cur->ptrs[i+1] = newNode; 
        
    } else {  // If the parent node is full and needs to be split
        BPlusNode* newInNode = new BPlusNode(maxNodeSize, false);
        numNodes ++;
        int tempKey;
        BPlusNode* tempPtr;
        // Find the position of the new node
        int i = 0, j;
        while (lowerBound > cur->keys[i] && i < maxNodeSize) {
            i ++;
        }
        // Insert the new node first
        if (i == maxNodeSize) {
            tempKey = lowerBound;
            tempPtr = newNode;
        } else {
            tempKey = cur->keys[maxNodeSize-1];
            tempPtr = cur->ptrs[maxNodeSize];
            for (j = maxNodeSize; j > i; j--) {
                cur->keys[j] = cur->keys[j-1];
            }
            for (j = maxNodeSize+1; j > i+1; j--) {
                cur->ptrs[j] = cur->ptrs[j-1];
            }
            cur->keys[i] = lowerBound;
            cur->ptrs[i+1] = newNode;
        }

        // Split the current node and fill the new node
        int numKeysOld = (maxNodeSize+1) / 2;
        int numKeysNew = maxNodeSize - (maxNodeSize+1) / 2;
        cur->numKeys = numKeysOld;
        newInNode->numKeys = numKeysNew;
        for (j = 0; j < numKeysNew-1; j++) {
            newInNode->keys[j] = cur->keys[numKeysOld+1+j];
        }
        for (j = 0; j < numKeysNew; j++) {
            newInNode->ptrs[j] = cur->ptrs[numKeysOld+1+j];
        }
        newInNode->keys[numKeysNew-1] = tempKey;
        newInNode->ptrs[numKeysNew] = tempPtr;

        // If the current node is the root, a new root is created
        if (cur == root) {
            BPlusNode* newRoot = new BPlusNode(maxNodeSize, false);
            numNodes ++;
            newRoot->keys[0] = cur->keys[cur->numKeys];
            newRoot->ptrs[0] = cur;
            newRoot->ptrs[1] = newInNode;
            newRoot->numKeys = 1;
            root = newRoot;
            height ++;
        } else {
            // Insert the new internal node recursively
            insertChildNode(cur->keys[cur->numKeys], newInNode, path);
        }
    }
}

vector<pair<int, int> > BPlusTree::query(int val) {
    int nodeCount = 0;
    vector<pair<int, int> > result;
    // If the tree is empty
    if (root == nullptr) {
        cout << "The B+ Tree is empty." << endl;
        return result;
    }

    BPlusNode* cur = root;
    // Locate the leaf node
    while (cur->isLeaf == false) {
        nodeCount ++;
       if (nodeCount < 6) {
            printf("Node %d: ", nodeCount);
            cur->printNode();
            cout << endl;
        }
        for (int i = 0; i < cur->numKeys; i++) {
            if (val < cur->keys[i]) {
                cur = cur->ptrs[i];
                break;
            }
            if (i == cur->numKeys - 1) {
                cur = cur->ptrs[i+1];
                break;
            }
        }
    }
    nodeCount ++;
    if (nodeCount < 6) {
            printf("Node %d: ", nodeCount);
            cur->printNode();
            cout << endl;
        }
    int i = 0;
    while (i < cur->numKeys && val > cur->keys[i]) {
        i++;
    }
    // If the value is not present
    if (i == cur->numKeys || val != cur->keys[i]) {
        cout << "Value not found." << endl;
        cout << '\n' << "Total nmber of nodes accessed: " << nodeCount << endl;
        return result;
    }
    // If the value is found
    result = cur->getDataBlkPtrs(i);
    cout << '\n' << "Total number of nodes accessed: " << nodeCount << endl;
    return result;
}

vector<pair<int, int> > BPlusTree::queryRange(int lower, int upper) {
    int nodeCount = 0;
    vector<pair<int, int> > result;
    // If the tree is empty
    if (root == nullptr) {
        cout << "The B+ Tree is empty." << endl;
        return result;
    }

    BPlusNode* cur = root;
    // Locate the leaf node
    while (cur->isLeaf == false) {
        nodeCount ++;
        if (nodeCount < 6) {
            printf("Node %d: ", nodeCount);
            cur->printNode();
            cout << endl;
        }
        for (int i = 0; i < cur->numKeys; i++) {
            if (lower < cur->keys[i]) {
                cur = cur->ptrs[i];
                break;
            }
            if (i == cur->numKeys - 1) {
                cur = cur->ptrs[i+1];
                break;
            }
        }
    }

    int i = 0;
    while (i < cur->numKeys && lower > cur->keys[i]) {
        i++;
    }
    BPlusNode* curLeaf = cur;
    int idx = i;
    bool finished = false;
    // If the lower bound is found
    while (curLeaf != nullptr && !finished) {
        nodeCount ++;
        if (nodeCount < 6) {
            printf("Node %d: ", nodeCount);
            cur->printNode();
            cout << endl;
        }
        for (int j = idx; j < curLeaf->numKeys; j++) {
            if (curLeaf->keys[j] > upper) {
                finished = true;
                break;
            }
            vector<pair<int, int> > blkPtrs = curLeaf->getDataBlkPtrs(j);
            result.insert(result.end(), blkPtrs.begin(), blkPtrs.end());
        }
        idx = 0;
        curLeaf = curLeaf->ptrs[curLeaf->maxSize];
    }

    cout << "Total number of nodes accessed: " << nodeCount << endl;
    return result;
}


void BPlusTree::printTree() {
    queue<BPlusNode*> q;
    int level = 1;
    q.push(root);
    if (root == nullptr) {
        cout << "The tree is empty!" << endl;
        return;
    }
    while (q.size() > 0 && q.front()->isLeaf == false) {
        queue<BPlusNode*> temp;
        printf("%d: ", level++);
        while (q.size() > 0) {
            BPlusNode* cur = q.front();
            q.pop();
            cur->printNode();
            for (int i = 0; i < cur->numKeys+1; i++) {
                temp.push(cur->ptrs[i]);
            }
        }
        q = temp;
    }
    printf("%d: ", level);
    while (q.size() > 0) {
        q.front()->printNode();
        q.pop();
    }
    // Print information of the tree
    printf("Max number of keys (n): %d\n", maxNodeSize);
    printf("Height: %d\n", height);
    printf("Number of nodes: %d\n", numNodes);
    printf("Root node: ");
    root->printNode();
    printf("1st child node of root: ");
    if (root->isLeaf) {
        printf("No child\n");
    } else {
        root->ptrs[0]->printNode();
    }
}

void BPlusTree::print1Child() {
    printf("The content of 1st child node of root: ");
    if (root->isLeaf) {
        printf("No child \n");
    } else {
        root->ptrs[0]->printNode();
    }
}


int BPlusTree::remove(int val) {
    int numDeleted = 0;
    // If the tree is empty
    if (root == nullptr) {
        cout << "The tree is empty, nothing to remove" << endl;
        return numDeleted;
    }

    BPlusNode* cur = root;
    BPlusNode* parent;
    int left, right;  // Index of the left and right nodes in the parent node
    // Move down to leaf node
    while (cur->isLeaf == false) {
        for (int i = 0; i < cur->numKeys; i++) {
            parent = cur;
            left = i - 1;
            right = i + 1;
            if (val < cur->keys[i]) {
                cur = cur->ptrs[i];
                break;
            }
            if (i == cur->numKeys-1) {
                left = i;
                right = i + 2;
                cur = cur->ptrs[i+1];
                break;
            }
        }
    }
    // Find the key in the leaf node
    bool found = false;
    int pos;
    for (pos = 0; pos < cur->numKeys; pos++) {
        if (cur->keys[pos] == val) {
            found = true;
            break;
        }
    }
    // If the value is not found
    if (!found) {
        cout << "Key value not found!" << endl;
        return numDeleted;
    }

    // Delete the key first
    for (int i = pos; i < cur->numKeys-1; i++) {
        cur->keys[i] = cur->keys[i+1];
        cur->dataPtrs[i] = cur->dataPtrs[i+1];
    }
    cur->numKeys -= 1;

    // If the tree is empty after deletion
    if (cur == root && cur->numKeys == 0) {
        delete[] cur->ptrs;
        delete[] cur->keys;
        root = nullptr;
        numNodes -= 1;
        numDeleted += 1;
        return numDeleted;
    }

    // If the leaf node is left with enough keys
    int minLeafKeys = (cur->maxSize + 1) / 2;
    if (cur->numKeys >= minLeafKeys) {
        parent->keys[left] = cur->keys[0];
        return numDeleted;
    }

    // Else the leaf node need to be merged
    // If a key can be borrowed from the left node
    if (left >= 0) {
        BPlusNode* leftNode = parent->ptrs[left];
        if (leftNode->numKeys >= minLeafKeys + 1) {
            for (int i = cur->numKeys; i > 0; i--) {
                cur->keys[i] = cur->keys[i-1];
                cur->dataPtrs[i] = cur->dataPtrs[i-1];
            }
            cur->numKeys += 1;
            cur->keys[0] = leftNode->keys[leftNode->numKeys-1];
            cur->dataPtrs[0] = leftNode->dataPtrs[0];
            leftNode->numKeys -= 1;
            parent->keys[left] = cur->keys[0];
            return numDeleted; 
        }
    }
    // If a key can be borrowed from the right node
    if (right <= parent->numKeys) {
        BPlusNode* rightNode = parent->ptrs[right];
        if (rightNode->numKeys >= minLeafKeys + 1) {
            cur->numKeys += 1;
            cur->keys[cur->numKeys-1] = rightNode->keys[0];
            cur->dataPtrs[cur->numKeys-1] = rightNode->dataPtrs[0];
            rightNode->numKeys -= 1;
            for (int i = 0; i < rightNode->numKeys; i++) {
                rightNode->keys[i] = rightNode->keys[i+1];
                rightNode->dataPtrs[i] = rightNode->dataPtrs[i+1];
            }
            parent->keys[right-1] = rightNode->keys[0];
            return numDeleted;
        }
    }

    // If the current node can be merged to the left node
    if (left >= 0) {
        BPlusNode* leftNode = parent->ptrs[left];
        for (int i = 0; i < cur->numKeys; i++) {
            leftNode->keys[leftNode->numKeys+i] = cur->keys[i];
            leftNode->dataPtrs[leftNode->numKeys+i] = cur->dataPtrs[i];
        }
        leftNode->numKeys += cur->numKeys;
        leftNode->ptrs[leftNode->maxSize] = cur->ptrs[cur->maxSize];
        numNodes -= 1;
        numDeleted += 1;
        numDeleted += removeInNode(parent->keys[left], parent, cur);
        delete[] cur->keys;
        delete[] cur->ptrs;
        delete cur;
        return numDeleted;
    }
    // If the right node can be merged to the current node
    if (right <= parent->numKeys) {
        BPlusNode* rightNode = parent->ptrs[right];
        for (int i = 0; i < rightNode->numKeys; i++) {
            cur->keys[cur->numKeys+i] = rightNode->keys[i];
            cur->dataPtrs[cur->numKeys+i] = rightNode->dataPtrs[i];
        }
        cur->numKeys += rightNode->numKeys;
        cur->ptrs[cur->maxSize] = rightNode->ptrs[rightNode->maxSize];
        numNodes -= 1;
        numDeleted += 1;
        numDeleted += removeInNode(parent->keys[right-1], parent, rightNode);
        delete[] rightNode->keys;
        delete[] rightNode->ptrs;
        delete rightNode;
        return numDeleted;
    }
    return numDeleted;
}


// Remove a pointer and the key from an internal node
int BPlusTree::removeInNode(int val, BPlusNode* cur, BPlusNode* child) {
    int numDeleted = 0;
    // If the root needs to be removed
    if (cur == root && cur->numKeys == 1) {
        if (cur->ptrs[1] == child) {
            root = cur->ptrs[0];
            height -= 1;
            delete[] cur->keys;
            delete[] cur->ptrs;
            delete cur;
            numNodes -= 1;
            numDeleted += 1;
            return numDeleted;
        } else if (cur->ptrs[0] == child) {
            root = cur->ptrs[1];
            height -= 1;
            delete[] cur->keys;
            delete[] cur->ptrs;
            delete cur;
            numNodes -= 1;
            numDeleted += 1;
            return numDeleted;
        }
    }
    
    // Locate the key to be removed
    int pos;
    for (pos = 0; pos < cur->numKeys; pos++) {
        if (cur->keys[pos] == val) {
            break;
        }
    }
    // Remove the key and pointer first
    for (int i = pos; i < cur->numKeys; i++) {
        cur->keys[i] = cur->keys[i+1];
    }
    for (int i = pos+1; i < cur->numKeys+1; i++) {
        cur->ptrs[i] = cur->ptrs[i+1];
    }
    cur->numKeys -= 1;

    // If the current node is left with enough keys
    int minInKeys = cur->maxSize / 2;
    if (cur->numKeys >= minInKeys) {
        return numDeleted;
    }
    // If the curent node is the root
    if (cur == root) {
        return numDeleted;
    }

    BPlusNode* parent = findParent(root, cur);
    int left, right;  // Position of the left and right adjacant nodes
    for (pos = 0; pos < parent->numKeys; pos++) {
        if (parent->ptrs[pos] == cur) {
            left = pos - 1;
            right = pos + 1;
            break;
        }
    }
    // If a key can be borrowed from the left node
    if (left >= 0) {
        BPlusNode* leftNode = parent->ptrs[left];
        if (leftNode->numKeys >= minInKeys + 1) {
            for (int i = cur->numKeys; i > 0; i--) {
                cur->keys[i] = cur->keys[i-1];
            }
            cur->keys[0] = parent->keys[left];
            parent->keys[left] = leftNode->keys[leftNode->numKeys-1];
            for (int i = cur->numKeys+1; i > 0; i--) {
                cur->ptrs[i] = cur->ptrs[i-1];
            }
            cur->ptrs[0] = leftNode->ptrs[leftNode->numKeys];
            cur->numKeys += 1;
            leftNode->numKeys -= 1;
            return numDeleted;
        }
    }

    // If a key can be borrowed from the right node
    if (right <= parent->numKeys) {
        BPlusNode* rightNode = parent->ptrs[right];
        if (rightNode->numKeys >= minInKeys + 1) {
            cur->keys[cur->numKeys] = rightNode->keys[0];
            parent->keys[pos] = rightNode->keys[0]; 
        }
        for (int i = 0; i < rightNode->numKeys-1; i++) {
            rightNode->keys[i] = rightNode->keys[i+1];
        }
        cur->ptrs[cur->numKeys+1] = rightNode->ptrs[0];
        for (int i = 0; i < rightNode->numKeys+1; i++) {
            rightNode->ptrs[i] = rightNode->ptrs[i+1];
        }
        cur->numKeys += 1;
        rightNode->numKeys -= 1;
        return numDeleted;
    }

    // If the curent node needs to be merged to the left node
    if (left >= 0) {
        BPlusNode* leftNode = parent->ptrs[left];
        leftNode->keys[leftNode->numKeys] = parent->keys[left];
        for (int i = 0; i < cur->numKeys; i++) {
            leftNode->keys[leftNode->numKeys+1+i] = cur->keys[i];
        }
        for (int i = 0; i < cur->numKeys+1; i++) {
            leftNode->ptrs[leftNode->numKeys+1+i] = cur->ptrs[i];
            cur->ptrs[i] = nullptr;
        }
        leftNode->numKeys += cur->numKeys + 1;
        cur->numKeys = 0;
        numNodes -= 1;
        numDeleted += 1;
        numDeleted += removeInNode(parent->keys[left], parent, cur);
        return numDeleted;
    }
    // If the right node needs to be merged to the current node
    if (right <= parent->numKeys) {
        BPlusNode* rightNode = parent->ptrs[right];
        cur->keys[cur->numKeys] = parent->keys[right-1];
        for (int i = 0; i < rightNode->numKeys; i++) {
            cur->keys[cur->numKeys+1+i] = rightNode->keys[i];
        }
        for (int i = 0; i < rightNode->numKeys+1 ; i++) {
            cur->ptrs[cur->numKeys+1+i] = rightNode->ptrs[i];
            rightNode->ptrs[i] = nullptr;
        }
        cur->numKeys += rightNode->numKeys + 1;
        rightNode->numKeys = 0;
        numNodes -= 1;
        numDeleted += 1;
        numDeleted += removeInNode(parent->keys[right-1], parent, rightNode);
        return numDeleted;
    }
    return numDeleted;
}


BPlusNode* BPlusTree::findParent(BPlusNode* cur, BPlusNode* child) {
    BPlusNode *parent = nullptr;
    if (cur->isLeaf || (cur->ptrs[0])->isLeaf) {
        return nullptr;
    }
    for (int i = 0; i < cur->numKeys + 1; i++) {
        if (cur->ptrs[i] == child) {
            parent = cur;
            return parent;
        } else {
            parent = findParent(cur->ptrs[i], child);
            if (parent != nullptr)
            return parent;
        }
    }
    return parent;
}
