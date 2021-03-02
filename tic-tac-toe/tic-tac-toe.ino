/**
   Binary Tic Tac Toe Program for ENEE459V Mini Project 1
   By Bashai Lyngdoh and Zachary Breit

   For this program we used an 8x8 LED matrix and 4 buttons
   to create a tic-tac-toe game.
*/

/* Data Structures
  ============================================== */
typedef enum { LEFT = 0, RIGHT = 1, UP = 2, DOWN = 3} Direction;

/* Settings
  ============================================== */
// Should be assigned so enum values work (i.e., 0th value is the LEFT pin, 1st value is the RIGHT pin, ...)
const int BTN_PINS[4] = {2, 3, 4, 5};

const int ROW_PINS[3] = {6, 7, 8};
const int COL_PINS[3] = {9, 10, 11};

/* State Variables
  ============================================== */
char board[3][3];

/* Settings
  ============================================== */
const int SHORT_BLINK_TIME = 400; // ms
const int LONG_BLINK_TIME = 800; // ms

/* Pin initialization
  ============================================== */
void setup() {
  // Initialize pins and turn off the LEDs to start off
  for (int i = 0; i < 4; i++) {
    pinMode(BTN_PINS[i], INPUT);
  }

  for (int i = 0; i < 3; i++) {
    pinMode(ROW_PINS[i], OUTPUT);
    pinMode(COL_PINS[i], OUTPUT);
  }

  Serial.begin(9600);

  clear_board();
}

/* Game loop
  ============================================== */
void loop() {
  char player = 'X';
  for (int num_moves = 0; calculate_winner() == ' ' && num_moves < 9; num_moves++) {
    await_move(player);
    player = (player == 'X') ? 'O' : 'X';
  }

  // Endgame screen
  char winner = calculate_winner();

  if (winner == ' ') {
    Serial.println("Tie game! Hit the 'reset' button to play again.");
    exit(0);
  }

  Serial.print(winner);
  Serial.println("Tie game! Hit the 'reset' button to play again.");
  Serial.println("won the game!");

  int prev_blink_state = LOW;

  while (true) {
    update_matrix();

    // Check if it's time to blink the matrix
    int curr_state = get_blink_state(LONG_BLINK_TIME);
    if (curr_state != prev_blink_state) {
      prev_blink_state = curr_state;
    
      if (curr_state == LOW) {
        clear_board();
      } else {
        if (winner == 'X') {
          // Make an X on the board
          // X X
          //  X
          // X X
          board[0][0] = 'X';
          board[1][1] = 'X';
          board[2][2] = 'X';
          board[0][2] = 'X';
          board[2][0] = 'X';
        } else {
          // Make a O on the board
          // XXX
          // X X
          // XXX
          for (int x = 0; x < 3; x++) {
            for (int y = 0; y < 3; y++) {
              board[x][y] = 'X';
            }
          }
          board[1][1] = ' ';
        }
      }
    }
  }
}

/* Helper functions
  ============================================== */

/* Waits for the current player to choose a square.
   Adjusts the number of moves and writes their move to the board. */
void await_move(char player) {
  int x, y;
  init_cursor(x, y);

  Serial.print(player);
  Serial.println("'s turn to move!");

  // Await user selection
  while (true) {
    // If the user is confirming their selection
    if (digitalRead(BTN_PINS[LEFT]) == HIGH &&
        digitalRead(BTN_PINS[RIGHT]) == HIGH &&
        digitalRead(BTN_PINS[DOWN]) == HIGH) {
      board[x][y] = player;
      print_board();
      return;
    } else if (digitalRead(BTN_PINS[LEFT] == HIGH)) {
      update_cursor(x, y, LEFT);
      print_board();
    } else if (digitalRead(BTN_PINS[RIGHT] == HIGH)) {
      update_cursor(x, y, RIGHT);
      print_board();
    } else if (digitalRead(BTN_PINS[UP] == HIGH)) {
      update_cursor(x, y, UP);
      print_board();
    } else if (digitalRead(BTN_PINS[DOWN] == HIGH)) {
      update_cursor(x, y, DOWN);
      print_board();
    }

    update_matrix();
    delay(10);
  }
}

/* Moves the cursor in a specified direction, looping around the board as needed */
void update_cursor(int &x, int &y, Direction dir) {
  int old_x = x;
  int old_y = y;

  do {
    switch (dir) {
      case UP:
        y = (y - 1) % 3;
        break;
      case DOWN:
        y = (y + 1) % 3;
        break;
      case LEFT:
        x = (x - 1) % 3;
        break;
      case RIGHT:
        x = (x + 1) % 3;
        break;
    }
  } while (board[x][y] != ' ' && old_x != x && old_y != y);

  board[old_x][old_y] = ' ';
  board[x][y] = '_';
}

/* Places the coordinates of the next empty square into the
   output arguments x and y */
void init_cursor(int &x, int &y) {
  for (; x < 3; x++) {
    for (; y < 3; y++) {
      if (board[x][y] == ' ') {
        board[x][y] = '_';
        return;
      }
    }
  }
}

/* Displays the current state of the tic-tac-toe board on the LED matrix*/
void update_matrix() {
  // Display on LED matrix using Display Multiplexing (AKA row scanning)
  for (int row = 0; row < 3; row++) {
    // Turn on the common anode for that row
    digitalWrite(ROW_PINS[row], HIGH);

    // Pick which columns are ON based on the current board position
    for (int col = 0; col < 3; col++) {
      switch (board[row][col]) {
        // X is always on
        case 'X':
          digitalWrite(COL_PINS[col], LOW);

        // O is long blinking
        case 'O':
          digitalWrite(COL_PINS[col], get_blink_state(SHORT_BLINK_TIME));

        // _ (the cursor) is long blinking
        case '_':
          digitalWrite(COL_PINS[col], get_blink_state(LONG_BLINK_TIME));

        // Default is off
        default:
          digitalWrite(COL_PINS[col], HIGH);
      }
    }
    // Keep the LEDs in this row on for 1 ms
    delay(1);

    // Move on to the next row
    digitalWrite(ROW_PINS[row], LOW);
  }
}

/* Prints the board to Serial */
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

/* Determines whether an LED's that's blinking with a particular period
   is HIGH or LOW */
int get_blink_state(int duty_cycle_period) {
  return (millis() % duty_cycle_period) > (duty_cycle_period / 2)
         ? LOW
         : HIGH;
}

/* Empties the board */
void clear_board() {
  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 3; col++) {
      board[row][col] = ' ';
    }
  }
}

/* Return 'X' if the X player is winning and 'O' if the O player is winning.
   If no one is winning, return ' '. */
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
