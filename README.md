This is an alpha version of a hardware serial based library for the RN52.      
It is intended for use with a Teensy as the only hardware serial on (most) arduinos is the one shared with USB, plus due to being based on Teensy's hardware serial will not compile for a non kinetis board.                       
For now this code is untested with an actual device as I am waiting on a breakout board for my RN52, but it at least compiles.      
Once the post office locates my breakout boards I will test the functions in this library.                           
Currently there is only code present for using Serial3 on the Teensy 3.X, however once verified the code will be extended to all ?6? serial ports on a Teensy3.           
