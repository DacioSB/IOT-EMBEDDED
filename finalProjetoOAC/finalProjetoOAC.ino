/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include "DHT.h"
#include "Servo.h"
#define DHTPIN D3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
Servo sd1;
int umidade;
int temperatura;

#define SENSOR_MOVIMENTO D0
#define GATE_GARAGEM D1
#define ALARME D2
#define LUZ D5
#define PIN_LUZES_SALA D6
#define VENTUINHA D7

// Replace with your network credentials
const char* ssid     = "afranio";
const char* password = "123123123";

String valorTemp;

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String gateState = "off";
String lightState = "off";
String alarmeState = "off";

// Assign output variables to GPIO pins
//const int gate = D1;
//const int light = D6;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;



void setup() {
  Serial.begin(115200);
  pinMode(SENSOR_MOVIMENTO, INPUT);
  pinMode(ALARME, OUTPUT);
  pinMode(LUZ, OUTPUT);
  // Initialize the output variables as outputs
  pinMode(PIN_LUZES_SALA, OUTPUT);
  
  
  // Set outputs to LOW
  digitalWrite(LUZ, LOW);
  digitalWrite(ALARME, LOW);
  digitalWrite(PIN_LUZES_SALA, LOW);
  sd1.attach(GATE_GARAGEM);
  dht.begin();
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void medirTempUmi(){
  umidade = dht.readHumidity();
  temperatura = dht.readTemperature(false);

  if(isnan(temperatura) || isnan(umidade)){
   Serial.println("Falhou"); 
  }else{
    if(umidade >= 65){
      digitalWrite(D7, HIGH);
    }else {
      digitalWrite(D7, LOW);  
   }
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.print("°C");
    Serial.print("   ");
    Serial.print("Umidade: ");
    Serial.println(umidade);
   }

   valorTemp = "Temperatura: " + (String)temperatura;
   valorTemp += "-- Umidade: " + (String)umidade; 
}


void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      medirTempUmi();
      funcMovimento();
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("GPIO 5 on");
              gateState = "on";
              sd1.write(180);
            } else if (header.indexOf("GET /5/off") >= 0) {
              Serial.println("GPIO 5 off");
              gateState = "off";
              sd1.write(-180);
            } else if (header.indexOf("GET /4/on") >= 0) {
              Serial.println("GPIO 12 on");
              lightState = "on";
              digitalWrite(PIN_LUZES_SALA, HIGH);
            } else if (header.indexOf("GET /4/off") >= 0) {
              Serial.println("GPIO 12 off");
              lightState = "off";
              digitalWrite(PIN_LUZES_SALA, LOW);
            } else if (header.indexOf("GET /3/on") >= 0) {
              Serial.println("GPIO x on");
              alarmeState = "on";
              //digitalWrite(PIN_LUZES_SALA, HIGH);
            } else if (header.indexOf("GET /3/off") >= 0) {
              Serial.println("GPIO x off");
              alarmeState = "off";
              //digitalWrite(PIN_LUZES_SALA, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");

            client.println("<meta http-equiv='refresh' content='5; loop())'>");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP8266 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 5  
            client.println("<p>GPIO 5 - State " + gateState + "</p>");
            // If the output5State is off, it displays the ON button       
            if (gateState=="off") {
              client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 4  
            client.println("<p>GPIO 12 - State " + lightState + "</p>");
            // If the output4State is off, it displays the ON button       
            if (lightState=="off") {
              client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            client.println("<p>GPIO 12 - State " + alarmeState + "</p>");
            // If the output4State is off, it displays the ON button       
            if (alarmeState=="off") {
              client.println("<p><a href=\"/3/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/3/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            
            client.println("<h2>" + valorTemp + "</h2>");
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }

}


void funcMovimento(){
  int sinal = digitalRead(SENSOR_MOVIMENTO);

  if(sinal == HIGH){
    digitalWrite(LUZ, HIGH);
    
  }else{
    digitalWrite(LUZ, LOW);
  }
  if(alarmeState == "on" && sinal == HIGH){
      digitalWrite(ALARME, HIGH);
   }else if (alarmeState =="off"){
    digitalWrite(ALARME, LOW); 
   }
  
}
