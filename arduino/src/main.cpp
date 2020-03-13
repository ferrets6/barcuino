#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <Servo.h>
// #include "motors.cpp"

// PIN declaration
#define LEFT_MOTOR D1
#define RIGHT_MOTOR D5
#define EJECT_SERVO D8

#define MAX_ANALOG_WRITE 1023

// Global variables
bool motorsEnabled = false; // flag to avoid motor activation
int step = 1;
String serializedJSON;

Servo ejectServo;

// functions declaration
void ascentCycle(uint8_t motor, bool debug, int minValue, int step);
void descentCycle(uint8_t motor, bool debug, int minValue, int step);
void stopMotors();
String setMotorsSpeed(int left, int right);
void handleSerialDataReceived(String serialData);
void handleDataReceived(char *dataStr);
void serialFlush();
String getValue(String data, char separator, int index);
String ejectPastura();
int getLeftMotorValue(double degrees);
int getRightMotorValue(double degrees);

// const
const String ssid = "Casa Crinella 2.4 GHz";
const String password = "unapasswordmoltocomplicata";

double maxSpeed = 1023;
double maxTurningSpeed = 511;
WiFiServer wifiServer(80);

void setup()
{
  // put your setup code here, to run once:

  // set pinMode
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RIGHT_MOTOR, OUTPUT);
  pinMode(LEFT_MOTOR, OUTPUT);

  // initialize pins values
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(RIGHT_MOTOR, LOW);
  digitalWrite(LEFT_MOTOR, LOW);
  // initialize servo
  ejectServo.attach(EJECT_SERVO);
  delay(15);
  ejectServo.write(0);

  // Start the Serial communication to send messages to the computer
  Serial.begin(115200); 
  delay(3000);

  // Start WiFi Server
  // WiFi.begin(ssid, password);
  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP("BarkiFi", "ciaociao") ? "Ready" : "Failed!");

  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(1000);
  //   Serial.println("Connecting..");
  // }

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  // Serial.print("Connected to WiFi. IP:");
  // Serial.println(WiFi.localIP());

  wifiServer.begin();
}

void loop() {

  WiFiClient client = wifiServer.available();
  String command = "";

  if (client) {

    while (client.connected()) {

      while (client.available()>0) {
        char c = client.read();
        if (c == '\n') {
          break;
        }
        command += c;
        Serial.write(c);
      }

      if (command != "") {
        handleSerialDataReceived(command);
      }

      command = "";
      delay(10);
    }

    client.stop();
    stopMotors();
    Serial.println("Client disconnected");
    
  }
}

// void loop()
// {
//   // Serial.println("********************");
//   // Serial.println("BEGIN");
//   // Serial.println("waiting for command...");

//   WiFiClient client = wifiServer.available();
//   String command = "";

//   if (client) {
//     while (client.connected()) {
//       while (client.available()>0) {
//         char c = client.read();
//         if (c == '\n') {
//           break;
//         }
//         command += c;
//         Serial.write(c);
//       }
      
//       if (command != "") {
//         handleSerialDataReceived(command);
//       }

//       command = "";
//       delay(10);
//     }

//     client.stop();
//     Serial.println("Client disconnected");
    
//   }


//   // // send data only when you receive data:
//   // while (!Serial.available())
//   // {
    
//   // }

//   // handleSerialDataReceived();
//   // serialFlush();

//   // Serial.println("END");
//   // Serial.println("********************");
// }




int getLeftMotorValue(double degrees)
{
  if (degrees >= 0 && degrees <= 90)
  {
    return (int) maxSpeed * (1 - (degrees / (90 * (maxSpeed / maxTurningSpeed))));
  }
  else if (degrees >= 270 && degrees <= 360)
  {
    return (int) maxSpeed * (1 - ((360 - degrees) / 90));
  }
  else
  {
    return 0;
  }
}

int getRightMotorValue(double degrees)
{
  if (degrees >= 0 && degrees <= 90)
  {
    return (int) maxSpeed * (1 - (degrees / 90));
  }
  else if (degrees >= 270 && degrees <= 360)
  {
    return (int) maxSpeed * (1 - ((360 - degrees) / (90 * (maxSpeed / maxTurningSpeed))));
  }
  else
  {
    return 0;
  }
}

String setMotorsSpeedFromPad(double degrees, double distance)
{
  int left = getLeftMotorValue(degrees);
  int right = getRightMotorValue(degrees);
  Serial.print("SX: ");
  Serial.println(left);
  Serial.print("DX: ");
  Serial.println(right);
  Serial.print("Distance: ");
  Serial.println(distance);

  if (distance > 0) {
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

void handleSerialDataReceived(String serialData)
{
  // String serialData = Serial.readStringUntil('!');
  serialData.trim();
  Serial.println(serialData);

  // command is at pos 0
  String command = getValue(serialData, ';', 0);
  Serial.println(command);

  if (command == "setMotorsSpeed")
  {
    String leftCommand = getValue(serialData, ';', 1);
    String rightCommand = getValue(serialData, ';', 2);

    int left = leftCommand.toInt();
    int right = rightCommand.toInt();

    setMotorsSpeed(left, right);
  }
  else if (command == "setMotorsSpeedFromPad")
  {
    String leftCommand = getValue(serialData, ';', 1);
    String rightCommand = getValue(serialData, ';', 2);

    double degrees = leftCommand.toDouble();
    double distance = rightCommand.toDouble();

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
  else
  {
    // Serial.println("No valid command");
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

void handleJsonDataReceived(char *dataStr)
{
  serializedJSON = "";
  DynamicJsonDocument reqDoc(1024);
  deserializeJson(reqDoc, dataStr);

  String command = reqDoc["command"];
  Serial.print("json ");
  Serial.println(command);

  if (command == "setMotorsSpeed")
  {
    String leftCommand = reqDoc["left"];
    String rightCommand = reqDoc["right"];

    int left = leftCommand.toInt();
    int right = rightCommand.toInt();

    setMotorsSpeed(left, right);
  }

  if (command == "ejectPastura")
  {
  }
}

void ascentCycle(uint8_t motor, bool debug, int minValue = 0, int step = 1)
{
  for (int i = minValue; i <= MAX_ANALOG_WRITE; i++)
  {
    if (i == MAX_ANALOG_WRITE || i == minValue || (i % step) == 0)
    {
      if (debug)
      {
        Serial.print("i: ");
        Serial.print(i);
        Serial.println();
      }
      if (motorsEnabled == true)
      {
        analogWrite(motor, i);
      }
    }
    delay(10);
  }
}

void descentCycle(uint8_t motor, bool debug, int minValue = 0, int step = 1)
{
  for (int i = MAX_ANALOG_WRITE; i > minValue; i--)
  {
    if (i == MAX_ANALOG_WRITE || i == minValue || (i % step) == 0)
    {
      if (debug)
      {
        Serial.print("i: ");
        Serial.print(i);
        Serial.println();
      }
      if (motorsEnabled == true)
      {
        analogWrite(motor, i);
      }
    }
    delay(10);
  }
}