#include <FlashAsEEPROM.h>
#include <FlashStorage.h>

#include <TinyGPS.h>
TinyGPS gps;

const byte interruptPin = 7;
float lat = 0, lon = 0;
unsigned long t, d, fix_age;
bool savePos = false;
int last_time = 0;
int last_flash_time = 0;
String data = "";

const int max_write_calls = 9500;
const int update_write_calls_interval = 60000; // Once every 60 seconds, blink an LED probs
int current_write_calls = 0;
FlashStorage(saved_current_write_calls, int);

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), savePosition, FALLING);
}

void loop() {
  //Serial.println(millis() - last_time);
  if (savePos && (millis() - last_time) > 1000) {
      // Serial.println("SAVE POS");
      savePos = false;
      last_time = millis();
    }
   if (millis() - last_flash_time > update_write_calls_interval) {
    // CHECK IF FLASH AND VAR IS SAME, IF NOT SAVE NEW AND UPDATE THE TIMER
    last_flash_time = millis();
   }
  // ddmmyy
  // hhmmddcc
  while (Serial1.available()) {
    
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
      //Serial.println(date_time+" : " + latitude + ";" + longitude);

      data = "GPSFIKS," + date_time + "," + latitude + "," + longitude;
      // Serial.println(data);
    }
  }
}

void saveToFlashStorage(String data_string) {
  // SAVE CURRENT DRAW CALL TO FLASH ALSO!!
  if (current_write_calls < max_write_calls) {
    
  }else {
    // FLASH RED LED
  }
}

void savePosition() {
  savePos = true;
}
