#include <ESP8266WiFi.h>        //Use ESP8266 functions  
#include <WiFiClientSecure.h>                                            
#include <ESP8266HTTPClient.h>

#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier


#include <Adafruit_NeoPixel.h>

const char* ssid = "qwertyu";                        
const char* password = "88888888";        //WIFI Password
const char* host = "api.thingspeak.com";  //We read the data from this host                                   
const int httpPortRead = 80;                           //   3QGEILIH1SL6A5S8             
                                                  
const char* url1 = "/apps/thinghttp/send_request?api_key=8ZEY1XXWQDLO3Q54";     //AMC
const char* url2 = "/apps/thinghttp/send_request?api_key=0XMX8222AUPR2N6B";     //GME
const char* url3 = "/apps/thinghttp/send_request?api_key=3QGEILIH1SL6A5S8";     //APPLS
const char* url4 = "/apps/thinghttp/send_request?api_key=WB4OSF5TDA00W9MM";     //TSLA
const char* url5 = "/apps/thinghttp/send_request?api_key=D8I5U8GANV6DG0IM";     //BTC
int To_remove;      //There are some irrelevant data on the string and here's how I keep the index
                      //of those characters 
String Amount,Increase,Ratio,Data_Raw,Data_Raw_1,Data_Raw_2,Data_Raw_3;
WiFiClient client;      //Create a WiFi client and http client                                                     
HTTPClient http; 


#define stripPin 12
#define stripPinB 14
#define ledTotal 256 // zero indexed, so one less than you need. I.e. 50 leds = 49 here
#define ledTotalB 0 // zero indexed...
#define rowSize 7 // zero indexed...
#define columnSize 31 // zero indexed...
#define charWidth 5 // ** one indexed **

