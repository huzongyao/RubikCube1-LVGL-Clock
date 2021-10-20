#include "RubikCube.h"
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
  lv_obj_t *label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "Hello Arduino! (V8.0.X)");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
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