// Include the library:
#include <TM1637Display.h>
#include <Adafruit_Sensor.h> // Including Adafruit sensor library:
#include <DHT.h> //including DHT-sensor-library
 

// Define the connections pins:
#define CLK 2 // attach pin D2 Arduino to pin CLK of TM1637Display
#define DIO 3 // attach pin D3 Arduino to pin DIO of TM1637Display

#define DHTPin 11       //defining the sensor pin (pin no-04)
#define DHTType DHT11  // As we are using DHT11 sensor thus Defining DHT 

#define echoPin 4 // attach pin D4 Arduino to pin Echo of HC-SR04
#define trigPin 5 //attach pin D5 Arduino to pin Trig of HC-SR04


// Create DHT display object of type DHT11:
DHT dht = DHT(DHTPin,DHTType);

// Create display object of type TM1637Display:
TM1637Display display = TM1637Display(CLK, DIO);
int k;
// Create array that turns all segments on:
const uint8_t data[] = {0xff, 0xff, 0xff, 0xff};



// defines variables
long duration; // variable for the duration of sound wave travel
float distance; // variable for the distance measurement
float speedOfSound; //defining speed of sound it will store the calculated speed of sound

int table_plate = 3;
int distance_to_sensor = 67;
int delta = table_plate+distance_to_sensor;
int tempDistanceLatest;
int tempDistance;

float memory1 = 41.0;
float memory2 = 8.0;
int memory1int;
int memory2int;

// Trying analog pins for relay control:

const int UpPin =  A0;
const int DownPin =  A1;
//

const int buttonUpPin = 6;
int buttonUpState = 0;
const int buttonDownPin = 7;
int buttonDownState = 0;
const int buttonM1Pin = 9;
int buttonM1State = 0;
const int buttonM2Pin = 8;
int buttonM2State = 0;
const int buttonMemPin = 10;
int buttonMem = 0;


float measureSpeedOfSound() {
  float temperature = dht.readTemperature();
  Serial.print("Temperature = ");
  Serial.print(temperature);     
  speedOfSound = 331.3+(0.606*temperature);
  Serial.print("speedOfSound = ");
  Serial.print(speedOfSound);     
  return speedOfSound;
}

float measureDistanceCompensation(float speedOfSound) {
    digitalWrite(trigPin, LOW);//setting trigger pin in LOW state
    delayMicroseconds(5);      //for 5 microseconds
   
    digitalWrite(trigPin, HIGH);  //setting trigger pin in HIGH state
    delayMicroseconds(10);       //for 10 microsecond and emits 8pulses from the transmitter and bounces back onces hit by an object
    digitalWrite(trigPin, LOW);  //after recieving the signal,setting the trigger pin to LOW state
   
    duration = pulseIn(echoPin, HIGH);
    distance = duration*(speedOfSound/10000)/2;

    distance = (floor((distance*2)+0.5)/2);
  return distance;
  }

int displayFloat(int tempDistance) {
  
  // Showing only if the distance is +- 0.5:
  if (tempDistanceLatest > tempDistance+5 || tempDistanceLatest < tempDistance-5) {
        tempDistanceLatest = tempDistance;  
        display.showNumberDecEx(tempDistance,0b00100000 ,false,4,0);
  }
}

int changeFloatTo4Digits(float distance, float delta) {
  float newDistance = distance+delta;
  int Distance4Digits = newDistance*10;
  return Distance4Digits;
}
  



int flash_screen(int tempDistance) {

            for(k = 0; k < 4; k++) {
            display.setBrightness(4, false);  // Turn off
            display.showNumberDecEx(tempDistance,0b00100000 ,false,4,0);
            delay(100);
            display.setBrightness(4, true); // Turn on
            display.showNumberDecEx(tempDistance,0b00100000 ,false,4,0);
            delay(100);
            }
}


