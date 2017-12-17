
/* Includes ------------------------------------------------------------------*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Encoder.h>

/* Ports ---------------------------------------------------------------------*/
#define CE_PIN   9
#define CSN_PIN 10
#define ACC_POTAR A1

#define SWITCH1 A5
#define SWITCH2 A7
#define BTN_PUSH A6
/*threadhold*/
#define MIN_POTAR_VAL 48
#define MAX_POTAR_VAL 341

/* nRF24l01 ------------------------------------------------------------------*/
// NOTE: the "LL" at the end of the constant is "LongLong" type
const uint64_t pipe = 0xE8E8F0F0E1LL; // Define the transmit pipe
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio
/* Joystick ------------------------------------------------------------------*/
int joystick[5]; // 2 element array holding Joystick readings
/* ---------------------------------------------------------------------------*/
Encoder myEnc(3, 4);

int barreValue = 512;

#define NB_LED 6
int ledPins[] = {
  A3, A2, 5, 6, 7, 8
}; 

 
void setup(){
  
  Serial.begin(9600);
  delay(1000);

  radio.begin();
  radio.setRetries(0, 15);
  radio.setPALevel(RF24_PA_HIGH);
  radio.openWritingPipe(pipe);

  barreValue = 512;
  pinMode(SWITCH1, INPUT);
  pinMode(SWITCH2, INPUT);
  pinMode(BTN_PUSH, INPUT);

  for (int thisLed = 0; thisLed < NB_LED; thisLed++) {
    pinMode(ledPins[thisLed], OUTPUT);
  }
}
 
void loop(){
  updateBarre();

  joystick[0] = barreValue;// analogRead(JOYSTICK_X);
  joystick[1] = map(analogRead(ACC_POTAR), MIN_POTAR_VAL, MAX_POTAR_VAL, 0, 100);
  joystick[2] = 0; 
  joystick[3] = 0;
  joystick[4] = 0;
  //joystick[1] = analogRead(ACC_POTAR);
  
  Serial.print("Valeur lue : ");
  Serial.println(joystick[1]);
  
  updateBarrGraph();
  
  radio.write( joystick, sizeof(joystick));
  
  delay(100);
} 

void updateBarrGraph()
{
  // map the result to a range from 0 to the number of LEDs:
  int ledLevel = (map(joystick[1], 0, 100, 0, NB_LED+1));

  // loop over the LED array:
  for (int thisLed = 0; thisLed < NB_LED; thisLed++) {
    // if the array element's index is less than ledLevel,
    // turn the pin for this element on:
    if (thisLed < ledLevel) {
      digitalWrite(ledPins[thisLed], HIGH);
    }
    // turn off all pins higher than the ledLevel:
    else {
      digitalWrite(ledPins[thisLed], LOW);
    }
  }
}

void updateBarre()
{
  barreValue += 10 * getSensRotation();

  if(barreValue > 1023) {
    barreValue = 1023;
  } else if(barreValue < 0){
    barreValue = 0;
  }
}

long oldPosition = -9999;
int8_t getSensRotation()
{
    long newPosition = myEnc.read();
    if (newPosition != oldPosition) {
      
      Serial.println(newPosition);
    }

    int val = oldPosition - newPosition;
    oldPosition = newPosition;
    return val;

    if(newPosition > oldPosition){
      oldPosition = newPosition;
      return 1;
    } else if(newPosition < oldPosition){
        oldPosition = newPosition;
        return -1;
    }
    oldPosition = newPosition;
    return 0;
}  
