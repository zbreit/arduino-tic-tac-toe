/**
 * Binary Tic Tac Toe Program for ENEE459V Mini Project 1
 * By Bashai Lyngdoh and Zachary Breit
 * 
 * For this program we used an 8x8 LED matrix and 4 buttons
 * to create a tic-tac-toe game.
 * ============================================ */

/* Data Structures
  ============================================== */
typedef enum { LEFT = 0, RIGHT = 1, UP = 2, DOWN = 3} Direction;

/* Pins
  ============================================== */
// Should be assigned so enum values work (i.e., 0th value is the LEFT pin, 1st value is the RIGHT pin, ...)
const int BTN_PINS[4] = {6, 5, 7, 4};
const int ROW_PINS[3] = {10, 9, 8};
const int COL_PINS[3] = {13, 12, 11};
const int SELECT_PIN = A0;

/* State Variables
  ============================================== */
// Entries can either be 'X', 'O', ' ' (empty), or '_' (cursor)
char board[3][3];

// Row and Col position in the board where the user is hovering
int col_cursor, row_cursor;

// Either 'X' or 'O'
char player = 'X';

// Current winner (used in the end game to avoid recalculating the winner
char winner = '\0';

// Number of moves played
int num_moves = 0;

// Whether the game is completed
bool game_over = false;

// Store the  button states (for debouncing presses)
int button_states[4];
int last_active_time[4] = {0, 0, 0, 0};

// State of the selector potentiometer
bool just_made_selection = true;
bool just_untrapped = true;

bool didnt_print_endgame_screen = true;

/* Settings
  ============================================== */
const int SHORT_BLINK_TIME_MS = 300;
const int LONG_BLINK_TIME_MS = 1200;
const int BUTTON_COOLDOWN_MS = 800;
const int SELECT_THRESHOLD = 200; // Any value from 0 to 1023

// 
const int UNTRAP_THRESHOLD = 900; // Any value from 0 to 1023

/* Initialization
  ============================================== */
void setup() {
  // Initialize pins and turn off the LEDs to start off
  for (int i = 0; i < 4; i++) {
    pinMode(BTN_PINS[i], INPUT);
  }

  for (int i = 0; i < 3; i++) {
    pinMode(ROW_PINS[i], OUTPUT);
    pinMode(COL_PINS[i], OUTPUT);
    
    digitalWrite(ROW_PINS[i], LOW);
  }

  Serial.begin(9600);

  clear_board();
  place_cursor();
}

/* Game loop
  ============================================== */
void loop() {
  if(!game_over) {
    check_for_move();
    game_over = is_game_over();
  } else {
    // If winner is unset
    if (winner == '\0') {
      winner = calculate_winner();
    }
    
    // If X's and O's tie, exit the program
    if (winner != 'X' && winner != 'O') {
      Serial.println("Tie game! Hit the 'reset' button to play again.");
      Serial.flush();
      exit(0);
    }

    // If someone wins, go to the endgame screen
    if (didnt_print_endgame_screen) {
      Serial.print(calculate_winner());
      Serial.println(" won the game!");
      didnt_print_endgame_screen = false;
    }
  
    // Check if it's time to blink the matrix
    if (get_blink_state(LONG_BLINK_TIME_MS) == LOW) {
      clear_board();
    } else {
      if (winner == 'X') {
        draw_x();
      } else {
        draw_o();
      }
    }
  }

  // On every loop, update the matrix and button states
  update_buttons();
  update_matrix();
}

/* Helper functions
  ============================================== */

/* Waits for the current player to choose a square.
   Adjusts the number of moves and writes their move to the board. */
void check_for_move() {
  // Check if the player is selecting their current cursor location
  bool old_selection_state = just_made_selection;
  just_made_selection = analogRead(SELECT_PIN) < SELECT_THRESHOLD;
  if (just_made_selection && !old_selection_state) {
    ++num_moves;
    board[row_cursor][col_cursor] = player;
    
    place_cursor();
    print_move();
    print_board();
    swap_player();
    return;
  }

  // Check if the user is sending in a fresh untrap request
  bool old_trap_state = just_untrapped;
  just_untrapped = analogRead(SELECT_PIN) > UNTRAP_THRESHOLD;
  if (just_untrapped && ! old_trap_state) {
    Serial.println("Just untrapped cursor:");
    untrap_cursor();
    print_board();
    return;
  }
  
  // In all other cases, check if one of the buttons just got activated during this loop
  // (the button_states variable is smart about only turning on after a button is freshly activated)
  if(button_states[LEFT]) {
    Serial.println("Moved cursor left:");
    move_cursor(LEFT);
  } else if (button_states[RIGHT]) {
    Serial.println("Moved cursor right:");
    move_cursor(RIGHT);
  } else if (button_states[UP]) {
    Serial.println("Moved cursor up:");
    move_cursor(UP);
  } else if (button_states[DOWN]) {
    Serial.println("Moved cursor down:");
    move_cursor(DOWN);
  }
}

