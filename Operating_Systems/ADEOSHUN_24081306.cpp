/*
Name: Adewole Adeoshun
Course: CSCI 340 (Operating Systems)
Instructor: Professor John S
ID: 24081306
IPC Pipe + Multithread Sum Program
*/


// Usage: ./ADEOSHUN_24081306.exe 1 lb1 ub1 lb2 ub2 ...
// Example: ./ADEOSHUN_24081306.exe 1 1 100 101 200 201 300 301 400 401 500

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
using namespace std;

// global variables
long gSum = 0;           // grand total sum, accumulated after all threads join
long *pSumArr = nullptr; // array of partial sums, one slot per thread
int  gNumPairs = 0;      // number of lower/upper bound pairs

// MyParam class
// used to pass lower bound, upper bound, and thread index into each pthread
class MyParam {
public:
    MyParam(long l, long u, int i) : lb(l), ub(u), lid(i) {}
    long getLower() { return lb; }
    long getUpper() { return ub; }
    int  getLid()   { return lid; }
private:
    long lb, ub;
    int  lid;
};

// myrunner
// pthread function, each thread sums its assigned range and stores result in pSumArr
void *myrunner(void *param) {
    MyParam *p = (MyParam *) param;
    long lower = p->getLower();
    long upper = p->getUpper();
    int  idx   = p->getLid();

    // sum from lower to upper inclusive
    for (long i = lower; i <= upper; ++i)
        pSumArr[idx] += i;

    // print what this thread did
    string s = "Child: Thread " + to_string(idx) +
               " executed: " + to_string(lower) + "," +
               to_string(upper) + "," + to_string(pSumArr[idx]) + "\n";
    cout << s;

    pthread_exit(0);
}

// main
int main(int argc, const char *argv[]) {

    // check we have at least one lb/ub pair after the flag argument
    // argv[0] = exe, argv[1] = flag, then pairs follow
    if (argc < 4 || (argc - 2) % 2 != 0) {
        cout << "Usage: " << argv[0] << " 1 lb1 ub1 lb2 ub2 ...\n";
        return 1;
    }

    // figure out how many pairs we have
    int numPairs = (argc - 2) / 2;

    // print startup message with all command line args
    cout << "Parent: program started with command line arguments: ";
    for (int i = 1; i < argc; ++i)
        cout << argv[i] << " ";
    cout << endl;

    // convert argv strings to longs and store in data array
    // data[0] = numPairs, then data[1..] = lb0, ub0, lb1, ub1, ...
    int totalInts = 1 + numPairs * 2;
    long *data    = new long[totalInts];
    data[0]       = numPairs;
    for (int i = 0; i < numPairs; ++i) {
        data[1 + i * 2]     = atol(argv[2 + i * 2]);      // lower bound
        data[1 + i * 2 + 1] = atol(argv[2 + i * 2 + 1]); // upper bound
    }

    // create two pipes before forking
    // pipe1: parent writes to child, pipe2: child writes back to parent
    int pipe1[2], pipe2[2];
    if (pipe(pipe1) < 0 || pipe(pipe2) < 0) {
        cout << "Parent: pipe creation failed\n";
        return 2;
    }
    cout << "Parent: pipes created." << endl;

    // fork the child process
    pid_t pid = fork();
    if (pid < 0) {
        cout << "Parent: fork failed\n";
        return 3;
    }

    // child process
    if (pid == 0) {
        cout << "Child: after fork()." << endl;

        // child only reads from pipe1 and writes to pipe2
        // close the ends we don't use
        close(pipe1[1]);
        close(pipe2[0]);

        // read number of pairs first, then read all the bounds
        long nPairs;
        read(pipe1[0], &nPairs, sizeof(long));

        long *bounds = new long[nPairs * 2];
        read(pipe1[0], bounds, nPairs * 2 * sizeof(long));
        close(pipe1[0]);

        // print what we got from pipe1
        cout << "Child: consumed lower and upper bound pairs from pipe1: ";
        for (int i = 0; i < nPairs; ++i)
            cout << bounds[i * 2] << "," << bounds[i * 2 + 1] << " ";
        cout << endl;

        // set up for multithreading
        gNumPairs = (int) nPairs;
        pSumArr   = new long[gNumPairs](); // zero-initialize partial sums

        MyParam       **p    = new MyParam*[gNumPairs];
        pthread_t     *tid   = new pthread_t[gNumPairs];
        pthread_attr_t *attr = new pthread_attr_t[gNumPairs];

        // build param objects for each thread
        for (int i = 0; i < gNumPairs; ++i)
            p[i] = new MyParam(bounds[i * 2], bounds[i * 2 + 1], i);

        // create all pthreads
        for (int i = 0; i < gNumPairs; ++i) {
            pthread_attr_init(&attr[i]);
            pthread_create(&tid[i], &attr[i], myrunner, p[i]);
            string s = "Child: Main Thread: Created thread " + to_string(i) + "\n";
            cout << s;
        }

        // wait for all pthreads to finish
        for (int i = 0; i < gNumPairs; ++i) {
            pthread_join(tid[i], NULL);
            string s = "Child: Main Thread: Joined thread " + to_string(i) + "\n";
            cout << s;
        }

        // add up all partial sums to get grand total
        for (int i = 0; i < gNumPairs; ++i)
            gSum += pSumArr[i];

        cout << "Child: Main Thread: Grand total sum = " << gSum << endl;

        // send grand total back to parent through pipe2
        write(pipe2[1], &gSum, sizeof(long));
        close(pipe2[1]);

        cout << "Child: produced into pipe 2. The grand total sum produced = "
             << gSum << endl;

        // cleanup
        for (int i = 0; i < gNumPairs; ++i) delete p[i];
        delete[] p;
        delete[] tid;
        delete[] attr;
        delete[] pSumArr;
        delete[] bounds;

        int childStatus = 0;
        cout << "Child: terminating with status = " << childStatus << endl;
        return childStatus;
    }

    // parent process
    else {
        cout << "Parent: child process forked with child process id = "
             << pid << endl;

        // parent only writes to pipe1 and reads from pipe2
        // close the ends we don't use
        close(pipe1[0]);
        close(pipe2[1]);

        // write number of pairs first, then all lb/ub pairs into pipe1
        long nPairs = numPairs;
        write(pipe1[1], &nPairs, sizeof(long));
        write(pipe1[1], &data[1], numPairs * 2 * sizeof(long));
        close(pipe1[1]);

        cout << "Parent: produced lower and upper bound pairs into pipe1: ";
        for (int i = 0; i < numPairs; ++i)
            cout << data[1 + i * 2] << "," << data[1 + i * 2 + 1] << " ";
        cout << endl;

        // read the grand total that child sends back through pipe2
        long grandTotal = 0;
        read(pipe2[0], &grandTotal, sizeof(long));
        close(pipe2[0]);

        cout << "Parent: consumed from pipe 2. The grand total sum consumed = "
             << grandTotal << endl;

        // wait for child to terminate and get its exit status
        int status;
        if (wait(&status) == pid) {
            if (WIFEXITED(status)) {
                int childStat = WEXITSTATUS(status);
                cout << "Parent: child terminated with return status = "
                     << childStat << endl;
            } else if (WIFSIGNALED(status)) {
                cout << "Parent: child terminated abnormally\n";
            }
        } else {
            cout << "Parent: wait() failed\n";
            return 4;
        }

        cout << "Parent: program terminating." << endl;

        delete[] data;
        return 0;
    }
}
