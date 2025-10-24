/*
Name: Adewole Adeoshun
Course: CSCI 323 (MonWed 3:10pm - 4:25pm)
Instructor: Tsaiyun Phillips
ID: 24081306
Project 5 - Huffman coding (part-2)  // continuation of part-1

How to run:
  javac AdeoshunA_Project5_Main.java
  java AdeoshunA_Project5_Main HuffmanCodingData.txt outFile.txt outFile2.txt logFile.txt logFile2.txt
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
    HTreeNode next;   // pointer to next node (for LList)
    HTreeNode left;   // left child
    HTreeNode right;  // right child

    HTreeNode(String c, int f, String cd, HTreeNode n, HTreeNode l, HTreeNode r) {
        chStr = c;
        frequency = f;
        code = cd;
        next = n;
        left = l;
        right = r;
    }

    // (T.chStr, T.frequency, T.code, T.left’s chStr, T.right’s chStr, T.next’s chStr)
    void printNode(PrintWriter ofile) {
        String l = (left  != null) ? left.chStr  : "null";
        String r = (right != null) ? right.chStr : "null";
        String n = (next  != null) ? next.chStr  : "null";
        ofile.printf("(%s, %d, %s, %s, %s, %s)%n", chStr, frequency, code, l, r, n);
    }
}

// ******************************************
// HuffmanTool (data + algorithms for both parts)
// ******************************************
class HuffmanTool {
    HTreeNode Root = null;
    HTreeNode listHead = new HTreeNode("dummy", 0, "", null, null, null);
    int[]    charCountAry = new int[256];
    String[] codeTable    = new String[256];

    HuffmanTool() {
        Arrays.fill(charCountAry, 0);
        Arrays.fill(codeTable, "");
    }

    // ========== Part-1 methods ==========

    void computeCharCounts(BufferedReader inFile, PrintWriter logFile) throws IOException {
        logFile.println("**** Entering computeCharCounts ****");
        int ch;
        while ((ch = inFile.read()) != -1) {
            if (0 <= ch && ch < 256) charCountAry[ch]++;
        }
        logFile.println("**** Leaving computeCharCounts ****");
    }

    // Print only count>0; skip 10,13,23,32 per professor
    void printCountAry(PrintWriter outFile) {
        outFile.println("**** Below is the character counts Table ****");
        outFile.println("Ascii\tcharacter\tcount");
        for (int i = 0; i < 256; i++) {
            if (charCountAry[i] > 0 && i != 10 && i != 13 && i != 23 && i != 32) {
                outFile.printf("%d\t%c\t%d%n", i, (char)i, charCountAry[i]);
            }
        }
    }

    HTreeNode findSpot(HTreeNode listHead, HTreeNode newNode, PrintWriter logFile) {
        logFile.println("**** entering findSpot method ****");
        HTreeNode Spot = listHead;
        while (Spot.next != null && Spot.next.frequency < newNode.frequency) {
            Spot = Spot.next;
        }
        if (Spot.next != null) {
            logFile.printf("in findSpot: Spot.next’s frequency is %d and newNode.frequency is %d%n",
                    Spot.next.frequency, newNode.frequency);
        }
        logFile.println("**** leaving findSpot method ****");
        return Spot;
    }

    void listInsert(HTreeNode Spot, HTreeNode newNode) {
        newNode.next = Spot.next;
        Spot.next = newNode;
    }

    void printList(HTreeNode listHead, PrintWriter oFile) {
        HTreeNode cur = listHead;
        while (cur != null) {
            cur.printNode(oFile);
            cur = cur.next;
        }
    }

    HTreeNode constructHuffmanLList(HTreeNode listHead, int[] charCountAry, PrintWriter logFile) {
        logFile.println("**** Entering constructHuffmanLList method ****");
        for (int index = 0; index < 256; index++) {
            if (charCountAry[index] > 0) {
                char chr = (char) index;
                int frequency = charCountAry[index];
                HTreeNode newNode = new HTreeNode(String.valueOf(chr), frequency, "", null, null, null);
                logFile.println("**** In construct LList, printing newNode ****");
                newNode.printNode(logFile);

                HTreeNode Spot = findSpot(listHead, newNode, logFile);
                listInsert(Spot, newNode);

                logFile.println("**** In construct LList, printing list after inserting newNode ****");
                printList(listHead, logFile);
            }
        }
        logFile.println("**** Leaving constructHuffmanLList method ****");
        return listHead;
    }

    HTreeNode constructHuffmanBinTree(HTreeNode listHead, PrintWriter outFile, PrintWriter logFile) {
        logFile.println("**** Entering constructHuffmanBinTree() method ****");
        while (listHead.next != null && listHead.next.next != null) {
            HTreeNode first  = listHead.next;
            HTreeNode second = first.next;

            HTreeNode newNode = new HTreeNode(
                    first.chStr + second.chStr,
                    first.frequency + second.frequency,
                    "", null, first, second);

            logFile.println("**** In constructHuffmanBinTree, printing newNode ****");
            newNode.printNode(logFile);

            HTreeNode Spot = findSpot(listHead, newNode, logFile);
            listInsert(Spot, newNode);

            // remove first two nodes
            listHead.next = second.next;

            logFile.println("**** In constructHuffmanBinTree method, printing the list after inserting newNode ****");
            printList(listHead, logFile);
        }
        Root = listHead.next;
        return Root;
    }

    boolean isLeaf(HTreeNode node) {
        return node != null && node.left == null && node.right == null;
    }

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

    void constructCharCode(HTreeNode T, String code, String[] codeTable) {
        if (T == null) return;
        if (isLeaf(T)) {
            T.code = code;
            int index = (int) T.chStr.charAt(0);
            codeTable[index] = code;
        } else {
            constructCharCode(T.left,  code + "0", codeTable);
            constructCharCode(T.right, code + "1", codeTable);
        }
    }

    // Print only characters that actually have a code (non-blank)
    void printCodeTable(String[] codeTable, PrintWriter outFile) {
        outFile.println("**** Below is the character code Table ****");
        outFile.println("Ascii\tcharacter\tcode");
        for (int i = 0; i < 256; i++) {
            if (!codeTable[i].equals("")) {
                outFile.printf("%d\t%c\t%s%n", i, (char)i, codeTable[i]);
            }
        }
    }

    // ========== Part-2 methods ==========

    // X. userInterface (Root, codeTable, logFile2)
    void userInterface(HTreeNode Root, String[] codeTable, PrintWriter logFile2) {
        Scanner sc = new Scanner(System.in);
        while (true) {
            System.out.print("Do you want to compress a text file? (Y for yes, N for no): ");
            String ans = sc.nextLine().trim();
            if (ans.length() == 0) continue;
            char yesNo = Character.toUpperCase(ans.charAt(0));
            if (yesNo == 'N') {
                System.out.println("Program exits.");
                return;
            }

            // name without .txt
            System.out.print("Enter file name to encode (without .txt), e.g., test1 : ");
            String nameOrg = sc.nextLine().trim();

            String orgPath        = nameOrg + ".txt";
            String nameCompress   = nameOrg + "_Compressed.txt";
            String nameDeCompress = nameOrg + "_deCompressed.txt";

            try (BufferedReader orgFile  = new BufferedReader(new FileReader(orgPath));
                 PrintWriter    compFile = new PrintWriter(new FileWriter(nameCompress));
                 PrintWriter    deComp   = new PrintWriter(new FileWriter(nameDeCompress))) {

                Encode(orgFile, compFile, codeTable, logFile2);
                compFile.flush();

                // reopen compressed as input for Decode
                try (BufferedReader compIn = new BufferedReader(new FileReader(nameCompress))) {
                    Decode(compIn, deComp, Root, logFile2);
                }

                System.out.println("Created: " + nameCompress + "  and  " + nameDeCompress);
            } catch (IOException ioe) {
                System.out.println("I/O Error: " + ioe.getMessage());
            }
        }
    }

    // XI. Encode (FileIn, FileOut, codeTable, logFile2)
    void Encode(BufferedReader FileIn, PrintWriter FileOut, String[] codeTable, PrintWriter logFile2) throws IOException {
        logFile2.println("**** Entering Encode method ****");
        int ch;
        while ((ch = FileIn.read()) != -1) {
            int index = (int) ch;                 // cast to integer
            String code = codeTable[index];       // lookup code
            // Even if code is empty (shouldn't happen if counted), we still write nothing.
            FileOut.print(code);                  // write code to compressed file
            logFile2.printf("** inside Encode() charIn=%d code=%s%n", index, code);
        }
        logFile2.println("**** Leaving Encode method ****");
    }

    // XII. Decode (FileIn, FileOut, Root, logFile2)
    void Decode(BufferedReader FileIn, PrintWriter FileOut, HTreeNode Root, PrintWriter logFile2) throws IOException {
        logFile2.println("**** Entering Decode method ****");
        HTreeNode Spot = Root;
        int ch;
        while ((ch = FileIn.read()) != -1) {
            char oneBit = (char) ch;  // should be '0' or '1'
            if (isLeaf(Spot)) {
                // write the leaf symbol, reset to root for next symbol
                FileOut.write(Spot.chStr);
                logFile2.println("Inside Decode method: Spot.chStr=" + Spot.chStr);
                Spot = Root;
            }

            if (oneBit == '0') {
                Spot = Spot.left;
            } else if (oneBit == '1') {
                Spot = Spot.right;
            } else {
                logFile2.println("Error! The compressed file contains invalid character!");
                return;
            }

            // If we just moved onto a leaf, immediately output and reset
            if (isLeaf(Spot)) {
                FileOut.write(Spot.chStr);
                logFile2.println("Inside Decode method: Spot.chStr=" + Spot.chStr);
                Spot = Root;
            }
        }

        // End of file. If Spot is not root and not a leaf, it ended mid-symbol.
        if (Spot != Root && !isLeaf(Spot)) {
            logFile2.println("Error: The compress file is corrupted!");
        }
        logFile2.println("**** Leaving Decode method ****");
    }
}

// ******************************************
// MAIN
// ******************************************
public class AdeoshunA_Project5_Main {
    public static void main(String[] args) {
        try {
            if (args.length != 5) {
                System.out.println("Usage:");
                System.out.println("  java AdeoshunA_Project5_Main inFile outFile outFile2 logFile logFile2");
                return;
            }

            BufferedReader inFile = new BufferedReader(new FileReader(args[0]));
            PrintWriter outFile   = new PrintWriter(new FileWriter(args[1])); // part-1
            PrintWriter outFile2  = new PrintWriter(new FileWriter(args[2])); // part-2
            PrintWriter logFile   = new PrintWriter(new FileWriter(args[3])); // part-1
            PrintWriter logFile2  = new PrintWriter(new FileWriter(args[4])); // part-2

            HuffmanTool tool = new HuffmanTool();

            // ======== Part 1 sequence ========
            tool.computeCharCounts(inFile, logFile);     inFile.close();
            tool.printCountAry(outFile);

            outFile.println("*** In main, calling constructHuffmanLList ***");
            tool.listHead = tool.constructHuffmanLList(tool.listHead, tool.charCountAry, logFile);

            outFile.println("*** In main (): printing list after list is constructed ***");
            tool.printList(tool.listHead, outFile);

            outFile.println("*** In main before calling constructHuffmanBinTree ***");
            tool.Root = tool.constructHuffmanBinTree(tool.listHead, outFile, logFile);

            outFile.println("*** In main (): printing in-Order traversal of the tree ***");
            tool.inOrderTraversal(tool.Root, outFile);

            tool.constructCharCode(tool.Root, "", tool.codeTable);

            outFile.println("*** In main (): printing character code Table ****");
            tool.printCodeTable(tool.codeTable, outFile);

            // ======== Part 2 sequence ========
            outFile2.println("*** In main (part-2): printing character code Table ****");
            tool.printCodeTable(tool.codeTable, outFile2);

            tool.userInterface(tool.Root, tool.codeTable, logFile2);

            outFile.close();
            outFile2.close();
            logFile.close();
            logFile2.close();

            System.out.println("Project Completed.");
        } catch (IOException ioe) {
            System.out.println("I/O Error: " + ioe.getMessage());
        }
    }
}