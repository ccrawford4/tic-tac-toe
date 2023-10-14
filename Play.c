// Author: Calum Crawford
// Date: 10/03/2023

// Include necessary libraries
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR_LEN 2
#define DEFAULT_SIZE 3

#define EMPTY '_'

typedef struct { // Defines the move struct
  int row;
  int column;
} move_t;

int max(int a, int b) { // Returns max from two ints
  if (a > b) {
    return a;
  }
  return b;
}

int min(int a, int b) { // Returns the min from two ints
  if (a < b) {
    return a;
  }
  return b;
}

// Iterates through the rows and columns of the board to print it out
void printBoard(char **board, int size) {
  for (int r = 0; r < size; r++) {
    for (int c = 0; c < size; c++) {
      if (c < size - 1) {
        printf(" %c |", board[r][c]);
      } else {
        printf(" %c\n", board[r][c]);
      }
    }
    int patternLength = size - 1; // Variable used to create a pattern for dynamic lengths
    if (r < patternLength) {
      for (int i = 0; i < patternLength; i++) {
        printf("---+");
      }
      printf("---\n");
    }
  }
}

char **createBoard(char *argv[], int size, bool newBoard) { 
  char **board = (char **)malloc(size * sizeof(char *)); // Allocates memory for the board
  if (board == NULL) {
    printf("Memory allocation failed\n");
    exit(-1);
  }
  for (int i = 0; i < size; i++) { // Iterates through the board to allocate memory for the columns
    board[i] = (char *)malloc(size * sizeof(char));
    if (board[i] == NULL) {
      printf("Memory allocation failed\n");
      exit(-1);
    }
  }
  int index = 1; // Start index gets changed depending on if we are using a larger than 3 sized board
  if (size > DEFAULT_SIZE) {
    index = 3;
  }
  for (int r = 0; r < size; r++) { // Iterates through the rows and columns to
                                   // fill the board from the command line
    for (int c = 0; c < size; c++) {
      if (newBoard) {
        board[r][c] = EMPTY;
      }
      else {
        if (*argv[index] != 'X' && *argv[index] != 'O' && *argv[index] != EMPTY) {
            printf("ERROR! %c Is an Invalid Character for Tic Tac Toe\n", *argv[index]);
            exit(-1);
        }
        board[r][c] = *argv[index++];
      }
    }
 }

  return board; // Returns the newley created board
}

void freeBoard(char **board, int size) { // Frees the allocated memory from the heap
  if (board == NULL) {
    printf("ERROR! No board to free.\n");
    exit(-1);
  }
  // Free memory for each row
  for (int i = 0; i < size; i++) {
    free(board[i]);
  }
  // Free memory for the array of row pointers
  free(board);
}

void displayWinner(int val) { // Prints the winner depending on the value passed
  switch (val) {
  case -1:
    printf("O Wins\n");
    break;
  case 1:
    printf("X Wins\n");
    break;
  default:
    printf("Draw\n");
    break;
  }
}

// Returns true if a win is found in the board for the given 'val'
bool isValid(char **board, int size, int row, int col) {
  char val = board[row][col];
  bool fillsRow = true;
  for (int c = 0; c < size; c++) { // Iterates through the row to find a win
    if (board[row][c] != val) {
      fillsRow = false;
      break;
    }
  }
  if (fillsRow) { // Returns true if the row wins
    return true;
  }
  // If statement to reduce overall runtime
  if (row == 0) { // Only need to search the columns left diag and right diag
                  // when iterating through first row
    bool fillsCol = true;
    for (int r = 0; r < size; r++) { // Iterates through the column
      if (board[r][col] != val) {
        fillsCol = false;
        break;
      }
    }
    if (fillsCol) { // Returns true if a win is found at the column
      return true;
    }

    bool fillsDiagRight = true;
    for (int r = 0, c = 0; r < size && c < size; r++, c++) { // Iterates through the right pointing diagonal
      if (board[r][c] != val) {
        fillsDiagRight = false;
        break;
      }
    }
    if (fillsDiagRight) { // Returns true if a win is found
      return true;
    }

    bool fillsDiagLeft = true;
    for (int r = 0, c = size - 1; r < size && c >= 0; r++, c--) { // Performs same operation for anti-diagonal
      if (board[r][c] != val) {
        fillsDiagLeft = false;
        break;
      }
    }
    if (fillsDiagLeft) { // Returns true if a win is found
      return true;
    }
  }
  return false; // Returns false if no win is found
}

