/*
Name: Adewole Adeoshun
Course: CSCI 323 (Mon/Wed)
Instructor: Tsaiyun Phillips
ID: 24081306
HashTable
*/


import java.io.BufferedReader;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.*;
// =======================
// Class for listNode
// =======================
class listNode {
    String data;        // The data stored in the node
    listNode next;      // Pointer to the next node

    // Constructor: create a new node with given data
    listNode(String d) {
        data = d;
        next = null;
    }

    // Print node in the required format
    void printNode(BufferedWriter out) throws IOException {
        if (next != null)
            out.write("(" + data + ", " + next.data + ") -> ");
        else
            out.write("(" + data + ", NULL) -> ");
    }
}

// =======================
// Class for HashTable
// =======================
class HashTable {
    private listNode[] table;   // Array of linked lists (buckets)
    private int size;           // Number of buckets

    // Constructor: initialize table with dummy heads
    HashTable(int size) {
        this.size = size;
        table = new listNode[size];
        for (int i = 0; i < size; i++) {
            table[i] = new listNode("dummy");
        }
    }

    // Hash function: simple mod of string hashCode
    private int hashIndex(String data) {
        int sum = 0;
        for (int i = 0; i < data.length(); i++) {
            sum += (int) data.charAt(i);
        }
        return sum % size;
    }

    // Insert a node into the table
    void hashInsert(String data, BufferedWriter logFile) throws IOException {
        int index = hashIndex(data);
        logFile.write("*** Calling hashInsert: data= " + data + "\n");
        logFile.write("*** enter hashInsert method; index= " + index + " data= " + data + "\n");

        // find spot to insert
        listNode spot = table[index];
        while (spot.next != null && spot.next.data.compareTo(data) < 0) {
            spot = spot.next;
        }
        if (spot.next != null && spot.next.data.equals(data)) {
            logFile.write("*** Warning, data is already in the database! ***\n");
            logFile.write("*** Leaving hashInsert (...) ***\n");
            return;
        }

        listNode newNode = new listNode(data);
        newNode.next = spot.next;
        spot.next = newNode;

        logFile.write("*** Leaving hashInsert (...) ***\n");
    }

    // Delete a node from the table
    void hashDelete(String data, BufferedWriter logFile) throws IOException {
        int index = hashIndex(data);
        logFile.write("*** Calling hashDelete: data= " + data + "\n");
        logFile.write("*** Inside hashDelete method. Index= " + index + " data= " + data + "\n");

        listNode spot = table[index];
        while (spot.next != null && !spot.next.data.equals(data)) {
            spot = spot.next;
        }

        if (spot.next == null) {
            logFile.write("*** Warning, data is *not* in the hashTable! ***\n");
        } else {
            spot.next = spot.next.next; // delete the node
        }

        logFile.write("*** Leaving hashDelete () ***\n");
    }

    // Retrieval: check if data is in the table
    void hashRetrieval(String data, BufferedWriter logFile, BufferedWriter outFile2) throws IOException {
        int index = hashIndex(data);
        logFile.write("*** Calling hashRetrieval: data= " + data + "\n");
        logFile.write("*** Inside hashRetrieval. Index= " + index + " data= " + data + "\n");

        listNode spot = table[index];
        while (spot.next != null && !spot.next.data.equals(data)) {
            spot = spot.next;
        }

        if (spot.next == null) {
            outFile2.write("*** Warning, the record is *not* in the database! ***\n");
        } else {
            outFile2.write("Yes, the record is in the database!\n");
        }
    }

    // Print the entire hash table
    void printHashTable(BufferedWriter out) throws IOException {
        for (int i = 0; i < size; i++) {
            out.write("HashTable[" + i + "] -> ");
            listNode spot = table[i];
            while (spot != null) {
                spot.printNode(out);
                spot = spot.next;
            }
            out.write("NULL\n");
        }
    }
}

// =======================
// Main class
// =======================
public class AdeoshunA_Project2_Main {
    public static void main(String[] args) throws IOException {
        // Check if the right number of arguments are passed
        if (args.length != 5) {
            System.out.println("Usage: java AdeoshunA_Project2_Main <inFile> <bucketSize> <outFile1> <outFile2> <logFile>");
            return;
        }

        // Parse command-line arguments
        String inFile = args[0];
        int bucketSize = Integer.parseInt(args[1]);
        String outFile1Name = args[2];
        String outFile2Name = args[3];
        String logFileName = args[4];

        // Create readers and writers
        BufferedReader reader = new BufferedReader(new FileReader(inFile));
        BufferedWriter outFile1 = new BufferedWriter(new FileWriter(outFile1Name));
        BufferedWriter outFile2 = new BufferedWriter(new FileWriter(outFile2Name));
        BufferedWriter logFile = new BufferedWriter(new FileWriter(logFileName));

        // Create the hash table
        HashTable table = new HashTable(bucketSize);

        String line;
        while ((line = reader.readLine()) != null) {
            String[] parts = line.split(" ");
            String op = parts[0];
            String data = parts.length > 1 ? parts[1] : "";

            int index = data.isEmpty() ? -1 : (data.hashCode() & 0x7fffffff) % bucketSize;

            logFile.write("In main(): op= " + op + " data= " + data + " index= " + index + "\n");

            if (op.equals("+")) {
                table.hashInsert(data, logFile);
            } else if (op.equals("-")) {
                table.hashDelete(data, logFile);
            } else if (op.equals("?")) {
                table.hashRetrieval(data, logFile, outFile2); // retrieval goes to outFile2
            } else {
                logFile.write("op is an illegal operation!\n");
            }
        }

        // At the end, print final hash table to outFile1
        outFile1.write("*** In main() below is the final hash Table ***\n");
        table.printHashTable(outFile1);

        // Also print final hash table to logFile
        logFile.write("*** In main() below is the final hash Table ***\n");
        table.printHashTable(logFile);

        // Close all streams
        reader.close();
        outFile1.close();
        outFile2.close();
        logFile.close();
    }
}
