#include <SPI.h>
#include <Ethernet.h>
#include <avr/pgmspace.h>

//Used to store large strings in progmem.
char p_buffer[80];
#define P(str) (strcpy_P(p_buffer, PSTR(str)), p_buffer)

// MAC address and IP address.
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,0, 51);

// Initialize the Ethernet server library on port 80
EthernetServer server(80);

//Request read buffer
#define bufferMax 64
int bufferSize;
char buffer[bufferMax];

/*
  Analog pin 0 used for reading temperature
 */
int thermistorPin = A0;

/*
 Capacity of time series
 */
const int CAPACITY = 12;

/*
Length of a period in milliseconds
 */
const int PERIOD = 5000;

/*
The set temperature.
 */
const float SET_POINT = 21.0;

/*
 The time series backing array
 */
float timeSeries[CAPACITY];

/*
 Two dimensional time series array used for presentation and calculation purposes.
 */
float readableTs[CAPACITY][2];

/*
 Pointer used for time series storage. 
 */
int pointer = 0;



/*
System setup
 */
void setup(){
  initTimeSeries();
  Serial.begin(9600);
  initEthernet();
}


/*
Main processing loop
 */
void loop(){

  static unsigned long before = 0;
  
  if(millis()>before+PERIOD){
    put(readTemperature());
    printForecast();
    before=millis();
  }

  serveWebRequests();
  delay(500);
}

/*
Web server request - response loop
 */
void serveWebRequests()
{
  // Listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println(P("new client"));
    bufferRequest(client);
    writeResponse(client);

    // Give the web browser time to receive the data
    delay(1);

    // Close the connection:
    client.stop();

    Serial.println(P("client disonnected"));
  }
}

/*
Buffer the request into local memory
 */
void bufferRequest(EthernetClient client)
{
  bufferSize = 0;
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      if (c == '\n')
        break;
      else
        if (bufferSize < bufferMax)
          buffer[bufferSize++] = c;
        else
          break;
    }
  }
}

/*
Dispatch to all requested commands.
 */
void writeResponse(EthernetClient client)
{
  writeHeader(client);
  writeWebPage(client);        
}

void writeHeader(EthernetClient client){
  client.println(P("HTTP/1.1 200 OK"));
  client.println(P("Content-Type: text/html"));
  client.println(P("Connection: close"));
  client.println();

}

/*
 Read the temperature.
 */
float readTemperature(){
  int thermistorReading = analogRead(thermistorPin);
  return thermistorReading * 0.053;
}

void printForecast()
{
  readTimeSeries();

  float temp = readableTs[CAPACITY-1][1];
  float m = calculateM();
  float n = calculateN(m);
  float fCast = forecastTime(m,n);

  Serial.print(P("T="));
  Serial.print(temp);
  Serial.print(P("  y="));
  Serial.print(m);
  Serial.print(P("x+"));
  Serial.print(n);
  Serial.print(P(" "));

  if(fCast > 0){
    Serial.print(fCast/1000);
    Serial.print(P("s"));
  }

  Serial.println("");
}

/*
  Prints the current time series without modifying its contents.
 */
void writeWebPage(EthernetClient client)
{
  readTimeSeries();
  client.println(P("<html>"));
  client.println(P("<head>"));
  client.println(P("<body>"));
  client.println(P("Hello World"));
  client.println(P("</body>"));
  client.println(P("</html>"));
}

/*
 Initialises time series arrays.
 */
void initTimeSeries(){

  float temp = readTemperature();
  for(int i = 0; i < CAPACITY; i++){
    timeSeries[i] = temp;
    readableTs[i][0] = 0.0;
    readableTs[i][1] = temp;
  }
}

/*
Start the Ethernet connection and the web server.
 */
void initEthernet()
{
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print(P("server is at "));
  Serial.println(Ethernet.localIP());
}

/*
 Put value in round-robin fashion into the time series array 
 */
void put (float value)
{
  pointer++;
  if(pointer % CAPACITY == 0){ 
    pointer = 0;
  } 
  timeSeries[pointer] = value;
}

/*
 Copies the current time series backing array into a presentable array.
 */
void readTimeSeries(){
  for(int i = 0; i < CAPACITY; i++){
    int index = pointer + i +1;
    if (index > CAPACITY-1) {
      index = index -CAPACITY;
    }
    readableTs[i][0] = i-CAPACITY;
    readableTs[i][1] = timeSeries[index];  
  }
}

/*
Calculate slope of tendency using least square method.
 */
float calculateM(){
  float xAverage = average(0);
  float yAverage = average(1);

  float sumUp = 0.0;
  float sumBottom = 0.0;

  for(int i = 0; i < CAPACITY; i++){
    sumUp = sumUp + ((readableTs[i][0] - xAverage)*(readableTs[i][1]-yAverage));
    sumBottom = sumBottom + ((readableTs[i][0] - xAverage)*(readableTs[i][0] - xAverage));
  }
  return sumUp/sumBottom;
}

float calculateN(float m)
{
  return average(1) - m* average(0);
}

/*
Calculates the average of x or y row of readable time series.
 */
float average(int rowId)
{
  float sum = 0;

  for (int i = 0; i < CAPACITY; i++){
    sum += readableTs[i][rowId];
  }
  return sum / CAPACITY;
}

float forecastTime(float m, float n)
{
  return (SET_POINT - n) / m * PERIOD; 
}


