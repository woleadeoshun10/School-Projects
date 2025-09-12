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
// each node holds a word (string) and a link to the next node
class listNode {
public:
    string data;       // the actual word
    listNode* next;    // pointer to the next node

    // constructor: initialize node with a word, next = NULL
    listNode(string d) {
        data = d;
        next = NULL;
    }

    // print the node in format: (currentData, nextData) ->
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
// simple linked list implementation of a stack (LIFO)
class LLStack {
public:
    listNode* top;  // dummy node at the top of the stack

    // constructor: create dummy node so stack is never "totally" empty to prevent program crash
    LLStack() {
        top = new listNode("dummy");
    }

    // push: put newNode right after the dummy
    void push(listNode* newNode) {
        newNode->next = top->next;
        top->next = newNode;
    }

    // pop: remove the first real node after dummy and return it
    listNode* pop(ofstream &logFile) {
        if (top->next == NULL) { // no real nodes
            logFile << "stack is empty" << endl;
            return NULL;
        }
        listNode* tmp = top->next; // take the node right after dummy
        top->next = tmp->next;     // top should point to temp's next pointer
        tmp->next = NULL;          // detach it completely to prevent unwanted links
        return tmp;                // return the popped node
    }

    // true if no real nodes exist
    bool isEmpty() {
        return top->next == NULL;
    }

    // print whole stack from dummy down
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
// linked list implementation of queue (FIFO)
class LLQueue {
public:
    listNode* head; // dummy node at the front
    listNode* tail; // always points to last node

    // constructor: head = dummy, tail starts same as head
    LLQueue() {
        head = new listNode("dummy");
        tail = head;
    }

    // enqueue: put newNode at the end
    void insertQ(listNode* newNode) {
        tail->next = newNode;
        tail = newNode;
    }

    // dequeue: remove node right after dummy
    listNode* deleteQ(ofstream &logFile) {
        logFile << "entering deleteQ()" << endl;

        // if queue is empty
        if (head == tail) {
            logFile << "Q is empty" << endl;
            logFile << "leaving deleteQ()" << endl;
            return NULL;
        }

        // temp points to first real node
        listNode* temp = head->next;
        // dummy skips over it
        head->next = temp->next;

        // if we removed the last node, tail resets to dummy
        if (tail == temp) tail = head;

        // detach node completely
        temp->next = NULL;

        logFile << "delete a node from Q, newNode’s data is " << temp->data << endl;
        logFile << "leaving deleteQ()" << endl;
        return temp;
    }

    // true if only dummy is left
    bool isEmpty() {
        return head == tail;
    }

    // print queue contents from head (dummy) to tail
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
// linked list that keeps nodes sorted alphabetically
class LLList {
public:
    listNode* listHead;

    // constructor: dummy node as head
    LLList() {
        listHead = new listNode("dummy");
    }

    // find where to insert the new node (sorted order)
    listNode* findSpot(listNode* newNode, ofstream &logFile) {
        logFile << "entering findSpot()" << endl;
        listNode* spot = listHead;

        // keep moving until we find the right place in ascending order
        while (spot->next != NULL && spot->next->data < newNode->data) {
            spot = spot->next;
        }

        logFile << "Spot’s data is " << spot->data << endl;
        logFile << "leaving findSpot()" << endl;
        return spot;
    }

    // insert newNode after spot
    void insertOneNode(listNode* spot, listNode* newNode) {
        newNode->next = spot->next;
        spot->next = newNode;
    }

    // print full list
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
// read words from file, create nodes, push onto stack
void buildStack(LLStack &S, ifstream &inFile, ofstream &logFile) {
    logFile << "entering buildStack()" << endl;
    string word;
    while (inFile >> word) { // read word by word
        logFile << "input data is " << word << endl;
        listNode* newNode = new listNode(word);
        S.push(newNode); // put word on stack
    }
    logFile << "leaving buildStack()" << endl;
}

// =======================
// buildQueue
// =======================
// pop from stack, enqueue into queue
void buildQueue(LLStack &S, LLQueue &Q, ofstream &logFile) {
    logFile << "entering buildQueue()" << endl;
    while (!S.isEmpty()) {
        listNode* newNode = S.pop(logFile);
        if (newNode != NULL) {
            logFile << "after pop stack, newNode’s data is " << newNode->data << endl;
            Q.insertQ(newNode); // enqueue node
        }
    }
    logFile << "leaving buildQueue()" << endl;
}

// =======================
// buildList
// =======================
// dequeue from queue, insert into sorted linked list
void buildList(LLQueue &Q, LLList &L, ofstream &logFile) {
    logFile << "entering buildList()" << endl;
    while (!Q.isEmpty()) {
        listNode* newNode = Q.deleteQ(logFile);
        if (newNode != NULL) {
            listNode* spot = L.findSpot(newNode, logFile);
            L.insertOneNode(spot, newNode); // place node in sorted position
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

    // open all files from argv
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
    S.printStack(outFile1); // snapshot stack into output1.txt

    // Step 2: build queue
    LLQueue Q;
    logFile << "calling buildQueue()" << endl;
    buildQueue(S, Q, logFile);
    Q.printQ(outFile2); // snapshot queue into output2.txt

    // Step 3: build ordered list
    LLList L;
    logFile << "calling buildList()" << endl;
    buildList(Q, L, logFile);

    // Step 4: print list
    logFile << "Printing list" << endl;
    L.printList(logFile);  // also print list into log for trace
    L.printList(outFile3); // final sorted list into output3.txt

    // close files
    inFile.close();
    outFile1.close();
    outFile2.close();
    outFile3.close();
    logFile.close();

    return 0;
}
