#ifndef __RUBIK_LVGL_H__
#define __RUBIK_LVGL_H__

#include <TFT_eSPI.h>
#include <lvgl.h>
#include <FT6336U.h>

extern TFT_eSPI tft;
extern FT6336U ft6336u;
extern hw_timer_t *lv_ticker;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[TFT_HEIGHT * 10];

void inline lvglStartTick() {
  lv_ticker = timerBegin(0, 80, true);
  timerAttachInterrupt(
      lv_ticker, [] { lv_tick_inc(1); }, true);
  timerAlarmWrite(lv_ticker, 1000, true);
  timerAlarmEnable(lv_ticker);
}

void inline lvglDisplayInit() {
  lv_init();
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, TFT_HEIGHT * 10);
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = TFT_HEIGHT;
  disp_drv.ver_res = TFT_WIDTH;
  /*显示设备*/
  disp_drv.flush_cb = [](lv_disp_drv_t *disp, const lv_area_t *area,
                         lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors(&color_p->full, w * h, true);
    tft.endWrite();
    lv_disp_flush_ready(disp);
  };
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /*触摸输入设备*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = [](lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    FT6336U_TouchPointType tp = ft6336u.scan();
    if (tp.touch_count) {
      // 只管一个触摸点
      data->point.x = tp.tp[0].x;
      data->point.y = tp.tp[0].y;
      data->state = LV_INDEV_STATE_PR;
    } else {
      data->state = LV_INDEV_STATE_REL;
    }
  };
  lv_indev_drv_register(&indev_drv);
}

#endif