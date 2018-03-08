/** ONLY FOR SERIAL3 **/
/** TODO:
    Change copyright
    Change discoMask input type to be just 8 bits long
    Finish other serials
    Track duration/Total count
    Caller name/number
    <Y,?> output type
    Go through variable sizes
    Test ALL commands
**/
/* Teensy Library
 * Derived from Teensy's hardware serial
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2017 PJRC.COM, LLC.
 * A significant number of RN52 specific functions have been taken from 
 * the RN52 library written by Thomas Cousins for http://doayee.co.uk
 * Adapted for hardware serial by me.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "RN52_HW.h"
#include "WString.h"
HardwareSerial3 Serial3;

/**
    mask stuff
**/
short RN52_HW3::IOMask = 0x0004;
short RN52_HW3::IOProtect = 0x3C64;
short RN52_HW3::IOStateMask = 0x0094;
short RN52_HW3::IOStateProtect = 0x3CF4;
short RN52_HW3::IO;
short RN52_HW3::IOState;

/**
    Two gifts from Paul
**/
void serialEvent3() __attribute__((weak));
void serialEvent3() {}

/**
    This function sets the RN52's GPIO pin directions.
    Only PIN 5, 10, 11, 12 & 13 are modifiable.
    print() and println() appear to be non virtual functions in the
    print class (parent of stream) that call write(), so print via UART.  
**/
bool RN52_HW3::GPIOPinMode(int pin, bool state)
{
  if (state) IO = IO | (1 << pin);  //select pin to change if state is 1
  else {                            //select pin to change if state is 0, using mask to protect command mode
    short mask = 65535 ^ (1 << pin);
    IO = IO & mask;
  }
  short toWrite = (IO | IOMask) & IOProtect;    //ensure that we do not try to overwrite non programmable IO
  print("I@,"); //send command
  if (toWrite < 4096) print("0");
  if (toWrite < 256) print("0");
  if (toWrite < 16) print("0");
  println(toWrite, HEX);
  while (available() == 0); //wait for ACK (AOK\r\n or AOK\n\r, I forget which)
  char c = read();
  if (c == 'A') {
    delay(100);
    flush();
    return 1;
  }
  else {
    delay(100);
    flush();
    return 0;
  }
  delay(50);
}

/**
    Writes outputs high or low, if inputs enables/disables internal pullup
**/
void RN52_HW3::GPIODigitalWrite(int pin, bool state)
{
  if (state) IOState = IOState | (1 << pin);    //select pin to change if state is 1
  else {                                        //select pin to change if state is 0, using mask to protect command mode
    short mask = 65535 ^ (1 << pin);
    IOState = IOState & mask;
  }
  short toWrite = (IOState | IOStateMask) & IOStateProtect; //ensure that we do not try to overwrite non programmable IO
  print("I&,"); //send command
  if (toWrite < 4096) print("0");
  if (toWrite < 256) print("0");
  if (toWrite < 16) print("0");
  println(toWrite, HEX);
  delay(50);
}

/**
    Reads back the current state of the GPIO
**/
bool RN52_HW3::GPIODigitalRead(int pin)
{
  while (available() > 0) //clear read buffer
  {
    char c = read();
  }
  println("I&");    //send command
  delay(100);
  short valueIn = 0;
  for (int i = 0; i < 4; i++)   // convert hex response to binary
  {
    while (available() == 0);
    char c = read();
    if (c >= '0' && c <= '9')
    {
      valueIn *= 16;
      valueIn += (c - '0');
    }
    else if (c >= 'A' && c <= 'F')
    {
      valueIn *= 16;
      valueIn += (c - 'A') + 10;
    }
  }
  return (valueIn & (1 << pin)) >> pin; //return the bit corresponding to the pin of choice
}

/**
    Does what it says on the tin
**/
void RN52_HW3::setDiscoverability(bool discoverable)
{
  print("@,");
  println(discoverable);
  delay(50);
}

/**
    Does what it says on the tin
**/
void RN52_HW3::toggleEcho()
{
  println("+");
  delay(50);
}

/**
    Sets the name of the RN52
**/
void RN52_HW3::name(String nom, bool normalized)
{
  print("S");
  if (normalized) print("-,");  //retain the last part of the MAC address
  else print("N,");
  println(nom);
  delay(50);
}

/**
    Gets the name of the RN52
**/
String RN52_HW3::name(void)
{
  while (available() > 0)   //clear buffer
  {
    char c = read();
  }
  println("GN");
  String nom;
  char c;
  while (available() == 0);
  while (c != '\r')
  {
    c = read();
    nom += c; //String.h's overloaded + operator adds new character to String?
  }
  return nom;
}

/**
    Does what it says on the tin
**/
void RN52_HW3::factoryReset()
{
  println("SF,1");
  delay(50);
}

