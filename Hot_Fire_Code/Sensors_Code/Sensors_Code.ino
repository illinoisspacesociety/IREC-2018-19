//include the libraries for the various sensors
#include <HX711.h>
#include <SPI.h>
#include "Adafruit_MAX31855.h"
#include <SD.h>
// define the constants for the thermocouple board
#define MAXDO 3 //Thermocouple DO on digital pin 3
#define MAXCS 4 // Thermocouple CS on digital pin 4
#define MAXCLK 5 // Thermocouple CLK on digital pin 5
// define the constants for the pressure transducer input
#define PT 3 // Pressure Transducer on analog pin 3 (A3)
// define the SD card digital pin CS
#define SDCS 10 // SD Card CS is at 10
// define the digital pins the HX711 board is mounted to
#define HXDOUT 12 // HX711 DOUT is at pin 12
#define HXCLK 11 // HX711 CLK is at pin 11
// define some useful floats to store data in
float TRead; // Temperature Read in C
float Time; // Time values
float PRead; // Pressure Read in PSI
float FRead; // Force in pounds
float calibFactor = 100; // calibration factor from the other code
bool Done;
int testTime;
// start the various sensors and initialize files for SD card
HX711 scale(HXDOUT,HXCLK);
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);
File TempFile;
File PresFile;
File TimeFile;
File ForceFile;
// run setup, do an initial write to the files, tare the load cell
void setup() {
testTime = 20000;
Serial.begin(9600);

pinMode(SDCS,OUTPUT);
if (!SD.begin(SDCS)) {
    Serial.println("initialization failed!");
    return;
  }
TempFile = SD.open("TempFile.txt",FILE_WRITE);
TempFile.println("Test Start [C]");
TempFile.close();
PresFile = SD.open("PresFile.txt",FILE_WRITE);
PresFile.println("Test Start [PSIG]");
PresFile.close();
TimeFile = SD.open("TimeFile.txt",FILE_WRITE);
TimeFile.println("Test Start [ms]");
TimeFile.close();
ForceFile = SD.open("ForceFile.txt",FILE_WRITE);
ForceFile.println("Test Start [lbf]");
ForceFile.close();

scale.set_scale();
scale.tare();
}
// loop section to call the data collection a lot
void loop() {
    // Let's get the time, Temperature, and Pressure, but only for some amount of time
    if (Time < testTime) {
      acquireData();
    }
    // If connected to a computer, tells us when the program is complete
    if(Done){
      Serial.println("Done.");
      Done = false;
    }
}
// function for getting force from load cell
float FReadf() {
    float FRead = scale.get_units();
    return(FRead);
}
// function for getting pressure from transducer
float PReadf() {
    float numRead = analogRead(PT);
    float milVRead = 5*(numRead/1023);
    float PReadf = 2000*(milVRead-.498)/5.02;
    return(PReadf);
}
// function for gathering all the data and writing to the SD card
void acquireData() {
    // Get ya data, time first, then the others
    Time = millis();
    if(Time > testTime) {
      Done = true;
    }
    TRead = thermocouple.readCelsius();
    PRead = PReadf();
    FRead = FReadf();
    
    // Let's print those
    Serial.println("Here is Temperature: " + String(TRead));
    Serial.println("Here is Pressure: " + String(PRead));
    Serial.println("Here is Force: " + String(FRead));
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
    ForceFile = SD.open("ForceFile.txt",FILE_WRITE);
    ForceFile.println(String(FRead));
    ForceFile.close();
}

