/*********
  Dacio Bezerra
    
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

//WEB
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
const char* ssid = "********"; 
const char* password = "********";
ESP8266WebServer server(80);
WiFiServer servidor(80);
WiFiClient nodemcuClient;

//--- MQTT ---
#include <PubSubClient.h>
const char* mqtt_Broker = "broker.hivemq.com";
const char* mqtt_ClientID = "WL_Dacinho";
PubSubClient client(nodemcuClient);
const char* topicoLevel = "projUltra/waterLvl";

#define TRIGGER_PIN  5
#define ECHO_PIN     4
int porcentagem;


void setup() {
  Serial.begin(115200);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(D5, OUTPUT);
  
  conectarWifi();
  WiFi.mode(WIFI_STA);
  client.setServer(mqtt_Broker, 1883);
  
  server.on("/", waterLevel);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  if (!client.connected()) {
    reconectMQTT();
  }
  server.handleClient();
  waterLevel();
  publicarNoTopico();

}
void reconectMQTT(){
  while(!client.connected()){
    client.connect(mqtt_ClientID);
  }
  
}

//--PUBLICA (MQTT) Nivel da Agua
void publicarNoTopico() {
  client.publish(topicoLevel, String(porcentagem).c_str(), true);
}

void conectarWifi(){
  delay(10);
  Serial.println(""); //PULA UMA LINHA NA JANELA SERIAL
  Serial.println(""); //PULA UMA LINHA NA JANELA SERIAL
  Serial.print("Conectando a "); //ESCREVE O TEXTO NA SERIAL
  Serial.print(ssid); //ESCREVE O NOME DA REDE NA SERIAL
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(""); //PULA UMA LINHA NA JANELA SERIAL
  Serial.print("Conectado a rede sem fio "); //ESCREVE O TEXTO NA SERIAL
  Serial.println(ssid); //ESCREVE O NOME DA REDE NA SERIAL
  Serial.println(""); //PULA UMA LINHA NA JANELA SERIAL
  Serial.print("Got IP: ");  
  Serial.println(WiFi.localIP());
    
}
void waterLevel(){
  long duration, distance;
  digitalWrite(TRIGGER_PIN, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(TRIGGER_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration/2) / 29.1;
  if (isnan(distance)) {    
    Serial.println("Failed to read from Ultrassonic sensor!");
  }
  else {
    porcentagem = 100 - ((distance * 100)/80); // 80 = Altura do tanque
    server.send(200, "text/html", SendHTML(porcentagem));
    Serial.print(porcentagem);
    Serial.println(" %");
    if(porcentagem >= 80){
      digitalWrite(D5, LOW);
    }else{
      digitalWrite(D5, HIGH);
    }

  delay(1000);

  }
}
void handle_NotFound(){
    server.send(404, "text/plain", "Not found");
}

String SendHTML(int pct){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<link href=\"https://fonts.googleapis.com/css?family=Open+Sans:300,400,600\" rel=\"stylesheet\">\n";
  ptr +="<title>ESP8266 Water Level Report</title>\n";
  ptr +="<style>html { font-family: 'Open Sans', sans-serif; display: block; margin: 0px auto; text-align: center;color: #333333;}\n";
  ptr +="body{margin-top: 50px;}\n";
  ptr +="h1 {margin: 50px auto 30px;}\n";
  ptr +=".side-by-side{display: inline-block;vertical-align: middle;position: relative;}\n";
  ptr +=".water-icon{background-color: #3498db;width: 30px;height: 30px;border-radius: 50%;line-height: 36px;}\n";
  ptr +=".water-text{font-weight: 600;padding-left: 15px;font-size: 19px;width: 160px;text-align: left;}\n";
  ptr +=".water{font-weight: 300;font-size: 60px;color: #3498db;}\n";
  ptr +=".superscript{font-size: 17px;font-weight: 600;position: absolute;right: -20px;top: 15px;}\n";
  ptr +=".data{padding: 10px;}\n";
  ptr +="</style>\n";
  ptr +="<script>\n";
  ptr +="setInterval(loadDoc,200);\n";
  ptr +="function loadDoc() {\n";
  ptr +="var xhttp = new XMLHttpRequest();\n";
  ptr +="xhttp.onreadystatechange = function() {\n";
  ptr +="if (this.readyState == 4 && this.status == 200) {\n";
  ptr +="document.getElementById(\"webpage\").innerHTML =this.responseText}\n";
  ptr +="};\n";
  ptr +="xhttp.open(\"GET\", \"/\", true);\n";
  ptr +="xhttp.send();\n";
  ptr +="}\n";
  ptr +="</script>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>ESP8266 Water Level Report</h1>\n";
  ptr +="<div class=\"data\">\n";
  ptr +="<div class=\"side-by-side water-icon\">\n";
  ptr +="<svg version=\"1.1\" id=\"Layer_2\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" x=\"0px\" y=\"0px\"\n\"; width=\"12px\" height=\"17.955px\" viewBox=\"0 0 13 17.955\" enable-background=\"new 0 0 13 17.955\" xml:space=\"preserve\">\n";
  ptr +="<path fill=\"#FFFFFF\" d=\"M1.819,6.217C3.139,4.064,6.5,0,6.5,0s3.363,4.064,4.681,6.217c1.793,2.926,2.133,5.05,1.571,7.057\n";
  ptr +="c-0.438,1.574-2.264,4.681-6.252,4.681c-3.988,0-5.813-3.107-6.252-4.681C-0.313,11.267,0.026,9.143,1.819,6.217\"></path>\n";
  ptr +="</svg>\n";
  ptr +="</div>\n";
  ptr +="<div class=\"side-by-side water-text\">Water Level</div>\n";
  ptr +="<div class=\"side-by-side water\">";
  ptr += pct;
  ptr +="<span class=\"superscript\">%</span></div>\n";
  ptr +="</div>\n";

  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
 /* String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<html>\n";
  ptr += "<head>\n";
  ptr += "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\" />\n";
  ptr += "<script src='plotly.min.js'></script>\n";
  ptr += "<link rel=\"stylesheet\" type=\"text/css\" href=\"styles.css\">\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<div class='navbar'><span>Real-Time Chart with Plotly.js</span></div>\n";
  ptr += "<div class='wrapper'>\n";
  ptr += "<div id='chart'></div>\n";
  ptr += "<script>\n";
  ptr += "function getData() {\n";
  ptr += "return Math.random();\n";
  ptr += " }\n";
  ptr += "Plotly.plot('chart',[{\n";
  ptr += "y:[getData()],\n";
  ptr += "type:'line'\n";
  ptr += "}]);\n";
  ptr += "var cnt = 0;\n";
  ptr += "setInterval(function(){\n";
  ptr += "Plotly.extendTraces('chart',{ y:[[getData()]]}, [0]);\n";
  ptr += "cnt++;\n";
  ptr += "if(cnt > 500) {\n";
  ptr += "Plotly.relayout('chart',{\n";
  ptr += "xaxis: {\n";
  ptr += "range: [cnt-500,cnt]\n";
  ptr += "}\n";
  ptr += "});\n";
  ptr += "}\n";
  ptr += "},1000);\n";
  ptr += "</script>\n";
  ptr += "</div>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;*/
  /*
  ptr += "<head>\n";
  ptr += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  ptr += "<script src= 'https://code.highcharts.com/highcharts.js'></script>\n";
  ptr += "<title>ESP8266 LEVEL WATER Report</title>\n";
  ptr += "<style>\n";
  ptr += "body {\n";
  ptr += "min-width: 310px;\n";
  ptr += "max-width: 800px;\n";
  ptr += "height: 400px;\n";
  ptr += "margin: 0 auto;\n";
  ptr += "}\n";
  ptr += "h2 {\n";
  ptr += "font-family: Arial;\n";
  ptr += "font-size: 2.5rem;\n";
  ptr += "text-align: center;\n";
  ptr += "}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h2>ESP Water Height Station</h2>\n";
  ptr += "<div id=\"chart-water\" class=\"container\"></div>\n";
  ptr += "</body>\n";
  ptr += "<script>\n";
  ptr += "var chartT = new Highcharts.Chart({\n";
  ptr += "chart:{ renderTo : 'chart-water' },\n";
  ptr += "title: { text: 'Sensor Altura de Agua (Tanque)' },\n";
  ptr += "series: [{\n";
  ptr += "showInLegend: false,\n";
  ptr += "data: []\n";
  ptr += "}],\n";
  ptr += "plotOptions: {\n";
  ptr += "line: { animation: false,\n";
  ptr += "dataLabels: { enabled: true }\n";
  ptr += "},\n";
  ptr += "series: { color: '#059e8a' }\n";
  ptr += "},\n";
  ptr += "xAxis: { type: 'datetime',\n";
  ptr += "dateTimeLabelFormats: { second: '%H:%M:%S' }\n";
  ptr += "},\n";
  ptr += "yAxis: {\n";
  ptr += "title: { text: 'Porcentagem (%)' }\n";
    //title: { text: 'Temperature (Fahrenheit)' }
  ptr += "},\n";
  ptr += "credits: { enabled: false }\n";
  ptr += "});\n";
  ptr += "setInterval(function ( ) {\n";
  ptr += "var xhttp = new XMLHttpRequest();\n";
  ptr += "xhttp.onreadystatechange = function() {\n";
  ptr += "if (this.readyState == 4 && this.status == 200) {\n";
  ptr += "var x = (new Date()).getTime(),\n";
  ptr += "y = parseFloat(" + pct + ");\n";
      //console.log(this.responseText);
  ptr += "if(chartT.series[0].data.length > 40) {\n";
  ptr += "chartT.series[0].addPoint([x, y], true, true, true);\n";
  ptr += "} else {\n";
  ptr += "chartT.series[0].addPoint([x, y], true, false, true);\n";
  ptr += "}\n";
  ptr += "}\n";
  ptr += "};\n";
  ptr += "xhttp.open(\"GET\",\"/\", true);\n";
  ptr += "xhttp.send();\n";
  ptr += "}, 1000 ) ;\n";
  ptr += "</script>\n";
  ptr += "</html>\n";
  return ptr;*/
}