/**
    Gets the idle power down timer
**/
int RN52_HW3::idlePowerDownTime(void)
{
  while (available() > 0)   //clear buffer
  {
    char c = read();
  }
  int timer = 0;
  println("G^");
  while (available() == 0);
  delay(50);
  while (available() > 2)
  {
    char c = read();
    timer *= 10;
    timer += (c - '0');
    delay(50);
  }
  return timer;
}
/**
    Sets the idle power down timer
**/
void RN52_HW3::idlePowerDownTime(int timer)
{
  print("S^,");
  println(timer);
  delay(50);
}

/**
    Does what it says on the tin
**/
void RN52_HW3::reboot()
{
  println("R,1");
  delay(2000);
}

/**
    Calls the specified number
**/
void RN52_HW3::call(String number)
{
  print("A,");
  println(number);
  delay(50);
}

/**
    Ends the current call
**/
void RN52_HW3::endCall()
{
  println("E");
  delay(50);
}

/**
    Toggles pause on the music
**/
void RN52_HW3::playPause()
{
  println("AP");
  delay(50);
}

/**
    Skips forward a track
**/
void RN52_HW3::nextTrack()
{
  println("AT+");
  delay(50);
}

/**
    Returns to the previous track
**/
void RN52_HW3::prevTrack()
{
  println("AT-");
  delay(50);
}

/** 
    Gets the track's metadata
**/
String RN52_HW3::getMetaData()
{
  while (available() > 0)   //clear buffer
  {
    char c = read();
  }
  println("AD");
  while (available() == 0);
  String metaData;
  int i = 6;
  long count;
  while (i != 0) //loop six times for six lines of data
  {
    if (available() > 0)
    {
      char c = read();
      count = millis();
      metaData += c;
      if (c == '\n') i--;
    }
    if ((millis() - count) > 500) i--;  //timeout
  }
  return metaData;
}

/** 
    Extracts the song title from the metadata
**/
String RN52_HW3::trackTitle()
{
  String metaData = getMetaData();
  int n = metaData.indexOf("Title=") + 6;
  if (n != -1) {    //removes everything in metadata String before title & everything after newline char
    metaData.remove(0, n);
    n = metaData.indexOf('\n');
    metaData.remove(n);
  }
  else metaData = "";
  return metaData;
}

/** 
    Extracts the album from the metadata
**/
String RN52_HW3::album()
{
  String metaData = getMetaData();
  int n = metaData.indexOf("Album=") + 6;
  if (n != -1) {    //removes everything in metadata String before album & everything after newline char
    metaData.remove(0, n);
    n = metaData.indexOf('\n');
    metaData.remove(n);
  }
  else metaData = "";
  return metaData;
}

/** 
    Extracts the artist from the metadata
**/
String RN52_HW3::artist()
{
  String metaData = getMetaData();
  int n = metaData.indexOf("Artist=") + 7;
  if (n != -1) {
    metaData.remove(0, n);
    n = metaData.indexOf('\n');
    metaData.remove(n);
  }
  else metaData = "";
  return metaData;
}

/** 
    Extracts the genre from the metadata
**/
String RN52_HW3::genre()
{
  String metaData = getMetaData();
  int n = metaData.indexOf("Genre=") + 6;
  if (n != -1) {
    metaData.remove(0, n);
    n = metaData.indexOf('\n');
    metaData.remove(n);
  }
  else metaData = "";
  return metaData;
}

/** 
    Extracts the track number from the metadata - what even is this? For CD players?
**/
int RN52_HW3::trackNumber()
{
  int trackNumber;
  String metaData = getMetaData();
  int n = metaData.indexOf("TrackNumber=") + 12;
  if (n != -1) {
    metaData.remove(0, n);
    n = metaData.indexOf('\n');
    metaData.remove(n);
    trackNumber = metaData.toInt();
  }
  else return 0;
  return trackNumber;
}

/** 
    Extracts the track duration from the metadata
**/
int RN52_HW3::trackDuration()
{
    return 0;
}

/** 
    Extracts the totalCount from the metadata
**/
int RN52_HW3::totalCount()
{
    return 0;
}

/** 
    Gets the currently active extended features:
        Bit 0 – Enable AVRCP buttons for EK
        Bit 1 – Enable reconnect on power-on
        Bit 2 – Discoverable on start up
        Bit 3 – Codec indicators PIO7 (AAC) and PIO6 (aptX)
        Bit 4 – Reboot after disconnect
        Bit 5 – Mute volume up/down tones
        Bit 6 – Enable voice command button on PIO4
        Bit 7 – Disable system tones
        Bit 8 – Power off after pairing timeout
        Bit 9 – Reset after power off
        Bit 10 – Enable list reconnect after panic
        Bit 11 – Enable latch event indicator PIO2
        Bit 12 – Enable track change event
        Bit 13 – Enable tones playback at fixed volume
        Bit 14 – Enable auto-accept passkey in Keyboard I/O Authentication mode
**/
short RN52_HW3::getExtFeatures()
{
  while (available() > 0)   //clear buffer
  {
    char c = read();
  }
  short valueIn = 0;
  char c;
  while (c != '\r') //continue until message terminator reached
  {
    while (available() == 0) {  //send command
      println("G%");
      delay(50);
    }
    c = read();
    if (c >= '0' && c <= '9') //once again conversion from ascii hex to binary
    {
      valueIn *= 16;
      valueIn += (c - '0');
    }
    else if (c >= 'A' && c <= 'F')
    {
      valueIn *= 16;
      valueIn += (c - 'A') + 10;
    }
    else if ((c == '?') || (c == '!'))
    {
      while (available() > 0)
      {
        char d = read();
      }
    }
  }
  return valueIn;
}

