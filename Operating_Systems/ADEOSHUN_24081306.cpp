/*
Name: Adewole Adeoshun
Course: CSCI 340 (Operating Systems)
Instructor: John S
ID: 24081306
Project 2 - Multithreaded Task Partitioning with Message Queues
*/

// Filename: ADEOSHUN_24081306.cpp
// Compile: g++ ADEOSHUN_24081306.cpp -o ADEOSHUN_24081306.exe -pthread
// Usage: ./ADEOSHUN_24081306.exe inputFile.txt

#include <iostream>
#include <string>
#include <cstring>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
using namespace std;

// constants
const int BUFFER_SIZE = 10;  // number of slots in circular buffer (floating element style)
const int MAX_LINE    = 4096; // max characters per line

// LineData struct
// holds one line's text and computed stats, passed through both message queues
struct LineData {
    int  lineNum;
    char content[MAX_LINE];
    int  charCount;
    int  wordCount;
    bool done;  // sentinel flag: true means no more data
};

// MessageQueue class
// mimics FIFO message queue using circular buffer + producer consumer algo
// adapted from pThdshMemProdCons.cpp provided in class
class MessageQueue {
public:
    LineData buffer[BUFFER_SIZE]; // circular buffer array
    int In;                       // producer inserts at In
    int Out;                      // consumer removes from Out
    pthread_mutex_t mutex;        // protects In/Out and buffer
    pthread_cond_t  notFull;      // wakes producer when space opens up
    pthread_cond_t  notEmpty;     // wakes consumer when data is available

    // constructor
    MessageQueue() {
        In  = 0;
        Out = 0;
        pthread_mutex_init(&mutex,    nullptr);
        pthread_cond_init(&notFull,   nullptr);
        pthread_cond_init(&notEmpty,  nullptr);
    }

    // enqueue - producer side
    // blocks if buffer is full (floating element: (In+1)%SIZE == Out)
    void enqueue(LineData item) {
        pthread_mutex_lock(&mutex);

        while (((In + 1) % BUFFER_SIZE) == Out)  // is buffer full?
            pthread_cond_wait(&notFull, &mutex);

        buffer[In] = item;
        In = (In + 1) % BUFFER_SIZE;

        pthread_cond_signal(&notEmpty);
        pthread_mutex_unlock(&mutex);
    }

    // dequeue - consumer side
    // blocks if buffer is empty (In == Out)
    LineData dequeue() {
        pthread_mutex_lock(&mutex);

        while (In == Out)  // is buffer empty?
            pthread_cond_wait(&notEmpty, &mutex);

        LineData item = buffer[Out];
        Out = (Out + 1) % BUFFER_SIZE;

        pthread_cond_signal(&notFull);
        pthread_mutex_unlock(&mutex);

        return item;
    }
};

// global message queues
MessageQueue queue1; // computation thread -> logger thread
MessageQueue queue2; // logger thread -> email thread

// global input file path
const char *inputFilePath = nullptr;

// global totals accumulated by computation thread, printed by logger thread
int totalChars = 0;
int totalWords = 0;
int totalLines = 0;

// countWords
// counts words by detecting whitespace to non-whitespace transitions
int countWords(const char *line) {
    int  count  = 0;
    bool inWord = false;
    for (int i = 0; line[i] != '\0'; i++) {
        if (line[i] != ' ' && line[i] != '\t' && line[i] != '\r') {
            if (!inWord) {
                count++;
                inWord = true;
            }
        } else {
            inWord = false;
        }
    }
    return count;
}

