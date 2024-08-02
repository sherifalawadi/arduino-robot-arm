#include <Servo.h>
#include <LiquidCrystal.h>

Servo baseServo;
Servo elbowServo;

#define basePin 5
#define elbowPin 6
#define fireSensorPin A0
#define DIG_PIN 3
#define alarmPin 10


LiquidCrystal lcd(12,11,9,8,7,4);
bool isAutomaticMode = true;
int fireIntensity = 0;
int newFire = 0;
int randomBase = 90;
int randomElbow = 90;

void setup() {
  
  randomSeed(analogRead(A1));
  
  baseServo.attach(basePin);
  elbowServo.attach(elbowPin);

  Serial.begin(9600);
  lcd.begin(16, 2);
  
  lcd.print("Initializing...");
  
  
  
  baseServo.write(randomBase);
  elbowServo.write(randomBase);

  pinMode(alarmPin, OUTPUT);
  pinMode(DIG_PIN, INPUT);
  displayMode();
}
 
void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    handleCommand(command);
  }

  if (!isAutomaticMode) {
    flame_detector();
  }
  if (isAutomaticMode) {
    flame_detector();
    automaticMode();
  }
}

void handleCommand(String command) {
  if(!digitalRead(DIG_PIN)){
  if (command == "A") {
    isAutomaticMode = true;
    automaticMode();
    displayMode();

  } else if (command == "M") {
    isAutomaticMode = false;
    displayMode();

  } 
  else if (!isAutomaticMode) {
    if (command.startsWith("B")) {
      int angle = command.substring(1).toInt();
      baseServo.write(angle);
      updateLCD("Base Servo: ", angle);
    } else if (command.startsWith("E")) {
      int angle = command.substring(1).toInt();
      elbowServo.write(angle);
      updateLCD("Elbow Servo: ", angle);

    }
  }
}
}

void automaticMode() {
  baseServo.write(randomBase);
  elbowServo.write(randomBase);
 if (!digitalRead(DIG_PIN)) { 
  randomBase = random(0,181);
  randomElbow = random(0,181);
  delay(1000);
  }
 }
void updateLCD(String label , int value ){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(label);
  lcd.print(value);
}

void updateLCD(String label1,String label2, int value) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(label1);
  lcd.setCursor(0, 1);
  lcd.print(label2);
  lcd.print(value);
}


void displayMode() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mode: ");
  lcd.print(isAutomaticMode ? "Auto" : "Manual");
}

void flame_detector() {
  newFire = analogRead(fireSensorPin);
  if(fireIntensity != newFire){
    fireIntensity = newFire;
    if (digitalRead(DIG_PIN)) {
      updateLCD("Fire Detected!","Intensity: ",fireIntensity);
      digitalWrite(alarmPin, HIGH);
  } else {
      updateLCD("No Fire Detected","Intensity: ",fireIntensity);
      digitalWrite(alarmPin, LOW);
  }
  }
}
