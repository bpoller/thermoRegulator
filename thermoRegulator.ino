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
 Time series array used for presentation and calculation purposes
*/
float readableTs[CAPACITY];

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
  printTimeSeries();

  delay(1000);
}


/*
 Read the temperature.
*/
float readTemperature()
{
  int thermistorReading = analogRead(thermistorPin);
  return thermistorReading * 0.05;
}

/*
  Prints the current time series without modifying its contents.
 */
void printTimeSeries()
{
  updateTS();
  for (int i = 0; i < CAPACITY; i++)    {
    Serial.print(readableTs[i]+" ");
  }
  Serial.println("");
}


/*
 Initialises time series arrays.
 */
void initTimeSeries()
{
  for(int i = 0; i < CAPACITY; i++)
  {
    timeSeries[i] = 0.0;
    readableTs[i] = 0.0;
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
void updateTS(){

  for(int i = 0; i < CAPACITY; i++)
  {
    int index = pointer + i +1;
    if (index > CAPACITY-1) {
      index = index -CAPACITY;
    }
    readableTs[i] = timeSeries[index];  
  } 
}
