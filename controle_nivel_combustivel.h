#Executa a leitura de um tanque cilindrico de combustivel utilizando o micro controlador atmega
#by José Tiago josetiagobsouza@gmail.com

#include <math.h>
#include <SoftwareSerial.h>    // Incluimos a livraria SoftwareSerial

#define TRIG 13 //RX
#define ECHO 12 //TX

//SMS
SoftwareSerial mySerial(8, 9); // Declaramos os pinos RX(8) y TX(9) que vamos a usar

//Calculadora = http://www.zhitov.ru/pt/tank1/
//Tanques = https://rextester.com/XKGG79627 https://www.fazforte.com.br/tanques-combustivel-horizontal.php

float diametro,comprimento_tanque, x, h, volum, comprimento, volume_tanque, capacidade,raio;
float altura_combustivel, alturacm, alturacmsensor, alturacanoTanqueCM, alturacmDesconto;
int ix,iy;

unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change:
int primsg = 1;
const long interval = 43200000;   //4,32e+7

void setup() {
   Serial.begin(9600);       // Iniciamos a comunicação serie
   mySerial.begin(9600); 
   pinMode(TRIG, OUTPUT); // Initializing Trigger Output and Echo Input 
   pinMode(ECHO, INPUT_PULLUP);
   digitalWrite(TRIG, LOW); // DESLIGA O TRIGGER E ESPERA 500 uS
   delayMicroseconds(500);
   pinMode(10, OUTPUT);
  
  /*** CONFIGURAÇÃO **********************************/
    diametro = 1.9;//Em metros
    comprimento_tanque = 2.76;//Em metros
    alturacanoTanqueCM = 0*100; //Converte em cm Essa altura e altura que está o sensor na boca do tanque
    
    
  /*** ******************************************* ***/
    
   
}

void loop() {

  if (mySerial.available()){          // Se a comunicação SoftwareSerial tem dados
  Serial.write(mySerial.read());    // Obtemo-los por comunicação serie normal
} 
  
if (Serial.available()){            // Se a comunicação serie normal tem dados
  while(Serial.available()) {       // e enquanto tenha dados para mostrar 
    mySerial.write(Serial.read());  // Obtemos pela comunicação SoftwareSerial
  } 
  mySerial.println();               // Enviamos um fim de linha
}
 
  //h = leituraDistancia(diametro);
  //Serial.print("Distancia H: ");
  //Serial.println(h);
  //h = diametro*100; //converte em cm 222
  //ix = (int) h;

  alturacm = diametro*100;//converte em cm
  alturacmsensor = leituraDistancia(); 
  alturacmDesconto = (alturacmsensor-alturacanoTanqueCM);
  h = alturacm - alturacmDesconto;
  
  Serial.print("h:");
  Serial.print(h);
  Serial.print(" == ");
  volume_tanque = calcularVolumeC1(raio, h, comprimento);
  Serial.print("alturacmsensor:");
  Serial.print(alturacmsensor);
  Serial.print(" => ");
  Serial.print("Volume:");
  Serial.print(volume_tanque);
  Serial.println(" Litros");
  //EnviaSMS(String(volume_tanque));

  if(primsg == 1){
    delay(5000);
    primsg = 2;
    digitalWrite(10, HIGH);  
    // save the last time you blinked the LED
   

   ///EnviaSMS(String(volume_tanque));
   digitalWrite(10, LOW);// Pequena pausa
    }
  
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    primsg = 2;
    digitalWrite(10, HIGH);  
    // save the last time you blinked the LED
    previousMillis = currentMillis;

   EnviaSMS(String(volume_tanque));
   
   digitalWrite(10, LOW);// Pequena pausa
  }
  delay(5000);
 }

//Calcula o volume de um cilindro Horizonta 
float calcularVolumeC1(float raio, float altura_combustivel, float comprimento){
  raio = (diametro*10)/2; //converte em milimetro
  comprimento =(comprimento_tanque*10);
  capacidade = M_PI *sq(raio)*comprimento;
  x = altura_combustivel/10;
  return round((sq(raio)/2) * (2*arco_em_graus((raio-x)/raio) - sin(2*arco_em_graus((raio-x)/raio)))*comprimento);
  }

float leituraDistancia() {
  digitalWrite(TRIG, LOW); // Set the trigger pin to low for 2uS 
  delayMicroseconds(2); 
  digitalWrite(TRIG, HIGH); // Send a 10uS high to trigger ranging
  delayMicroseconds(20); 
  digitalWrite(TRIG, LOW); // Send pin low again
  int distance = pulseIn(ECHO, HIGH,26000); // Read in times pulse
  distance = (distance/58); //retorna distancia em centimentros medida
  return distance;

}

//Formula que calcula o arco em graus em 8 bits, retorna o arco em graus cujo cosseno é número. //ARCCOS pascal
double arco_em_graus(double x) {
  double negate = double(x < 0);
  x = abs(x); 
  x -= double(x>1.0)*(x-1.0); // <- equivalente a min (1,0, x), mais rápido
  double ret = -0.0187293;
  ret = ret * x;
  ret = ret + 0.0742610;
  ret = ret * x;
  ret = ret - 0.2121144;
  ret = ret * x;
  ret = ret + 1.5707288;
  ret = ret * sqrt(1.0-x);
  ret = ret - 2 * negate * ret;
  return negate * 3.14159265358979 + ret;
}

// Função para o envio de um SMS
void EnviaSMS(String litros){       
 mySerial.println("AT+CMGF=1");                 // Activamos a função de envio de SMS
 delay(100);                                    // Pequena pausa
 mySerial.println("AT+CMGS=\"+5562999955753\"");  // Definimos o número do destinatário em formato internacional
 delay(100);                                    // Pequena pausa
 mySerial.print("001 LITROS: "+litros);                 // Definimos o corpo da mensagem
 delay(500);                                    // Pequena pausa
 mySerial.print(char(26));                      // Enviamos o equivalente a Control+Z 
 delay(100);                                    // Pequena pausa
 mySerial.println("");                          // Enviamos um fim de linha
 delay(100);     
 
}
