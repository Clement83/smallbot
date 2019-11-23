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

#define ZERO_THREHOLD 0

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
    
    while (radio.available())
    {
    // Fetch the data payload
      radio.read(joystick, sizeof(joystick));
      Serial.print("X = ");
      Serial.print(joystick[0]);
      Serial.print(" Y = ");      
      Serial.println(joystick[1]);
      Serial.print(" Phare = ");      
      Serial.println(joystick[2]);
      Serial.print(" Giro = ");      
      Serial.println(joystick[3]);
      Serial.print(" buzzer = ");      
      Serial.println(joystick[4]);

      int dir = map(joystick[0], 0, 100, 115 + ZERO_THREHOLD, 65 + ZERO_THREHOLD);
      //int acc = map(map(joystick[1], 512, 1024, 0, 512), 0,512, 0,200); //crawler radio
      int acc = map(joystick[1], 0, 100, 0, 200);
    
      Serial.print(" real angle = ");      
      Serial.println(dir);
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
