// ************************************* //
// * Arduino Project RFLink-esp        * //
// * https://github.com/couin3/RFLink  * //
// * 2018..2020 Stormteam - Marc RIVES * //
// * More details in RFLink.ino file   * //
// ************************************* //

#ifndef AutoConnect_h
#define AutoConnect_h

#include <Arduino.h>
#include "RFLink.h"

#ifdef ESP8266
#define GET_CHIPID() (ESP.getChipId())
#elif ESP32
#define GET_CHIPID() ((uint32_t)ESP.getEfuseMac())
#endif //ESP8266

void load_JSON_config();

#endif