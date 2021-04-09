#include "sevenSegmentDisplay.h"
#include "PinChangeInterrupt.h"

// Pinos do display
int pinA = 10;
int pinB = 8;
int pinC = 6;
int pinD = 5;
int pinE = 4;
int pinF = 11;
int pinG = 12;
int pinDP = 7;

// Pino do botão para Pinchange
int botaoReset = A0;

// Pinos dos botões INT0 e INT1
int botaoProx = 3;
int botaoAnt = 2;

// Configura o display utilizando a biblioteca
sevenSegmentDisplay display("COMMON_ANODE", pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP);


char texto[] = "HELL0 At328P"; // Texto que será impresso
unsigned int i = 0; // Iterador do array
int tamanho = sizeof(texto) - 1;  // Tamanho do texto
int letra = texto[0]; // Letra inicial

// Para piscar na troca de letra
int i_ant = i;
int troca = 0;

// Contador do timer1
int cont = 0;

int periodo = 0;

void setup()
{

  // Config dos botões
  pinMode(botaoProx,INPUT_PULLUP);
  pinMode(botaoAnt,INPUT_PULLUP);
  pinMode(botaoReset, INPUT_PULLUP);

  // Config das interrupções externas
  // Foi escolhido RISING para a interrupção ocorrer ao soltar o botão
  attachInterrupt(digitalPinToInterrupt(botaoProx), proxHandler, RISING);
  attachInterrupt(digitalPinToInterrupt(botaoAnt), antHandler, RISING);

  // Config da interrupção por pinchange
  attachPCINT(digitalPinToPCINT(botaoReset), resetHandler, RISING);
  

  // Config do timer1 
  TCCR1A = 0; // Configura timer para operação normal pinos OC1A e OC1B desconectados
  TCCR1B = 0; // Limpa registrador
  TIMSK1 |= (1 << TOIE1); // Habilita a interrupção do TIMER1
  
  TCCR1B = 1; // Modo normal sem prescaler
  TCNT1 = 0;

  // 65536 ciclos * 6.25e-08 (periodo do ciclo em s) =  0.004096 s = 4.09ms tempo para interrupcao
  // 10000 ms / 4.09 ms/interrupcao = +-245 
  // 245 ciclos teremos 1S
  periodo = 2 * 245; // 2 segundos
}

void loop()
{
  // Pisca ao trocar de letra
  if ( i != i_ant )
  {
    i_ant = i;
    imprimir(' ');
    troca = 30000;
    return;
  }
  
  if ( troca > 0 )
  {
    troca--;
    return;
  }

  //Imprime a letra
  imprimir(letra);
}

void imprimir(int letra)
{  
  // Converte a string do número para int
  if( letra >= 48 && letra <= 57 )
    {
      letra -= '0';
    }

  display.set(letra);
}

// Avança uma letra
void avancar()
{
  i++;
  letra = texto[i % tamanho];
}


// Retorna uma letra
void voltar()
{
  if(i == 0) i = tamanho - 1; // Vai para o fim do texto
  else i--;
  letra = texto[i % tamanho];
}

// Retorna a primeira letra do texto
void resetar()
{
  i=0;
  letra = texto[i % tamanho];
}

// Interrupção do TIMER1 
ISR(TIMER1_OVF_vect)
{
  if(cont == periodo)
  {
    avancar();
    cont = 0; // Reinicia o contador do timer
    return;
  }
 
  cont++;
}

// Interrupção dos botões
void proxHandler()
{
  if ( cont < 50) return; // Para evitar click duplo

  cont = 0; // Reinicia o contador do timer
  avancar();
}

void antHandler()
{
  if ( cont < 50) return;

  cont = 0; // Reinicia o contador do timer
  voltar();
}

void resetHandler()
{
  if ( cont < 50) return;

  cont = 0; // Reinicia o contador do timer
  resetar();
}
