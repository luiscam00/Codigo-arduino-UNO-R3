//////////////////////////////////////////////////////////////////////////////////
//////////////////////////CODIGO BORRADOR////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
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
const char* ssid = "ElSSID"; //Tu WIFI tiene un SSID, y lo debes poner aquí (Puedes buscar en Google cómo hacerlo, no es tan díficil)
const char* password = "LaContraseña"; //Aquí lo mismo que en el SSID, pero con la contraseña del wifi 

// Crear objeto de OneWire y DS18B20
OneWire oneWire(ONE_WIRE_BUS); // Aquí se inicia la comunicación con el sensor de temperatura DS18B20 usando un protocolo llamdo OneWire. (Tmb puedes buscar en google de qué se trata el protocolo, tmb es fácil de entender)
DallasTemperature sensors(&oneWire); //Aquí Inicializa el sensor de temperatura DS18B20 para leer la temperatura.
// Crear objeto para el módulo ESP8266 (SoftwareSerial)
SoftwareSerial esp8266(10, 11); // RX, TX Aquí configura la llamada "comunicación serial" con el módulo ESP8266 usando los pines 10 (RX) y 11 (TX). 
// Variables para almacenar lecturas
float pHValue = 0.0; //Aquí creo una "variable" que es como una caja que guarda un valor(En este caso un número). Y lo que almacena esa caja es un "float", es decir un número decimal
float TDSValue = 0.0; //Lo mismo que arriba, pero esta variable se llama RDSvalue, pero hace lo mismo que lo de arriba
float turbidezValue = 0.0; //Lo mismo que las dos variables anteriores
float temperature = 0.0; //Lo mismo que las tres variables anteriores 

void setup() {
  Serial.begin(115200);
  esp8266.begin(115200);

  // Iniciar el sensor de temperatura
  sensors.begin(); //begin se usa para iniciar algo

  // Configurar el módulo WiFi ESP8266
  connectWiFi(); //Se conecta con el módulo que compraste (el WiFi ESP8266)

  // Configurar pines de los sensores
  pinMode(PH_SENSOR_PIN, INPUT); //Aquí puedes ver que estoy cofigurando los pines que invoqué al inicio, y si ves, hay algo que dice "INPUT", INPUT en este caso configura el pin para recibir señales, es decir, para leer datos desde el sensor o dispositivo conectado a ese pin.
  pinMode(TDS_SENSOR_PIN, INPUT); //Lo mismo del anterior
  pinMode(TURBIDITY_SENSOR_PIN, INPUT); //Lo mismo del anterior
}

void loop() {
  sensors.requestTemperatures(); // Aquí pido que el sensor DS18B20 lea la temperatura.
  temperature = sensors.getTempCByIndex(0); // Aquí obtengo la temperatura que leyó el sensor, y la guardo en la variable "temperature".

  pHValue = analogRead(PH_SENSOR_PIN); // Aquí leo el valor que nos envia el sensor de pH.
  pHValue = map(pHValue, 0, 1023, 0, 14); // Mapea el valor leído del sensor (de 0 a 1023) a un rango de pH (de 0 a 14). Los números 0 y 1023 son el "rango" de salida del sensor, y 0 y 14 son el rango de pH (escala de acidez a alcalinidad).
  TDSValue = analogRead(TDS_SENSOR_PIN); // Aquí leo el valor del sensor de TDS, que mide la conductividad en el agua (también un valor analógico).
  turbidezValue = analogRead(TURBIDITY_SENSOR_PIN); // Aquí leo el valor del sensor de turbidez, que mide la claridad del agua (también un valor analógico).

  sendDataToServer(); // Aquí llamo a una función que enviará todos los datos (temperatura, pH, TDS y turbidez) al "servidor" por medio de WiFi.
  
  sendDataToBluetooth(); // Aquí "llamo" a una función que enviará los datos por Bluetooth.

  delay(5000); // Aquí hago una pausa de 5 segundos antes de leer los sensores otra vez (y así evitar que el sistema se sature mucho, por eso son 5 segundos).
}

void connectWiFi() {
  esp8266.println("AT"); // Aquí le envío el comando "AT" al ESP8266 para ver si responde correctamente (es como decir "Hola al sensor").
  delay(1000); // Pauso el código por 1 segundo para asegurar que el comando anterior se haya procesado.
  esp8266.println("AT+CWMODE=1"); // Aquí le digo al ESP8266 que se ponga en "modo estación" para que se pueda conectar a una red WiFi.
  delay(1000); // Pauso el código por 1 segundo.
  esp8266.println("AT+CWJAP=\"" + String(ssid) + "\",\"" + String(password) + "\""); // Aquí le envío el comando para conectarse a la red WiFi con el SSID y contraseña que puse antes.
  delay(5000); // Pauso el código por 5 segundos para dar tiempo al ESP8266 de conectarse.
}

void sendDataToServer() {
  String data = "Temperature=" + String(temperature) + "&pH=" + String(pHValue) + "&TDS=" + String(TDSValue) + "&Turbidity=" + String(turbidezValue); 
  // Aquí preparo los datos (temperatura, pH, TDS y turbidez) para enviarlos al servidor, los convierto en una cadena de texto.

  esp8266.println("AT+CIPSTART=\"TCP\",\"your.server.com\",80"); // Aquí le pido al ESP8266 que abra una conexión "TCP" con un "servidor" ("your.server.com" es un ejemplo de más o menos como funciona esto).
  delay(2000); // Pauso por 2 segundos para esperar que la conexión se haga.
  esp8266.println("AT+CIPSEND=" + String(data.length() + 4)); // Aquí le indico al ESP8266 cuántos datos voy a enviar (los datos más 4 caracteres para el formato correcto).
  delay(2000); // Pauso 2 segundos.
  esp8266.print(data); // Aquí envío los datos al servidor.
  esp8266.println("\r\n"); // Aquí envío el comando para finalizar la transmisión de datos.
}

void sendDataToBluetooth() {
  Serial.print("Temperature: "); // Aquí le digo al puerto serial (Bluetooth) que imprima "Temperature: ".
  Serial.print(temperature); // Aquí imprimo el valor de la temperatura por Bluetooth.
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
