#include "BluetoothSerial.h"

const int ledPin = 22;  // Use GPIO2 for the LED
String device_name = "VisuTech";

// Check if Bluetooth is available
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Check Serial Port Profile
#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin(device_name);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  char dados;
  dados = SerialBT.read();
  if (dados == 'A') {
    digitalWrite(ledPin, HIGH);
  }
  if (dados == 'a') {
    digitalWrite(ledPin, LOW);
  }
}