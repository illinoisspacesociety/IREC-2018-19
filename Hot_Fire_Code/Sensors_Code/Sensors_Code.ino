//include the libraries for the various sensors
#include "HX711.h"
#include <SPI.h>
#include "Adafruit_MAX31855.h"
#include "SdFat.h"
#include <Wire.h>

// define the constants for the thermocouple board
#define MAXDO 3 //Thermocouple DO on digital pin 3
#define MAXCS 4 // Thermocouple CS on digital pin 4
#define MAXCLK 5 // Thermocouple CLK on digital pin 5

// define the constants for the pressure transducer input
#define PT 3 // Pressure Transducer on analog pin 3 (A3)

// define the SD card digital pin CS
#define SDCS 10 // SD Card CS is at 10
# define Sample_Rate = 25; // time in milliseconds for write delay

// define the digital pins the HX711 board is mounted to
#define HXDOUT 7 // HX711 DOUT is at pin 12
#define HXCLK 6 // HX711 CLK is at pin 11

// define some useful floats to store data in
float TRead; // Temperature Read in C
float Time; // Time values
float PRead; // Pressure Read in PSI
float FRead; // Force in pounds
bool Done;
int testTime = 20000;
float scale_factor = 4675;
float tare_value;

// start the various sensors and initialize files for SD card
HX711 scale(HXDOUT,HXCLK);
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

SdFat SD;
File ForFile;
File TempFile;
File PresFile;
File TimeFile;

// run setup, do an initial write to the files, tare the load cell
void setup() {

Serial.begin(115200);

pinMode(SDCS,OUTPUT);
if (!SD.begin(SDCS,SD_SCK_MHZ(50))) {
    Serial.println("initialization failed!");
  }
ForFile = SD.open("ForFile.txt",FILE_WRITE);
ForFile.println("Test Start [lbf]");
ForFile.close();
TempFile = SD.open("TempFile.txt",FILE_WRITE);
TempFile.println("Test Start [C]");
TempFile.close();
PresFile = SD.open("PresFile.txt",FILE_WRITE);
PresFile.println("Test Start [PSIG]");
PresFile.close();
TimeFile = SD.open("TimeFile.txt",FILE_WRITE);
TimeFile.println("Test Start [ms]");
TimeFile.close();

scale.tare();
scale.set_scale(scale_factor);
}

// loop section to call the data collection a lot
void loop() {
    
    // Let's get the time, Temperature, and Pressure, but only for some amount of time
    if (!Done) {
      acquireData();
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
    float PReadf = 2000*(milVRead-.51)/5.02; // numbers come from calibration
    return(PReadf);
}

// function for gathering all the data and writing to the SD card
void acquireData() {

    // Get ya data, time first, then the others
    if(millis() > testTime) {
      Done = true;
    }

    // Let's print those
//    Serial.println("Here is Temperature: " + String(thermocouple.readCelsius()));
//    Serial.println("Here is Pressure: " + String(PReadf()));
//    Serial.println("Here is Force: " + String(FReadf()));
//    Serial.println("Here is Time: " + String(millis()));
//    Serial.println();

    // Now let's store them on the SD card
    ForFile = SD.open("ForFile.txt",FILE_WRITE);
    ForFile.println(String(FReadf()));
    ForFile.close();
    TempFile = SD.open("TempFile.txt",FILE_WRITE);
    TempFile.println(String(thermocouple.readCelsius()));
    TempFile.close();
    PresFile = SD.open("PresFile.txt",FILE_WRITE);
    PresFile.println(String(PReadf()));
    PresFile.close();
    TimeFile = SD.open("TimeFile.txt",FILE_WRITE);
    TimeFile.println(String(millis()));
    TimeFile.close();
}
