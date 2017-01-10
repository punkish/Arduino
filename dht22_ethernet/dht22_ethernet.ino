#include <SPI.h>
#include <Ethernet.h>

#include <dht.h>

dht DHT;

#define DHT22_PIN 2

// the media access control (ethernet hardware) address for the shield:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  

// IP address for the shield
byte ip[] = { 10, 0, 0, 177 };

//the IP address for the server
byte server[] = { 192, 168, 1, 114 };

EthernetClient client;

//#define HOST "192.168.100.157"

#define PATH "/readings"
#define HEADERS "Content-Type: application/json\r\n"

#define PARAM_BUFFER_LENGTH 100
char paramBuffer[ PARAM_BUFFER_LENGTH ];
int paramBufferPos = 0;
int paramCount = 0;

void setup() {
  
  // configure USB/serial comunication with computer
  Serial.begin(9600);
  
  Serial.println("Configuring Ethernet");

  // give the ethernet module time to boot up:
  delay(1000);
  
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Serial.println("no point in carrying on, so do nothing forevermore");
    
    for(;;)
      ;
  }
  
  // print your local IP address:
  Serial.println(Ethernet.localIP());
}

unsigned long lastTime = 0;
unsigned long lastDisplayTime = 0;

// last time you connected to the server, in milliseconds
unsigned long lastConnectionTime = 0;

// state of the connection last time through the main loop
boolean lastConnected = false;

float temp = 0.0;
float hum = 0.0;

void loop () {
  
  // if enough time has elapsed since last loop, run the processSensors()
  // and sendSensors functions
  //uint32_t start = micros();
  int chk = DHT.read22(DHT22_PIN);
  //uint32_t stop = micros();
    
  unsigned long time = millis();
  if (time - lastTime > 5000) {
    lastTime = time;
    processSensors();
  }
  
  //if (!client.connected() && (time - lastTime > 30000) ) {
//  if (time - lastTime > 5000) {
//    lastTime = time;
//    sendSensors(temp, hum);
//  }

  // if there's no connection, but there was one last time through the loop,
  // then stop the client
  if (!client.connected() && lastConnected) {
    Serial.println();
    Serial.println("disconnecting now.");
    client.stop();
  }

  // store the state of the connection for next time through the loop
  lastConnected = client.connected();
}

void processSensors() {
  temp = DHT.temperature;
  hum = DHT.humidity;
  sendSensors(temp, hum);
  // DISPLAY DATA
//  Serial.print(DHT.humidity, 1);
//  Serial.print(",\t");
//  Serial.print(DHT.temperature, 1);
//  Serial.print(",\t");
//  Serial.println();
}

void sendSensors( float temp, float hum ) {
//  add( "temp", temp, 2 );
//  add( "hum", hum, 2 );
  
  sendDataSerial();
}

bool sendDataSerial() {
  bool error = true;
  
  // if there's a successful connection:
  if (client.connect(server, 3000)) {
    Serial.println("connecting...");

//    Serial.println("PUT /readings HTTP/1.1");
//    Serial.println("Host: 192.168.1.114:3000");
//    Serial.print("Content-Length: ");
//    Serial.println(strlen(paramBuffer));
//
//    // last pieces of the HTTP request:
//    Serial.println("Content-Type: application/json");
//    Serial.println("Connection: close");
//    Serial.println();
//
//    // here's the actual content of the request:
//    Serial.println(paramBuffer);

    String a = "PUT /readings HTTP/1.1";
    String b = "Connection: keep-alive";
    String c = "Content-Type: application/json";
    String d = "Content-Length: 30";
    String e = "{\"temp\":\"";
    String f = "\",\"hum\":\"";
    String g = "\"}";
    String h = "Connection: close";

    Serial.println(a);
    Serial.println(b);
    Serial.println(c);
    Serial.println(d);
    Serial.println(h);
    Serial.print(e);
    Serial.print(temp);
    Serial.print(f);
    Serial.print(hum);
    Serial.println(g);
    Serial.println();

    client.println(a);
    client.println(b);
    client.println(c);
    client.println(d);
    client.println(h);
    client.print(e);
    client.print(temp);
    client.print(f);
    client.println(g);
    client.println();
    error = false;
  }
  else {
    
    // if you couldn't make a connection:
    Serial.println("connection failed so disconnecting");
    client.stop();
  }

  // Clear for next time
  paramBuffer[ 0 ] = 0;
  paramBufferPos = 0;
  paramCount = 0;

  // note the time that the connection was made or attempted:
  lastConnectionTime = millis();
  return error;
}

// this method makes a HTTP connection to the server:
//bool sendData() {
//  bool error = true;
//  
//  // if there's a successful connection:
//  if (client.connect(HOST, 80)) {
//    Serial.println("connecting...");
//
//    client.print("PUT ");
//    client.print(PATH);
//    client.println(" HTTP/1.1");
//
//    client.print("Host: ");
//    client.println(HOST);
//    client.print("Content-Length: ");
//    client.println(strlen(paramBuffer));
//
//    // last pieces of the HTTP request:
//    client.println("Content-Type: application/json");
//    client.println("Connection: close");
//    client.println();
//
//    // here's the actual content of the request:
//    client.println(paramBuffer);
//    error = false;
//  } 
//  else {
//    
//    // if you couldn't make a connection:
//    Serial.println("connection failed");
//    Serial.println();
//    Serial.println("disconnecting.");
//    client.stop();
//  }
//  
//  // Clear for next time
//  paramBuffer[ 0 ] = 0;
//  paramBufferPos = 0;
//  paramCount = 0;
//
//  // note the time that the connection was made or attempted:
//  lastConnectionTime = millis();
//  return error;
//}

// These functions add data to the buffer for the Ethernet Shield to send:
void append( const char *str ) {
  while (str[ 0 ]) {
    paramBuffer[ paramBufferPos++ ] = str[ 0 ];
    str++;
    if (paramBufferPos + 1 >= PARAM_BUFFER_LENGTH) // leave room for zero terminator
      break;
  }
  paramBuffer[ paramBufferPos ] = 0; // add zero terminator
}

// add a value to transmit with the next call to send()
void add( const char *name, const char *value ) {
  if (paramCount)
    append( "&" );
  append( name );
  append( "=" );
  append( value );
  paramCount++;
}

// add a value to transmit with the next call to send()
void add( const char *name, long value ) {
  if (paramBufferPos + 12 < PARAM_BUFFER_LENGTH) {
    if (paramCount)
      append( "&" );
    append( name );
    append( "=" );
    ltoa( value, paramBuffer + paramBufferPos, 10 );
    while (paramBuffer[ paramBufferPos ] && paramBufferPos < PARAM_BUFFER_LENGTH) // find new end of string
      paramBufferPos++;
    paramCount++;
  }
}

// add a value to transmit with the next call to send()
void add( const char *name, double value, byte decimalPlaces ) {
  if (paramBufferPos + 15 < PARAM_BUFFER_LENGTH) { // we'll assume that the value doesn't have more than 14 digits
    if (paramCount)
      append( "&" );
    append( name );
    append( "=" );
    dtostrf( value, decimalPlaces, decimalPlaces, paramBuffer + paramBufferPos );
    while (paramBuffer[ paramBufferPos ] && paramBufferPos < PARAM_BUFFER_LENGTH) // find new end of string
      paramBufferPos++;
    paramCount++;
  }
}

// add a value to transmit with the next call to send()
void add( const char *name, float value, byte decimalPlaces ) {
  add( name, (double) value, decimalPlaces );
}

// add a value to transmit with the next call to send()
void add( const char *name, int value ) {
  add( name, (long) value );
}


