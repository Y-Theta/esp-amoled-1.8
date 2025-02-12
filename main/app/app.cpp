#include "app.h"

mmap_assets_handle_t myapp::mmap_drive_handle = NULL;
esp_lv_decoder_handle_t myapp::decoder_handle = NULL;

void myapp::init() {
    disp_manager->init_i2c();
    power_manager->init();
    disp_manager->init_io_expander();
    disp_manager->init_screen();
    disp_manager->set_brightness(64);
    disp_manager->init_touch();
    disp_manager->framework_init();
    power_manager->start_power_monitor();
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
    disp_manager = new dispmanager();

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
    esp_io_expander_get_level(app->disp_manager->io_expander_handle, IO_EXPANDER_PIN_NUM_4, &pin_levels);
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
    custom_gpio_obj *custom_btn = (custom_gpio_obj *)calloc(1, sizeof(custom_gpio_obj));
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

static void on_setting_tap(lv_event_t *event) {
    ESP_LOGI(TAG, "btn tap");
}

void myapp::create_image_btn(lv_obj_t *pointer, lv_obj_t *screen, myapp *app, MMAP_RESOURCES_LISTS image, lv_event_cb_t cb) {

    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_bg_opa(&style, LV_OPA_0);
    static lv_style_t style_pr;
    lv_style_init(&style_pr);
    lv_style_set_bg_opa(&style_pr, LV_OPA_0);

    pointer = lv_btn_create(screen);
    lv_obj_add_event_cb(pointer, cb, LV_EVENT_CLICKED, NULL);
    lv_obj_remove_style_all(pointer);
    lv_obj_add_style(pointer, &style, 0);
    lv_obj_add_style(pointer, &style_pr, LV_STATE_PRESSED);
    lv_obj_set_size(pointer, BTN_MAIN_SIZE, BTN_MAIN_SIZE);
    lv_obj_set_flex_grow(pointer, 0);

    auto lvimage = lv_img_create(pointer);
    static lv_img_dsc_t img_wink_png;
    img_wink_png.data_size = mmap_assets_get_size(mmap_drive_handle, image);
    img_wink_png.data = mmap_assets_get_mem(mmap_drive_handle, image);
    lv_img_set_src(lvimage, &img_wink_png);
    lv_obj_align(lvimage, LV_ALIGN_CENTER, 0, 0);

    // lv_obj_add_event_cb(pointer, on_setting_tap, LV_EVENT_PRESSED, NULL);
}

void myapp::create_battery_label(lv_obj_t *baselayout) {
    auto screen = lv_scr_act();

    static lv_style_t style_bat_bg;
    lv_style_init(&style_bat_bg);
    lv_style_set_bg_opa(&style_bat_bg, LV_OPA_100);
    lv_style_set_bg_color(&style_bat_bg, lv_color_hex(BAT_COLOR_BG_NORMAL));
    lv_style_set_radius(&style_bat_bg, BAT_RADIUS);

    battery_bg = lv_obj_create(baselayout);
    lv_obj_remove_style_all(battery_bg);
    lv_obj_add_style(battery_bg, &style_bat_bg, 0);
    lv_obj_align(battery_bg, LV_ALIGN_TOP_RIGHT, -BTN_TOP_PADDING, BAT_PADDING);
    lv_obj_set_size(battery_bg, BAT_WIDTH, BAT_HEIGHT);
    lv_obj_set_flex_grow(battery_bg, 0);

    static lv_style_t style_bat_lab;
    lv_style_init(&style_bat_lab);
    lv_style_set_text_font(&style_bat_lab, &lv_font_montserrat_14);
    lv_style_set_text_color(&style_bat_lab, lv_color_hex(BAT_COLOR_FG_NORMAL));

    battery_label = lv_label_create(battery_bg);
    lv_obj_add_style(battery_label, &style_bat_lab, 0);
    lv_label_set_text(battery_label, "BAT");
    lv_obj_align(battery_label, LV_ALIGN_CENTER, 0, 0);
}

void myapp::create_wifi_label() {
    auto screen = lv_scr_act();

    wifi_icon = lv_img_create(screen);
    static lv_img_dsc_t img_wink_png;
    img_wink_png.data_size = mmap_assets_get_size(mmap_drive_handle, MMAP_RESOURCES_NOWIFI_SPNG);
    img_wink_png.data = mmap_assets_get_mem(mmap_drive_handle, MMAP_RESOURCES_NOWIFI_SPNG);
    lv_img_set_src(wifi_icon, &img_wink_png);
    lv_obj_align(wifi_icon, LV_ALIGN_CENTER, 0, 0);
}

lv_obj_t *myapp::init_layout() {
    auto screen = lv_scr_act();
    lv_obj_set_layout(screen, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(screen, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

    lv_style_init(&panel_style);
    lv_style_set_bg_opa(&panel_style, LV_OPA_0);

    header_bar = lv_obj_create(screen);
    lv_obj_remove_style_all(header_bar);
    lv_obj_add_style(header_bar, &panel_style, 0);
    lv_obj_set_size(header_bar, SCREEN_H_RES, LAYOUT_CTL_PANEL_HEIGHT);
    lv_obj_set_style_pad_top(header_bar, 8, 0);
    lv_obj_set_style_pad_left(header_bar, 12, 0);
    lv_obj_set_style_pad_right(header_bar, 12, 0);
    lv_obj_set_layout(header_bar, LV_LAYOUT_FLEX);
    lv_obj_set_flex_grow(header_bar, 0);
    lv_obj_set_flex_flow(header_bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header_bar, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    return screen;
}

void myapp::init_ui_elements() {

    auto screen = init_layout();

    // const uint8_t *uint8_data = mmap_assets_get_mem(mmap_drive_handle, MMAP_RESOURCES_DATA_JSON);
    // int uint8_length = mmap_assets_get_size(mmap_drive_handle, MMAP_RESOURCES_DATA_JSON);
    // lottie_ani = lv_lottie_create(lv_scr_act());
    // lv_obj_center(lottie_ani);
    // static void *fb = heap_caps_malloc(LOTTIE_SIZE * LOTTIE_SIZE * 4, MALLOC_CAP_SPIRAM);
    // lv_lottie_set_buffer(lottie_ani, LOTTIE_SIZE, LOTTIE_SIZE, fb);
    // lv_lottie_set_src_data(lottie_ani, uint8_data, uint8_length);

    create_image_btn(setting_image, header_bar, this, MMAP_RESOURCES_SETTING_SPNG, on_setting_tap);
    create_battery_label(header_bar);

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