#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// ================= WIFI =================
const char* ssid = "WIFI_NAME";
const char* password = "WIFI_PASSWORD";

WebServer server(80);

// ================= FUNCTION PROTOTYPES =================
void handleRoot();
void handleMove();
void handleServo();
void handleMode();
void emergencyStop();

// ================= MOTORS =================
const int in1 = 2;
const int in2 = 15;
const int enA = 4;

const int in3 = 27;
const int in4 = 26;
const int enB = 25;

// ================= SERVOS =================
Servo s1, s2, s3, s4;

const int p1 = 19;
const int p2 = 21;
const int p3 = 22;
const int p4 = 23;

// Safe initial values (YOU manually adjust later)
int pos1 = 180;
int pos2 = 90;
int pos3 = 90;
int pos4 = 45;

// smoothing helper
int last3 = 90;

const int stepSize = 2;

// ================= MODES =================
enum Mode {
  DRIVE_MODE,
  ARM_MODE
};

Mode mode = DRIVE_MODE;

// ================= MOTOR CONTROL =================
void setMotors(int left, int right) {

  // LEFT
  if (left > 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(enA, HIGH);
  } else if (left < 0) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(enA, HIGH);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(enA, LOW);
  }

  // RIGHT
  if (right > 0) {
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    digitalWrite(enB, HIGH);
  } else if (right < 0) {
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    digitalWrite(enB, HIGH);
  } else {
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
    digitalWrite(enB, LOW);
  }
}

// ================= EMERGENCY STOP =================
void emergencyStop() {
  setMotors(0,0);

  // optional safety micro-reset for servos (prevents runaway jitter)
  delay(50);
}

// ================= ROOT PAGE =================
const char page[] PROGMEM = R"rawliteral(

<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">

<style>
*{
  user-select:none;
  -webkit-user-select:none;
  -ms-user-select:none;
  -webkit-tap-highlight-color:transparent;
  touch-action:manipulation;
}

body{
  font-family:Arial;
  background:#111;
  color:white;
  text-align:center;
}

button{
  width:120px;
  height:55px;
  margin:6px;
  font-size:18px;
  border:none;
  border-radius:10px;
  background:#444;
  color:white;
}

button:active{
  background:#00b7ff;
}
</style>

<script>

let x=0,y=0;
let driveTimer=null;

// ================= DRIVE =================
function press(axis,val,e){
  e.preventDefault();

  if(axis=='x') x=val;
  if(axis=='y') y=val;

  if(!driveTimer){
    driveTimer=setInterval(()=>{
      fetch(`/move?x=${x}&y=${y}`);
    },120);
  }
}

function release(e){
  e.preventDefault();

  x=0;y=0;

  clearInterval(driveTimer);
  driveTimer=null;

  fetch(`/move?x=0&y=0`);
}

// ================= SERVO =================
let servoTimer=null;
let sid=0;
let sdir="";

function servoDown(id,dir,e){
  e.preventDefault();

  sid=id;
  sdir=dir;

  fetch(`/servo?id=${id}&dir=${dir}`);

  servoTimer=setInterval(()=>{
    fetch(`/servo?id=${sid}&dir=${sdir}`);
  },180);
}

function servoUp(e){
  e.preventDefault();
  clearInterval(servoTimer);
  servoTimer=null;
}

// ================= MODE =================
function mode(m){
  fetch(`/mode?mode=${m}`);
}

// ================= EMERGENCY =================
function stopAll(){
  fetch(`/stop`);
}

</script>
</head>

<body>

<h2>🚗 ROBOT CONTROL</h2>

<h3>Mode</h3>
<button onclick="mode('drive')">🚗 DRIVE</button>
<button onclick="mode('arm')">🤖 ARM</button>

<button onclick="stopAll()" style="background:#ff4444;">🛑 STOP</button>

<h3>Drive</h3>
<button onpointerdown="press('y',1,event)" onpointerup="release(event)">FWD</button>
<button onpointerdown="press('y',-1,event)" onpointerup="release(event)">REV</button>

<button onpointerdown="press('x',-1,event)" onpointerup="release(event)">LEFT</button>
<button onpointerdown="press('x',1,event)" onpointerup="release(event)">RIGHT</button>

