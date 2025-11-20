//Program that allows ESP-32 to read in values from a capacitive keyboard.

#include <Wire.h>
#include <Adafruit_MPR121.h>

//Create MPR121 object
Adafruit_MPR121 cap = Adafruit_MPR121();

int keys[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

//Track previous touch state to detect new presses
uint16_t lastTouched = 0;
uint16_t currTouched = 0;

void setup() {
  Serial.begin(115200);
  
  delay(1000);
  
  
  // nitialize I2C with GPIO21 (SDA) and GPIO22 (SCL)
  Wire.setPins(21, 22);
  
  //Initialize MPR121 with default I2C address 0x5A
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found");
    while (1);
  }
  
  Serial.println("MPR121 found");
  
  //Enable auto-configuration
  cap.setAutoconfig(true);

}

void loop() {
  //Get currently touched pads
  currTouched = cap.touched();
  
  //Check each of the 12 electrodes
  for (int i = 0; i < 12; i++) {
    //Check if electrode is currently touched
    if ((currTouched & (1 << i)) && !(lastTouched & (1 << i))) {
      //New touch detected (wasn't touched before, is touched now)
      Serial.print("Key pressed: ");
      Serial.println(keys[i]);
    }
  }
  
  //Update last touched state
  lastTouched = currTouched;
  
  //Small delay for debouncing
  delay(50);
}

