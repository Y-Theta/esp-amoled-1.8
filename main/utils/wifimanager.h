#ifndef USE_COMMON
#include "common.h"
#endif

#ifndef USE_WIFI
#define USE_WIFI

#define NVS_NAMESPACE "wifi"

class wifimanager {
private:
    esp_event_handler_instance_t instance_any_id_ = nullptr;
    std::vector<COMMON::SsidItem> ssid_list_;

    bool is_scna_mode = false;
    esp_netif_t* _handle_t = nullptr;
    wifi_init_config_t* _wifi_sta_config = nullptr;

public:
    wifimanager();
    ~wifimanager();

    void *userctx;
    COMMON::wifi_scan_result *scan_result;
    std::function<void(wifimanager *manager, void *userctx)> on_wifi_scaned;
    std::function<void(wifimanager *manager, void *userctx)> on_wifi_connected;
    std::function<void(wifimanager *manager, void *userctx)> on_wifi_disconnected;

    void enter_scan_mode();
    void exit_scan_mode();

    void scan();
    void get_saved_ssid_list();
    void save_to_ssid_list();

    COMMON::wifi_connect_result connect();
};

#endif
