#include <SPI.h>
#include <SD.h>

File myFile;

// change this to match your SD shield or module;
// const int chipSelect = 6;
const int chipSelect = SDCARD_SS_PIN;

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  Serial.print("Initializing SD card...");

  if (!SD.begin()) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");  

  // re-open the file for reading:
  myFile = SD.open("fishies.csv");
  if (myFile) {
    Serial.println("fishies.csv:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening fishies.csv");
  }
}

void loop()
{
  // nothing happens after setup
}