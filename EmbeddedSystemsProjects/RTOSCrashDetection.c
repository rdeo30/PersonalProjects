//ESP32-C6 RTOS Crash Detection
//Rajan Deo

#include <Arduino.h>
#include <Wire.h>
#include <ICM_20948.h>

#define DisplaySerial Serial1
#include "Goldelox_Serial_4DLib.h"
#include "Goldelox_Const4D.h"

//
#define DISPLAY_RX 16    // ESP32 RX <- uLCD TX
#define DISPLAY_TX 17    // ESP32 TX -> uLCD RX
#define RESETLINE  2     // uLCD RESET
Goldelox_Serial_4DLib Display(&DisplaySerial);

//I2C pinout for IMU
#define SDA_PIN 21
#define SCL_PIN 22

//PWM for motor controller 
#define PIN_PWMA 5
#define PIN_AIN1 6
#define PIN_AIN2 7
#define PIN_STBY 8
#define PIN_POT  4

// PWM initalization values
const int PWM_FREQ_HZ = 20000;
const int PWM_RES_BITS = 8;

// Motor tuning
const int  MIN_START_DUTY = 12;   // overcome stiction
const int  STOP_DEADBAND = 8;    // near zero knob = stop
const bool INVERT_KNOB = false;

//
ICM_20948_I2C imu;

// Smoothing & crash threshold
float ax_s = 0.0f, ay_s = 0.0f, az_s = 0.0f;
const float IMU_SMOOTHING = 0.8f;
const float CRASH_G       = 2.5f;

// 0.5g = ~30 deg tilt, 0.7g = ~45 deg tilt
const float FORWARD_TILT_G = 0.2f; 

static float desiredSpeed = 0.0f;  
static float currentSpeed = 0.0f;  
static bool  crashLatched = false;

// Shared IMU data (for display)
static float shared_ax = 0.0f;
static float shared_ay = 0.0f;
static float shared_az = 0.0f;
static float shared_gx = 0.0f;
static float shared_gy = 0.0f;
static float shared_gz = 0.0f;
static float shared_mx = 0.0f;
static float shared_my = 0.0f;
static float shared_mz = 0.0f;
static float shared_temp = 0.0f;

static SemaphoreHandle_t stateMutex;
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
static EventGroupHandle_t crashEvents;
#define CRASH_BIT (1 << 0)

//draws info to uLCD screen
static inline void drawText(uint8_t col, uint8_t row, const String& text, uint16_t fg=WHITE, uint16_t bg=BLACK) {
  Display.txt_FGcolour(fg);
  Display.txt_BGcolour(bg);
  Display.txt_MoveCursor(row, col);
  for (size_t i = 0; i < text.length(); ++i) Display.putCH(text[i]);
}

// Green ball(no crash yet)
void drawSmoothBall() {
  // Clear icon area first
  Display.gfx_RectangleFilled(0, 80, 127, 127, BLACK);
  // Draw green ball
  Display.gfx_CircleFilled(64, 104, 18, GREEN);
}

// Red STOP after crash detected
void drawStopSign() {
  // Red panel
  Display.gfx_RectangleFilled(0, 80, 127, 127, RED);
  // White STOP text centered
  Display.txt_FGcolour(WHITE);
  Display.txt_BGcolour(RED);
  Display.txt_MoveCursor(6, 4); 
  char stopTxt[] = "STOP";
  Display.putstr(stopTxt);
}

void runMotorDuty(uint8_t duty) {
  digitalWrite(PIN_AIN1, HIGH);
  digitalWrite(PIN_AIN2, LOW);
  digitalWrite(PIN_STBY, HIGH);
  ledcWrite(PIN_PWMA, duty);
}

void stopMotor() {
  digitalWrite(PIN_AIN1, LOW);
  digitalWrite(PIN_AIN2, LOW);
  ledcWrite(PIN_PWMA, 0);
}

