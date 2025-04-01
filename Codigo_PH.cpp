#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

#define ONE_WIRE_BUS 2  // Pin digital donde está conectado el DS18B20

// Pines analógicos para los sensores
#define PH_SENSOR_PIN A1
#define TDS_SENSOR_PIN A2
#define TURBIDITY_SENSOR_PIN A3

// Configuración de WiFi
const char* ssid = "TuSSID";
const char* password = "TuPassword";

// Crear objeto de OneWire y DS18B20
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Crear objeto para el módulo ESP8266 (SoftwareSerial)
SoftwareSerial esp8266(10, 11); // RX, TX

// Variables para almacenar lecturas
float pHValue = 0.0;
float TDSValue = 0.0;
float turbidezValue = 0.0;
float temperature = 0.0;

void setup() {
  Serial.begin(115200);
  esp8266.begin(115200);

  // Iniciar el sensor de temperatura
  sensors.begin();

  // Configurar el módulo WiFi ESP8266
  connectWiFi();

  // Configurar pines de los sensores
  pinMode(PH_SENSOR_PIN, INPUT);
  pinMode(TDS_SENSOR_PIN, INPUT);
  pinMode(TURBIDITY_SENSOR_PIN, INPUT);
}

void loop() {
  // Leer temperatura
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);

  // Leer pH
  pHValue = analogRead(PH_SENSOR_PIN);
  pHValue = map(pHValue, 0, 1023, 0, 14); // Mapear la lectura a un rango de pH (ajustar según el sensor)

  // Leer TDS (Conductividad)
  TDSValue = analogRead(TDS_SENSOR_PIN); // Lectura analógica de TDS (ajustar según el sensor)

  // Leer turbidez
  turbidezValue = analogRead(TURBIDITY_SENSOR_PIN); // Lectura analógica de turbidez (ajustar según el sensor)

  // Enviar datos a través de WiFi
  sendDataToServer();

  // Enviar datos a través de Bluetooth
  sendDataToBluetooth();

  // Esperar antes de tomar otra lectura
  delay(5000);
}

void connectWiFi() {
  esp8266.println("AT");
  delay(1000);
  esp8266.println("AT+CWMODE=1");
  delay(1000);
  esp8266.println("AT+CWJAP=\"" + String(ssid) + "\",\"" + String(password) + "\"");
  delay(5000);
}

void sendDataToServer() {
  String data = "Temperature=" + String(temperature) + "&pH=" + String(pHValue) + "&TDS=" + String(TDSValue) + "&Turbidity=" + String(turbidezValue);
  esp8266.println("AT+CIPSTART=\"TCP\",\"your.server.com\",80");
  delay(2000);
  esp8266.println("AT+CIPSEND=" + String(data.length() + 4)); // +4 for the \r\n\r\n
  delay(2000);
  esp8266.print(data);
  esp8266.println("\r\n");
}

void sendDataToBluetooth() {
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" C | ");
  Serial.print("pH: ");
  Serial.print(pHValue);
  Serial.print(" | ");
  Serial.print("TDS: ");
  Serial.print(TDSValue);
  Serial.print(" | ");
  Serial.print("Turbidity: ");
  Serial.println(turbidezValue);
}
