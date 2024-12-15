#include <Servo.h>


const int trigPin = 7;       // Pino TRIG do HC-SR04
const int echoPin = 6;       // Pino ECHO do HC-SR04
const int servoPin = 9;      // Pino do servo motor
const int relayPin = 8;      // Pino do relé (para alarme/luz)

// Variáveis
long duration;
int distance;
Servo meuServo;             // Criação do objeto Servo

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(relayPin, OUTPUT);

  meuServo.attach(servoPin);
  
  // Inicializa comunicação serial para monitoramento e com o ESP32
  Serial.begin(9600);   // Monitor serial
  Serial1.begin(19200); // Comunicação com ESP32
}

void loop() {
  // Envia o sinal do TRIG (emitir o pulso)
  digitalWrite(trigPin, LOW);  
  delayMicroseconds(2);         
  digitalWrite(trigPin, HIGH);  
  delayMicroseconds(10);        
  digitalWrite(trigPin, LOW);  

  
  duration = pulseIn(echoPin, HIGH);
  
  distance = duration * 0.0344 / 2;

  // Envia  ao ESP32 via Serial1
  // Envia "Liberado" ou "Bloqueado" com base na distância
  if (distance < 10) {
    Serial1.println("Bloqueado");
  } else {
    Serial1.println("Liberado");
  }
  
  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Se a distância for menor que 10 cm, aciona o servo e o relé
  if (distance < 10) {
    Serial.println("Objeto detectado!");
    
    digitalWrite(relayPin, HIGH);
    
    // Move o servo para liberar o item  90 graus
    meuServo.write(90);      
    delay(1000);              
    meuServo.write(0);       
    
    // Desativa o relé
    digitalWrite(relayPin, LOW);
    delay(10000);              
  } else {
    // Se a distância for maior que 10 cm, o relé é desligado
    digitalWrite(relayPin, LOW);
  }
}
