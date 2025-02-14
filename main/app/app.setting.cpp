#include "app.h"

static void on_back_tap(lv_event_t *param) {
    auto app = (myapp *)param->user_data;
    app->close_setting();
}

void myapp::open_setting() {
    wifi_manager->enter_scan_mode();
    setting_window();
    wifi_manager->scan();
}

void myapp::close_setting() {
    if (setting_window_ptr != NULL) {
        lv_obj_del(setting_window_ptr);
        wifi_list_ptr = NULL;
        wifi_manager->exit_scan_mode();
    }
}

void create_wifi_list(myapp *app) {
}

void myapp::update_wifi_aps() {
    auto scanedinfos = wifi_manager->scan_result;
    if (scanedinfos == nullptr || scanedinfos->infos == nullptr)
        return;
    for (size_t i = 0; i < scanedinfos->count; i++) {
        auto info = scanedinfos->infos[i];
        ESP_LOGI(TAG,);
        // auto btn = lv_list_add_btn(wifi_list_ptr, LV_SYMBOL_WIFI, (char *)info.ssid);
    }
}

void myapp::setting_window() {
    setting_window_ptr = lv_win_create(lv_scr_act(), 0);
    lv_obj_clear_flag(setting_window_ptr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(setting_window_ptr, 0, 0);
    lv_obj_set_style_bg_color(setting_window_ptr, lv_color_hex(0x000000), 0);

    auto content = lv_win_get_content(setting_window_ptr);
    lv_obj_clear_flag(content, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(content, lv_color_hex(0x000000), 0);

    auto returnbtn = create_image_btn(this, content, MMAP_RESOURCES_BACK_SPNG, ICON_ACTIVE_H, on_back_tap);
    lv_obj_align(returnbtn, LV_ALIGN_BOTTOM_MID, 0, 0);

    wifi_list_ptr = lv_list_create(content);
    lv_obj_set_style_bg_color(wifi_list_ptr, lv_color_hex(0x000000), 0);
    lv_obj_set_style_opa(wifi_list_ptr, LV_OPA_100, 0);
    lv_obj_set_style_border_width(wifi_list_ptr, 0, 0);
    lv_obj_set_style_pad_all(wifi_list_ptr, 0, 0);
    lv_obj_set_size(wifi_list_ptr, SCREEN_H_RES - 32, SCREEN_V_RES - LAYOUT_STATUS_HEIGHT - 16);
    lv_obj_align(wifi_list_ptr, LV_ALIGN_TOP_MID, 0, 0);

    auto title1 = lv_list_add_text(wifi_list_ptr, "System");
    set_text_style(title1);
    auto wifi_btn = lv_list_add_btn(wifi_list_ptr, LV_SYMBOL_WIFI, "WIFI SETTING");

    auto title2 = lv_list_add_text(wifi_list_ptr, "App");
    set_text_style(title2);
}