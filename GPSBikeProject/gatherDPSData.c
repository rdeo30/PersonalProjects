/*
 * @author Rajan Deo
 * - Reads GPS data via UART
 * - Parses latitude/longitude with TinyGPS++
 * - Displays lat/lon on uLCD
 */

#include <Arduino.h>

//
#define DisplaySerial Serial1
#include "Goldelox_Serial_4DLib.h"
#include "Goldelox_Const4D.h"

#include <TinyGPSPlus.h>
HardwareSerial GPSSerial(2);//Uses UART2 for GPS
TinyGPSPlus gps;

//uLCD Pins
#define DISPLAY_RX 17    // ESP32 RX <- uLCD TX
#define DISPLAY_TX 16    // ESP32 TX -> uLCD RX
#define RESETLINE  2     // uLCD RESET

Goldelox_Serial_4DLib Display(&DisplaySerial);

//GPS Pins
#define GPS_RX 5         // ESP32 RX <- GPS TX
#define GPS_TX 4         // ESP32 TX -> GPS RX 

void mycallback(int ErrCode, unsigned char ErrByte)
{
  Serial.print(F("uLCD Error "));
  Serial.print(ErrCode);
  Serial.print(F("  Byte: 0x"));
  Serial.println(ErrByte, HEX);
}

void displayText(uint16_t x, uint16_t y, uint16_t color, const char *text)
{
  Display.gfx_MoveTo(x, y);
  Display.txt_FGcolour(color);
  Display.txt_BGcolour(BLACK);
  Display.txt_Opacity(OPAQUE);
  Display.txt_Height(2);
  Display.txt_Width(2);
  Display.putstr(text);
}

void initDisplay()
{
  //Start the UART to the display
  DisplaySerial.begin(9600, SERIAL_8N1, DISPLAY_RX, DISPLAY_TX);

  //Reset pulse to uLCD
  pinMode(RESETLINE, OUTPUT);
  digitalWrite(RESETLINE, LOW);
  delay(100);
  digitalWrite(RESETLINE, HIGH);
  delay(2000); 

  //Setup library time limit & callback
  Display.TimeLimit4D(5000);
  Display.Callback4D(mycallback);

  //Clear screen and basic text settings
  Display.gfx_Cls();
  Display.txt_BGcolour(BLACK);
  Display.txt_FGcolour(WHITE);
  Display.txt_Opacity(OPAQUE);
  Display.txt_Height(2);
  Display.txt_Width(2);

  displayText(0, 0, YELLOW, "GPS\n COORDINATES");
}

void initGPS()
{
  //Start GPS serial at 9600 baud
  GPSSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
}

//Simple helper: feed all available GPS bytes into TinyGPS++
void readGPS()
{
  while (GPSSerial.available() > 0)
  {
    char c = GPSSerial.read();
    gps.encode(c);
    //For debugging:
    // Serial.write(c);
  }
}

void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println();

  initDisplay();
  initGPS();

}

unsigned long lastUpdate = 0;

void loop()
{
  //Continuously read incoming GPS data
  readGPS();

  //Update display at most once per second
  if (millis() - lastUpdate >= 1000)
  {
    lastUpdate = millis();

    Display.gfx_Cls();
    displayText(0, 0, YELLOW, "GPS Lat/Lon Demo");

    if (gps.location.isValid())
    {
      double lat = gps.location.lat();
      double lon = gps.location.lng();

      char buf[32];

      // Print to Serial Monitor
      Serial.print(F("Lattitude: "));
      Serial.print(lat, 6);
      Serial.print(F("Longitude: "));
      Serial.println(lon, 6);

      // Show on uLCD
      snprintf(buf, sizeof(buf), "Lat: %.6f", lat);
      displayText(0, 30, WHITE, buf);

      snprintf(buf, sizeof(buf), "Lon: %.6f", lon);
      displayText(0, 60, WHITE, buf);

    }
    else
    {
      Serial.println(F("No valid GPS"));
      displayText(0, 30, RED, "No valid GPS detect");
    }
  }
}

