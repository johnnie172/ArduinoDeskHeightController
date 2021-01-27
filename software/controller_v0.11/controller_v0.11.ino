// Include the library:
#include <TM1637Display.h>
// Define the connections pins:
#define CLK 2 // attach pin D2 Arduino to pin CLK of TM1637Display
#define DIO 3 // attach pin D3 Arduino to pin DIO of TM1637Display
// Create display object of type TM1637Display:
TM1637Display display = TM1637Display(CLK, DIO);
int k;
// Create array that turns all segments on:
const uint8_t data[] = {0xff, 0xff, 0xff, 0xff};

#define echoPin 4 // attach pin D4 Arduino to pin Echo of HC-SR04
#define trigPin 5 //attach pin D5 Arduino to pin Trig of HC-SR04

// defines variables
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement

int table_plate = 3;
int distance_to_sensor = 69;
int delta = table_plate+distance_to_sensor;

int memory1 = 41;
int memory2 = 8;

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



void setup() {

  ///
  // Trying analog pins for relay control:
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
///
  
  digitalWrite(DownPin, HIGH);
  digitalWrite(UpPin, HIGH);

  Serial.begin(9600);
  
  pinMode(buttonUpPin, INPUT);
  pinMode(buttonDownPin, INPUT);
  pinMode(buttonM1Pin, INPUT);
  pinMode(buttonM2Pin, INPUT);
  pinMode(buttonMemPin, INPUT);
  pinMode(UpPin, OUTPUT);
  pinMode(DownPin, OUTPUT);

  display.clear();
  
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  
  display.setBrightness(7);
  display.setSegments(data);
  delay(500);
  display.clear();
  Serial.print("Memory1:");
  Serial.println(memory1);
  Serial.print("Memory2:");
  Serial.println(memory2);
}

int measure_dustance() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = (duration/2) / 29.1;
    return distance;
}

int flash_screen(int distance) {

            for(k = 0; k < 4; k++) {
            display.setBrightness(4, false);  // Turn off
            display.showNumberDecEx(distance+delta,0,false,3,0);
            delay(100);
            display.setBrightness(4, true); // Turn on
            display.showNumberDecEx(distance+delta,0,false,3,0);
            delay(100);
            }
}



void loop() {
    

    digitalWrite(DownPin, HIGH);
    digitalWrite(UpPin, HIGH);
    distance = measure_dustance();
    
    // buttons for UP/DOWN:
    while (digitalRead(buttonUpPin) == HIGH && distance > 2 && distance < 80 ) {
      // Up:
      distance = measure_dustance();
      Serial.print("distance:");
      Serial.println(distance);
      digitalWrite(UpPin, LOW);
      delay(100);
      Serial.print("UpPin state:");
      Serial.println(digitalRead(UpPin));
    }
    
    while (digitalRead(buttonDownPin) == HIGH && distance > 2 && distance < 80 ) {
      // Down:
      distance = measure_dustance();
      Serial.print("distance:");
      Serial.println(distance);
      digitalWrite(DownPin, LOW);
      Serial.print("Downpin state:");
      Serial.println(digitalRead(DownPin));
    }

    
    // buttons for Memory:

    if (digitalRead(buttonM1Pin) == HIGH){
      Serial.print("M1 Button");
      delay(100);
      flash_screen(memory1);
      distance = measure_dustance();
      
      while (distance != memory1 && digitalRead(buttonUpPin) == LOW && digitalRead(buttonDownPin) == LOW) {
        distance = measure_dustance();
        Serial.print("distance:");
        Serial.println(distance);
        
        if (distance > memory1){
          digitalWrite(DownPin, LOW);
          Serial.print("Downpin state:");
          Serial.println(digitalRead(DownPin));
        }
        if (distance < memory1) {
          digitalWrite(UpPin, LOW);
          Serial.print("UpPin state:");
          Serial.println(digitalRead(UpPin)); 
        }
      }
    }

    
    if (digitalRead(buttonM2Pin) == HIGH){
      Serial.print("M2 Button");
      delay(100);
      flash_screen(memory2);
      distance = measure_dustance();
      
      while (distance != memory2 && digitalRead(buttonUpPin) == LOW && digitalRead(buttonDownPin) == LOW) {
        distance = measure_dustance();
        Serial.print("distance:");
        Serial.println(distance);
        
        if (distance > memory2){
          digitalWrite(DownPin, LOW);
          Serial.print("Downpin state:");
          Serial.println(digitalRead(DownPin));
        }
        if (distance < memory2) {
          digitalWrite(UpPin, LOW);
          Serial.print("UpPin state:");
          Serial.println(digitalRead(UpPin)); 
        }
      }
    }
    // Set M1 or M2 buttons to the current distance:
    if (digitalRead(buttonMemPin) == HIGH){
        while (digitalRead(buttonM2Pin) == LOW && digitalRead(buttonM1Pin) == LOW){
            Serial.print("mem buttonn");

            display.setBrightness(1, true);
            display.showNumberDecEx(distance+delta,0,false,3,0);
            
            if (digitalRead(buttonM1Pin) == HIGH){
                memory1 = distance;
                Serial.print("memory1:");
                Serial.println(memory1);
                display.setBrightness(7, true);
                flash_screen(memory1);
                display.clear();
                break;

            }
            if (digitalRead(buttonM2Pin) == HIGH){
                memory2 = distance;
                Serial.print("memory2:");
                Serial.println(memory2);
                display.setBrightness(7, true);
                flash_screen(memory2);
                display.clear();
                break;
            }
            if (digitalRead(buttonUpPin) == HIGH || digitalRead(buttonDownPin) == HIGH){
              display.setBrightness(7, true);
              display.clear();
              break;
            }
      }
  }
  
    
    
  display.showNumberDecEx(distance+delta,0,false,3,0);
  
}