// computationThread
// T0: reads input file line by line using POSIX read() syscall
// computes char count and word count per line
// enqueues each line's data into queue1 for logger thread
void *computationThread(void *param) {
    cout << "Computation Thread: started" << endl;

    // open file using POSIX open() - no fstream allowed
    int fd = open(inputFilePath, O_RDONLY);
    if (fd < 0) {
        cout << "Computation Thread: error opening file" << endl;
        pthread_exit(nullptr);
    }

    char lineBuf[MAX_LINE];
    int  lineIdx = 0;
    int  lineNum = 0;
    char ch;

    // read one character at a time to detect newlines
    while (read(fd, &ch, 1) > 0) {
        if (ch == '\n') {
            // strip \r for Windows line endings
            if (lineIdx > 0 && lineBuf[lineIdx - 1] == '\r')
                lineIdx--;

            lineBuf[lineIdx] = '\0';
            lineNum++;

            // build and fill LineData struct
            LineData ld;
            ld.lineNum   = lineNum;
            ld.charCount = lineIdx;
            ld.wordCount = countWords(lineBuf);
            ld.done      = false;
            strncpy(ld.content, lineBuf, MAX_LINE);

            // update running totals
            totalChars += ld.charCount;
            totalWords += ld.wordCount;
            totalLines++;

            string s = "Computation Thread: processed line " + to_string(lineNum) + "\n";
            cout << s;

            // produce into queue1
            queue1.enqueue(ld);

            lineIdx = 0; // reset for next line
        } else {
            if (lineIdx < MAX_LINE - 1)
                lineBuf[lineIdx++] = ch;
        }
    }

    // handle last line if file doesn't end with newline
    if (lineIdx > 0) {
        if (lineBuf[lineIdx - 1] == '\r') lineIdx--;
        lineBuf[lineIdx] = '\0';
        lineNum++;

        LineData ld;
        ld.lineNum   = lineNum;
        ld.charCount = lineIdx;
        ld.wordCount = countWords(lineBuf);
        ld.done      = false;
        strncpy(ld.content, lineBuf, MAX_LINE);

        totalChars += ld.charCount;
        totalWords += ld.wordCount;
        totalLines++;

        queue1.enqueue(ld);
    }

    close(fd);

    // send sentinel so logger thread knows we are done
    LineData sentinel;
    sentinel.done = true;
    queue1.enqueue(sentinel);

    cout << "\nComputation Thread: done, sentinel sent to queue1" << endl;
    pthread_exit(0);
}

// loggerThread
// T1: dequeues from queue1, prints per-line stats to stdout
// forwards each item to queue2 for email thread
// prints totals when sentinel arrives
void *loggerThread(void *param) {
    cout << "Logger Thread: started" << endl;

    while (1) {
        LineData ld = queue1.dequeue();

        // sentinel means computation thread is done
        if (ld.done) {
            // print totals
            cout << "Total number of characters= " << totalChars << endl;
            cout << "Total number of words= "      << totalWords << endl;
            cout << "Total number of lines= "      << totalLines << endl;

            // forward sentinel to email thread
            queue2.enqueue(ld);
            break;
        }

        // print line stats
        string s = "Line " + to_string(ld.lineNum) +
                   " characters: " + to_string(ld.charCount) +
                   " words: "      + to_string(ld.wordCount) + "\n";
        cout << s;

        // produce into queue2
        queue2.enqueue(ld);
    }

    cout << "Logger Thread: done" << endl;
    pthread_exit(0);
}

// emailThread
// T2: dequeues from queue2, composes email body from line content
// simulates sending to SMTP server
void *emailThread(void *param) {
    cout << "Email Thread: started" << endl;

    string emailBody = "";

    while (1) {
        LineData ld = queue2.dequeue();

        // sentinel means logger thread is done
        if (ld.done) break;

        // compose email body line by line
        emailBody += "Line " + to_string(ld.lineNum) + ": " +
                     string(ld.content) + "\n";
    }

    // simulate SMTP send
    cout << "Email Thread: composed email with "
         << totalLines << " lines, ready to send via SMTP" << endl;

    cout << "Email Thread: done" << endl;
    pthread_exit(0);
}

// main
int main(int argc, const char *argv[]) {

    // validate args
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " inputFile.txt\n";
        return 1;
    }

    inputFilePath = argv[1];

    cout << "Main Thread: starting, input file = " << inputFilePath << endl;

    // create 3 pthreads, one per task
    pthread_t      tid[3];
    pthread_attr_t attr[3];

    int ret = 0;

    // initialize attributes
    for (int i = 0; i < 3; i++) {
        if ((ret = pthread_attr_init(&attr[i])) != 0) {
            cerr << "pthread_attr_init failed for thread " << i << endl;
            return ret;
        }
    }

    // create computation thread
    if ((ret = pthread_create(&tid[0], &attr[0], computationThread, nullptr)) != 0) {
        cerr << "pthread_create failed for computation thread\n";
        return ret;
    }
    cout << "Main Thread: created Computation Thread" << endl;

    // create logger thread
    if ((ret = pthread_create(&tid[1], &attr[1], loggerThread, nullptr)) != 0) {
        cerr << "pthread_create failed for logger thread\n";
        return ret;
    }
    cout << "Main Thread: created Logger Thread" << endl;

    // create email thread
    if ((ret = pthread_create(&tid[2], &attr[2], emailThread, nullptr)) != 0) {
        cerr << "pthread_create failed for email thread\n";
        return ret;
    }
    cout << "Main Thread: created Email Thread" << endl;

    // join all 3 threads - main blocks until each finishes
    for (int i = 0; i < 3; i++) {
        if ((ret = pthread_join(tid[i], nullptr)) != 0) {
            cerr << "pthread_join failed for thread " << i << endl;
            return ret;
        }
        cout << "Main Thread: joined thread " << i << endl;
    }

    cout << "Main Thread: all threads done, program terminating" << endl;

    return 0;
}
