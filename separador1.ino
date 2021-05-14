/*
Separador de pilhas
Modo 1: configuração
Modo 2: separação
*/

//bibliotecas display
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

//biblioteca servo
#include <Servo.h>

//Declara o LCD como Adafruit_PCD8544(CLK,DIN,D/C,CE,RST);
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);
//Adafruit_PCD8544 display = Adafruit_PCD8544(3, 4, 5, 6, 7);

//Declara os servos
Servo ServoPrincipal;
Servo ServoAuxiliar;

//define os pinos
#define modo 8
#define potenciometro A2
#define batteryPin A3
#define pinoServoPrincipal 9
#define pinoServoAuxiliar 10

//Voltagem mínima de separação
float voltagemMinima = 1.2;

//rotação do servo auxiliar quando não está fazendo contato
int rotMinimaServoAuxiliar = 40;

void setup() {
  //configura o interruptor do modo
  pinMode(modo, INPUT);
  
  //inicia o display
  display.begin();
  //configura o contraste
  display.setContrast(57);
  //limpa imagens que já estivessem na tela
  display.clearDisplay();

  //Configura os servos
  ServoPrincipal.attach(pinoServoPrincipal);
  ServoAuxiliar.attach(pinoServoAuxiliar);

  //Coloca os servos nas posições iniciais
  //servo principal começa no meio, para receber a pilha
  ServoPrincipal.write(90); 
  //servo auxiliar começa sem fazer contato
  ServoAuxiliar.write(rotMinimaServoAuxiliar); 
}

void loop() {
  //modo de configuração
  if(digitalRead(modo)){
    
    //lê o valor do potenciometro, que vai de 0 a 1023
    float leitura = analogRead(potenciometro);
    //Como a leitura é meio instável nos números mais baixos (por ex, nunca chega a zero), limita a um intervalo de 100 a 1023
    leitura = constrain(leitura,100,1023);
    //mapeia o valor para mV, considerando 100 -> 0mV e 1023 -> 1500mV
    leitura = map(leitura, 100, 1023, 0, 1600);
    
    //transforma mV em V
    voltagemMinima = leitura/1000;
      
    //Escreve o título no display
    //Tamanho do texto
    display.setTextSize(1);
    //Cor (display é monocromático, mas tem que especificar)
    display.setTextColor(BLACK);
    //Coordenadas do texto
    display.setCursor(0,0);
    //escreve o texto
    display.println("Selecione o   limite:");
    
    //Escreve a voltagem
    display.setTextColor(BLACK);
    display.setCursor(0,19);
    display.setTextSize(2);
    //escreve a voltagem, adicionando a medida no final
    display.println(String(voltagemMinima)+"v");

    //Desenha um seletor
    //desenha um retangulo vazio como plano de fundo (x, y, largura, altura, cor)
    display.drawRect(0, 38, 84, 4, BLACK);
    //desenha o puxador, usando a voltagem mínima como referencia de posição
    display.fillRect(map(int(voltagemMinima*100),0,160,0,80), 37, 4, 6, BLACK);
    
    //atualiza o display
    display.display();
    //dá um delay de 0.1 segundos
    delay(100);
    //limpa o display para o próximo ciclo
    display.clearDisplay();
  }
  //modo de separação
  else{

    //desenha o título no display
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor(0,0);
    display.println("Separando...");

    //desenha o corpo da pilha    
    display.drawRect(24, 13, 56, 14, BLACK);
    //desenha a ponta da pilha
    display.drawRect(79, 16, 5, 7, BLACK);
    
    //Escreve a voltagem mínima (definida na configuração)
    display.setTextColor(BLACK);
    display.setCursor(0,39);
    display.setTextSize(1);
    display.println("Base:" +String(voltagemMinima)+"v"); 

    //loop para fechar o contato com a pilha, com i sendo o angulo que vai até 90º 
    for(int i = rotMinimaServoAuxiliar; i>=0;i-=2){       

      //limpa um pedaço só do display
      display.fillRect(0,16,24,20,WHITE);
      
      //desenha o ponto de apoio do contato
      display.fillRect(21,29,4,3,BLACK);

      //desenha o contato, como duas linhas (x_inicial, y_inicial, x_final, y_final, cor)
      //seno e cosseno aí é para fazer um movimento circular
      display.drawLine(23,31,24 + cos(rad(270-i)) * 15, 31+ sin(rad(270-i)) * 15,BLACK);
      display.drawLine(22,31,23 + cos(rad(270-i)) * 15, 31+ sin(rad(270-i)) * 15,BLACK);

      //Escreve o angulo no servo      
      ServoAuxiliar.write(i);
      
      //delay bem curto
      delay(10);      
    
      //atualiza o display
      display.display();
    }
    //delay de 0.3 segundos
    delay(300);

    float voltagem = 0.0;
    
    int leitura = 0;
    for(int i = 0;i<100;i++){
      int novaLeitura =constrain(analogRead(batteryPin),0,1023);
      if(novaLeitura>leitura){
        leitura = novaLeitura;
      }
      delay(1);
    }
    voltagem = constrain(analogRead(batteryPin),0,1023)/303.0 * 1.32;      

    
        
    //escreve a voltagem lida
    display.setTextColor(BLACK);
    display.setCursor(28,16);
    display.setTextSize(1);
    display.println(String(voltagem)+"v");
    
    //atualiza o display
    display.display();

    //aguarda 0.3 segundos
    delay(300);

    //loop para abrir o contato com a pilha
    for(int i = 0; i<rotMinimaServoAuxiliar;i+=2){ 
        
      //limpa um pedaço só do display
      display.fillRect(0,16,24,20,WHITE);
      
      //desenha o ponto de apoio do contato
      display.fillRect(21,29,4,3,BLACK);

      //desenha o contato, como duas linhas (x_inicial, y_inicial, x_final, y_final, cor)
      //seno e cosseno aí é para fazer um movimento circular
      display.drawLine(23,31,24 + cos(rad(270-i)) * 15, 31+ sin(rad(270-i)) * 15,BLACK);
      display.drawLine(22,31,23 + cos(rad(270-i)) * 15, 31+ sin(rad(270-i)) * 15,BLACK);      

      //Escreve o angulo no servo      
      ServoAuxiliar.write(i);
      
      //delay bem curto
      delay(10);      
    
      //atualiza o display
      display.display();
    }
    
    //decisão se descarta ou não a pilha
    if(voltagem < voltagemMinima){
      //descarta a pilha

      //gira o servo principal para deixar a pilha cair na caixa das ruins
      for(int i = 90; i>=0;i-=1){ 
        ServoPrincipal.write(i); 
        delay(10);            
      }

      //aguarda 0.3 segundos
      delay(300);

      //gira o servo principal de volta para a posição inicial
      for(int i = 0; i<=90;i+=1){ 
        ServoPrincipal.write(i); 
        delay(10);            
      }
    }
    else{
      //recolhe a pilha

      //gira o servo principal para deixar a pilha cair na caixa das boas
      for(int i = 90; i<=180;i+=1){ 
        ServoPrincipal.write(i); 
        delay(10);            
      }
      
      //aguarda 0.3 segundos
      delay(300);

      //gira o servo principal de volta para a posição inicial      
      for(int i = 180; i>=90;i-=1){ 
        ServoPrincipal.write(i); 
        delay(10);            
      }
      
    }  

    //aguarda 1 segundo para medir a próxima pilha
    delay(1000);

    //limpa o display para o próximo ciclo    
    display.clearDisplay();
    
  }
  
}

//converte grau para radianos
float rad(int degrees){
  return float(degrees) * 1000 / 57296;
}
