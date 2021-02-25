/**
 * Binary Tic Tac Toe Program for ENEE459V Mini Project 1
 * By Bashai Lyngdoh and Zachary Breit
 *
 * For this program we used an 8x8 LED matrix and 4 buttons
 * to create a tic-tac-toe game.
 */

/**
 * Pin outs
 * =========================================================
 */
// Navigation
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
bool is_x_player;
char const board[3][3];

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

void loop() { delay(500); }

/**
 * Displays the current state of the tic-tac-toe boar
 */
void display_board() {
   for (int row = 0; row < 3; row++) {
      for (int col = 0; col < 3; col++) {
         Serial.println(board[row][col]);
      }
   }
}

void create_new_game() {
   is_x_player = true;
   for (int row = 0; row < 3; row++) {
      for (int col = 0; col < 3; col++) {
         board[row][col] = '';
      }
   }
}