// ************************************* //
// * Arduino Project RFLink-esp        * //
// * https://github.com/couin3/RFLink  * //
// * 2018..2020 Stormteam - Marc RIVES * //
// * More details in RFLink.ino file   * //
// ************************************* //

#ifndef OLED_h
#define OLED_h

#include <Arduino.h>
#include "RFLink.h"

#ifdef OLED_ENABLED

#define PIN_OLED_GND_0 NOT_A_PIN // Ground power on this pin
#define PIN_OLED_VCC_0 NOT_A_PIN // +3 volt / Vcc power on this pin#
#ifdef ESP8266
#define PIN_OLED_SCL_0 D1        // I2C SCL
#define PIN_OLED_SDA_0 D2        // I2C SDA
#elif ESP32
#define PIN_OLED_SCL_0 22        // I2C SCL
#define PIN_OLED_SDA_0 21        // I2C SDA
#endif

extern uint8_t PIN_OLED_GND;
extern uint8_t PIN_OLED_VCC;
extern uint8_t PIN_OLED_SCL;
extern uint8_t PIN_OLED_SDA;



void setup_OLED();
void splash_OLED();
void print_OLED();

#endif // OLED_ENABLED
#endif // OLED_h