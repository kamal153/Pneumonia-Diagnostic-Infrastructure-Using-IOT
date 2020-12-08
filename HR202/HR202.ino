int temppin = 12; // analog pin
int humpin = 13; // humidity
int tempc = 0,tempf=0; // temperature variables
int maxi = -100,mini = 100; // to start max/min temperature

float humi = 0;
float prehum = 0;
float humconst = 0;
float truehum = 0;
float pretruehum = 0;
long pretruehumconst = 0;
long valb = 0;

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
delay(1000);

Serial.print(tempf,DEC);
Serial.println(" Celsius, ");
Serial.print("Humidity: ");
Serial.print((long)truehum);
Serial.println ("% ");
Serial.println("_______________________________________________________________________");
delay(1000); // delay before loop
}
