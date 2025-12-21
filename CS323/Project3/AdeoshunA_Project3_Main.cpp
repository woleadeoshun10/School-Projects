/*
Name: Adewole Adeoshun
Course: CSCI 323 (MonWed 3.10pm - 4.25pm)
Instructor: Tsaiyun Phillips
ID: 24081306
Project 3 - Radix Sort 
*/

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// =======================
// listNode class
// =======================
class listNode {
public:
    string data; // holds string data
    listNode* next; // pointer to next node

    // constructor makes node with data
    listNode(string d) {
        data = d;
        next = NULL;
    }

    // print one node in (data,next) form
    void printNode(ofstream &out) {
        if (next != NULL)
            out << "(" << data << ", " << next->data << ") -> ";
        else
            out << "(" << data << ", NULL) -> ";
    }
};

// =======================
// Queue class
// =======================
class LLQueue {
public:
    listNode* head; // dummy node
    listNode* tail; // last node

    // constructor creates dummy
    LLQueue() {
        head = new listNode("dummy");
        tail = head;
    }

    // insert newNode at the end
    void insertQ(listNode* newNode) {
        tail->next = newNode;
        tail = newNode;
    }

    // delete node after dummy
    listNode* deleteQ() {
        if (head == tail) return NULL; // empty queue
        listNode* temp = head->next;   // first real node
        head->next = temp->next;       // skip over node
        if (tail == temp) tail = head; // reset if last node removed
        temp->next = NULL;             // detach node
        return temp;
    }

    // check empty
    bool isEmpty() {
        return head == tail;
    }

    // print queue from dummy down
    void printQ(ofstream &out) {
        listNode* spot = head;
        while (spot != NULL) {
            spot->printNode(out);
            spot = spot->next;
        }
        out << "NULL" << endl;
    }
};

// =======================
// ASCCII
// =======================
const int TABLE_SIZE = 256; // ascii size
int longestStringLength = 0; // keep max length
int currentIndex = 0; // which char we are on
int currentTable = 0; // which table is active
LLQueue* hashTable[2][TABLE_SIZE]; // 2 tables with 256 queues

// =======================
// padString
// =======================
string padString(string s) {
    while (s.length() < longestStringLength) {
        s = " " + s; // pad left with spaces
    }
    return s;
}

// =======================
// printTable
// =======================
void printTable(int tableNum, ofstream &out) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        out << "Table[" << tableNum << "][" << i << "]: ";
        hashTable[tableNum][i]->printQ(out);
    }
}

// =======================
// firstReading
// =======================
void firstReading(ifstream &inFile, ofstream &logFile) {
    string data;
    while (inFile >> data) {
        if ((int)data.length() > longestStringLength) {
            longestStringLength = data.length(); // update longest
        }
    }
    logFile << "*** longestStringLength = " << longestStringLength << " ***" << endl;
}

// =======================
// populateFirstTable
// =======================
void populateFirstTable(ifstream &inFile, ofstream &outFile, ofstream &logFile) {
    logFile << "*** Entering populateFirstTable() ***" << endl;
    string data;
    currentIndex = longestStringLength - 1; // start with rightmost char
    currentTable = 0; // use table 0 first

    while(inFile >> data) {
        string paddedData = padString(data); // pad shorter words
        listNode* newNode = new listNode(paddedData); // create node
        int hashIndex = (int)paddedData[currentIndex]; // char ascii code
        hashTable[currentTable][hashIndex]->insertQ(newNode); // insert into queue

        // log info
        logFile << "*** paddedData = " << paddedData 
                << " ; currentIndex = " << currentIndex
                << " ; hashIndex = " << hashIndex
                << " ; currentTable = " << currentTable << " ***" << endl;
    }

    // print table to out and log
    printTable(currentTable, logFile);
    printTable(currentTable, outFile);

    logFile << "*** Leaving populateFirstTable() ***" << endl;
}

// =======================
// radixSort
// =======================
void radixSort(ofstream &outFile, ofstream &logFile) {
    logFile << "*** Entering radixSort() ***" << endl;

    int oldTable = currentTable;        // start with table used in populate
    int newTable = (currentTable + 1) % 2; // alternate tables

    // go from rightmost index down to 0
    for (int i = currentIndex - 1; i >= 0; i--) {
        logFile << "*** currentIndex = " << i << " ***" << endl;

        // reset newTable queues
        for (int j = 0; j < TABLE_SIZE; j++) {
            hashTable[newTable][j] = new LLQueue();
        }

        // move nodes from old table to new table
        for (int j = 0; j < TABLE_SIZE; j++) {
            while (!hashTable[oldTable][j]->isEmpty()) {
                listNode* node = hashTable[oldTable][j]->deleteQ(); // take out
                int hashIndex = (int)node->data[i]; // char ascii
                hashTable[newTable][hashIndex]->insertQ(node); // put into new

                // log details
                logFile << "*** moving data = " << node->data
                        << " ; from Table " << oldTable
                        << " ; to Table " << newTable
                        << " ; hashIndex = " << hashIndex << " ***" << endl;
            }
        }

        // print table after pass
        printTable(newTable, logFile);
        printTable(newTable, outFile);

        oldTable = newTable;            // swap for next round
        newTable = (oldTable + 1) % 2;
    }

    currentTable = oldTable;            // final table is oldTable
    logFile << "*** Leaving radixSort() ***" << endl;
}

// =======================
// main
// =======================
int main(int argc, char* argv[]) {
    if (argc != 4) {
        cout << "Usage: " << argv[0] << " <inFile> <outFile> <logFile>" << endl;
        return 1;
    }

    ifstream inFile(argv[1]);    // input file
    ofstream outFile(argv[2]);   // output file
    ofstream logFile(argv[3]);   // log file

    if (!inFile || !outFile || !logFile) {
        cout << "Error opening one of the files" << endl;
        return 1;
    }

    // init all queues for both tables
    for (int t = 0; t < 2; t++) {
        for (int i = 0; i < TABLE_SIZE; i++) {
            hashTable[t][i] = new LLQueue();
        }
    }

    // step1: first reading
    firstReading(inFile, logFile);
    inFile.close();

    // step2: reopen and populate
    inFile.open(argv[1]);
    populateFirstTable(inFile, outFile, logFile);
    inFile.close();

    // step3: radix sort
    radixSort(outFile, logFile);

    // close all files
    outFile.close();
    logFile.close();

    return 0;
}

