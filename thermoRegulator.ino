/*
  Analog pin 0 used for reading temperature
 */
int thermistorPin = A0;

/*
 Capacity of time series
 */
const int CAPACITY = 5;

/*
Length of a period in milliseconds
 */
const int PERIOD = 1000;

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
}


/*
Main processing loop
 */
void loop(){

  put(readTemperature());
  readTimeSeries();
  //printTimeSeries();
  
  calculateM();

  delay(PERIOD);
}


/*
 Read the temperature.
 */
float readTemperature(){
  int thermistorReading = analogRead(thermistorPin);
  return thermistorReading * 0.05;
}

/*
  Prints the current time series without modifying its contents.
 */
void printTimeSeries()
{
  for (int i = 0; i < CAPACITY; i++){
    Serial.print("(");
    Serial.print(readableTs[i][0]);
    Serial.print(", ");
    Serial.print(readableTs[i][1]);
    Serial.print(") ");
  }
  Serial.println("");
}


/*
 Initialises time series arrays.
 */
void initTimeSeries(){
  for(int i = 0; i < CAPACITY; i++){
    timeSeries[i] = 0.0;
    readableTs[i][0] = 0.0;
    readableTs[i][1] = 0.0;
  }
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
    readableTs[i][0] = 1+i-CAPACITY;
    readableTs[i][1] = timeSeries[index];  
  }
}

/*
Calculate slope of tendency using least square method
 */
float calculateM(){
  float xAverage = average(0);
  float yAverage = average(1);
  Serial.println(xAverage);
  Serial.println(yAverage);
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
