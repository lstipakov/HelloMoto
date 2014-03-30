#include <Wire.h>
#include <SoftwareSerial.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include "Timer.h"
#include <NewPing.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor *m1 = AFMS.getMotor(1);
Adafruit_DCMotor *m2 = AFMS.getMotor(2);

SoftwareSerial ss = SoftwareSerial(6, 5);
int pingPin = 7;



Timer t;

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}

long cm;

void sendDistance() {
  // establish variables for duration of the ping, 
  // and the distance result in inches and centimeters:
  long duration, inches, new_cm;

  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(5);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);
  
  new_cm = microsecondsToCentimeters(duration);
  
  if (new_cm != cm) {
    cm = new_cm;
    Serial.println(cm);
    ss.write(cm);
  }
}

void setup() {
    Serial.begin(9600);           // set up Serial library at 9600 bps
    Serial.println("Adafruit Motorshield v2 - DC Motor test!");
    
    AFMS.begin();  // create with the default frequency 1.6KHz
    
    ss.begin(9600);
    
    m1->setSpeed(0);
    m2->setSpeed(0);
    
    m1->run(FORWARD);
    m2->run(FORWARD);

    t.every(500, sendDistance);
}

void drive(int b) {
    if ((b >= 56) && (b < 72)) {
        Serial.println("stop");
        m1->run(RELEASE);
        m2->run(RELEASE);
    } else {
        int speed = 0;
        if (b >= 64) {
            speed = min(((b - 64) * 4), 255);
        } else {
            speed = min(((64 - b) * 4), 255);
        }
        
        m1->setSpeed(100);
        m2->setSpeed(100);
        
        if (b > 64) {
            Serial.print("drive forward ");
            Serial.println(speed);
            // forward
            m1->run(FORWARD);
            m2->run(FORWARD);
        } else {
            Serial.print("drive backward ");
            Serial.println(speed);
            // backward
            m1->run(BACKWARD);
            m2->run(BACKWARD);
        }
    }
}

void turn(int b) {
    
    int speed = 0;
    
    if ((b >= 56) && (b < 72)) {
        Serial.println("stop");
        m1->run(RELEASE);
        m2->run(RELEASE);
    } else if (b >= 64) {
        // right
        speed = (b - 64) * 2;
        Serial.print("right ");
        Serial.println(speed);
        m1->setSpeed(speed);
        m2->setSpeed(speed);
        m1->run(FORWARD);
        m2->run(BACKWARD);
    } else {
        // left
        speed = (64 - b) * 2;
        Serial.print("left ");
        Serial.println(speed);
        m1->setSpeed(speed);
        m2->setSpeed(speed);
        m2->run(FORWARD);
        m1->run(BACKWARD);
    }    
}

void loop() {
    int b;
    
    t.update();
    
    if (ss.available() > 0) {
        b = ss.read();
        
        //Serial.println(b, BIN);
        
        // turn ?
        if (b & 0x80) {
            b &= ~0x80;
            
            turn(b);
        } else {
            // drive
            drive(b);           
        }
    }
}
