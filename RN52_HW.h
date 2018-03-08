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

#ifndef RN52_HW_h
#define RN52_HW_h

#include "kinetis.h"

// Uncomment to enable 9 bit formats.  These are default disabled to save memory.
//#define SERIAL_9BIT_SUPPORT
//
// On Windows & Linux, this file is in Arduino's hardware/teensy/avr/cores/teensy3
//   folder.  The Windows installer puts Arduino in C:\Program Files (x86)\Arduino
// On Macintosh, you must control-click Arduino and select "Show Package Contents", then
//   look in Contents/Java/hardware/teensy/avr/cores/teensy3 to find this file.
//
// Teensy 3.x boards support 9 bit mode on all their serial ports
// Teensy LC only supports 9 bit mode on Serial1.  Serial2 & Serial3 can't use 9 bits.


#define SERIAL_7E1 0x02
#define SERIAL_7O1 0x03
#define SERIAL_8N1 0x00
#define SERIAL_8E1 0x06
#define SERIAL_8O1 0x07
#define SERIAL_7E1_RXINV 0x12
#define SERIAL_7O1_RXINV 0x13
#define SERIAL_8N1_RXINV 0x10
#define SERIAL_8E1_RXINV 0x16
#define SERIAL_8O1_RXINV 0x17
#define SERIAL_7E1_TXINV 0x22
#define SERIAL_7O1_TXINV 0x23
#define SERIAL_8N1_TXINV 0x20
#define SERIAL_8E1_TXINV 0x26
#define SERIAL_8O1_TXINV 0x27
#define SERIAL_7E1_RXINV_TXINV 0x32
#define SERIAL_7O1_RXINV_TXINV 0x33
#define SERIAL_8N1_RXINV_TXINV 0x30
#define SERIAL_8E1_RXINV_TXINV 0x36
#define SERIAL_8O1_RXINV_TXINV 0x37
#ifdef SERIAL_9BIT_SUPPORT
#define SERIAL_9N1 0x84
#define SERIAL_9E1 0x8E
#define SERIAL_9O1 0x8F
#define SERIAL_9N1_RXINV 0x94
#define SERIAL_9E1_RXINV 0x9E
#define SERIAL_9O1_RXINV 0x9F
#define SERIAL_9N1_TXINV 0xA4
#define SERIAL_9E1_TXINV 0xAE
#define SERIAL_9O1_TXINV 0xAF
#define SERIAL_9N1_RXINV_TXINV 0xB4
#define SERIAL_9E1_RXINV_TXINV 0xBE
#define SERIAL_9O1_RXINV_TXINV 0xBF
#endif
// Teensy LC and 3.5 and 3.6 Uarts have 1/2 bit stop setting
#if defined(__MK64FX512__) || defined(__MK66FX1M0__) || defined(KINETISL)
#define SERIAL_2STOP_BITS 0x100
#define SERIAL_8E2 (SERIAL_8E1 | SERIAL_2STOP_BITS)
#define SERIAL_8O2 (SERIAL_8O1 | SERIAL_2STOP_BITS)
#define SERIAL_8E2_RXINV (SERIAL_8E1_RXINV | SERIAL_2STOP_BITS)
#define SERIAL_8O2_RXINV (SERIAL_8O1_RXINV | SERIAL_2STOP_BITS)
#define SERIAL_8E2_TXINV (SERIAL_8E1_TXINV | SERIAL_2STOP_BITS)
#define SERIAL_8O2_TXINV (SERIAL_8O1_TXINV | SERIAL_2STOP_BITS)
#define SERIAL_8E2_RXINV_TXINV (SERIAL_8E1_RXINV_TXINV | SERIAL_2STOP_BITS)
#define SERIAL_8O2_RXINV_TXINV (SERIAL_8O1_RXINV_TXINV | SERIAL_2STOP_BITS)
#define SERIAL_8N2 (SERIAL_8N1 | SERIAL_2STOP_BITS)
#define SERIAL_8N2_RXINV (SERIAL_8N1_RXINV | SERIAL_2STOP_BITS)
#define SERIAL_8N2_TXINV (SERIAL_8N1_TXINV | SERIAL_2STOP_BITS)
#define SERIAL_8N2_RXINV_TXINV (SERIAL_8N1_RXINV_TXINV | SERIAL_2STOP_BITS)
#else
// for Teensy 3.0-3.2 we can fake 2 stop bits by using 9 bit mode
#define SERIAL_8N2 0x04
#define SERIAL_8N2_RXINV 0x14
#define SERIAL_8N2_TXINV 0x24
#define SERIAL_8N2_RXINV_TXINV 0x34
#endif
// bit0: parity, 0=even, 1=odd
// bit1: parity, 0=disable, 1=enable
// bit2: mode, 1=9bit, 0=8bit
// bit3: mode10: 1=10bit, 0=8bit
// bit4: rxinv, 0=normal, 1=inverted
// bit5: txinv, 0=normal, 1=inverted
// bit6: unused
// bit7: actual data goes into 9th bit


