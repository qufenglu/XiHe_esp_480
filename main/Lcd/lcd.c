#include <pthread.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "driver/spi_master.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "lvgl.h"
#include "lcd.h"

#define TAG "DISPLAY"

static esp_lcd_panel_handle_t  g_hLcdPanel = NULL;
static pthread_mutex_t g_sLcdMutex = PTHREAD_MUTEX_INITIALIZER;

static void lvgl_flush_cb(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* color_map)
{
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)drv->user_data;
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;

    pthread_mutex_lock(&g_sLcdMutex);
    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
    pthread_mutex_unlock(&g_sLcdMutex);

    lv_disp_flush_ready(drv);
}

int32_t DrawBitmap(int left, int top, int right, int bottom, const void* data)
{
    int32_t ret = 0;
    esp_err_t err;
    pthread_mutex_lock(&g_sLcdMutex);
    if (g_hLcdPanel == NULL)
    {
        ESP_LOGI(TAG, "LCD Uninitialized");
        ret = -1; goto end;
    }

    err = esp_lcd_panel_draw_bitmap(g_hLcdPanel, left, top, right, bottom, data);
    if (err != ESP_OK)
    {
        ESP_LOGI(TAG, "draw bitmap fail,return:%d", err);
        ret = -1; goto end;
    }

end:
    pthread_mutex_unlock(&g_sLcdMutex);
    return ret;
}

lv_disp_t* LCDInit(void)
{
    static lv_disp_draw_buf_t disp_buf;
    static lv_disp_drv_t disp_drv;
    gpio_config_t bk_gpio_config = {
    .pin_bit_mask = 1ULL << PIN_NUM_BACK_LIGHT,
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));

    ESP_LOGI(TAG, "Install RGB LCD panel driver");
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_rgb_panel_config_t panel_config = {
          .data_width = 16, // RGB565 in parallel mode, thus 16bit in width
          .psram_trans_align = 64,
          .clk_src = LCD_CLK_SRC_PLL240M,
          .disp_gpio_num = PIN_NUM_DISP_EN,
          .pclk_gpio_num = PIN_NUM_PCLK,
          .vsync_gpio_num = PIN_NUM_VSYNC,
          .hsync_gpio_num = PIN_NUM_HSYNC,
          .de_gpio_num = PIN_NUM_DE,
          .data_gpio_nums = {
              PIN_NUM_DATA0,
              PIN_NUM_DATA1,
              PIN_NUM_DATA2,
              PIN_NUM_DATA3,
              PIN_NUM_DATA4,
              PIN_NUM_DATA5,
              PIN_NUM_DATA6,
              PIN_NUM_DATA7,
              PIN_NUM_DATA8,
              PIN_NUM_DATA9,
              PIN_NUM_DATA10,
              PIN_NUM_DATA11,
              PIN_NUM_DATA12,
              PIN_NUM_DATA13,
              PIN_NUM_DATA14,
              PIN_NUM_DATA15,
          },
          .timings = {
              .pclk_hz = LCD_PIXEL_CLOCK_HZ,
              .h_res = LCD_H_RES,
              .v_res = LCD_V_RES,
              // The following parameters should refer to LCD spec
              .hsync_back_porch = 41,
              .hsync_front_porch = 4,
              .hsync_pulse_width = 2,
              .vsync_back_porch = 10,
              .vsync_front_porch = 4,
              .vsync_pulse_width = 2,
              .flags.pclk_active_neg = true,
          },
          .flags.fb_in_psram = true,
      };
    /*esp_lcd_rgb_panel_config_t panel_config =
    {
    .clk_src = LCD_CLK_SRC_PLL240M,
    .timings = {
        .pclk_hz = LCD_PIXEL_CLOCK_HZ,
        .h_res = LCD_H_RES,
        .v_res = LCD_V_RES,
        .hsync_pulse_width = 4,
        .hsync_back_porch = 8,
        .hsync_front_porch = 8,
        .vsync_pulse_width = 4,
        .vsync_back_porch = 8,
        .vsync_front_porch = 8,
        .flags = {
        .hsync_idle_low = false,
        .vsync_idle_low = false,
        .de_idle_high = false,
        .pclk_active_neg = true,
        .pclk_idle_high = false
        },
    },
    .data_width = 16,
    .sram_trans_align = 0,
    .psram_trans_align = 64,
    .hsync_gpio_num = PIN_NUM_HSYNC,
    .vsync_gpio_num = PIN_NUM_VSYNC,
    .de_gpio_num = PIN_NUM_DE,
    .pclk_gpio_num = PIN_NUM_PCLK,
    .disp_gpio_num = PIN_NUM_DISP_EN,
    .data_gpio_nums = {
        PIN_NUM_DATA0,
        PIN_NUM_DATA1,
        PIN_NUM_DATA2,
        PIN_NUM_DATA3,
        PIN_NUM_DATA4,
        PIN_NUM_DATA5,
        PIN_NUM_DATA6,
        PIN_NUM_DATA7,
        PIN_NUM_DATA8,
        PIN_NUM_DATA9,
        PIN_NUM_DATA10,
        PIN_NUM_DATA11,
        PIN_NUM_DATA12,
        PIN_NUM_DATA13,
        PIN_NUM_DATA14,
        PIN_NUM_DATA15,
    },
    .flags = {
        .disp_active_low = 0,
        .fb_in_psram = true,
    },
    };*/
    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));

    ESP_LOGI(TAG, "Initialize RGB LCD panel");
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();
    void* buf1 = NULL;
    void* buf2 = NULL;
    ESP_LOGI(TAG, "Allocate separate LVGL draw buffers from PSRAM");
    buf1 = heap_caps_malloc(LCD_H_RES * (LCD_V_RES / 4) * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    assert(buf1);
    buf2 = heap_caps_malloc(LCD_H_RES * (LCD_V_RES / 4) * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    assert(buf2);
    // initialize LVGL draw buffers
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, LCD_H_RES * (LCD_V_RES / 4));

    ESP_LOGI(TAG, "Register display driver to LVGL");
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LCD_H_RES;
    disp_drv.ver_res = LCD_V_RES;
    disp_drv.flush_cb = lvgl_flush_cb;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.user_data = panel_handle;
    lv_disp_t* disp = lv_disp_drv_register(&disp_drv);
    g_hLcdPanel = panel_handle;

    gpio_set_level(PIN_NUM_BACK_LIGHT, LCD_BK_LIGHT_ON_LEVEL);

    return disp;
}

int32_t RefreshScreen()
{
    lv_obj_invalidate(lv_scr_act());//к╒фа
    return 0;
}