int checkForTerminalState(char **board, int row, int size, bool hasEmpty) {
  if (row == size && hasEmpty) { // If iterates through the entire board and
                                 // there are still empty spaces
    return -2; // Returns -2 to indicate the game is not finished
  }
  if (row == size) { // If you iterate through the entire board and no win is found
    return 0; // return 0 indicating a draw
  }
  for (int c = 0; c < size; c++) { // Iterate through columns of the row
    if (board[row][c] == EMPTY) {
      hasEmpty = true; // Boolean to keep track if there are still empty spaces
                       // available
    }
    if (board[row][c] != EMPTY && isValid(board, size, row, c)) { // Checks to see if there is a win for this char
      switch (board[row][c]) {
      case 'X': // Returns the integers associated with the character that won
        return 1;
      case 'O':
        return -1;
      default:
        return 0;
      }
    }
  }
  return checkForTerminalState(board, row + 1, size, hasEmpty); // Otherwise search the next row recursively
}

bool isBoardEmpty(char **board, int size) { // Iterates through the board to see if it is empty
  for (int r = 0; r < size; r++) {
    for (int c = 0; c < size; c++) {
      if (board[r][c] != EMPTY) {
        return false; // Returns false if a char that is not an underscore is
                      // found
      }
    }
  }
  return true; // Returns true if all the chars are underscores
}


// Minimax algorithm to calculate scores of a move
int minimax(char **board, int size, int depth, int alpha, int beta, bool maximizingPlayer) {
  int result = checkForTerminalState(board, 0, size, false); // First checks to see if the board is full or someone won
  if (result != -2) { // If someone won or its a draw (ie the function did not
                      // indicate to keep playing)
    switch (result) {
    case 1:
      return 10 - depth; // If the maximizer won ('X') then return the maximizer
                         // score minus the depth (moves it took to get there)
    case -1:
      return -10 + depth; // If the minimizer won ('O') then return the minimzer
                          // score minus the depth (moves it took to get there)
    default:
      return 0; // Returns 0 indicating a draw
    }
  }

  if (maximizingPlayer) {   // If the current player is the maximizing one
    int maxScore = -100000; // Initalizes a small value to the maxScore
    for (int r = 0; r < size; r++) { // Iterates through the rows and columns
      for (int c = 0; c < size; c++) {
        if (board[r][c] == EMPTY) { // Only play a move if the space is empty
          board[r][c] = 'X';        // If it is maximizer then play an 'X'
          int score = minimax(board, size, depth + 1, alpha, beta, false); // Get the score recursively by letting
                                                                          // 'O' play the next move
          board[r][c] = EMPTY; // Unassign the move to keep searching for the best move
          maxScore = max(maxScore, score); // Assign the max score with the correct value
          alpha = max(alpha, score); // Alpha variable is used to keep track of
                                     // the current largest score in the tree
          if (beta <= alpha) { // If the beta is less than or equal to the head then we
                               // found the optimal choice and so we can stop searching
            break;
          }
        }
      }
    }
    return maxScore; // Return the max score of the move that was made in the
                     // findBestMove function
  } else {
    int minScore = 100000; // Does the same thing but opposite for the minimzer
    for (int r = 0; r < size; r++) {
      for (int c = 0; c < size; c++) {
        if (board[r][c] == EMPTY) {
          board[r][c] = 'O'; // Places an O in an empty spot
          int score = minimax(board, size, depth + 1, alpha, beta, true); // Plays next move
          board[r][c] = EMPTY; // Undoes the move so we can try a new move
          minScore = min(minScore, score); // Calculates the minimum score
          beta = min(beta, minScore);      // Calculates beta
          if (beta <= alpha) { // If beta <= alpha then we found the optimal
                               // move and can stop searching
            break;
          }
        }
      }
    }
    return minScore; // Return the minimum score
  }
}

move_t findBestMove(char **board, int size, bool isMaximizer) { // Returns the best move given a board and a player
  move_t bestMove;      // Initalizes the bestMove
  if (isBoardEmpty(board, size)) { // Checks to see if the board is empty
    bestMove.row = 0; // Manually assigns the best move to be the top right spot
    bestMove.column = 0;
    return bestMove;
  }
  char val = (isMaximizer) ? 'X' : 'O'; // Asigns the val being placed based on
                                        // if its the maximizer or not
  int maxScore = -10;                   // Max score starts with a small value
  int minScore = 10;                    // Min score starts with a large value

  bestMove.row = -1; // Random values for the best move at the start
  bestMove.column = -1;

  for (int r = 0; r < size; r++) { // Iterates through the board
    for (int c = 0; c < size; c++) {
      if (board[r][c] == EMPTY) { // Places the value (x or o) on the first empty spot it finds
        board[r][c] = val;
        int score = minimax(board, size, 0, maxScore, minScore, !isMaximizer); // Calculates score for that spot
                                           // with the opponent playing first
        board[r][c] = EMPTY; // Unassigns the move to evaluate the score for the
                             // other spots
        if (isMaximizer) {   // If its the maximizer then assign the maxScore to
                             // the largest score found
          if (score > maxScore) {
            maxScore = score;
            bestMove.row = r; // Best move's row and columns are updated
            bestMove.column = c;
          }
        } else { // If its the minimizer then assigns minScore to the smallest
                 // score found
          if (score < minScore) {
            minScore = score;
            bestMove.row = r; // Best move's row and columns are updated
            bestMove.column = c;
          }
        }
      }
    }
  }

  return bestMove; // Returns the best move after evaluating all the potential spots
}

