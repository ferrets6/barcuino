#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Servo.h>
#include <LedController.h>
#include <math.h>
#include <DallasTemperature.h>

// PIN declaration
#define LEFT_MOTOR D8
#define RIGHT_MOTOR D7
#define EJECT_SERVO D4

#define LED_RGB_RED D1
#define LED_RGB_GREEN D6
#define LED_RGB_BLUE D5
#define LED_BACK D0

#define TEMP_SENSORS_BUS D3

// temp sensor
OneWire oneWire(TEMP_SENSORS_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress tempSensor1 = { 0x28, 0xAA, 0x2C, 0xCA, 0x4F, 0x14, 0x01, 0x91 };
DeviceAddress tempSensor2 = { 0x28, 0xAA, 0xD8, 0xDD, 0x4F, 0x14, 0x01, 0x96 };

int tempSensorResolution = 10;

#define MAX_ANALOG_WRITE 1023

ESP8266WebServer server;
WebSocketsServer webSocket = WebSocketsServer(81);

// Global variables
char commandSeparator = ';';
Servo ejectServo;
LedController ledRgbRed;
LedController ledRgbBlue;
LedController ledRgbGreen;
LedController ledBack;

double maxSpeed = 1023;
double minMotorSpeed = 200;  // sotto questa velocità i motori fischiano ma non si muove
double maxTurningSpeed = 1023;
// WiFiServer wifiServer(80);

// functions declaration
void stopMotors();
String setMotorsSpeed(int left, int right);
void handleDataReceived(char *dataStr);
void serialFlush();
String getValue(String data, char separator, int index);
String ejectPastura();
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);

String myPassword = "ciaociao";
String mySsid = "BarkiFi";

IPAddress local_ip(192,168,1,4);
IPAddress gateway(192,168,1,1);
IPAddress netmask(255,255,255,0);

char webpage[] PROGMEM = R"=====(
<html>
<head>
  <script>
    var Socket;
    function init() {
      Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
      Socket.onmessage = function(event){
        document.getElementById("rxConsole").value += event.data;
      }
    }
    function sendText(){
      Socket.send(document.getElementById("txBar").value);
      document.getElementById("txBar").value = "";
    }
    function sendBrightness(){
      Socket.send("#"+document.getElementById("brightness").value);
    }    
  </script>
</head>
<body onload="javascript:init()">
  <div>
    <textarea id="rxConsole"></textarea>
  </div>
  <hr/>
  <div>
    <input type="text" id="txBar" onkeydown="if(event.keyCode == 13) sendText();" />
  </div>
  <hr/>
  <div>
    <input type="range" min="0" max="1023" value="512" id="brightness" oninput="sendBrightness()" />
  </div>  
</body>
</html>
)=====";

void setup()
{
  // put your setup code here, to run once:

  // set pinMode
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RIGHT_MOTOR, OUTPUT);
  pinMode(LEFT_MOTOR, OUTPUT);

  // initialize pins values
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(RIGHT_MOTOR, LOW);
  digitalWrite(LEFT_MOTOR, LOW);

  // create leds
  ledBack.attach(LED_BACK, UNDEFINED);
  ledRgbBlue.attach(LED_RGB_BLUE, BLUE);
  ledRgbRed.attach(LED_RGB_RED, RED);
  ledRgbGreen.attach(LED_RGB_GREEN, GREEN);

  // initialize servo
  ejectServo.attach(EJECT_SERVO);
  delay(15);
  ejectServo.write(0);

  // initialize sensors and set resolution
  sensors.begin();
  sensors.setResolution(tempSensor1, tempSensorResolution);
  sensors.setResolution(tempSensor2, tempSensorResolution);

  // Start the Serial communication to send messages to the computer
  Serial.begin(115200); 
  delay(5000);

  server.on("/", [] () {
    server.send_P(200, "text/html", webpage);  
  });
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.mode(WIFI_AP);
  // WiFi.softAPConfig(local_ip, gateway, netmask);
  WiFi.softAP(mySsid, myPassword);

  // setup finished, switch on red led
  ledRgbRed.on();
}

void loop()
{
  webSocket.loop();
  server.handleClient();
  if(Serial.available() > 0){
    char c[] = {(char)Serial.read()};
    webSocket.broadcastTXT(c, sizeof(c));
  }
}

double absPro(double x)
{
  if (x > 0)
  {
    return x;
  }
  else
  {
    return -x;
  }
}

int getLeftMotorValueNew(double degrees, double distance)
{
  double result = 0;
  if (degrees >= 0 && degrees <= 180) {
    result = maxSpeed;
  }
  else {
    result = maxSpeed * absPro(cos(radians(degrees)));
  }
  return (int) (result * distance) * (1 - minMotorSpeed / maxSpeed) + minMotorSpeed;
}

int getRightMotorValueNew(double degrees, double distance)
{
  double result = 0;
  if (degrees >= 0 && degrees <= 180) {
    result = absPro(cos(radians(degrees)));
  }
  else {
    result = maxSpeed;
  }
  return (int) (result * distance) * (1 - minMotorSpeed / maxSpeed) + minMotorSpeed;
}

String setMotorsSpeedFromPad(double degrees, double distance)
{
  if (distance > 0) {
    int left = getLeftMotorValueNew(degrees, distance);
    int right = getRightMotorValueNew(degrees, distance);
    Serial.print("degrees: "); Serial.println(degrees);
    Serial.print("SX: "); Serial.println(left);
    Serial.print("DX: "); Serial.println(right);
    Serial.print("Distance: "); Serial.println(distance);

    setMotorsSpeed(left, right);
    return "OK";
  }
  else {
    stopMotors();
    Serial.println("Distance 0. Motors stopped");
    return "Distance 0. Motors stopped";
  }
}

