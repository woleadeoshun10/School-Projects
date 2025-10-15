/*
Name: Adewole Adeoshun
Course: CSCI 323 (MonWed 3.10pm - 4.25pm)
Instructor: Tsaiyun Phillips
ID: 24081306
Project 4 - Huffman coding (part-1)
*/


import java.io.*;
import java.util.*;

// ******************************************
// HTreeNode CLASS
// ******************************************
class HTreeNode {
    String chStr;     // character(s)
    int frequency;    // frequency count
    String code;      // Huffman code
    HTreeNode next;   // pointer to next node
    HTreeNode left;   // left child
    HTreeNode right;  // right child

    // Constructor
    HTreeNode(String c, int f, String cd, HTreeNode n, HTreeNode l, HTreeNode r) {
        chStr = c;
        frequency = f;
        code = cd;
        next = n;
        left = l;
        right = r;
    }

    // Print a node
    void printNode(PrintWriter outFile) {
        String leftStr = (left != null) ? left.chStr : "null";
        String rightStr = (right != null) ? right.chStr : "null";
        String nextStr = (next != null) ? next.chStr : "null";
        outFile.printf("(%s, %d, %s, %s, %s, %s)%n",
                chStr, frequency, code, leftStr, rightStr, nextStr);
    }
}

// ******************************************
// HUFFMAN TOOL CLASS  (algorithm + data structure)
// ******************************************
class HuffmanTool {
    HTreeNode Root;
    HTreeNode listHead;
    int[] charCountAry = new int[256];
    String[] codeTable = new String[256];

    // Constructor
    HuffmanTool() {
        Root = null;
        listHead = new HTreeNode("dummy", 0, "", null, null, null);
        Arrays.fill(charCountAry, 0);
        Arrays.fill(codeTable, "");
    }

    // ******************************************
    // computeCharCounts
    // ******************************************
    void computeCharCounts(BufferedReader inFile, PrintWriter logFile) throws IOException {
        logFile.println("*** Entering computeCharCounts ***");
        int ch;
        while ((ch = inFile.read()) != -1) {
            if (ch >= 0 && ch < 256) {
                charCountAry[ch]++;
            }
        }
        logFile.println("*** Leaving computeCharCounts ***");
    }

    // ******************************************
    // printCountAry
    // ******************************************
    void printCountAry(PrintWriter outFile) {
        outFile.println("**** Below is the character counts Table ****");
        outFile.println("Ascii\tCharacter\tCount");
        for (int i = 0; i < 256; i++) {
            if (charCountAry[i] > 0 && i != 10 && i != 13 && i != 23 && i != 32) {
                outFile.printf("%d\t%c\t%d%n", i, (char) i, charCountAry[i]);
            }
        }
    }

    // ******************************************
    // findSpot
    // ******************************************
    HTreeNode findSpot(HTreeNode listHead, HTreeNode newNode, PrintWriter logFile) {
        logFile.println("*** Entering findSpot method ***");
        HTreeNode Spot = listHead;
        while (Spot.next != null && Spot.next.frequency < newNode.frequency) {
            Spot = Spot.next;
        }
        logFile.printf("in findSpot: Spot.next’s frequency is %s and newNode.frequency is %d%n",
                (Spot.next != null ? Spot.next.frequency : "null"), newNode.frequency);
        logFile.println("*** Leaving findSpot method ***");
        return Spot;
    }

    // ******************************************
    // listInsert
    // ******************************************
    void listInsert(HTreeNode Spot, HTreeNode newNode) {
        newNode.next = Spot.next;
        Spot.next = newNode;
    }

    // ******************************************
    // printList
    // ******************************************
    void printList(HTreeNode listHead, PrintWriter outFile) {
        HTreeNode spot = listHead;
        while (spot != null) {
            spot.printNode(outFile);
            spot = spot.next;
        }
    }

    // ******************************************
    // constructHuffmanLList
    // ******************************************
    HTreeNode constructHuffmanLList(HTreeNode listHead, int[] charCountAry, PrintWriter logFile) {
        logFile.println("*** Entering constructHuffmanLList method ***");
        for (int index = 0; index < 256; index++) {
            if (charCountAry[index] > 0) {
                char chr = (char) index;
                int frequency = charCountAry[index];
                HTreeNode newNode = new HTreeNode(String.valueOf(chr), frequency, "", null, null, null);
                logFile.println("*** In construct LList, printing newNode ***");
                newNode.printNode(logFile);

                HTreeNode Spot = findSpot(listHead, newNode, logFile);
                listInsert(Spot, newNode);

                logFile.println("*** In construct LList, printing list after inserting newNode ***");
                printList(listHead, logFile);
            }
        }
        logFile.println("*** Leaving constructHuffmanLList method ***");
        return listHead;
    }

