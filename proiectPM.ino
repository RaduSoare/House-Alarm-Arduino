#include <Servo.h>
#include <IRremote.h>

// PIR defines 
const int PIR_SENSOR = 3; // Citesc de pe PIN-ul 3 valoarea PIR-ului
int PIR_state = LOW;
int PIR_value = 0;

// Proximity sensor defines
const int ULTRASONIC_SENSOR_TRIG = 5;
const int ULTRASONIC_SENSOR_ECHO = 6;
const int MAX_DISTANCE_ALLOWED = 50; // in CM
long duration;
int distance;

// Servo-motor defines
const int SERVO_MOTOR = 9;
Servo myservo;  // create servo object to control a servo
int servo_position = 0; // variable to store the servo position

// Remote defines
#define Button_0 0xC101E57B
#define Button_1 0x9716BE3F
#define Button_2 0x3D9AE3F7
#define Button_3 0x6182021B
#define Button_4 0x8C22657B
#define Button_5 0x488F3CBB
#define Button_6 0x449E79F
#define Button_7 0x32C6FDF7
#define Button_8 0x1BC0157B
#define Button_9 0x3EC3FC1B

const int IR_RECEIVER = 11;
IRrecv irrecv(IR_RECEIVER); // creeaza o instanta a receiverului
decode_results remote_results;


int password[] = {3, 7, 1, 9};
int current_password_digit = 0;
int motion_detected = 0;

unsigned long myTime = 0;

void setup() {
  pinMode(PIR_SENSOR, INPUT); // Set PIR Pin as input
  
  pinMode(ULTRASONIC_SENSOR_TRIG, OUTPUT); // Sets the trigPin as an Output
  pinMode(ULTRASONIC_SENSOR_ECHO, INPUT); // Sets the echoPin as an Input

  myservo.attach(SERVO_MOTOR);  // attaches the servo on pin 9 to the servo object

  irrecv.enableIRIn(); // start IR receiver
  
  Serial.begin(9600); 

  
}

void get_ultrasonic_data() {
   // GET DATA FROM ULTRASONIC SENSOR
    /* Calibrate ultrasonic sensor */
    // Clears the ULTRASONIC_SENSOR_TRIG
    digitalWrite(ULTRASONIC_SENSOR_TRIG, LOW);
    delay(10);
    // Sets the ULTRASONIC_SENSOR_TRIG on HIGH state for 10 micro seconds
    digitalWrite(ULTRASONIC_SENSOR_TRIG, HIGH);
    delay(10);
    digitalWrite(ULTRASONIC_SENSOR_TRIG, LOW);
  
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(ULTRASONIC_SENSOR_ECHO, HIGH);
    // Calculating the distance
    distance= duration*0.034/2;
    // Prints the distance on the Serial Monitor
    //Serial.print("Distance: ");
    //Serial.println(servo_position);
    //Serial.println(distance);
    if (distance > 0 && distance < MAX_DISTANCE_ALLOWED) {
      Serial.println("Ultrrasonic Motion detected!");
      motion_detected = 1;
    }
}

void get_pir_data() {
  // GET DATA FROM MOTION SENSOR
    PIR_value = digitalRead(PIR_SENSOR);
    if (PIR_value == HIGH) {
      if (PIR_state == LOW) {
        //Serial.println(servo_position);
        Serial.println("PIR Motion detected!");
       // motion_detected = 1;
        // Serial.println(millis() / 1000);
        PIR_state = HIGH;
        delay(10);
      }
    } else {
        if (PIR_state == HIGH){
          //Serial.println(servo_position);
          //Serial.println("Motion stopped!");
          PIR_state = LOW;
          delay(10);
      }
    }
}

void loop() {
  
  if (motion_detected == 0) {
     
    
    for (servo_position = 0; servo_position <= 45; servo_position += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      myservo.write(servo_position);              // tell servo to go to position in variable 'servo_position'
      get_ultrasonic_data();
      delay(10);
      get_pir_data();
      
      if (motion_detected == 1) {
        myservo.write(0); 
        break;
      }
      delay(30);                       // waits 15ms for the servo to reach the position
    }
    for (servo_position = 45; servo_position >= 0; servo_position -= 1) { // goes from 180 degrees to 0 degrees
      myservo.write(servo_position);              // tell servo to go to position in variable 'servo_position'
      get_ultrasonic_data();
      delay(10);
      get_pir_data();
      if (motion_detected == 1) {
  
        myservo.write(0); 
        break;
      }
      delay(30);                       // waits 15ms for the servo to reach the position
    }
  
    
  } else {
      //Serial.println("Motion detected!");
      myTime = millis();
      if ( motion_detected == 1) {
        
        if ((myTime / 1000) > 40) {
          Serial.println("Alarm");
          delay(100);
          exit(-1);
        }
      }
      

      
      
      if (irrecv.decode(&remote_results)) {
        int current_digit_input = 0;
        switch(remote_results.value) {
          case Button_0: current_digit_input = 0; break;
          case Button_1: current_digit_input = 1; break;
          case Button_2: current_digit_input = 2; break;
          case Button_3: current_digit_input = 3; break;
          case Button_4: current_digit_input = 4; break;
          case Button_5: current_digit_input = 5; break;
          case Button_6: current_digit_input = 6; break;
          case Button_7: current_digit_input = 7; break;
          case Button_8: current_digit_input = 8; break;
          case Button_9: current_digit_input = 9; break;
          default : current_digit_input = -1; break;
        }
        //Serial.println(current_digit_input);
        if (current_digit_input != -1) {
          Serial.println("Insert digit number: " + (String)current_password_digit);
          Serial.println("Ai introdus: " + (String)current_digit_input);
          if (current_digit_input == password[current_password_digit]) {
            Serial.println("Correct digit!");
          } else {
            Serial.println("Alarm");
            delay(100);
            exit(-1);
          }
          current_password_digit++;
          if (current_password_digit == 4) {
            Serial.println("Deactivated");
            
            delay(100);
            exit(-1);
            // Opreste sistemul dupa ce alarma a fost dezactivata
          }
        } else {
          Serial.println("Insert digit again!");
        }
        irrecv.resume(); // obtine urmatoarea valoarea
    }
  }

}
