#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "Samsung Z2"; /* wifi ssid */
const char* password = "poiuytrewq"; /* wifi psk */

ESP8266WebServer server(80);

boolean pin[9];
int pinMap[] = {D0, D1, D2, D3, D4, D5, D6, D7, D8};

void handleRoot() {
  String out = "{";
  for(int c = 0; c < sizeof(pin); c++) {
    int stat = pin[c] ? 1 : 0;
    String out2 = "\"p";
    out2 += c;
    out2 += "\":";
    out2 += stat;
    out2 += " ";
    out += out2;
  }
  out.trim();
  out.replace(" ", ",");
  out += "}";
  server.send(200, "application/json", out);
}

void handlePin(int a, boolean onoff) {
  pin[a] = onoff;
  /* int stat = pin[a] ? 0 : 1; */
  digitalWrite(pinMap[a], !pin[a]);
  /* handleRoot(); */
}

void handlePin(int a) {
  handlePin(a, !pin[a]);
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void){
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  for(int c = 0; c < sizeof(pin); c++) {
    pinMode(pinMap[c], OUTPUT);
    pin[c] = false;
    handlePin(c, false);
  }

  server.on("/0", [](){
    if (server.args()==0) {
      for(int c = 0; c < sizeof(pin); c++) {
        handlePin(c, false);
      }
    }
    for (uint8_t i=0; i<server.args(); i++){
      if (server.argName(i).equals("p")) {
        handlePin(server.arg(i).toInt(), false);
      }
    }
    handleRoot();
  });

  server.on("/1", [](){
    if (server.args()==0) {
      for(int c = 0; c < sizeof(pin); c++) {
        handlePin(c, true);
      }
    }
    for (uint8_t i=0; i<server.args(); i++){
      if (server.argName(i).equals("p")) {
        handlePin(server.arg(i).toInt(), true);
      }
    }
    handleRoot();
  });

  server.on("/2", [](){
    if (server.args()==0) {
      for(int c = 0; c < sizeof(pin); c++) {
        handlePin(c);
      }
    }
    for (uint8_t i=0; i<server.args(); i++){
      if (server.argName(i).equals("p")) {
        handlePin(server.arg(i).toInt());
      }
    }
    handleRoot();
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
