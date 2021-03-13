#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
// #include <LiquidCrystal.h>

Adafruit_7segment matrix_1 = Adafruit_7segment();
Adafruit_7segment matrix_2 = Adafruit_7segment();

// const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
// LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Define the pins to use for our main switch, buttons, and buzzer
static const int m_switch = 10;
static const int button_1 = 3, button_2 = 4, button_3 = 5, button_4 = 6;
static const int buzzer = 7;

void setup() {
	// put your setup code here, to run once:
	Serial.begin(9600);
	// Serial.println("Double 7 Segment Backpack Test");
	matrix_1.begin(0x70);
	matrix_2.begin(0x71);

	pinMode(buzzer, OUTPUT);
	pinMode(8, OUTPUT);
	pinMode(9, OUTPUT);
	pinMode(10, INPUT);
	digitalWrite(8, HIGH);
	digitalWrite(9, LOW);

	pinMode(button_1, INPUT);
	pinMode(button_2, INPUT);
	pinMode(button_3, INPUT);

	// TODO - Currently unused
	pinMode(button_4, INPUT);

	// lcd.begin(16, 2);
	// lcd.print("Hello, World!");
}

// Take a millisecond time code and format it to a displayable number
void print_time(long time_code, Adafruit_7segment *matrix) {
	long output = time_code / 1000;

	// If player has less than 60 seconds switch to a precision display
	if (output < 60) {
		output = (output * 100) + (time_code % 1000 / 10);
	} else {
		output = (output / 60 * 100) + (time_code / 1000 % 60);
	}

	// Print the current digits to the clock
	matrix->print(output);

	// Only display the colon if time is above a minute
	matrix->drawColon(output >= 60);
	// TODO - Need to add decimal for precision values

	// Write out to the matrix
	matrix->writeDisplay();
}

void loop() {
	// Define struct for different possible game modes
	struct game_mode {
		String name;
		long time;
		long increment;
	};

	// Define each of the playable game modes
	struct game_mode modes[12] = {{"Bullet", 60, 0}, {"3 Blitz", 180, 0}, {"3|2 Blitz", 180, 2},
								{"5 Blitz", 300, 0}, {"5|5 Blitz", 300, 5}, {"10 Rapid", 600, 0},
								{"15 Rapid", 900, 0}, {"15|10 Rapid", 900, 10}, {"30 Min", 1800, 0},
								{"45|45", 2700, 45}, {"1 Hour", 3600, 0}, {"Max", 5999, 0}};

	// 0 = Move switch, 1 = button_1, 2 = button_2, 3 = button_3
	int button_state[4] = {digitalRead(m_switch), digitalRead(button_1), digitalRead(button_2), digitalRead(button_3)};

	// Tracks which mode is currently selected
	int mode = 0;

	long time_start = 0;

	// Choose game mode Loop
	while (true) {
		// Increments chosen game mode when button 1 is pressed
		if (digitalRead(button_1) != button_state[1]) {
			// Only increments on button down, not both down and up
			if (digitalRead(button_1) == 1 ) {
				mode++;
				mode %= 12;
				Serial.println(modes[mode].name);
			} else {}
			delay(50);
			button_state[1] = digitalRead(button_1);
		}

		// Increments chosen game mode when button 2 is pressed
		if (digitalRead(button_2) != button_state[2]) {
			// Only decrements on button down, not both down and up
			if (digitalRead(button_2) == 1 ) {
				mode--;
				// shitty underflow fix
				if (mode < 0) {
					mode = 11;
				}
				mode %= 12;
				Serial.println(modes[mode].name);
			} else {}
			delay(50);
			button_state[2] = digitalRead(button_2);
		}

		// Breaks the loop when button 3 is pressed
		if (digitalRead(button_3) != button_state[3]) {
			time_start = millis();
			break;
		}
	}

	// Total time remaining for each player, and original starting time
	long totaltime = modes[mode].time * 1000L;
	long player_time[2] = {totaltime, totaltime};

	// TODO - Display starting times on the clocks, can have a designated function to do this

	// Get stuck in this loop until the switch is hit for the first time
	/*
	while (true) {
		if (digitalRead(m_switch) != button_state[0]) {
			time_start = millis();
			break;
		}
	}
	*/

	// Main gameplay loop
	while (true) {
		long current_elapsed = millis() - time_start;

		// In the event the switch is hit
		if (button_state[0] != digitalRead(m_switch)) {

			// TODO - Only here in case the new code breaks and we need to revert
			// Dock the previous players time
			/* if (button_state[0]) { */
			/* 	player_time[0] = player_time[0] - current_elapsed + (modes[mode % 12].increment * 1000); */
			/* } else { */
			/* 	player_time[1] = player_time[1] - current_elapsed + (modes[mode % 12].increment * 1000); */
			/* } */
			/* player_time[!button_state[0]] = player_time[!button_state[0]] - current_elapsed + (modes[mode % 12].increment * 1000); */

			// Adjust player time and apply the increment
			player_time[!button_state[0]] -= current_elapsed - (modes[mode].increment * 1000);

			// Set a new time start and reset the button_state[0]
			time_start = millis();
			button_state[0] = digitalRead(10);
		}

		// Decrement play time by the time passed since the last button press
		long output = player_time[!button_state[0]] - current_elapsed;
		if (button_state[0]) {
			print_time(output, &matrix_1);
			print_time(player_time[1], &matrix_2);
		} else {
			print_time(output, &matrix_2);
			print_time(player_time[0], &matrix_1);
		}

		// If a win condition is met
		if (output <= 0) {
			// TODO - This is stupid but I don't know what the arduino will actually take so can't test
			// Ideally we just pass button_state[0] in as a variable in the string instead of two print lines
			if (!button_state[0]) {
				Serial.println("Game over! Player 1 has run out of time, player 2 wins!");
			} else {
				Serial.println("Game over! Player 2 has run out of time, player 1 wins!");
			}

			tone(buzzer, 500);
			delay(1000);
			noTone(buzzer);
			break;
		}
	}
}

// TODO
// Play pause button
// Time precision at low time, with decimal