//uLCD init
void initULCD() {
  pinMode(RESETLINE, OUTPUT);
  digitalWrite(RESETLINE, HIGH); delay(500);
  digitalWrite(RESETLINE, LOW);  delay(500);
  digitalWrite(RESETLINE, HIGH); delay(7000);

  DisplaySerial.begin(9600, SERIAL_8N1, DISPLAY_RX, DISPLAY_TX);
  Display.TimeLimit4D = 5000;

  Display.gfx_Cls();

  //Title in YELLOW
  drawText(0, 0, "Vehicle Monitor", YELLOW);
  //Static labels; values will be updated by DisplayTask
  drawText(0, 2, "Speed: --", WHITE);
  drawText(0, 3, "X: --.- g", CYAN);
  drawText(0, 4, "Y: --.- g", CYAN);
  drawText(0, 5, "Status: OK", GREEN);

  //Initial smooth icon
  drawSmoothBall();
}

//find IMU address (either 0x69 or 0x68 for my IMU)
bool initIMU() {
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(400000);
  
  Serial.print("[IMU] Probing 0x69 ... ");
  imu.begin(Wire, 1); // AD0_VAL = 1
  if (imu.status == ICM_20948_Stat_Ok) {
    Serial.println("found!");
    return true;
  }
  Serial.println("not found.");

  Serial.print("[IMU] Probing 0x68 ... ");
  imu.begin(Wire, 0); // AD0_VAL = 0
  if (imu.status == ICM_20948_Stat_Ok) {
    Serial.println("found!");
    return true;
  }
  Serial.println("not found.");
  
  return false;
}

//tasks
void MotorTask(void* arg) {
  const float slew = 2.0f; // % per tick
  const TickType_t period = pdMS_TO_TICKS(20);

  for (;;) {
    EventBits_t bits = xEventGroupGetBits(crashEvents);

    float want, cur;
    bool crashed;
    xSemaphoreTake(stateMutex, portMAX_DELAY);
    want = desiredSpeed;
    cur  = currentSpeed;
    crashed = crashLatched || (bits & CRASH_BIT);
    xSemaphoreGive(stateMutex);

    if (crashed) {
      cur = 0.0f;
      stopMotor();
    } else {
      // Slew toward desired
      if (cur < want) cur = min(cur + slew, want);
      else if (cur > want) cur = max(cur - slew, want);

      // Duty & run
      uint8_t duty = (uint8_t)map((int)roundf(cur), 0, 100, 0, 255);
      if (duty <= STOP_DEADBAND) {
        stopMotor();
      } else {
        if (duty < MIN_START_DUTY) duty = MIN_START_DUTY;
        runMotorDuty(duty);
      }
    }

    xSemaphoreTake(stateMutex, portMAX_DELAY);
    currentSpeed = cur;
    xSemaphoreGive(stateMutex);

    vTaskDelay(period);
  }
}

void PotTask(void* arg) {
  const TickType_t period = pdMS_TO_TICKS(50);
  for (;;) {
    int raw = analogRead(PIN_POT); // 0..4095
    if (INVERT_KNOB) raw = 4095 - raw;
    float pct = (float)raw / 4095.0f * 100.0f;

    xSemaphoreTake(stateMutex, portMAX_DELAY);
    desiredSpeed = pct;
    xSemaphoreGive(stateMutex);

    vTaskDelay(period);
  }
}

void ImuTask(void* arg) {
  if (!initIMU()) {
    Serial.println("ICM not detected");
    vTaskDelete(NULL);
  }

  Serial.println("Init OK. ImuTask running.");

  const TickType_t period = pdMS_TO_TICKS(10);
  for (;;) {
    if (imu.dataReady()) {
      imu.getAGMT(); 

      //Depending on ICM library scale, keep using previous conversions
      float ax_g = imu.accX() / 1000.0f;
      float ay_g = imu.accY() / 1000.0f;
      float az_g = imu.accZ() / 1000.0f;

      //Smooth accel for crash detection & display
      ax_s = ax_s * IMU_SMOOTHING + ax_g * (1.0f - IMU_SMOOTHING);
      ay_s = ay_s * IMU_SMOOTHING + ay_g * (1.0f - IMU_SMOOTHING);
      az_s = az_s * IMU_SMOOTHING + az_g * (1.0f - IMU_SMOOTHING);

      float gx_dps = imu.gyrX();
      float gy_dps = imu.gyrY();
      float gz_dps = imu.gyrZ();

      float mx_uT = imu.magX();
      float my_uT = imu.magY();
      float mz_uT = imu.magZ();

      float temp_C = imu.temp();

      float mag_g = sqrtf(ax_s*ax_s + ay_s*ay_s + az_s*az_s);
      bool triggerCrash = (mag_g >= CRASH_G || ay_s > FORWARD_TILT_G);
      
      xSemaphoreTake(stateMutex, portMAX_DELAY);
      shared_ax = ax_s;
      shared_ay = ay_s;
      shared_az = az_s;
      shared_gx = gx_dps;
      shared_gy = gy_dps;
      shared_gz = gz_dps;
      shared_mx = mx_uT;
      shared_my = my_uT;
      shared_mz = mz_uT;
      shared_temp = temp_C;
      if (triggerCrash) crashLatched = true;
      xSemaphoreGive(stateMutex);

      if (triggerCrash) {
        xEventGroupSetBits(crashEvents, CRASH_BIT);
      }
    }
    vTaskDelay(period);
  }
}

