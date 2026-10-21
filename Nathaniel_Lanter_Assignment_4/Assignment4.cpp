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
        for (int r = 0; r < SIZE; r++) {                    // goes through each row
            if (r > 0 && r % 3 == 0) out << "------+-------+------\n";  // line between blcok rows
            for (int c = 0; c < SIZE; c++) {                // goes through each column
                if (c > 0 && c % 3 == 0) out << "| ";       // line between block column
                if (cells[r][c] == 0) out << "_";           // blank cell prints as underscore
                else out << cells[r][c];                    // filled cell prints its number
                out << " ";                                 // space between cells
            }
            out << "\n";                                    // new line at the end of row
        }
    }
//
private:
    std::array<std::array<int, SIZE>, SIZE> cells;          // the 9x9 grid, 0 = blank
};

//solves the puzzle using recursive backtracking
class Solver {
public:
    explicit Solver(const Puzzle& start) : board(start) {   // copy the puzzle to work on
        for (int i = 0; i < 9; i++) rowUsed[i] = colUsed[i] = boxUsed[i] = 0;  // nothing used yet
    }

    // runs solver and returns found solutions
    int solve() {
        solutions.clear();                          // clears old solutions
        if (!initMasks()) return 0;                 // if given numbers break the rules there is no solution
        search(countGivens());                      // starts recursion with the given cells counted as filled
        return (int)solutions.size();               // number of solutions found
    }

    const std::vector<Puzzle>& getSolutions() const { return solutions; }  // gives access to the solution

private:
    Puzzle board;                                   // working copy of the puzzle
    int rowUsed[9], colUsed[9], boxUsed[9];         // each bit says if a digit is already used in that row column or box
    std::vector<Puzzle> solutions;                  // every solution found
    static int boxIndex(int r, int c) { return (r / 3) * 3 + (c / 3); }  // turns row and column into box number 0-8

    int countGivens() const {                       //  counts the cells that start filled in
        int n = 0;                                  // running count 
        for (int r = 0; r < 9; r++)                 // each row
            for (int c = 0; c < 9; c++)             //each column
                if (board.get(r, c) != 0) n++;      // total filled cells
        return n;
    }

    //sets up the used-digit masks from the given numbers, false if a given number repeats
    bool initMasks() {
        for (int r = 0; r < 9; r++) {               // each row
            for (int c = 0; c < 9; c++) {           // each column
                int d = board.get(r, c);            // digit in this cell
                if (d == 0) continue;               // skips blank cells
                int bit = 1 << (d - 1);             // makes a number with only the bit for this digit set
                int b = boxIndex(r, c);             // which box this cell is in
                if ((rowUsed[r] & bit) || (colUsed[c] & bit) || (boxUsed[b] & bit))
                    return false;                   // digit already used in row, column or box so the puzzle is invalid
                rowUsed[r] |= bit;                  // marks digit used in this row
                colUsed[c] |= bit;                  // marks digit used in this column
                boxUsed[b] |= bit;                  // marks digit used in this box
            }
        }
        return true;                                // all given numbers are fine
    }

    // recursive search, fillled is how many cells are filled so far
    void search(int filled) {
        if (filled == 81) {                         // base case, every cell is filled
            solutions.push_back(board);             // saves a copy of the solved board
            return;                                 // goes back to look for other solutions
        }

        // finds the empty cell with the fewest possible digits, this keeps the search small
        int bestR = -1, bestC = -1, bestMask = 0, bestCount = 10; // best cell so far, 10 is more than any cell can have
        for (int r = 0; r < 9; r++) { //each row
            for (int c = 0; c < 9; c++) { // each column
                if (board.get(r, c) != 0) continue;                 // skip filled cells
                int mask = ~(rowUsed[r] | colUsed[c] | boxUsed[boxIndex(r, c)]) & 0x1FF;  // bits left over are digits that can still go here
                int cnt = __builtin_popcount(mask);                 // counts how many digits can go here
                if (cnt < bestCount) {                              // fewer options than the best cell so far
                    bestCount = cnt; bestMask = mask;               // remember the count and the options
                    bestR = r; bestC = c;                           // remember where the cell iss
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
