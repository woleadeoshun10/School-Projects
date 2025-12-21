/*
Name: Adewole Adeoshun
Course: CSCI 323 (Mon/Wed)
Instructor: Tsaiyun Phillips
ID: 24081306
Minimum Spending Tree using Kruskal's method
*/


#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

// Edge class
class Edge {
public:
    int nU;
    int nW;
    int cost;
    Edge* next;

    // constructor
    Edge(int u, int w, int c) {
        nU = u;
        nW = w;
        cost = c;
        next = nullptr;
    }

    // print edge
    void printEdge(ofstream &outFile1) {
        outFile1 << "(" << nU << ", " << nW << ", " << cost << ") -> ";
    }
};

// KruskalMST class
class KruskalMST {
public:
    int N;
    int* whichSet;
    int numSets;
    int totalMSTcost;
    Edge* edgeList;
    Edge* mstList;

    // constructor
    KruskalMST(int numOfNodes) {
        N = numOfNodes;
        numSets = N;
        totalMSTcost = 0;

        whichSet = new int[N+1];
        for (int i = 1; i <= N; i++){
            whichSet[i] = i;
        }

        edgeList = new Edge(0, 0, 0);
        mstList  = new Edge(0, 0, 0);
    }

    // find spot
    Edge* findSpot(Edge* listHead, Edge* newEdge) {
        Edge* Spot = listHead;
        while (Spot->next != nullptr && Spot->next->cost < newEdge->cost) {
            Spot = Spot->next;
        }
        return Spot;
    }

    // insert edge
    void insertEdge(Edge* newEdge, Edge* listHead) {
        Edge* Spot = findSpot(listHead, newEdge);
        newEdge->next = Spot->next;
        Spot->next = newEdge;
    }

    // pop node after dummy
    Edge* removeEdge(Edge* listHead) {
        if (listHead->next == nullptr) return nullptr;
        Edge* tmp = listHead->next;
        listHead->next = tmp->next;
        tmp->next = nullptr;
        return tmp;
    }

    // merge2Sets
    void merge2Sets(int Ni, int Nj) {
        if (whichSet[Ni] < whichSet[Nj]) {
            updateWhichSet(whichSet[Nj], whichSet[Ni]);
        } else {
            updateWhichSet(whichSet[Ni], whichSet[Nj]);
        }
    }

    // updateWhichSet
    void updateWhichSet(int a, int b) {
        for (int i = 1; i <= N; i++) {
            if (whichSet[i] == a) {
                whichSet[i] = b;
            }
        }
    }

    // print Array
    void printAry(ofstream &logFile) {
        logFile << "nodeID\twhichSet\n";
        logFile << "==================\n";

        for (int i = 1; i <= N; i++) {
            logFile << setw(5) << i << "\t" << setw(5) << whichSet[i] << endl;
        }
        logFile << endl;
    }

    // printList
    void printList(Edge* listHead, ofstream &oFile) {
        Edge* curr = listHead;
        oFile << "listHead -> ";
        while (curr != nullptr) {
            oFile << "<" << curr->nU << ", " << curr->nW << ", " << curr->cost << "> -> ";
            curr = curr->next;
        }
        oFile << "NULL\n" << endl;
    }

    // printMST
    void printMST(ofstream &outFile1) {
        outFile1 << "*** A Kruskal’s MST of the input graph is given below: ***\n";
        outFile1 << N << endl;

        Edge* curr = mstList->next;
        while (curr != nullptr) {
            outFile1 << curr->nU << " " << curr->nW << " " << curr->cost << endl;
            curr = curr->next;
        }
        outFile1 << "*** The total cost of a Kruskal's MST is: "
                 << totalMSTcost << endl;
    }
};

// main
int main(int argc, char** argv) {

    if (argc != 4) {
        cout << "Usage: ./a.out inFile outFile logFile\n";
        return 1;
    }

    ifstream inFile(argv[1]);
    ofstream outFile1(argv[2]);
    ofstream logFile(argv[3]);

    if (!inFile.is_open() || !outFile1.is_open() || !logFile.is_open()) {
        cout << "Error opening one of the files" << endl;
        exit(1);
    }

    int N;
    inFile >> N;

    KruskalMST mst(N);

    //step 1-4

    int u, w, cost;
    while (inFile >> u >> w >> cost) {

        Edge* newEdge = new Edge(u, w, cost);

        logFile << "*** In main() newEdge from inFile is ";
        newEdge->printEdge(logFile);
        logFile << endl;

        mst.insertEdge(newEdge, mst.edgeList);

        logFile << "*** In main() Printing edgeList after insert the new edge:\n";
        mst.printList(mst.edgeList, logFile);
    }
    logFile << "*** In main() At the end of printing all edges of the input graph\n\n";
    

    //step 5-9
    while (mst.numSets > 1) {

        Edge* nextEdge = mst.removeEdge(mst.edgeList);
        while (nextEdge != nullptr &&
               mst.whichSet[nextEdge->nU] == mst.whichSet[nextEdge->nW]) {
            nextEdge = mst.removeEdge(mst.edgeList);
        }

        if (nextEdge == nullptr) break;

        logFile << "*** In main() the nextEdge is ";
        nextEdge->printEdge(logFile);
        logFile << endl;

        mst.insertEdge(nextEdge, mst.mstList);
        mst.totalMSTcost += nextEdge->cost;

        mst.merge2Sets(nextEdge->nU, nextEdge->nW);
        mst.numSets--;

        logFile << "In main(): after merge two sets: numSets is "
                << mst.numSets << endl;

        logFile << "*** In main(): Printing whichSet array\n";
        mst.printAry(logFile);

        logFile << "*** In main(): Printing the remaining of edgeList\n";
        mst.printList(mst.edgeList, logFile);

        logFile << "*** In main(): Print the growing MST list\n";
        mst.printList(mst.mstList, logFile);
    }
    
    // step 10-12
    outFile1 << "*** In main(): At the end, printing the final whichSet array\n";
    mst.printAry(outFile1);

    mst.printMST(outFile1);

    // close files
    inFile.close();
    outFile1.close();
    logFile.close();

    return 0;
}

