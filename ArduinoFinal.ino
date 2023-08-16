#define DECODE_NEC  // Includes Apple and Onkyo

#include <Arduino.h>
#include "PinDefinitionsAndMore.h"  // Define macros for input and output pin etc.
#include <IRremote.hpp>
#include <Wire.h>
#include "RtcDS1307.h"  //DS1307时钟模块的库

/*
  GPIO Arrange
  D2 OUT 3W LED
  D3 OUT fan
  D4 OUT buzzer


*/

#define IR_RECEIVE_PIN 5

int fan_pin = 3;
int buzzer_pin = 4;
int ps_pin = 5; 
int led_pin = 2;
RtcDS1307<TwoWire> Rtc(Wire);// i2c接口


// notes in the melody:
int melody[] = {
  659, 659, 740, 659, 440, 830, 
  659, 659, 740, 659, 494, 440
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
float noteDurations[] = {
  2, 2, 1, 1, 1, 0.5,
  2, 2, 1, 1, 1, 0.5
};

void setup() {
  Serial.begin(9600);
  pinMode(fan_pin, OUTPUT);
  pinMode(buzzer_pin, OUTPUT);
  pinMode(led_pin, OUTPUT);
  // ps sensor
  // pinMode(ps_pin, INPUT);
  // attachInterrupt(0, ps_func, RISING);

  // DS1307

  Serial.println("Serial begin");
  Rtc.Begin();
  Serial.println("begin");
  Rtc.SetIsRunning(true);
  Serial.println("SetIsRunning");
  Rtc.SetDateTime(RtcDateTime(__DATE__, __TIME__));
  Serial.println("SetDateTime");

  // Just to know which program is running on my Arduino
  Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

  // Start the receiver and if not 3. parameter specified, take LED_BUILTIN pin from the internal boards definition as default feedback LED
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  Serial.print(F("Ready to receive IR signals of protocols: "));
  printActiveIRProtocols(&Serial);
  Serial.println(F("at pin " STR(IR_RECEIVE_PIN)));
}

void loop() {
  /*
     * Check if received data is available and if yes, try to decode it.
     * Decoded result is in the IrReceiver.decodedIRData structure.
     *
     * E.g. command is in IrReceiver.decodedIRData.command
     * address is in command is in IrReceiver.decodedIRData.address
     * and up to 32 bit raw data in IrReceiver.decodedIRData.decodedRawData
     */
  if (IrReceiver.decode()) {
    /*
         * Print a short summary of received data
         */
    IrReceiver.printIRResultShort(&Serial);
    IrReceiver.printIRSendUsage(&Serial);
    if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
      Serial.println(F("Received noise or an unknown (or not yet enabled) protocol"));
      // We have an unknown protocol here, print more info
      IrReceiver.printIRResultRawFormatted(&Serial, true);
    }
    Serial.println();

    /*
         * !!!Important!!! Enable receiving of the next value,
         * since receiving has stopped after the end of the current received data packet.
         */
    
    /*
    * Finally, check the received data and perform actions according to the received command
    */
    play_beep();
    if (IrReceiver.decodedIRData.command == 0x19) {
      // FOL-
      // open fan
      Serial.println("The fan is turning on.");
      digitalWrite(fan_pin, HIGH);
    } else if (IrReceiver.decodedIRData.command == 0xD) {
      // FOL+
      // close fan
      Serial.println("The fan is turning off.");
      digitalWrite(fan_pin, LOW);
    } else if (IrReceiver.decodedIRData.command == 0x16) {
      play_song();
    } else if (IrReceiver.decodedIRData.command == 0x7) {
      digitalWrite(led_pin, HIGH);
    } else if (IrReceiver.decodedIRData.command == 0x15) {
      digitalWrite(led_pin, LOW);
    }
    IrReceiver.resume();  // Enable receiving of the next value
  }
  Serial.print(Rtc.GetDateTime().Year());
  Serial.print("    ");
  Serial.print(Rtc.GetDateTime().Month());
  Serial.print(Rtc.GetDateTime().Second());
  Serial.print("    ");
  Serial.println(Rtc.GetDateTime().DayOfWeek());
  delay(1000);//延时1秒
}

void newtone(byte tonePin, int frequency, int duration) {
  int period = 1000000L / frequency;
  int pulse = period / 2;
  for (long i = 0; i < duration * 1000L; i += period) {
    digitalWrite(tonePin, HIGH);
    delayMicroseconds(pulse);
    digitalWrite(tonePin, LOW);
    delayMicroseconds(pulse);
  }
}

void play_song() {
  for (int thisNote = 0; thisNote < 12; thisNote++) {
    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 * 3 / 4 / noteDurations[thisNote];
    newtone(buzzer_pin, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration;
    Serial.print(melody[thisNote]);
    Serial.print("  ");
    Serial.println(noteDuration);

    delay(pauseBetweenNotes);
    // stop the tone playing:
  }
  noTone(buzzer_pin);
}

void play_beep() {
  newtone(buzzer_pin, 659, 100);
  delay(100);
  noTone(buzzer_pin);
}

void ps_func() {
  // Serial.println("people showed");
}

