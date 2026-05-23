#define BLYNK_TEMPLATE_ID "TMPL3AD9i7t-Y"
#define BLYNK_TEMPLATE_NAME "car"
#define BLYNK_AUTH_TOKEN "lec-Zdcy1tqwADvJoPzq2yG4yWgK4DOZ"

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "lec-Zdcy1tqwADvJoPzq2yG4yWgK4DOZ";
char ssid[] = "CMF";
char pass[] = "12345678";

// Terminal on V4
WidgetTerminal terminal(V4);


#define m1p1 D0
#define m1p2 D1
#define m2p1 D2
#define m2p2 D3
#define m1ap D4
#define m2ap D5
#define buz D8
#define trig D6
#define echo D7

int distance;
int lastdistance;
int power=0;
int forwardEnabled =0;   //  state variable
int backwardEnabled=0;
int leftEnabled=0;
int rightEnabled=0;
int right_analog_speed=120;
int left_analog_speed=120;
int analog_speed=100;

int ultra() {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duration = pulseIn(echo, HIGH, 20000);
  int d = duration * 0.034 / 2;
  if(d!=lastdistance){
    terminal.print("Distance : ");
    terminal.print(d);
    terminal.println("cm");
    lastdistance=d;
  }  
  if(d==0){
    return 400;
  }
  return d;
}

//Motors stop
void stopMotors() {
  digitalWrite(m1p1, LOW);
  digitalWrite(m1p2, LOW);
  digitalWrite(m2p1, LOW);
  digitalWrite(m2p2, LOW);
  analogWrite(m1ap, 0);
  analogWrite(m2ap, 0);
}

//Forward code
void moveForward() {
  digitalWrite(m1p1, HIGH);
  digitalWrite(m1p2, LOW);
  digitalWrite(m2p1, HIGH);
  digitalWrite(m2p2, LOW);
  analogWrite(m1ap, analog_speed);
  analogWrite(m2ap, analog_speed);
}

//Backward code
void backward(){
    digitalWrite(m1p1, LOW);
    digitalWrite(m1p2, HIGH);
    digitalWrite(m2p1, LOW);
    digitalWrite(m2p2, HIGH);
    analogWrite(m1ap, analog_speed);
    analogWrite(m2ap, analog_speed);
}

//left side 
void moveleft(){
   digitalWrite(m1p1, LOW);
  digitalWrite(m1p2, LOW);
  digitalWrite(m2p1, HIGH);
  digitalWrite(m2p2, LOW);
  analogWrite(m1ap, 0);
  analogWrite(m2ap, left_analog_speed);
  }

//Right side 
void moveright(){
   digitalWrite(m1p1, HIGH);
  digitalWrite(m1p2, LOW);
  digitalWrite(m2p1, LOW);
  digitalWrite(m2p2, LOW);
  analogWrite(m1ap, right_analog_speed);
  analogWrite(m2ap, 0);
}



void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);

  pinMode(m1p1, OUTPUT);
  pinMode(m1p2, OUTPUT);
  pinMode(m2p1, OUTPUT);
  pinMode(m2p2, OUTPUT);
  pinMode(m1ap, OUTPUT);
  pinMode(m2ap, OUTPUT);
  pinMode(buz, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
}

/* ---------- BLYNK BUTTONS ---------- */

// Backward button (no ultrasonic restriction)
BLYNK_WRITE(V0) {
  int w = param.asInt();
  backwardEnabled=w;
  if (w == 1 && power==1) {
   backward();
  } else if(backwardEnabled==0) {
    stopMotors();
  }
}

// Forward button
BLYNK_WRITE(V1) {
  int u = param.asInt();
  forwardEnabled = u ;   //  just set state

  if (forwardEnabled==0) {
    stopMotors();
    noTone(buz);
  }
}

//Left side moving
BLYNK_WRITE(V2) {
  int y = param.asInt();
  leftEnabled=y;

  if(leftEnabled==0){
     stopMotors();
    noTone(buz);
  }
}

// Right side moving 
BLYNK_WRITE(V3) {
  int r = param.asInt();
  rightEnabled=r;

  if(rightEnabled==0 ){
     stopMotors();
    noTone(buz);
  }
}

//power pin
BLYNK_WRITE(V5){
   int p=param.asInt();
   power=p;
}
//analog speed
BLYNK_WRITE(V6){
  int a=param.asInt();
  analog_speed=a;

}

//right analog speed
BLYNK_WRITE(V7){
  int b=param.asInt();
  right_analog_speed=b;
}
//left analog speed
BLYNK_WRITE(V8){
  int c=param.asInt();
  left_analog_speed=c;
}


//calling loop function for continous calling of blynk
void blynk_calling(){
  if(backwardEnabled==0 && (forwardEnabled==1 || rightEnabled==1 || leftEnabled==1)){
  distance = ultra();
    delay(80);
    Serial.println(distance);
    if(backwardEnabled==0 && distance<20 && distance>0){
     
      if(distance>10  && rightEnabled==1 ){
        moveright();
        noTone(buz);
    }
      else if(distance>10 && leftEnabled==1){
        moveleft();
        noTone(buz);
      }
    else{
      stopMotors();
      if(forwardEnabled==1 || rightEnabled==1 || leftEnabled==1 )
      tone(buz,2000);
    }

    }

    else if(backwardEnabled==0 && distance>20 ) {
      noTone(buz);
       if (forwardEnabled==1 && rightEnabled==0 && leftEnabled==0 ) {
        moveForward();
      }
      else if(forwardEnabled==0 && rightEnabled==1 && leftEnabled==0){
        moveright();
      }
      else if(forwardEnabled==0 && rightEnabled==0 && leftEnabled==1){
          moveleft();
    }

}
    }
}

/* ---------- MAIN LOOP ---------- */

void loop() {
  Blynk.run();   // always runs 
  if(power==1){
  blynk_calling();
  }
}