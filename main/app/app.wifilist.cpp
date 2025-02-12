#include "app.h"

static void on_back_tap(lv_event_t *param) {
    auto window = (lv_obj_t *)param->user_data;
    lv_obj_del(window);
}

void myapp::open_wifi_setting() {
    wifi_window();
}

void myapp::wifi_window() {
    auto window = lv_win_create(lv_scr_act(), WIN_WIFI_LIST_HEADER_HEIGHT);

    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_bg_opa(&style, LV_OPA_0);
    static lv_style_t style_pr;
    lv_style_init(&style_pr);
    lv_style_set_bg_opa(&style_pr, LV_OPA_0);
    
    auto header = lv_win_get_header(window);

    lv_obj_set_style_bg_color(header, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(header, LV_OPA_100, 0);
    lv_obj_set_layout(header, LV_LAYOUT_FLEX);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_left(header, 24, 0);

    auto ret_btn = lv_btn_create(header);
    lv_obj_remove_style_all(ret_btn);
    lv_obj_add_style(ret_btn, &style, 0);
    lv_obj_add_style(ret_btn, &style_pr, LV_STATE_PRESSED);
    lv_obj_set_size(ret_btn, BTN_MAIN_SIZE, BTN_MAIN_SIZE);
    lv_obj_add_event_cb(ret_btn, on_back_tap, LV_EVENT_CLICKED, window);
    lv_obj_set_flex_grow(ret_btn, 0);

    auto ret_img = lv_img_create(ret_btn);
    static lv_img_dsc_t img_wink_png;
    auto assets_info = myapp::get_mmap_assets(MMAP_RESOURCES_BACK_SPNG);
    img_wink_png.data_size = assets_info->size;
    img_wink_png.data = assets_info->buf;
    lv_img_set_src(ret_img, &img_wink_png);
    lv_obj_align(ret_img, LV_ALIGN_CENTER, 0, 0);

    auto title = lv_label_create(header);
    lv_obj_set_style_text_font(header, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_align(header, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_bg_opa(title, LV_OPA_0, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(WIN_WIFI_LIST_TITLE_FG), 0);
    lv_label_set_text(title, "WIFI setting");
    lv_obj_set_flex_grow(title, 1);

    auto content = lv_win_get_content(window);
    lv_obj_set_style_radius(content, 16, 0);
    lv_obj_set_style_opa(content, LV_OPA_20, 0);

}