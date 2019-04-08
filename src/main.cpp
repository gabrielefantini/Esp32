#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress server(192, 168, 1, 245);

#define RELAY1 13 // pin ai quali sono connessi i relè
#define RELAY2 12
#define RELAY3 27
#define RELAY4 33
//pin ai quali sono connessi i bottoni 
#define BUTTON1 21
#define BUTTON2 4
#define BUTTON3 36 
#define BUTTON4 39
//-----------parte relativa al controllo dei bottoni
const int numberOfButton = 4;
long unsigned int timeOut[numberOfButton] = {};
void handler(int n);
void clicked(int par){        //invoco l'handler relativo al bottone premuto e avvio il timeout
    if(par>=numberOfButton){Serial.println("clicked out of par"); return;}
    if (millis()-timeOut[par]>700||millis()-timeOut[par]<0) {
        handler(par);
        timeOut[par]=millis();
    } 
}
//-----------fine parte relativa al controllo dei bottoni

//-----------funzioni relative al controllo dei rele
String AttivaRele(int numerorele) {
  if (numerorele == 1)
    digitalWrite(RELAY1, LOW);
  if (numerorele == 2)
    digitalWrite(RELAY2, LOW);
  if (numerorele == 3)
    digitalWrite(RELAY3, LOW);
  if (numerorele == 4)
    digitalWrite(RELAY4, LOW);
  String ret = "on";
  return ret;
}
String DisattivaRele(int numerorele) {
  if (numerorele == 1)
    digitalWrite(RELAY1, HIGH);
  if (numerorele == 2)
    digitalWrite(RELAY2, HIGH);
  if (numerorele == 3)
    digitalWrite(RELAY3, HIGH);
  if (numerorele == 4)
    digitalWrite(RELAY4, HIGH);
  String ret = "off";
  return ret;
}
//-----------fine funzioni relative ai rele

void callback(char* topic, byte* payload, unsigned int length);

EthernetClient ethClient;
PubSubClient client(ethClient);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic","hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
      client.subscribe("AttivaRele");
      client.subscribe("DisattivaRele");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(9600);
  //inizializzo i timeout dei bottoni
  for(int i = 0; i < numberOfButton; i++) //inizializzo i timeout a 0
    {
        timeOut[i]=0;
    }
  //setto le specifiche del client per l'mqtt
  client.setServer(server, 1883);
  client.setCallback(callback);
  //setto le impostazioni dell'ethernet
  Ethernet.init(33);
  Ethernet.begin(mac);
  // Allow the hardware to sort itself out
  pinMode(RELAY1, OUTPUT); //setto la modalità dei pin digitali relativi ai relè
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);
  pinMode(BUTTON1, INPUT); //setto la modalità dei pin digitali relativi ai bottoni
  pinMode(BUTTON2, INPUT);
  pinMode(BUTTON3, INPUT);
  pinMode(BUTTON4, INPUT);
  delay(1500);
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  if (digitalRead(BUTTON1)== LOW) {
        clicked(0);
  }
  if (digitalRead(BUTTON2)== LOW) {
      clicked(1);
  }
  if (digitalRead(BUTTON3)== LOW) {
      clicked(2);
  }   
  if (digitalRead(BUTTON4)== LOW) {
      clicked(3);
  }
  client.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message = "";
  String top = String(topic);
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
    message += (char)payload[i];
  }
  Serial.println();
  if(top =="AttivaRele"){
    int n = message.toInt();
    String attivo = AttivaRele(n);
    int m = sizeof(attivo);
    char val[m];
    strncpy(val, attivo.c_str(),m);
    if(n==1)
    client.publish("checkrele1",val);
    if(n==2)
    client.publish("checkrele2",val);
    if(n==3)
    client.publish("checkrele3",val);
    if(n==4)
    client.publish("checkrele4",val);
    Serial.println(val);
  }
  if(top == "DisattivaRele"){
    int n = message.toInt();
    String disattivo = DisattivaRele(n);
    int m = sizeof(disattivo);
    char val[m];
    strncpy(val, disattivo.c_str(), m);
    if(n==1)
    client.publish("checkrele1",val);
    if(n==2)
    client.publish("checkrele2",val);
    if(n==3)
    client.publish("checkrele3",val);
    if(n==4)
    client.publish("checkrele4",val);
    Serial.println(val);
 }
}
void handler(int n){
    Serial.println("Clickedd!!!!!!!!");
    Serial.println(n);
    String valore = String(n);
    int m = sizeof(valore);
    char val[m];
    strncpy(val, valore.c_str(), m);
    client.publish("clicked",val);
    //pubblica via mqtt che il bottone è stato schiacciato
}