    // ******************************************
    // constructHuffmanBinTree
    // ******************************************
    HTreeNode constructHuffmanBinTree(HTreeNode listHead, PrintWriter outFile, PrintWriter logFile) {
        logFile.println("*** Entering constructHuffmanBinTree() method ***");
        while (listHead.next != null && listHead.next.next != null) {
            HTreeNode first = listHead.next;
            HTreeNode second = first.next;

            String combinedCh = first.chStr + second.chStr;
            int combinedFreq = first.frequency + second.frequency;
            HTreeNode newNode = new HTreeNode(combinedCh, combinedFreq, "", null, first, second);
            logFile.println("*** In constructHuffmanBinTree, printing newNode ***");
            newNode.printNode(logFile);

            HTreeNode Spot = findSpot(listHead, newNode, logFile);
            listInsert(Spot, newNode);

            // remove first two nodes
            listHead.next = second.next;
            logFile.println("*** In constructHuffmanBinTree method, printing the list after inserting newNode ***");
            printList(listHead, logFile);
        }
        Root = listHead.next;
        logFile.println("*** Leaving constructHuffmanBinTree() ***");
        return Root;
    }

    // ******************************************
    // isLeaf
    // ******************************************
    boolean isLeaf(HTreeNode node) {
        return node.left == null && node.right == null;
    }

    // ******************************************
    // inOrderTraversal
    // ******************************************
    void inOrderTraversal(HTreeNode T, PrintWriter outFile) {
        if (T == null) return;
        if (isLeaf(T)) {
            T.printNode(outFile);
        } else {
            inOrderTraversal(T.left, outFile);
            T.printNode(outFile);
            inOrderTraversal(T.right, outFile);
        }
    }

    // ******************************************
    // constructCharCode
    // ******************************************
    void constructCharCode(HTreeNode T, String code, String[] codeTable) {
        if (T == null) return;
        if (isLeaf(T)) {
            T.code = code;
            int index = (int) T.chStr.charAt(0);
            codeTable[index] = code;
        } else {
            constructCharCode(T.left, code + "0", codeTable);
            constructCharCode(T.right, code + "1", codeTable);
        }
    }

    // ******************************************
    // printCodeTable
    // ******************************************
    void printCodeTable(String[] codeTable, PrintWriter outFile) {
        outFile.println("**** Below is the character code Table ****");
        outFile.println("Ascii\tCharacter\tCode");
        for (int i = 0; i < 256; i++) {
            if (!codeTable[i].equals("")) {
                outFile.printf("%d\t%c\t%s%n", i, (char) i, codeTable[i]);
            }
        }
    }
}

// ******************************************
// MAIN DRIVER CLASS
// ******************************************
public class AdeoshunA_Project4_Main {
    public static void main(String[] args) throws IOException {
        if (args.length != 3) {
            System.out.println("Usage: java AdeoshunA_Project4_Main inFile outFile logFile");
            return;
        }

        BufferedReader inFile = new BufferedReader(new FileReader(args[0]));
        PrintWriter outFile = new PrintWriter(new FileWriter(args[1]));
        PrintWriter logFile = new PrintWriter(new FileWriter(args[2]));

        HuffmanTool huffman = new HuffmanTool();

        // Step 1: compute character counts
        huffman.computeCharCounts(inFile, logFile);
        inFile.close();

        // Step 2: print character counts
        huffman.printCountAry(outFile);

        // Step 3: construct Huffman linked list
        outFile.println("*** In main, calling constructHuffmanLList ***");
        huffman.listHead = huffman.constructHuffmanLList(huffman.listHead, huffman.charCountAry, logFile);

        // Step 4: print linked list
        outFile.println("*** In main (): printing list after list is constructed ***");
        huffman.printList(huffman.listHead, outFile);

        // Step 5: build binary tree
        outFile.println("*** In main before calling constructHuffmanBinTree ***");
        huffman.Root = huffman.constructHuffmanBinTree(huffman.listHead, outFile, logFile);

        // Step 6: in-order traversal
        outFile.println("*** In main (): printing in-Order traversal of the tree ***");
        huffman.inOrderTraversal(huffman.Root, outFile);

        // Step 7: construct char code
        huffman.constructCharCode(huffman.Root, "", huffman.codeTable);

        // Step 8: print code table
        huffman.printCodeTable(huffman.codeTable, outFile);

        outFile.close();
        logFile.close();

        System.out.println("✅ Huffman Coding Part 1 completed successfully.");
    }
}
