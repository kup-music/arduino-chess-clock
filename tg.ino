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
}


long time_format(long time_code) {
	// long mins = time_code / 1000 / 60;
	// long secs = time_code / 1000 % 60;
	long final = ((time_code / 1000 / 60) * 100) + (time_code / 1000 % 60);
	return final;

}


void loop() {
	// Total time remaining for each player
	long totaltime = 300000; 
	long player_time[2] = {totaltime, totaltime};


	while (true) {
		long tick = millis();
		delay(100);

		if (digitalRead(10) == 1) {
			long time_taken = millis() - tick;

			player_time[0] -= time_taken;


			long output = time_format(player_time[0]);

			matrix1.print(output);
			matrix2.print(time_format(player_time[1]));

			if (time_format(player_time[0]) < 60) {
				matrix1.drawColon(false);
			} else {
				matrix1.drawColon(true);
			}
			if (time_format(player_time[1]) < 60) {
				matrix2.drawColon(false);
			} else {
				matrix2.drawColon(true);
			}


			matrix1.writeDisplay();
			matrix2.writeDisplay();
		} else {
			long time_taken = millis() - tick;

			player_time[1] -= time_taken;


			long output = time_format(player_time[1]);

			matrix2.print(output);
			matrix1.print(time_format(player_time[0]));

			if (time_format(player_time[0]) < 60) {
				matrix1.drawColon(false);
			} else {
				matrix1.drawColon(true);
			}
			if (time_format(player_time[1]) < 60) {
				matrix2.drawColon(false);
			} else {
				matrix2.drawColon(true);
			}


			matrix1.writeDisplay();
			matrix2.writeDisplay();
		}
  }
  // put your main code here, to run repeatedly:


  lcd.setCursor(0, 1);
  
  // lcd.print(starttime);
  //lcd.print(millis() / 1000);
}