/** 
    Sets an extended feature:
        Bit 0 – Enable AVRCP buttons for EK
        Bit 1 – Enable reconnect on power-on
        Bit 2 – Discoverable on start up
        Bit 3 – Codec indicators PIO7 (AAC) and PIO6 (aptX)
        Bit 4 – Reboot after disconnect
        Bit 5 – Mute volume up/down tones
        Bit 6 – Enable voice command button on PIO4
        Bit 7 – Disable system tones
        Bit 8 – Power off after pairing timeout
        Bit 9 – Reset after power off
        Bit 10 – Enable list reconnect after panic
        Bit 11 – Enable latch event indicator PIO2
        Bit 12 – Enable track change event
        Bit 13 – Enable tones playback at fixed volume
        Bit 14 – Enable auto-accept passkey in Keyboard I/O Authentication mode
**/
void RN52_HW3::setExtFeatures(bool state, int bit)
{
  short toWrite;
  if (state) toWrite = getExtFeatures() | (1 << bit);
  else toWrite = getExtFeatures() & (65535 ^ (1 << bit));
  print("S%,");
  if (toWrite < 4096) print("0");
  if (toWrite < 256)  print("0");
  if (toWrite < 16)   print("0");
  println(toWrite, HEX);
  delay(100);
}

/** 
    Sets the extended features:
        Bit 0 – Enable AVRCP buttons for EK
        Bit 1 – Enable reconnect on power-on
        Bit 2 – Discoverable on start up
        Bit 3 – Codec indicators PIO7 (AAC) and PIO6 (aptX)
        Bit 4 – Reboot after disconnect
        Bit 5 – Mute volume up/down tones
        Bit 6 – Enable voice command button on PIO4
        Bit 7 – Disable system tones
        Bit 8 – Power off after pairing timeout
        Bit 9 – Reset after power off
        Bit 10 – Enable list reconnect after panic
        Bit 11 – Enable latch event indicator PIO2
        Bit 12 – Enable track change event
        Bit 13 – Enable tones playback at fixed volume
        Bit 14 – Enable auto-accept passkey in Keyboard I/O Authentication mode
**/
void RN52_HW3::setExtFeatures(short settings)
{
  short toWrite = settings;
  print("S%,");
  if (toWrite < 4096) print("0");
  if (toWrite < 256)  print("0");
  if (toWrite < 16)   print("0");
  println(toWrite, HEX);
  delay(100);
}

/** 
    Returns Bit 0 of the ext features result
        GPIO10  Volume Down
        GPIO11  Previous Track
        GPIO12  Next Track
        GPIO13  Play/Pause
        GPIO5   Volume Up
**/
bool RN52_HW3::AVRCPButtons()
{
  return (getExtFeatures()) & 1;
}

/** 
    Sets Bit 0 of the ext features 
        GPIO10  Volume Down
        GPIO11  Previous Track
        GPIO12  Next Track
        GPIO13  Play/Pause
        GPIO5   Volume Up
**/
void RN52_HW3::AVRCPButtons(bool state)
{
  setExtFeatures(state, 0);
}

/** 
    Returns Bit 1 of the ext features result
**/
bool RN52_HW3::powerUpReconnect()
{
  return (getExtFeatures() & (1 << 1)) >> 1;
}

/** 
    Sets Bit 1 of the ext features 
**/
void RN52_HW3::powerUpReconnect(bool state)
{
  setExtFeatures(state, 1);
}

/** 
    Returns Bit 2 of the ext features result
**/
bool RN52_HW3::startUpDiscoverable()
{
  return (getExtFeatures() & (1 << 2)) >> 2;
}

/** 
    Sets Bit 2 of the ext features 
**/
void RN52_HW3::startUpDiscoverable(bool state)
{
  setExtFeatures(state, 2);
}

/** 
    Returns Bit 3 of the ext features result
**/
bool RN52_HW3::codecIndicators()
{
  return (getExtFeatures() & (1 << 3)) >> 3;
}

/** 
    Sets Bit 3 of the ext features 
**/

void RN52_HW3::codecIndicators(bool state)
{
  setExtFeatures(state, 3);
}
/** 
    Returns Bit 4 of the ext features result
**/
bool RN52_HW3::rebootOnDisconnect()
{
  return (getExtFeatures() & (1 << 4)) >> 4;
}

