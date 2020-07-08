#include <WiFi.h>
#include <MFRC522.h>  // MFRC522 RFID module library.
#include <SPI.h>      // SPI device communication library.
#include <Servo.h>

#define LEDVERMELHO 32
#define LEDVERDE 12
#define BUTTONP1 26
#define PINO 0
#define PINOTWO 2

char msg[100];
boolean wordEmpty = true;

int button_corrente;
  
boolean buttonOne = false;

static const int servoPin = 16;

Servo servo1;

/*char ssid[] = "MEO-B9F320";
char pass[] = "80a1846007";*/
char ssid[] = "Galaxy A711588";
char pass[] = "12345678";

/*char ssid[] = "HUAWEIjulio";
char pass[] = "Juliodaal";*/

#define SERVER_PORT 50000
#define LOCAL_PORT 50001

#define SS_PIN    21
#define RST_PIN   22

IPAddress server_IP(192, 168, 43, 100);
WiFiUDP udp;
char buffer[100];

char msge = 'A';
String code;
byte readCard[6];     // Array that will hold UID of the RFID card.d

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Creates MFRC522 instance.
MFRC522::MIFARE_Key key;          // Creates MIFARE key instance.


/**********************************************************************
 * 
 * void servo(uint8_t value, IPAddress &address, uint16_t port) {
 *
 **********************************************************************/

void servoMotor() {
  
   servo1.attach(servoPin);
    delay(500);
    
      //.......................
      
      for(int posDegrees = 0; posDegrees <= 100; posDegrees++) {
        servo1.write(posDegrees);
        delay(20);
      }
      servo1.detach();
      servo1.attach(servoPin);
      delay(5000);
      for(int posDegrees = 100; posDegrees >= 0; posDegrees--) {
          servo1.write(posDegrees);
          delay(20);
      } 
      servo1.detach();
}

/**********************************************************************
 * 
 * void send_UDP_packet(uint8_t value, IPAddress &address, uint16_t port) {
 *
 **********************************************************************/
void receive_UDP_packet(uint16_t udp_lenght) {
    
    if (udp_lenght >= sizeof(buffer)) udp_lenght = sizeof(buffer)-1;

    Serial.print("Rx ");
    Serial.print(udp_lenght);
    Serial.print(" bytes from ");

    Serial.print(udp.remoteIP());
    Serial.print(":");
    Serial.println(udp.remotePort());

    udp.read(buffer, udp_lenght);

    buffer[udp_lenght] = '\0';
    Serial.println(buffer);
    String x ="";
    x = String(x + buffer);
    Serial.print("Esta es la X : ");
    Serial.println(x);
      if(x == "true"){
        Serial.println("Acesso concedido");  
        digitalWrite(LEDVERDE,HIGH);
        servoMotor();
        delay(500);
        digitalWrite(LEDVERDE,LOW);
        
      }
      if(x == "false"){
        Serial.println("Acesso restrito"); 
        digitalWrite(LEDVERMELHO,HIGH);
        delay(3000);
        digitalWrite(LEDVERMELHO,LOW);
      }
      if(x == "trueP"){
        digitalWrite(PINO,HIGH);
        delay(500);
        digitalWrite(PINO,LOW);
        digitalWrite(LEDVERDE,LOW);
        
      } 
      if(x == "falseP"){
        Serial.println("Acesso restrito");
        digitalWrite(PINOTWO,HIGH);
        delay(500);
        digitalWrite(PINOTWO,LOW);
        digitalWrite(LEDVERMELHO,HIGH);
        delay(3000);
        digitalWrite(LEDVERMELHO,LOW);
      }
    }



void send_UDP_packet(uint8_t value, IPAddress &address, uint16_t port) {

    Serial.print("Sending packet to ");
    Serial.print(address);
    Serial.print(":");
    Serial.print(port);
    Serial.print(", Value = ");
    Serial.println(msg);

    udp.beginPacket(address, port);
    udp.write((uint8_t *)&msg, sizeof(msg));
    udp.endPacket();
    }
/**********************************************************************
 * 
 *  void setup(void)
 *
 **********************************************************************/
void setup( void ) {

    Serial.begin(115200);
    Serial.println();
    Serial.println();

    pinMode(LEDVERMELHO, OUTPUT);
    digitalWrite(LEDVERMELHO,LOW);
    pinMode(LEDVERDE, OUTPUT);
    digitalWrite(LEDVERDE,LOW);
    
    pinMode(BUTTONP1,INPUT_PULLDOWN);

    pinMode(PINO,OUTPUT);
    pinMode(PINOTWO,OUTPUT);

    // Ligação à rede Wireless  
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        }
    Serial.println("");

    Serial.println("WiFi connected");
    Serial.print("Endereço ip = ");
    Serial.print(WiFi.localIP());
    SPI.begin(); // Init SPI bus
    mfrc522.PCD_Init();
    
    // Inicia o carater a enviar.
    }
/**********************************************************************
 * 
 *  void loop(void)
 *
 **********************************************************************/
void loop() {
  button_corrente = digitalRead(BUTTONP1);

  if(button_corrente == HIGH){
      buttonOne = true;
      Serial.println(1);
  }

  
  if(Serial.available() > 0){
     char capture = Serial.read();
     wordEmpty = false;
  }
  getID();  
  
  delay(1);
 
  uint16_t udp_lenght = udp.parsePacket();
  if (udp_lenght) receive_UDP_packet(udp_lenght);
  
  }

int getID() {
// Function that will read and print the RFID cards UID.


  if ( ! mfrc522.PICC_IsNewCardPresent())  // If statement that looks for new cards.
  {
    return 1;
  }

  if ( ! mfrc522.PICC_ReadCardSerial())    // If statement that selects one of the cards.
  {
    return 1;
  }
  
  //Serial.println(mfrc522.uid.size);
  Serial.print("UID: ");  
  code="";
  for (int i = 0; i < 6; i++) {  
    delay(100);
    readCard[i] = mfrc522.uid.uidByte[i];   // Reads RFID cards UID.
    Serial.print(readCard[i], HEX); 
     
    code = code + String(readCard[i],HEX);
    mfrc522.PICC_HaltA();
  } 
  memset(mfrc522.uid.uidByte , 0, mfrc522.uid.size);
    String preco = "";
    String nome  = "";
    String cartao = "cancela#%#";
    if(buttonOne == true){
        cartao = "cancelaPagamentoProduto#%#";
        preco = "1.5";
        nome = "Agua#%#";
    }
    cartao = String(cartao + code + "#%#");
    if(buttonOne == true){
      cartao = String(cartao + preco + "#%#" + nome);
       buttonOne = false;
    }
    cartao.toCharArray(msg, 100);
    Serial.println("");
    send_UDP_packet(msge, server_IP, SERVER_PORT);
}
