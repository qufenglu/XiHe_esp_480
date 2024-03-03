#include <thread>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "XiheClient.h"
#include "Log.h"
#include "WIFiAdapter.h"
#define LVGL_TICK_PERIOD_MS  2

extern "C"
{
#include "lvgl.h"
#include "lcd.h"
#include "gt911.h"
}

static void increase_lvgl_tick(void* arg)
{
    lv_tick_inc(LVGL_TICK_PERIOD_MS);
}

static int32_t InitPHY();
static int32_t InitLVGL();
static int32_t Init();

extern "C" int app_main(void)
{
    int32_t ret = 0;
    XiheClient* pXiheClient = nullptr;

    ret = Init();
    if (ret != 0)
    {
        Error("InitPHY fail,return %d", ret);
        ret = -1; goto end;
    }

    pXiheClient = new XiheClient();
    ret = pXiheClient->Run();
    if (ret != 0)
    {
        Error("GuiManager InitUI fail,return %d", ret);
        ret = -2; goto end;
    }

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        lv_timer_handler();
    }

end:
    lv_deinit();
    return ret;
}

int32_t InitPHY()
{
    Trace("Init LCD");
    lv_disp_t* dsp = LCDInit();
    if (dsp == nullptr)
    {
        Error("LCDInit fail");
        return -1;
    }

    Trace("Init Touch");
    gt911_init(0);

    Trace("Init WIFI");
    int ret = WIFiAdapter::InitWifiAsSTA();
    if (ret != 0)
    {
        Error("Init wifi as STA fail,return:%d", ret);
        return -2;
    }
    WIFiAdapter::ConnectToAP("XiHe", "12345678");
    WIFiAdapter::StartAutoConnect();

    return 0;
}

int32_t InitLVGL()
{
    Trace("Install LVGL tick timer");
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &increase_lvgl_tick,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t lvgl_tick_timer = nullptr;
    esp_err_t  err = esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer);
    if (err != ESP_OK)
    {
        Error("create timer fail:%s", esp_err_to_name(err));
        return -1;
    }
    err = esp_timer_start_periodic(lvgl_tick_timer, LVGL_TICK_PERIOD_MS * 1000);
    if (err != ESP_OK)
    {
        Error("timer start fail:%s", esp_err_to_name(err));
        return -2;
    }

    return 0;
}

int32_t Init()
{
    int ret = InitPHY();
    if (ret != 0)
    {
        Error("InitPHY fail,return %d", ret);
        ret = -1; goto fail;
    }

    ret = InitLVGL();
    if (ret != 0)
    {
        Error("InitLVGL fail,return %d", ret);
        ret = -2; goto fail;
    }
    return 0;

fail:
    return ret;
}