#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "SIHIPO"; /* wifi ssid */
const char* password = "sistemhidroponik"; /* wifi psk */
const String device_id = "C01";
const String device_type = "SIHIPO_C";
const boolean isAP = true;

ESP8266WebServer server(80);

boolean pin[9];
int pinMap[] = {D0, D1, D2, D3, D4, D8, D7, D6, D5}; /* Physical PIN Mapping */

void handleRoot() {
  String out = "{\"id\":\"" + device_id + "\",\"type\":\"" + device_type + "\",\"value\":[";
  for(int c = 0; c < sizeof(pin); c++) {
    int stat = pin[c] ? 1 : 0;
    /* String out2 = "\"p";
    out2 += c;
    out2 += "\":";
    out2 += stat;
    out2 += " "; */
    String out2 = "";
    out2 += stat;
    out2 += " ";
    out += out2;
  }
  out.trim();
  out.replace(" ", ",");
  out += "]}";
  server.send(200, "application/json", out);
}

void handleUI() {
  String out = "";
  out += "<html>";
  out += "  <body>";
  out += "    <script>";
  out += "      function setPin(pin, stat) {";
  out += "        var xhttp = new XMLHttpRequest();";
  out += "        xhttp.open(\"GET\", \"/\" + stat + \"?p=\" + pin, true);";
  out += "        xhttp.send();";
  out += "      }";
  out += "    </script>";
  out += "    <pre>";
  out += "      PIN 01 : <input type=\"button\" value=\"ON\" onclick=\"setPin(1,1)\" /><input type=\"button\" value=\"OFF\" onclick=\"setPin(1,0)\" /><br />";
  out += "      PIN 02 : <input type=\"button\" value=\"ON\" onclick=\"setPin(2,1)\" /><input type=\"button\" value=\"OFF\" onclick=\"setPin(2,0)\" /><br />";
  out += "      PIN 03 : <input type=\"button\" value=\"ON\" onclick=\"setPin(3,1)\" /><input type=\"button\" value=\"OFF\" onclick=\"setPin(3,0)\" /><br />";
  out += "      PIN 04 : <input type=\"button\" value=\"ON\" onclick=\"setPin(4,1)\" /><input type=\"button\" value=\"OFF\" onclick=\"setPin(4,0)\" /><br />";
  out += "      PIN 05 : <input type=\"button\" value=\"ON\" onclick=\"setPin(5,1)\" /><input type=\"button\" value=\"OFF\" onclick=\"setPin(5,0)\" /><br />";
  out += "      PIN 06 : <input type=\"button\" value=\"ON\" onclick=\"setPin(6,1)\" /><input type=\"button\" value=\"OFF\" onclick=\"setPin(6,0)\" /><br />";
  out += "      PIN 07 : <input type=\"button\" value=\"ON\" onclick=\"setPin(7,1)\" /><input type=\"button\" value=\"OFF\" onclick=\"setPin(7,0)\" /><br />";
  out += "      PIN 08 : <input type=\"button\" value=\"ON\" onclick=\"setPin(8,1)\" /><input type=\"button\" value=\"OFF\" onclick=\"setPin(8,0)\" /><br />";
  out += "    </pre>";
  out += "  </body>";
  out += "</html>";
  server.send(200, "text/html", out);
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
  if (isAP) {
    delay(1000);
    Serial.begin(115200);
    Serial.println();
    Serial.print("Configuring access point...");
    WiFi.softAP(ssid, password);
  
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
  } else {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    Serial.println("");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  for(int c = 0; c < sizeof(pin); c++) {
    pinMode(pinMap[c], OUTPUT);
    pin[c] = false;
    handlePin(c, false);
  }

  server.on("/", [](){
    for (uint8_t i=0; i<server.args(); i++){
      if (server.argName(i).equals("p")) {
        boolean ar[sizeof(pin)];
        for (uint16_t i2 = 0; i2 < sizeof(pin); i2++) {
          ar[i2] = server.arg(i).toInt() & (1 << i2) ? true : false;
          handlePin(i2, ar[i2]);
        }
      }
    }
    handleRoot();
  });

  server.on("/ui", handleUI);

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
