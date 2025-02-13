#include "app.h"


void myapp::set_wifi_status(bool flag) {
    static lv_img_dsc_t img_wink_png;
    COMMON::assets_info_t *assets_info = NULL;
    if (flag) {
        assets_info = myapp::get_mmap_assets(MMAP_RESOURCES_WIFI_SPNG);
    } else {
        assets_info = myapp::get_mmap_assets(MMAP_RESOURCES_WIFI_NO_SPNG);
    }
    img_wink_png.data_size = assets_info->size;
    img_wink_png.data = assets_info->buf;

    lv_img_set_src(wifi_icon, &img_wink_png);
}

void myapp::create_battery_label(lv_obj_t *baselayout) {
    static lv_style_t style_bat_bg;
    lv_style_init(&style_bat_bg);
    lv_style_set_bg_opa(&style_bat_bg, LV_OPA_100);
    lv_style_set_bg_color(&style_bat_bg, lv_color_hex(BAT_COLOR_BG_NORMAL));
    lv_style_set_radius(&style_bat_bg, BAT_RADIUS);

    battery_bg = lv_obj_create(baselayout);
    lv_obj_remove_style_all(battery_bg);
    lv_obj_add_style(battery_bg, &style_bat_bg, 0);
    // lv_obj_align(battery_bg, LV_ALIGN_TOP_RIGHT, -BTN_TOP_PADDING, BAT_PADDING);
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

void myapp::create_wifi_label(lv_obj_t *baselayout) {
    wifi_icon = lv_img_create(baselayout);
    set_wifi_status(this->view_model.is_wifi_connect);
    lv_img_set_size_mode(wifi_icon, LV_IMG_SIZE_MODE_REAL);
    lv_obj_set_size(wifi_icon, ICON_INACTIVE_W, ICON_INACTIVE_H);
    // lv_obj_set_style_pad_right(wifi_icon, 12, 0);
    lv_obj_set_flex_grow(wifi_icon, 0);
}

void myapp::create_header_bar(lv_obj_t *baselayout) {
    // create_image_btn(setting_btn, baselayout, this, MMAP_RESOURCES_SETTING_SPNG, on_setting_tap);
    create_wifi_label(baselayout);
    auto spaceobj = lv_obj_create(baselayout);
    lv_obj_remove_style_all(spaceobj);
    lv_obj_set_flex_grow(spaceobj, 1);
    create_battery_label(baselayout);
}
