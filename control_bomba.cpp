#include <Arduino.h>
#include "control_bomba.h"

const int servo1Pin = 30;
const int servo2Pin = 31;
const int servo3Pin = 32;
const int servo4Pin = 33;

const int triggerPin = A5;
const int echoPin = A4;

float medirNivelAguaCM() {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  long duracion = pulseIn(echoPin, HIGH);
  float distancia = duracion * 0.0343 / 2.0;  // Convertir a cm
  return distancia;
}

void modo1() {           //PECERA - PECERA
//    Movimiento de servos 1 y 2            108 CERRAR   --  7 ABRIR
    servo1.attach(servo1Pin);
    servo2.attach(servo2Pin);
    servo1.write(7);
    servo2.write(108);
    delay(3000);
    servo1.detach();
    servo2.detach();
    delay(1000);  // tiempo entre grupos

    // Movimiento de servos 3 y 4
    servo3.attach(servo3Pin);
    servo4.attach(servo4Pin);
    servo3.write(108);
    servo4.write(7);
    delay(3000);
    servo3.detach();
    servo4.detach();
    delay(1000);

    // Enciende la bomba después del movimiento de servos
    digitalWrite(bombaPin, HIGH);
    delay(10000);  // Bomba activa por 5 segundos
    digitalWrite(bombaPin, LOW);
}


void modo2() {  // VACIADO

  // Grupo 1: Abrir servo1, cerrar servo2
  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);
  servo1.write(7);
  servo2.write(108);
  delay(3000);
  servo2.detach();
  delay(1000);

  // Grupo 2: Abrir servo3, cerrar servo4
  servo3.attach(servo3Pin);
  servo4.attach(servo4Pin);
  servo3.write(7);   // abrir salida
  servo4.write(108); // cerrar otra
  delay(3000);
  servo3.detach();
  servo4.detach();  // Solo se mantiene servo3 para luego cerrarlo

  // Enciende la bomba
  digitalWrite(bombaPin, HIGH);

  // Espera hasta que el nivel llegue a 20 cm o menos
  while (true) {
    float nivel = medirNivelAguaCM();
    Serial.print("Nivel de agua: ");
    Serial.print(nivel);
    Serial.println(" cm");

    if (medirNivelAguaCM() >= 10.0) {
      break;
    }

    delay(500);  // Medir cada 0.5 s
  }

  // Apaga bomba y cierra servo3
  digitalWrite(bombaPin, LOW);
  servo1.write(108);  // cerrar salida
  delay(3000);
  servo1.detach();
  delay(1000);
}


void modo3() {  // LLENADO

  // Abrir válvula de entrada: servo2 ABRE, servo1 CIERRA
  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);
  servo1.write(108);  // cerrar
  servo2.write(7);    // abrir
  delay(3000);
  servo1.detach();
  servo2.detach();
  delay(1000);

  // Cerrar salida: servo3 CIERRA, servo4 ABRE
  servo3.attach(servo3Pin);
  servo4.attach(servo4Pin);
  servo3.write(108);  // cerrar
  servo4.write(7);    // abrir
  delay(3000);
  servo3.detach();
  servo4.detach();
  delay(1000);

  // Activar bomba de llenado
  digitalWrite(bombaPin, HIGH);

  // Monitorear nivel de agua
  while (medirNivelAguaCM() > 3.0) {  // esperar a que el agua llegue a 3 cm
    delay(500);  // reducir carga del sistema
  }

  // Cuando el agua esté a 3 cm, cerrar servo2, abrir servo1
  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);
  servo1.write(7);    // abrir
  servo2.write(108);  // cerrar
  delay(3000);
  servo1.detach();
  servo2.detach();

  // Apagar bomba
  digitalWrite(bombaPin, LOW);
}
