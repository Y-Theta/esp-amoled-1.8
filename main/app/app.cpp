#include "app.h"

mmap_assets_handle_t myapp::mmap_drive_handle = NULL;
esp_lv_decoder_handle_t myapp::decoder_handle = NULL;
std::map<MMAP_RESOURCES_LISTS ,COMMON::assets_info_t> myapp::assets_map;

void myapp::init() {
    device_manager->init_i2c();
    power_manager->init();
    device_manager->init_io_expander();
    device_manager->init_screen();
    device_manager->set_brightness(128);
    device_manager->init_touch();
    power_manager->start_power_monitor();
    init_framework();
    init_btn();
    esp_lv_decoder_init(&decoder_handle);
}

myapp::myapp() {
    if (mmap_drive_handle == NULL) {
        init_mmapfile();
    }

    tvg_engine_init(TVG_ENGINE_SW, 0);

    fs_manager = new fsmanager();
    power_manager = new powermanager();
    wifi_manager = new wifimanager();
    device_manager = new devicemanager();

    auto settingjson = fs_manager->read_json("/setting/wifi.json");
    if (settingjson) {
        auto wifi_ssiditem = cJSON_GetObjectItem(settingjson, "wifi_ssid");
        auto str = cJSON_GetStringValue(wifi_ssiditem);
        config.wifi_ssid = str;

        cJSON_Delete(settingjson);
    }
}

myapp::~myapp() {
    release_mmapfile();
    esp_lv_decoder_deinit(decoder_handle);
    tvg_engine_term(TVG_ENGINE_SW);
}

void myapp::update_battery_status(powermanager *manager) {
    if (battery_label != NULL) {
        char post[6];
        auto p = manager->get_percent();
        auto ischarge = manager->is_charging();
        snprintf(post, sizeof(post), "%d", p);
        lv_label_set_text(battery_label, post);
        if (ischarge) {
            lv_obj_set_style_text_color(battery_label, lv_color_hex(BAT_COLOR_FG_CHARGING), 0);
            lv_obj_set_style_bg_color(battery_bg, lv_color_hex(BAT_COLOR_BG_CHARGING), 0);
        } else {
            lv_obj_set_style_text_color(battery_label, lv_color_hex(BAT_COLOR_FG_NORMAL), 0);
            lv_obj_set_style_bg_color(battery_bg, lv_color_hex(BAT_COLOR_BG_NORMAL), 0);
        }
    }
}

COMMON::assets_info_t *myapp::get_mmap_assets(MMAP_RESOURCES_LISTS assets_name) {
    if (assets_map.contains(assets_name)) {
        auto items = assets_map.find(assets_name);
        return &items->second;
    }

    static COMMON::assets_info_t info;
    info.buf = mmap_assets_get_mem(myapp::mmap_drive_handle, assets_name);
    info.size = mmap_assets_get_size(myapp::mmap_drive_handle, assets_name);
    assets_map.insert(std::make_pair(assets_name, info));

    return &info;
}

static void boot_btn_event_cb(void *arg, void *data) {
    myapp *app = (myapp *)data;
    button_event_t event = iot_button_get_event((button_handle_t)arg);
    if (BUTTON_SINGLE_CLICK == event) {
        ESP_LOGI(TAG, "boot click");
        if (app->boot_click) {
            app->boot_click(app);
        };
    }
}

static void pwr_btn_event_cb(void *arg, void *data) {
    myapp *app = (myapp *)data;
    button_event_t event = iot_button_get_event((button_handle_t)arg);
    if (BUTTON_SINGLE_CLICK == event) {
        ESP_LOGI(TAG, "pwr click");
        if (app->pwr_click) {
            app->pwr_click(app);
        };
    }
}

static uint8_t get_pwr_level(button_driver_t *button_driver) {
    auto app = (myapp *)button_driver->user_data;
    uint32_t pin_levels = 0;
    esp_io_expander_get_level(app->device_manager->io_expander_handle, IO_EXPANDER_PIN_NUM_4, &pin_levels);
    return pin_levels ? 1 : 0;
}

