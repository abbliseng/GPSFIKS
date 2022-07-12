#include <FlashAsEEPROM.h>
#include <FlashStorage.h>

#include <TinyGPS.h>
TinyGPS gps;

const byte interruptPin = 7;
float lat = 0, lon = 0;
unsigned long t, d, fix_age;
bool savePos = false;
int last_time = 0;
String data = "";

int incomingByte;      // variable stores  serial data

typedef struct {
  boolean valid;
  String fish_positions[100];
  int amount_stored;
} FishStorage;


FishStorage current_fish_storage;

FlashStorage(fish_storage, FishStorage)

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  current_fish_storage = fish_storage.read();

  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), savePosition, FALLING);
}

void loop() {
  // CHECK IF WRITING POS TO PC
  if (Serial.available() > 0) {
    // read the oldest byte in the serial buffer:
    incomingByte = Serial.read();
    if (incomingByte == 'R') {
      sendFlashStorageData();
    }
    else if (incomingByte == 'C') {
      clearFlashStorageData();
    }
  }
  // CHECK IF SAVING POSITION
  if (savePos && (millis() - last_time) > 1000) {
      // Serial.println("SAVE POS : "+data);
      saveToFlashStorage(data);
      savePos = false;
      last_time = millis();
  }
  // CHECK TO READ THE GPS
  while (Serial1.available()) {
    if (gps.encode(Serial1.read())) {
      gps.f_get_position(&lat, &lon, &fix_age); // get latitude and longitude
      gps.get_datetime(&d, &t, &fix_age);
      String latitude = String(lat, 6);
      String longitude = String(lon, 6);
      
      String hour = String(t / 1000000);
      String minute = String((t - (t / 1000000 * 1000000)) / 10000);
      String second = String((t - ((t - (t / 1000000 * 1000000)) / 10000) * 10000 - (t / 1000000 * 1000000)) / 100);
      
      String date_time = String(d) + "," + hour + ":" + minute + ":" + second;

      data = date_time + "," + latitude + "," + longitude;
      //Serial.println(data);
    }
  }
}

void clearFlashStorageData() {
  for (int i = 0; i < current_fish_storage.amount_stored; i++) {
    current_fish_storage.fish_positions[i] = "";
  }
  current_fish_storage.amount_stored = 0;
  current_fish_storage.valid = false;
  Serial.print("FLASH CLEARED#");
  // SAVE THIS TO THE FLASH TOO OBVI
}

void sendFlashStorageData() {
  Serial.print("SENDING FLASH#");
  for (int i = 0; i < current_fish_storage.amount_stored; i++) {
    //Serial.print(String(i)+"#"+current_fish_storage.fish_positions[i]+"#");
    Serial.print(current_fish_storage.fish_positions[i]+"#");
  }
  Serial.print("FLASH SENT#");
}

void saveToFlashStorage(String data_string) {
  int current = 0;
  if (current_fish_storage.valid == false) {
    current_fish_storage.amount_stored = 0; // INITIALIZE THE STORED AMOUNT TO ONE
    current_fish_storage.valid = true;
  }
  current = current_fish_storage.amount_stored;
  current_fish_storage.fish_positions[current] = data_string; // SAVE THE DATA TO CORRECT LOCATION
  current_fish_storage.amount_stored = current + 1; // INCREASE SAVED NUMBER FOR NEXT TIME

  // DEBUG
  /*
  for (int i = 0; i < current_fish_storage.amount_stored; i++) {
    Serial.println(String(i)+" : "+current_fish_storage.fish_positions[i]);
  }
  */
}

void savePosition() {
  savePos = true;
}