<h3>Claw (always active)</h3>
<button onpointerdown="servoDown(4,'up',event)" onpointerup="servoUp(event)">OPEN</button>
<button onpointerdown="servoDown(4,'down',event)" onpointerup="servoUp(event)">CLOSE</button>

<h3>Arm (ARM MODE ONLY)</h3>
<button onpointerdown="servoDown(1,'up',event)" onpointerup="servoUp(event)">S1+</button>
<button onpointerdown="servoDown(1,'down',event)" onpointerup="servoUp(event)">S1-</button>

<button onpointerdown="servoDown(2,'up',event)" onpointerup="servoUp(event)">S2+</button>
<button onpointerdown="servoDown(2,'down',event)" onpointerup="servoUp(event)">S2-</button>

<button onpointerdown="servoDown(3,'up',event)" onpointerup="servoUp(event)">S3+</button>
<button onpointerdown="servoDown(3,'down',event)" onpointerup="servoUp(event)">S3-</button>

</body>
</html>

)rawliteral";

// ================= HANDLERS =================
void handleRoot() {
  server.send_P(200, "text/html", page);
}

// ================= MOVE =================
void handleMove() {

  if (mode != DRIVE_MODE) {
    server.send(200,"text/plain","ARM LOCKED");
    return;
  }

  int x = server.arg("x").toInt();
  int y = server.arg("y").toInt();

  int left = y;
  int right = y;

  if (x == -1) { left = y - 1; right = y + 1; }
  if (x == 1)  { left = y + 1; right = y - 1; }

  setMotors(constrain(left,-1,1), constrain(right,-1,1));

  server.send(200,"text/plain","OK");
}

// ================= SERVO =================
void handleServo() {

  int id = server.arg("id").toInt();
  String dir = server.arg("dir");

  int step = (dir == "up") ? stepSize : -stepSize;

  // CLAW ALWAYS ACTIVE
  if (id == 4) {
    pos4 = constrain(pos4 + step, 10, 90);
    s4.write(pos4);
    server.send(200,"text/plain","OK");
    return;
  }

  // ARM ONLY IN ARM MODE
  if (mode != ARM_MODE) {
    server.send(200,"text/plain","LOCKED");
    return;
  }

  if (id == 1) {
    pos1 = constrain(pos1 + step, 0, 180);
    s1.write(pos1);
  }

  if (id == 2) {
    pos2 = constrain(pos2 + step, 0, 180);
    s2.write(pos2);
  }

  if (id == 3) {
    pos3 = constrain(pos3 + step, 0, 180);

    if (abs(pos3 - last3) > 2) {
      s3.write(pos3);
      last3 = pos3;
    }
  }

  server.send(200,"text/plain","OK");
}

// ================= MODE =================
void handleMode() {

  String m = server.arg("mode");

  if (m == "drive") {
    mode = DRIVE_MODE;
    setMotors(0,0);
  }

  if (m == "arm") {
    mode = ARM_MODE;
    setMotors(0,0);
  }

  server.send(200,"text/plain","OK");
}

// ================= STOP =================
void handleStop() {
  emergencyStop();
  server.send(200,"text/plain","STOPPED");
}

// ================= SETUP =================
void setup() {

  Serial.begin(115200);

  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT);
  pinMode(in3,OUTPUT);
  pinMode(in4,OUTPUT);
  pinMode(enA,OUTPUT);
  pinMode(enB,OUTPUT);

  setMotors(0,0);

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  s1.attach(p1,500,2400);
  s2.attach(p2,500,2400);
  s3.attach(p3,500,2400);
  s4.attach(p4,500,2400);

  delay(600);

  s1.write(pos1);
  s2.write(pos2);
  s3.write(pos3);
  s4.write(pos4);

  WiFi.mode(WIFI_AP);
  WiFi.setSleep(false);
  WiFi.softAP(ssid,password);

  server.on("/",handleRoot);
  server.on("/move",handleMove);
  server.on("/servo",handleServo);
  server.on("/mode",handleMode);
  server.on("/stop",handleStop);

  server.begin();
}

// ================= LOOP =================
void loop() {
  server.handleClient();
}