/** 
    Sets Bit 4 of the ext features 
**/
void RN52_HW3::rebootOnDisconnect(bool state)
{
  setExtFeatures(state, 4);
}

/** 
    Returns Bit 5 of the ext features result
**/
bool RN52_HW3::volumeToneMute()
{
  return (getExtFeatures() & (1 << 5)) >> 5;
}

/** 
    Sets Bit 5 of the ext features 
**/
void RN52_HW3::volumeToneMute(bool state)
{
  setExtFeatures(state, 5);
}

/** 
    Returns Bit 6 of the ext features result
**/
bool RN52_HW3::voiceCommandButton()
{
  return (getExtFeatures() & (1 << 6)) >> 6;
}

/** 
    Sets Bit 6 of the ext features 
**/
void RN52_HW3::voiceCommandButton(bool state)
{
  setExtFeatures(state, 6);
}
/** 
    Returns Bit 7 of the ext features result
**/
bool RN52_HW3::systemTonesDisabled()
{
  return (getExtFeatures() & (1 << 7)) >> 7;
}

/** 
    Sets Bit 7 of the ext features 
**/
void RN52_HW3::systemTonesDisabled(bool state)
{
  setExtFeatures(state, 7);
}

/** 
    Returns Bit 8 of the ext features result
**/
bool RN52_HW3::powerDownAfterPairingTimeout()
{
  return (getExtFeatures() & (1 << 8)) >> 8;
}

/** 
    Sets Bit 8 of the ext features 
**/
void RN52_HW3::powerDownAfterPairingTimeout(bool state)
{
  setExtFeatures(state, 8);
}

/** 
    Returns Bit 9 of the ext features result
**/
bool RN52_HW3::resetAfterPowerDown()
{
  return (getExtFeatures() & (1 << 9)) >> 9;
}

/** 
    Sets Bit 9 of the ext features result
**/
void RN52_HW3::resetAfterPowerDown(bool state)
{
  setExtFeatures(state, 9);
}

/** 
    Returns Bit 10 of the ext features result
**/
bool RN52_HW3::reconnectAfterPanic()
{
  return (getExtFeatures() & (1 << 10)) >> 10;
}

/** 
    Sets Bit 10 of the ext features result
**/
void RN52_HW3::reconnectAfterPanic(bool state)
{
  setExtFeatures(state, 10);
}

/** 
    Returns Bit 11 of the ext features result
**/
bool RN52_HW3::latchEventIndicator()
{
  return (getExtFeatures() & (1 << 11)) >> 11;
}

/** 
    Sets Bit 11 of the ext features result
**/
void RN52_HW3::latchEventIndicator(bool state)
{
  setExtFeatures(state, 11);
}
/** 
    Returns Bit 12 of the ext features result
**/
bool RN52_HW3::trackChangeEvent()
{
  return (getExtFeatures() & (1 << 12)) >> 12;
}

/** 
    Sets Bit 12 of the ext features result
**/
void RN52_HW3::trackChangeEvent(bool state)
{
  setExtFeatures(state, 12);
}

/** 
    Returns Bit 13 of the ext features result
**/
bool RN52_HW3::tonesAtFixedVolume()
{
  return (getExtFeatures() & (1 << 13)) >> 13;
}

/** 
    Sets Bit 13 of the ext features result
**/
void RN52_HW3::tonesAtFixedVolume(bool state)
{
  setExtFeatures(state, 13);
}

/** 
    Returns Bit 14 of the ext features result
**/
bool RN52_HW3::autoAcceptPasskey()
{
  return (getExtFeatures() & (1 << 14)) >> 14;
}

/** 
    Sets Bit 14 of the ext features result
**/
void RN52_HW3::autoAcceptPasskey(bool state)
{
  setExtFeatures(state, 14);
}

/** 
    Gets the Rn52's startup volume
**/
int RN52_HW3::volumeOnStartup(void)
{
  while (available() > 0)   //clear buffer
  {
    char c = read();
  }
  println("GS");
  delay(100);
  int vol = 0;
  char c;
  while (c != '\r')
  {
    while (available() == 0);
    c = read();
    if (c >= '0' && c <= '9')
    {
      vol *= 16;
      vol += (c - '0');
    }
    else if (c >= 'A' && c <= 'F')
    {
      vol *= 16;
      vol += (c - 'A') + 10;
    }
  }
  return vol;
}

/** 
    Sets the Rn52's startup volume
**/
void RN52_HW3::volumeOnStartup(int vol)
{
  print("SS,");
  print("0");
  println(vol, HEX);
  delay(50);
}

/** 
    Raises the current volume
**/
void RN52_HW3::volumeUp(void)
{
  println("AV+");
  delay(50);
}

/** 
    Lowers the current volume
**/
void RN52_HW3::volumeDown(void)
{
  println("AV-");
  delay(50);
}