#if defined(KINETISK)
#define BAUD2DIV(baud)  (((F_CPU * 2) + ((baud) >> 1)) / (baud))
#define BAUD2DIV2(baud) (((F_CPU * 2) + ((baud) >> 1)) / (baud))
#define BAUD2DIV3(baud) (((F_BUS * 2) + ((baud) >> 1)) / (baud))
#elif defined(KINETISL)

#if F_CPU <= 2000000
#define BAUD2DIV(baud)  (((F_PLL / 16 ) + ((baud) >> 1)) / (baud))
#elif F_CPU <= 16000000
#define BAUD2DIV(baud)  (((F_PLL / (F_PLL / 1000000)) + ((baud) >> 1)) / (baud))
#else
#define BAUD2DIV(baud)  (((F_PLL / 2 / 16) + ((baud) >> 1)) / (baud))
#endif

#define BAUD2DIV2(baud) (((F_BUS / 16) + ((baud) >> 1)) / (baud))
#define BAUD2DIV3(baud) (((F_BUS / 16) + ((baud) >> 1)) / (baud))
#endif


// C language implementation
//
#ifdef __cplusplus
extern "C" {
#endif

void serial3_begin(uint32_t divisor);
void serial3_format(uint32_t format);
void serial3_end(void);
void serial3_set_transmit_pin(uint8_t pin);
void serial3_set_rx(uint8_t pin);
void serial3_set_tx(uint8_t pin, uint8_t opendrain);
int serial3_set_rts(uint8_t pin);
int serial3_set_cts(uint8_t pin);
void serial3_putchar(uint32_t c);
void serial3_write(const void *buf, unsigned int count);
void serial3_flush(void);
int serial3_write_buffer_free(void);
int serial3_available(void);
int serial3_getchar(void);
int serial3_peek(void);
void serial3_clear(void);

#ifdef __cplusplus
}
#endif


// C++ interface
//
#ifdef __cplusplus
#include "Stream.h"
#include "WString.h"
class RN52_HW : public Stream
{
private:
    static short IOMask;
    static short IOProtect;
    static short IOStateMask;
    static short IOStateProtect;
    static short IO;
    static short IOState;   
public:
    constexpr RN52_HW() {}
    virtual void begin(uint32_t baud) { serial_begin(BAUD2DIV(baud)); }
    virtual void begin(uint32_t baud, uint32_t format) {
                      serial_begin(BAUD2DIV(baud));
                      serial_format(format); }
    virtual void end(void)      { serial_end(); }
    virtual void transmitterEnable(uint8_t pin) { serial_set_transmit_pin(pin); }
    virtual void setRX(uint8_t pin) { serial_set_rx(pin); }
    virtual void setTX(uint8_t pin, bool opendrain=false) { serial_set_tx(pin, opendrain); }
    virtual bool attachRts(uint8_t pin) { return serial_set_rts(pin); }
    virtual bool attachCts(uint8_t pin) { return serial_set_cts(pin); }
    virtual int available(void)     { return serial_available(); }
    virtual int peek(void)          { return serial_peek(); }
    virtual int read(void)          { return serial_getchar(); }
    virtual void flush(void)        { serial_flush(); }
    virtual void clear(void)    { serial_clear(); }
    virtual int availableForWrite(void) { return serial_write_buffer_free(); }
    using Print::write;
    virtual size_t write(uint8_t c) { serial_putchar(c); return 1; }
    virtual size_t write(unsigned long n)   { return write((uint8_t)n); }
    virtual size_t write(long n)            { return write((uint8_t)n); }
    virtual size_t write(unsigned int n)    { return write((uint8_t)n); }
    virtual size_t write(int n)             { return write((uint8_t)n); }
    virtual size_t write(const uint8_t *buffer, size_t size)
                    { serial_write(buffer, size); return size; }
        virtual size_t write(const char *str)   { size_t len = strlen(str);
                      serial_write((const uint8_t *)str, len);
                      return len; }
    virtual size_t write9bit(uint32_t c)    { serial_putchar(c); return 1; }
    operator bool()         { return true; }
};


