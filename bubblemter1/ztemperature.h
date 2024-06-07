// zf240607.1144

// Temperature sensor DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>
// ATTENTION, c'est le brochage en VCC -> 0 pour le densimètre où il n'y a PAS de mesure de la tension de la batterie !
// const int vccPin = 0;       // the number of the VCC pin
// const int pullupPin = 1;    // the number of the PULLUP pin
// const int oneWireBus = 2;   // GPIO where the DS18B20 is connected to
// const int gndPin = 3;       // the number of the GND pin
// ATTENTION, c'est le brochage en VCC -> 1 pour le densimètre où il n'y a une mesure de la tension de la batterie !
const int vccPin = 1;       // the number of the VCC pin
const int pullupPin = 2;    // the number of the PULLUP pin
const int oneWireBus = 3;   // GPIO where the DS18B20 is connected to
const int gndPin = 4;       // the number of the GND pin
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);


// Temperature sensor DS18B20 initialising
void initDS18B20Sensor(){
    pinMode(gndPin, OUTPUT);   // gnd
    digitalWrite(gndPin, LOW);
    pinMode(pullupPin, INPUT_PULLUP);   // pull up
    pinMode(vccPin, OUTPUT );   // vcc
    digitalWrite(vccPin, HIGH);
    // Start the DS18B20 sensor
    sensors.begin();
}


RTC_DATA_ATTR float tempInternal1 = 0;
RTC_DATA_ATTR float tempInternal2 = 0;


// Lit les senseurs
void readSensor(){
    // lit la température interne
    sensorValue1 = temperatureRead();
    sensorValue1 = sensorValue1 - 8.0;        // Enlève des ° en trop, je ne sais pas pourquoi ? zf240526.1142, zf240530.0908

    // moyenne glissante
    sensorValue1 = (sensorValue1 + tempInternal1 + tempInternal2) / 3;
    tempInternal1 = tempInternal2;
    tempInternal2 = sensorValue1;

    // lit la température du DS18B20
    sensors.requestTemperatures(); 
    sensorValue5 = sensors.getTempCByIndex(0);
}
