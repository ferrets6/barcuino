#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

// PIN declaration
#define LEFT_MOTOR D1
#define RIGHT_MOTOR D5
#define EJECT_SERVO D8

#define MAX_ANALOG_WRITE 1023

// Global variables
bool motorsEnabled = false; // flag to avoid motor activation
int step = 1;
String serializedJSON;

// functions declaration
void ascentCycle(uint8_t motor, bool debug, int minValue, int step);
void descentCycle(uint8_t motor, bool debug, int minValue, int step);
String setMotorsSpeed(int left, int right);
void handleSerialDataReceived(String serialData);
void handleDataReceived(char *dataStr);
void serialFlush();
String getValue(String data, char separator, int index);

// const
const String ssid = "Casa Crinella 2.4 GHz";
const String password = "unapasswordmoltocomplicata";
WiFiServer wifiServer(80);

void setup()
{
  // put your setup code here, to run once:

  // set pinMode
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RIGHT_MOTOR, OUTPUT);
  pinMode(LEFT_MOTOR, OUTPUT);
  pinMode(EJECT_SERVO, OUTPUT);

  // initialize pins values
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(RIGHT_MOTOR, LOW);
  digitalWrite(LEFT_MOTOR, LOW);
  digitalWrite(EJECT_SERVO, LOW);

  // Start the Serial communication to send messages to the computer
  Serial.begin(115200); 
  delay(100);
  Serial.println();
  Serial.println("Starting...");

  // Start WiFi Server
  // WiFi.begin(ssid, password);
  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP("BarkiFi", "ciaociao") ? "Ready" : "Failed!");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting..");
  }

  Serial.print("Connected to WiFi. IP:");
  Serial.println(WiFi.localIP());

  wifiServer.begin();
}

void loop()
{
  // Serial.println("********************");
  // Serial.println("BEGIN");
  // Serial.println("waiting for command...");

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
    Serial.println("Client disconnected");
    
  }


  // // send data only when you receive data:
  // while (!Serial.available())
  // {
    
  // }

  // handleSerialDataReceived();
  // serialFlush();

  // Serial.println("END");
  // Serial.println("********************");
}

void testLoop()
{
  String serialMessage = "";
  Serial.println("********************");
  Serial.println("loop begin\r\n");
  // put your main code here, to run repeatedly:

  // try PWN on motors
  Serial.println("Start RIGHT ascent cycle");
  ascentCycle(RIGHT_MOTOR, true, 0, step);
  delay(1500);

  Serial.println("Start RIGHT descent cycle");
  descentCycle(RIGHT_MOTOR, true, 0, step);
  delay(1000);

  // try PWN on motors
  Serial.println("Start LEFT ascent cycle");
  ascentCycle(LEFT_MOTOR, true, 0, step);
  delay(1500);

  Serial.println("Start LEFT descent cycle");
  descentCycle(LEFT_MOTOR, true, 0, step);
  delay(1000);

  Serial.println("\r\nloop end");
  Serial.println("********************");
}

String setMotorsSpeed(int left, int right)
{
  if ((0 <= left && left <= MAX_ANALOG_WRITE) && (0 <= right && right <= MAX_ANALOG_WRITE)) {
    analogWrite(LEFT_MOTOR, left);
    analogWrite(RIGHT_MOTOR, right);
    return "OK";
  }
  Serial.println("Not valid values");
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
  else if (command == "stopMotors")
  {
    setMotorsSpeed(0, 0);
  }
  else if (command == "ejectPastura")
  {
    // todo
    Serial.println("Ejecting Pastura! Yeeeeeee!");
  }
  else
  {
    Serial.println("No valid command");
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