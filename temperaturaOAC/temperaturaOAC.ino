#include "DHT.h"
#include "Servo.h"
#define DHTPIN D3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
Servo sd1;

void serv() {
  // put your main code here, to run repeatedly:
  sd1.write(180);
  delay(1000);
  sd1.write(-180);
  delay(1000);
}

//void abrir(){
  //sd1.write(180);

//}
//void fechar(){
  //sd1.write(-180);

//}
int umidade;
int temperatura;


void setup() {
  sd1.attach(D1);
  Serial.begin(115200);
  dht.begin();
  pinMode(D2, OUTPUT);
  
}
void loop(){
  medirTempUmi();
  serv();
  delay(1000);
}
void medirTempUmi(){
  umidade = dht.readHumidity();
  temperatura = dht.readTemperature(false);
  
  if(isnan(temperatura) || isnan(umidade)){
   Serial.println("Falhou"); 
  }else{
    if(temperatura >= 25){
      digitalWrite(D2, HIGH);
    }else {
      digitalWrite(D2, LOW);  
    }
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.print("°C");
    Serial.print("   ");
    Serial.print("Umidade: ");
    Serial.println(umidade);
   }
}
