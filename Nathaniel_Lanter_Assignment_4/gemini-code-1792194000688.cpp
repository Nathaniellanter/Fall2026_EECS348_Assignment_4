/**
 * @file Assignment4.cpp
 * @brief EECS 348 Assignment 4 - Recursive Object-Oriented Sudoku Solver
 * @details Reads five Sudoku puzzle files (puzzle1.txt through puzzle5.txt) 
 *          containing numbers and underscores for blanks, and solves them 
 *          using a recursive backtracking algorithm implemented via C++ classes.
 * @inputs puzzle1.txt, puzzle2.txt, puzzle3.txt, puzzle4.txt, puzzle5.txt
 * @outputs Prints the puzzle filename, initial board, and solved board(s) to the console.
 * @collaborators None
 * @sources EECS 348 Assignment 4 Instructions, Standard C++ Documentation
 * @author Nathaniel Lanter
 * @date 2026-10-16
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

// Class representing an individual Sudoku Cell
class SudokuCell {
private:
    int value; // Stored digit 1-9, or 0 if empty

public:
    // Default constructor initializes cell to empty (0)
    SudokuCell() {
        value = 0; // Authored by Nathaniel Lanter
    }

    // Parameterized constructor to set cell value
    SudokuCell(int val) {
        value = val; // Authored by Nathaniel Lanter
    }

    // Get the current value of the cell
    int getValue() const {
        return value; // Authored by Nathaniel Lanter
    }

    // Set the value of the cell
    void setValue(int val) {
        value = val; // Authored by Nathaniel Lanter
    }

    // Check if the cell is currently empty
    bool isEmpty() const {
        return value == 0; // Authored by Nathaniel Lanter
    }
};

// Class managing the 9x9 Sudoku board, validation, and recursive solving logic
class SudokuBoard {
private:
    SudokuCell board[9][9]; // 9x9 grid of SudokuCell objects

    // Helper method to check if placing a number is valid according to Sudoku rules
    bool isValid(int row, int col, int num) const {
        // Loop through each column in the current row to check for duplicates
        for (int c = 0; c < 9; ++c) {
            // Check if the number already exists in the same row (excluding current cell)
            if (board[row][c].getValue() == num) {
                return false; // Duplicate found in row
            }
        }

        // Loop through each row in the current column to check for duplicates
        for (int r = 0; r < 9; ++r) {
            // Check if the number already exists in the same column (excluding current cell)
            if (board[r][col].getValue() == num) {
                return false; // Duplicate found in column
            }
        }

        // Determine the top-left starting row index for the 3x3 sub-grid
        int startRow = row - (row % 3);
        // Determine the top-left starting column index for the 3x3 sub-grid
        int startCol = col - (col % 3);

        // Loop through the 3x3 sub-grid rows
        for (int r = 0; r < 3; ++r) {
            // Loop through the 3x3 sub-grid columns
            for (int c = 0; c < 3; ++c) {
                // Check if the number already exists within the 3x3 block
                if (board[startRow + r][startCol + c].getValue() == num) {
                    return false; // Duplicate found in 3x3 sub-grid
                }
            }
        }

        return true; // Placement is valid
    }

    // Recursive backtracking helper method to solve the puzzle
    bool solveRecursive(int row, int col) {
        // Base case: if we have moved past the last row (row 8), the puzzle is fully solved
        if (row == 9) {
            return true; // Solution found successfully
        }

        // Move to the next row if we have reached the end of the current column
        int nextRow = (col == 8) ? row + 1 : row;
        // Move to the next column or wrap around to the first column (0)
        int nextCol = (col == 8) ? 0 : col + 1;

        // If the current cell is already filled, skip to the next cell recursively
        if (!board[row][col].isEmpty()) {
            return solveRecursive(nextRow, nextCol); // Proceed without modifying current cell
        }

        // Try digits 1 through 9 sequentially in the empty cell
        for (int num = 1; num <= 9; ++num) {
            // Check if placing the current digit obeys Sudoku constraints
            if (isValid(row, col, num)) {
                // Temporarily place the number into the cell
                board[row][col].setValue(num);

                // Recursively attempt to solve the rest of the board with this choice
                if (solveRecursive(nextRow, nextCol)) {
                    return true; // Propagate success upward
                }

                // If placement leads to a dead end, backtrack by resetting the cell to empty
                board[row][col].setValue(0); // Reset cell value
            }
        }

        return false; // Trigger backtracking when no candidates work
    }

public:
    // Default constructor initializes an empty board
    SudokuBoard() {
        // Grid elements initialized via SudokuCell default constructor
    } // Authored by Nathaniel Lanter

    // Load board state from a specified text file containing numbers and underscores
    bool loadFromFile(const std::string& filename) {
        // Open the puzzle file stream
        std::ifstream file(filename);
        // Check if the file opened successfully
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filename << std::endl;
            return false; // File open failed
        }

        // Loop through each row of the 9x9 grid
        for (int r = 0; r < 9; ++r) {
            // Loop through each column of the current row
            for (int c = 0; c < 9; ++c) {
                char ch;
                // Read a character from the file
                if (!(file >> ch)) {
                    std::cerr << "Error: Format error in file " << filename << std::endl;
                    return false; // Malformed file format
                }

                // Check if the character represents an empty space (underscore)
                if (ch == '_') {
                    board[r][c] = SudokuCell(0); // Set cell as empty
                } else if (ch >= '1' && ch <= '9') {
                    board[r][c] = SudokuCell(ch - '0'); // Convert char digit to integer and store
                } else {
                    std::cerr << "Error: Invalid character '" << ch << "' in file " << filename << std::endl;
                    return false; // Invalid character found
                }
            }
        }

        file.close(); // Close the file stream
        return true;  // Successfully loaded board
    }

    // Print the current state of the Sudoku board to standard output
    void display() const {
        // Loop through each row for printing
        for (int r = 0; r < 9; ++r) {
            // Print horizontal separators for 3x3 block visual clarity
            if (r > 0 && r % 3 == 0) {
                std::cout << "---------------------\n";
            }

            // Loop through each column in the row
            for (int c = 0; c < 9; ++c) {
                // Print vertical separators for 3x3 block visual clarity
                if (c > 0 && c % 3 == 0) {
                    std::cout << "| ";
                }

                // Retrieve the cell value
                int val = board[r][c].getValue();
                // Print underscore if empty, or the digit if filled
                if (val == 0) {
                    std::cout << "_ ";
                } else {
                    std::cout << val << " ";
                }
            }
            std::cout << std::endl; // End line for the current row
        }
    } // Authored by Nathaniel Lanter

    // Public wrapper method to execute the recursive backtracking solver
    bool solve() {
        return solveRecursive(0, 0); // Initiate recursion from top-left cell (0,0)
    } // Authored by Nathaniel Lanter
};

// Main driver function to process all required puzzle files
int main() {
    // Array containing the names of the five required puzzle files
    std::string puzzleFiles[5] = {
        "puzzle1.txt",
        "puzzle2.txt",
        "puzzle3.txt",
        "puzzle4.txt",
        "puzzle5.txt"
    };

    // Loop through each puzzle file name in the array
    for (int i = 0; i < 5; ++i) {
        // Instantiate a new SudokuBoard object for the current puzzle
        SudokuBoard puzzleSolver;

        // Print header indicating the current puzzle file being processed
        std::cout << "===============================\n";
        std::cout << "Puzzle File: " << puzzleFiles[i] << "\n";
        std::cout << "===============================\n";

        // Attempt to load the puzzle from the corresponding text file
        if (puzzleSolver.loadFromFile(puzzleFiles[i])) {
            // Print the initial unsolved puzzle board configuration
            std::cout << "Initial Puzzle:\n";
            puzzleSolver.display();
            std::cout << "\n-------------------------------\n";

            // Execute the recursive solver and check if a solution exists
            if (puzzleSolver.solve()) {
                // Print the successfully solved puzzle board
                std::cout << "Solution Found:\n";
                puzzleSolver.display();
            } else {
                // Print notification message if no valid solution exists
                std::cout << "No solution found.\n";
            }
        } else {
            // Print notification if loading the puzzle file failed
            std::cout << "Failed to load " << puzzleFiles[i] << std::endl;
        }

        std::cout << "\n\n"; // Separate outputs between different puzzle files
    }

    return 0; // Return success status code to operating system
} // Authored by Nathaniel Lanter