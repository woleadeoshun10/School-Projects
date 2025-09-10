#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// Class holding name and age.
class Person {
private:
    string name;
    int age;

public:
    // Constructor
    Person(string name, int age) {
        this->name = name;
        this->age = age;
    }
    void printPerson(ofstream &ofile) {
        ofile << name << " is " << age << " years old.\n";
    }//end print
}; // end class Person

int main(int argc, char** argv) {
    if (argc != 3) {
        cout << "Your command line need to include two parameters: input file and output file \n";
        exit(1);
    } // end if argc

    // Open input file from argv[1]
    ifstream inFile(argv[1]);
    if (!inFile.is_open()) {
        cout << "Unable to open the input file" << endl;
        exit(1);
    }

    // Open output file from argv[2]
    ofstream outFile(argv[2]);
    if (!outFile.is_open()) {
        cout << "Unable to open the output file" << endl;
        exit(1);
    }

    int numOfPeople;
    inFile >> numOfPeople;

    //Write header line before listing people
    outFile << "*** There are " << numOfPeople << " people ***\n";

    // Create an array of Person pointers
    Person** people = new Person*[numOfPeople];
    string Tname;
    int Tage;
    int index = 0;

    // Read each person's name and age, create Person objects, and store pointers
    while (index < numOfPeople) {
        inFile >> Tname;
        inFile >> Tage;
        Person* p = new Person(Tname, Tage);
        people[index++] = p; // save the person pointer
    } // end while()

    // Print each person to the output file
    for (int index = 0; index < numOfPeople; index++) {
        people[index]->printPerson(outFile);
    } // end for()

    // Clean up dynamically allocated memory and close files
    for (int index = 0; index < numOfPeople; index++) {
        delete people[index];
    }
    delete[] people;

    inFile.close();
    outFile.close();

    exit(0);
} // end main()



