#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include <MKRNB.h>
#include <Ultrasonic.h>

// Debug options
#define PRINT_AT        true   // Show or hide AT command output from the modem
#define USRF_PIN 1

// PIN Number
const char PINNUMBER[]     = "";
char apn[] = "soracom.io";
char user[] = "sora";
char pass[] = "sora";

// Server details
const char server[] = "unified.soracom.io";
const int  port = 80; 


NBClient client;
GPRS gprs;
NB nbAccess(PRINT_AT);
HttpClient http(client, server, port);

Ultrasonic usrf(USRF_PIN);

// Alert Range in CM
// If the Ultrasonic sensor sees any value 
// less than or equal to this value, it will send an alert
int alertRange = 10;

// connection state
bool connected = false;

// Publish interval
long previousMillis = 0; 
long interval = 20000; // milliseconds

void setup() {
  pinMode(SARA_RESETN, OUTPUT);
  digitalWrite(SARA_RESETN, LOW);
  pinMode(SARA_PWR_ON, OUTPUT);
  
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  start_and_connect_modem();
}

unsigned long last_sent = millis(); 

void loop() {
  int rangeToTarget = usrf.MeasureInCentimeters();
  Serial.print(F("Range to target : "));
  Serial.print(rangeToTarget, DEC);
  Serial.println(F(" CM"));

  // Make sure the device is still connected to CatM network
  if (nbAccess.isAccessAlive()) {
    if(millis() - last_sent >= 1000) {
      if(rangeToTarget < alertRange) {
        last_sent = millis();
        // Construct the JSON data to send
        StaticJsonDocument<200> doc;
        doc["time"] = millis();
        doc["range_cm"] = rangeToTarget;
        char jsonBuffer[512];
        serializeJson(doc, jsonBuffer); // print to client
        post_data(jsonBuffer); 
      }
    }
  } else {
    Serial.println("Modem disconnected, reconnecting");
    connected = false;
    connect_modem();
  }

  delay(50);
}



void post_data(String postData) {
  Serial.println("making POST request");
  String contentType = "application/json";

  http.post("/", contentType, postData);

  // read the status code and body of the response
  int statusCode = http.responseStatusCode();
  String response = http.responseBody();

  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);

}

void start_and_connect_modem(){
  // Send Poweron pulse
  digitalWrite(SARA_PWR_ON, HIGH);
  delay(300);
  digitalWrite(SARA_PWR_ON, LOW);
  delay(1000);
  // Turn on the modem
  if (Serial) Serial.print(">>> Turning on modem...");
  if (MODEM.begin()) {
    if (Serial) Serial.println(" done!");
  } else {
    if (Serial) Serial.println(" error, could not turn on modem! Try power-cycling.");
    return;
  }

  while (!MODEM.noop());
  MODEM.sendf("ATI9");
  MODEM.waitForResponse(2000);
  
  // Run AT commands to reset the modem to global default settings
  Serial.print(">>> Resetting modem to default settings...");
  MODEM.sendf("AT+CFUN=0");
  MODEM.waitForResponse(6000);
  MODEM.sendf("AT+UMNOPROF=2");
  MODEM.waitForResponse(6000);
  MODEM.sendf("AT+CFUN=15");
  MODEM.waitForResponse(6000);
  MODEM.sendf("AT+CFUN=0");
  MODEM.waitForResponse(6000);
  MODEM.sendf("AT+UMNOPROF=0");
  MODEM.waitForResponse(6000);
  MODEM.sendf("AT+CFUN=15");
  MODEM.waitForResponse(6000);
  MODEM.sendf("AT+URAT=7");
  MODEM.waitForResponse(6000);
  MODEM.sendf("AT+UBANDMASK?");
  MODEM.waitForResponse(6000);
  Serial.println(" done!");
  
  delay(2000);
  // attempt to connect to GSM and GPRS:
  Serial.print("Attempting to connect to GSM and GPRS");
   
  connect_modem();
  
}

void connect_modem(){
  // After starting the modem with GSM.begin()
  // attach the shield to the GPRS network with the APN, login and password
  while (!connected) {
    if ((nbAccess.begin(PINNUMBER, apn, user, pass) == NB_READY) &&
        (gprs.attachGPRS() == GPRS_READY)) {
      connected = true;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("You're connected to the network");  
}
