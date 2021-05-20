#include <Servo.h>
#include <IRremote.h>

// PIR defines 
const int PIR_SENSOR = 3;
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
IRrecv irrecv(IR_RECEIVER); // creeaza a receiver instance
decode_results remote_results;

// LED defines
const int LED = 12;

// Password for alarm deactivation
int password[] = {3, 7, 1, 9};
int current_password_digit = 0;
int wrong_password_count = 3;
int motion_detected = 0;
const int TIME_LIMIT = 20; // seconds


void setup() {
  pinMode(PIR_SENSOR, INPUT); // Set PIR Pin as input
  
  pinMode(ULTRASONIC_SENSOR_TRIG, OUTPUT); // Sets the trigPin as an Output
  pinMode(ULTRASONIC_SENSOR_ECHO, INPUT); // Sets the echoPin as an Input

  myservo.attach(SERVO_MOTOR);  // attaches the servo on pin 9 to the servo object

  irrecv.enableIRIn(); // start IR receiver

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  
  Serial.begin(9600); 

  
}

/* GET DATA FROM ULTRASONIC SENSOR */
void get_ultrasonic_data() {
   
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
    distance = duration*0.034/2;
    // Prints the distance on the Serial Monitor
    if (distance > 0 && distance < MAX_DISTANCE_ALLOWED) {
       Serial.println("Motion detected!");
      motion_detected = 1;
    }
}

// GET DATA FROM MOTION SENSOR
void get_pir_data() {
    PIR_value = digitalRead(PIR_SENSOR);
    if (PIR_value == HIGH) {
      if (PIR_state == LOW) {
        Serial.println("Motion detected!");
        motion_detected = 1;
        PIR_state = HIGH;
        delay(10);
      }
    } else {
        if (PIR_state == HIGH){
          PIR_state = LOW;
          delay(10);
      }
    }
}

void loop() {

  // While motion was not detected, read data from sensors
  if (motion_detected == 0) {
     
    for (servo_position = 0; servo_position <= 45; servo_position += 1) {
      myservo.write(servo_position); // tell servo to go to position in variable 'servo_position'
      get_ultrasonic_data();
      delay(10);
      get_pir_data();
      
      if (motion_detected == 1) {
        myservo.write(0); 
        break;
      }
      delay(30); // waits 30ms for the servo to reach the position
    }
    for (servo_position = 45; servo_position >= 0; servo_position -= 1) {
      myservo.write(servo_position); // tell servo to go to position in variable 'servo_position'
      get_ultrasonic_data();
      delay(10);
      get_pir_data();
      if (motion_detected == 1) {
        myservo.write(0); 
        break;
      }
      delay(30); // waits 30ms for the servo to reach the position
    }
  
    
  } else {
     
      // Count seconds from alarm activation
      static int startedTime = millis();
      
      // Mark alarm activation with the LED
      digitalWrite(LED, HIGH);
      
      if ( motion_detected == 1) {
        // When the TIME_LIMIT (seconds) have elapsed, the alarm is triggered
        if (((millis() - startedTime)  / 1000) > TIME_LIMIT) {
          Serial.println("Alarm");
          digitalWrite(LED, LOW);
          delay(100);
          exit(-1);
        }
      }

      // Get user input from IR remote
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
        
        // Check for unaltered digit from remote 
        if (current_digit_input != -1) {
          Serial.println("Insert digit number: " + (String)current_password_digit);
          Serial.println("Ai introdus: " + (String)current_digit_input);
          if (current_digit_input == password[current_password_digit]) {
            Serial.println("Correct digit!");
            current_password_digit++;
          } else {
            current_password_digit = 0;
            wrong_password_count--; 
            // When the password is wrong for the third time, the alarm in triggered
            if (wrong_password_count == 0) {
              Serial.println("Alarm");
              digitalWrite(LED, LOW);
              delay(100);
              exit(-1);
            }
            Serial.println("Wrong password! You have " + (String)wrong_password_count + " more attempts");
            
          }
          
          if (current_password_digit == 4) {
            Serial.println("Deactivated");
            digitalWrite(LED, LOW);
            delay(100);
            exit(-1);
            // System is shut down when the alarm is deactivated
          }
        } else {
          Serial.println("Insert digit again!");
        }
        irrecv.resume(); // get next value
    }
  }

}
