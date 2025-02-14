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

void wifimanager::save_to_ssid_list() {
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
    esp_wifi_scan_start(nullptr, false);
}

void wifimanager::enter_scan_mode() {
    if (is_scna_mode)
        return;
    esp_event_loop_create_default();
    esp_event_handler_instance_register(WIFI_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        wifi_handler,
                                        this,
                                        &instance_any_id_);
    _handle_t = esp_netif_create_default_wifi_sta();
    // Initialize the WiFi stack in station mode
    // _wifi_sta_config = (wifi_init_config_t *)heap_caps_malloc(sizeof(wifi_init_config_t), MALLOC_CAP_SPIRAM);
    // _wifi_sta_config->osi_funcs = &g_wifi_osi_funcs;
    // _wifi_sta_config->wpa_crypto_funcs = g_wifi_default_wpa_crypto_funcs;
    // _wifi_sta_config->static_rx_buf_num = CONFIG_ESP_WIFI_STATIC_RX_BUFFER_NUM;
    // _wifi_sta_config->dynamic_rx_buf_num = CONFIG_ESP_WIFI_DYNAMIC_RX_BUFFER_NUM;
    // _wifi_sta_config->tx_buf_type = CONFIG_ESP_WIFI_TX_BUFFER_TYPE;
    // _wifi_sta_config->static_tx_buf_num = WIFI_STATIC_TX_BUFFER_NUM;
    // _wifi_sta_config->dynamic_tx_buf_num = WIFI_DYNAMIC_TX_BUFFER_NUM;
    // _wifi_sta_config->rx_mgmt_buf_type = CONFIG_ESP_WIFI_DYNAMIC_RX_MGMT_BUF;
    // _wifi_sta_config->rx_mgmt_buf_num = WIFI_RX_MGMT_BUF_NUM_DEF;
    // _wifi_sta_config->cache_tx_buf_num = WIFI_CACHE_TX_BUFFER_NUM;
    // _wifi_sta_config->csi_enable = WIFI_CSI_ENABLED;
    // _wifi_sta_config->ampdu_rx_enable = WIFI_AMPDU_RX_ENABLED;
    // _wifi_sta_config->ampdu_tx_enable = WIFI_AMPDU_TX_ENABLED;
    // _wifi_sta_config->amsdu_tx_enable = WIFI_AMSDU_TX_ENABLED;
    // _wifi_sta_config->nvs_enable = WIFI_NVS_ENABLED;
    // _wifi_sta_config->nano_enable = WIFI_NANO_FORMAT_ENABLED;
    // _wifi_sta_config->rx_ba_win = WIFI_DEFAULT_RX_BA_WIN;
    // _wifi_sta_config->wifi_task_core_id = WIFI_TASK_CORE_ID;
    // _wifi_sta_config->beacon_max_len = WIFI_SOFTAP_BEACON_MAX_LEN;
    // _wifi_sta_config->mgmt_sbuf_num = WIFI_MGMT_SBUF_NUM;
    // _wifi_sta_config->feature_caps = WIFI_FEATURE_CAPS;
    // _wifi_sta_config->sta_disconnected_pm = WIFI_STA_DISCONNECTED_PM_ENABLED;
    // _wifi_sta_config->espnow_max_encrypt_num = CONFIG_ESP_WIFI_ESPNOW_MAX_ENCRYPT_NUM;
    // _wifi_sta_config->tx_hetb_queue_num = WIFI_TX_HETB_QUEUE_NUM;
    // _wifi_sta_config->dump_hesigb_enable = WIFI_DUMP_HESIGB_ENABLED;
    // _wifi_sta_config->magic = WIFI_INIT_CONFIG_MAGIC;
    // _wifi_sta_config->nvs_enable = false;
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    cfg.nvs_enable = false;
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
    is_scna_mode = true;
}

void wifimanager::exit_scan_mode() {
    if (!is_scna_mode)
        return;
    esp_wifi_stop();
    esp_wifi_deinit();

    // 取消注册事件处理程序
    if (instance_any_id_ != nullptr) {
        esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id_);
        instance_any_id_ = nullptr;
    }
    if (_handle_t != nullptr) {
        esp_wifi_clear_default_wifi_driver_and_handlers(_handle_t);
        esp_netif_destroy(_handle_t);
        _handle_t = nullptr;
    }
    if (_wifi_sta_config != nullptr) {
        free(_wifi_sta_config);
        _wifi_sta_config = nullptr;
    }
    esp_event_loop_delete_default();
    is_scna_mode = false;
}

COMMON::wifi_connect_result wifimanager::connect() {

    return COMMON::connected;
}