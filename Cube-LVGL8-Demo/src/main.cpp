#include "RubikCube.h"
#include "lv_demo.h"
#include <Arduino.h>

unsigned long ticker1S = 0;
unsigned long ticker5Ms = 0;
TFT_eSPI tft = TFT_eSPI();
FT6336U ft6336u(I2C_SDA, I2C_SCL, RST_N_PIN, INT_N_PIN);
char str_buf[32] = {0};
hw_timer_t *lv_ticker = nullptr;

void setup() {
  Serial.begin(115200);
  setupDisplay(500);
  wifiAutoConnect();
  configSystemTime();
  // 打开OTA
  setupOTAConfig();
  delay(3000);
  // 运行LVGL程序
  lv_demo_widgets();
  // 开始运行LVGL
  lvglStartTick();
}

void loop() {
  auto ms = millis();
  // 5ms周期
  if (ms - ticker5Ms >= 5) {
    ticker5Ms = ms;
    lv_task_handler();
  }

  // 1秒周期
  if (ms - ticker1S >= 1000) {
    ticker1S = ms;
    ArduinoOTA.handle();
  }
}