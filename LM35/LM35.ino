int temppin = 12; // analog pin
int tempc = 0,tempf=0; // temperature variables
int maxi = -100,mini = 100; // to start max/min temperature

void setup()
{
Serial.begin(9600); // start serial communication
}

void loop()
{

tempc = tempc/8.0;
tempf = (tempc * 9)/ 5 + 32;
delay(1000);

Serial.print(tempf,DEC);
Serial.println(" Celsius");
Serial.println("_______________________________________________________________________");
delay(1000); // delay before loop
}