void placeMove(char **board, move_t *move, bool isMaximizer) { // Places a move on the board
  char val =
      (isMaximizer) ? 'X' : 'O'; // Initalizes the val depending on whos playing
  board[move->row][move->column] = val; // Plays the move
}

void playGame(char **board, int size,bool interactive) { // Play game   
  bool isMaximizer = true; // Initalies isMaximizer
  while (true) {
    int check = checkForTerminalState(board, 0, size, false); // Checks to see if game is in terminal state
    if (check != -2) {          // If the game is in terminal state
      printBoard(board, size);  // Print the board
      displayWinner(check);     // Display the winner
      return;                   // End the function
    }
    if (!interactive) { // Ends the game if its not interactive and finds the best move for 'O'
      printBoard(board, size);
      move_t bestMove = findBestMove(board, size, false);
      printf("O: %d %d\n", bestMove.row, bestMove.column);
      return;
    }

    move_t currentMove; // Initalize current move variable

    if (isMaximizer) { // If is maximizer then use minimax algorithm to find and
                       // place the best move
      printBoard(board, size); // Print the board only when the user takes a turn
      printf("\n");
      int r, c; // Will hold the row and column values that the user pases
      bool validInput = false;
      do {
        printf("X: ");
        if (fscanf(stdin, "%d %d", &r, &c) != 2) { // Gets user import
          printf("ERROR! Please enter two integers.\n");
          while (fgetc(stdin) != '\n'); // cleans up the input to not include any newlines
        } else if (r < 0 || r >= size || c < 0 || c >= size) { // Accounts for if the user input is not in the
                                                               // bounds of the board
          printf("ERROR! Row and column must be within the bounds of the board.\n");
        } else if (board[r][c] != EMPTY) {
          printf("ERROR! That spot is already taken.\n"); // Accounts for if the
                                                          // move is not on an
                                                          // empty space
        } else {
          validInput = true; // Otherwise the input is valid and the loop breaks
        }
      } while (!validInput);

      currentMove.row = r; // initalize the current moves row and column values
      currentMove.column = c;

    } else { // Otherwise then let the user play
      currentMove = findBestMove(board, size, isMaximizer);
    }
    placeMove(board, &currentMove, isMaximizer); // Places the move
    isMaximizer = !isMaximizer; // Resets whos playing every time
  }
}

int convertCharToInt(char *character) { // Takes in a character pointer and
                                        // converts the value into an integer
  if (!character || character[0] == '\0') {
    printf("ERROR! Character is null\n");
    exit(-1); // Prints error message and exits if failed
  }
  int number = atoi(character);
  if (number == 0 && character[0] != '0') {
    printf("ERROR! atoi function failed\n");
    exit(-1); // Prints error message and exits if failed
  }
  return number; // If successful then return the integer value associated with
                 // the character
}

int main(int argc, char *argv[]) {
  int size; // Booleans responsible for how the game will be played
  bool interactive;
  bool newBoard;
    
  // Controls what values the booleans get assigned to based on the command line arguments
  if (argc == 1) {
    interactive = true;
    newBoard = true;
    size = DEFAULT_SIZE;
  } else if (argc == 10) {
    interactive = false;
    newBoard = false;
    size = DEFAULT_SIZE;
  } else if (argc > 10 && strncmp(argv[1], "-s", MAX_STR_LEN) == 0) {
    interactive = false;
    newBoard = false;
    int number = convertCharToInt(argv[2]);
    size = number;
  } else {
    printf("ERROR! Invalid Command Line Argumeents.\n");
    exit(-1);
  }
  
  char **board = createBoard(argv, size, newBoard); // Creates the board
  playGame(board, size, interactive); // Plays the game
  freeBoard(board, size); // Frees the board when done
}
