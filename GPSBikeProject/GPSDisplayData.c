/*
 * @author Rajan Deo
 * - Draw 128x128 RGB565 RAW image from SD card
 * - Plot a dot on the map based on GPS latitude/longitude
 */

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

#define DisplaySerial Serial1
#include "Goldelox_Serial_4DLib.h"
#include "Goldelox_Const4D.h"

#include <TinyGPSPlus.h>

//uLCD PINS
#define DISPLAY_RX 17   // ESP32 RX <- uLCD TX
#define DISPLAY_TX 16   // ESP32 TX -> uLCD RX
#define RESETLINE  2    // uLCD RESET

Goldelox_Serial_4DLib Display(&DisplaySerial);

//SD CARD PINS (SPI)
#define SD_CS   7
#define SD_MOSI 5
#define SD_MISO 4
#define SD_SCK  6

//GPS PINS
#define GPS_RX 9         // ESP32 RX <- GPS TX
#define GPS_TX 8         // ESP32 TX -> GPS RX

HardwareSerial GPSSerial(2);   // UART2 for GPS
TinyGPSPlus gps;

const char *MAP_FILE = "/map.raw";
const int MAP_W = 128;
const int MAP_H = 128;
const int MAP_X0 = 0;
const int MAP_Y0 = 0;


//need to figure out a way to automate pulling this data from image 
const double MAP_LAT_MIN = 33.60;   // bottom of map
const double MAP_LAT_MAX = 33.84;   // top of map
const double MAP_LON_MIN = -84.55;  // left side of map
const double MAP_LON_MAX = -84.30;  // right side of map
=
//uLCD ERROR CALLBAC
void mycallback(int ErrCode, unsigned char ErrByte) {
  Serial.print(F("uLCD Error "));
  Serial.print(ErrCode);
  Serial.print(F("  Byte: 0x"));
  Serial.println(ErrByte, HEX);
}

void resetDisplay() {
  pinMode(RESETLINE, OUTPUT);
  digitalWrite(RESETLINE, HIGH);
  delay(100);
  digitalWrite(RESETLINE, LOW);
  delay(100);
  digitalWrite(RESETLINE, HIGH);
  delay(3000); //allow uLCD to boot
}


void displayText(uint16_t x, uint16_t y, uint16_t color, const char *text) {
  Display.gfx_MoveTo(x, y);
  Display.txt_FGcolour(color);
  Display.txt_BGcolour(BLACK);
  Display.txt_Opacity(OPAQUE);
  Display.txt_Height(2);
  Display.txt_Width(2);
  Display.putstr(text);
}

bool initDisplay() {
  Serial.println("Initializing...");
  resetDisplay();

  DisplaySerial.begin(9600, SERIAL_8N1, DISPLAY_RX, DISPLAY_TX);

  Display.TimeLimit4D(5000);
  Display.Callback4D(mycallback);

  Display.gfx_Cls();
  Display.txt_BGcolour(BLACK);
  Display.txt_FGcolour(WHITE);
  Display.txt_Opacity(OPAQUE);
  Display.txt_Height(2);
  Display.txt_Width(2);

  displayText(0, 0, YELLOW, "uLCD Ready");
  Serial.println(" Display OK.");
  return true;
}


bool initSD() {
  Serial.println("Initializing..");

  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  if (!SD.begin(SD_CS)) {
    Serial.println("failed");
    return false;
  }

  Serial.println("SD found!");
  return true;
}


bool drawRawImage(const char *filename, int width, int height, int x0, int y0) {
  File f = SD.open(filename, FILE_READ);
  if (!f) {
    Serial.print("Failed to open ");
    Serial.println(filename);
    return false;
  }

  size_t expectedSize = width * height * 2;
  if (f.size() != expectedSize) {
    f.close();
    return false;
  }

  Serial.print("[Image] Drawing ");
  Serial.println(filename);
  Display.gfx_Cls();

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int hb = f.read();
      int lb = f.read();
      if (hb < 0 || lb < 0) {
        Serial.println("  Unexpected EOF");
        f.close();
        return false;
      }
      uint16_t color = (hb << 8) | lb;
      Display.gfx_PutPixel(x0 + x, y0 + y, color);
    }

    if (y % 16 == 0) {
      Serial.print("  Row ");
      Serial.println(y);
    }
  }

  f.close();
  Serial.println("[Image] Done.");
  return true;
}


