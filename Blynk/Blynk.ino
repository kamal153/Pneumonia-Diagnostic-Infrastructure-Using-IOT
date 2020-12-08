#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "82c4536040084862bd9a18f778f982a0";

// Replace with your network credentials
const char* ssid     = "KAMAL_KAKADIYA 1231";
const char* password = "434[E5v6";

void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, password);
}

void loop()
{
  Blynk.run();
  Blynk.notify("Health monitering");
  delay(2000);
}
