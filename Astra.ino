#define BLYNK_TEMPLATE_ID "TMPL3AD9i7t-Y"
#define BLYNK_TEMPLATE_NAME "car"
#define BLYNK_AUTH_TOKEN "lec-Zdcy1tqwADvJoPzq2yG4yWgK4DOZ"

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
//UDP server
#include <WiFiUdp.h>

char auth[] = "lec-Zdcy1tqwADvJoPzq2yG4yWgK4DOZ";

const char* ssid = "CMF";
const char* password = "12345678";

WiFiUDP udp;
unsigned int localUdpPort = 4210;
char incomingPacket[255];


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
int right_analog_speed=120;
int left_analog_speed=120;
int analog_speed=100;

char cmd[5]="04";
int mode=1;
int temp_mode=0;
unsigned long int last_time=0;


void setup() {
  Serial.begin(9600);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  pinMode(m1p1, OUTPUT);
  pinMode(m1p2, OUTPUT);
  pinMode(m2p1, OUTPUT);
  pinMode(m2p2, OUTPUT);
  pinMode(m1ap, OUTPUT);
  pinMode(m2ap, OUTPUT);
  pinMode(buz, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");

  Serial.print("📡 ESP IP: ");
  Serial.println(WiFi.localIP());
  // Start UDP server
  udp.begin(localUdpPort);
  Serial.println("📨 UDP listener started on port 4210");
  
  // Start Blynk
  Blynk.config(auth, "blynk.cloud", 80);
  Blynk.connect();
  delay(4000);
  terminal.println("WiFi connected");
  terminal.println("UDP server connected");
  terminal.println("Blynk server connected");
  delay(1000);
}


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



/* ---------- BLYNK BUTTONS ---------- */

// Backward button (no ultrasonic restriction)
BLYNK_WRITE(V0) {
  int w = param.asInt();
    if(w==1){
 strcpy(cmd, "00");
  Serial.println("on");
 terminal.println("backward");
    }
  else {
  strcpy(cmd, "04");
  Serial.println("stop");
   terminal.println("stop");
  }
}

// Forward button
BLYNK_WRITE(V1) {
  int u = param.asInt();
  
  if(u==1){
 strcpy(cmd, "01");
 Serial.println("on");
  terminal.println("forward");
  }
  else {
  strcpy(cmd, "04");
  Serial.println("stop");
   terminal.println("stop");
  }
}

//Left side moving
BLYNK_WRITE(V2) {
  int y = param.asInt();
  if(y==1){
 strcpy(cmd, "03");
 Serial.println("left");
  terminal.println("left");
  }
  else {
  strcpy(cmd, "04");
  Serial.println("on");
  terminal.println("stop");
  }
}

// Right side moving 
BLYNK_WRITE(V3) {
  int r = param.asInt();
   if(r==1){
 strcpy(cmd, "02");
 Serial.println("right");
  terminal.println("right");
  }
  else {
  strcpy(cmd, "04");
  Serial.println("on");
  terminal.println("stop");
  }
}

//power pin
BLYNK_WRITE(V5){
   int p=param.asInt();
   power=p;
   if(power==1){
   Serial.println("on");
   terminal.println("on");
   }
  else{
  Serial.println("off");
   terminal.println("off");;
  }
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


//eyes calling function
void eyes_calling(){
  if(strcmp(cmd,"s")==0)
    Serial.println("off");
  else if(strcmp(cmd,"02")==0)
    Serial.println("right");
  else if(strcmp(cmd,"03")==0)
    Serial.println("left");
  else if(strcmp(cmd,"00")==0 || strcmp(cmd,"01")==0|| strcmp(cmd,"u")==0  || strcmp(cmd,"04")==0)
    Serial.println("on");
  
}
void server_udp() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(incomingPacket, 255);
    if (len > 0) incomingPacket[len] = 0;

     strcpy(cmd, incomingPacket);
    
    Serial.print("➡️ CMD: ");
    Serial.println(cmd);
    terminal.print("➡️ CMD: ");
    terminal.println(cmd);
    if (strcmp(cmd,"r")==0)
      mode=1; //remote controlling mode
    else if (strcmp(cmd,"u")==0)
      mode=2; //UDP server mode 
    else if(strcmp(cmd,"s")==0)
      mode=0; // shutdown condition
    //Printing mode value only one time in blynk to aviod continuos writing mode value in terminal
    if(mode!=temp_mode){
    terminal.print("mode : ");
    terminal.println(mode);
    temp_mode=mode;
    }
    eyes_calling();
  }
}  
    //calling loop function for continous calling of blynk
void blynk_calling(){
  
 if(strcmp(cmd, "00") != 0 && (strcmp(cmd, "01") == 0 || strcmp(cmd, "02") == 0 || strcmp(cmd, "03") == 0)){
  int  distance = ultra();
    delay(100);
    Serial.println(distance);
  if( distance<30 && distance>0){
     
      if(distance>20  && strcmp(cmd, "02") == 0 ){
        moveright();
        noTone(buz);
    }
      else if(distance>20 && strcmp(cmd, "03") == 0){
        moveleft();
        noTone(buz);
      }
    else{
      stopMotors();
      if(strcmp(cmd, "01") == 0|| strcmp(cmd, "02") == 0 || strcmp(cmd, "03") == 0 )
      tone(buz,2000);
    }

    }

    else if( distance>30 ) {
      noTone(buz);
       if (strcmp(cmd, "01") == 0 && strcmp(cmd, "02") != 0 && strcmp(cmd, "03") != 0 ) {
        moveForward();
      }
      else if(strcmp(cmd, "01") != 0 && strcmp(cmd, "02") == 0 && strcmp(cmd, "03") != 0){
        moveright();
      }
      else if(strcmp(cmd, "01") != 0 && strcmp(cmd, "02") != 0 && strcmp(cmd, "03") == 0){
          moveleft();
           }
    }
    }
    else if(strcmp(cmd, "00") == 0){
      backward();
      noTone(buz);
      }
      else{
        stopMotors();
        noTone(buz);
      }
    
}





/* ---------- MAIN LOOP ---------- */

void loop() {
  server_udp();
if(mode==1){
    Blynk.run();
    if(power==1){
      blynk_calling();
      
    } 
    else{
        stopMotors();
        
      
    }
}
  else if(mode==2){
    blynk_calling();
  }
  else if(mode==0){
    stopMotors();
  }





    
    
}