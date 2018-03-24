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
    Serial.begin(230400);
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
    Serial.print("Please connect to the module and begin to play some music\n");
    while(Serial.available() == 0)
    while(Serial.available() > 0){
          c = Serial.read();
    }
    Serial.println(RN52_Serial3.getMetaData());
        
    //finally reset and reboot
    Serial.println("Factory resetting and rebooting...");
    RN52_Serial3.factoryReset();
    rebooter();
    Serial.println("Done.");
}

void loop() {
  //Do nothing
}
