#include <movingAvg.h>

#define espSerial Serial3
#define dbgTerminal Serial

movingAvg avstand(4); //bruker 10 målinger for avstand
float duration, distance;
float tid;
const int relay1 = 5;
int relay1State = HIGH;           // Relay State - (Disconnected) 
int lukketid = 10000;
int status;
int sensorMovingAvg;
int triggerdistance = 60; //avsyand som forteller om porten er åpen eller lukket

#define trigPin 11
#define echoPin 12
#define BUFFER_SIZE 256          // Set the Buffer Size (should be updated According to Arduino Board Type) 
char buffer[BUFFER_SIZE]; 
 
 void setup() {  
   pinMode(relay1, OUTPUT);   
   pinMode(trigPin, OUTPUT);
   pinMode(echoPin, INPUT);
   digitalWrite(relay1,relay1State); 

   avstand.begin();

   dbgTerminal.begin(9600); // espSerial monitor 
   espSerial.begin(115200);
       
   delay(2000); 
    
   clearespSerialBuffer();  //Function to Clear the 8266 Buffer 
    
   //connect to WIFI router 
   connectWiFi("SSID", "password"); 
   delay(10000);
   
   //test if the module is ready  
   dbgTerminal.print("AT : "); 
   dbgTerminal.println( GetRonse("AT",100) ); 
      
   //Change to mode 1 (Client Mode) 
   dbgTerminal.print("AT+CWMODE=1 : "); 
   dbgTerminal.println( GetRonse("AT+CWMODE=1",10) ); 
          
   //set the multiple connection mode 
   dbgTerminal.print(F("AT+CIPMUX=1 : ")); 
   dbgTerminal.println( GetRonse("AT+CIPMUX=1",10) ); 
    
   //set the server of port 80 check "no change" or "OK",  it can be changed according to your configuration 
   dbgTerminal.print(F("AT+CIPSERVER=1,8888 : ")); 
   dbgTerminal.println( GetRonse("AT+CIPSERVER=1,8888", 10) ); 
    
    //print the ip addr 
   dbgTerminal.print(F("ip address : ")); 
   dbgTerminal.println( GetRonse("AT+CIFSR", 20) ); 
   delay(200); 
  
   dbgTerminal.println(); 
   dbgTerminal.println(F("Starting Webserver"));   
 } 
 
 void loop() { 

  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) * 0.0344;
  dbgTerminal.println(distance); 

  sensorMovingAvg = avstand.reading(distance);
  dbgTerminal.print("Average ");
  dbgTerminal.println(sensorMovingAvg); 


   int ch_id, packet_len; 
   char *pb;   
   espSerial.readBytesUntil('\n', buffer, BUFFER_SIZE); 
    
   if(strncmp(buffer, "+IPD,", 5)==0) { 
     // request: +IPD,ch,len:data 
     sscanf(buffer+5, "%d,%d", &ch_id, &packet_len); 
     if (packet_len > 0) { 
       // read espSerial until packet_len character received 
       // start from : 
       pb = buffer+5; 
       while(*pb!=':') pb++; 
       pb++; 

         if (strncmp(pb, "GET /?button1", 13) == 0){
          if (sensorMovingAvg > triggerdistance)
             status = 0; //lukker
          else
             status = 1;

          tid = millis() + lukketid;
          digitalWrite(relay1, LOW);
          delay(1000);
          digitalWrite(relay1, HIGH); 
          delay(100); 
          clearespSerialBuffer(); 
          homepage1(ch_id); 
           }

        else if (strncmp(pb, "GET / ", 6) == 0) { 
         dbgTerminal.print(millis()); 
         dbgTerminal.print(" : "); 
         dbgTerminal.println(buffer); 
         dbgTerminal.print( "get Status from ch:" ); 
         dbgTerminal.println(ch_id);       
         delay(100); 
         clearespSerialBuffer(); 
         homepage1(ch_id); 
       } 
     } 
   } 
   clearBuffer(); 
 } 
 

 // Get the data from the WiFi module and send it to the debug espSerial port 
 String GetRonse(String AT_Command, int wait){ 
   String tmpData; 
    
   espSerial.println(AT_Command); 
   delay(10); 
   while (espSerial.available() >0 )  { 
     char c = espSerial.read(); 
     tmpData += c; 
      
     if ( tmpData.indexOf(AT_Command) > -1 )          
       tmpData = ""; 
     else 
       tmpData.trim();        
            
    } 
    return tmpData; 
 } 
 
 void clearespSerialBuffer(void) { 
        while ( espSerial.available() > 0 ) { 
          espSerial.read(); 
        } 
 } 
 
 void clearBuffer(void) { 
        for (int i =0;i<BUFFER_SIZE;i++ ) { 
          buffer[i]=0; 
        } 
 } 
           
 boolean connectWiFi(String NetworkSSID,String NetworkPASS) { 
   String cmd = "AT+CWJAP=\""; 
   cmd += NetworkSSID; 
   cmd += "\",\""; 
   cmd += NetworkPASS; 
   cmd += "\""; 
    
   dbgTerminal.println(cmd);  
   dbgTerminal.println(GetRonse(cmd,10)); 
 } 

void homepage1(int ch_id) { 
   String Header; 
 
   Header =  "HTTP/1.1 200 OK\r\n"; 
   Header += "Content-Type: text/html\r\n"; 
   Header += "Connection: close\r\n";   
   Header += "Refresh: 5;URL='//192.168.10.126:8888/'>\r\n"; 
    
   String Content; 

           Content ="<HTML>\r\n";
           Content +="<HEAD>\r\n";
           Content +="<meta name='apple-mobile-web-app-capable' content='yes' />\r\n";
           Content +="<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />\r\n";
           Content +="<meta name='viewport' content='width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no' />\r\n"; //kan slettes      
           Content +="<link rel='stylesheet' type='text/css' href='http://randomnerdtutorials.com/ethernetcss.css' />\r\n";
           Content +="<TITLE>Garasjeportkontroll</TITLE>\r\n";
           Content +="</HEAD>\r\n";
           Content +="<BODY>\r\n";
           Content +="<H1>Garasjeport</H1>\r\n";
           Content +="<hr />\r\n";
           Content +="<br />\r\n";

           if (tid > millis() & status == 1)
              Content +="<H2>Lukker</H2>\r\n";
           else if (tid > millis() & status == 0)
              Content +="<H2>&#197pner</H2>\r\n";
           else
              if (sensorMovingAvg < triggerdistance)
                 Content +="<H2>&#197pen</H2>\r\n";
              else
                 Content +="<H2>Lukket</H2>\r\n";
              
           Content +="<br />\r\n";
           
           if (sensorMovingAvg < triggerdistance) 
              Content +="<a href=\"/?button1\"\">Lukk</a><br />\r\n";   
           else 
              Content +="<a href=\"/?button1\"\">&#197pne</a><br />\r\n";
               
           Content +="</BODY>\r\n";
           Content +="</HTML>\r\n";
    
   Header += "Content-Length: "; 
   Header += (int)(Content.length()); 
   Header += "\r\n\r\n"; 
    
   espSerial.print("AT+CIPSEND="); 
   espSerial.print(ch_id); 
   espSerial.print(","); 
   espSerial.println(Header.length()+Content.length()); 
   delay(10); 
    
   if (espSerial.find(">")) { 
       espSerial.print(Header); 
       espSerial.print(Content); 
       delay(10); 
    } 
 }
