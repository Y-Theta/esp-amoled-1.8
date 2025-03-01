#include "app.h"

mmap_assets_handle_t myapp::mmap_drive_handle = NULL;
// esp_lv_decoder_handle_t myapp::decoder_handle = NULL;

myapp::myapp() {
    if (mmap_drive_handle == NULL) {
        init_mmapfile();
    }
}

myapp::~myapp() {
    release_mmapfile();
    // esp_lv_decoder_deinit(decoder_handle);
}

void myapp::update_battery_status(powermanager *manager) {
    if (battery_label != NULL) {
        char post[6];
        auto p = manager->get_percent();
        auto ischarge = manager->is_charging();
        snprintf(post, sizeof(post), "%d %%", p);
        lv_label_set_text(battery_label, post);
        if (ischarge) {
            lv_obj_set_style_text_color(battery_label, lv_color_hex(0x00ff22), 0);
        } else {
            lv_obj_set_style_text_color(battery_label, lv_color_hex(0xffffff), 0);
        }
    }
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
    lv_obj_set_size(pointer, 30, 30);
    lv_obj_align(pointer, LV_ALIGN_TOP_LEFT, 24, 24);

    auto lvimage = lv_image_create(pointer);
    static lv_img_dsc_t img_wink_png;
    img_wink_png.data_size = mmap_assets_get_size(mmap_drive_handle, image);
    img_wink_png.data = mmap_assets_get_mem(mmap_drive_handle, image);
    lv_image_set_src(lvimage, &img_wink_png);
    lv_obj_align(lvimage, LV_ALIGN_CENTER, 0, 0);

    // lv_obj_add_event_cb(pointer, on_setting_tap, LV_EVENT_PRESSED, NULL);
}

void myapp::init_ui_elements() {
    // esp_lv_decoder_init(&decoder_handle);
    lv_libpng_init();
    auto screen = lv_screen_active();
    // auto uint8_data = mmap_assets_get_mem(mmap_drive_handle, MMAP_RESOURCES_DATA_JSON);
    // auto uint8_length = mmap_assets_get_size(mmap_drive_handle, MMAP_RESOURCES_DATA_JSON);
    // char *utf8_str = new char[uint8_length + 1];
    // memcpy(utf8_str, uint8_data, uint8_length);
    // utf8_str[uint8_length] = '\0';
    // lottie_ani = lv_rlottie_create_from_raw(screen, 160, 160, utf8_str);
    // lv_obj_center(lottie_ani);
    // delete[] utf8_str;

    create_image_btn(setting_image, screen, this, MMAP_RESOURCES_SETTING_PNG, on_setting_tap);

    battery_label = lv_label_create(screen);
    static lv_style_t style_bat;
    lv_style_init(&style_bat);
    lv_style_set_text_font(&style_bat, &lv_font_montserrat_14);
    lv_style_set_text_color(&style_bat, lv_color_hex(0xffffff));
    lv_obj_add_style(battery_label, &style_bat, 0);
    lv_label_set_text(battery_label, "BAT");
    lv_obj_align(battery_label, LV_ALIGN_TOP_RIGHT, -20, 20);

    lv_obj_set_style_bg_color(screen, LV_COLOR_MAKE(0, 0, 0), LV_STATE_DEFAULT);
}

void myapp::pause_ani() {
    // lv_rlottie_set_play_mode(lottie_ani, LV_RLOTTIE_CTRL_PAUSE);
}

void myapp::resume_ani() {
    // const lv_rlottie_ctrl_t item = (lv_rlottie_ctrl_t)(8);
    // lv_rlottie_set_play_mode(lottie_ani, item);
}