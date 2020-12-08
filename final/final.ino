#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include "ThingSpeak.h"
#include "secrets.h"
#include <WiFi.h>

MAX30105 particleSensor;
#define MAX_BRIGHTNESS 255
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
  uint16_t irBuffer[100]; //infrared LED sensor data
  uint16_t redBuffer[100];  //red LED sensor data
#else
  uint32_t irBuffer[100]; //infrared LED sensor data
  uint32_t redBuffer[100];  //red LED sensor data
#endif

//MAX30105
int32_t bufferLength; //data length
int32_t spo2; //SPO2 value
int8_t validSPO2; //indicator to show if the SPO2 calculation is valid
int32_t heartRate; //heart rate value
int8_t validHeartRate; //indicator to show if the heart rate calculation is valid
        
//Pin Description
int temppin = 26; // analog pin
int humpin = 13; // humidity
int PulseSensorPurplePin = 14; // Pulse Sensor WIRE connected to Pin 14
int ecgPin=27;
//SDA=D21  SCL=D22

//ECG
float ecgReading;

//LM32
int tempc = 0,tempf=0; // temperature variables
int maxi = -100,mini = 100; // to start max/min temperature

//HR202
float humi = 0;
float prehum = 0;
float humconst = 0;
float truehum = 0;
float pretruehum = 0;
long pretruehumconst = 0;
long valb = 0;
long truehumi_long=0;

//Heart Rate
int Signal; // holds the incoming raw data. Signal value can range from 0-1024
int Threshold = 2400; // Determine which Signal to "count as a beat", and which to ingore.

//Thingspeack
char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;
String myStatus = "";

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

void setup()
{
  Serial.begin(115200); // initialize serial communication at 115200 bits per second:

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println(F("MAX30105 was not found. Please check wiring/power."));
    while (1);
  }

  particleSensor.setup(60, 4, 2, 100, 411, 4096); //Configure sensor with these settings

  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop()
{
  bufferLength = 100; //buffer length of 100 stores 4 seconds of samples running at 25sps
  Serial.println();
  for (byte i = 0 ; i < bufferLength ; i++)
  {
    while (particleSensor.available() == false) //do we have new data?
      particleSensor.check(); //Check the sensor for new data

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample(); //We're finished with this sample so move to next sample

    Serial.print(".");
  }

    Serial.println();
  //calculate heart rate and SpO2 after first 100 samples (first 4 seconds of samples)
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

  while (1)
  {  
    for (byte i = 25; i < 100; i++)
    {
      redBuffer[i - 25] = redBuffer[i];
      irBuffer[i - 25] = irBuffer[i];
    }

    for (byte i = 75; i < 100; i++)
    {
      while (particleSensor.available() == false) //do we have new data?
        particleSensor.check(); //Check the sensor for new data

      redBuffer[i] = particleSensor.getRed();
      irBuffer[i] = particleSensor.getIR();
      particleSensor.nextSample(); //We're finished with this sample so move to next sample

      heartRate=abs(heartRate%100);
      Serial.print(F("HR="));
      Serial.print(heartRate, DEC);

      Serial.print(F(", HRvalid="));
      Serial.print(validHeartRate, DEC);

      spo2=abs(spo2%100);
      Serial.print(F(", SPO2="));
      Serial.print(spo2, DEC);

      Serial.print(F(", SPO2Valid="));
      Serial.print(validSPO2, DEC);

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
      
      if(WiFi.status() != WL_CONNECTED)
      {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(SECRET_SSID);
        while(WiFi.status() != WL_CONNECTED)
        {
          WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
          Serial.print(".");
          delay(5000);     
        } 
        Serial.println("\nConnected.");
      }

      // set the fields with the values
      ThingSpeak.setField(1, heartRate);
      ThingSpeak.setField(2, truehumi_long);
      ThingSpeak.setField(3, spo2);
      ThingSpeak.setField(4, tempf);
      ThingSpeak.setField(5, ecgReading);

      // set the status
      ThingSpeak.setStatus(myStatus);
      
      // write to the ThingSpeak channel
      int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
      if(x == 200){
        Serial.println("Channel update successful.");
      }
      else{
        Serial.println("Problem updating channel. HTTP error code " + String(x));
      }
      
      delay(20000); // delay before loop
      
      Serial.println("_______________________________________________________________________");
      
    }

    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
  }
}

float round(float var) 
{ 
    float value = (int)(var * 100 + .5); 
    return (float)value / 100; 
} 