void DisplayTask(void* arg) {
  const TickType_t period = pdMS_TO_TICKS(100);
  bool prevCrash = false;

  for (;;) {
    float sp, ax, ay;
    bool crashedNow;

    xSemaphoreTake(stateMutex, portMAX_DELAY);
    sp = currentSpeed;
    ax = shared_ax;
    ay = shared_ay;
    crashedNow = crashLatched || ((xEventGroupGetBits(crashEvents) & CRASH_BIT) != 0);
    xSemaphoreGive(stateMutex);

    //Speed
    {
      char buf[24];
      snprintf(buf, sizeof(buf), "Speed: %3.0f", sp);
      drawText(0, 2, buf, WHITE, BLACK);
    }

    //X/Y accel (g)
    {
      char buf[24];
      snprintf(buf, sizeof(buf), "X: %+0.2f g   ", ax);
      drawText(0, 3, buf, CYAN, BLACK);
      snprintf(buf, sizeof(buf), "Y: %+0.2f g   ", ay);
      drawText(0, 4, buf, CYAN, BLACK);
    }

    // Status + Icon
    if (crashedNow) {
      drawText(0, 5, "Status: CRASH", YELLOW, BLACK);
      if (!prevCrash) {
        drawStopSign();        // red STOP at bottom
      }
    } else {
      drawText(0, 5, "Status: OK   ", GREEN, BLACK);
      if (prevCrash) {
        drawSmoothBall();      // return to green ball after clear
      } else if (!prevCrash) {
        // keep green ball visible
        drawSmoothBall();
      }
    }

    prevCrash = crashedNow;
    vTaskDelay(period);
  }
}

//write 'c' to serial monitor to clear crash 
void clearCrashAndResume() {
  xEventGroupClearBits(crashEvents, CRASH_BIT);
  xSemaphoreTake(stateMutex, portMAX_DELAY);
  crashLatched = false;
  xSemaphoreGive(stateMutex);
  //Restore visuals
  drawText(0, 5, "Status: OK   ", GREEN, BLACK);
  drawSmoothBall();
}

void setup() {
  Serial.begin(115200);
  delay(200);

  //uLCD
  initULCD();

  //Motor driver pins
  pinMode(PIN_AIN1, OUTPUT);
  pinMode(PIN_AIN2, OUTPUT);
  pinMode(PIN_STBY, OUTPUT);
  digitalWrite(PIN_STBY, HIGH); // enable driver

  //Attach PWM (Arduino-ESP32 v3 API)
  ledcAttach(PIN_PWMA, PWM_FREQ_HZ, PWM_RES_BITS);
  ledcWrite(PIN_PWMA, 0);

  //ADC
  analogReadResolution(12); // 0..4095

  //RTOS
  stateMutex  = xSemaphoreCreateMutex();
  crashEvents = xEventGroupCreate();

  //Start tasks
  xTaskCreate(MotorTask,   "MotorTask",   4096, nullptr, 3, nullptr);
  xTaskCreate(PotTask,     "PotTask",     3072, nullptr, 2, nullptr);
  xTaskCreate(ImuTask,     "ImuTask",     4096, nullptr, 3, nullptr);
  xTaskCreate(DisplayTask, "DisplayTask", 4096, nullptr, 1, nullptr);

  Serial.println("System started. Type 'c' in Serial to clear CRASH.");
}

void loop() {
  if (Serial.available()) {
    int c = Serial.read();
    if (c == 'c' || c == 'C') {
      clearCrashAndResume();
      Serial.println("Crash cleared.");
    }
  }
  vTaskDelay(pdMS_TO_TICKS(50));
}

