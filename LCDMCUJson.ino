#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#define BUZZERPIN 16
#define BTNVERT 12
#define BTNROUGE 14

ESP8266WiFiMulti WiFiMulti;

const char* ssid = "IoE Server";
const char* password = "projetLinkyByMaker2017";
bool alarm = false;
int printedInfo =0;
int conCompte =0;
int compteurSendingIoTinf=0;


// Construct an LCD object and pass it the 
// I2C address, width (in characters) and
// height (in characters). Depending on the
// Actual device, the IC2 address may change.
LiquidCrystal_I2C lcd(0x3F, 16, 2);

struct IoTobject{
  IPAddress ip;
  char* id;
  char* device_type;
  
  void printconsole(){
     Serial.println("info into IoTObject ");
     Serial.print(" IP: ");
     Serial.println(ip);
     Serial.print(" id: ");
     Serial.println(id);
     Serial.print(" device: ");
     Serial.println(device_type);
  }
};
IoTobject monit;


struct InfoEnergie{
  int IINST;
  int ISOUSC; 
  int IMAX; 
  char PTEC[4]; 
  float seuil = 0.9;
  
  void printconsole(){
    Serial.println("info InfoEnergie ");
     Serial.print(" IINST: ");
     Serial.println(IINST);
     Serial.print(" ISOUSC: ");
     Serial.println(ISOUSC);
     Serial.print(" IMAX: ");
     Serial.println(IMAX);
     Serial.print(" PTEC: ");
     Serial.println(PTEC);
     Serial.print(" seuil: ");
     Serial.println(seuil);
  }
};
InfoEnergie infoEnergie;

HTTPClient http;

void setup() {
   Serial.begin(9600);
  while (!Serial) {
    // wait serial port initialization
  }
  for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }

    WiFiMulti.addAP("IoE Server", "projetLinkyByMaker2017");

  // The begin call takes the width and height. This
  // Should match the number provided to the constructor.
  lcd.begin(16,2);
  lcd.init();

  // Turn on the backlight.
  lcd.backlight();
  
  pinMode(BUZZERPIN, OUTPUT);
  pinMode(BTNVERT, INPUT);
  pinMode(BTNROUGE, INPUT);

 monit.ip=WiFi.localIP();
 monit.device_type = "monitor";
 monit.id="00001";
 
}

void loop() {
  if(compteurSendingIoTinf == false){
      compteurSendingIoTinf=sendIotInfo();
  }
  //monit.printconsole();
   if((WiFiMulti.run() == WL_CONNECTED)) {
        
        http.begin("http://192.168.42.1:5000/monitoring/");
        int httpCode = http.GET();
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] GET monitoring code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                //Serial.println(payload);
                JSONparsing(payload);
                /*String inputest = "{\"IINST\": 15, \"PTEC\": \"HPJR\", \"ISOUSC\": 45, \"IMAX\": 90}"; 
                JSONparsing(inputest);*/
                printScreen();
                delestage();
            }
        } else {
            //Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
            lcd.setCursor(0, 0);
            lcd.print("error:          ");
            lcd.setCursor(0, 1);
            lcd.print("con. failed     ");
        }

        http.end();
    }
}

void JSONparsing(String stg){
  int beginningJSON = stg.indexOf('{');
  int endJSON = stg.lastIndexOf('}')+1;
  String jsonAlone=stg.substring(beginningJSON,endJSON);
  jsonAlone.replace("&#34;","\"");
  const size_t bufferSize = JSON_OBJECT_SIZE(5) + 60;
  DynamicJsonBuffer jsonBuffer(bufferSize);
  //Serial.print(stg.substring(beginningJSON,endJSON));
  JsonObject& root = jsonBuffer.parseObject(jsonAlone);
  //JsonObject& root = jsonBuffer.parseObject(inputest);

  // Test if parsing succeeds.
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }
  infoEnergie.IINST = root["IINST"]; // "000"
  infoEnergie.ISOUSC = root["ISOUSC"]; // "00"
  infoEnergie.IMAX = root["IMAX"]; // "00"
  //infoEnergie.printconsole();
}

void printScreen(){

  switch(printedInfo){
    case 0:
    lcd.setCursor(0, 0);
    lcd.print("IINST           ");
    lcd.setCursor(10, 0);
    lcd.print(infoEnergie.IINST);
    lcd.setCursor(0, 1);
    lcd.print("ISOUSC          ");
    lcd.setCursor(10, 1);
    lcd.print(infoEnergie.ISOUSC);
    break;
    case 1:
    lcd.setCursor(0, 0);
    lcd.print("ISOUSC          ");
    lcd.setCursor(10, 0);
    lcd.print(infoEnergie.ISOUSC);
    lcd.setCursor(0, 1);
    lcd.print("IMAX            ");
    lcd.setCursor(10, 1);
    lcd.print(infoEnergie.IMAX);
    break;
    case 2:
    lcd.setCursor(0, 0);
    lcd.print("IMAX            ");
    lcd.setCursor(10, 0);
    lcd.print(infoEnergie.IMAX);
    lcd.setCursor(0, 1);
    lcd.print("                ");
    //lcd.setCursor(10, 1);
    //lcd.print(infoEnergie.ISOUSC);
    break;
  }
  if(digitalRead(BTNVERT) == HIGH){
    printedInfo +=1;
    if(printedInfo>2){
      printedInfo = 0;
    }
  }
}
void delestage(){
  long calcul =  infoEnergie.ISOUSC*infoEnergie.seuil;
  //Serial.println(calcul);
  if(infoEnergie.IINST >= calcul){
    alarm = true;
  }else{
    alarm = false;
  }
  
  if(alarm == true){
    digitalWrite(BUZZERPIN, HIGH);
  }else{
    digitalWrite(BUZZERPIN, LOW);
  }
  
  if(alarm == true && digitalRead(BTNROUGE) == HIGH){
    http.begin("http://192.168.42.1:5000/delesting/");
    int delestingReqCode = http.GET();
    if(delestingReqCode > 0) {
      Serial.printf("[HTTP] GET delesting code: %d\n", delestingReqCode);
      if(delestingReqCode == HTTP_CODE_OK) {
        for(int i; i<=1000;i++){
          lcd.setCursor(0, 0);
          lcd.print("Delestage       ");
          lcd.setCursor(0, 0);
          lcd.print("en cours        ");
        }
      }
    }
  }
}
bool sendIotInfo(){
  http.begin("http://192.168.42.1:5000/objectport/");
  http.addHeader("Content-Type", "application/json");
  /*String s= "{\"ip_adress\": ";
  s+=monit.ip;
  s+=", \"id\": ";
  s+=monit.id;
  s+=",\"device_type\": ";
  s+=monit.device_type;
  s+="}";
  int postCode = http.POST(s);*/
   int postCode = http.POST("{\"ip_adress\": \"192.168.42.17\", \"id\": \"00001\",\"device_type\": \"monitor\"}");
  http.writeToStream(&Serial);
  if (postCode == HTTP_CODE_OK){
    return true; 
  }else{
    return false;
  }
}

