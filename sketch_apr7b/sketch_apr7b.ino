#include <SPI.h>
#include <SD.h>

#include <TinyGPS.h>
TinyGPS gps;

const byte interruptPin = 7;
float lat = 0, lon = 0;
unsigned long t, d, fix_age;
bool savePos = false;
int last_time = 0;
String data = "";

int incomingByte;

const int chipSelect = SDCARD_SS_PIN;
File fish_storage;
String titles = "date,time,lat,long";
String filename = "fishies.csv";

int save_delay = 1500;

bool warning = true;
int last_warning_blink_time = 0;
bool warning_led_state = true;
int init_warning = 0;

bool warning_gps = false;
bool warning_gps_state = false;
bool save_blink = false;
int gps_cutoff_time = 2000;
int last_gps_time = 0;

// GPS debugging variables
unsigned long last_debug_print = 0;
int debug_print_interval = 1000;  // Print status every second
unsigned long gps_chars_received = 0;
unsigned long gps_sentences_valid = 0;
unsigned long gps_sentences_failed = 0;
bool had_fix = false;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  delay(500);

  Serial.println("=== GPS Fish Logger Starting ===");
  Serial.println("Waiting for GPS data...");

  // Initialize SD card
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  // Initialize interrupt for button
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), savePosition, FALLING);

  // LEDS
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  digitalWrite(0, LOW);
  digitalWrite(1, LOW);
}

void loop() {
  if ((millis() - last_gps_time) > gps_cutoff_time) {
    warning_gps = true;
  } else {
    warning_gps = false;
  }

  if ((millis() - last_warning_blink_time) > 500) {
    if (warning) {
      digitalWrite(0, !warning_led_state);
      warning_led_state = !warning_led_state;  
    } else {
      digitalWrite(0, false);
      warning_led_state = false;
    }
    if (warning_gps) {
      digitalWrite(1, !warning_gps_state);
      warning_gps_state = !warning_gps_state;
      save_blink = false;
    }
    else if (save_blink) {
      digitalWrite(1, false);
      if ((millis() - last_time) > save_delay) {
        save_blink = false;
      }
    }
    else {
      digitalWrite(1, true);
      warning_gps_state = true;
    }
    last_warning_blink_time = millis();
  } 

  // CHECK IF SAVING POSITION
  if (savePos) {
      Serial.println("SAVE POS : "+data);
      save_blink = true;
      savePosition(data);
      savePos = false;
  }

  // READ THE GPS
  while (Serial1.available()) {
    char c = Serial1.read();
    gps_chars_received++;
    
    if (gps.encode(c)) {
      gps_sentences_valid++;
      init_warning += 1;
      if ((init_warning >= 25) && (init_warning <= 100)) {
        warning = false;
        init_warning = 101;
        Serial.println(">>> GPS INITIALIZED - Valid data stream established <<<");
      }
      
      gps.f_get_position(&lat, &lon, &fix_age);
      gps.get_datetime(&d, &t, &fix_age);
      
      // Check if we just got a fix
      if (!had_fix && fix_age != TinyGPS::GPS_INVALID_AGE && fix_age < 2000) {
        had_fix = true;
        Serial.println(">>> GPS FIX ACQUIRED <<<");
        Serial.print("    Position: ");
        Serial.print(lat, 6);
        Serial.print(", ");
        Serial.println(lon, 6);
      }
      
      // Check if we lost the fix
      if (had_fix && (fix_age == TinyGPS::GPS_INVALID_AGE || fix_age > 5000)) {
        had_fix = false;
        Serial.println(">>> GPS FIX LOST <<<");
      }
      
      String latitude = String(lat, 6);
      String longitude = String(lon, 6);
      
      String hour = String(t / 1000000);
      String minute = String((t - (t / 1000000 * 1000000)) / 10000);
      String second = String((t - ((t - (t / 1000000 * 1000000)) / 10000) * 10000 - (t / 1000000 * 1000000)) / 100);

      int day = d / 10000;
      int month = (d - day*10000) / 100;
      int year = (d - (day*10000) - (month*100));

      String day_str = String(day);
      if (day < 10) {day_str = "0"+String(day);}
      String month_str = String(month);
      if (month < 10) {month_str = "0"+String(month);}
      String year_str = "20"+String(year);

      String date = year_str + "-" + month_str + "-" + day_str;

      String date_time = date + "," + hour + ":" + minute + ":" + second;
      data = date_time + "," + latitude + "," + longitude;
      last_gps_time = millis();
    }
  }

  // Periodic GPS status debug output
  if ((millis() - last_debug_print) > debug_print_interval) {
    printGpsDebug();
    last_debug_print = millis();
  }
}

void printGpsDebug() {
  Serial.println("--- GPS Status ---");
  Serial.print("  Chars received: ");
  Serial.println(gps_chars_received);
  Serial.print("  Valid sentences: ");
  Serial.println(gps_sentences_valid);
  
  // Get TinyGPS stats
  unsigned long chars;
  unsigned short sentences, failed;
  gps.stats(&chars, &sentences, &failed);
  Serial.print("  Failed checksums: ");
  Serial.println(failed);
  
  Serial.print("  Satellites: ");
  int sats = gps.satellites();
  if (sats == TinyGPS::GPS_INVALID_SATELLITES) {
    Serial.println("INVALID");
  } else {
    Serial.println(sats);
  }
  
  Serial.print("  Fix age (ms): ");
  if (fix_age == TinyGPS::GPS_INVALID_AGE) {
    Serial.println("NO FIX");
  } else {
    Serial.println(fix_age);
  }
  
  Serial.print("  Position: ");
  if (lat == TinyGPS::GPS_INVALID_F_ANGLE) {
    Serial.println("INVALID");
  } else {
    Serial.print(lat, 6);
    Serial.print(", ");
    Serial.println(lon, 6);
  }
  
  Serial.print("  HDOP: ");
  long hdop = gps.hdop();
  if (hdop == TinyGPS::GPS_INVALID_HDOP) {
    Serial.println("INVALID");
  } else {
    Serial.println(hdop / 100.0);
  }
  
  Serial.print("  Status: ");
  if (warning_gps) {
    Serial.println("NO DATA (timeout)");
  } else if (!had_fix) {
    Serial.println("SEARCHING FOR FIX");
  } else {
    Serial.println("FIX OK");
  }
  Serial.println();
}

// Save to position to the file.
void savePosition(String data_string) {
  bool exists = SD.exists(filename);
  Serial.println(exists);
  fish_storage = SD.open(filename, FILE_WRITE);
  if (fish_storage) {
    Serial.print("Writing to file...");
    if (!exists) {
      fish_storage.println(titles);
    }
    fish_storage.println(data_string);
    Serial.println("done.");
  } else {
    Serial.println("Error opening file");
  }
  fish_storage.close();
}

void savePosition() {
  if ((millis() - last_time) > save_delay) {
    savePos = true;
    last_time = millis();
  }
}
