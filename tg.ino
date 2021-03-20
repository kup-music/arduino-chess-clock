#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc. 
#include "Adafruit_LEDBackpack.h"
#include <LiquidCrystal.h>

Adafruit_7segment matrix_1 = Adafruit_7segment();
Adafruit_7segment matrix_2 = Adafruit_7segment();

const int rs = 46, en = 44, d4 = 40, d5 = 38, d6 = 36, d7 = 34;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Define the pins to use for our main switch, buttons, and buzzer
static const int m_switch = 47;
static const int button_1 = 31, button_2 = 33, button_3 = 35, button_4 = 37;
static const int buzzer = 53;

void setup() {
	// put your setup code here, to run once:
	Serial.begin(9600);
	// Serial.println("Double 7 Segment Backpack Test");
	matrix_1.begin(0x70);
	matrix_2.begin(0x71);

	pinMode(buzzer, OUTPUT);
	pinMode(43, OUTPUT);
	pinMode(45, OUTPUT);
	pinMode(47, INPUT);
	digitalWrite(43, HIGH);
	digitalWrite(45, LOW);

	pinMode(button_1, INPUT);
	pinMode(button_2, INPUT);
	pinMode(button_3, INPUT);

	// TODO - Currently unused
	pinMode(button_4, INPUT);

	lcd.begin(16, 2);
	// lcd.print("Hello, World!");
}

// Take a millisecond time code and format it to a displayable number
void print_time(long time_code, Adafruit_7segment *matrix) {
	long output = time_code / 1000;

	float precise = 0;

	// If player has less than 60 seconds switch to a precision display
	if (output < 60) {
		// Writes SS.MS
		output = (output * 100) + (time_code % 1000 / 10);
		precise = ((float)output / 100);
		Serial.println(precise);
		matrix->print(precise);
		matrix->drawColon(false);
	} else {
		// Writes MM:SS
		output = (output / 60 * 100) + (time_code / 1000 % 60);
		matrix->print(output);
		matrix->drawColon(true);
	}

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
	struct game_mode modes[12] = {{"1 Minute Bullet", 60, 0}, {"3 Minute Blitz", 180, 0}, {"3 Minute Blitz", 180, 2},
								{"5 Minute Blitz", 300, 0}, {"5 Minute Blitz", 300, 5}, {"10 Minute Rapid", 600, 0},
								{"15 Minute Rapid", 900, 0}, {"15 Minute Rapid", 900, 10}, {"30 Minute", 1800, 0},
								{"45 Minute", 2700, 45}, {"1 Hour", 3600, 0}, {"Max", 5999, 0}};

	// TODO - The buttons should just be 0, we should try testing with this
	// 0 = Move switch, 1 = button_1, 2 = button_2, 3 = button_3
	int button_state[5] = {digitalRead(m_switch), digitalRead(button_1), digitalRead(button_2), digitalRead(button_3), digitalRead(button_4)};

	// Tracks which mode is currently selected, and stores our starting time
	int mode = 0;
	long time_start = 0;

	// Choose game mode Loop
	while (true) {
		// Increments chosen game mode when button 1 is pressed
		// Only increments on button down, not both down and up
		if (digitalRead(button_1) != button_state[1]) {
			if (digitalRead(button_1) == 1) {
				mode++;
				if (mode > 11) mode = 0;
				//Serial.println(modes[mode].name);
				lcd.clear();
        		lcd.setCursor(0, 0);
	    	    lcd.print(modes[mode].name);
	    	    lcd.setCursor(0, 1);
		        lcd.print("Increment: " + String(modes[mode].increment));
			}
			delay(50);
			button_state[1] = digitalRead(button_1);
		}

		// Increments chosen game mode when button 2 is pressed
		// Only decrements on button down, not both down and up
		if (digitalRead(button_2) != button_state[2]) {
			if (digitalRead(button_2) == 1) {
				mode--;
				if (mode < 0) mode = 11;
				//Serial.println(modes[mode].name);
				lcd.clear();
        		lcd.setCursor(0, 0);
	    	    lcd.print(modes[mode].name);
	    	    lcd.setCursor(0, 1);
		        lcd.print("Increment: " + String(modes[mode].increment));
			}
			delay(50);
			button_state[2] = digitalRead(button_2);
		}

		// Display static time on the clocks
        print_time(modes[mode].time * 1000L, &matrix_1);
        print_time(modes[mode].time * 1000L, &matrix_2);

        // Display Game Information on LCD


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

	// Main gameplay loop
	while (true) {
		long current_elapsed = millis() - time_start;

		// In the event the switch is hit
		if (button_state[0] != digitalRead(m_switch)) {
			// Adjust player time and apply the increment
			player_time[!button_state[0]] -= current_elapsed - (modes[mode].increment * 1000);

			// Set a new time start and reset the button_state[0]
			time_start = millis();
			button_state[0] = digitalRead(47);
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

		// Pause/Play
		if (button_state[4] != digitalRead(button_4)) {
			if (digitalRead(button_4) == 1) {
				while (true) {

				}
			} else {}
		}

		// If a win condition is met
		if (output <= 0) {
			// TODO - This is stupid but I don't know what the arduino will actually take so can't test
			// Ideally we just pass button_state[0] in as a variable in the string instead of two print lines
			if (!button_state[0]) {
				lcd.clear();
				lcd.setCursor(0, 0);
				lcd.print("Game Over!");
				lcd.setCursor(0, 1);
				lcd.print("Player 2 Wins!");
				//Serial.println("Game over! Player 1 has run out of time, player 2 wins!");
			} else {
				//Serial.println("Game over! Player 2 has run out of time, player 1 wins!");
				lcd.clear();
				lcd.setCursor(0, 0);
				lcd.print("Game Over!");
				lcd.setCursor(0, 1);
				lcd.print("Player 1 Wins!");
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
