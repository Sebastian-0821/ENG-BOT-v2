
// ---------------------------------------------------------------- // 
// Arduino Origami Bot 
// Created by Team Lightning
// Using Arduino IDE 1.8.19
// Tested Oct. 4
// ---------------------------------------------------------------- // 

#include <LiquidCrystal_I2C.h> // liquid crystal library

LiquidCrystal_I2C lcd(0x27, 16, 2); // specifices how to refrence the LCD and it's characteristics


// define varibales

char array1[] = "Robot State:    ";
char array2[] = "STOPPED         ";
char array3[] = "BACKWARD        ";
char array4[] = "FORWARD         ";
char array5[] = "OUT OF RANGE    ";

int echoPin = 8; // attach pin D8 to pin Echo of HC-SR04
int trigPin = 7; // attach pin D7 to pin Trig of HC-SR04
int in1 = 4; // variable names for the L298N
int in2 = 5; // variable names for the L298N
int in3 = 9; // variable names for the L298N
int in4 = 10; // variable names for the L298N

int LCDprev = 99; // helps to update the LCD state tracks previous
int LCDcurr = 100; // helps to update the LCD state tracks current

int ledpin = 13; //specify the LED pin to pin 13

long duration; // duration of sound wave travel
int distance; // distance measurement

int enA = 3; //H-Bridge ENA pin subject to change
int enB = 6; //H_Bridge ENB pin subject to change

const int MIN_PWM = 90; //Lowest motor value
const int MAX_PWM = 255; //Full speed
int speedPWM = 0;

const int Boundary = 3;//3 cm of overlap between the changes in direction and speed
int state = 4;//1=BACKWARDS , 2=STOPPED, 3=FORWARD, 4=OUT OF RANGE

int nextState(int d) {
  if (state == 1 && d <= 30 + Boundary) return 1;//Checks what state should be used based off of the boundary constant
  if (state == 2 && d > 30 - Boundary && d <= 50 + Boundary) return 2;//^
  if (state == 3 && d >50 - Boundary && d <= 100 + Boundary) return 3;//^
  if (state == 4 && d > 100 - Boundary) return 4;//^

  if (d <= 30) return 1;//If the distance i well within the boundary prevoius steps are ignored
  if (d <= 50) return 2;//^
  if (d <= 100) return 3;//^
  return 4;//^
}

void setup() {

  lcd.init(); // initializes LCD
  lcd.backlight(); // turn on backlight

  pinMode (trigPin, OUTPUT) ; // sonar digital pin mode for trig
  pinMode (echoPin, INPUT); // sonar digital pin mode for echo
  
  pinMode (in1, OUTPUT); // all L298N digital pins are outputs
  pinMode (in2, OUTPUT); // all L298N digital pins are outputs
  pinMode (in3, OUTPUT); // all L298N digital pins are outputs
  pinMode (in4, OUTPUT); // all L298N digital pins are outputs
  pinMode (ledpin, OUTPUT) ; // configure LCD pin as an output

  Serial.begin (9600) ; // use default of 9600 baud rate for serial communication

  lcd.setCursor(0, 0); // set cursor at top left

  lcd.print (array1); //prints the first line "Robot State:

  pinMode(enA, OUTPUT);//OUTPUTS ENA pin
  pinMode(enB, OUTPUT);//OUTPUTS ENB pin
  
}


void loop() {
  
/*  
  NOTE: it is important only to update the LCD screen when a change of state has occured
  Otherwise, the program become inefficient. We only want to write to the LCD when 
  there is a change in what the robot is doing. The LCDprey and LCDcurr variables 
   help us to do this
*/
  LCDprev = LCDcurr;

  // Clears the trigPin condition
  digitalWrite (trigPin, LOW);//starts trig pin low
  delayMicroseconds (2); //waits for hardware to respond
  digitalWrite (trigPin, HIGH); //makes trig pin high
  delayMicroseconds (10) ; //continues high trig pin state for 10 microseconds
  digitalWrite (trigPin, LOW); //after 10 microseconds trig pin is brought low
  duration = pulseIn(echoPin, HIGH, 30000);//30ms timeout 
  if (duration == 0) {
    distance = 999;//no echo when out of rage
  } else{
    distance = duration * 0.034 / 2;
  }
  // Displays the distance on the serial Monitor
  state = nextState(distance);
  Serial.print ("Distance: ");
  Serial.print (distance);
  Serial.println(" cm");
  
  if (state == 1) {
    LCDcurr = 1;

    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    digitalWrite(ledpin, HIGH);

    speedPWM = map(constrain(distance, 0, 30), 0, 30, MIN_PWM, MAX_PWM);//Changes the speed based on distance
    analogWrite(enA, speedPWM);//Outputs analog ENA speed
    analogWrite(enB, speedPWM);//Outputs analog ENB speed

    if (LCDprev != LCDcurr) {
      lcd.setCursor(0, 1);
      lcd.print(array3);
      delay(10);
    }
    
  } else if (state == 2) {
    LCDcurr = 2;

    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
    digitalWrite(ledpin, LOW);

        speedPWM = map(constrain(distance, 30, 50), 30, 50, MIN_PWM, MAX_PWM);//Changes speed between 15 and 30cm
    analogWrite(enA, speedPWM);//Output speed based on speedPWM
    analogWrite(enB, speedPWM);//Output speed based on speedPWM

    if (LCDprev != LCDcurr) {
      lcd.setCursor(0, 1);
      lcd.print(array2);
      delay(10);
    }
    
  } else if (state == 3) {
    LCDcurr = 3;

    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    digitalWrite(ledpin, HIGH);

    speedPWM = map(constrain(distance, 50, 100), 50, 100, MIN_PWM, MAX_PWM);//Changes speed depending on whether the root is between 30 and 80 cm
    analogWrite(enA, speedPWM);//Outputs speed based on speedPWM
    analogWrite(enB, speedPWM);//Output speed based on speedPWM

    if (LCDprev != LCDcurr) {
      lcd.setCursor(0, 1);
      lcd.print(array4);
      delay(10);
    }
    
  } else {
    LCDcurr = 4;

    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
    digitalWrite(ledpin, LOW);

    speedPWM = 0;
    analogWrite(enA, 0);
    analogWrite(enB, 0);

    if (LCDprev != LCDcurr) {
      lcd.setCursor(0, 1);
      lcd.print(array5);
      delay(10);
    }
    
  }
    Serial.print("Speed PWM: ");
    Serial.println(speedPWM);//remove if code breaks
  delay(10);
  
}
