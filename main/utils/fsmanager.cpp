#include "fsmanager.h"

fsmanager::fsmanager() {
    conf = (esp_vfs_littlefs_conf_t *)heap_caps_malloc(sizeof(esp_vfs_littlefs_conf_t), MALLOC_CAP_DMA);
    conf->base_path = "/setting";
    conf->partition_label = "setting";
    conf->format_if_mount_failed = true;
    conf->dont_mount = false;
    esp_err_t ret = esp_vfs_littlefs_register(conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find LittleFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize LittleFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_littlefs_info(conf->partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get LittleFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    // cJSON *json = read_json("/setting/wifi.json");
    // if (json != NULL) {
    //     auto wifi_ssiditem = cJSON_GetObjectItem(json, "wifi_ssid");
    //     auto str = cJSON_GetStringValue(wifi_ssiditem);
    //     ESP_LOGI(TAG, "json setting %s", str);
    // }
}

fsmanager::~fsmanager() {
    esp_vfs_littlefs_unregister(conf->partition_label);
}

void fsmanager::save_config(COMMON::global_config config, char *path) {
    FILE *fp = fopen(path, "w+");
    fseek(fp, 0, SEEK_SET);

    auto ptr1 = cJSON_CreateObject();
    cJSON_AddStringToObject(ptr1, "wifi_ssid", config.wifi_ssid);
    auto content = cJSON_Print(ptr1);
    fputs(content, fp);
}

cJSON *fsmanager::read_json(char *path) {
    cJSON *ptr = NULL;
    FILE *fp = fopen(path, "w+");

    char *str = nullptr;
    char buf[read_buf_size];
    fseek(fp, 0, SEEK_SET);
    int size = 0;
    while (true) {
        auto readsize = fread(buf, sizeof(char), read_buf_size, fp);
        size += read_buf_size - 1;
        if (str == nullptr) {
            str = (char *)heap_caps_calloc(read_buf_size, sizeof(char), MALLOC_CAP_SPIRAM);
            mempcpy(str, buf, read_buf_size);
        } else if (readsize < read_buf_size) {
            str = (char *)heap_caps_realloc(str, sizeof(char) * size, MALLOC_CAP_SPIRAM);
            mempcpy(str + size - read_buf_size, buf, read_buf_size);
        }
        if (readsize < read_buf_size) {
            str[size - read_buf_size + readsize + 1] = '\0';
            break;
        }
    }

    // cJSON_Delete(ptr1);
    // cJSON_free(content);

    ESP_LOGI(TAG, "json setting %s", str);
    if (str[0] == '\0') {
        goto final;
    }

    ptr = cJSON_Parse(str);
    goto final;

final:
    fclose(fp);
    free(str);
    return ptr;
}