/** 
    Gets the Rn52's audio routing. Returned as <AUWR>.
        AU:
            00  Analog output (default)
            01  Set the output for I2S.
            02  Set the output for S/PDIF
            03  Intercom DAC mode
        W (Bits per sample):
            0   24
            1   32  
        R (Samples Per Second):
            0   8K
            1   32K
            2   44K1
            3   48K
**/
short RN52_HW3::getAudioRouting()
{
  while (available() > 0)   //clear buffer
  {
    char c = read();
  }
  println("G|");
  delay(100);
  short routing = 0;
  char c;
  while (c != '\r')
  {
    while (available() == 0);
    c = read();
    if (c >= '0' && c <= '9')
    {
      routing *= 16;
      routing += (c - '0');
    }
    else if (c >= 'A' && c <= 'F')
    {
      routing *= 16;
      routing += (c - 'A') + 10;
    }
  }
  return routing;
}

/** 
    Gets the Rn52's sample width.
        W (Bits per sample):
            0   24
            1   32
**/
int RN52_HW3::sampleWidth()
{
  int width = (getAudioRouting() & 0x00F0) >> 4;
  return width;
}

/** 
    Sets the Rn52's sample width.
        W (Bits per sample):
            0   24
            1   32
**/
void RN52_HW3::sampleWidth(int width)
{
  short mask = getAudioRouting() & 0xFF0F;
  short toWrite = mask | (width << 4);
  print("S|,");
  if (toWrite < 4096) print("0");
  if (toWrite < 256) print("0");
  if (toWrite < 16) print("0");
  println(toWrite, HEX);
  delay(50);
}

/** 
    Gets the Rn52's audio routing.
        R (Samples Per Second):
            0   8K
            1   32K
            2   44K1
            3   48K
**/
int RN52_HW3::sampleRate()
{
  int rate = getAudioRouting() & 0x000F;
  return rate;
}

/** 
    Sets the Rn52's audio routing.
        R (Samples Per Second):
            0   8K
            1   32K
            2   44K1
            3   48K
**/
void RN52_HW3::sampleRate(int rate)
{
  short mask = getAudioRouting() & 0xFFF0;
  short toWrite = mask | rate;
  print("S|,");
  if (toWrite < 4096) print("0");
  if (toWrite < 256) print("0");
  if (toWrite < 16) print("0");
  println(toWrite, HEX);
  delay(50);
}

/** 
    Gets the Rn52's audio routing.
        AU:
            00  Analog output (default)
            01  Set the output for I2S.
            02  Set the output for S/PDIF
            03  Intercom DAC mode

**/
int RN52_HW3::A2DPRoute()
{
  int route = (getAudioRouting() & 0x0F00) >> 8;
  return route; 
}

/** 
    Sets the Rn52's audio routing.
        AU:
            00  Analog output (default)
            01  Set the output for I2S.
            02  Set the output for S/PDIF
            03  Intercom DAC mode

**/
void RN52_HW3::A2DPRoute(int route)
{
  short mask = getAudioRouting() & 0x00FF;
  short toWrite = mask | (route << 8);
  print("S|,");
  if (toWrite < 4096) print("0");
  if (toWrite < 256) print("0");
  if (toWrite < 16) print("0");
  println(toWrite, HEX);
  delay(50);
}

