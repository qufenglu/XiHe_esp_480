#pragma once
#include <inttypes.h>
#include <thread>
#include <mutex>
#include <functional>
#include <string>
#include <set>
#include "esp_wifi.h"

class WIFiAdapter
{
public:
    WIFiAdapter();
    ~WIFiAdapter();
    static int32_t InitWifiAsSoftAP(const std::string& ssid, const std::string& password);
    static int32_t InitWifiAsSTA();
    static int32_t ConnectToAP(const std::string& ssid, const std::string& password);
    static int32_t CloseWifi();
    static int32_t StartAutoConnect();
    static int32_t StopAutoConnect();
    static inline wifi_mode_t GetWifiMode() { return m_eWifiMode; };
    static inline bool IsWifiConnected() { return m_bWifiConnected; };
    static inline bool IsGotRemoteIp() { return m_bGotRemoteIp; };
    static inline esp_ip4_addr_t GetLocalIp() { return m_LocalIp; };
    static inline esp_ip4_addr_t GetRemoteIp() { return m_RemoteIp; };

private:
    static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
    static int32_t UpdataRemoteIp();
    static int32_t AutoConnectThread();

private:
    static wifi_mode_t m_eWifiMode;
    static bool m_bWifiConnected;
    static bool m_bGotRemoteIp;
    static esp_ip4_addr_t m_LocalIp;
    static esp_ip4_addr_t m_RemoteIp;
    static std::string  m_cStrSsid;
    static std::string  m_cStrPassword;
    static bool m_bStopAutoConnect;
    static std::thread* m_pAutoConnectThread;
};
