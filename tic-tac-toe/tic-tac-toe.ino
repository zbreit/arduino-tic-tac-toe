/**
   Binary Tic Tac Toe Program for ENEE459V Mini Project 1
   By Bashai Lyngdoh and Zachary Breit

   For this program we used an 8x8 LED matrix and 4 buttons
   to create a tic-tac-toe game.
*/

/**
   Settings
   ===========================================
*/
// Pin outs
const int UP_PIN = 2;
const int DOWN_PIN = 2;
const int LEFT_PIN = 2;
const int RIGHT_PIN = 2;
const int SELECT_PIN = 4;

const int ROW_PINS[3] = {1, 2, 3};
const int COL_PINS[3] = {1, 2, 3};

/**
   State Variables
   ===========================================
*/
char board[3][3];

/**
   Settings
   ===========================================
*/
const int SHORT_BLINK_TIME = 400; // ms
const int LONG_BLINK_TIME = 800; // ms

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

  clear_board();
}

void loop() {
  // Game loop
  char player = 'X';
  for (int num_moves = 0; calculate_winner() == ' ' && num_moves < 9; num_moves++) {
    make_move(player);
    display_board();

    player = (player == 'X') ? 'O' : 'X';
  }
}

/**
   Allows the current player to choose a square.
   Adjusts the number of moves and writes their move to the board.
*/
void make_move(char player) {
  // Find next empty square
  int x, y;
  for (x = 0; x < 3; x++) {
    for (y = 0; y < 3; y++) {
      if (board[x][y] != 'X' && board[x][y] != 'O') {
        goto found_empty_square;
      }
    }
  }

found_empty_square:
  // Wait for user input
  bool awaiting_move = true;
  while (awaiting_move) {
    board[x][y] = '_';
  }
}

/**
 *  Displays the current state of the tic-tac-toe board
 */
void display_board() {
  // Display on LED matrix using Display Multiplexing (AKA row scanning)
  for (int row = 0; row < 3; row++) {
    // Turn on the common anode for that row
    digitalWrite(ROW_PINS[row], HIGH);

    // Pick which columns are ON based on the current board position
    for (int col = 0; col < 3; col++) {
      switch(board[row][col]) {
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
    // Keep the LEDs in this row on for 1ms
    delay(1);

    // Move on to the next row
    digitalWrite(ROW_PINS[row], LOW);
  }

  // Print to Serial
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
 * Determines whether an LED's that's blinking with a particular period
 * is HIGH or LOW
 */
int get_blink_state(int duty_cycle_period) {
  return (millis() % duty_cycle_period) > (duty_cycle_period / 2) 
    ? LOW 
    : HIGH;
}

/**
   Empties the board
*/
void clear_board() {
  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 3; col++) {
      board[row][col] = ' ';
    }
  }
}

/**
   Return 'X' if the X player is winning and 'O' if the O player is winning.
   If no one is winning, return ' '.
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
