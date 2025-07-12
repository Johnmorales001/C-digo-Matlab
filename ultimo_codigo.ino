#include <OneWire.h>                  // Librería para comunicación OneWire
#include <DallasTemperature.h>       // Librería para trabajar con sensores DS18B20

#define SSR_PIN 3                     // Pin donde está conectado el relé de estado sólido (niquelina)
#define ONE_WIRE_BUS 2               // Pin donde está conectado el sensor DS18B20

OneWire oneWire(ONE_WIRE_BUS);       // Inicializa la comunicación OneWire en el pin 2
DallasTemperature sensors(&oneWire); // Crea un objeto para manejar el sensor de temperatura DS18B20

// Unión para convertir entre float y bytes
union {
  byte bytes[4];     // Arreglo de 4 bytes
  float value;       // Valor flotante
} floatUnion;

int byteCount = 0;                   // Contador de bytes recibidos por serial
float pwmValue = 0.0;                // Valor de PWM recibido desde Simulink
bool simulinkActivo = false;        // Bandera para saber si Simulink está enviando datos
unsigned long ultimaLectura = 0;    // Momento en el que se recibió el último dato
const unsigned long tiempoEspera = 2000; // Tiempo de espera antes de considerar que Simulink se desconectó (2 segundos)

void setup() {
  Serial.begin(9600);               // Inicia la comunicación serial a 9600 baudios
  sensors.begin();                  // Inicializa el sensor de temperatura
  pinMode(SSR_PIN, OUTPUT);        // Configura el pin del SSR como salida
  analogWrite(SSR_PIN, 0);         // Apaga inicialmente la niquelina
}

void loop() {
  // Leer datos enviados desde Simulink por el puerto serial
  while (Serial.available()) {
    floatUnion.bytes[byteCount++] = Serial.read(); // Almacena cada byte recibido

    if (byteCount == 4) {           // Si ya se han recibido los 4 bytes de un float
      byteCount = 0;                // Reinicia el contador
      ultimaLectura = millis();     // Actualiza el tiempo de última lectura
      simulinkActivo = true;        // Marca que Simulink sigue enviando datos
      pwmValue = floatUnion.value;  // Convierte los 4 bytes al valor float correspondiente
    }
  }

  // Verifica si han pasado más de 2 segundos sin recibir datos de Simulink
  if (millis() - ultimaLectura > tiempoEspera) {
    simulinkActivo = false;         // Si es así, se asume que Simulink ya no está activo
  }

  // Controla la niquelina según el valor de PWM recibido
  if (simulinkActivo) {
    int pwm = constrain((int)pwmValue, 0, 255);  // Asegura que el valor esté dentro del rango válido para PWM
    analogWrite(SSR_PIN, pwm);       // Aplica el valor PWM al relé
  } else {
    analogWrite(SSR_PIN, 0);         // Apaga la niquelina si Simulink no está activo
  }

  // Envía la temperatura por serial cada 1
