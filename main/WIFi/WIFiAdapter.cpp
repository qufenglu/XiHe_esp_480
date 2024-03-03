#include <cstdlib>
#include <cstring>
#include <lwip/netdb.h>
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "WIFiAdapter.h"
#include "Log.h"

wifi_mode_t WIFiAdapter::m_eWifiMode = WIFI_MODE_NULL;
bool WIFiAdapter::m_bWifiConnected = false;
bool WIFiAdapter::m_bGotRemoteIp = false;
esp_ip4_addr_t WIFiAdapter::m_LocalIp = { 0 };
esp_ip4_addr_t WIFiAdapter::m_RemoteIp = { 0 };
std::string WIFiAdapter::m_cStrSsid = "";
std::string  WIFiAdapter::m_cStrPassword = "";
bool WIFiAdapter::m_bStopAutoConnect = true;
std::thread* WIFiAdapter::m_pAutoConnectThread = nullptr;

WIFiAdapter::WIFiAdapter()
{
}

WIFiAdapter::~WIFiAdapter()
{
}

int32_t WIFiAdapter::UpdataRemoteIp()
{
    Trace("[WIFiAdapter::UpdataRemoteIp] Enter UpdataRemoteIp()");

    wifi_sta_list_t wifiStaList;
    esp_err_t err = esp_wifi_ap_get_sta_list(&wifiStaList);
    if (err != ESP_OK)
    {
        Error("[WIFiAdapter::UpDataRemoteIp] Wifi get sta list fail,return:%d", err);
        return -1;
    }

    tcpip_adapter_sta_list_t tcpipStaList;
    err = tcpip_adapter_get_sta_list(&wifiStaList, &tcpipStaList);
    if (err != ESP_OK)
    {
        Error("[WIFiAdapter::UpDataRemoteIp] Adapter get sta fail,return:%d", err);
        return -2;
    }

    if (tcpipStaList.num > 0 && tcpipStaList.sta[0].ip.addr != 0)
    {
        m_RemoteIp = tcpipStaList.sta[0].ip;
        m_bGotRemoteIp = true;
        Trace("[WIFiAdapter::UpDataRemoteIp] Updata remote ip:"IPSTR"", IP2STR(&m_RemoteIp));
    }
    else
    {
        Trace("[WIFiAdapter::UpDataRemoteIp] No sta connected");
    }

    return 0;
}

void WIFiAdapter::wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED)
    {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*)event_data;
        Trace("[WIFiAdapter::WifiEvent] Station "MACSTR" join, AID=%d", MAC2STR(event->mac), event->aid);
        tcpip_adapter_ip_info_t info;
        tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_AP, &info);
        char strip[16];
        strcpy(strip, ip4addr_ntoa(&info.ip));
        esp_netif_str_to_ip4(strip, &m_LocalIp);
        Trace("[WIFiAdapter::WifiEvent] Local ip:%s", strip);
        m_bWifiConnected = true;
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*)event_data;
        Trace("[WIFiAdapter::WifiEvent] Station "MACSTR" leave, AID=%d", MAC2STR(event->mac), event->aid);
        m_RemoteIp = { 0 };
        m_bWifiConnected = false;
        m_bGotRemoteIp = false;
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED)
    {
        Trace("[WIFiAdapter::WifiEvent] STA connected");
        m_bWifiConnected = true;
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (m_bWifiConnected)
        {
            Trace("[WIFiAdapter::WifiEvent] STA disconnected");
        }
        m_RemoteIp = { 0 };
        m_LocalIp = { 0 };
        m_bWifiConnected = false;
        m_bGotRemoteIp = false;
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
        m_LocalIp = event->ip_info.ip;
        m_RemoteIp = event->ip_info.gw;
        m_bGotRemoteIp = true;
        char strip[16];
        esp_ip4addr_ntoa(&m_LocalIp, strip, IP4ADDR_STRLEN_MAX);
        Trace("[WIFiAdapter::WifiEvent] Got local ip:%s", strip);
        esp_ip4addr_ntoa(&m_RemoteIp, strip, IP4ADDR_STRLEN_MAX);
        Trace("[WIFiAdapter::WifiEvent] Got Remote ip:%s", strip);
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_AP_STAIPASSIGNED)
    {
        UpdataRemoteIp();
    }
}