void initGPS() {
  Serial.println("[GPS] Initializing...");
  GPSSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
  Serial.println("  GPS OK (waiting for fix)...");
}


void readGPS() {
  while (GPSSerial.available() > 0) {
    char c = GPSSerial.read();
    gps.encode(c);
    // Serial.write(c); //uncomment 
  }
}


// Returns true if point is inside map bounds, false otherwise
bool latLonToPixel(double lat, double lon, int &px, int &py) {
  //Check bounds
  if (lat < MAP_LAT_MIN || lat > MAP_LAT_MAX ||
      lon < MAP_LON_MIN || lon > MAP_LON_MAX) {
    return false;
  }

  double u = (lon - MAP_LON_MIN) / (MAP_LON_MAX - MAP_LON_MIN);  //0..1
  double v = (MAP_LAT_MAX - lat) / (MAP_LAT_MAX - MAP_LAT_MIN);  //0..1, flipped

  //Scale to 0..127 and clamp
  px = (int)(u * (MAP_W - 1) + 0.5);
  py = (int)(v * (MAP_H - 1) + 0.5);

  if (px < 0) px = 0;
  if (px >= MAP_W) px = MAP_W - 1;
  if (py < 0) py = 0;
  if (py >= MAP_H) py = MAP_H - 1;

  //Offset by MAP_X0, MAP_Y0 when drawing
  return true;
}


void setup() {
  Serial.begin(115200);
  delay(2000);

  if (!initDisplay()) {
    Serial.println("Display init failed.");
    while (true) delay(1000);
  }

  if (!initSD()) {
    Serial.println("SD init failed.");
    Display.gfx_Cls();
    displayText(0, 40, RED, "SD ERROR");
    while (true) delay(1000);
  }

  initGPS();

  //Initial map draw
  if (!drawRawImage(MAP_FILE, MAP_W, MAP_H, MAP_X0, MAP_Y0)) {
    Display.gfx_Cls();
    displayText(0, 40, RED, "IMG ERROR");
  } else {
    displayText(0, 100, YELLOW, "Waiting GPS...");
  }
}

unsigned long lastUpdate = 0;

void loop() {
  //Continuously read incoming GPS data
  readGPS();

  //Update map + dot at most once per second (eats up a lot of power and bandwidth)
  if (millis() - lastUpdate >= 1000) {
    lastUpdate = millis();

    if (gps.location.isValid()) {
      double lat = gps.location.lat();
      double lon = gps.location.lng();

      char buf[48];

      Serial.print(F("Latitude: "));
      Serial.print(lat, 6);
      Serial.print(F("  Longitude: "));
      Serial.println(lon, 6);

      //Map lat/lon to pixel coordinates
      int px, py;
      bool onMap = latLonToPixel(lat, lon, px, py);

      //Redraw base map each update so the dot moves cleanly
      if (!drawRawImage(MAP_FILE, MAP_W, MAP_H, MAP_X0, MAP_Y0)) {
        Display.gfx_Cls();
        displayText(0, 40, RED, "IMG ERROR");
        return;
      }

      //Draw GPS dot if within bounds
      if (onMap) {
        int sx = MAP_X0 + px;
        int sy = MAP_Y0 + py;

        //Small red dot (3x3 block)
        for (int dy = -1; dy <= 1; dy++) {
          for (int dx = -1; dx <= 1; dx++) {
            int xx = sx + dx;
            int yy = sy + dy;
            if (xx >= MAP_X0 && xx < MAP_X0 + MAP_W &&
                yy >= MAP_Y0 && yy < MAP_Y0 + MAP_H) {
              Display.gfx_PutPixel(xx, yy, RED);
            }
          }
        }

        //Show coords text under the map
        snprintf(buf, sizeof(buf), "Lat: %.5f", lat);
        displayText(0, 100, WHITE, buf);

        snprintf(buf, sizeof(buf), "Lon: %.5f", lon);
        displayText(0, 120, WHITE, buf);

      } else {
        //Outside map bounds
        displayText(0, 100, RED, "Out of map bounds");
      }

    } else {
      Serial.println(F("No valid GPS location yet"));

      //Redraw map and show status text
      drawRawImage(MAP_FILE, MAP_W, MAP_H, MAP_X0, MAP_Y0);
      displayText(0, 100, RED, "No valid GPS");
    }
  }
}
