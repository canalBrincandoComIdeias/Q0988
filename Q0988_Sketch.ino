/*  
*  AUTOR:    BrincandoComIdeias
*  APRENDA:  https://cursodearduino.net/
*  SKETCH:   Sensor Fim de Curso
*  DATA:     16/02/23
*/

#include <Nextion.h>

// DEFINIÇÕES DE PINOS
#define pinLdr A0
#define pinAbrir 9
#define pinFechar 10

#define pinFimAbre 7
#define pinFimFecha 6

// DEFINIÇÕES
#define luzAbrir 600
#define luzFechar 400

#define SUBINDO 2
#define ABERTA 1
#define PARADA 0
#define FECHADA -1
#define DESCENDO -2

#define AUTO 1
#define MANUAL 0

#define intervalo 1500

// VARIÁVEIS GLOBAIS
int persiana;
int modo = AUTO;
unsigned long ultimaLeitura;

// INSTANCIANDO OBJETOS
NexButton btAbre = NexButton(1, 1, "b0");
NexButton btFecha = NexButton(1, 2, "b1");
NexDSButton btModo = NexDSButton(1, 4, "bt0");
NexNumber nvLuz = NexNumber(1, 6, "n0");

NexTouch *nex_listen_list[] = {
  &btAbre,
  &btFecha,
  &btModo,
  NULL
};

// DECLARANDO FUNÇÕES
void btAbreApertado(void *ptr);
void btAbreSolto(void *ptr);
void btFechaApertado(void *ptr);
void btFechaSolto(void *ptr);
void btModoApertado(void *ptr);

void setup() {
  nexInit();

  btAbre.attachPush(btAbreApertado);
  btAbre.attachPop(btAbreSolto);
  btFecha.attachPush(btFechaApertado);
  btFecha.attachPop(btFechaSolto);
  btModo.attachPush(btModoApertado);

  // CONFIGURAÇÃO DOS PINOS
  pinMode(pinLdr, INPUT);

  /*  O módulo fim de curso já tem circuito de pullup
      mantemos o INPUT_PULLUP pois serve tanto para o modulo 
      quanto para o botão fim de curso ligado diretamente
  */
  pinMode(pinFimAbre, INPUT_PULLUP);
  pinMode(pinFimFecha, INPUT_PULLUP);

  pinMode(pinAbrir, OUTPUT);
  pinMode(pinFechar, OUTPUT);

  pinMode(13, OUTPUT);  // DEBUG

  // CONFIRMA O MOTOR PARADO
  digitalWrite(pinAbrir, LOW);
  digitalWrite(pinFechar, LOW);

  // VERIFICANDO POSICAO DA CORTINA AO INICIAR
  if (!digitalRead(pinFimAbre)) {
    persiana = ABERTA;
  } else if (!digitalRead(pinFimFecha)) {
    persiana = FECHADA;
  } else {
    persiana = PARADA;
  }
}

void loop() {
  nexLoop(nex_listen_list);

  if (millis() - ultimaLeitura > intervalo) {
    // LEITURA DO SENSOR
    int leitura = analogRead(pinLdr);

    // INVERTE A LÓGICA DO SENSOR ANALÓGICO
    leitura = map(leitura, 0, 1023, 1023, 0);

    // ENVIA A LEITURA PARA O DISPLAY
    if (persiana != SUBINDO && persiana != DESCENDO)
      nvLuz.setValue(map(leitura, 1023, 0, 100, 0));

    if (modo == AUTO) {

      if (persiana != ABERTA) {
        if (leitura > luzAbrir) {
          abrirPersiana();
          persiana = ABERTA;
        }
      }

      if (persiana != FECHADA) {
        if (leitura < luzFechar) {
          fecharPersiana();
          persiana = FECHADA;
        }
      }

    }

    ultimaLeitura = millis();
  }
  if (modo == MANUAL) {
    if (!digitalRead(pinFimAbre) && persiana == SUBINDO) {
      digitalWrite(pinAbrir, LOW);
      digitalWrite(pinFechar, LOW);

      persiana = ABERTA;
    }

    if (!digitalRead(pinFimFecha) && persiana == DESCENDO) {
      digitalWrite(pinAbrir, LOW);
      digitalWrite(pinFechar, LOW);
      
      persiana = FECHADA;
    }
  }
}

void abrirPersiana() {
  /* Liga o motor */
  analogWrite(pinAbrir, 120);
  digitalWrite(pinFechar, LOW);

  /* Não faz nada até que o fim de curso seja pressionado */
  while (digitalRead(pinFimAbre)) { ; }

  /* Desliga o motor */
  digitalWrite(pinAbrir, LOW);
  digitalWrite(pinFechar, LOW);
}

void fecharPersiana() {
  /* Liga o motor */
  digitalWrite(pinAbrir, LOW);
  analogWrite(pinFechar, 120);

  /* Não faz nada até que o fim de curso seja pressionado */
  while (digitalRead(pinFimFecha)) { ; }

  /* Desliga o motor */
  digitalWrite(pinAbrir, LOW);
  digitalWrite(pinFechar, LOW);
}

void btAbreApertado(void *ptr) {
  /* VERIFICA SE NÃO ESTÁ NO FIM DE CURSO */
  if (digitalRead(pinFimAbre)) {
    modo = MANUAL;
    btModo.setValue(modo);
    btModo.setText("MANUAL");

    /* Liga o motor */
    analogWrite(pinAbrir, 120);
    digitalWrite(pinFechar, LOW);

    persiana = SUBINDO;
  }
}

void btAbreSolto(void *ptr) {
  /* Desliga o motor */
  digitalWrite(pinAbrir, LOW);
  digitalWrite(pinFechar, LOW);
  
  persiana = PARADA;
}

void btFechaApertado(void *ptr) {
  /* VERIFICA SE NÃO ESTÁ NO FIM DE CURSO */
  if (digitalRead(pinFimFecha)) {
    modo = MANUAL;
    btModo.setValue(modo);
    btModo.setText("MANUAL");

    /* Liga o motor */
    analogWrite(pinAbrir, 120);
    digitalWrite(pinFechar, LOW);   

    persiana = DESCENDO;
  }
}

void btFechaSolto(void *ptr) {
  /* Desliga o motor */
  digitalWrite(pinAbrir, LOW);
  digitalWrite(pinFechar, LOW);
  
  persiana = PARADA;
}

void btModoApertado(void *ptr) {
  uint32_t estadoBot;

  btModo.getValue(&estadoBot);
  modo = estadoBot;
  if (estadoBot == AUTO) {
    btModo.setText("AUTO");
  } else if (estadoBot == MANUAL) {
    btModo.setText("MANUAL");
  }
}