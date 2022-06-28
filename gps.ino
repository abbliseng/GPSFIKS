#include <SoftwareSerial.h>
#include <TinyGPS.h>

float lat = 0, lon = 0;

SoftwareSerial gpsSerial(3, 4); //rx,tx
TinyGPS gps; // create gps object

void setup() {
  Serial.begin(9600); // connect serial
  //Serial.println("The GPS Received Signal:");
  gpsSerial.begin(9600); // connect gps sensor
}

void loop() {
  while (gpsSerial.available()) {
    if (gps.encode(gpsSerial.read())) {
      gps.f_get_position(&lat, &lon); // get latitude and longitude
      String latitude = String(lat, 6);
      String longitude = String(lon, 6);
      Serial.println(latitude + ";" + longitude);
    }
  }
}
