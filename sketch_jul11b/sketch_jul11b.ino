//my web site, replace with yours
#define DST_IP "192.168.88.35"

//Define the pin for the on board LED
int led = 13;

//Initialize Device
void setup()
{
//Enable the Onboard LED to act as a Status indicator light
pinMode(led,OUTPUT);
//Set the speed for communicating with the ESP8266 module
Serial.begin(9600);
//Send a series of flashes to show us we are in the bootup phase.
blinkcode ();
//Reset the ESP8266
Serial.println("AT+RST");
//Wait for the WiFi module to bootup
delay(800);
//Switch the chip to "client" mode
Serial.println("AT+CWMODE=1");
//Connect to our Wireless network NOTE: change SSID to your wireless
//network and PASSWORD to the key used to join the network.
//Serial.println("AT+CWJAP=\"SSID\",\"PASSWORD\"");
//Once again blink some lights because it is fun!
//blinkcode ();
}

void loop() {
  
}
void noiloop()
{
//Open a connection to the web server
String cmd = "AT+CIPSTART=\"TCP\",\""; //make this command: AT+CPISTART="TCP","192.168.88.35",80
cmd += DST_IP;
cmd += "\",80";
Serial.println(cmd);

//wait a little while for 'Linked'
delay(300);

//This is our HTTP GET Request change to the page and server you want to load.
cmd = "GET /status.html HTTP/1.0\r\n";
cmd += "Host: 192.168.88.35\r\n\r\n";

//The ESP8266 needs to know the size of the GET request
Serial.print("AT+CIPSEND=");
Serial.println(cmd.length());

//Look for the > prompt from the esp8266
if(Serial.find(">"))
{
//Send our http GET request
Serial.println(cmd);
}
else
{
//Something didn't work...
Serial.println("AT+CIPCLOSE");
}

//Check the returned header & web page. Looking for a keyword. I used YES12321

if (Serial.find("YES12321"))
{
//If the string was found we know the page is up and we turn on the LED status
//light to show the server is ONLINE
digitalWrite(led,HIGH);
}
else
{
//If the string was not found then we can assume the server is offline therefore
//we should turn of the light.
digitalWrite(led,LOW);
}
//Wait a second for everything to settle.
delay(1000);
//Be great people and close our connection.
Serial.println("AT+CIPCLOSE");

//Introduce a delay timer before we finish and go back to the begining.
delay (5000);
}

void blinkcode ()
{
//Simply loop a few times flashing the status light (this is used during boot up)
int i;
for (i = 1; i <= 10; i++){
delay (100);
digitalWrite(led,HIGH);
delay (100);
digitalWrite(led,LOW);
}
}