void myapp::init_btn() {
    const button_config_t boot_cfg = {0};
    const button_gpio_config_t boot_gpio_cfg = {
        .gpio_num = PIN_NUM_BOOT,
        .active_level = 0,
    };
    static button_handle_t btn_boot = NULL;
    iot_button_new_gpio_device(&boot_cfg, &boot_gpio_cfg, &btn_boot);
    iot_button_register_cb(btn_boot, BUTTON_SINGLE_CLICK, NULL, boot_btn_event_cb, this);

    static button_handle_t btn_pwr = NULL;
    const button_config_t pwr_cfg = {0};
    COMMON::custom_gpio_obj *custom_btn = (COMMON::custom_gpio_obj *)calloc(1, sizeof(COMMON::custom_gpio_obj));
    custom_btn->active_level = 1;
    custom_btn->gpio_num = IO_EXPANDER_PIN_NUM_4;
    custom_btn->base.get_key_level = get_pwr_level;
    custom_btn->base.del = NULL;
    custom_btn->base.user_data = this;
    iot_button_create(&pwr_cfg, &custom_btn->base, &btn_pwr);
    iot_button_register_cb(btn_pwr, BUTTON_SINGLE_CLICK, NULL, pwr_btn_event_cb, this);
}

esp_err_t myapp::init_mmapfile(void) {
    const mmap_assets_config_t asset_cfg = {
        .partition_label = "resources",
        .max_files = MMAP_RESOURCES_FILES,
        .checksum = MMAP_RESOURCES_CHECKSUM,
        .flags = {.mmap_enable = true}};
    esp_err_t ret = mmap_assets_new(&asset_cfg, &mmap_drive_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize %s", asset_cfg.partition_label);
        return ret;
    }

    return ESP_OK;
}

esp_err_t myapp::release_mmapfile(void) {
    esp_err_t ret = mmap_assets_del(mmap_drive_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to delete ");
    }
    return ESP_OK;
}

lv_obj_t *myapp::init_layout() {
    auto screen = lv_scr_act();
    lv_obj_set_layout(screen, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(screen, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

    lv_style_init(&header_bar_style);
    lv_style_set_bg_opa(&header_bar_style, LV_OPA_0);

    header_bar = lv_obj_create(screen);
    lv_obj_remove_style_all(header_bar);
    lv_obj_add_style(header_bar, &header_bar_style, 0);
    lv_obj_clear_flag(header_bar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(header_bar, SCREEN_H_RES, LAYOUT_CTL_PANEL_HEIGHT);
    lv_obj_set_style_pad_top(header_bar, LAYOUT_HEADER_BAR_PAD_TOP, 0);
    lv_obj_set_style_pad_left(header_bar, LAYOUT_HEADER_BAR_PAD_LEFT, 0);
    lv_obj_set_style_pad_right(header_bar, LAYOUT_HEADER_BAR_PAD_RIGHT, 0);
    lv_obj_set_layout(header_bar, LV_LAYOUT_FLEX);
    lv_obj_set_flex_grow(header_bar, 0);
    lv_obj_set_flex_flow(header_bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header_bar, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    return screen;
}

void myapp::init_ui_elements() {

    auto screen = init_layout();

    // 创建状态栏
    create_header_bar(header_bar);

    lv_obj_set_style_bg_color(screen, LV_COLOR_MAKE(0, 0, 0), LV_STATE_DEFAULT);
}

void myapp::pause_ani() {
    ESP_LOGI(TAG, "free heap: %d,\nfree internal: %d ", esp_get_free_heap_size(), esp_get_free_internal_heap_size());
    // lv_rlottie_set_play_mode(lottie_ani, LV_RLOTTIE_CTRL_PAUSE);
}

void myapp::resume_ani() {
    // auto lottie_ = lv_lottie_get_anim(lottie_ani);
    // uint32_t a, b;
    // lv_lottie_get_segment(lottie_ani, &a, &b);
    // ESP_LOGI(TAG, "%d  %d", b - a, lottie_->current_value);
    // const lv_rlottie_ctrl_t item = (lv_rlottie_ctrl_t)(8);
    // lv_rlottie_set_play_mode(lottie_ani, item);
}