int32_t WIFiAdapter::InitWifiAsSoftAP(const std::string& ssid, const std::string& password)
{
    Trace("[WIFiAdapter::InitWifiAsSoftAP] Enter InitWifiAsSoftAP(ssid:%s pass:%s)", ssid.c_str(), password.c_str());

    if (m_eWifiMode != WIFI_MODE_NULL)
    {
        Error("[WIFiAdapter::InitWifiAsSoftAP] Wifi has init as:%d", m_eWifiMode);
        return -1;
    }

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        nvs_flash_erase();
        err = nvs_flash_init();
    }
    if (err != ESP_OK)
    {
        Error("[WIFiAdapter::InitWifiAsSoftAP] Flash init fali,retunn:%d", err);
        return -2;
    }

    esp_netif_init();
    err = esp_event_loop_create_default();
    if (err != ESP_OK)
    {
        Error("[WIFiAdapter::InitWifiAsSoftAP] Create default even loop fail,retunn:%d", err);
        return -3;
    }

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    err = esp_wifi_init(&cfg);
    if (ESP_OK != err)
    {
        Error("[WIFiAdapter::InitWifiAsSoftAP] Init wifi fail,retunn:%d", err);
        return -4;
    }

    err = esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &WIFiAdapter::wifi_event_handler, NULL);
    if (ESP_OK != err)
    {
        Error("[WIFiAdapter::InitWifiAsSoftAP] Reg wifi event handler fail,retunn:%d", err);
        return -5;
    }
    err = esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &WIFiAdapter::wifi_event_handler, NULL);
    if (ESP_OK != err)
    {
        Error("[WIFiAdapter::InitWifiAsSoftAP] Reg ip event handler fail,retunn:%d", err);
        return -6;
    }

    wifi_config_t config;
    strcpy((char*)config.ap.ssid, ssid.c_str());
    config.ap.ssid_len = strlen(ssid.c_str());
    config.ap.channel = 1;
    strcpy((char*)config.ap.password, password.c_str());
    config.ap.max_connection = 1;
    if (strlen(password.c_str()) != 0)
    {
        config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    }
    else
    {
        config.ap.authmode = WIFI_AUTH_OPEN;
    }

    err = esp_wifi_set_mode(WIFI_MODE_AP);
    if (ESP_OK != err)
    {
        Error("[WIFiAdapter::InitWifiAsSoftAP] Set AP mode fail,retunn:%d", err);
        return -7;
    }
    err = esp_wifi_set_config(WIFI_IF_AP, &config);
    if (ESP_OK != err)
    {
        Error("[WIFiAdapter::InitWifiAsSoftAP] Set config fail,retunn:%d", err);
        return -8;
    }
    err = esp_wifi_start();
    if (ESP_OK != err)
    {
        Error("[WIFiAdapter::InitWifiAsSoftAP] Start wifi fail,retunn:%d", err);
        return -10;
    }

    m_eWifiMode = WIFI_MODE_AP;
    m_cStrSsid = ssid;
    m_cStrPassword = password;

    return 0;
}

