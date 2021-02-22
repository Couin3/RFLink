//#######################################################################################################
//##                    This Plugin is only for use with the RFLink software package                   ##
//##                                     Plugin-065: 640us-based keyfobs                               ##
//#######################################################################################################
/*********************************************************************************************\
 * This protocol provides support for basic keyfobs, used in garage openers
 *
 * Author  (present)  : StormTeam 2018..2020 - Marc RIVES (aka Couin3)
 * Support (present)  : https://github.com/couin3/RFLink 
 * Author  (original) : StuntTeam 2015..2016
 * Support (original) : http://sourceforge.net/projects/rflink/
 * License            : This code is free for use in any open source project when this header is included.
 *                      Usage of any parts of this code in a commercial application is prohibited!
 *********************************************************************************************
 * Technical data:
 * Devices send 48 pulses, 24 bits total.
 *
 * Sample:
 * 20;XX;DEBUG;Pulses=49;Pulses(uSec)=192,448,160,480,480,160,480,160,448,192,128,512,128,544,96,544,448,192,128,512,160,512,480,160,128,512,480,160,480,192,128,512,480,160,480,160,128,512,448,192,128,512,128,512,128,512,448,192,96;
 * 20;XX;DEBUG;Pulses=49;Pulses(uSec)=160,480,192,448,480,160,480,160,448,192,128,512,128,512,128,512,448,192,96,544,96,512,480,160,128,512,480,160,480,160,128,512,480,160,480,160,128,512,480,160,128,512,128,512,128,512,480,160,96;
 * 00111000 10010110 11010001   = 0x3896d1
 \*********************************************************************************************/
#define GARAGE640_PLUGIN_ID 065
#define PLUGIN_DESC_065 "GARAGE640"

#define GARAGE640_PULSECOUNT 48

#define GARAGE640_PULSEMID (320 / RAWSIGNAL_SAMPLE_RATE)
#define GARAGE640_PULSEMAX (540 / RAWSIGNAL_SAMPLE_RATE)
#define GARAGE640_PULSEMIN (60 / RAWSIGNAL_SAMPLE_RATE)

#ifdef PLUGIN_065
#include "../4_Display.h"

boolean Plugin_065(byte function, char *string) {
   if (RawSignal.Number != GARAGE640_PULSECOUNT || RawSignal.Pulses[0] != 65)
      return false;


   unsigned long bitstream = 0L;
   //==================================================================================
   // Get all 24 bits
   //==================================================================================
   for (byte x = 1; x < GARAGE640_PULSECOUNT; x += 2)
   {
      if (RawSignal.Pulses[x] > GARAGE640_PULSEMID)
      {
         if (RawSignal.Pulses[x] > GARAGE640_PULSEMAX)
            return false; // pulse too long
         if (RawSignal.Pulses[x + 1] > GARAGE640_PULSEMID)
            return false; // invalid pulse sequence 10/01
         bitstream = (bitstream << 1) | 0x1;
      }
      else
      {
         if (RawSignal.Pulses[x] < GARAGE640_PULSEMIN)
            return false; // pulse too short
         if (RawSignal.Pulses[x + 1] < GARAGE640_PULSEMID)
            return false; // invalid pulse sequence 10/01
         bitstream = bitstream << 1;
      }
   }
   //==================================================================================
   // Prevent repeating signals from showing up
   //==================================================================================
   if ((SignalHash != SignalHashPrevious) || ((RepeatingTimer + 200) < millis()) || (SignalCRC != bitstream))
   {
      // not seen the RF packet recently
      if (bitstream == 0)
         return false;

      SignalCRC = bitstream;
   }
   else
   {
      // already seen the RF packet recently
      return true;
   }
   //==================================================================================
   // Validity checks
   //==================================================================================
   // Output
   // ----------------------------------
   display_Header();
   display_Name(PSTR("GARAGE640"));
   display_IDn((bitstream & 0xFFFFFF), 6); // "%S%06lx"
   display_Footer();

   //==================================================================================
   RawSignal.Repeats = true; // suppress repeats of the same RF packet
   RawSignal.Number = 0;
   return true;
}
#endif // Plugin_065
