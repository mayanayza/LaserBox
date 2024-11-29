#include <ServoEasing.hpp>
ServoEasing servo;

#include <SparkFun_Alphanumeric_Display.h>
HT16K33 display;

#include <Wire.h>

#define LASER_PIN 3
#define SERVO_PIN 1
#define VAPOR_PIN 4

#define SCLPin 2
#define SDAPin 0

#define SERVO_START_SPEED 100
#define SERVO_START_POSITION 0
#define SERVO_MIN_ANGLE 100
#define SERVO_MAX_ANGLE 170

#define TEXT_SCROLL_SPEED 200
#define MESSAGE_OFF_INTERVAL 5000 

#define VAPOR_ON_INTERVAL 4000
#define VAPOR_OFF_INTERVAL 4000

const char* messages[] = {"WHAT HAPPENED?", "WHERE AM I?", "THIS ISN'T WHAT I EXPECTED", "SOMETHING ISN'T RIGHT", "HOW DO I GET OUT OF HERE?", "PLEASE HELP ME", "I DON'T UNDERSTAND", "I'M SCARED"};

void setup() {

  servo.attach(SERVO_PIN, SERVO_START_POSITION);
  servo.setSpeed(SERVO_START_SPEED);
  servo.setEasingType(EASE_CUBIC_IN_OUT);

  pinMode(LASER_PIN, OUTPUT);
  digitalWrite(LASER_PIN, HIGH);

  pinMode(VAPOR_PIN, OUTPUT);
  digitalWrite(VAPOR_PIN, HIGH);
  
  Wire.begin();

  if (display.begin() == false)
  {
    Serial.println("Device did not acknowledge! Freezing.");
    while (1);
  }
  Serial.println("Display acknowledged.");
  display.setBrightness(1);
}

int currentServoPosition;
int nextServoPosition;
int nextServoSpeed;

int randomValue;
int lastServoUpdateMs = 0;
int servoUpdateWaitMs = 1000;
int ms = 0;

bool isPrintingMessage = true;
int lastMessageEndedMs = 0;
int lastMessageUpdateMs = 0;
int messageIndex = 0;
const char* message = "";

int lastVaporEndedMs = 0;
int lastVaporStartedMs = 0;
bool isEmittingVapor = true;

void loop() {

  ms = millis();

  if (isPrintingMessage){

    display.print( message );

    if (ms - TEXT_SCROLL_SPEED > lastMessageUpdateMs) {
      
      lastMessageUpdateMs = ms;
      
      if (strlen(message) == 0){
        lastMessageEndedMs = ms;
        isPrintingMessage = false;
        messageIndex = random(0,8);
        message = messages[messageIndex];
      } else {
        message = message + 1;
      }
    }

  } 
  else if (MESSAGE_OFF_INTERVAL <  ms - lastMessageEndedMs && isEmittingVapor) {
    isPrintingMessage = true;
    lastMessageUpdateMs = ms;
    display.print( message );
  }

  if (isEmittingVapor){
    
    if (ms - lastVaporStartedMs > VAPOR_ON_INTERVAL){
      digitalWrite(VAPOR_PIN, LOW);
      isEmittingVapor = false;
      isPrintingMessage = false;
      lastVaporEndedMs = ms;
    }

  } else {

    if (VAPOR_OFF_INTERVAL < ms - lastVaporEndedMs){
      digitalWrite(VAPOR_PIN, HIGH);
      isEmittingVapor = true;
      lastVaporStartedMs = ms;
    }

  }

  if (servoUpdateWaitMs < ms - lastServoUpdateMs && !servo.isMoving()){

    currentServoPosition = servo.getCurrentAngle();

    if (isEmittingVapor) nextServoPosition = random(SERVO_MIN_ANGLE,SERVO_MAX_ANGLE);
    else {

      if ( abs(SERVO_MAX_ANGLE - currentServoPosition) > abs(SERVO_MIN_ANGLE - currentServoPosition) ) nextServoPosition = SERVO_MAX_ANGLE;
      else nextServoPosition = SERVO_MIN_ANGLE;

    }

    nextServoSpeed = random(50,150);
    servo.startEaseTo(nextServoPosition, nextServoSpeed);
    lastServoUpdateMs = ms;
    servoUpdateWaitMs = random(2000,5000);

  }
}
