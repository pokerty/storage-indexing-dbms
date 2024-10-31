#include <iostream>
#include <string>
#include "Experiments.cpp"
#include "bplustree.h"
#include <vector>


using namespace std;


int main() {
    const string PARAGRAPHBREAK = "------------------------------------";
    int blockSize = 200;
    char choice;
    int index;
    string dataPath = "data.tsv";
     // Initialize the experiment object for this project
    Experiments diskInitialisation(dataPath, blockSize);
    // Initialize the experiment object for this project
    Experiments project(dataPath, blockSize);

    while (true) {
        cout << PARAGRAPHBREAK << endl;
        cout << "Experiment 1: Store data on the disk" << endl;
        cout << "Experiment 2: Build B+ tree on \"numVotes\"" << endl;
        cout << "Experiment 3: Retrieve movies with \"numVotes\" equal 500" << endl;
        cout << "Experiment 4: Retreve movies with \"numVotes\" from 30,000 to 40,000" << endl;
        cout << "Experiment 5: Delete movies with \"numVotes\" equal 1000" << endl;
        cout << "Option 6: Exit the program" << endl;
        cout << PARAGRAPHBREAK << endl;
        cout << "Select an experiment (1-5), or '6' to exit program: ";
        cin >> index;

        // invalid input by user
        while (cin.fail() || index < 1 || index > 6) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Please select only number from 1-6: " ;
            cin >> index;
        }

        switch(index) {
            case 1:
                cout << "\n" << "/// Starting Experiment 1: Store the data on the Disk" << endl;;
                diskInitialisation.experiment1();
                printf("/// Exited experiment 1\n");
                break;
            case 2:
                cout << "\n" << "/// Starting Experiment 2: Build a B+ tree on attribute \"numVotes\"" << endl;;
                project.buildBTree();
                project.experiment2();
                printf("/// Exited experiment 2\n");
                break;
            case 3:
                cout << "\n" << "/// Starting Experiment 3: retrieve those movies with the \"numVotes\" equal to 500" << endl;
                project.experiment3();
                printf("/// Exited experiment 3\n");
                break;
            case 4:
                cout << "\n" << "/// Starting Experiment 4: retrieve those movies with the attribute \"numVotes\" from 30,000 to 40,000, both inclusively" << endl;
                project.experiment4();
                printf("/// Exited experiment 4\n");
                break;
            case 5:
                cout << "\n" << "/// Starting Experiment 5: delete those movies with the attribute \"numVotes\" equal to 1,000, update the B+ tree accordingly" << endl;
                project.experiment5();
                printf("/// Exited experiment 5\n");
                break;
            case 6:
                printf("...Exiting program");
                return 0;
            default:
                cout << "Please select a valid number 1-6";
        }
    }

}
