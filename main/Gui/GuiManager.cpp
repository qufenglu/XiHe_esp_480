#include <thread>
#include "GuiManager.h"
#include "esp_log.h"
#include "jpeg_decoder.h"
#include "esp_heap_caps.h"

extern "C"
{
#include "lcd.h"
}

static const char* TAG = "GuiManager";
#define IMAGE_WIDTH 480
#define IMAGE_HIGHT 272
#define IMAGE_BUFF_SIZE (IMAGE_WIDTH*IMAGE_HIGHT*2)

GuiManager::GuiManager()
{
    m_pBtnConnetEventDsc = nullptr;
    m_pBtnExitEventDsc = nullptr;
    m_bHasInit = false;
    m_pBtnConnetClickedCallback = nullptr;
    m_pBtnExitPlayClickedCallback = nullptr;
    m_pRenderRGBBuff = nullptr;
}

GuiManager::~GuiManager()
{
    UnitUI();
}

static void ta_event_cb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* ta = lv_event_get_target(e);
    lv_obj_t* kb = (lv_obj_t*)lv_event_get_user_data(e);
    if (code == LV_EVENT_FOCUSED)
    {
        if (lv_indev_get_type(lv_indev_get_act()) != LV_INDEV_TYPE_KEYPAD)
        {
            lv_keyboard_set_textarea(kb, ta);
            lv_obj_set_style_max_height(kb, LV_HOR_RES / 6, 0);
            lv_obj_set_style_max_width(kb, LV_VER_RES / 4 * 3, 0);
            lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
            lv_obj_scroll_to_view_recursive(ta, LV_ANIM_OFF);
        }
    }
    else if (code == LV_EVENT_DEFOCUSED)
    {
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_indev_reset(nullptr, ta);
    }
    else if (code == LV_EVENT_READY || code == LV_EVENT_CANCEL)
    {
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_state(ta, LV_STATE_FOCUSED);
        lv_indev_reset(nullptr, ta);
    }
}

int32_t GuiManager::InitUI()
{
    ESP_LOGI(TAG, "InitUI");
    if (m_bHasInit)
    {
        ESP_LOGI(TAG, "UI already initialized");
        return 0;
    }

    setup_ui(&m_sLvUi);
    custom_init(&m_sLvUi);

    m_pBtnConnetEventDsc = lv_obj_add_event_cb(m_sLvUi.LoginWin_btn_connet, &GuiManager::OnBtnConnectEvent, LV_EVENT_ALL, this);
    m_pBtnExitEventDsc = lv_obj_add_event_cb(m_sLvUi.PlayWin_btn_exit, &GuiManager::OnBtnExitPlayEvent, LV_EVENT_ALL, this);
    lv_obj_set_scrollbar_mode(m_sLvUi.LoginWin_ta_ip, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scrollbar_mode(m_sLvUi.LoginWin_ta_port, LV_SCROLLBAR_MODE_OFF);

    lv_obj_t* kb = lv_keyboard_create(m_sLvUi.LoginWin);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(m_sLvUi.LoginWin_ta_ip, ta_event_cb, LV_EVENT_ALL, kb);
    lv_obj_add_event_cb(m_sLvUi.LoginWin_ta_port, ta_event_cb, LV_EVENT_ALL, kb);
    lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_NUMBER);

    m_pRenderRGBBuff = (uint8_t*)heap_caps_malloc(IMAGE_BUFF_SIZE, MALLOC_CAP_SPIRAM);
    if (m_pRenderRGBBuff == nullptr)
    {
        ESP_LOGE(TAG, "malloc RGBBuff fail,w:%d h:%d", IMAGE_WIDTH, IMAGE_HIGHT);
        return -1;
    }

    m_bHasInit = true;
    return 0;
}

