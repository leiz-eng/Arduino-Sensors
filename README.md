"# Arduino-Sensors" 

This repo is about using infrared sensor to control some electronic components which includes 3W LED, buzzer, DC generator and so on. I will try add more into it.
***
Hardwares:
1. Arduino
2. 3W LED
3. buzzer
4. DC generator
5. Servo
6. Ultrasonic
***
Software libs:
1. IRremote
2. DS1307
3. Protothreads  
this lib is used for miltitask running because you cannot put a delay function into loop() function, it will influence other features
4. Ultrasonic SR04

***
GPIO Arrange  
D2 OUT 3W LED  
D3 OUT fan  
D4 OUT buzzer  
D5 IN  IR RECEIVE pin  
D7 & 8 Ultrasonic sensor  
D9 OUT Servo  
***
