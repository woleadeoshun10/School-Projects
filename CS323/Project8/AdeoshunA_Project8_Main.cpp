#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

class Node {
public:
    int ID;
    Node* next;

    Node(int d){
        ID = d;
        next = nullptr;
    }
};

class Coloring {
public:
    int numNodes;
    int numUncolor;
    Node** hashTable;
    int* colorAry;

    Coloring(int n){
        numNodes = n;
        numUncolor = n;

        hashTable = new Node*[numNodes + 1];
        for(int i = 1; i <= numNodes; i++){
            hashTable[i] = new Node(-999);
        }

        colorAry = new int[numNodes + 1];
        for(int i = 1; i <= numNodes; i++){
            colorAry[i] = 0;
        }
    }

    void hashInsert(int id1, int id2){
        Node* newNode = new Node(id2);
        newNode->next = hashTable[id1]->next;
        hashTable[id1]->next = newNode;
    }

    void loadGraph(ifstream &inFile){
        int i, j;
        while(inFile >> i >> j){
            hashInsert(i, j);
            hashInsert(j, i);
        }
    }

    void printHashTable(ofstream &outFile){
        for(int i = 1; i <= numNodes; i++){
            outFile << "hashTable[" << i << "] -> ";
            Node* curr = hashTable[i]->next;

            while(curr != nullptr){
                outFile << curr->ID << " -> ";
                curr = curr->next;
            }
            outFile << "NULL\n";
        }
        outFile << endl;
    }

    bool checkNeighbors(int nodeID, int color){
        Node* nextNode = hashTable[nodeID]->next;
        while(nextNode != nullptr){
            if(colorAry[nextNode->ID] == color){
                return false;
            }
            nextNode = nextNode->next;
        }
        return true;
    }

    void method1(ofstream &outFile1, ofstream &logFile){
        logFile << "*** Entering Method 1 ***\n";
        int newColor = 64;

        while(numUncolor > 0){
            newColor++;
            int nodeID = 1;

            while(nodeID <= numNodes){
                if(colorAry[nodeID] == 0){
                    if(checkNeighbors(nodeID, newColor)){
                        colorAry[nodeID] = newColor;
                        numUncolor--;
                    }
                }
                nodeID++;
            }
            logFile << "*** Printing colorARY\n";
            printAry(logFile, 1);
        }
        logFile << "*** Leaving Method 1\n";
    }

    void method2(ofstream &outFile1, ofstream &logFile){
        logFile << "*** Entering Method 2 ***\n";
        int lastUsedColor = 64;
        int nextNodeID = 0;

        while(nextNodeID < numNodes){
            nextNodeID++;
            int nextUsedColor = 65;
            bool coloredFlag = false;

            while(coloredFlag == false && nextUsedColor <= lastUsedColor){
                if(lastUsedColor > 64 && checkNeighbors(nextNodeID, nextUsedColor)){
                    colorAry[nextNodeID] = nextUsedColor;
                    coloredFlag = true;
                }
                else nextUsedColor++;
            }

            if(coloredFlag == false){
                lastUsedColor++;
                colorAry[nextNodeID] = lastUsedColor;
                logFile << "lastUsedColor is " << lastUsedColor << endl;
            }

            logFile << "*** Printing colorARY\n";
            printAry(logFile, 2);
        }

        logFile << "*** Leaving Method 2\n";
    }

    void printAry(ofstream &oFile, int whatMethod){
        oFile << ">> Method " << whatMethod
              << " is used to color the input graph of number of nodes = "
              << numNodes << " <<" << endl;

        oFile << ">> Below is the result of the color assignment <<\n";

        for(int i = 1; i <= numNodes; i++){
            char colorChar = (char) colorAry[i];
            oFile << i << " " << colorChar << endl;
        }
        oFile << endl;
    }

};

int main(int argc, char** argv){
    // step 0
    if(argc != 5){
        cout << "Usage: ./a.out inFile method outFile logFile" << endl;
        exit(1);
    }

    ifstream inFile(argv[1]);
    int whichMethod = atoi(argv[2]);
    ofstream outFile1(argv[3]);
    ofstream logFile(argv[4]);


    if(!inFile.is_open() || !outFile1.is_open() || !logFile.is_open()){
        cout << "Error opening one of the files!" << endl;
        exit(1);
    }

    int numNodes;
    inFile >> numNodes;

    Coloring graph(numNodes);

    // step 1
    graph.loadGraph(inFile);

    // step 2
    outFile1 << "*** Below is the Hash table of input graph ***\n";
    graph.printHashTable(outFile1);

    // step 4
    switch(whichMethod){
        case 1:
            graph.method1(outFile1, logFile);
            break;
        case 2:
            graph.method2(outFile1, logFile);
            break;
        default:
            logFile << "ERROR: argv[2] only accepts 1 or 2\n";
            exit(1);
    }

    // step 5
    graph.printAry(outFile1, whichMethod);
    
   // close files
    inFile.close();
    outFile1.close();
    logFile.close();

    return 0;
}