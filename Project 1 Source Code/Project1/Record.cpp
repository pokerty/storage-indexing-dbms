#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include "record.h"

using namespace std;


Record::Record(string s) {
    istringstream iss(s);
    vector<string> tokens;
    string tempWord;
            
    while (getline(iss, tempWord, '\t'))
        tokens.push_back(tempWord);

    strncpy(this->tConst, tokens[0].c_str(), LEN);
    this->averageRating = stof(tokens[1]);
    this->numVotes = stoi(tokens[2]);
    this->isDeleted = 'n';
}


string Record::getTconst() {
    return this->tConst;
}

double Record::getAverageRating() {
    return this->averageRating;
}

int Record::getNumVotes() {
    return this->numVotes;
}

int Record::getRecordBytes() const{
    return sizeof(this->tConst) + sizeof(this->averageRating) + sizeof(this->numVotes) + sizeof(this->isDeleted);
}

string Record::toString() {
    ostringstream strout;
    strout << getTconst() << "\t" << getAverageRating() << "\t" << getNumVotes();
    return strout.str();
}

char Record::getIsDeleted(){
    return this->isDeleted;
}

void Record::setIsDeleted(){
    this->isDeleted = 'y';
}
