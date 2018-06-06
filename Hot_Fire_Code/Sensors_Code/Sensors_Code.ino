#define MAXDO 3 //Thermocouple DO on digital pin 3
#define MAXCS 4 // Thermocouple CS on digital pin 4
#define MAXCLK 5 // Thermocouple CLK on digital pin 5
#define PT 17 // Pressure Transducer on analog pin 3 (17)

float TRead // Temperature Read in C
float time // Time values
float PRead // Pressure Read in PSI

Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

void setup() {
Serial.begin(9600);
}

void loop() {
    delay(20) // Don't want this going too fast
    // Let's get the time, Temperature, and Pressure
    time = millis();
    TRead = thermocouple.readCelsius();
    PRead = PReadf();
    // Let's print those
    Serial.println("Here is Temperature: " + string(TRead))
    Serial.println("Here is Pressure: " + string(PRead))
    Serial.println("Here is Time: " + string(time))
    // Now let's store them on the SD card
}

float PReadf() {
    numRead = analogRead(PT);
    milVRead = 5*(numRead/1023);
    PRead = 2000*(pressureReadingInVolts-.498)/5.02;
    return(PRead);
}
