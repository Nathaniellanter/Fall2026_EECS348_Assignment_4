/*
 * Program Name : Sudoku solver
 * Description  :Solves Sudoku using recursion
 * 
 * Inputs       : Puzzle files
 * Output       : solved puzzles in the form of terminal output
 * Collaborators: Claude wrote the initial versiond
 * Other sources: none
 * Author       : Nathaniel Lanter
 * Creation date: 10/15/2026
 * Revision date: 10/15/2026
 * 
 * 
 */



#include <iostream>   // input and output library
#include <fstream>    // allows for file input 
#include <string>     // allows for name input of files
#include <vector>     // resizeable array where memory is automatically allocated
#include <array>      // array library

//class definitinon
class Puzzle {
public:
    static const int SIZE = 9;                 // size of the sudoku

    Puzzle() {                                  //constructor
        for (auto& row : cells) row.fill(0);  //sets every cell in the sudoku puzzle to 0
    }

    //loads file
    bool loadFromFile(const std::string& fileName) { 
        std::ifstream in(fileName);            // finds file by name
        if (!in) return false;                 // if file not found the function returns false
        int count = 0;                         //number of cells read, bounds while loop
        char ch;                               //character in sudoku grid
        while (count < SIZE * SIZE && in.get(ch)) {  // loop to convert files into something the program can work with
            if (ch >= '1' && ch <= '9') {              // check if valid digit
                cells[count / SIZE][count % SIZE] = ch - '0';  // 
                count++;                               //increments 
            } else if (ch == '_' || ch == '0' || ch == '.') {  //check if valid character
                cells[count / SIZE][count % SIZE] = 0;         // stores 0 for a blank cell
                count++;                                       // moves to next cell
            }                                          
        }
        return count == SIZE * SIZE;           //true only if 81 cells were read
    }

    int get(int r, int c) const { return cells[r][c]; }     // returns the number in row r, column c
    void set(int r, int c, int v) { cells[r][c] = v; }      // puts number v in row r, column c

    // prints the grid with lines between the three 3x3 blocks
    void print(std::ostream& out) const {
        for (int r = 0; r < SIZE; r++) {                    // 
            if (r > 0 && r % 3 == 0) out << "------+-------+------\n";  // 
            for (int c = 0; c < SIZE; c++) {                // 
                if (c > 0 && c % 3 == 0) out << "| ";       // 
                if (cells[r][c] == 0) out << "_";           // 
                else out << cells[r][c];                    // 
                out << " ";                                 // 
            }
            out << "\n";                                    // 
        }
    }

private:
    std::array<std::array<int, SIZE>, SIZE> cells;          // 
};

//solves the puzzle using recursive backtracking
class Solver {
public:
    explicit Solver(const Puzzle& start) : board(start) {   // copy the puzzle to work on
        for (int i = 0; i < 9; i++) rowUsed[i] = colUsed[i] = boxUsed[i] = 0;  // nothing used yet
    }

    // returns the 
    int solve() {
        solutions.clear();                          // 
        if (!initMasks()) return 0;                 // 
        search(countGivens());                      // 
        return (int)solutions.size();               //
    }

    const std::vector<Puzzle>& getSolutions() const { return solutions; }  // 

private:
    Puzzle board;                                   //
    int rowUsed[9], colUsed[9], boxUsed[9];         // 
    std::vector<Puzzle> solutions;                  // 
    static int boxIndex(int r, int c) { return (r / 3) * 3 + (c / 3); }  // 

    int countGivens() const {                       // 
        int n = 0;
        for (int r = 0; r < 9; r++)
            for (int c = 0; c < 9; c++)
                if (board.get(r, c) != 0) n++;
        return n;
    }

    //
    bool initMasks() {
        for (int r = 0; r < 9; r++) {
            for (int c = 0; c < 9; c++) {
                int d = board.get(r, c);            //
                if (d == 0) continue;               // 
                int bit = 1 << (d - 1);             // 
                int b = boxIndex(r, c);             // 
                if ((rowUsed[r] & bit) || (colUsed[c] & bit) || (boxUsed[b] & bit))
                    return false;                   // 
                rowUsed[r] |= bit;                  // 
                colUsed[c] |= bit;                  // 
                boxUsed[b] |= bit;                  // 
            }
        }
        return true;                                //
    }

    //
    void search(int filled) {
        if (filled == 81) {                         // 
            solutions.push_back(board);             //
            return;                                 // 
        }

        // 
        int bestR = -1, bestC = -1, bestMask = 0, bestCount = 10;
        for (int r = 0; r < 9; r++) {
            for (int c = 0; c < 9; c++) {
                if (board.get(r, c) != 0) continue;                 // 
                int mask = ~(rowUsed[r] | colUsed[c] | boxUsed[boxIndex(r, c)]) & 0x1FF;  //
                int cnt = __builtin_popcount(mask);                 //
                if (cnt < bestCount) {                              // 
                    bestCount = cnt; bestMask = mask;               //
                    bestR = r; bestC = c;                           // 
                }
            }
        }
        if (bestCount == 0) return;                 //

        int b = boxIndex(bestR, bestC);             // 
        for (int d = 1; d <= 9; d++) {              //
            int bit = 1 << (d - 1);                 // 
            if (!(bestMask & bit)) continue;        // 
            board.set(bestR, bestC, d);             // 
            rowUsed[bestR] |= bit; colUsed[bestC] |= bit; boxUsed[b] |= bit;  // 
            search(filled + 1);                     // 
            board.set(bestR, bestC, 0);             // 
            rowUsed[bestR] &= ~bit; colUsed[bestC] &= ~bit; boxUsed[b] &= ~bit;  // 
        }
    }
};

// creates puzzle and solver
class PuzzleRunner {
public:
    explicit PuzzleRunner(const std::string& file) : fileName(file) {}  // 

    void run() const {
        std::cout << "==============================\n";
        std::cout << fileName << "\n";                       //
        Puzzle puzzle;                                       // 
        if (!puzzle.loadFromFile(fileName)) {                // 
            std::cout << "Could not read a valid puzzle from this file.\n\n";
            return;                                          // 
        }
        std::cout << "Puzzle:\n";
        puzzle.print(std::cout);                             // 

        Solver solver(puzzle);                               // 
        int count = solver.solve();                          // 
        if (count == 0) {                                    // 
            std::cout << "\nNo solution found\n\n";          //
            return;
        }
        std::cout << "\nSolutions found: " << count << "\n"; // how many solutions there are
        int n = 1;                                           // solution number for labeling
        for (const Puzzle& s : solver.getSolutions()) {      // goes through every solution
            std::cout << "\nSolution " << n++ << ":\n";      //prints lable then increments n by one
            s.print(std::cout); // prints solved grid
        }
        std::cout << "\n";   // blank line after last solution
    }

private:
    std::string fileName;                                    // name of the puzzle file
};

int main() {
    for (int i = 1; i <= 5; i++) {                           // goes through puzzles 1 to 5
        PuzzleRunner runner("puzzle" + std::to_string(i) + ".txt");  // 
        runner.run();                                        // loads and solves puzzle and prints
    }
    return 0;                                                // program end
}