class RN52_HW3 : public RN52_HW
{
private:
    static short IOMask;
    static short IOProtect;
    static short IOStateMask;
    static short IOStateProtect;
    static short IO;
    static short IOState;
public:
    constexpr RN52_HW3() {}
    virtual void begin(uint32_t baud) { serial3_begin(BAUD2DIV3(baud)); }
    virtual void begin(uint32_t baud, uint32_t format) {
                      serial3_begin(BAUD2DIV3(baud));
                      serial3_format(format); }
    virtual void end(void)          { serial3_end(); }
    virtual void transmitterEnable(uint8_t pin) { serial3_set_transmit_pin(pin); }
    virtual void setRX(uint8_t pin) { serial3_set_rx(pin); }
    virtual void setTX(uint8_t pin, bool opendrain=false) { serial3_set_tx(pin, opendrain); }
    virtual bool attachRts(uint8_t pin) { return serial3_set_rts(pin); }
    virtual bool attachCts(uint8_t pin) { return serial3_set_cts(pin); }
    virtual int available(void)     { return serial3_available(); }
    virtual int peek(void)          { return serial3_peek(); }
    virtual int read(void)          { return serial3_getchar(); }
    virtual void flush(void)        { serial3_flush(); }
    virtual void clear(void)    { serial3_clear(); }
    virtual int availableForWrite(void) { return serial3_write_buffer_free(); }
    using Print::write;
    virtual size_t write(uint8_t c) { serial3_putchar(c); return 1; }
    virtual size_t write(unsigned long n)   { return write((uint8_t)n); }
    virtual size_t write(long n)            { return write((uint8_t)n); }
    virtual size_t write(unsigned int n)    { return write((uint8_t)n); }
    virtual size_t write(int n)             { return write((uint8_t)n); }
    virtual size_t write(const uint8_t *buffer, size_t size)
                    { serial3_write(buffer, size); return size; }
        virtual size_t write(const char *str)   { size_t len = strlen(str);
                      serial3_write((const uint8_t *)str, len);
                      return len; }
    virtual size_t write9bit(uint32_t c)    { serial3_putchar(c); return 1; }
    operator bool()         { return true; }
    static inline void handle_interrupt() __attribute__((__always_inline__));
    bool GPIOPinMode(int pin, bool state);
    void GPIODigitalWrite(int pin, bool state);
    bool GPIODigitalRead(int pin);
    void setDiscoverability(bool discoverable);
    void toggleEcho();
    void name(String nom, bool normalized);
    String name(void);
    void factoryReset();
    int idlePowerDownTime(void);
    void idlePowerDownTime(int timer);
    void reboot();
    void call(String number);
    void endCall();
    void playPause();
    void nextTrack();
    void prevTrack();
    String getMetaData();
    String trackTitle();
    String album();
    String artist();
    String genre();
    int trackNumber();
    int totalCount();
    int trackDuration();
    short getExtFeatures();
    void setExtFeatures(bool state, int bit);
    void setExtFeatures(short settings);
    bool AVRCPButtons();
    void AVRCPButtons(bool state);
    bool powerUpReconnect();
    void powerUpReconnect(bool state);
    bool startUpDiscoverable();
    void startUpDiscoverable(bool state);
    bool rebootOnDisconnect();
    void rebootOnDisconnect(bool state);
    bool volumeToneMute();
    void volumeToneMute(bool state);
    bool systemTonesDisabled();
    void systemTonesDisabled(bool state);
    bool powerDownAfterPairingTimeout();
    void powerDownAfterPairingTimeout(bool state);
    bool resetAfterPowerDown();
    void resetAfterPowerDown(bool state);
    bool reconnectAfterPanic();
    void reconnectAfterPanic(bool state);
    bool trackChangeEvent();
    void trackChangeEvent(bool state);
    bool tonesAtFixedVolume();
    void tonesAtFixedVolume(bool state);
    bool autoAcceptPasskey();
    void autoAcceptPasskey(bool state);
    bool codecIndicators();
    void codecIndicators(bool state);
    bool voiceCommandButton();
    void voiceCommandButton(bool state);
    bool latchEventIndicator();
    void latchEventIndicator(bool state);
    int volumeOnStartup();
    void volumeOnStartup(int vol);
    void volumeUp();
    void volumeDown();
    short getAudioRouting();
    int sampleWidth();
    void sampleWidth(int width);
    int sampleRate();
    void sampleRate(int rate);
    int A2DPRoute();
    void A2DPRoute(int route);
    //New AT commands
    void authentication(int auth);
    int authentication(void);
    void classOfDevice(int cod);
    int classOfDevice(void);
    void discoveryMask(int mask);
    int discoveryMask(void);
    void connectionMask(int mask);
    int connectionMask(void);
    void microphoneLevel(int levels);
    int microphoneLevel(void);
    void pincode(String pinCode);
    String pincode(void);
    void toneGain(int gain);
    int toneGain(void);
    void connDelay(int cDelay);
    int connDelay(void);
    int pairingTimeout(void);
    void pairingTimeout(int pTimeout);
    String getBatteryLevel();
    void uartBaud(int rate);
    int uartBaud(void);
    String firmwareV(void);
    //H - Just read the AT guide.. http://ww1.microchip.com/downloads/en/DeviceDoc/50002154A.pdf
    void authAcceptReject(bool action);
    void enterDFU(void);
    void redailLast(void);
    void reconnectLast(void);
    void acceptCall(void);
    void killCall(void);
    void killHeldCalls(void);
    void killActiveCalls(void);
    void holdActiveCalls(void);
    void addActiveCall(void);
    void transferActiveCall(void);
    void swapActiveDevice(bool action); 
    void activateVCApp(void);
    void killConnection(int connID);
    void muteCall(bool action); 
    int queryState(void);
    int hfpVolumeLevel(bool action);
    String getCallerID(void);
    String callerName(void);
    String callerNumber(void);
    void rebootDevice(void); 
    void wipePairedDevices(void);
};
extern HardwareSerial3 Serial3;
extern void serialEvent3(void);


#endif
#endif
