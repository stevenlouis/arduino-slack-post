/* This sketch listens for a signal from the input pin, then when a signal 
 * is detected, posts a message to Slack via the Slack API. The loop method checks 
 * for wifi connectivity, and if disconnected, will attempt to reconnect. Two status
 * lights are added to indicate wifi connectivity, Slack post execution, and running status.
 */

#include <ESP8266WiFi.h>
#include <ArduinoCloud.h>

// Wifi Details for secured network
const char* ssid = "NetworkName";
const char* password = "NetworkPassword";

/*
//Wifi details for open network
const char* ssid     = "NetworkName";
*/

// slack webhook host and endpoint information
const char* host = "hooks.slack.com";
const char* SlackWebhookURL = "/services/.../..."; //Your Slack Webhook

// SSL Certificate finngerprint for the host
const char* fingerprint = "‎‎ab f0 5b a9 1a e0 ae 5f ce 32 2e 7c 66 67 49 ec dd 6d 6a 38";

// Slack Channel Info, Bot User Name, and Message
const String channel = "#channelname";
const String username = "The Doorbell Bot";
const String message = "Ding Dong! Someone needs assistance.";

//SSL Client Variable
WiFiClientSecure client;

// Pin Setup
const int buttonPin = 13;     
const int errorLightPin = 12; 
const int goodLightPin = 14;
 
//int buttonState = 0;
int wifiCheckCounter = 0;

 
void setup() {
  //Serial.begin(115200); //Begin serial
  //delay(1000); //delay for serial start

  // initialize the input pin, LED pins.
  pinMode(buttonPin, INPUT);
  pinMode(errorLightPin, OUTPUT);
  pinMode(goodLightPin, OUTPUT);
  digitalWrite(goodLightPin, LOW);
 
  // We start by connecting to a WiFi network
  //Serial.println("Connecting to ");
  //Serial.println(ssid);


  //Connect to secured wifi network
  WiFi.begin(ssid,password);
  
  /*
  //Connect to unsecured wifi network
  //WiFi.begin(ssid);
  */
 
  delay(5000);
 
  //Print wifi info
  //Serial.println("WiFi connected"); 
  //Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());

}
 
void loop() {

  if(wifiCheckCounter < 1) //
  {
    digitalWrite(goodLightPin, HIGH);
    delay(250);
    digitalWrite(goodLightPin, LOW);
    if(WiFi.status() != WL_CONNECTED)
    {

      errorLightOn();

      //Serial.println("Reonnecting to ");
      //Serial.println(ssid);
      WiFi.begin(ssid);//, password);

      while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      //Serial.print(".");


      }

      errorLightOff();    
    }
    wifiCheckCounter = 900000;
  }

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if(digitalRead(buttonPin) == HIGH){
    slackPost();
    //cloudPost("ON");
    //alreadyPostedOff = false;
  }
  wifiCheckCounter--;
  //Serial.println(String(wifiCheckCounter));

}


void errorLightOn()
{
  digitalWrite(errorLightPin, HIGH);
  //digitalWrite(goodLightPin, LOW);
}

void errorLightOff()
{
  digitalWrite(errorLightPin, LOW);
  //digitalWrite(goodLightPin, HIGH);
}


void slackPost() {

  digitalWrite(goodLightPin, HIGH);
  digitalWrite(errorLightPin, HIGH);

  //Serial.print("connecting to ");
  //Serial.println(host);

  // create a secure connection using WiFiClientSecure
  const int httpPort = 443;
  if (client.connect(host, httpPort)) {

    // verify the signature of the ssl certificate
    if (client.verify(fingerprint, host)) {
      //Serial.println("ssl cert matches");
    } else {
      //Serial.println("ssl cert mismatch");
    }
 
    String PostData="payload={\"channel\": \"" + channel + "\", \"username\": \"" + username + "\", \"text\": \"" + message + "\", \"icon_emoji\": \":bell:\"}";
    //Serial.println(PostData);
   
    client.print("POST ");
    client.print(SlackWebhookURL);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(host);
    client.println("User-Agent: ArduinoIoT/1.0");
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded;");
    client.print("Content-Length: ");
    client.println(PostData.length());
    client.println();
    client.println(PostData);
   
    delay(500);
    
    // Read all the lines of the reply from server and print them to Serial for debugging
    //while(client.available()){
    //String line = client.readStringUntil('\r');
    //Serial.print(line);
    //}
    
    client.stop();
    //Serial.println();
    //Serial.println("closing connection"); 
  }

  digitalWrite(goodLightPin, LOW);
  digitalWrite(errorLightPin, LOW);

}