String setMotorsSpeed(int left, int right)
{
  if ((0 <= left && left <= MAX_ANALOG_WRITE) && (0 <= right && right <= MAX_ANALOG_WRITE)) {
    analogWrite(LEFT_MOTOR, left);
    analogWrite(RIGHT_MOTOR, right);
    return "OK";
  }
  else {
    Serial.println("Not valid values");
    return "Error";
  }
}

void stopMotors() {
  setMotorsSpeed(0,0);
}

String ejectPastura() {
  if(ejectServo.attached()) {
    ejectServo.write(90);
    delay(500);
    ejectServo.write(0);
    Serial.println("Pastura ejected");
    return "OK";
  }
  else {
    Serial.println("Servo not attached!");
    return "Error!";
  }
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void serialFlush()
{
  while (Serial.available() > 0)
  {
    Serial.read();
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_CONNECTED)
  {
    char payload[] = {"Hi! My name is Barkino."};
    webSocket.broadcastTXT(payload, sizeof(payload));
  }
  else if (type == WStype_DISCONNECTED)
  {
    Serial.println("WebSocket client disconnected, stopping motors");
    stopMotors();
  }
  else if (type == WStype_ERROR) {
    Serial.println("WebSocket client error, stopping motors");
    stopMotors();
  }
  else if (type == WStype_TEXT) {
    String serialData = String((char *)payload);
    if (serialData.charAt(0) == '#') {
      serialData = serialData.substring(1);
      // uint16_t command = (uint16_t) strtol((const char *) &payload[1], NULL, 10);
      // String command = (String) strtol((const char *) &payload[0], NULL, 10);
      
      // String serialData = Serial.readStringUntil('!');
      serialData.trim();
      Serial.println("****************************");
      Serial.println(serialData);

      // command is at pos 0
      String command = getValue(serialData, commandSeparator, 0);
      Serial.println(command);

      if (command == "setMotorsSpeed")
      {
        String leftCommand = getValue(serialData, commandSeparator, 1);
        String rightCommand = getValue(serialData, commandSeparator, 2);

        int left = leftCommand.toInt();
        int right = rightCommand.toInt();

        setMotorsSpeed(left, right);
      }
      else if (command == "setMotorsSpeedFromPad")
      {
        String degreesCmd = getValue(serialData, commandSeparator, 1);
        String distanceCmd = getValue(serialData, commandSeparator, 2);

        double degrees = degreesCmd.toDouble();
        double distance = distanceCmd.toDouble();

        setMotorsSpeedFromPad(degrees, distance);
      }
      else if (command == "stopMotors")
      {
        setMotorsSpeed(0, 0);
      }
      else if (command == "ejectPastura")
      {
        ejectPastura();
      }
      else if (command == "led")
      {
        String type = getValue(serialData, commandSeparator, 1);
        String intensityCmd = getValue(serialData, commandSeparator, 2);
        int intensity = intensityCmd != "" ? intensityCmd.toInt() : -1;

        if (type == "green")
        {
          intensity != -1 ? ledRgbGreen.setIntensity(intensity) : ledRgbGreen.toggle();
        }
        else if (type == "red")
        {
          intensity != -1 ? ledRgbRed.setIntensity(intensity) : ledRgbRed.toggle();
        }
        else if (type == "blue")
        {
          intensity != -1 ? ledRgbBlue.setIntensity(intensity) : ledRgbBlue.toggle();
        }
        else if (type == "back")
        {
          intensity != -1 ? ledBack.setIntensity(intensity) : ledBack.toggle();
        }
        else if (type == "off")
        {
          ledBack.off();
          ledRgbRed.off();
          ledRgbGreen.off();
          ledRgbBlue.off();
          Serial.println("Switched off!");
        }
        else if (type == "on")
        {
          ledBack.on();
          ledRgbRed.on();
          ledRgbGreen.on();
          ledRgbBlue.on();
          Serial.println("Switched on!");
        }
      }
      else if (command == "sensors") {
        bool goOn = true;
        String type = getValue(serialData, commandSeparator, 1);
        uint8_t* sensor; // selected sensor address
        String result;

        if (type == "1")
          sensor = tempSensor1;
        else if (type == "2")
          sensor = tempSensor2;
        else {
            // command error
            result = "Sensor type not found!";
            goOn = false;
        }

        if (goOn) {
          String function = getValue(serialData, commandSeparator, 2); //getTemp or setRes
          if (function == "getTemp") {
            sensors.requestTemperaturesByAddress(sensor);
            float temp = sensors.getTempC(sensor);
            result = '#' + String(temp);
          }
          else if (function == "setRes") {
            String value = getValue(serialData, commandSeparator, 3); // value for setRes
            int newResolution = value.toInt();

            if (newResolution >= 9 && newResolution <= 11) {
              sensors.setResolution(sensor, newResolution);
              Serial.print("Resolution set to: ");Serial.println(newResolution);
              result = "Ok!";
            }
            else {
              // resolution not supported
              result = "Resolution not supported!";
              goOn = false;
            }
          }
          else {
              // function error
              result = "Function not valid!";
              goOn = false;
          }
        }
        Serial.println(result);
        webSocket.broadcastTXT(result);
      }
      else
      {
        Serial.println("No valid command");
      }
    }
  }
  
}


