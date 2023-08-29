
#include <Wire.h> //Inclusão da biblioteca I2C da plataforma Arduino

#define endmin 0x01 //Endereço inicial da varredura automática
#define endmax 0x08 //Endereço final da varredura automática

class SensorI2C {

  public:
    SensorI2C(byte endereco) : Endereco(endereco) {} //Método construtor

    void atualizar() { //Método que atualiza um determinado objeto

      String data;                    //String que gardará a os dados obtidos pelo I2C
      Wire.requestFrom(Endereco, 2);  //Requisição dos dados enderaço do sensor associado ao objeto

      delay(100);

      while (Wire.available()) //Lógia para leitura dos dados vindos do barramento I2C
      {
        data += (int)Wire.read();
      }

      temperatura = data.substring(0, 2); //Os primeiros dois bytes se referem a temperatura
      umidade = data.substring(2, 4);     //O dois ultimos bytes se referem a umidade

      //Serial.println(data);
    }

    String obterTemperatura() { //método que retorna a temperatura
      return temperatura;
    }
    String obterUmidade() {    //método que retorna a umidade
      return umidade;
    }

    static void detectarSensores (byte endMin, byte endMax, byte* sensores, int& numSensores ) { //Método que detecta automaticamente dispositivos conectados no intervalo selecionado

      numSensores = 0;  //variável que acumulará o número de sensores descobertos

      for (byte endereco = endMin; endereco <= endMax; endereco++) { //Lógica de varredura dos endereços

        Wire.beginTransmission(endereco);
        boolean conectado = Wire.endTransmission();

        if (!conectado) {
          Serial.print("Sensor I2C detectado no endereço 0x");
          Serial.println(endereco, HEX);
          sensores[numSensores] = endereco;
          numSensores++;

        }
      }
    }

  private:
    byte Endereco;       //Endereco do sensor associado ao objeto
    String temperatura;  //Temperatura do sensor
    String umidade;      //Umidade do sensor
};

byte Enderecos[(endmax - endmin + 1)];

int sensoresEncontrados = 0;
int stat = 0;
int Comando = 0;
int IndiceMonitorado = 0;
unsigned long int Intervalo = 0;

boolean PlotTemp = 0;
boolean PlotUmidade = 0;

String Recebida;

SensorI2C* sensor[(endmax - endmin + 1)];

void setup() {

  Serial.begin(9600);
  Wire.begin();

  SensorI2C::detectarSensores (endmin, endmax, Enderecos, sensoresEncontrados); //Detecção automática dos sensores
}


void loop() {


  switch (stat) {

    case 0:

      if (!stat)  {
        for (int i = 0; i < sensoresEncontrados; i++)
        {
          sensor[i] = new SensorI2C(Enderecos[i]);
        }
        stat = 1;
      }
      break;

    case 1:

      Serial.println("Qual endereço deseja monitorar?");
      Serial.flush();

      while (Serial.available() < 1);
      delay(100);
      Recebida = Serial.readStringUntil('\n');

      Comando = Recebida.toInt();
      Serial.println(Comando);
      //Comando = (char)Serial.read();


      for (int i = 0; i < sensoresEncontrados; i++) {

        if ((Comando) == char(Enderecos[i]))
        {

          IndiceMonitorado = i;
          Comando = 0;
          stat = 2;

          break;
        }
      }
      if (stat == 1) {

        Serial.println("Endereco invalido!");
        Serial.println(" ");
        stat = 1;
      }

      break;


    case 2:

      Serial.println("Envie 1 para temperatura, 2 para umidade ou 3 para ambos.");
      Serial.flush();
      while (Serial.available() < 1);
      delay(100);
      Recebida = Serial.readStringUntil('\n');
      Comando = Recebida.toInt();

      if ((Comando) == 1)
      {
        PlotTemp = 1;
        PlotUmidade = 0;
        stat = 3;

      } else if ((Comando) == 2)
      {

        PlotTemp = 0;
        PlotUmidade = 1;
        stat = 3;

      } else if ((Comando) == 3)
      {

        PlotTemp = 1;
        PlotUmidade = 1;
        stat = 3;

      } else
      {
        Serial.println("Comando Invalido");
        stat = 1;

      }


      break;

    case 3:

      Serial.println("Qual intervalo entre as amostras?");

      while (!Serial.available());
      Recebida = Serial.readStringUntil('\n');
      Intervalo = Recebida.toInt();

      if ((Intervalo > 0) && (Intervalo < 1000))
      {
        stat = 4;
      } else {
        Serial.println("Intervalo invalido");
        Intervalo = 0;
        stat = 1;
      }
      break;

    case 4:

      if (Serial.available()) {
        Recebida = Serial.readStringUntil('\n');
        stat = 1;
      }

      sensor[IndiceMonitorado]->atualizar();

      if (PlotTemp) {
        String Temperatura = sensor[IndiceMonitorado]->obterTemperatura();
        Serial.println("A temperatura de 0x"+ String(Enderecos[IndiceMonitorado])+ " e "+String(Temperatura));
        //Serial.println(Temperatura);
      }
      if (PlotUmidade) {
        String Umidade = sensor[IndiceMonitorado]->obterUmidade();
        Serial.println("A umidade de 0x"+ String(Enderecos[IndiceMonitorado])+ " e "+String(Umidade));
        //Serial.println(Umidade);
      }
      delay(Intervalo);

      break;




  }
}
