//Comunicación MQTT con servo empregando ESP8266

#include <ESP8266WiFi.h>
#include <Servo.h>
#include <PubSubClient.h>


//wifi Mobil
#define MAX_INTENTOS 50
#define WIFI_SSID "WiFi.leite"
#define WIFI_PASS "pass"


// MQTT
// Datos MQTTHQ brocker en mqtthq.com
// URL: public.mqtthq.com  //TCP Port: 1883
//WebSocket Port(porto): 8083
//Websocket Path (camiño): /mqtt
#define MQTT_HOST IPAddress(52, 13, 116, 147)   
#define MQTT_NOME_PORT 1883                   

// Servo
#define SERVOPIN 0                                
Servo servo;                                      
#define MQTT_PUB_SERVO "wemos/robotica/servo"     
#define MQTT_NOME_CLIENTE "Cliente servo"       

WiFiClient espClient;                             
PubSubClient mqttClient(espClient);              
 
// Pins datos
// GPI014 D5
#define LED 14

bool conectado = false;
int tempo = 500;
int posicion = 0;

void setup () {                                       
Serial.begin(115200);                                 
pinMode(LED, OUTPUT); 
servo.attach(SERVOPIN);                               
conectado = conectarWiFi();                          
mqttClient.setServer (MQTT_HOST, MQTT_NOME_PORT);          
mqttClient.setCallback (callback);                    

}
void loop () {
  if(conectado) escintila(tempo);
  else escintila(tempo/10);
  
}

void escintila(int espera){
  digitalWrite(LED, HIGH);
  delay (espera);
  digitalWrite(LED, LOW);
  delay(espera);
}

//Funcion que se encarga de xestionar a conexion a rede
bool conectarWiFi() {
  WiFi.mode(WIFI_STA);                                                     
  WiFi.disconnect();                                                      
  WiFi.begin(WIFI_SSID, WIFI_PASS);                                       
  Serial.print("\n\nAgardando pola WiFi");    
  int contador = 0;                                                      
  while(WiFi.status() != WL_CONNECTED and contador < MAX_INTENTOS){      
    contador++;
    delay (500);
    Serial.print(".");
    
    }
    Serial.println("");
    //INFORMA DO ESTADO DA CONEXION IP E EN CASO DE EXITO
    if(contador < MAX_INTENTOS){
    Serial.print("Conectado a WiFi coa IP: "); Serial.println(WiFi.localIP());
    conectado = true;
  }
  else {
    Serial.print("No se pudo conectar a WiFi");
    conectado = false;
    }

  return (conectado);
  }

  //FUNCIÓNS PARA CONEXIÓN SERVIDOR MQTT
 /* ====================================================================================================
 
     -Definición da función callback local
     -É chamada polo método callback do obxeto que descrebe a conexión MQTT, cada
     vez que un dispositivopublica unha mensaxe nova nun canal (topic) ao que está 
     suscrito este ESP8266.

     -Nesta función vai tamén a lóxica que fai accionar os actuadores (servo) conforme
     o mensaxe que reciban no topic ao que estén subscritos.

     -Os sensores publican desde a función ´loop()´ non desde esta función. 

     ================================================================================================
     */

     void callback (String topic, byte* message, unsigned int len) {          
      Serial.print("nova mensaxe no topic:  "      ); Serial.print(topic);    
      Serial.print(". Mensaxe: ");                                             
      String mensaxeTmp = "";                                                  
      for(int i=0; i< len; i++) {                                            
        Serial.print((char)message[i]);                                       
        mensaxeTmp += (char)message[i];                                        
      }
      Serial.println();

      // lóxica que se executa ao recibir o payload
      accionarServo(mensaxeTmp);                                             

     }
     
      /*=============================================================================================================================

      -Definicion da funcion reconnect local ( metodo do obxecto 'mqttClient).
      -Cada vez que se incluia unha nova iteracion do 'loop(), compróbase que existe a
      conexión ao servidor MQTT. Se non é así chámase a esta función.

          -Encárgase de:
                -conectar ao servidor MQTT
                -comunicar por saida serie o estado da conexion MQTT
                -subscribir os sensores/actuadores declarados no topic correspondente
      ==============================================================================================================================
      */
      void reconnect (){
        //Mentres non se reconecta ao servidor MQTT
        while(!espClient.connected()) {
          Serial.print("Tentando conectar ao sevidor MQTT. . .");
          if(mqttClient.connect(MQTT_NOME_CLIENTE)) {
            Serial.print(" Conectado ");
            mqttClient.subscribe(MQTT_PUB_SERVO);

          }
          else {
            Serial.print("Fallo ao conectar ao servidor MQTT, rc=");
            Serial.print(mqttClient.state());
            Serial.println( " nova tentativa en 5 s");
            delay(5000);
          }
        }
      }

     void accionarServo(String orde) {   // declaramos a funcion
          //comprobamos se hai orde no teclado
      orde.toLowerCase();
      if(orde.equals("esquerda")) posicion = 180;
      else if(orde.equals("dereita")) posicion = 0;
      else if(orde.equals("centro")) posicion = 90;
      else {
        int tmp = orde.toInt();
        if(tmp >=0 && tmp <=180) posicion = tmp;
        else posicion = 0 ;

    }
    servo.write(posicion);
    delay(tempo);
     }
