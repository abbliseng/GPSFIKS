#include <TinyGPS.h>
TinyGPS gps;

float lat = 0, lon = 0;
unsigned long t, d, fix_age;
String data = "";
bool updated = false;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (updated){
    String latitude = String(lat, 6);
    String longitude = String(lon, 6);
    Serial.println(latitude + ";" + longitude);
    //Serial.println(d);

    int hour = t / 1000000;
    int minute = (t - (t / 1000000 * 1000000)) / 10000;
    int second = (t - ((t - (t / 1000000 * 1000000)) / 10000) * 10000 - (t / 1000000 * 1000000)) / 100;
    String t_formatted = String(hour) + ":" + String(minute) + ":" + String(second);
    Serial.println(String(d) + ";" + t_formatted + ";" + latitude + ";" + longitude);
    
    updated = false;
  }
  while (Serial1.available()) {
    // Serial.println("STILL WORKING");
    if (gps.encode(Serial1.read())){
      Serial.println("MAYBE WORKING");
      gps.f_get_position(&lat, &lon, &fix_age); // get latitude and longitude
      gps.get_datetime(&d, &t, &fix_age);
      updated = true;
      
      /*
      
      Serial.println(date_time+";" + latitude + ";" + longitude);
      */
    }
    
    //Serial.println(date_time);
    /*
    String hour = String(t / 1000000);
    String minute = String((t - (t / 1000000 * 1000000)) / 10000);
    String second = String((t - ((t - (t / 1000000 * 1000000)) / 10000) * 10000 - (t / 1000000 * 1000000)) / 100);
    String date_time = d + "," + hour + ":" + minute + ":" + second;
    
    if (gps.encode(Serial1.read())) {
      gps.f_get_position(&lat, &lon, &fix_age); // get latitude and longitude
      gps.get_datetime(&d, &t, &fix_age);
      String latitude = String(lat, 6);
      String longitude = String(lon, 6);
      
      String hour = String(t / 1000000);
      String minute = String((t - (t / 1000000 * 1000000)) / 10000);
      String second = String((t - ((t - (t / 1000000 * 1000000)) / 10000) * 10000 - (t / 1000000 * 1000000)) / 100);

      //String date_time = year + "-" + month + "-" + day + "," + hour + ":" + minute + ":" + second;
      String date_time = d + "," + hour + ":" + minute + ":" + second;
      Serial.println(date_time+" : " + latitude + ";" + longitude);

      //data = "GPSFIKS," + date_time + "," + latitude + "," + longitude;
      //Serial.println(data);
    }
    */
  }
}
