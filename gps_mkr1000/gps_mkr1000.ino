#include <TinyGPS.h>
TinyGPS gps;
/*
   PROGRAMMET PÅ DATORN SKA KUNNA
   1. Fråga efter hur mycket minne den har,
   2. Spara det som finns i minnet till en ny csv-fil,
   3. Tömma minnet
*/
const byte interruptPin = 7;

float lat = 0, lon = 0;
unsigned long t, d, fix_age;

bool savePos = false;
int last_time = 0;

String data = "";

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), savePosition, FALLING);
}

void loop() {
  //Serial.println(millis() - last_time);
  if (savePos && (millis() - last_time) > 1000) {
      Serial.println("SAVE POS");
      savePos = false;
      last_time = millis();
    }
  // ddmmyy
  // hhmmddcc
  while (Serial1.available()) {
    
    if (gps.encode(Serial1.read())) {
      gps.f_get_position(&lat, &lon, &fix_age); // get latitude and longitude
      gps.get_datetime(&d, &t, &fix_age);
      String latitude = String(lat, 6);
      String longitude = String(lon, 6);
      /*
      String day = String(d / 10000);
      String month = String((d - (d / 10000) * 10000) / 100);
      String year = String(d - ((d - (d / 10000) * 10000) / 100) * 100 - (d / 10000) * 10000);
      */
      String hour = String(t / 1000000);
      String minute = String((t - (t / 1000000 * 1000000)) / 10000);
      String second = String((t - ((t - (t / 1000000 * 1000000)) / 10000) * 10000 - (t / 1000000 * 1000000)) / 100);

      //String date_time = year + "-" + month + "-" + day + "," + hour + ":" + minute + ":" + second;
      String date_time = d + "," + hour + ":" + minute + ":" + second;
      //Serial.println(date_time+" : " + latitude + ";" + longitude);

      data = "GPSFIKS," + date_time + "," + latitude + "," + longitude;
      Serial.println(data);
    }
  }
}

void savePosition() {
  savePos = true;
}
