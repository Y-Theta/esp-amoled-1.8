#include "wifimanager.h"

wifimanager::wifimanager() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(esp_netif_init());
}

wifimanager::~wifimanager() {
}

static void get_scan_results(COMMON::wifi_scan_result *result) {
    uint16_t ap_num = 0;
    esp_wifi_scan_get_ap_num(&ap_num);
    if (ap_num > WIFI_SCAN_LIST_SIZE) {
        ap_num = WIFI_SCAN_LIST_SIZE;
    }

    result->count = ap_num;
    result->infos = (wifi_ap_record_t *)heap_caps_malloc(ap_num * sizeof(wifi_ap_record_t), MALLOC_CAP_SPIRAM);
    esp_wifi_scan_get_ap_records(&ap_num, result->infos);

    std::sort(result->infos, result->infos + ap_num, [](const wifi_ap_record_t &a, const wifi_ap_record_t &b) {
        return a.rssi > b.rssi;
    });
}

static void wifi_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    auto *this_ = static_cast<wifimanager *>(arg);
    if (event_id == WIFI_EVENT_SCAN_DONE) {
        COMMON::wifi_scan_result *result = (COMMON::wifi_scan_result *)heap_caps_malloc(sizeof(COMMON::wifi_scan_result), MALLOC_CAP_SPIRAM);
        get_scan_results(result);
        this_->scan_result = result;
        if (this_->on_wifi_scaned) {
            this_->on_wifi_scaned(this_, this_->userctx);
        }
        free(result->infos);
        free(result);
        this_->scan_result = nullptr;
    }
}

void wifimanager::save_to_ssid_list(){
     nvs_handle_t nvs_handle;
    ESP_ERROR_CHECK(nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle));
    for (int i = 0; i < 10; i++) {
        char *ssid_key = (char *)heap_caps_malloc(sizeof(char) * 6, MALLOC_CAP_SPIRAM);
        if (i > 0) {
            snprintf(ssid_key, 6, "ssid%d\0", i);
        }
        char *password_key = (char *)heap_caps_malloc(sizeof(char) * 5, MALLOC_CAP_SPIRAM);
        if (i > 0) {
            snprintf(password_key, 10, "password%d\0", i);
        }
        
        if (i < ssid_list_.size()) {
            nvs_set_str(nvs_handle, ssid_key, ssid_list_[i].ssid);
            nvs_set_str(nvs_handle, password_key, ssid_list_[i].password);
        } else {
            nvs_erase_key(nvs_handle, ssid_key);
            nvs_erase_key(nvs_handle, password_key);
        }
    }
    nvs_commit(nvs_handle);
    nvs_close(nvs_handle);
}

void wifimanager::get_saved_ssid_list() {
    ssid_list_.clear();

    // Load ssid and password from NVS from namespace "wifi"
    // ssid, ssid1, ssid2, ... ssid9
    // password, password1, password2, ... password9
    nvs_handle_t nvs_handle;
    auto ret = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (ret != ESP_OK) {
        // The namespace doesn't exist, just return
        ESP_LOGW(TAG, "NVS namespace %s doesn't exist", NVS_NAMESPACE);
        return;
    }
    for (int i = 0; i < 10; i++) {
        char *ssid_key = (char *)heap_caps_malloc(sizeof(char) * 6, MALLOC_CAP_SPIRAM);
        if (i > 0) {
            snprintf(ssid_key, 6, "ssid%d\0", i);
        }
        char *password_key = (char *)heap_caps_malloc(sizeof(char) * 5, MALLOC_CAP_SPIRAM);
        if (i > 0) {
            snprintf(password_key, 10, "password%d\0", i);
        }

        char ssid[33];
        char password[65];
        size_t length = sizeof(ssid);
        if (nvs_get_str(nvs_handle, ssid_key, ssid, &length) != ESP_OK) {
            continue;
        }
        length = sizeof(password);
        if (nvs_get_str(nvs_handle, password_key, password, &length) != ESP_OK) {
            continue;
        }
        ssid_list_.push_back({ssid, password});
    }
    nvs_close(nvs_handle);
}

void wifimanager::scan() {
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        wifi_handler,
                                                        this,
                                                        &instance_any_id_));

    auto handle_t = esp_netif_create_default_wifi_sta();
    // Initialize the WiFi stack in station mode
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    cfg.nvs_enable = false;
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    esp_wifi_scan_start(nullptr, true);

    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_ERROR_CHECK(esp_wifi_deinit());

    // 取消注册事件处理程序
    if (instance_any_id_ != nullptr) {
        ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id_));
        instance_any_id_ = nullptr;
    }

    esp_wifi_clear_default_wifi_driver_and_handlers(handle_t);
    esp_netif_destroy(handle_t);
    ESP_ERROR_CHECK(esp_event_loop_delete_default());
}

COMMON::wifi_connect_result wifimanager::connect() {

    return COMMON::connected;
}