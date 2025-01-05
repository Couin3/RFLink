// *******************************************************************
// *  Plugin-084: FANJU Temperature and Humidity Sensor              *
// *******************************************************************
// *  This plugin decodes the signals from FANJU temperature and     *
// *  humidity sensors operating at 433 MHz.                         *
// *                                                                 *
// *  Author: Sébastien LANGE                                              *
// *  Date: 2025-01-05                                               *
// *                                                                 *
// *  License: This code is released under the GNU General Public    *
// *  License v3.0.                                                  *
// *                                                                 *
// *  This code was inspired by the implementation found at:         *
// *  https://github.com/dgomes/homeGW/blob/master/fanju.cpp         *
// *                                                                 *
// *******************************************************************
// *  Technical Details:                                             *
// *  - Protocol: FANJU                                              *
// *  - Pulse count: 82                                              *
// *  - Pulse timing:                                                *
// *    - FANJU_START: 812 / RAWSIGNAL_SAMPLE_RATE                   *
// *    - FANJU_VALUE: 2048 / RAWSIGNAL_SAMPLE_RATE                  *
// *******************************************************************

#define FANJU_PLUGIN_ID 084
#define PLUGIN_DESC_084 PSTR("FANJU")
#define FANJU_PULSECOUNT 82

#define FANJU_START 812 / RAWSIGNAL_SAMPLE_RATE
#define FANJU_VALUE 2048 / RAWSIGNAL_SAMPLE_RATE

#ifdef PLUGIN_084
#include "../4_Display.h"


uint8_t binToDecRev(char *bitArray, int start, int end) {
   uint8_t value = 0;
   for (int i = start; i <= end; i++) {
      value = (value << 1) | (bitArray[i] - '0');
   }
   return value;
}

bool ChkSum(char *bitArray) {
   uint8_t mask = 0xc;
   uint8_t x, bit, csum = 0;

   uint8_t chkSum = binToDecRev(bitArray, 8, 11);

   int binary_cpy[40];

   // move channel to the checksum position
   x = 0;
   for (unsigned i = 0; i < 8; i++) {
      binary_cpy[x++] = bitArray[i] - '0';
   }
   for (unsigned i = 36; i < 40; i++) {
      binary_cpy[x++] = bitArray[i] - '0';
   }
   for (unsigned i = 12; i < 36; i++) {
      binary_cpy[x++] = bitArray[i] - '0';
   }

   // verify checksum
   for (unsigned i = 0; i < 36; i++) {
      bit = mask & 0x1;
      mask >>= 1;
      if (bit == 0x1) {
         mask ^= 0x9;
      }
      if (binary_cpy[i] == 1) {
         csum ^= mask;
      }
   }

   return csum == chkSum;
}

boolean Plugin_084(byte function, char *string)
{
   char dbuffer[64];

   if (RawSignal.Number != FANJU_PULSECOUNT) {
      return false;
   }

   // Decode FANJU protocol
   char bitArray[80] = {0}; // Array to store bits for temperature and humidity
   int bitIndex = 0;

   for (int i = 1; i <= RawSignal.Number+1; i += 2) {

      if (RawSignal.Pulses[i] < FANJU_START) {
         if (RawSignal.Pulses[i + 1] < FANJU_VALUE) {
            // It's a 0
            bitArray[bitIndex] = '0';
         } else {
            // It's a 1
            bitArray[bitIndex] = '1';
         }
         bitIndex++;
      }
   }

#ifdef PLUGIN_084_DEBUG
   // Print the bit array in groups of 8
   Serial.print("Bits: ");
   for (int i = 0; i < bitIndex; i++) {
      Serial.print(bitArray[i]);
      if ((i + 1) % 8 == 0) {
         Serial.print(" ");
      }
   }
   Serial.println();
#endif

   // Convert bitArray to temperature and humidity
   int id = 0;
   int channel = 0;
   int battery = 0;
   int temperature = 0;
   int humidityTens = 0;
   int humidityUnits = 0;

   for (int i = 0; i < 8; i++) {
      id = (id << 1) | (bitArray[i] - '0');
   }

   for (int i = 38; i < 40; i++) {
      channel = (channel << 1) | (bitArray[i] - '0');
   }

   battery = bitArray[13] - '0';

   // Extract the tens place of humidity (bits 28 to 31)
   for (int i = 28; i < 32; i++) {
      humidityTens = (humidityTens << 1) | (bitArray[i] - '0');
   }

   // Extract the units place of humidity (bits 32 to 35)
   for (int i = 32; i < 36; i++) {
      humidityUnits = (humidityUnits << 1) | (bitArray[i] - '0');
   }

   // Combine tens and units to get the full humidity value
   int humidity = humidityTens * 10 + humidityUnits;

   for (int i = 16; i < 28; i++) {
      temperature = (temperature << 1) | (bitArray[i] - '0');
   }

   // Calculate checksum
   if (!ChkSum(bitArray)) {
      Serial.println("Checksum error");
      return false;
   }

   // Convert temperature and humidity to human-readable format
   // Convert temperature from Fareheit to Celsius
   float tempFarenheit = (temperature - 900) / 10.0;
   float tempCelsius = (tempFarenheit - 32) * 5 / 9;

      // Check temperature range
   if (tempCelsius < -40 || tempCelsius > 90) {
      Serial.println("Temperature out of range");
      return false;
   }

   // Check humidity range
   if (humidity < 0 || humidity > 100) {
      Serial.println("Humidity out of range");
      return false;
   }

#ifdef PLUGIN_084_DEBUG
   // Print the decoded values
   char floatStr[10]; // float converted to string
   dtostrf(tempCelsius, 8, 1, floatStr);  // 8 char min total width, 6 after decimal
   sprintf(dbuffer, "ID: %d, Channel: %d, Battery: %d, Temperature: %s°C, Humidity: %d%%", id, channel, battery, floatStr, humidity);
   Serial.println(dbuffer);

   // Debugging: Print raw values
   Serial.print("Raw ID value: ");
   Serial.println(id);
   Serial.print("Raw Channel value: ");
   Serial.println(channel);
   Serial.print("Raw Battery value: ");
   Serial.println(battery);
   Serial.print("Raw temperature value: ");
   Serial.println(temperature);
   Serial.print("Raw humidity value: ");
   Serial.println(humidity);
#endif

      // ----------------------------------
      // Output
      // ----------------------------------
      display_Header();
      display_Name(PSTR("FANJU"));
      display_IDn((id << 8 | channel), 4);
      display_TEMP((int)(tempCelsius * 10));
      display_HUM(humidity, HUM_HEX);
      boolean batOK=!battery;
      display_BAT(batOK);
      display_Footer();

   RawSignal.Repeats = true; // suppress repeats of the same RF packet
   RawSignal.Number = 0;

   return true;
}

#endif