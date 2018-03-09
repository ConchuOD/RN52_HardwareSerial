#include <RN52_HardwareSerial.h>
//TODO: add tests for the calling functions
void rebooter(void){  //run this after using a set command
    Serial.println("__Rebooting__\n");
    RN52_Serial.reboot();
    digitalWrite(PIN_A0, HIGH);
    delay(1000);
    digitalWrite(PIN_A0, LOW); //drive low to init command mode
    while (RN52_Serial.available() == 0); //wait for ACK (AOK\r\n or AOK\n\r, I forget which)
    char c = RN52_Serial.read();
    if (c == 'A') {
       delay(100);
       RN52_Serial.flush();
       Serial.println("Successfully entered command mode");
    }
}
void setup() {
    char c;
    //RN52_Serial object is instantiated globally in library.
    pinMode(PIN_A0, OUTPUT);
    digitalWrite(PIN_A0, HIGH);
    RN52_Serial.begin(115200);
    Serial.begin(9600);
    while (!Serial){}
    digitalWrite(PIN_A0, LOW);  //drive low to init command mode
    while (RN52_Serial.available() == 0);  //wait for ACK (AOK\r\n or AOK\n\r, I forget which)
    c = RN52_Serial.read();
    if (c == 'A') {
       delay(100);
       RN52_Serial.flush();
       Serial.println("Successfully entered command mode");
    }
    //testing GPIO read and write
    int i = 0;
    for (i=0;i<16;i++){
        Serial.print("Initial GPIO");
        Serial.print(i);
        Serial.print("pin status: ");
        Serial.println(RN52_Serial.GPIODigitalRead(i));
        RN52_Serial.GPIODigitalWrite(i, 0);
        Serial.print("Set to zero GPIO");
        Serial.print(i);
        Serial.print("pin status: ");
        Serial.println(RN52_Serial.GPIODigitalRead(i));
        RN52_Serial.GPIODigitalWrite(i, 1);
        Serial.print("Set to one GPIO");
        Serial.print(i);
        Serial.print("pin status: ");
        Serial.println(RN52_Serial.GPIODigitalRead(i));
        }
    //testing gpio mode change
    for (i=0;i<16;i++){
        if(RN52_Serial.GPIOPinMode(i, 1)){
          Serial.print("GPIO");
          Serial.print(i);
          Serial.print(" successfully set to output (if i=5,10,11,12,1)\n");
        }
    }
    //testing discoverability
    RN52_Serial.setDiscoverability(0);
    Serial.print("Please check if module is discoverable\n");
    while(Serial.available() == 0)
    RN52_Serial.setDiscoverability(1);
    Serial.print("Please check if module is discoverable\n");
    while(Serial.available() == 0)
    while (Serial.available() > 0) //clear read buffer
        {
          c = Serial.read();
        }
    //testing toggle echo, factory reset, reboot and naming
    RN52_Serial.toggleEcho();
    Serial.print("The current name of the RN52 is: ");
    Serial.println(RN52_Serial.name());
    RN52_Serial.toggleEcho();
    RN52_Serial.name("testname", 0);
    rebooter();
    Serial.print("The current name of the RN52 is: ");
    Serial.println(RN52_Serial.name());
    RN52_Serial.name("secondtestname", 1);
    rebooter();
    Serial.print("The current name of the RN52 is: ");
    Serial.println(RN52_Serial.name());
    Serial.print("Factory resetting and rebooting...");
    RN52_Serial.factoryReset();
    rebooter();
    Serial.print("The current name of the RN52 is: ");
    Serial.println(RN52_Serial.name());
    Serial.print("The idle power down time of the RN52 is: ");
    Serial.print(RN52_Serial.idlePowerDownTime());
    RN52_Serial.idlePowerDownTime(1);
    rebooter();
    Serial.print("The idle power down time of the RN52 is: ");
    Serial.println(RN52_Serial.idlePowerDownTime());
    RN52_Serial.idlePowerDownTime(0);
    rebooter();
    Serial.print("The idle power down time of the RN52 is: ");
    Serial.println(RN52_Serial.idlePowerDownTime());
    //testing audio functions
    Serial.print("Please connect to the module and begin to play some music\n");
    while(Serial.available() == 0)
    while (Serial.available() > 0) //clear read buffer
        {
          c = Serial.read();
        }
    RN52_Serial.playPause();
    delay(2000);
    RN52_Serial.playPause();
    delay(2000);
    RN52_Serial.nextTrack();
    delay(2000);
    RN52_Serial.prevTrack();
    Serial.println(RN52_Serial.getMetaData());
    Serial.println(RN52_Serial.trackTitle());
    Serial.println(RN52_Serial.album());
    Serial.println(RN52_Serial.artist());
    Serial.println(RN52_Serial.genre());
    Serial.println(RN52_Serial.trackNumber());
    Serial.println(RN52_Serial.trackDuration());
    Serial.println(RN52_Serial.totalCount());
    RN52_Serial.volumeUp();
    RN52_Serial.volumeUp();
    RN52_Serial.volumeUp();
    delay(3000);
    RN52_Serial.volumeDown();
    RN52_Serial.volumeDown();
    RN52_Serial.volumeDown();
    RN52_Serial.volumeDown();
    RN52_Serial.volumeDown();
    delay(000);

    //testing ext. features
    Serial.print("The ext features currently active are: ");
    Serial.println(RN52_Serial.getExtFeatures(),BIN);
    for (i=0;i<16;i++){
        RN52_Serial.setExtFeatures(0,i);
    }
    rebooter();
    Serial.print("The ext features currently active are: ");
    Serial.println(RN52_Serial.getExtFeatures(),BIN);
    for (i=0;i<16;i++){
        RN52_Serial.setExtFeatures(1,i);
    }
    rebooter();
    Serial.print("The ext features currently active are: ");
    Serial.println(RN52_Serial.getExtFeatures(),BIN);
    Serial.print("The volume on startup currently is: ");
    Serial.println(RN52_Serial.volumeOnStartup());
    rebooter();
    RN52_Serial.volumeOnStartup(8);
    Serial.print("The volume on startup currently is: ");
    Serial.println(RN52_Serial.volumeOnStartup());
    Serial.print("The routing currently is: ");
    Serial.println(RN52_Serial.getAudioRouting());
    RN52_Serial.sampleWidth(1);
    RN52_Serial.sampleRate(2);
    RN52_Serial.A2DPRoute(1);
    rebooter();
    Serial.print("The routing currently is: ");
    Serial.println(RN52_Serial.getAudioRouting());
    Serial.print("The auth currently is: ");
    Serial.println(RN52_Serial.authentication());
    RN52_Serial.authentication(4);
    rebooter();
    Serial.print("The auth currently is: ");
    Serial.println(RN52_Serial.authentication());
    Serial.print("The CoD currently is: ");
    Serial.println(RN52_Serial.classOfDevice());
    Serial.print("The discovery mask currently is: ");
    Serial.println(RN52_Serial.discoveryMask());
    Serial.print("The connection mask currently is: ");
    Serial.println(RN52_Serial.connectionMask());
    Serial.print("The the mic levels are: ");
    Serial.println(RN52_Serial.microphoneLevel());
    Serial.print("The the current pincode is: ");
    Serial.println(RN52_Serial.pincode());
    Serial.print("The the current toneGain is: ");
    Serial.println(RN52_Serial.toneGain());
    Serial.print("The the current uartBaud is: ");
    Serial.println(RN52_Serial.uartBaud());
    Serial.print("The the current connDelay is: ");
    Serial.println(RN52_Serial.connDelay());
    Serial.print("The the current pairingTimeout is: ");
    Serial.println(RN52_Serial.pairingTimeout());
    Serial.print("The the current BatteryLevel is: ");
    Serial.println(RN52_Serial.getBatteryLevel());
    Serial.print("The the current firmwareV is: ");
    Serial.println(RN52_Serial.firmwareV());
    RN52_Serial.wipePairedDevices();
    Serial.print("Factory resetting and rebooting...");
    RN52_Serial.factoryReset();
    rebooter();
    Serial.print("Please connect to the module and begin to play some music\n");
    while(Serial.available() == 0)
    while (Serial.available() > 0) //clear read buffer
        {
          c = Serial.read();
        }
    RN52_Serial.killConnection(04);
    Serial.print("The the event register state is: ");
    Serial.println(RN52_Serial.queryState());
    
    
    //finally reset and reboot
    Serial.print("Factory resetting and rebooting...");
    RN52_Serial.factoryReset();
    rebooter();
}

void loop() {
  //Do nothing
}
