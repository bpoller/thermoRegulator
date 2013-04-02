/*
  Analog pin 0 used for reading temperature
 */
int thermistorPin = A0;

/*
 Time Series
 */
const int CAPACITY = 5;
float timeSeries[CAPACITY];
float readableTs[CAPACITY];
int pointer = 0;

void setup(){
  initTimeSeries();
  Serial.begin(9600);
}

void loop(){

  put(readTemperature());
  printTimeSeries();

  delay(1000); //just here to slow down the output for easier reading
}

float readTemperature()
{
  int thermistorReading = analogRead(thermistorPin);
  return thermistorReading * 0.05;
}


/*
  Prints the contents of the buffer without modifying its contents.
 */
void printTimeSeries()
{

  updateTS();
  for (int i = 0; i < CAPACITY; i++)    {
    Serial.print(readableTs[i]+" ");
  }
  Serial.println("");
}

void put (float value)
{
  pointer++;
  if(pointer % CAPACITY == 0){ 
    pointer = 0;
  } 
  timeSeries[pointer] = value;
}

void initTimeSeries()
{
  for(int i = 0; i < CAPACITY; i++)
  {
    timeSeries[i] = 0.0;
    readableTs[i] = 0.0;
  }
}

void updateTS(){
  
  for(int i = 0; i < CAPACITY; i++)
  {
    int index = pointer + i +1;
    if (index > CAPACITY-1) {index = index -CAPACITY;}
    readableTs[i] = timeSeries[index];  
}
  
}
