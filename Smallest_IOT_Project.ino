/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

#define DEBUG 0 // Turn it to 1 to show results in Serial Monitor (Do not turn it 1 for ESP8266 01 module)


/* Set these to your desired credentials. */
const char *ssid = "ESPap";
const char *pass = "thereisnospoon";
const char* space = "%20";
int connection_flag;
WiFiServer server(80);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/
int x = 0;
void setup() {

  if(DEBUG == 1)
  {
  Serial.begin(115200);
  Serial.println();
  }
  delay(5000);
  if (WiFi.status() == WL_CONNECTED)
  {
     if(DEBUG == 1){
    Serial.println("Connected with previously added WiFi router");
    Serial.println(WiFi.localIP());
     }
    connection_flag = 0;
  }
  else
  {
   if(DEBUG == 1)  
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, pass);

  IPAddress myIP = WiFi.softAPIP();
   if(DEBUG == 1){
  Serial.print("AP IP address: ");
  Serial.println(myIP);
   }
  server.begin();
   if(DEBUG == 1)
  Serial.println("HTTP server started");
  connection_flag = 1;
  
  }
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {

  if (connection_flag == 1)
  {
    WiFiClient client = server.available();
    if (!client) {
      return;
    }

    // Wait until the client sends some data
     if(DEBUG == 1)
    Serial.println("new client");
    while (!client.available()) {
      delay(1);
    }

    // Read the first line of the request
    String req = client.readStringUntil('\r');
     if(DEBUG == 1)
    Serial.println(req);
    client.flush();

    char* cmp = "parameters";

    char* string_char = &req[0];

    if (strstr(string_char, cmp))
    {
      x = 0;
      while (req.charAt(x) != '?')
        x++;
      x++;
    }
    
    else
    {

       if(DEBUG == 1)
      Serial.println("Sorry, Wrong Format");
      client.stop();
      return;
    }
    req = req.substring(x);

    int i = 0;
    while (req.charAt(i) != '=')
      i++;
    i++;
    int j = 0;
    while (req.charAt(i + j) != '&')
      j++;
    String ssid_name = req.substring(i, i + j);
    ssid_name.replace(space, " ");
     if(DEBUG == 1)
    Serial.println(ssid_name);
    j++;
    int k = 0;
    while (req.charAt(i + j + k) != '=')
      k++;
    k++;
    int l = 0;
    while (req.charAt(i + j + k + l) != '/')
      l++;
    String password = req.substring(i + j + k, i + j + k + l);
    password.replace(space, " ");
     if(DEBUG == 1)
    Serial.println(password);

    char* ssid_name_ = &ssid_name[0];
    char* password_ = &password[0];
     if(DEBUG == 1)
    Serial.println("Client disonnected");
    WiFi.begin(ssid_name_, password_);
    delay(5000);
    
    String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nReset your ESP board. ";
    s += "</html>\n";

    // Send the response to the client
    client.print(s);
    client.flush();
    client.stop();
    if (WiFi.status() == WL_CONNECTED)
    {
       if(DEBUG == 1)
      Serial.println(WiFi.localIP());

      connection_flag = 0;
    }
    else
      return;
  }

  if (connection_flag == 0 )
  {

    HTTPClient http;
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    http.begin("https://maker.ifttt.com/trigger/ +++ event +++ /with/key/ +++ Your Key +++"); // Edit the "event" nad "Your Key" in this link according to your account

    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled

      // file found at server
      if (httpCode == HTTP_CODE_OK) {

        String payload = http.getString();

        for (int i = 0; i < 5; i++)
        {
          digitalWrite(LED_BUILTIN, LOW);
          delay(50);
          digitalWrite(LED_BUILTIN, HIGH);
          delay(50);
        }
      }
    } else {
      return;
    }

    http.end();
    ESP.deepSleep(0);
  }
}