/**
    Gets the authentication settings
        0   Open
        1   SSP KB I/O
        2   SSP "Just Works"
        4   Pin Code (Legacy)
**/
int RN52_HW3::authentication(void) 
{
  while (available() > 0)   //clear buffer
  {
    char c = read();
  }
  int auth = 0;
  println("GA");
  while (available() == 0);
  delay(50);
  while (available() > 2)   // returned as decimal
  {
    char c = read();
    auth *= 10;
    auth += (c - '0');
    delay(50);
  }
  return auth;
}
/**
    Sets the authentication settings
        0   Open
        1   SSP KB I/O
        2   SSP "Just Works"
        4   Pin Code (Legacy)
**/
void RN52_HW3::authentication(int auth)
{
  print("SA,");
  println(auth);
  delay(50);

/**
    Gets the Class of Device
    See AT command guide/https://www.bluetooth.org/apps/content for more info.
**/
int RN52_HW3::classOfDevice(void)
{
  while (available() > 0)   //clear buffer
  {
    char c = read();
  }
  int cod = 0;
  println("GC");
  while (available() == 0);
  delay(50);
  while (available() > 2)    // returned as decimal
  {
    char c = read();
    cod *= 10;
    cod += (c - '0');
    delay(50);
  }
  return auth;
}
/**
    Sets the Class of Device  
    See AT command guide/https://www.bluetooth.org/apps/content for more info.
**/
void RN52_HW3::classOfDevice(int cod)
{
  print("SC,");
  println(cod);
  delay(50);
}

/**
    Sets the discovery mask
        Bit Position    Value   Profile
        0               01      iAP
        1               02      SPP
        2               04      A2DP
        3               08      HFP 
        Default is 0xFF
**/
void RN52_HW3::discoveryMask(int mask)
{
    print("SD,"); //send command
    if (mask < 16) print("0");
    println(mask, HEX);
    delay(50);
}

/**
    Gets the discovery mask
        Bit Position    Hex Value   Profile
        0               01          iAP
        1               02          SPP
        2               04          A2DP
        3               08          HFP        
        Default is 0xFF
**/
int RN52_HW3::discoveryMask(void)
{
    while (available() > 0) //clear read buffer
    {
        char c = read();
    }
    println("GD");    //send command
    delay(100);
    int valueIn = 0;
    for (int i = 0; i < 2; i++)   // convert hex response to binary
    {
        while (available() == 0);
        char c = read();
        if (c >= '0' && c <= '9')
        {
            valueIn *= 16;
            valueIn += (c - '0');
        }
        else if (c >= 'A' && c <= 'F')
        {
            valueIn *= 16;
            valueIn += (c - 'A') + 10;
        }
    }
    return valueIn;
}

/**
    Sets the connection mask
        Bit Position    Value   Profile
        0               01      iAP
        1               02      SPP
        2               04      A2DP
        3               08      HFP 
        Default is 0xFF
**/
void RN52_HW3::connectionMask(int mask)
{
    print("SK,"); //send command
    if (mask < 16) print("0");
    println(mask, HEX);
    delay(50);
}

/**
    Gets the connection mask
        Bit Position    Hex Value   Profile
        0               01          iAP
        1               02          SPP
        2               04          A2DP
        3               08          HFP        
        Default is 0xFF
**/
int RN52_HW3::connectionMask(void)
{
    while (available() > 0) //clear read buffer
    {
        char c = read();
    }
    println("GK");    //send command
    delay(100);
    int valueIn = 0;
    for (int i = 0; i < 2; i++)   // convert hex response to binary
    {
        while (available() == 0);
        char c = read();
        if (c >= '0' && c <= '9')
        {
            valueIn *= 16;
            valueIn += (c - '0');
        }
        else if (c >= 'A' && c <= 'F')
        {
            valueIn *= 16;
            valueIn += (c - 'A') + 10;
        }
    }
    return valueIn;
}

/**
    Sets the microphone level
        Format of the command parameter is <resresMBMA>.
        MA - gain of microphone A
        MB - gain of microphone B
        Levels from 00 -> 1F
        Default is 0x0909
**/
void RN52_HW3::microphoneLevel(int levels)
{
    print("SM,0000"); //send command, first 4 digits are reserved
    if (levels < 4096) print("0");
    if (levels < 256) print("0");
    if (levels < 16) print("0");
    println(levels, HEX);
    delay(50);
}

/**
    Gets the microphone level
        Format of the command parameter is <MBMA>.
        MA - gain of microphone A
        MB - gain of microphone B
        Levels from 00 -> 1F
        Default is 0x0909
**/
int RN52_HW3::microphoneLevel(void)
{
    while (available() > 0) //clear read buffer
    {
        char c = read();
    }
    println("GM");    //send command
    delay(100);
    int valueIn = 0;
    for (int i = 0; i < 8; i++) //convert hex response to binary
    {                           //first 4 digits will be zero so can still output an int
        while (available() == 0);
        char c = read();
        if (c >= '0' && c <= '9')
        {
            valueIn *= 16;
            valueIn += (c - '0');
        }
        else if (c >= 'A' && c <= 'F')
        {
            valueIn *= 16;
            valueIn += (c - 'A') + 10;
        }
    }
    return valueIn;
}

/**
    Sets the security pin code
        Up to 20 alphanumeric characters

**/
void RN52_HW3::pincode(String pinCode)
{
    print("SP,"); //send command
    println(pinCode);
    delay(50);
}

/**
    Gets the security pin code
        Up to 20 alphanumeric characters
**/
String RN52_HW3::pincode(void)
{
  while (available() > 0)   //clear buffer
  {
    char c = read();
  }
  println("GP");
  String pinCode;
  char c;
  while (available() == 0);
  while (c != '\r')
  {
    c = read();
    pinCode += c; //String.h's overloaded + operator adds new character to String?
  }
  return pinCode;
}

/**
    Sets the tone level
        Levels from 00 -> 1F
**/
void RN52_HW3::toneGain(int gain)
{
    print("ST,"); //send command
    if (gain < 16) print("0");
    println(gain, HEX);
    delay(50);
}

/**
    Gets the tone level
        Levels from 00 -> 1F
**/
int RN52_HW3::toneGain(void)
{
    while (available() > 0) //clear read buffer
    {
        char c = read();
    }
    println("GT");    //send command
    delay(100);
    int valueIn = 0;
    for (int i = 0; i < 2; i++)   // convert hex response to binary
    {
        while (available() == 0);
        char c = read();
        if (c >= '0' && c <= '9')
        {
            valueIn *= 16;
            valueIn += (c - '0');
        }
        else if (c >= 'A' && c <= 'F')
        {
            valueIn *= 16;
            valueIn += (c - 'A') + 10;
        }
    }
    return valueIn;
}

/**
    Sets  the UART baudrate
        01  9600 bps
        02  19200 bps
        03  38400 bps
        04  57600 bps
        05  115200 bps
        06  230400 bps
        07  460800 bps
**/
void RN52_HW3::uartBaud(int rate)
{
    print("SU,0"); //send command
    println(rate, HEX);
    delay(50);
}

/**
    Gets  the UART baudrate
        01  9600 bps
        02  19200 bps
        03  38400 bps
        04  57600 bps
        05  115200 bps
        06  230400 bps
        07  460800 bps
**/
int RN52_HW3::uartBaud(void)
{
    while (available() > 0) //clear read buffer
    {
        char c = read();
    }
    println("GU");    //send command
    delay(100);
    int valueIn = 0;
    for (int i = 0; i < 2; i++)   // convert hex response to binary
    {
        while (available() == 0);
        char c = read();
        if (c >= '0' && c <= '9')
        {
            valueIn *= 16;
            valueIn += (c - '0');
        }
        else if (c >= 'A' && c <= 'F')
        {
            valueIn *= 16;
            valueIn += (c - 'A') + 10;
        }
    }
    return valueIn;
}

/**
    Gets the delay between connection tries in milliseconds
    Default 0
**/
int RN52_HW3::connDelay(void)
{
  while (available() > 0)   //clear buffer
  {
    char c = read();
  }
  int cod = 0;
  println("GTA");
  while (available() == 0);
  delay(50);
  while (available() > 2)    // returned as decimal
  {
    char c = read();
    cod *= 10;
    cod += (c - '0');
    delay(50);
  }
  return auth;
}
/**
    Sets the delay between connection tries in milliseconds
    Default 0
**/
void RN52_HW3::connDelay(int cDelay)
{
  print("STA,");
  println(cDelay);
  delay(50);
}

/**
    Gets the pairing timeout in seconds.
    Default 0
**/
int RN52_HW3::pairingTimeout(void)
{
  while (available() > 0)   //clear buffer
  {
    char c = read();
  }
  int cod = 0;
  println("GTP");
  while (available() == 0);
  delay(50);
  while (available() > 2)    // returned as decimal
  {
    char c = read();
    cod *= 10;
    cod += (c - '0');
    delay(50);
  }
  return auth;
}
/**
    Sets the pairing timeout in seconds.
    Default 0
**/
void RN52_HW3::pairingTimeout(int pTimeout)
{
  print("STP,");
  println(pTimeout);
  delay(50);
}

/** 
    Gets audio gateway's battery level - guessing this is the audio source?
**/
String RN52_HW3::getBatteryLevel()
{
  while (available() > 0)   //clear buffer
  {
    char c = read();
  }
  println("GB");
  while (available() == 0);
  String batteryLevel;
  if (available() > 0)
    {
      char c = read();
      batteryLevel += c;
      if (c == '\n') i--;
    }
  int n = batteryLevel.indexOf("AGBatteryLevel =") + 16;
  if (n != -1) {    //removes everything in metadata String before title & everything after newline char
    batteryLevel.remove(0, n);
    n = batteryLevel.indexOf('\n');
    batteryLevel.remove(n);
  }
  else batteryLevel = "";
  return batteryLevel;
}

/**
    Gets the firmware version of the RN52
**/
String RN52_HW3::firmwareV(void)
{
  while (available() > 0)   //clear buffer
  {
    char c = read();
  }
  println("V");
  String ver;
  char c;
  while (available() == 0);
  while (c != '\r')
  {
    c = read();
    ver += c; //String.h's overloaded + operator adds new character to String?
  }
  return ver;
}

/**
    Does what it says on the tin - 0 rejects & 1 accepts
**/
void RN52_HW3::authAcceptReject(bool action)
{
  print("#,");
  println(action);
  delay(50);
}

/**
    Put the RN52 module into Device Firmware Upgrade mode - 0 rejects & 1 accepts
**/
void RN52_HW3::enterDFU(void)
{
  println("$");
  delay(50);
}

/**
    Redial last dialed number.
**/
void RN52_HW3::redailLast(void)
{
  println("AR");
  delay(50);
}

/**
    Connect to last device
**/
void RN52_HW3::reconnectLast(void)
{
  println("B");
  delay(50);
}

/**
    Accept incoming call
**/
void RN52_HW3::acceptCall(void)
{
  println("C");
  delay(50);
}

/**
    Reject incoming call or terminate call
**/
void RN52_HW3::killCall(void)
{
  println("E");
  delay(50);
}

/**
    Releases/terminates all held calls or sets user busy for waiting call. 
**/
void RN52_HW3::killHeldCalls(void)
{
  println("F");
  delay(50);
}

/**
    releases/terminates all active calls and accepts other held or waiting calls. 
**/
void RN52_HW3::killActiveCalls(void)
{
  println("J");
  delay(50);
}

/**
    Places all active calls on hold and accepts other held or waiting calls.  
**/
void RN52_HW3::holdActiveCalls(void)
{
  println("L");
  delay(50);
}

/**
    Adds a held call to the active conversation.  
**/
void RN52_HW3::addActiveCall(void)
{
  println("N");
  delay(50);
}

/**
    Connects the two calls and disconnects the subscriber from both calls.   
**/
void RN52_HW3::transferActiveCall(void)
{
  println("O");
  delay(50);
}

/**
    Transfers the active call between HF and AG
        0   Transfer to HF
        1   Transfer to AG
**/
void RN52_HW3::swapActiveDevice(bool action)
{
  print("X,");
  println(action);
  delay(50);
}

/**
    Activates the voice command application on the media device   
**/
void RN52_HW3::activateVCApp(void)
{
  println("P");
  delay(50);
}

/**
    Controls the hold/mute function for the current call
        0   Unmute
        1   Mute
**/
void RN52_HW3::muteCall(bool action)
{
  print("M,");
  println(action);
  delay(50);
}

/**
    Resets and clears all the previously paired devices   
**/
void RN52_HW3::wipePairedDevices(void)
{
  println("U");
  delay(50);
}

/**
    Reboot device   
**/
void RN52_HW3::rebootDevice(void)
{
  println("R,1");
  delay(50);
}

/** 
    Gets the caller ID
**/
String RN52_HW3::getCallerID()
{
  while (available() > 0)   //clear buffer
  {
    char c = read();
  }
  println("T");
  while (available() == 0);
  String iD;
  int i = 2;
  long count;
  while (i != 0) //loop two times for two lines of data
  {
    if (available() > 0)
    {
      char c = read();
      count = millis();
      iD += c;
      if (c == '\n') i--;
    }
    if ((millis() - count) > 500) i--;  //timeout
  }
  return iD;
}

/** 
    Extracts the caller's name
**/
String RN52_HW3::callerName()
{
    return "";
}

/** 
    Extracts the caller's number
**/
String RN52_HW3::callerNumber()
{
    return "";
}

/**
    Kills the selected connection.
        Bit Position    Value   Profile
        0               01      iAP
        1               02      SPP
        2               04      A2DP
        3               08      HFP 
**/
void RN52_HW3::killConnection(int connID)
{
    print("K,"); //send command
    if (connID < 16) print("0");
    println(connID, HEX);
    delay(50);
}

/**
    Queries the current connection status in the event/status register
    Byte 0:
        0       iAP wireless active connection to remote device
        1       SPP active connection to remote device
        2       A2DP active connection to remote device
        3       HFP/HSP active connection to remote device
        4       Caller ID notification event from audio gateway
        5       Track change event notification
        6-7       Reserved
    Byte 1:
        0-3     These bits indicate the connection state.:
                0       Limbo Logically off, but physically on
                1       Connectable The module is connectable, page scanning
                2       Connectable and discoverable The module is connectable and discoverable, page and inquiry scanning
                3       Connected The module is connected to an audio gateway
                4       Outgoing call established The connected audio gateway has an outgoing call in progress
                5       Incoming call established The connected audio gateway has an active call in progress and the audiois in the headset
                6       Active call The connected audio gateway has an active call in progress and the audiois in the headset
                7       Test mode The headset is in Test mode
                8       Three-way call waiting The connected audio gateway has an active call and a second call on hold
                9       Three-way call on hold The connected audio gateway has an active call and a second call on hold
                10      Three-way call multi-call The connected audio gateway has an active call and a second call on hold
                11      Incoming call on hold The connected audio gateway has an incoming call on hold
                12      Active call The connected audio gateway has an active call and the audio is in thehandset
                13      Audio streaming The headset is streaming A2DP audio
                14      Low battery The system has a low battery
        4       HFP audio volume level change from audio gateway (phone).
        5       HFP audio microphone level change from audio gateway (phone).
        6-7     Reserved
**/
int RN52_HW3::queryState(void)
{
  while (available() > 0)   //clear buffer
  {
    char c = read();
  }
  println("Q");
  delay(100);
  int state = 0;
  char c;
  while (c != '\r')
  {
    while (available() == 0);
    c = read();
    if (c >= '0' && c <= '9')
    {
      state *= 16;
      state += (c - '0');
    }
    else if (c >= 'A' && c <= 'F')
    {
      state *= 16;
      state += (c - 'A') + 10;
    }
  }
  return state;
}

/**
    Gets the HFP volume levels in audio gateway
        0   Audio volume level
        1   Mic volume level
**/
int RN52_HW3::hfpVolumeLevel(bool action) //FIX THIS*****
{
  print("Y,");
  println(action);
  delay(100);
  int level = 0;
  char c;
  while (c != '\r')
  {
    while (available() == 0);
    c = read();
    if (c >= '0' && c <= '9')
    {
      level *= 16;
      level += (c - '0');
    }
    else if (c >= 'A' && c <= 'F')
    {
      level *= 16;
      level += (c - 'A') + 10;
    }
  }
  return state;
}























