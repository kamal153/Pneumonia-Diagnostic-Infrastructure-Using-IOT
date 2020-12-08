int temppin = 12; // analog pin
int humpin = 13; // humidity
int PulseSensorPurplePin = 14; // Pulse Sensor WIRE connected to Pin 14
int ecgPin=27;

float ecgReading;

int tempc = 0,tempf=0; // temperature variables
int maxi = -100,mini = 100; // to start max/min temperature

float humi = 0;
float prehum = 0;
float humconst = 0;
float truehum = 0;
float pretruehum = 0;
long pretruehumconst = 0;
long valb = 0;
long truehumi_long=0;

int Signal; // holds the incoming raw data. Signal value can range from 0-1024
int Threshold = 2400; // Determine which Signal to "count as a beat", and which to ingore.

void setup()
{
  Serial.begin(9600); // start serial communication
}

void loop()
{

  tempc = tempc/8.0;
  tempf = (tempc * 9)/ 5 + 32;
  
  valb = analogRead(humpin); // humidity calculation
  prehum = (valb/5);
  humconst = (0.16/0.0062);
  humi = prehum - humconst;
  pretruehumconst = 0.00216*tempc;
  pretruehum = 1.0546-pretruehumconst;
  truehum = humi/pretruehum ;
  
  Signal = analogRead(PulseSensorPurplePin);  // Read the PulseSensor's value.
                                              // Assign this value to the "Signal" variable.
  Signal=Signal%150; 

  ecgReading = analogRead(ecgPin);
  ecgReading = round(ecgReading);
  ecgReading = ecgReading/2-30;
  
  Serial.print("Temprature : ");
  Serial.print(tempf,DEC);
  Serial.print(" Celsius, ");
  
  Serial.print("Humidity: ");
  truehumi_long=(long)truehum;
  Serial.print(truehumi_long);
  
  Serial.print ("%, Heart rate: ");
  Serial.print(Signal);                 // Send the Signal value to Serial Plotter.
  
  Serial.print(", Ecg : ");
  Serial.println(ecgReading);
  
  Serial.println("_______________________________________________________________________");
  delay(1000); // delay before loop
}

float round(float var) 
{ 
    float value = (int)(var * 100 + .5); 
    return (float)value / 100; 
} 
