/*
Name: Adewole Adeoshun
Course: CSCI 323 (Mon/Wed 3.10pm - 4.25pm)
Instr: Tsaiyun Phillips
ID: 24081306
Project name: Linked-list implementation of stacks, queues, and lists
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
    string data;       // the word
    listNode* next;    // pointer to next node

    listNode(string d) {
        data = d;
        next = NULL;
    }

    // print node format
    void printNode(ofstream &out) {
        if (next != NULL)
            out << "(" << data << ", " << next->data << ") -> ";
        else
            out << "(" << data << ", NULL) -> ";
    }
};

// =======================
// LLStack class
// =======================
class LLStack {
public:
    listNode* top;

    LLStack() {
        top = new listNode("dummy");
    }

    // push node after dummy
    void push(listNode* newNode) {
        newNode->next = top->next;
        top->next = newNode;
    }

    // pop: remove first real node after dummy
    listNode* pop(ofstream &outFile1) {
        if (top->next == NULL) {
            outFile1 << "stack is empty" << endl;
            return NULL;
        }
        listNode* tmp = top->next;
        top->next = tmp->next;
        tmp->next = NULL;
        return tmp;
    }

    bool isEmpty() {
        return top->next == NULL;
    }

    void printStack(ofstream &out) {
        out << "Top -> ";
        listNode* spot = top;
        while (spot != NULL) {
            spot->printNode(out);
            spot = spot->next;
        }
        out << "NULL" << endl;
    }
};

// =======================
// LLQueue class
// =======================
class LLQueue {
public:
    listNode* head;
    listNode* tail;

    LLQueue() {
        head = new listNode("dummy");
        tail = head;
    }

    void insertQ(listNode* newNode) {
        tail->next = newNode;
        tail = newNode;
    }

    listNode* deleteQ(ofstream &outFile2, ofstream &logFile) {
        logFile << "entering deleteQ()" << endl;

        if (head == tail) {
            outFile2 << "Q is empty" << endl;
            logFile << "Q is empty" << endl;
            logFile << "leaving deleteQ()" << endl;
            return NULL;
        }

        listNode* temp = head->next;
        head->next = temp->next;

        if (tail == temp)
            tail = head;

        temp->next = NULL;

        outFile2 << "delete a node from Q, newNode’s data is " << temp->data << endl;
        logFile << "delete a node from Q, newNode’s data is " << temp->data << endl;
        logFile << "leaving deleteQ()" << endl;
        return temp;
    }

    bool isEmpty() {
        return head == tail;
    }

    void printQ(ofstream &out) {
        out << "Head -> ";
        listNode* spot = head;
        while (spot != NULL) {
            spot->printNode(out);
            spot = spot->next;
        }
        out << "Tail" << endl;
    }
};

// =======================
// LLList class
// =======================
class LLList {
public:
    listNode* listHead;

    LLList() {
        listHead = new listNode("dummy");
    }

    listNode* findSpot(listNode* newNode, ofstream &logFile) {
        logFile << "entering findSpot()" << endl;
        listNode* spot = listHead;

        while (spot->next != NULL && spot->next->data < newNode->data) {
            spot = spot->next;
        }

        logFile << "Spot’s data is " << spot->data << endl;
        logFile << "leaving findSpot()" << endl;
        return spot;
    }

    void insertOneNode(listNode* spot, listNode* newNode) {
        newNode->next = spot->next;
        spot->next = newNode;
    }

    void printList(ofstream &out) {
        out << "listHead -> ";
        listNode* spot = listHead;
        while (spot != NULL) {
            spot->printNode(out);
            spot = spot->next;
        }
        out << "NULL" << endl;
    }
};

// =======================
// buildStack
// =======================
void buildStack(LLStack &S, ifstream &inFile, ofstream &logFile) {
    logFile << "entering buildStack()" << endl;
    string word;
    while (inFile >> word) {
        logFile << "input data is " << word << endl;
        listNode* newNode = new listNode(word);
        S.push(newNode);
    }
    logFile << "leaving buildStack()" << endl;
}

// =======================
// buildQueue
// =======================
void buildQueue(LLStack &S, LLQueue &Q, ofstream &outFile1, ofstream &logFile) {
    logFile << "entering buildQueue()" << endl;
    while (!S.isEmpty()) {
        listNode* newNode = S.pop(outFile1);
        if (newNode != NULL) {
            outFile1 << "after pop stack, newNode’s data is " << newNode->data << endl;
            logFile << "after pop stack, newNode’s data is " << newNode->data << endl;
            Q.insertQ(newNode);
        }
    }
    logFile << "leaving buildQueue()" << endl;
}

// =======================
// buildList
// =======================
void buildList(LLQueue &Q, LLList &L, ofstream &outFile2, ofstream &logFile) {
    logFile << "entering buildList()" << endl;
    while (!Q.isEmpty()) {
        listNode* newNode = Q.deleteQ(outFile2, logFile);
        if (newNode != NULL) {
            listNode* spot = L.findSpot(newNode, logFile);
            L.insertOneNode(spot, newNode);
        }
    }
    logFile << "leaving buildList()" << endl;
}

// =======================
// main
// =======================
int main(int argc, char* argv[]) {
    if (argc != 6) {
        cout << "Usage: " << argv[0] << " infile outFile1 outFile2 outFile3 logFile" << endl;
        return 1;
    }

    ifstream inFile(argv[1]);
    ofstream outFile1(argv[2]);
    ofstream outFile2(argv[3]);
    ofstream outFile3(argv[4]);
    ofstream logFile(argv[5]);

    if (!inFile || !outFile1 || !outFile2 || !outFile3 || !logFile) {
        cout << "Error opening one of the files" << endl;
        return 1;
    }

    // Step 1: build stack
    LLStack S;
    logFile << "calling buildStack()" << endl;
    buildStack(S, inFile, logFile);
    S.printStack(outFile1);

    // Step 2: build queue
    LLQueue Q;
    logFile << "calling buildQueue()" << endl;
    buildQueue(S, Q, outFile1, logFile);
    Q.printQ(outFile2);

    // Step 3: build list
    LLList L;
    logFile << "calling buildList()" << endl;
    buildList(Q, L, outFile2, logFile);

    // Step 4: print final list
    logFile << "Printing list" << endl;
    L.printList(logFile);
    L.printList(outFile3);

    inFile.close();
    outFile1.close();
    outFile2.close();
    outFile3.close();
    logFile.close();

    return 0;
}
