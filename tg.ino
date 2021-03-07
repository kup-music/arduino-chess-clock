#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <LiquidCrystal.h>

Adafruit_7segment matrix1 = Adafruit_7segment();
Adafruit_7segment matrix2 = Adafruit_7segment();

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

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

	lcd.begin(16, 2);
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
long time_format(long time_code) {
	long output = time_code / 1000;

	// If player has less than 10 seconds switch to a precision display
	if (output > 10) {
		output = ((time_code / 1000) * 100) + (time_code % 100);
	} else {
		output = ((time_code / 1000 / 60) * 100) + (time_code / 1000 % 60);
	}

	return output;
}


void loop() {
	// Total time remaining for each player, and original starting time
	long totaltime = 300000;
	long player_time[2] = {totaltime, totaltime};


	// TODO - This won't actually display anything till the clock is hit
	// but this whole segment needs a rewrite anyway, fix it later
	// TODO - This "memory" can also later be used to have time increments when a player moves
	// Get stuck in this loop until the button is hit for the first time
	int latch = digitalRead(10);
	while (true) {
		if (digitalRead(10) != latch) break;
	}

	while (true) {
		// Measure a unit of time for each run of the loop
		long tick = millis();
		delay(100);
		tick = millis() - tick;

		// Keep an array of the two numbers we are outputting
		long output[2] = {time_format(player_time[0]), time_format(player_time[1])};

		// Decrement the players clock corresponding to the switch position
		if (digitalRead(10) == 1) {
			player_time[0] -= tick;
		} else {
			player_time[1] -= tick;
		}

		// Print the current digits to the clock
		matrix1.print(output[0]);
		matrix2.print(output[1]);

		// By default, colons are not drawn
		matrix1.drawColon(false);
		matrix2.drawColon(false);

		// If the output is not under 60 seconds, draw the colon
		if (output[0] >= 60) matrix1.drawColon(true);
		if (output[1] >= 60) matrix2.drawColon(true);

		// Write out to both matrices
		matrix1.writeDisplay();
		matrix2.writeDisplay();
	}

	// TODO - Idk why this is here yet lmao
	lcd.setCursor(0, 1);
}


// TODO - Win condition with player name display for whoever won
// Increments on player moves
// Library of presets
// Input to change presets before game [PHYSICAL]
// Starting opponents clock
// Play pause button [PHYSICAL]
// Time precision at low time
// Toggle switch [PHYSICAL]
