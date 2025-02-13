#include "app.h"

static void on_back_tap(lv_event_t *param) {
    auto app = (myapp *)param->user_data;
    app->close_setting();
}

void myapp::open_setting() {
    setting_window();
}

void myapp::close_setting() {
    if (setting_window_ptr != NULL) {
        lv_obj_del(setting_window_ptr);
        wifi_list_ptr = NULL;
    }
}

void create_wifi_list(myapp *app) {
}

void myapp::setting_window() {
    setting_window_ptr = lv_win_create(lv_scr_act(), 0);

    // auto header = lv_win_get_header(setting_window_ptr);
    // lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);
    // lv_obj_set_style_bg_color(header, lv_color_hex(0x000000), 0);
    // lv_obj_set_style_bg_opa(header, LV_OPA_100, 0);
    // lv_obj_set_layout(header, LV_LAYOUT_FLEX);
    // lv_obj_set_flex_align(header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    // lv_obj_set_style_pad_top(header, 12, 0);
    // lv_obj_set_style_pad_left(header, 16, 0);

    // auto ret_btn = lv_btn_create(header);
    // lv_obj_remove_style_all(ret_btn);
    // lv_obj_add_style(ret_btn, &style, 0);
    // lv_obj_add_style(ret_btn, &style_pr, LV_STATE_PRESSED);
    // lv_obj_set_size(ret_btn, ICON_ACTIVE_W, ICON_ACTIVE_H);
    // lv_obj_add_event_cb(ret_btn, on_back_tap, LV_EVENT_CLICKED, window);
    // lv_obj_set_flex_grow(ret_btn, 0);

    // auto ret_img = lv_img_create(ret_btn);
    // static lv_img_dsc_t img_wink_png;
    // auto assets_info = myapp::get_mmap_assets(MMAP_RESOURCES_BACK_SPNG);
    // img_wink_png.data_size = assets_info->size;
    // img_wink_png.data = assets_info->buf;
    // lv_img_set_src(ret_img, &img_wink_png);
    // lv_obj_align(ret_img, LV_ALIGN_CENTER, 0, 0);

    // auto title = lv_label_create(header);
    // lv_obj_set_style_text_font(header, &lv_font_montserrat_20, 0);
    // lv_obj_set_style_text_align(header, LV_TEXT_ALIGN_CENTER, 0);
    // lv_obj_set_style_bg_opa(title, LV_OPA_0, 0);
    // lv_obj_set_style_text_color(title, lv_color_hex(WIN_WIFI_LIST_TITLE_FG), 0);
    // lv_label_set_text(title, "SETTINGS");
    // lv_obj_set_flex_grow(title, 1);

    auto content = lv_win_get_content(setting_window_ptr);
    lv_obj_set_style_bg_color(content, lv_color_hex(0x000000), 0);
    lv_obj_set_style_opa(content, LV_OPA_100, 0);
    lv_obj_clear_flag(content, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_get_style_pad_left(content, 0);
    // lv_obj_set_layout(content, LV_LAYOUT_FLEX);
    // lv_obj_set_flex_align(content, LV_FLEX_ALIGN_SPACE_BETWEEN,LV_FLEX_ALIGN_CENTER,LV_FLEX_ALIGN_CENTER);

    auto statusbar = lv_obj_create(content);
    lv_obj_set_style_bg_color(statusbar, lv_color_hex(0x000000), 0);
    lv_obj_set_style_opa(statusbar, LV_OPA_100, 0);
    lv_obj_set_style_border_width(statusbar, 0, 0);
    lv_obj_clear_flag(statusbar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(statusbar, SCREEN_H_RES, LAYOUT_STATUS_HEIGHT);
    lv_obj_align(statusbar, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_set_layout(statusbar, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(statusbar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(statusbar, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    auto returnbtn = create_image_btn(this, statusbar, MMAP_RESOURCES_BACK_SPNG, ICON_ACTIVE_H, on_back_tap);

    // wifi_list_ptr = lv_list_create(content);
    // // lv_obj_set_style_bg_color(wifi_list_ptr, lv_color_hex(0x000000), 0);
    // // lv_obj_set_style_opa(wifi_list_ptr, LV_OPA_100, 0);
    // lv_obj_set_size(wifi_list_ptr, SCREEN_H_RES, SCREEN_V_RES - LAYOUT_STATUS_HEIGHT);
    // lv_obj_align(statusbar, LV_ALIGN_TOP_LEFT, 0, 0);
}