byte displayOutput[columnSize + 1] = {0};
byte displayColors[columnSize + 1] = {0};
byte currentStatus = 0; // 0 = off, 1 = on
unsigned int maxLoad = 0;
int flag = 0;
int val = 0; // val will be used to store the state
// of the input pin
int old_val = 0; // this variable stores the previous
// value of "val"                     
int state = 0; // 0 = LED off and 1 = LED on
// was 68 with lower case chars, 42 without -- 40 without l/r arrow -- 42 WITH PLUS AND DOT
byte displayLetters[44][charWidth] = {  // [A] = chars/digits contained in array, [B] = charWidth (no need to add 1, as was 'one indexed' at #define)
  {0x3e, 0x51, 0x49, 0x45, 0x3e}, // 0 // , 0x00
  {0x00, 0x42, 0x7f, 0x40, 0x00}, // 1
  {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
  {0x21, 0x41, 0x45, 0x4b, 0x31}, // 3
  {0x18, 0x14, 0x12, 0x7f, 0x10}, // 4
  {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
  {0x3c, 0x4a, 0x49, 0x49, 0x30}, // 6
  {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
  {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
  {0x06, 0x49, 0x49, 0x29, 0x1e}, // 9

  {0x7e, 0x11, 0x11, 0x11, 0x7e}, // A
  {0x7f, 0x49, 0x49, 0x49, 0x36}, // B
  {0x3e, 0x41, 0x41, 0x41, 0x22}, // C
  {0x7f, 0x41, 0x41, 0x22, 0x1c}, // D
  {0x7f, 0x49, 0x49, 0x49, 0x41}, // E
  {0x7f, 0x09, 0x09, 0x09, 0x01}, // F
  {0x3e, 0x41, 0x49, 0x49, 0x7a}, // G
  {0x7f, 0x08, 0x08, 0x08, 0x7f}, // H
  {0x00, 0x41, 0x7f, 0x41, 0x00}, // I
  {0x20, 0x40, 0x41, 0x3f, 0x01}, // J
  {0x7f, 0x08, 0x14, 0x22, 0x41}, // K
  {0x7f, 0x40, 0x40, 0x40, 0x40}, // L
  {0x7f, 0x02, 0x0c, 0x02, 0x7f}, // M
  {0x7f, 0x04, 0x08, 0x10, 0x7f}, // N
  {0x3e, 0x41, 0x41, 0x41, 0x3e}, // O
  {0x7f, 0x09, 0x09, 0x09, 0x06}, // P
  {0x3e, 0x41, 0x51, 0x21, 0x5e}, // Q
  {0x7f, 0x09, 0x19, 0x29, 0x46}, // R
  {0x46, 0x49, 0x49, 0x49, 0x31}, // S
  {0x01, 0x01, 0x7f, 0x01, 0x01}, // T
  {0x3f, 0x40, 0x40, 0x40, 0x3f}, // U
  {0x1f, 0x20, 0x40, 0x20, 0x1f}, // V
  {0x3f, 0x40, 0x38, 0x40, 0x3f}, // W
  {0x63, 0x14, 0x08, 0x14, 0x63}, // X
  {0x07, 0x08, 0x70, 0x08, 0x07}, // Y
  {0x61, 0x51, 0x49, 0x45, 0x43}, // Z

  {0x00, 0x00, 0x00, 0x00, 0x00}, // space
  {0x10, 0x18, 0x1c, 0x18, 0x10}, // new up arrow
  {0x04, 0x0c, 0x1c, 0x0c, 0x04}, // new down arrow
  {0x08, 0x08, 0x08, 0x08, 0x08}, // dash / maintain
  {0x08, 0x08, 0x3E, 0x08, 0x08}, // plus
  {0x00, 0x60, 0x60, 0x00, 0x00}, // full stop
  {0x26, 0x16, 0x08, 0x34, 0x32}, // %
  {0x24, 0x2a, 0xff, 0x2a, 0x12}// $
};

const int stockQty = 17; // this is the amount of different stock values to be contained, *!* ONE indexed *!*
const int stockStepQty = 5; // *!* ONE indexed *!*
int stockStep = 0;


Adafruit_NeoPixel strip = Adafruit_NeoPixel(ledTotal + 1, stripPin, NEO_RGB + NEO_KHZ800);

void setup() {

   Serial.begin(115200);
   WiFi.mode(WIFI_STA);
   WiFi.disconnect();             //Disconnect and reconnect to the Wifi you set                                                 
   delay(1000);                                                                  
   WiFi.begin(ssid, password);                                                   
   Serial.println("Connected to the WiFi network");   //Display feedback on the serial monitor                                        
   Serial.println(WiFi.localIP());

   if(WiFi.status() != WL_CONNECTED)     //In case the Wifi connexion is lost                                    
      { 
        
        WiFi.disconnect();                                                        
        delay(1000);                                                             
        
        WiFi.begin(ssid, password);                                              
        Serial.println("Reconnecting to WiFi..");       
        delay(10000);                                                             
      } 
   strip.begin();
  strip.show();
  
}


void stock_AMC() {
  
  if(http.begin(host,httpPortRead,url1)){

        int httpCode = http.GET();                //Check feedback if there's a response                                                  
        if (httpCode > 0)                                                              
        {
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) 
            {                
              
              Data_Raw = http.getString();   //Here we store the raw data string
            
              
              Data_Raw_1 = Data_Raw;                                    
              To_remove = Data_Raw_1.indexOf(">");                      //I look for the position of this symbol ">"                                 
              Data_Raw_1.remove(0,To_remove+1);                        //I remove it and everything that's before

              To_remove = Data_Raw_1.indexOf(">");                      //I look for the position of this symbol ">"                                 
              Data_Raw_1.remove(0,To_remove+1); 

              To_remove = Data_Raw_1.indexOf("<");                     //I look for the position of this symbol ">"                                
              Data_Raw_1.remove(To_remove,Data_Raw_1.length());

                                                            
              Amount=Data_Raw_1;
              Serial.print("C: ");  //I choosed to display it on the serial monitor to help you debug
              Serial.println(Amount); 


              Data_Raw_2=Data_Raw;
              To_remove = Data_Raw_2.indexOf(">");
              Data_Raw_2.remove(0,To_remove+1);

             
              To_remove = Data_Raw_2.indexOf("</td>");
              Data_Raw_2.remove(0,To_remove+5);

              To_remove = Data_Raw_2.indexOf(">");
              Data_Raw_2.remove(0,To_remove+1);


              To_remove = Data_Raw_2.indexOf("</td>");                                           
              Data_Raw_2.remove(To_remove,Data_Raw_2.length());

              Increase=Data_Raw_2;
              Serial.print("D: ");
              Serial.println(Increase);



              
              Data_Raw_3=Data_Raw;
              To_remove = Data_Raw_3.indexOf("</td>");
              Data_Raw_3.remove(0,To_remove+5);

             
              To_remove = Data_Raw_3.indexOf("</td>");
              Data_Raw_3.remove(0,To_remove+5);

              To_remove = Data_Raw_3.indexOf(">");
              Data_Raw_3.remove(0,To_remove+1);


              To_remove = Data_Raw_3.indexOf("<");                                                   
              Data_Raw_3.remove(To_remove,Data_Raw_3.length());


              Ratio=Data_Raw_3;
              
              Serial.print("R: ");
              Serial.println(Ratio);           
                                                                                      
            }
        }
        else //If we can't get data
        {
          Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        
        http.end();
     }
                                                                     
}



void stock_GME(){
  
  if(http.begin(host,httpPortRead,url2)){

        int httpCode = http.GET();                //Check feedback if there's a response                                                  
        if (httpCode > 0)                                                              
        {
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) 
            {                
              
              Data_Raw = http.getString();   //Here we store the raw data string
            
              
              Data_Raw_1 = Data_Raw;                                    
              To_remove = Data_Raw_1.indexOf(">");                      //I look for the position of this symbol ">"                                 
              Data_Raw_1.remove(0,To_remove+1);                        //I remove it and everything that's before

              To_remove = Data_Raw_1.indexOf(">");                      //I look for the position of this symbol ">"                                 
              Data_Raw_1.remove(0,To_remove+1); 

              To_remove = Data_Raw_1.indexOf("<");                     //I look for the position of this symbol ">"                                
              Data_Raw_1.remove(To_remove,Data_Raw_1.length());

                                                                   
              Amount=Data_Raw_1;
              Serial.print("C: ");  //I choosed to display it on the serial monitor to help you debug
              Serial.println(Amount); 


              Data_Raw_2=Data_Raw;
              To_remove = Data_Raw_2.indexOf(">");
              Data_Raw_2.remove(0,To_remove+1);

             
              To_remove = Data_Raw_2.indexOf("</td>");
              Data_Raw_2.remove(0,To_remove+5);

              To_remove = Data_Raw_2.indexOf(">");
              Data_Raw_2.remove(0,To_remove+1);


              To_remove = Data_Raw_2.indexOf("</td>");                                           
              Data_Raw_2.remove(To_remove,Data_Raw_2.length());

              Increase=Data_Raw_2;
              Serial.print("D: ");
              Serial.println(Increase);
              
              Data_Raw_3=Data_Raw;
              To_remove = Data_Raw_3.indexOf("</td>");
              Data_Raw_3.remove(0,To_remove+5);

             
              To_remove = Data_Raw_3.indexOf("</td>");
              Data_Raw_3.remove(0,To_remove+5);

              To_remove = Data_Raw_3.indexOf(">");
              Data_Raw_3.remove(0,To_remove+1);


              To_remove = Data_Raw_3.indexOf("<");                                                   
              Data_Raw_3.remove(To_remove,Data_Raw_3.length());


              Ratio=Data_Raw_3;
              
              Serial.print("R: ");
              Serial.println(Ratio);           
                                                                                      
            }
        }
        else //If we can't get data
        {
          Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        
        http.end(); 

  }                                                                  
       
}

void stock_TSLA(){
 if(http.begin(host,httpPortRead,url3)){

        int httpCode = http.GET();                //Check feedback if there's a response                                                  
        if (httpCode > 0)                                                              
        {
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) 
            {                
              
              Data_Raw = http.getString();   //Here we store the raw data string
            
              
              Data_Raw_1 = Data_Raw;                                    
              To_remove = Data_Raw_1.indexOf(">");                      //I look for the position of this symbol ">"                                 
              Data_Raw_1.remove(0,To_remove+1);                        //I remove it and everything that's before

              To_remove = Data_Raw_1.indexOf(">");                      //I look for the position of this symbol ">"                                 
              Data_Raw_1.remove(0,To_remove+1); 

              To_remove = Data_Raw_1.indexOf("<");                     //I look for the position of this symbol ">"                                
              Data_Raw_1.remove(To_remove,Data_Raw_1.length());

                                                                     
              Amount=Data_Raw_1;
              Serial.print("C: ");  //I choosed to display it on the serial monitor to help you debug
              Serial.println(Amount); 


              Data_Raw_2=Data_Raw;
              To_remove = Data_Raw_2.indexOf(">");
              Data_Raw_2.remove(0,To_remove+1);

             
              To_remove = Data_Raw_2.indexOf("</td>");
              Data_Raw_2.remove(0,To_remove+5);

              To_remove = Data_Raw_2.indexOf(">");
              Data_Raw_2.remove(0,To_remove+1);


              To_remove = Data_Raw_2.indexOf("</td>");                                           
              Data_Raw_2.remove(To_remove,Data_Raw_2.length());

              Increase=Data_Raw_2;
              Serial.print("D: ");
              Serial.println(Increase);



              
              Data_Raw_3=Data_Raw;
              To_remove = Data_Raw_3.indexOf("</td>");
              Data_Raw_3.remove(0,To_remove+5);

             
              To_remove = Data_Raw_3.indexOf("</td>");
              Data_Raw_3.remove(0,To_remove+5);

              To_remove = Data_Raw_3.indexOf(">");
              Data_Raw_3.remove(0,To_remove+1);


              To_remove = Data_Raw_3.indexOf("<");                                                   
              Data_Raw_3.remove(To_remove,Data_Raw_3.length());


              Ratio=Data_Raw_3;
              
              Serial.print("R: ");
              Serial.println(Ratio);           
                                                                                      
            }
        }
        else //If we can't get data
        {
          Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        
        http.end(); 

  }  
}

void stock_AAPL(){
 if(http.begin(host,httpPortRead,url4)){

        int httpCode = http.GET();                //Check feedback if there's a response                                                  
        if (httpCode > 0)                                                              
        {
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) 
            {                
              
              Data_Raw = http.getString();   //Here we store the raw data string
            
              
              Data_Raw_1 = Data_Raw;                                    
              To_remove = Data_Raw_1.indexOf(">");                      //I look for the position of this symbol ">"                                 
              Data_Raw_1.remove(0,To_remove+1);                        //I remove it and everything that's before

              To_remove = Data_Raw_1.indexOf(">");                      //I look for the position of this symbol ">"                                 
              Data_Raw_1.remove(0,To_remove+1); 

              To_remove = Data_Raw_1.indexOf("<");                     //I look for the position of this symbol ">"                                
              Data_Raw_1.remove(To_remove,Data_Raw_1.length());

                                                                     
              Amount=Data_Raw_1;
              Serial.print("C: ");  //I choosed to display it on the serial monitor to help you debug
              Serial.println(Amount); 


              Data_Raw_2=Data_Raw;
              To_remove = Data_Raw_2.indexOf(">");
              Data_Raw_2.remove(0,To_remove+1);

             
              To_remove = Data_Raw_2.indexOf("</td>");
              Data_Raw_2.remove(0,To_remove+5);

              To_remove = Data_Raw_2.indexOf(">");
              Data_Raw_2.remove(0,To_remove+1);


              To_remove = Data_Raw_2.indexOf("</td>");                                           
              Data_Raw_2.remove(To_remove,Data_Raw_2.length());

              Increase=Data_Raw_2;
              Serial.print("D: ");
              Serial.println(Increase);



              
              Data_Raw_3=Data_Raw;
              To_remove = Data_Raw_3.indexOf("</td>");
              Data_Raw_3.remove(0,To_remove+5);

             
              To_remove = Data_Raw_3.indexOf("</td>");
              Data_Raw_3.remove(0,To_remove+5);

              To_remove = Data_Raw_3.indexOf(">");
              Data_Raw_3.remove(0,To_remove+1);


              To_remove = Data_Raw_3.indexOf("<");                                                   
              Data_Raw_3.remove(To_remove,Data_Raw_3.length());


              Ratio=Data_Raw_3;
              
              Serial.print("R: ");
              Serial.println(Ratio);           
                                                                                      
            }
        }
        else //If we can't get data
        {
          Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        
        http.end(); 

  }  
}

void stock_BTC(){
 if(http.begin(host,httpPortRead,url5)){

        int httpCode = http.GET();                //Check feedback if there's a response                                                  
        if (httpCode > 0)                                                              
        {
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) 
            {                
              
              Data_Raw = http.getString();   //Here we store the raw data string
            
              
              Data_Raw_1 = Data_Raw;                                    
              To_remove = Data_Raw_1.indexOf("</sup>");                      //I look for the position of this symbol ">"                                 
              Data_Raw_1.remove(0,To_remove+6);                        //I remove it and everything that's before

              To_remove = Data_Raw_1.indexOf(">");                      //I look for the position of this symbol ">"                                 
              Data_Raw_1.remove(0,To_remove+1); 

              To_remove = Data_Raw_1.indexOf("<");                     //I look for the position of this symbol ">"                                
              Data_Raw_1.remove(To_remove,Data_Raw_1.length());

                                                                     
              Amount=Data_Raw_1;
              Serial.print("C: ");  //I choosed to display it on the serial monitor to help you debug
              Serial.println(Amount); 


              Data_Raw_2=Data_Raw;
              To_remove = Data_Raw_2.indexOf("</i>");
              Data_Raw_2.remove(0,To_remove+4);

             
              To_remove = Data_Raw_2.indexOf(">");
              Data_Raw_2.remove(0,To_remove+1);

              To_remove = Data_Raw_2.indexOf(">");
              Data_Raw_2.remove(0,To_remove+1);


              To_remove = Data_Raw_2.indexOf("<");                                           
              Data_Raw_2.remove(To_remove,Data_Raw_2.length());

              Increase=Data_Raw_2;
              Serial.print("D: ");
              Serial.println(Increase);



              
              Data_Raw_3=Data_Raw;
              To_remove = Data_Raw_3.indexOf("</i>");
              Data_Raw_3.remove(0,To_remove+4);

             
              To_remove = Data_Raw_3.indexOf(">");
              Data_Raw_3.remove(0,To_remove+1);

              To_remove = Data_Raw_3.indexOf(">");
              Data_Raw_3.remove(0,To_remove+1);
              
              To_remove = Data_Raw_3.indexOf(">");
              Data_Raw_3.remove(0,To_remove+1);

              To_remove = Data_Raw_3.indexOf(">");
              Data_Raw_3.remove(0,To_remove+1);

              To_remove = Data_Raw_3.indexOf(">");
              Data_Raw_3.remove(0,To_remove+1);

              To_remove = Data_Raw_3.indexOf(">");
              Data_Raw_3.remove(0,To_remove+1);


              To_remove = Data_Raw_3.indexOf("</bg-quote>");                                                   
              Data_Raw_3.remove(To_remove,Data_Raw_3.length());


              Ratio=Data_Raw_3;
              
              Serial.print("R: ");
              Serial.println(Ratio);           
                                                                                      
            }
        }
        else //If we can't get data
        {
          Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        
        http.end(); 

  }  
}


String titil;
String inputString;
void loop(){
  
   stock_AMC();
   titil="AMC";
   displayled();

   stock_GME();
   titil="GME";
   displayled();

   stock_TSLA();
   titil="TSLA";
   displayled();

   stock_AAPL();
   titil="AAPL";
   displayled();

   stock_BTC();
   titil="BTC";
   displayled();
}

char b[10];
char c[10];

void displayled(){

  
  strcpy(b,Increase.c_str());
  strcpy(c,Amount.c_str());
  if((b[0] == '-') && (c[0]== '$')){
  inputString = (titil+" "+Amount+"|"+Increase+" "+"|"+Ratio+" ");
  }
  else if ((b[0] == '-') && (c[0]!= '$')){
  inputString = (titil+" "+"$"+Amount+" "+"|"+Increase+" "+"|"+Ratio+" ");
  }
  else if ((b[0] != '-') && (c[0]== '$')){
  inputString = (titil+" "+Amount+"^"+"+"+Increase+" "+"^"+"+"+Ratio+" ");
  }
  else if ((b[0] != '-') && (c[0]!= '$')){
  inputString = (titil+" "+"$"+Amount+" "+"^"+"+"+Increase+" "+"^"+"+"+Ratio+" ");
  }

     for (int i = 0; i < inputString.length(); i++) {
    char currentChar = inputString[i];
    int currentCol, colorMode;// = int(colString[i]) - 48; // minus 48 because int("0") will return the ascii value of char '0' (48)

    if ((currentChar >= 'A') && (currentChar <= 'Z')) {
      currentCol = 0;
    } /*else if ((currentChar == '+') || (currentChar == '-')) {
      currentCol = colorMode;
    } *//*else if((currentChar >= '0') && (currentChar <= '9') ||(currentChar == '%')){
      colorMode = 3;
      currentCol = colorMode;
    }*/else if ((currentChar == '^') || (currentChar >= '0') && (currentChar <= '9') &&(currentChar == '%')){
      colorMode = 2;
      currentCol = colorMode;
    } else if ((currentChar == '|') || (currentChar >= '0') && (currentChar <= '9') && (currentChar == '%')){
      colorMode = 1;
      currentCol = colorMode;
    }
    else if ((currentChar == '$')|| (currentChar >= '0') && (currentChar <= '9') && (currentChar == '%')) {
      colorMode = 4;
      currentCol = colorMode;
    }

    int indexValue;
    if ((currentChar >= '0') && (currentChar <= '9')) indexValue = currentChar - 48; // 0-9
    else if ((currentChar >= 'A') && (currentChar <= 'Z')) indexValue = currentChar - 55; // upper case
    //else if ((currentChar >= 'a') && (currentChar <= 'z')) indexValue = currentChar - 61; // lower case , check that these are in the right place / the offset is correct, as i may have put the array chunk back in the wrong place!!
    else if ((currentChar == ' ')) indexValue = 36; // space, was 62 when lower case chars
    else if ((currentChar == '^')) indexValue = 37; // up arrow, was 63 when lower case chars
    else if ((currentChar == '|')) indexValue = 38; // down arrow, was 64 when lower case chars
     else if ((currentChar == '%')) indexValue = 42;
//    else if ((currentChar == '<')) indexValue = 39; // left arrow, was 65 when lower case chars
//    else if ((currentChar == '>')) indexValue = 40; // right arrow, was 66 when lower case chars
    else if ((currentChar == '-')) indexValue = 39; // dash / maintain, was 67 when lower case chars, 41 with l/r arrows
    else if ((currentChar == '+')) indexValue = 40; // plus
    else if ((currentChar == '.')) indexValue = 41; // full stop
    else if ((currentChar == '$')) indexValue = 43; // full stop

    for (int x = 0; x < charWidth; x++) {
      if (currentStatus == 0) currentStatus = 1;
      else if (currentStatus == 1) currentStatus = 0;
      setStatusIndicator(currentStatus);
      
      nudgeColumns(displayLetters[indexValue][x], currentCol); // inputCols[i]); // strip.Color(0,0,255));
      displayUpdate(0);

      delay(50);
    }
    nudgeColumns(0, 0); // - 1 might break things here? or make things go white due to the lookup default being white
    if (currentStatus == 0) currentStatus = 1;
    else if (currentStatus == 1) currentStatus = 0;
    setStatusIndicator(currentStatus);
    displayUpdate(0);
    delay(50); // this will be 200 as per the rest once confirmed that the double jump issue is done!
  }
}

int getIntLength(int value){
  // Function created by "zed_0xff" at: https://stackoverflow.com/a/3068412/7905386
  int l=!value;
  while(value){ l++; value/=10; }
  return l;
}

int getMappedPixel(int columnID, int rowID){
  int z;

  z = columnSize - columnID;
  if (columnID % 2) z++;

  z = z* (rowSize + 1);
  if (columnID % 2) z = z - rowID - 1;
  else z = z + rowID;

  if (z>= 256) z++;
  return z;
}

void setMappedColor(int columnID, int rowID, long pixelColor) {
  int mappedPixel = getMappedPixel(columnID, rowID);

  if (mappedPixel < ledTotal) {
    strip.setPixelColor(mappedPixel, pixelColor);
  }
  else {
    mappedPixel = mappedPixel - ledTotal;
    strip.setPixelColor(mappedPixel, pixelColor);
  }
}

void colorAll(long color) {
  for (int i = 0; i <= rowSize; i++) {
    for (int j = 0; j <= columnSize; j++) {
      setMappedColor(j, i, color);
    }
  }
}

void convertToBits(byte inputValue, int outputArray[]) {
  if (inputValue & 0x80) outputArray[0] = 1; // not used?
  if (inputValue & 0x40) outputArray[0] = 1;
  if (inputValue & 0x20) outputArray[1] = 1;
  if (inputValue & 0x10) outputArray[2] = 1;
  if (inputValue & 0x08) outputArray[3] = 1;
  if (inputValue & 0x04) outputArray[4] = 1;
  if (inputValue & 0x02) outputArray[5] = 1;
  if (inputValue & 0x01) outputArray[6] = 1;
}

void displayUpdate(long backColor) {
  colorAll(backColor);
  for (int i = 0; i <= columnSize; i++) {
    int columnData[rowSize + 1] = {0};
    convertToBits(displayOutput[i], columnData);

    for (int j = 0; j <= rowSize; j++) {
      if (columnData[rowSize-j] == 1) setMappedColor(i, j, colorLookup(displayColors[i])); // was displayColors[i][j] before memory save
    }
  }

  strip.show();
  //stripB.show();
  
}

void nudgeColumns(byte newData, byte newColor) { 
  for (int i = 0; i < columnSize; i++) {
    displayOutput[i] = displayOutput[i + 1];
    for (int j = 0; j <= rowSize; j++) {
      displayColors[i] = displayColors[i + 1];
    }
  }
  displayOutput[columnSize] = newData;

  if (newColor >= 0) { // created for memory save
    for (int k = 0; k <= rowSize; k++) {
      displayColors[columnSize] = newColor;
    }
  }
}

long colorLookup(int inputColor) {
    long outputColor = 0;
    switch (inputColor) {
      case 0: 
        outputColor = strip.Color(255,255,255); // white
        break;
      case 1:
        outputColor = strip.Color(255,0,0); // red
        break;
      case 2:
        outputColor = strip.Color(0,255,0); // green
        break;
      case 3:
        outputColor = strip.Color(0,0,255); // blue
        break;
      case 4:
        outputColor = strip.Color(255,255,0); // yellow/amber
        break;
      case 5:
        outputColor = strip.Color(0,255,255); // cyan
        break;
      case 6:
        outputColor = strip.Color(255,0,255); // magenta
        break;
      default:
        outputColor = strip.Color(255,255,255); // white
        break;
    }

    return outputColor;
}

void setStatusIndicator(byte inputStatus) {
  if (inputStatus == 0) {
    digitalWrite(13, LOW);
  } else {
    digitalWrite(13, HIGH);
  }
}