/* Moves the cursor in a specified direction, looping around the board as needed */
void move_cursor(int dir) { // TODO: replace `int` with `Direction`
  int old_col = col_cursor;
  int old_row = row_cursor;

  // Continue searching for the new cursor until you land on an empty spot
  // or return to your original spot
  do {
    switch (dir) {
      case UP:
        if (row_cursor == 0) {
          row_cursor = 2;
        } else {
          --row_cursor;
        }
        break;
      case LEFT:
        if (col_cursor == 0) {
          col_cursor = 2;
        }
        else {
          --col_cursor;
        }
        break;
      case DOWN:
        row_cursor = (row_cursor + 1) % 3;
        break;
      case RIGHT:
        col_cursor = (col_cursor + 1) % 3;
        break;
    }
  } while (board[row_cursor][col_cursor] != ' ' && !(row_cursor == old_row && col_cursor == old_col));

  board[old_row][old_col] = ' ';
  board[row_cursor][col_cursor] = '_';
  print_board();
}

/* Places the row and column cursors in the next empty square. 
  Does nothing if the board is filled up */
void place_cursor() {
  for (row_cursor = 0; row_cursor < 3; row_cursor++) {
    for (col_cursor = 0; col_cursor < 3; col_cursor++) {
      if (board[row_cursor][col_cursor] == ' ') {
        board[row_cursor][col_cursor] = '_';
        return;
      }
    }
  }
}

/* Untraps the cursor if it can't move horizontally or vertically */
void untrap_cursor() {
  if (num_moves < 8) {
    int old_row = row_cursor;
    int old_col = col_cursor;
    board[row_cursor][col_cursor] = '?'; // Fill the board location w/ some junk value (not a space)
    place_cursor();
    board[old_row][old_col] = ' ';
  }
}

/* Displays the current state of the tic-tac-toe board on the LED matrix */
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
          digitalWrite(COL_PINS[col], get_blink_state(LONG_BLINK_TIME_MS));

        // _ (the cursor) is long blinking
        case '_':
          digitalWrite(COL_PINS[col], get_blink_state(SHORT_BLINK_TIME_MS));

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


/* Updates the state of all buttons, only allowing */
void update_buttons() {
  int curr_time = millis();
  for(int btn = 0; btn < 4; btn++) {
    bool is_active = digitalRead(BTN_PINS[btn]) == HIGH && (curr_time - last_active_time[btn]) > BUTTON_COOLDOWN_MS;
    button_states[btn] = is_active;

    if (is_active) {
      last_active_time[btn] = curr_time;
    }
  }
}

/* Empties the board */
void clear_board() {
  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 3; col++) {
      board[row][col] = ' ';
    }
  }
}

/* Determines whether an LED's that's blinking with a particular period
   is HIGH or LOW */
int get_blink_state(int time_per_blink) {
  return (millis() % time_per_blink) > (time_per_blink / 2)
         ? LOW
         : HIGH;
}

/* Return 'X' if the X player is winning and 'O' if the O player is winning.
   If no one is winning, return ' '. */
char calculate_winner() {
  char target;

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

/* Whether the game is still active */
bool is_game_over() {
  char curr_winner = calculate_winner();
  return num_moves >= 9 || curr_winner == 'X' || curr_winner == 'O';
}


/* Swaps player from 'X' <--> 'O' */
void swap_player() {
  if (player == 'X') player = 'O';
  else player = 'X';
}

/* Make an X on the board
   X X
    X
   X X */
void draw_x() {
  board[0][0] = 'X';
  board[1][1] = 'X';
  board[2][2] = 'X';
  board[0][2] = 'X';
  board[2][0] = 'X';
}

/* Make an O on the board
   XXX
   X X
   XXX */
void draw_o() {
    for (int x = 0; x < 3; x++) {
      for (int y = 0; y < 3; y++) {
        board[x][y] = 'X';
      }
    }
    board[1][1] = ' ';
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

/* Prints the move that was just made */
void print_move() {
  Serial.print(player);
  Serial.println(" just made a move.");
  if (!is_game_over()) {
    Serial.print("Now it's ");
    Serial.print(player == 'X' ? 'O' : 'X');
    Serial.println("'s turn:");
  }
}

/* Debugging function for button states */
void print_buttons() {
  for(int i = 0; i < 4; i++) {
    Serial.print("Button ");
    Serial.print(i);
    Serial.print(" is ");
    Serial.print(button_states[i]? "HIGH" : "LOW");
    Serial.print(" and was updated at ");
    Serial.print(last_active_time[i]);
    Serial.println("ms");
  }
  Serial.println();
}

/* Debugging function for (x, y) coordinates */
void print_point(int x, int y) {
  Serial.print("(");
  Serial.print(x);
  Serial.print(", ");
  Serial.print(y);
  Serial.print(")");
}
