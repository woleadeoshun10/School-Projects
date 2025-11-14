/*
Name: Adewole Adeoshun
Course: CSCI 323 (Mon/Wed)
Instructor: Tsaiyun Phillips
ID: 24081306
All pair shortest paths using Dijkstra’s algorithm
*/

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
using namespace std;

#define INF 999

class SSS {
public:
    int N;
    int sourceNode;
    int minNode;
    int currentNode;
    int newCost;

    int **costMatrix;
    int *ToDo;
    int *Best;
    int *Parent;

    ofstream SSSfile;
    ofstream logFile;

    SSS(int nodes) {
        N = nodes;
        costMatrix = nullptr;
        ToDo = nullptr;
        Best = nullptr;
        Parent = nullptr;
    }

    void initMatrix() {
        costMatrix = new int*[N + 1];
        for (int i = 0; i <= N; i++)
            costMatrix[i] = new int[N + 1];

        for (int i = 1; i <= N; i++) {
            for (int j = 1; j <= N; j++) {
                if (i == j)
                    costMatrix[i][j] = 0;
                else
                    costMatrix[i][j] = INF;
            }
        }
    }

    void loadCostMatrix(ifstream &inFile) {
        int n1, n2, cost;
        while (inFile >> n1 >> n2 >> cost) {
            costMatrix[n1][n2] = cost;
        }
    }

    void initArys(int s) {
        sourceNode = s;
        ToDo = new int[N + 1];
        Best = new int[N + 1];
        Parent = new int[N + 1];

        for (int i = 1; i <= N; i++) {
            Best[i] = costMatrix[sourceNode][i];
            Parent[i] = sourceNode;
            ToDo[i] = 1;
        }
        ToDo[sourceNode] = 0;
    }

    int findMinNode() {
        logFile << "Entering findMinNode() method" << endl;
        int minCost = INF;
        int index = 1;
        minNode = 0;

        while (index <= N) {
            if (ToDo[index] == 1 && Best[index] < minCost) {
                minCost = Best[index];
                minNode = index;
            }
            index++;
        }

        logFile << "Leaving findMinNode(): minNode = " << minNode
                << "  minCost = " << minCost << endl;
        return minNode;
    }

    bool checkToDo() {
        for (int i = 1; i <= N; i++) {
            if (ToDo[i] != 0)
                return false;
        }
        return true;
    }

    void printAry(int *Ary, ofstream &oFile) {
        oFile << "Index\tParent" << endl;
        oFile << "==================" << endl;
        for (int i = 1; i <= N; i++) {
            oFile << setw(5) << i << "\t" << setw(5) << Ary[i] << endl;
        }
        oFile << endl;
    }

    void printShortestPath(int currentNode, int sourceNode) {
        SSSfile << "The path from " << sourceNode << " to " << currentNode << " : ";
        int father = Parent[currentNode];
        SSSfile << currentNode;
        while (father != sourceNode) {
            SSSfile << " <- " << father;
            father = Parent[father];
        }
        SSSfile << " <- " << sourceNode;
        SSSfile << " : cost = " << Best[currentNode] << endl;
    }
    
    //got help with this 
    void linkFiles(ofstream &outSSS, ofstream &outLog) {
        SSSfile.copyfmt(outSSS);
        SSSfile.basic_ios<char>::rdbuf(outSSS.rdbuf());
        logFile.copyfmt(outLog);
        logFile.basic_ios<char>::rdbuf(outLog.rdbuf());
    }

  //Deconstructor
    ~SSS() {
        for (int i = 0; i <= N; i++)
            delete[] costMatrix[i];
        delete[] costMatrix;
    }
};

int main(int argc, char **argv) {
    if (argc != 4) {
        cout << "Usage: ./a.out Data1.txt SSSfile.txt logFile.txt" << endl;
        return 1;
    }

    ifstream inFile(argv[1]);
    ofstream SSSfile(argv[2]);
    ofstream logFile(argv[3]);

    if (!inFile || !SSSfile || !logFile) {
        cout << "Error: could not open one or more files." << endl;
        return 1;
    }

    int N;
    inFile >> N;
    SSS sss(N);
    sss.linkFiles(SSSfile, logFile);

    sss.initMatrix();
    sss.loadCostMatrix(inFile);

    sss.SSSfile << "==================================================" << endl;
    sss.SSSfile << "There are " << N << " nodes in the input graph. Below are all pairs of shortest paths:" << endl;
    sss.SSSfile << "==================================================" << endl << endl;

    sss.sourceNode = 1;
    while (sss.sourceNode <= sss.N) {
        sss.initArys(sss.sourceNode);

        do {
            sss.minNode = sss.findMinNode();
            logFile << "In main(), minNode is " << sss.minNode << endl;
            sss.ToDo[sss.minNode] = 0;

            int childNode = 1;
            while (childNode <= sss.N) {
                if (sss.ToDo[childNode] == 1) {
                    sss.newCost = sss.Best[sss.minNode] + sss.costMatrix[sss.minNode][childNode];
                    if (sss.newCost < sss.Best[childNode]) {
                        sss.Best[childNode] = sss.newCost;
                        sss.Parent[childNode] = sss.minNode;
                    }
                }
                childNode++;
            }
        } while (!sss.checkToDo());

        sss.SSSfile << endl;
        if (sss.sourceNode == 1)
            sss.SSSfile << "Source node = " << sss.sourceNode << endl;
        else
            sss.SSSfile << "The source node = " << sss.sourceNode << endl;

        for (sss.currentNode = 1; sss.currentNode <= sss.N; sss.currentNode++)
            sss.printShortestPath(sss.currentNode, sss.sourceNode);

        sss.SSSfile << endl;
        sss.SSSfile << "===========================" << endl;
        sss.SSSfile << endl;

        sss.SSSfile << "*** Below is ToDo array ***" << endl;
        sss.printAry(sss.ToDo, sss.SSSfile);
        sss.SSSfile << "*** Below is Best array ***" << endl;
        sss.printAry(sss.Best, sss.SSSfile);
        sss.SSSfile << "*** Below is Parent array ***" << endl;
        sss.printAry(sss.Parent, sss.SSSfile);

        sss.sourceNode++;
    }

    inFile.close();
    SSSfile.close();
    logFile.close();

    cout << "Program completed successfully." << endl;
    return 0;
}


