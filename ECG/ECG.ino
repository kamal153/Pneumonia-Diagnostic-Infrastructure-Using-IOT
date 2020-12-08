int ecgPin=12;
float ecgReading;
  
void setup() {
  Serial.begin(115200);
  pinMode(ecgPin,INPUT);
}

void loop() {
  ecgReading = analogRead(ecgPin);
  ecgReading = round(ecgReading);
  Serial.print("Ecg : ");
  Serial.println(ecgReading/2-30);
  delay(500);
}

float round(float var) 
{ 
    float value = (int)(var * 100 + .5); 
    return (float)value / 100; 
} 
