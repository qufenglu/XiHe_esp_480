#pragma once

#define LCD_BK_LIGHT_ON_LEVEL  1
#define LCD_BK_LIGHT_OFF_LEVEL !LCD_BK_LIGHT_ON_LEVEL

#define LCD_PIXEL_CLOCK_HZ     (12 * 1000 * 1000)
#define PIN_NUM_BACK_LIGHT     2
#define PIN_NUM_HSYNC          39
#define PIN_NUM_VSYNC          41
#define PIN_NUM_DE             40
#define PIN_NUM_PCLK           42
#define PIN_NUM_DATA0          8 // B0
#define PIN_NUM_DATA1          3 // B1
#define PIN_NUM_DATA2          46 // B2
#define PIN_NUM_DATA3          9 // B3
#define PIN_NUM_DATA4          1 // B4
#define PIN_NUM_DATA5          5 // G0
#define PIN_NUM_DATA6          6 // G1
#define PIN_NUM_DATA7          7 // G2
#define PIN_NUM_DATA8          15 // G3
#define PIN_NUM_DATA9          16 // G4
#define PIN_NUM_DATA10         4 // G5
#define PIN_NUM_DATA11         45  // R0
#define PIN_NUM_DATA12         48 // R1
#define PIN_NUM_DATA13         47 // R2
#define PIN_NUM_DATA14         21 // R3
#define PIN_NUM_DATA15         14 // R4
#define PIN_NUM_DISP_EN        -1

#define LCD_H_RES 480
#define LCD_V_RES 272

lv_disp_t* LCDInit(void);
int32_t DrawBitmap(int left, int top, int right, int bottom, const void* data);
int32_t RefreshScreen();
