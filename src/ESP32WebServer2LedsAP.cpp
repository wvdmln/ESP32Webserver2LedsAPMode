// Laad Wi-Fi library
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

// Vervang door je netwerkgegevens
const char* ssid = "ESP32AP";  // vervang!!
const char* password = "12345678";  // vervang!!

// Set web server port number op 80
WiFiServer server(80);

// Variabele  HTTP request
String header;

// Variabelen output states
String output15GroenState = "off";
String output4RoodState = "off";

// Wijs variabelen toe aan GPIO pins
const int output15Groen = 15;
const int output4Rood = 4;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Definieer timeout time in milliseconden (voorbeeld: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  
  
  Serial.begin(115200);
  // Print de naam van de geladen sketch 
  Serial.print("Geladen Sketch: ");
  Serial.println ("ESP32WebServer2LedsAP");
  Serial.println();
  Serial.println("Config access point...");
  // Initialiseer de output variabelen als outputs
  pinMode(output15Groen, OUTPUT);
  pinMode(output4Rood, OUTPUT);
  // Set outputs LOW
  digitalWrite(output15Groen, LOW);
  digitalWrite(output4Rood, LOW);

  // Verwijder password voor een open AP
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  Serial.println("Server gestart");
  Serial.println("Je kan connecteren met: ");
  Serial.print("ssid: ");
  Serial.println (ssid);
  Serial.print("password: ");
  Serial.println(password);



  
}

void loop(){
  WiFiClient client = server.available();   // Luister naar incoming clients

  if (client) {                             // Als nieuwe client connecteert,
    
    Serial.println("New Client found.");          // print boodschap in serial monitor
    String currentLine = "";                // String om incoming data van de client te bewaren
    while (client.connected() ) {  // loop while zo lang de client geconnecteerd is
      
      if (client.available()) {             // als de client beschikbaar is
        char c = client.read();             // read byte
        Serial.write(c);                    // print in de serial monitor
        header += c;
        if (c == '\n') {                    // als byte een newline character is
          // als de current line blank is, krijg je 2 newline characters na mekaar
          // dat is het einde van de client HTTP request, zend response:
          if (currentLine.length() == 0) {
            // HTTP headers starten altijd met een response code (HTTP/1.1 200 OK)
            // en een content-type zodat de client weet wat komt, daarna blanke lijn:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // zet  GPIOs on / off
            if (header.indexOf("GET /15/on") >= 0) {
              Serial.println("GPIO 15 On");
              output15GroenState = "on";
              digitalWrite(output15Groen, HIGH);
            } else if (header.indexOf("GET /15/off") >= 0) {
              Serial.println("GPIO 15 Off");
              output15GroenState = "off";
              digitalWrite(output15Groen, LOW);
            } else if (header.indexOf("GET /4/on") >= 0) {
              Serial.println("GPIO 4 on");
              output4RoodState = "on";
              digitalWrite(output4Rood, HIGH);
            } else if (header.indexOf("GET /4/off") >= 0) {
              Serial.println("GPIO 4 off");
              output4RoodState = "off";
              digitalWrite(output4Rood, LOW);
            }
            
            // Display de HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS om de style van de on/off knoppen te bepalen 
            // Wijzig eventueel de code om het uitzicht te wijzigen
            // Via deze website kun je makkelijk CSS code voor buttons genereren
            // https://www.bestcssbuttongenerator.com/
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".buttongroen { background-color: #44CC44; border-radius:20px; border:4px solid #22AA22; color: #0000FF; width: 150px;padding: 16px;");
            client.println("text-decoration: none; font-size: 30px; cursor: pointer;}");
            client.println(".button2groen {background-color: #558855;}");
            client.println(".buttonrood {	background-color:#CC4444;	border-radius:20px;	cursor:pointer;	font-size: 30px;color: #FFFF00;width: 150px;padding: 20px;	 }");
            client.println(".button2rood {background-color: #885555;}</style></head>");
            
            // Web Page hoofding
            client.println("<body><h1>Thomas More</h1>");
            client.println("<body><h2>ESP32 Web Server</h2>");
            
            // Display actuele status, voor GPIO 15  
            client.println("<p>GPIO 15 - LED is " + output15GroenState + "</p>");
            // Als output15GroenState is off, display donkergroene knop (OFF) ELSE helgroene knop (ON)      
            if (output15GroenState=="off") {
              client.println("<p><a href=\"/15/on\"><button class=\"buttongroen button2groen\">OFF</button></a></p>");
            } else {
              client.println("<p><a href=\"/15/off\"><button class=\"buttongroen \">ON</button></a></p>");
            } 
               
            // Display actuele status en ON/OFF buttons voor GPIO 4  
            client.println("<p>GPIO 4 - LED is " + output4RoodState + "</p>");
                
            if (output4RoodState=="off") {
              client.println("<p><a href=\"/4/on\"><button class=\"buttonrood button2rood\"> OFF </button></a></p>");
            } else {
              client.println("<p><a href=\"/4/off\"><button class=\"buttonrood\"> ON </button></a></p>");
            }
            client.println("</body></html>");
            
            // The HTTP response eindigt met een blanke lijn
            client.println();
            // Stop de while loop
            break;
          } else { // bij newline, clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // bij iets anders dan carriage return character,
          currentLine += c;      // voeg toe aan currentLine
        }
      }
    }
    // Clear de header variabele
    header = "";
    // Sluit de connectie
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
