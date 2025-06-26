#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(4, 5); // CE, CSN
const char texto[] = "Isabel";

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.setChannel(5);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_HIGH);
  radio.openWritingPipe(0x1234567890LL); // Endereço de escrita
}

void loop() {
  bool sucesso = radio.write(&texto, sizeof(texto));
  
  if (sucesso) {
    Serial.print("Enviado: ");
    Serial.println(texto);
  } else {
    Serial.println("Falha ao enviar.");
  }

  delay(1000);
}