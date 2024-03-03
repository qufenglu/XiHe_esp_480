#pragma once
#include <stdint.h>
#include <functional>
#include <memory>
#include "Common.h"
extern "C"
{
#include "gui_guider.h"
#include "custom.h"
}

class GuiManager
{
public:
    typedef std::function<void(void*)> OnButtonClicked;

public:
    GuiManager();
    ~GuiManager();
    int32_t InitUI();
    int32_t UnitUI();
    int32_t ShowLoginWin();
    int32_t ShowPlayWin();
    const char* GetConnectIp();
    const char* GetConnectPort();
    void SetBtnConnetClickedCallback(const OnButtonClicked& callback);
    void SetBtnExitPlayClickedCallback(const OnButtonClicked& callback);
    void RenderVideoFrame(std::shared_ptr<MediaPacket>& frame);

private:
    static void OnBtnConnectEvent(lv_event_t* e);
    static void OnBtnExitPlayEvent(lv_event_t* e);

private:
    lv_ui m_sLvUi;
    _lv_event_dsc_t* m_pBtnConnetEventDsc;
    _lv_event_dsc_t* m_pBtnExitEventDsc;
    bool m_bHasInit;
    OnButtonClicked m_pBtnConnetClickedCallback;
    OnButtonClicked m_pBtnExitPlayClickedCallback;
    uint8_t* m_pRenderRGBBuff;
};
