#include <RN52_HardwareSerial.h>
//TODO: add tests for the calling functions
void rebooter(void){  //run this after using a set command
    Serial.println("__Rebooting__\n");
    RN52_Serial3.reboot();
    digitalWrite(PIN_A0, HIGH);
    delay(1000);
    digitalWrite(PIN_A0, LOW); //drive low to init command mode
    while (RN52_Serial3.available() == 0); //wait for ACK (AOK\r\n or AOK\n\r, I forget which)
    char c = RN52_Serial3.read();
    if (c == 'C') {
       delay(100);
       RN52_Serial3.flush();
       Serial.println("Successfully entered command mode");
    }
    delay(1000);
    while(RN52_Serial3.available() != 0){
        c = RN52_Serial3.read();
    }
}
void setup() {
    char c;
    //RN52_Serial3 object is instantiated globally in library.
    pinMode(PIN_A0, OUTPUT);
    digitalWrite(PIN_A0, HIGH);
    Serial.begin(115200);
    while (!Serial){}
    Serial.println("Ready!");
    RN52_Serial3.begin(115200);
    delay(1000);
    Serial.println("RN52:");
    digitalWrite(PIN_A0, LOW);  //drive low to init command mode
    while (RN52_Serial3.available() == 0);  //wait for ACK (AOK\r\n or AOK\n\r, I forget which)
    Serial.println("recieved something");
    c = RN52_Serial3.read();
    Serial.print(c);
    if (c == 'C') {
       delay(100);
       RN52_Serial3.flush();
       Serial.println("Successfully entered command mode");
    }
    //testing GPIO read and write
    int i = 0;
    for (i=0;i<16;i++){
        Serial.print("Initial GPIO");
        Serial.print(i);
        Serial.print("pin status: ");
        Serial.println(RN52_Serial3.GPIODigitalRead(i));
        RN52_Serial3.GPIODigitalWrite(i, 0);
        Serial.print("Set to zero GPIO");
        Serial.print(i);
        Serial.print("pin status: ");
        Serial.println(RN52_Serial3.GPIODigitalRead(i));
        RN52_Serial3.GPIODigitalWrite(i, 1);
        Serial.print("Set to one GPIO");
        Serial.print(i);
        Serial.print("pin status: ");
        Serial.println(RN52_Serial3.GPIODigitalRead(i));
        }
        delay(1000);
    //testing gpio mode change
    for (i=0;i<16;i++){
        if(RN52_Serial3.GPIOPinMode(i, 1)){
          Serial.print("GPIO");
          Serial.print(i);
          Serial.print(" successfully set to output (if i=5,10,11,12,1)\n");
        }
    }
    delay(1000);
    //testing discoverability
    RN52_Serial3.setDiscoverability(0);
    while (Serial.available() > 0){ //clear read buffer
          c = Serial.read();
    }
    Serial.print("Please check if module is discoverable\n");
    while(Serial.available() == 0)
    RN52_Serial3.setDiscoverability(1);
    Serial.print("Please check if module is discoverable\n");
    while(Serial.available() == 0)
    while (Serial.available() > 0){ //clear read buffer
          c = Serial.read();
    }
    delay(1000);
    //testing toggle echo, factory reset, reboot and naming
    RN52_Serial3.toggleEcho();
    delay(1000);
    RN52_Serial3.toggleEcho();
    Serial.print("The current name of the RN52 is: ");
    Serial.println(RN52_Serial3.name());
    RN52_Serial3.name("testname", 0); //doesnt work
    delay(1000);
    rebooter();
    Serial.print("The current name of the RN52 is: ");
    Serial.println(RN52_Serial3.name());
    RN52_Serial3.name("secondtestname", 1);
    rebooter();
    Serial.print("The current name of the RN52 is: ");
    Serial.println(RN52_Serial3.name());
    Serial.print("Factory resetting and rebooting...");
    RN52_Serial3.factoryReset();
    rebooter();
    Serial.print("The current name of the RN52 is: ");
    Serial.println(RN52_Serial3.name());
    Serial.print("The idle power down time of the RN52 is: ");
    Serial.println(RN52_Serial3.idlePowerDownTime());
    RN52_Serial3.idlePowerDownTime(1);
    rebooter();
    Serial.print("The idle power down time of the RN52 is: ");
    Serial.println(RN52_Serial3.idlePowerDownTime());
    RN52_Serial3.idlePowerDownTime(0);
    rebooter();
    Serial.print("The idle power down time of the RN52 is: ");
    Serial.println(RN52_Serial3.idlePowerDownTime());
    //testing audio functions
    while(Serial.available() != 0){
        c = Serial.read();
    }
    Serial.print("Please connect to the module and begin to play some music\n");
    while(Serial.available() == 0)
    while (Serial.available() > 0) //clear read buffer
        {
          c = Serial.read();
        }
    RN52_Serial3.println("S%,0001");
    Serial.println("Doing AVRCP stuff");
    RN52_Serial3.playPause();
    delay(2000);
    RN52_Serial3.playPause();
    delay(2000);
    RN52_Serial3.nextTrack();
    delay(2000);
    RN52_Serial3.prevTrack();
    delay(100);
    RN52_Serial3.getMetaData();
    Serial.println(RN52_Serial3.trackTitle());
    Serial.println(RN52_Serial3.album());
    Serial.println(RN52_Serial3.artist());
    Serial.println(RN52_Serial3.genre());
    Serial.println(RN52_Serial3.trackNumber());
    Serial.println(RN52_Serial3.trackDuration());
    Serial.println(RN52_Serial3.totalCount());
    RN52_Serial3.volumeUp();
    RN52_Serial3.volumeUp();
    RN52_Serial3.volumeUp();
    delay(3000);
    RN52_Serial3.volumeDown();
    RN52_Serial3.volumeDown();
    RN52_Serial3.volumeDown();
    RN52_Serial3.volumeDown();
    RN52_Serial3.volumeDown();
    delay(1000);

    //testing ext. features
    Serial.print("The ext features currently active are: ");
    Serial.println(RN52_Serial3.getExtFeatures(),BIN);
    for (i=0;i<16;i++){
        RN52_Serial3.setExtFeatures(0,i);
    }
    rebooter();
    Serial.print("The ext features currently active are: ");
    Serial.println(RN52_Serial3.getExtFeatures(),BIN);
    for (i=0;i<16;i++){
        RN52_Serial3.setExtFeatures(1,i);
    }
    rebooter();
    Serial.print("The ext features currently active are: ");
    Serial.println(RN52_Serial3.getExtFeatures(),BIN);
    Serial.print("The volume on startup currently is: ");
    Serial.println(RN52_Serial3.volumeOnStartup());
    rebooter();
    RN52_Serial3.volumeOnStartup(8);
    Serial.print("The volume on startup currently is: ");
    Serial.println(RN52_Serial3.volumeOnStartup());
    Serial.print("The routing currently is: ");
    Serial.println(RN52_Serial3.getAudioRouting());
    RN52_Serial3.sampleWidth(1);
    RN52_Serial3.sampleRate(2);
    RN52_Serial3.A2DPRoute(1);
    rebooter();
    Serial.print("The routing currently is: ");
    Serial.println(RN52_Serial3.getAudioRouting());
    Serial.print("The auth currently is: ");
    Serial.println(RN52_Serial3.authentication());
    RN52_Serial3.authentication(4);
    rebooter();
    Serial.print("The auth currently is: ");
    Serial.println(RN52_Serial3.authentication());
    delay(50);
    Serial.print("The CoD currently is: ");
    Serial.println(RN52_Serial3.classOfDevice());
    delay(50);
    Serial.print("The discovery mask currently is: ");
    Serial.println(RN52_Serial3.discoveryMask());
    delay(50);
    Serial.print("The connection mask currently is: ");
    Serial.println(RN52_Serial3.connectionMask());
    delay(50);
    Serial.print("The the mic levels are: ");
    Serial.println(RN52_Serial3.microphoneLevel());
    delay(50);
    Serial.print("The the current pincode is: ");
    Serial.println(RN52_Serial3.pincode());
    delay(50);
    Serial.print("The the current toneGain is: ");
    Serial.println(RN52_Serial3.toneGain());
    delay(50);
    Serial.print("The the current uartBaud is: ");
    Serial.println(RN52_Serial3.uartBaud());
    delay(50);
    Serial.print("The the current connDelay is: ");
    Serial.println(RN52_Serial3.connDelay());
    delay(50);
    Serial.print("The the current pairingTimeout is: ");
    Serial.println(RN52_Serial3.pairingTimeout());
    delay(50);
    Serial.print("The the current BatteryLevel is: ");
    Serial.println(RN52_Serial3.getBatteryLevel());
    delay(50);
    Serial.print("The the current firmwareV is: ");
    Serial.println(RN52_Serial3.firmwareV());
    delay(50);
    RN52_Serial3.wipePairedDevices();
    Serial.print("Factory resetting and rebooting...");
    RN52_Serial3.factoryReset();
    rebooter();
    while(Serial.available() != 0){
        c = Serial.read();
    }
    Serial.print("Please connect to the module and begin to play some music\n");
    while(Serial.available() == 0)
    while (Serial.available() > 0) //clear read buffer
        {
          c = Serial.read();
        }
    RN52_Serial3.killConnection(04);
    Serial.print("The the event register state is: ");
    Serial.println(RN52_Serial3.queryState());
    
    
    //finally reset and reboot
    Serial.println("Factory resetting and rebooting...");
    RN52_Serial3.factoryReset();
    rebooter();
    Serial.println("Done.");
}

void loop() {
  //Do nothing
}