int32_t GuiManager::UnitUI()
{
    ESP_LOGI(TAG, "UnitUI");
    lv_obj_remove_event_dsc(m_sLvUi.LoginWin_btn_connet, m_pBtnConnetEventDsc);
    lv_obj_remove_event_dsc(m_sLvUi.PlayWin_btn_exit, m_pBtnExitEventDsc);
    return 0;
}

int32_t GuiManager::ShowLoginWin()
{
    ESP_LOGI(TAG, "ShowLoginWin");
    lv_scr_load(m_sLvUi.LoginWin);
    return 0;
}

int32_t GuiManager::ShowPlayWin()
{
    ESP_LOGI(TAG, "ShowPlayWin");
    lv_scr_load(m_sLvUi.PlayWin);
    return 0;
}

void GuiManager::OnBtnConnectEvent(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    GuiManager* pGuiManager = (GuiManager*)e->user_data;
    lv_ui* ui = &pGuiManager->m_sLvUi;
    switch (code)
    {
    case LV_EVENT_CLICKED:
        ESP_LOGI(TAG, "Button connect clicked");
        if (pGuiManager->m_pBtnConnetClickedCallback != nullptr)
        {
            pGuiManager->m_pBtnConnetClickedCallback(nullptr);
        }
        break;
    default:
        break;
    }
}

const char* GuiManager::GetConnectIp()
{
    const char* ip = lv_textarea_get_text(m_sLvUi.LoginWin_ta_ip);
    ESP_LOGI(TAG, "connect ip:%s", ip);
    return ip;
}

const char* GuiManager::GetConnectPort()
{
    const char* port = lv_textarea_get_text(m_sLvUi.LoginWin_ta_port);
    ESP_LOGI(TAG, "connect port:%s", port);
    return port;
}

void GuiManager::OnBtnExitPlayEvent(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    GuiManager* pGuiManager = (GuiManager*)e->user_data;
    lv_ui* ui = &pGuiManager->m_sLvUi;
    switch (code)
    {
    case LV_EVENT_CLICKED:
        ESP_LOGI(TAG, "Button exit play clicked");
        if (pGuiManager->m_pBtnExitPlayClickedCallback != nullptr)
        {
            pGuiManager->m_pBtnExitPlayClickedCallback(nullptr);
        }
        break;
    default:
        break;
    }
}

void GuiManager::SetBtnConnetClickedCallback(const OnButtonClicked& callback)
{
    m_pBtnConnetClickedCallback = callback;
}

void GuiManager::SetBtnExitPlayClickedCallback(const OnButtonClicked& callback)
{
    m_pBtnExitPlayClickedCallback = callback;
}

lv_img_dsc_t img = { 0 };
void GuiManager::RenderVideoFrame(std::shared_ptr<MediaPacket>& frame)
{
    esp_jpeg_image_cfg_t cfg;
    cfg.indata = frame->m_pData;
    cfg.indata_size = frame->m_nLength;
    cfg.outbuf = m_pRenderRGBBuff;   
    cfg.outbuf_size = IMAGE_BUFF_SIZE;
    cfg.out_format = JPEG_IMAGE_FORMAT_RGB565;
    cfg.out_scale = JPEG_IMAGE_SCALE_0;
    cfg.flags.swap_color_bytes = 0;

    esp_jpeg_image_output_t outimg;
    esp_err_t err = esp_jpeg_decode(&cfg, &outimg);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "RenderVideoFrame size:%d esp_jpeg_decode return:%d", frame->m_nLength, err);
        return;
    }

    //LVGL刷新太慢，直接调用LCD渲染
    /*img.header.cf = LV_IMG_CF_TRUE_COLOR;
    img.header.always_zero = 0;
    img.header.reserved = 0; 
    img.header.w = outimg.width;
    img.header.h = outimg.height;
    img.data_size = outimg.width * outimg.height * 2;
    img.data = m_pRenderRGBBuff;
    lv_img_set_src(m_sLvUi.PlayWin_img_play, &img);*/

    DrawBitmap(0, 0, outimg.width, outimg.height, m_pRenderRGBBuff);
}
