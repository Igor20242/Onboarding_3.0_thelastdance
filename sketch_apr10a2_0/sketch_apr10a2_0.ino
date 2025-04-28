#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Wire.h>
#include <RTClib.h>

// Display Nokia 5110
#define PIN_SCLK  13
#define PIN_DIN   11
#define PIN_DC     5
#define PIN_CS     7
#define PIN_RST    6

Adafruit_PCD8544 display = Adafruit_PCD8544(PIN_SCLK, PIN_DIN, PIN_DC, PIN_CS, PIN_RST);

// Pinos dos sensores e outros aparatos do projeto
#define PIN_UMIDADE A0
#define PIN_RELE     2
#define PIN_BUZZER   3
#define PIN_LED_VERDE 4
/* Essa linha cria um objeto chamado rtc, para que depois eu possa usa-lo nas linhas abaixo,
pra eu poder pegar a hora e data atual, e usar as seguintes funções rtc.begin() e
rtc.now() */
RTC_DS3231 rtc;
/* Aqui defino onde vai ficar cada mensagem a ser exposta como a hora na linha tal,
a data na linha tal, para facilitar o código posteriormente */
void mostrarMensagem(String linha1, String linha2, String hora = "", String data = "") {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(linha1);
  display.setCursor(0, 10);
  display.println(linha2);
  if (hora != "") {
    // nesse caso aqui define onde fica a hora, na linha 3
    display.setCursor(0, 20);
    display.println(hora);
  }
  if (data != "") {
    // e nesse caso aqui define onde fica a data, na linha 4
    display.setCursor(0, 30);
    display.println(data);
  }
  display.display();
}

void setup() {
  pinMode(PIN_RELE, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LED_VERDE, OUTPUT);

  digitalWrite(PIN_RELE, LOW);
  digitalWrite(PIN_BUZZER, LOW);
  digitalWrite(PIN_LED_VERDE, LOW);

  Serial.begin(9600);

  display.begin();
  // definindo as specificidades do LCD, visuais
  display.setContrast(60);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);
  // mensagem a ser exposta inicialmente 
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Iniciando");
  display.setCursor(0, 10);
  display.println("Bomba");
  display.setCursor(0, 20);
  display.println("d'gua");
  display.setCursor(0, 30);
  display.println("...");
  display.display();
  delay(4000);

  Wire.begin();
  if (!rtc.begin()) {
    delay(1000);
    /* se ele não identificar sinal do RTC, ver se ele está 
     ou não funcionando e caso não, 
     retorna com a mensagem '' RTC'' ''OFF'' */
    mostrarMensagem("RTC", "OFF");
  }
}

void loop() {
  // a variavel leitura é o que vai ser lido no pino do sensor de umidade
  int leitura = analogRead(PIN_UMIDADE);
  
  Serial.print("Umidade lida: ");
  Serial.println(leitura);
// strings definidas anteriormente, lá em cima, que irão aparecer a hora e a data
  String horaStr = "", dataStr = "";
  /* isso tenta iniciar o RTC, se der certo ele entra no bloco e pega os dados, 
  se o RTC não for encontrado, esse bloco é ignorado */
  if (rtc.begin()) {
    /* aqui ele cria um objeto agora que contém a data e hora atual do RTC 
    podendo ver o dia, mês, ano, hora, minuto e segundo*/
    DateTime agora = rtc.now();
    // formatando a hora, como ela vai aparecer para nós
    horaStr = String(agora.hour()) + ":" +
              (agora.minute() < 10 ? "0" : "") + String(agora.minute()) + ":" +
              (agora.second() < 10 ? "0" : "") + String(agora.second());

    // formatando a data, como ela irá aparecer para nós
    dataStr = String(agora.day()) + "/" +
              String(agora.month()) + "/" +
              String(agora.year());
  }

  if (leitura > 850) {
  /* quando a umidade for menor que 300 , retorna umidade baixa
  e então o buzzer aciona e o relé é ativado, ligado */
  digitalWrite(PIN_RELE, HIGH);
  digitalWrite(PIN_BUZZER, HIGH);
  digitalWrite(PIN_LED_VERDE, LOW);
  mostrarMensagem("umidade", "baixa", horaStr, dataStr);
}
else if (leitura >= 351 && leitura <= 850) {
  /* quando a umidade for maior ou igual a 300 e menor ou
   igual a 500 , retorna umidade normal e led,buzzer e relé não acionam*/
  digitalWrite(PIN_RELE, LOW);
  digitalWrite(PIN_BUZZER, LOW);
  digitalWrite(PIN_LED_VERDE, LOW);
  mostrarMensagem("umidade", "normal", horaStr, dataStr);
}
else {
  /* quando a umidade for maior que 500, retorna umidade alta
  e aciona a led */
  digitalWrite(PIN_RELE, LOW);
  digitalWrite(PIN_BUZZER, LOW);
  digitalWrite(PIN_LED_VERDE, HIGH);
  mostrarMensagem("umidade", "alta", horaStr, dataStr);
}
  delay(5000);

  // reinicia a análise
  digitalWrite(PIN_RELE, LOW);
  digitalWrite(PIN_BUZZER, LOW);
  digitalWrite(PIN_LED_VERDE, LOW);

  mostrarMensagem("analisando", "novamente");
  delay(2000);
}
