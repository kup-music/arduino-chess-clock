#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
// #include <LiquidCrystal.h>

Adafruit_7segment matrix1 = Adafruit_7segment();
Adafruit_7segment matrix2 = Adafruit_7segment();

// const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
// LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
	// put your setup code here, to run once:
	Serial.begin(9600);
	// Serial.println("Double 7 Segment Backpack Test");
	matrix1.begin(0x70);
	matrix2.begin(0x71);


	pinMode(8, OUTPUT);
	pinMode(9, OUTPUT);
	pinMode(10, INPUT);
	digitalWrite(8, HIGH);
	digitalWrite(9, LOW);

	// lcd.begin(16, 2);
	// lcd.print("Hello, World!");


	// TODO - Eventually can decide the "game name" based on input time, increment is not considered
	/* if (time <= 2) { */
	/* 	// Bullet */
	/* } else if (time <= 10) { */
	/* 	// Blitz */
	/* } else if (time <= 15) { */
	/* 	// Rapid */
	/* } else if (time <= 30) { */
	/* 	// Clasical */
	/* } else { */
	/* 	// Custom game */
	/* } */
}

// Take a millisecond time code and format it to a displayable number
void print_time(long time_code, Adafruit_7segment *matrix) {
	long output = time_code / 1000;

	// If player has less than 10 seconds switch to a precision display
	if (output < 10) {
		output = ((time_code / 1000) * 100) + ((time_code % 1000) / 10);
	} else {
		output = ((time_code / 1000 / 60) * 100) + (time_code / 1000 % 60);
	}


	// Print the current digits to the clock
	matrix->print(output);

	// Only display the colon if we are above a minute
	/* if (output >= 60) { */
	/* 	matrix->drawColon(true); */
	/* } else { */
	/* 	matrix->drawColon(false); */
	/* } */
	// TODO
	// Oneliner of above block (I think)
	matrix->drawColon(output >= 60);
	// TODO - Need to add decimal for precision values

	// Write out to the matrix
	matrix->writeDisplay();
}


void loop() {
	long time_start = 0;

	// Total time remaining for each player, and original starting time
	long totaltime = 300000;
	long player_time[2] = {totaltime, totaltime};


	// TODO - This won't actually display anything till the clock is hit
	// but this whole segment needs a rewrite anyway, fix it later
	// TODO - This "memory" can also later be used to have time increments when a player moves
	// Get stuck in this loop until the button is hit for the first time
	int latch = digitalRead(10);
	while (true) {
		if (digitalRead(10) != latch) {
			time_start = millis();
			break;
		}
	}


	while (true) {
		long current_elapsed = millis() - time_start;

		// In the event the switch is hit
		if (latch != digitalRead(10)) {

			// TODO - Increments can be added here
			// Dock the previous players time
			if (latch) {
				player_time[0] = player_time[0] - current_elapsed;
			} else {
				player_time[1] = player_time[1] - current_elapsed;
			}

			// Set a new time start and reset the latch
			time_start = millis();
			latch = digitalRead(10);
		}




		if (latch) {
			long output = player_time[0] - current_elapsed;
			print_time(output, &matrix1);
			print_time(player_time[1], &matrix2);
		} else {
			long output = player_time[1] - current_elapsed;
			print_time(output, &matrix2);
			print_time(player_time[0], &matrix1);
		}

		// TODO - Formatstrings
		// Win condition
		if (player_time[0] <= 0) {
			Serial.println("Game over! Player 1 has run out of time, player 2 wins!");
			break;
		} else if (player_time[1] <= 0) {
			Serial.println("Game over! Player 2 has run out of time, player 1 wins!");
			break;
		}
	}

	// Win condition breaks us out of the true loop, idk what we wanna do after the game

	// TODO - Idk why this is here yet lmao
	// lcd.setCursor(0, 1);
}


// TODO - Win condition with player name display for whoever won
// Increments on player moves
// Library of presets
// Input to change presets before game [PHYSICAL]
// Starting opponents clock
// Play pause button [PHYSICAL]
// Time precision at low time
// Toggle switch [PHYSICAL]