void setup() {
  ///
  // Trying analog pins for relay control:
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
///
  
  digitalWrite(DownPin, HIGH);
  digitalWrite(UpPin, HIGH);

  Serial.begin(9600);
  dht.begin();
  
  pinMode(buttonUpPin, INPUT);
  pinMode(buttonDownPin, INPUT);
  pinMode(buttonM1Pin, INPUT);
  pinMode(buttonM2Pin, INPUT);
  pinMode(buttonMemPin, INPUT);
  pinMode(UpPin, OUTPUT);
  pinMode(DownPin, OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT

  display.clear();
  
  display.setBrightness(7);
  display.setSegments(data);
  delay(500);
  display.clear();
  Serial.print("Memory1:");
  Serial.println(memory1);
  Serial.print("Memory2:");
  Serial.println(memory2);

  speedOfSound = measureSpeedOfSound();
  distance = measureDistanceCompensation(speedOfSound);
  Serial.print("distance:");
  Serial.println(distance);
}



void loop() {

    digitalWrite(DownPin, HIGH);
    digitalWrite(UpPin, HIGH);
    distance = measureDistanceCompensation(speedOfSound);
    
    // buttons for UP/DOWN:
    while (digitalRead(buttonUpPin) == HIGH && distance > 2 && distance < 80 ) {
      // Up:
      distance = measureDistanceCompensation(speedOfSound);
      Serial.print("distance:");
      Serial.println(distance);
      digitalWrite(UpPin, LOW);
      delay(100);
      Serial.print("UpPin state:");
      Serial.println(digitalRead(UpPin));
    }
    
    while (digitalRead(buttonDownPin) == HIGH && distance > 2 && distance < 80 ) {
      // Down:
      distance = measureDistanceCompensation(speedOfSound);
      Serial.print("distance:");
      Serial.println(distance);
      digitalWrite(DownPin, LOW);
      Serial.print("Downpin state:");
      Serial.println(digitalRead(DownPin));
    }

    
    // buttons for Memory:

    if (digitalRead(buttonM1Pin) == HIGH){
      Serial.print("M1 Button");
      Serial.print("memory1:");
      Serial.println(memory1);
      delay(100);
      memory1int = changeFloatTo4Digits(memory1, delta);
      flash_screen(memory1int);
      speedOfSound = measureSpeedOfSound();
      distance = measureDistanceCompensation(speedOfSound);
      
      while (distance != memory1 && digitalRead(buttonUpPin) == LOW && digitalRead(buttonDownPin) == LOW) {
        distance = measureDistanceCompensation(speedOfSound);
        Serial.print("distance:");
        Serial.println(distance);
        
        if (distance > memory1){
          digitalWrite(UpPin, HIGH);
          digitalWrite(DownPin, LOW);
          Serial.print("Downpin state:");
          Serial.println(digitalRead(DownPin));
        }
        if (distance < memory1) {
          digitalWrite(DownPin, HIGH);
          digitalWrite(UpPin, LOW);
          Serial.print("UpPin state:");
          Serial.println(digitalRead(UpPin)); 
        }
      }
    }

    
    if (digitalRead(buttonM2Pin) == HIGH){
      Serial.print("M2 Button");
      Serial.print("memory2:");
      Serial.println(memory2);
      delay(100);
      memory2int = changeFloatTo4Digits(memory2, delta);
      flash_screen(memory2int);
      speedOfSound = measureSpeedOfSound();
      distance = measureDistanceCompensation(speedOfSound);
      
      while (distance != memory2 && digitalRead(buttonUpPin) == LOW && digitalRead(buttonDownPin) == LOW) {
        distance = measureDistanceCompensation(speedOfSound);
        Serial.print("distance:");
        Serial.println(distance);
        
        if (distance > memory2){
          digitalWrite(UpPin, HIGH);
          digitalWrite(DownPin, LOW);
          Serial.print("Downpin state:");
          Serial.println(digitalRead(DownPin));
        }
        if (distance < memory2) {
          digitalWrite(DownPin, HIGH);
          digitalWrite(UpPin, LOW);
          Serial.print("UpPin state:");
          Serial.println(digitalRead(UpPin)); 
        }
      }
    }
    // Set M1 or M2 buttons to the current distance:
    if (digitalRead(buttonMemPin) == HIGH){
        display.setBrightness(1, true);
        speedOfSound = measureSpeedOfSound();
        distance = measureDistanceCompensation(speedOfSound);
        
        while (digitalRead(buttonM2Pin) == LOW && digitalRead(buttonM1Pin) == LOW){
            Serial.print("mem buttonn");
            
            if (digitalRead(buttonM1Pin) == HIGH){
                memory1 = distance;
                Serial.print("memory1:");
                Serial.println(memory1);
                display.setBrightness(7, true);
                memory1int = changeFloatTo4Digits(memory1, delta);
                flash_screen(memory1int);
                display.clear();
                display.setBrightness(7, true);
                break;

            }
            if (digitalRead(buttonM2Pin) == HIGH){
                memory2 = distance;
                Serial.print("memory2:");
                Serial.println(memory2);
                display.setBrightness(7, true);
                memory2int = changeFloatTo4Digits(memory2, delta);
                flash_screen(memory2int);
                display.clear();
                display.setBrightness(7, true);
                break;
            }
            if (digitalRead(buttonUpPin) == HIGH || digitalRead(buttonDownPin) == HIGH){
              
              display.clear();
              display.setBrightness(7, true);
              break;
            }
      }
  }
  
  tempDistance = changeFloatTo4Digits(distance, delta);
  displayFloat(tempDistance);
//  display.showNumberDecEx(distance+delta,0b00100000 ,false,3,0);
}
