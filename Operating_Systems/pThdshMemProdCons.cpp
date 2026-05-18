// g++ pThdshMemProdCons.cpp -opThdshMemProdCons.exe -pthread 
// Adapted from Operating Systems by Silberschatz et al.

#include <iostream>
#include <sys/types.h>     
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>
#include <string>

using namespace std;

const int BUFFER_SIZE = 10;  // Number of elements in our array Buffer for Producer and Consumer

int In = 0, Out = 0, intBuff[BUFFER_SIZE];


// Producer shared memory pthread source code using POSIX pthreads.
void *producer (void *param) {

	cout << "Producer: executing...\n";

	int next_produced = 0;   // Parent's private variable that child doesn't see.

	while (1) {
 
 		while( ( ( In + 1 ) % BUFFER_SIZE) == Out);    // Is buffer full?

		intBuff[In] = next_produced;

		string pDisplay = "Producer: next element produced at " + to_string(In) + " has value " + to_string(next_produced) + "\n";
		cout << pDisplay;

	 	In = (In + 1) % BUFFER_SIZE;

                // We break from this loop after a number of iterations, 35. 
		if (next_produced++ == 35) break; 
                } 

		cout << "Producer: after producer loop... terminating\n";
	     
	        pthread_exit(0);
} 



// Consumer shared memory pthread source code using POSIX pthreads.
void *consumer (void *param) {

	cout << "Consumer: executing...\n";

	while (1) {
		while (In == Out);    // Is buffer empty?

		int next_consumed = intBuff[Out];   // child's private variable 

		string cDisplay;
		cDisplay = "Consumer: next element consumed at " + to_string(Out) + " has value " + to_string(next_consumed) + "\n";  
		cout << cDisplay;

		Out = ( Out + 1) % BUFFER_SIZE;
	
                // We break from this loop after a number of iterations, 35.  
		if (next_consumed == 35 ) break;
		}

		cout << "Consumer: after consumer loop ... terminating\n";

   		pthread_exit(0);
} 

int main(int argc, const char *argv[]) { 

// Create the pthreads
  pthread_t       p_tid, c_tid;
  pthread_attr_t  p_attr, c_attr;

  int ret = 0;

// Initialize pthread attributes for producer and consumer.

   if ((ret = pthread_attr_init(&p_attr)) != 0) {
	cerr << "pthread_attr_init(&(p_attr) failed\n";
	return ret;
   }

   if ((ret = pthread_attr_init(&c_attr)) != 0) {
	cerr << "pthread_attr_init(&(c_attr) failed\n";
	return ret;
   }

// Create pthreads for producer and consumer.

   if ((ret = pthread_create(&p_tid, &p_attr, producer, NULL)) != 0) {
	cerr << "pthread_create(&p_tid, &p_attr, producer, NULL) failed\n";
	return ret;
   }


   if ((ret = pthread_create(&c_tid, &c_attr, consumer, NULL)) != 0) {
	cerr << "pthread_create(&c_tid, &c_attr, consumer, NULL) failed\n" ;
	return ret;
   }


// wait for pthreads to finish 

   if ( (ret = pthread_join(p_tid, NULL)) != 0) {
	cerr << "pthread_join(p_tid, NULL) failed\n";
	return ret;
   } 
 
  cout << "main: Joined producer thread\n"; 
 
   if ( (ret = pthread_join(c_tid, NULL)) != 0) {
	cerr << "pthread_join(c_tid, NULL) failed\n";
	return ret;
   } 

   cout << "main: Joined consumer thread\n"; 

   cout << "main: Terminating.\n"; 
   return 0;
}


