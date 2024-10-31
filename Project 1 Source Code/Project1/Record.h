#ifndef RECORD_H
#define RECORD_H 

#include <string>
#include <vector>
using namespace std;

# define LEN 11

class Record {  
    private:
         // LEN = "tt10630794" = 10 + 1
        char tConst[LEN];
        float averageRating;
        int numVotes;
        char isDeleted;

    public:             
        Record(string s);
        string getTconst();
        double getAverageRating();
        int getNumVotes();
        int getRecordBytes() const;
        string toString();
        char getIsDeleted();
        void setIsDeleted();

};

#endif // RECORD_H