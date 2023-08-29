#include <Wire.h>
#define escravo 0x04

int temperatura = 25;
int umidade = 60;
String Concatenado;




void setup() {
  Serial.begin(9600);
  Serial.println("Ligado");
  Wire.begin(escravo);
  Wire.onRequest(Requisitado);

}

void loop() {
  // put your main code here, to run repeatedly:

}
void Requisitado()
{
  Serial.println("Requisitado");
  Wire.write(char(temperatura));
  Wire.write(char(umidade));

if(temperatura < 18)
{
  temperatura = random((temperatura+1), (temperatura+2));
} else if (temperatura >22){
  temperatura = random((temperatura-2), (temperatura));
} else {
  temperatura = random((temperatura-1), (temperatura+2));
}

if(temperatura < 30)
{
  umidade = random(umidade+1, (umidade+2));
} else if (umidade >60){
  umidade = random((umidade-2), umidade);
} else {
  umidade = random((umidade-1), (umidade+2));
}


  Serial.println("Enviado");
}
