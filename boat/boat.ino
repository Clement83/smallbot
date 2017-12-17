/* Includes ------------------------------------------------------------------*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

/* Ports ---------------------------------------------------------------------*/
//#define CE_PIN 9
//#define CSN_PIN 10
#define CE_PIN 4
#define CSN_PIN 5

#define LED_ORANGE 2
#define LED_WHITE 3

#define PIN_GOUVERNAIL 8
#define PIN_MOTEUR 6

/* nRF24l01 ------------------------------------------------------------------*/
// NOTE: the "LL" at the end of the constant is "LongLong" type
const uint64_t pipe = 0xE8E8F0F0E1LL; // Define the transmit pipe
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio
/* Joystick ------------------------------------------------------------------*/
int joystick[5]; // 2 element array holding Joystick readings
/* ---------------------------------------------------------------------------*/
Servo monServo; 

int counter = 0;
void setup(){
  Serial.begin(9600);
  delay(1000);
  Serial.println("nRF24l01 Receiver Starting");
  radio.begin();
  radio.setRetries(0, 15);
  radio.setPALevel(RF24_PA_HIGH);
  radio.openReadingPipe(1,pipe);
  radio.startListening();

  monServo.attach(PIN_GOUVERNAIL);
  pinMode(PIN_MOTEUR,OUTPUT);
  pinMode(LED_ORANGE,OUTPUT);
  pinMode(LED_WHITE,OUTPUT);
  pinMode(10,OUTPUT); //on prépare le pin 3 en mode sortie
}
 

void loop(){
    
  if ( radio.available() )
  {
    // Read the data payload until we've received everything
    bool done = false;
    while (!done)
    {
    // Fetch the data payload
      done = radio.read(joystick, sizeof(joystick));
      Serial.print("X = ");
      Serial.print(joystick[0]);
      Serial.print(" Y = ");      
      Serial.println(joystick[1]);

      int dir = map(joystick[0], 0, 1024, 130, 80);
      //int acc = map(map(joystick[1], 512, 1024, 0, 512), 0,512, 0,200); //crawler radio
      int acc = map(joystick[1], 0, 100, 0, 200);
    
      monServo.write(dir);
      analogWrite(PIN_MOTEUR, acc);

      if(joystick[2] == 1) {
        analogWrite(LED_ORANGE, 200);
      } else {
        analogWrite(LED_ORANGE, 0);
      }
      
      if(joystick[3] == 1) {
        analogWrite(LED_WHITE, 200);
      } else {
        analogWrite(LED_WHITE, 0);
      }

      
      if(joystick[4] == 1) {
        buzzer();
      } else {
        digitalWrite(10, 0);
      }

      delay(50);
    }
    counter = 0;
  }
  else
  {    
    if(++counter>10) {
      Serial.println("No radio available");
      monServo.write(105);
      analogWrite(PIN_MOTEUR, 0);
      digitalWrite(10, 0);
    }else {
      delay(100);
    }
  }
}

int p=50;
void buzzer()
{
  p--; // on incémente p à chaque boucle de loop();
  if (p<10){ // teste la limite de p
    p=50; // on le remet à 10
  }
  digitalWrite(10,0); // état bas
  delayMicroseconds(p); //on attend p milli-secondes
  digitalWrite(10,1); // état haut
  delayMicroseconds(p); // on attend p millisecondes
}
