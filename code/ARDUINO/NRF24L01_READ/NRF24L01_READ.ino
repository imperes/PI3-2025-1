#include <nRF24L01.h> 
#include <RF24.h>

RF24 radio(9, 10); // CE, CSN
char texto[32]; // buffer de recepção
float distancia = 0.0;
bool aguardandoDado = false;

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("Receptor NRF24L01 iniciado.");
  Serial.println("Digite a distância em metros e pressione Enter:");
  
  // Inicializa o rádio (só precisa uma vez)
  radio.begin();
  radio.setChannel(5);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_HIGH);
  radio.openReadingPipe(0, 0x1234567890LL);
  radio.startListening();
}

void loop() {
  // Etapa 1: digita uma nova distância
  if (Serial.available() && !aguardandoDado) {
    distancia = Serial.parseFloat();
    while (Serial.available()) Serial.read(); // limpa buffer

    //Serial.print("\nDistância informada: ");
    //Serial.print(distancia);
   // Serial.println(" metros");
   // Serial.println("Aguardando dado do transmissor...");
    aguardandoDado = true;
  }

  // Etapa 2: após digitar a distância, aguarda dado via rádio
  if (aguardandoDado && radio.available()) {
    radio.read(&texto, sizeof(texto));
    Serial.print("\nRecebido na distância de ");
    Serial.print(distancia);
    Serial.print(" m: ");
    Serial.println(texto);
    Serial.println("\nDigite uma nova distância para testar:");
    aguardandoDado = false;
  }
}