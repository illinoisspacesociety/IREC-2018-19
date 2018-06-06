#include <SPI.h>
#include "Adafruit_MAX31855.h"
#include <SD.h>

#define MAXDO 3 //Thermocouple DO on digital pin 3
#define MAXCS 4 // Thermocouple CS on digital pin 4
#define MAXCLK 5 // Thermocouple CLK on digital pin 5
#define PT 17 // Pressure Transducer on analog pin 3 (17)
#define SDCS 9 // SD Card CS is at 10

float TRead; // Temperature Read in C
float Time; // Time values
float PRead; // Pressure Read in PSI
bool Done;
int testTime;

Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);
File TempFile;
File PresFile;
File TimeFile;

void setup() {
SD.remove("TempFile.txt");
SD.remove("PresFile.txt");
if(!SD.remove("TimeFile.txt")) {
  Serial.println("What");
}

testTime = 5000;
Serial.begin(9600);
pinMode(SDCS,OUTPUT);
if (!SD.begin(SDCS)) {
    Serial.println("initialization failed!");
    return;
  }
}

void loop() {
    //delay(20); // Don't want this going too fast
    // Let's get the time, Temperature, and Pressure, but only for some amount of time
    if (Time < testTime) {
      acquireData();
    }
    if(Done){
      Serial.println("All done! Hope it went well.");
      Done = false;
    }
}

float PReadf() {
    float numRead = analogRead(PT);
    float milVRead = 5*(numRead/1023);
    float PReadf = 2000*(milVRead-.498)/5.02;
    return(PReadf);
}

void acquireData() {
    Time = millis();
    if(Time > testTime) {
      Done = true;
    }
    TRead = thermocouple.readCelsius();
    PRead = PReadf();
    
    // Let's print those
    Serial.println("Here is Temperature: " + String(TRead));
    Serial.println("Here is Pressure: " + String(PRead));
    Serial.println("Here is Time: " + String(Time));
    Serial.println();
    
    // Now let's store them on the SD card
    TempFile = SD.open("TempFile.txt",FILE_WRITE);
    TempFile.println(String(TRead));
    TempFile.close();
    PresFile = SD.open("PresFile.txt",FILE_WRITE);
    PresFile.println(String(PRead));
    PresFile.close();
    TimeFile = SD.open("TimeFile.txt",FILE_WRITE);
    TimeFile.println(String(Time));
    TimeFile.close();
}

