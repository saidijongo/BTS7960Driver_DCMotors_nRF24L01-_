//mostor control
#include <nRF24L01.h>
#include <RF24.h>
 
#define S1F 5
#define S1B 6
#define S2F 9
#define S2B 10

RF24 radio(8, 7); // CE, CSN
const byte address[6] = "00001";
int data[3];

unsigned long lastReceiveTime = 0;
unsigned long currentTime = 0;

int xAxis = 510; // Forward/backward, 1023 - full forward, 0 - full backward
int yAxis = 510; // Left/right, 0 - full left, 1023 - full right
int lAxis; // limiter
int motorSpeed1F = 0;
int motorSpeed1B = 0;
int motorSpeed2F = 0;
int motorSpeed2B = 0;

void setup() {
  
  pinMode(S1F, OUTPUT); // PWM motor 1 Forward
  pinMode(S1B, OUTPUT); // PWM motor 1 Backward
  pinMode(S2F, OUTPUT); // PWM motor 2 Forward
  pinMode(S2B, OUTPUT); // PWM motor 2 Backward
  
 
  radio.begin();
  // radio.setChannel(81);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);

  radio.openReadingPipe(0, address);
  radio.startListening(); 
  
  Serial.begin(9600);
  }

void loop() {
     currentTime = millis();
     if ( currentTime - lastReceiveTime > 1000 ) {
     resetData();
     }
  
    if (radio.available()) { 
    radio.read( data, sizeof(data) );
    xAxis = data[0];
    yAxis = data[1];
    lAxis = data[2];
    Serial.println("xAxis:");
    Serial.println(xAxis);
    Serial.println("yAxis:");
    Serial.println(yAxis); 
    Serial.println("lAxis:");
    Serial.println(lAxis);
    lastReceiveTime = millis(); 
    }

  // OŚ X - BACKWARD < 490 (set medium = 510 +-20)
  if (xAxis < 490) {
 
    digitalWrite(S1F, LOW);   // disable forward motor 1
    digitalWrite(S2F, LOW);  // disable forward motor 2
   
    motorSpeed1B = map(xAxis, 490, 0, 0, 255); // set pwm backward motor 1 
    motorSpeed2B = map(xAxis, 490, 0, 0, 255); // set pwm backward motor 2
  }
  // OŚ X - FORWARD > 530
  else if (xAxis > 530) {
    
    digitalWrite(S1B, LOW);   // disable backward motor 1
    digitalWrite(S2B, LOW);   // disable backward motor 2
  
    motorSpeed1F = map(xAxis, 530, 1023, 0, 255);
    motorSpeed2F = map(xAxis, 530, 1023, 0, 255);
  }
  // else do nothing, set zero all PWM
  else {
    motorSpeed1F = 0;
    motorSpeed1B = 0;
    motorSpeed2F = 0;
    motorSpeed2B = 0;
    
   digitalWrite(S1B, LOW);
   digitalWrite(S2B, LOW);
   digitalWrite(S1F, LOW);
   digitalWrite(S2F, LOW);
   }
     
  // OŚ Y - TURN RIGHT, LEFT
  
  if (yAxis < 490) // turn LEFT
   { int xMapped = map(yAxis, 490, 0, 0, 255); 
  // Backward - motor 1 minus
  if (xAxis < 490) {
     motorSpeed1B = motorSpeed1B - xMapped;
     motorSpeed1F = 0;
     motorSpeed2F = 0;
     if (motorSpeed1B < 0 ) {
         motorSpeed1B = 0;}
      }
  // Forward - motor 1 minus
  if (xAxis > 530) {
    motorSpeed1F = motorSpeed1F - xMapped;
     motorSpeed1B = 0;
     motorSpeed2B = 0;
     if (motorSpeed1F < 0 ) {
         motorSpeed1F = 0;
              }
       }
   }
    
  if (yAxis > 530) // turn RIGHT
   { int xMapped = map(yAxis, 530, 1023, 0, 255);
    // Backward - motor 2 minus:
     if (xAxis < 490) {
    motorSpeed2B = motorSpeed2B - xMapped;
    motorSpeed1F = 0;
    motorSpeed2F = 0;
       if (motorSpeed2B < 0 ) {
           motorSpeed2B = 0;}
           }
    // Forward - motor 2 minus  
     if (xAxis > 530) {
    motorSpeed2F = motorSpeed2F - xMapped;
    motorSpeed1B = 0;
    motorSpeed2B = 0;
       if (motorSpeed2F < 0 ) {
           motorSpeed2F = 0; 
              }
       }
  }

  if (lAxis < 1024) // limiter
   { int lMapped = map(lAxis, 0, 1023, 100, 0); 
    motorSpeed1F = motorSpeed1F * lMapped/100;
    motorSpeed1B = motorSpeed1B * lMapped/100;
    motorSpeed2F = motorSpeed2F * lMapped/100;
    motorSpeed2B = motorSpeed2B * lMapped/100;
   }
    
    analogWrite(S1F, motorSpeed1F); 
    analogWrite(S1B, motorSpeed1B); 
    analogWrite(S2F, motorSpeed2F);
    analogWrite(S2B, motorSpeed2B);  
   
    Serial.println("motor A:");
    Serial.println(motorSpeed1F);
    Serial.println(motorSpeed1B);
    Serial.println("motor B:");
    Serial.println(motorSpeed2F);
    Serial.println(motorSpeed2B);
}

void resetData() {
  xAxis = 510;
  yAxis = 510;
   Serial.println("resetData");
}
 
