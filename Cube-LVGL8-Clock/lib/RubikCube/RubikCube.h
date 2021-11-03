#ifndef __RUBIK_CUBE_H__
#define __RUBIK_CUBE_H__

#include "RubikLvgl.h"
#include <Arduino.h>
#include <ArduinoOTA.h>
#include <FT6336U.h>
#include <TFT_eSPI.h>

#define SPEAKER_DAC 25

#define TZ 8     // 中国时区为8
#define DST_MN 0 // 默认为0

#define TZ_MN ((TZ)*60) //时间换算
#define TZ_SEC ((TZ)*3600)
#define DST_SEC ((DST_MN)*60)

#define I2C_SDA 21
#define I2C_SCL 22
#define RST_N_PIN 38
#define INT_N_PIN 37

extern TFT_eSPI tft;
extern FT6336U ft6336u;
extern char str_buf[32];
extern hw_timer_t *lv_ticker;

void inline configSystemTime() {
  configTime(TZ_SEC, DST_SEC, "ntp.ntsc.ac.cn", "ntp1.aliyun.com");
}

void inline setupDisplay(int brightness) {
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextFont(2);
  lvglDisplayInit();
  // LED背光
  ledcSetup(10, 5000, 10);
  ledcAttachPin(TFT_BL, 10);
  analogReadResolution(10);
  ledcWrite(10, brightness);
  // 触摸
  ft6336u.begin();
}

void inline wifiAutoConnect() {
  tft.println("Start Wifi Connect!");
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin();
  for (int i = 0; WiFi.status() != WL_CONNECTED && i < 100; i++) {
    delay(100);
  }
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.mode(WIFI_MODE_APSTA);
    WiFi.beginSmartConfig();
    tft.println("Connect With EspTouch!");
    while (WiFi.status() != WL_CONNECTED) {
      delay(100);
    }
    WiFi.stopSmartConfig();
    WiFi.mode(WIFI_MODE_STA);
  }
  tft.println("Wifi Connected!");
  tft.printf("IP: %s\n", WiFi.localIP().toString().c_str());
}

void inline disableSpeaker() {
  pinMode(SPEAKER_DAC, OUTPUT);
  digitalWrite(SPEAKER_DAC, LOW);
}

void inline setupOTAConfig() {
  ArduinoOTA.onStart([] {
    timerEnd(lv_ticker);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("OTA Update", 160, 60, 4);
    tft.drawRoundRect(60, 160, 200, 20, 6, TFT_ORANGE);
  });
  ArduinoOTA.onProgress([](u32_t pro, u32_t total) {
    tft.setTextColor(TFT_SKYBLUE, TFT_BLACK);
    sprintf(str_buf, "%d / %d", pro, total);
    tft.drawCentreString(str_buf, 160, 120, 2);
    int pros = pro * 200 / total;
    tft.fillRoundRect(60, 160, pros, 20, 6, TFT_ORANGE);
  });
  ArduinoOTA.onEnd([] {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("Update Succeed!!", 160, 60, 4);
    tft.setTextColor(TFT_SKYBLUE, TFT_BLACK);
    tft.drawCentreString("Restarting...", 160, 140, 2);
  });
  ArduinoOTA.onError([](ota_error_t err) { ESP.restart(); });
  ArduinoOTA.begin();
}

#endif