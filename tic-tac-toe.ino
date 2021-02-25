/**
 * Binary Tic Tac Toe Program for ENEE459V Mini Project 1
 * By Bashai Lyngdoh and Zachary Breit
 *
 * For this program we used an 8x8 LED matrix and 4 buttons
 * to create a tic-tac-toe game.
 */

/**
 * Pin outs
 */
const int UP_PIN = 2;
const int DOWN_PIN = 2;
const int LEFT_PIN = 2;
const int RIGHT_PIN = 2;
const int SELECT_PIN = 4;

// LED Matrix
const int ROW_PINS[3] = {1, 2, 3};
const int COL_PINS[3] = {1, 2, 3};

/**
 * State Variables
 */
char player;
char board[3][3];
int num_moves;

void setup() {
   // Initialize pins and turn off the LEDs to start off
   pinMode(UP_PIN, INPUT);
   pinMode(DOWN_PIN, INPUT);
   pinMode(LEFT_PIN, INPUT);
   pinMode(RIGHT_PIN, INPUT);
   pinMode(SELECT_PIN, INPUT);

   for (int i = 0; i < 3; i++) {
      pinMode(ROW_PINS[i], OUTPUT);
      pinMode(COL_PINS[i], OUTPUT);

      digitalWrite(ROW_PINS[i], HIGH);
      digitalWrite(ROW_PINS[i], HIGH);
   }

   Serial.begin(9600);

   create_new_game();
}

void loop() { 
  while(calculate_winner == ' ' && num_moves < 9) {
    make_move(player);
    print_board();
  }
}

/**
 * Allows the current player to choose a square. 
 * Adjusts the number of moves and writes their move to the board.
 */
void make_move(char player) {
  // Find next empty square
  for(; startx < 3; startx++) {
    for(; starty < 3; starty++) {
      if
    }
  }
}

void find_empty_sqaure(int startx, int starty) {
  
}

/**
 * Displays the current state of the tic-tac-toe board
 */
void print_board() {
   for (int row = 0; row < 3; row++) {
      for (int col = 0; col < 3; col++) {
         Serial.print(board[row][col]);
         Serial.print(' ');
      }
      Serial.println();
   }
   Serial.println();
}

/**
 * Empties the board and sets player one to winning
 */
void create_new_game() {
   player = 'X';
   num_moves = 0;
   for (int row = 0; row < 3; row++) {
      for (int col = 0; col < 3; col++) {
         board[row][col] = '0' + (row * 3 + col);
      }
   }
}

/**
 * Return 'X' if the X player is winning and 'O' if the O player is winning.
 * If no one is winning, return ' '.
 */
char calculate_winner() {  
  char target = ' ';
  // Check for winning rows
  for (int row = 0; row < 3; row++) {
    target = board[row][0];
    if (target != ' ' && board[row][1] == target && board[row][2] == target) {
      return target;
    }
  }

  // Check for winning cols
  for (int col = 0; col < 3; col++) {
    target = board[0][col];
    if (target != ' ' && board[1][col] == target && board[2][col] == target) {
      return target;
    }
  }

  // Check for winning diagonals
  target = board[0][0];
  if (target != ' ' && board[1][1] == target && board[2][2] == target) {
    return target;
  }
  
  target = board[0][2];
  if (target != ' ' && board[1][1] == target && board[2][0] == target) {
    return target;
  }

  return ' ';
}