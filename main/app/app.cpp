#include "app.h"

mmap_assets_handle_t myapp::mmap_drive_handle = NULL;
esp_lv_decoder_handle_t myapp::decoder_handle = NULL;
std::map<MMAP_RESOURCES_LISTS, COMMON::assets_info_t> myapp::assets_map;

static void on_wifi_scaned(wifimanager *manager, void *userctx) {
    auto *app = static_cast<myapp *>(userctx);
    if (manager->scan_result && (manager->scan_result->count > 0)) {
        if (utils::GetInstance().lvgl_lock(-1)) {
            app->update_wifi_aps();
            utils::GetInstance().lvgl_unlock();
        }
    };
}

void myapp::init() {
    device_manager->init_i2c();
    power_manager->init();
    device_manager->init_io_expander();
    device_manager->init_screen();
    device_manager->set_brightness(128);
    device_manager->init_touch();
    power_manager->start_power_monitor();

    wifi_manager->userctx = this;
    wifi_manager->on_wifi_scaned = on_wifi_scaned;

    init_btn();
    init_framework();
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

lv_obj_t *myapp::create_image_btn(myapp *app, lv_obj_t *screen, MMAP_RESOURCES_LISTS image, int32_t btn_size, lv_event_cb_t cb) {

    lv_obj_t *pointer = lv_btn_create(screen);
    lv_obj_add_event_cb(pointer, cb, LV_EVENT_CLICKED, app);
    lv_obj_set_style_bg_opa(pointer, LV_OPA_0, 0);
    lv_obj_set_style_shadow_width(pointer, 0, 0);
    lv_obj_set_style_bg_opa(pointer, LV_OPA_0, LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(pointer, 0, LV_STATE_PRESSED);
    lv_obj_set_size(pointer, btn_size, btn_size);
    lv_obj_set_flex_grow(pointer, 0);

    auto lvimage = lv_img_create(pointer);
    auto assets_info = myapp::get_mmap_assets(image);
    lv_img_dsc_t *img_wink_png = (lv_img_dsc_t *)heap_caps_malloc(sizeof(lv_img_dsc_t), MALLOC_CAP_SPIRAM);
    img_wink_png->data_size = assets_info->size;
    img_wink_png->data = assets_info->buf;
    lv_img_set_src(lvimage, img_wink_png);
    lv_obj_align(lvimage, LV_ALIGN_CENTER, 0, 0);

    return pointer;
}

lv_obj_t *myapp::init_layout() {
    auto screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, LV_COLOR_MAKE(0, 0, 0), LV_STATE_DEFAULT);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    // lv_obj_set_layout(screen, LV_LAYOUT_FLEX);
    // lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    // lv_obj_set_flex_align(screen, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

    static lv_style_t header_bar_style;
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
    // lv_obj_set_flex_grow(header_bar, 0);
    lv_obj_set_flex_flow(header_bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header_bar, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_align(header_bar, LV_ALIGN_TOP_LEFT, 0, 0);

    static lv_style_t body_style;
    lv_style_init(&body_style);
    lv_style_set_bg_opa(&body_style, LV_OPA_0);

    lottie_area = lv_obj_create(screen);
    lv_obj_remove_style_all(lottie_area);
    lv_obj_add_style(lottie_area, &body_style, 0);
    lv_obj_clear_flag(lottie_area, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(lottie_area, LAYOUT_LOTTIE_AREA_W, LAYOUT_LOTTIE_AREA_H);
    lv_obj_align(lottie_area, LV_ALIGN_CENTER, 0, -((SCREEN_V_RES / 2) - (LAYOUT_LOTTIE_AREA_H / 2) - LAYOUT_CTL_PANEL_HEIGHT));

    static lv_style_t console_prompt_style;
    lv_style_init(&console_prompt_style);
    lv_style_set_bg_opa(&console_prompt_style, LV_OPA_0);

    console_prompt = lv_obj_create(screen);
    lv_obj_remove_style_all(console_prompt);
    lv_obj_add_style(console_prompt, &console_prompt_style, 0);
    lv_obj_clear_flag(console_prompt, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(console_prompt, LAYOUT_TEXT_AREA_W, LAYOUT_TEXT_AREA_H);
    lv_obj_align(console_prompt, LV_ALIGN_CENTER, 0, -((SCREEN_V_RES / 2) - LAYOUT_LOTTIE_AREA_H - LAYOUT_CTL_PANEL_HEIGHT));

    static lv_style_t status_bar_style;
    lv_style_init(&status_bar_style);
    lv_style_set_bg_opa(&status_bar_style, LV_OPA_0);

    status_bar = lv_obj_create(screen);
    lv_obj_remove_style_all(status_bar);
    lv_obj_add_style(status_bar, &status_bar_style, 0);
    lv_obj_clear_flag(status_bar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(status_bar, SCREEN_H_RES, LAYOUT_STATUS_HEIGHT);
    lv_obj_set_style_pad_bottom(status_bar, LAYOUT_STATUS_PAD_BOTTOM, 0);
    lv_obj_set_style_pad_left(status_bar, LAYOUT_STATUS_PAD_LEFT, 0);
    lv_obj_set_style_pad_right(status_bar, LAYOUT_STATUS_PAD_RIGHT, 0);
    lv_obj_set_layout(status_bar, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(status_bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(status_bar, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_align(status_bar, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    return screen;
}

void myapp::init_ui_elements() {

    auto screen = init_layout();

    // 创建状态栏
    create_header_bar(header_bar);
    create_status_bar(status_bar);
}

void myapp::pause_ani() {
    ESP_LOGI(TAG, "\nfree heap: %d,\nfree internal: %d ", esp_get_free_heap_size(), esp_get_free_internal_heap_size());
    // lv_rlottie_set_play_mode(lottie_ani, LV_RLOTTIE_CTRL_PAUSE);
}

void myapp::resume_ani() {
    ESP_LOGI(TAG, "\nfree heap: %d,\nfree internal: %d ", esp_get_free_heap_size(), esp_get_free_internal_heap_size());
    // auto lottie_ = lv_lottie_get_anim(lottie_ani);
    // uint32_t a, b;
    // lv_lottie_get_segment(lottie_ani, &a, &b);
    // ESP_LOGI(TAG, "%d  %d", b - a, lottie_->current_value);
    // const lv_rlottie_ctrl_t item = (lv_rlottie_ctrl_t)(8);
    // lv_rlottie_set_play_mode(lottie_ani, item);
}