#include <ServoEasing.hpp>
ServoEasing SE;
#include <SparkFun_Alphanumeric_Display.h>
HT16K33 DISP;
#include <Wire.h>

#define LASER_PIN 3
#define SERVO_PIN 1
#define VAPOR_PIN 4

//#define SCLPin 2
//#define SDAPin 0

#define SERVO_START_SPEED 100
#define SERVO_START_POSITION 0
#define SERVO_MIN_ANGLE 100
#define SERVO_MAX_ANGLE 170

#define TEXT_SCROLL_SPEED 200
#define MESSAGE_OFF_INTERVAL 5000 

#define VAPOR_ON_INTERVAL 4000
#define VAPOR_OFF_INTERVAL 4000

#define MESSAGE_COUNT 8
const char* messages[MESSAGE_COUNT] = {"WHAT HAPPENED?", "WHERE AM I?", "THIS ISN'T WHAT I EXPECTED", "SOMETHING ISN'T RIGHT", "HOW DO I GET OUT OF HERE?", "PLEASE HELP ME", "I DON'T UNDERSTAND", "I'M SCARED"};

struct Display {
  private:
    unsigned long lastMessageEndedMs = 0;
    unsigned long lastMessageUpdatedMs = 0;
    const char* message = "";

  public:
    bool hasMessage = false;
    void init(){
      Wire.begin();
      if (DISP.begin() == false) {
        Serial.println("Device did not acknowledge! Freezing.");
        while (1);
      }
      Serial.println("Display acknowledged.");
      DISP.setBrightness(1);
    }
    bool shouldScroll() { return millis() - TEXT_SCROLL_SPEED > lastMessageUpdatedMs;}
    bool displayHasBeenOff() { return MESSAGE_OFF_INTERVAL <  millis() - lastMessageEndedMs; }
    void prepareMessage() {
      uint8_t messageIndex = random(0,MESSAGE_COUNT);
      message = messages[messageIndex];
      hasMessage = true;
      lastMessageUpdatedMs = millis();
      Serial.print("🗣️ Starting new message: ");
      Serial.println(message);
    }
    void print(){ DISP.print(message); }
    void scroll(){
      unsigned long now = millis();
      lastMessageUpdatedMs = now;
      if (strlen(message) == 0){
        lastMessageEndedMs = now;
        hasMessage = false;
        Serial.println("🗣️ End of message.");
      } else {
        message = message + 1;
      }
    }
};

struct Vapor {
  private:
    unsigned long lastVaporEndedMs = 0;
    unsigned long lastVaporStartedMs = 0;
  public:
    bool isEmitting = false;
    void init(){
      pinMode(VAPOR_PIN, OUTPUT);
      digitalWrite(VAPOR_PIN, HIGH);
    }
    bool shouldStopEmitting() { return isEmitting && millis() - lastVaporStartedMs > VAPOR_ON_INTERVAL; }
    void stopEmitting(){
      digitalWrite(VAPOR_PIN, LOW);
      isEmitting = false;
      lastVaporEndedMs = millis();
      Serial.println("💦 Stopping vapor");
    }
    bool shouldStartEmitting(){ return !isEmitting && VAPOR_OFF_INTERVAL < millis() - lastVaporEndedMs; }
    void startEmitting(){
      digitalWrite(VAPOR_PIN, HIGH);
      isEmitting = true;
      lastVaporStartedMs = millis();
      Serial.println("💦 Starting vapor");
    }
};

struct ServoMotor {
  private:
    uint8_t currentServoPosition = 0;
    unsigned long lastServoUpdateMs = 0;
    int servoUpdateWaitInterval = 2000;
  public:
    void init(){
      SE.attach(SERVO_PIN, SERVO_START_POSITION);
      SE.setSpeed(SERVO_START_SPEED);
      SE.setEasingType(EASE_CUBIC_IN_OUT);
    }
    bool shouldMove(){ return servoUpdateWaitInterval < millis() - lastServoUpdateMs && !SE.isMoving(); }
    void moveToRandomPoint(){
      uint8_t nextAngle = random(SERVO_MIN_ANGLE,SERVO_MAX_ANGLE);
      uint8_t nextSpeed = random(50,150);
      lastServoUpdateMs = millis();
      servoUpdateWaitInterval = random(2000,5000);
      this->moveTo(nextAngle, nextSpeed);
    }
    void moveToFurthestPoint(){
      uint8_t currentServoPosition = SE.getCurrentAngle();
      uint8_t nextAngle = abs(SERVO_MAX_ANGLE-currentServoPosition) > abs(SERVO_MIN_ANGLE-currentServoPosition) ? SERVO_MAX_ANGLE : SERVO_MIN_ANGLE;
      uint8_t nextSpeed = random(50,150);
      servoUpdateWaitInterval = random(2000,5000);
      lastServoUpdateMs = millis();
      this->moveTo(nextAngle, nextSpeed);
    }
    void moveTo(uint8_t angle, uint8_t speed){
      Serial.print("🏃🏻‍♀️ Moving: ");
      Serial.print(SE.getCurrentAngle());
      Serial.print("->");
      Serial.print(angle);
      Serial.print(" at speed ");
      Serial.println(speed);
      SE.startEaseTo(angle, speed);
    }
};

struct Laser{
  public:
    void init(){
      pinMode(LASER_PIN, OUTPUT);
      digitalWrite(LASER_PIN, HIGH);
    }
};

Laser laser;
Vapor vapor;
Display display;
ServoMotor servo;

void setup() {
  laser.init();
  vapor.init();
  display.init();
  servo.init();
}

void loop() {

  if ( display.hasMessage ){
    display.print();
    if ( display.shouldScroll() ) display.scroll();
  } 
  else if (display.displayHasBeenOff() && vapor.isEmitting) {
    display.prepareMessage();
    display.print();
  }

  if (vapor.shouldStopEmitting()) vapor.stopEmitting();
  else if (vapor.shouldStartEmitting()) vapor.startEmitting();

  if (servo.shouldMove()){
    if ( vapor.isEmitting ) servo.moveToRandomPoint();
    else servo.moveToFurthestPoint();
  }
}