int32_t WIFiAdapter::InitWifiAsSTA()
{
    Trace("[WIFiAdapter::InitWifiAsSTA] Enter InitWifiAsSTA()");

    if (m_eWifiMode != WIFI_MODE_NULL)
    {
        Error("[WIFiAdapter::InitWifiAsSTA] Wifi has init as:%d", m_eWifiMode);
        return -1;
    }

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        nvs_flash_erase();
        err = nvs_flash_init();
    }
    if (err != ESP_OK)
    {
        Error("[WIFiAdapter::InitWifiAsSTA] Flash init fali,retunn:%d", err);
        return -2;
    }

    esp_netif_init();
    err = esp_event_loop_create_default();
    if (err != ESP_OK)
    {
        Error("[WIFiAdapter::InitWifiAsSTA] Create default even loop fail,retunn:%d", err);
        return -3;
    }

    esp_netif_create_default_wifi_sta();

    err = esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &WIFiAdapter::wifi_event_handler, NULL);
    if (ESP_OK != err)
    {
        Error("[WIFiAdapter::InitWifiAsSTA] Reg wifi event handler fail,retunn:%d", err);
        return -4;
    }

    err = esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &WIFiAdapter::wifi_event_handler, NULL);
    if (ESP_OK != err)
    {
        Error("[WIFiAdapter::InitWifiAsSTA] Reg ip event handler fail,retunn:%d", err);
        return -5;
    }

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    err = esp_wifi_init(&cfg);
    if (ESP_OK != err)
    {
        Error("[WIFiAdapter::InitWifiAsSTA] Init wifi fail,retunn:%d", err);
        return -6;
    }

    err = esp_wifi_set_mode(WIFI_MODE_STA);
    if (ESP_OK != err)
    {
        Error("[WIFiAdapter::InitWifiAsSoftAP] Set STA mode fail,retunn:%d", err);
        return -7;
    }

    err = esp_wifi_start();
    if (ESP_OK != err)
    {
        Error("[WIFiAdapter::InitWifiAsSoftAP] Start wifi fail,retunn:%d", err);
        return -8;
    }

    m_eWifiMode = WIFI_MODE_STA;

    return 0;
}

int32_t WIFiAdapter::ConnectToAP(const std::string& ssid, const std::string& password)
{
    Trace("[WIFiAdapter::ConnectToAP] Enter ConnectToAP(ssid:%s pass:%s)", ssid.c_str(), password.c_str());

    wifi_config_t config = { 0 };
    strcpy((char*)config.sta.ssid, ssid.c_str());
    strcpy((char*)config.sta.password, password.c_str());
    config.sta.pmf_cfg.capable = true;
    config.sta.pmf_cfg.required = false;

    esp_err_t err = esp_wifi_set_config(WIFI_IF_STA, &config);
    if (err != ESP_OK)
    {
        Error("[WIFiAdapter::ConnectToAP] Set config fail,return:%d", err);
        return -1;
    }

    err = esp_wifi_connect();
    if (ESP_OK != err)
    {
        Error("[WIFiAdapter::ConnectToAP] Connect wifi fail,return:%d", err);
        return -2;
    }
    m_cStrSsid = ssid;
    m_cStrPassword = password;

    return 0;
}

int32_t WIFiAdapter::CloseWifi()
{
    Trace("[WIFiAdapter::CloseWifi] Enter CloseWifi()");

    esp_wifi_stop();
    esp_wifi_deinit();
    m_LocalIp = { 0 };
    m_RemoteIp = { 0 };
    m_bWifiConnected = false;
    m_bGotRemoteIp = false;
    m_eWifiMode = WIFI_MODE_NULL;

    return 0;
}

int32_t WIFiAdapter::StartAutoConnect()
{
    Trace("[WIFiAdapter::StartAutoConnect] StartAutoConnect");
    if (m_pAutoConnectThread != nullptr)
    {
        Error("[WIFiAdapter::StartAutoConnect] Auto connect thread has been activated");
        return -1;
    }

    m_bStopAutoConnect = false;
    m_pAutoConnectThread = new std::thread(&WIFiAdapter::AutoConnectThread);

    return 0;
}

int32_t WIFiAdapter::StopAutoConnect()
{
    Trace("[WIFiAdapter::StopAutoConnect] StopAutoConnect");
    if (m_pAutoConnectThread == nullptr)
    {
        return 0;
    }

    m_bStopAutoConnect = true;
    if (m_pAutoConnectThread->joinable())
    {
        m_pAutoConnectThread->join();
    }
    delete m_pAutoConnectThread;

    return 0;
}

int32_t WIFiAdapter::AutoConnectThread()
{
    Trace("[WIFiAdapter::AutoConnectThread] AutoConnectThread");
    while (!m_bStopAutoConnect)
    {
        for (int i = 0; i < 20; i++)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            if (m_bStopAutoConnect)
            {
                break;
            }
        }

        if (m_eWifiMode == WIFI_MODE_STA)
        {
            if (!m_bWifiConnected && m_cStrSsid != "")
            {
                ConnectToAP(m_cStrSsid, m_cStrPassword);
            }
        }
    }
    return 0;
}