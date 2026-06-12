#include <Wire.h>
#include <Servo.h>
#include "Adafruit_TCS34725.h"

// Inicializa o sensor de cor
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

// Inicializa o servomotor
Servo motorServo;
const int pinoServo = 9;

// --- ANGULOS DO SERVO ---
const int ANGULO_INICIAL = 83;
const int ANGULO_VERMELHO    = 15;
const int ANGULO_AMARELO = 45;
const int ANGULO_VERDE  = 130;
const int ANGULO_AZUL   = 160;

// --- MEDIAS DE REFERENCIA DE COR ---
// Media de medida das cores
const int REF_VAZIO[]  = {88, 90, 59};
const int REF_VERMELHO[]    = {134, 67, 45};
const int REF_AZUL[]   = {48, 89, 104};
const int REF_AMARELO[] = {116, 89, 33};
const int REF_VERDE[]  = {77, 111, 48};

// Enumeradores para a maquina
enum Cor { VAZIO, VERMELHO, AZUL, AMARELO, VERDE, DESCONHECIDA };
enum Estado { ESPERANDO, ACOMODANDO, ESVAZIANDO };

Estado estadoAtual = ESPERANDO;

void setup() {
  Serial.begin(9600);

  motorServo.attach(pinoServo);
  motorServo.write(ANGULO_INICIAL);

  if (!tcs.begin()) {
    // Se falhar ao encontrar o sensor, codigo para aqui
    while (1);
  }
}

void loop() {
  float r, g, b;

  // Le a cor atual capturada pelo sensor
  tcs.getRGB(&r, &g, &b);
  Cor corDetectada = obterCorMaisProxima(r, g, b);

  // LOGICA DA MAQUINA DE ESTADOS
  switch (estadoAtual) {

    // ESTADO 0: Aguardando a chegada de um M&M
    case ESPERANDO:
      if (corDetectada != VAZIO) {
        // Um objeto bloqueou o sensor.
        // Aguarda 150ms para o M&M se acomodar perfeitamente no lugar
        delay(150);
        estadoAtual = ACOMODANDO;
      }
      break;

      // ESTADO 1: M&M acomodado, le a cor, move o servo, envia para o site
    case ACOMODANDO:
      // Le a cor novamente agora que o M&M parou de se mover
      tcs.getRGB(&r, &g, &b);
      corDetectada = obterCorMaisProxima(r, g, b);

      if (corDetectada != VAZIO) {
        // Move o servo para o copo correto e envia a palavra pela Porta Serial
        if (corDetectada == VERMELHO) {
          motorServo.write(ANGULO_VERMELHO);
          Serial.println("red");
        }
        else if (corDetectada == AZUL) {
          motorServo.write(ANGULO_AZUL);
          Serial.println("blue");
        }
        else if (corDetectada == AMARELO) {
          motorServo.write(ANGULO_AMARELO);
          Serial.println("yellow");
        }
        else if (corDetectada == VERDE) {
          motorServo.write(ANGULO_VERDE);
          Serial.println("green");
        }

        // Aguarda 2 segundos no copo
        delay(2000);

        // Retorna para o ponto de partida original
        motorServo.write(ANGULO_INICIAL);

        // Aguarda o servo terminar fisicamente o movimento de retorno
        delay(500);

        // Transita para a espera da queda do M&M para liberar a camara
        estadoAtual = ESVAZIANDO;
      } else {
        // Alarme falso (ex: uma sombra), volta para o estado de espera
        estadoAtual = ESPERANDO;
      }
      break;

      // ESTADO 2: Aguarda ate a camara esvaziar antes de recomecar a leitura
    case ESVAZIANDO:
      if (corDetectada == VAZIO) {
        // Aguarda 50ms extras para garantir que o M&M caiu por completo
        // e nao esta apenas rolando na frente do sensor
        delay(50);
        estadoAtual = ESPERANDO;
      }
      break;
  }
}

// --- CLASSIFICADOR MATEMATICO DE CORES ---
// Calcula a distancia 3D entre a leitura atual e os perfis de cor mapeados
Cor obterCorMaisProxima(float r, float g, float b) {
  long dVazio  = calcularDistancia(r, g, b, REF_VAZIO);
  long dVermelho    = calcularDistancia(r, g, b, REF_VERMELHO);
  long dAzul   = calcularDistancia(r, g, b, REF_AZUL);
  long dAmarelo = calcularDistancia(r, g, b, REF_AMARELO);
  long dVerde  = calcularDistancia(r, g, b, REF_VERDE);

  // Encontra a distancia minima absoluta
  long distMinima = min(dVazio, min(dVermelho, min(dAzul, min(dAmarelo, dVerde))));

  // Retorna a cor correspondente encontrada
  if (distMinima == dVazio)  return VAZIO;
  if (distMinima == dVermelho)    return VERMELHO;
  if (distMinima == dAzul)   return AZUL;
  if (distMinima == dAmarelo) return AMARELO;
  if (distMinima == dVerde)  return VERDE;

  return DESCONHECIDA;
}

// Funcao auxiliar para calcular a distancia ao quadrado no espaco RGB
long calcularDistancia(float r, float g, float b, const int ref[]) {
  long difR = r - ref[0];
  long difG = g - ref[1];
  long difB = b - ref[2];
  return (difR * difR) + (difG * difG) + (difB * difB);
}
