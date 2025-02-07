#include "app.h"
#include "esp_lv_decoder.h"

const char lottiedata[] = "{\"v\":\"5.7.1\",\"fr\":60,\"ip\":0,\"op\":60,\"w\":256,\"h\":256,\"nm\":\"合成1\",\"ddd\":0,\"assets\":[],\"layers\":[{\"ddd\":0,\"ind\":1,\"ty\":4,\"nm\":\"“未标题-2”轮廓\",\"sr\":1,\"ks\":{\"o\":{\"a\":0,\"k\":100,\"ix\":11},\"r\":{\"a\":0,\"k\":0,\"ix\":10},\"p\":{\"a\":0,\"k\":[128,128,0],\"ix\":2},\"a\":{\"a\":0,\"k\":[128,128,0],\"ix\":1},\"s\":{\"a\":0,\"k\":[100,100,100],\"ix\":6}},\"ao\":0,\"shapes\":[{\"ty\":\"gr\",\"it\":[{\"ind\":0,\"ty\":\"sh\",\"ix\":1,\"ks\":{\"a\":1,\"k\":[{\"i\":{\"x\":0.833,\"y\":0.833},\"o\":{\"x\":0.167,\"y\":0.167},\"t\":0,\"s\":[{\"i\":[[7.732,0],[0,7.732],[-7.732,0],[0,-7.732]],\"o\":[[-7.732,0],[0,-7.732],[7.732,0],[0,7.732]],\"v\":[[40.249,9.567],[26.249,-4.433],[40.249,-18.433],[54.249,-4.433]],\"c\":true}]},{\"i\":{\"x\":0.833,\"y\":0.833},\"o\":{\"x\":0.167,\"y\":0.167},\"t\":30,\"s\":[{\"i\":[[7.732,0],[0,7.732],[-7.732,0],[0,-7.732]],\"o\":[[-7.732,0],[0,-7.732],[7.732,0],[0,7.732]],\"v\":[[27.499,0.067],[13.499,-13.933],[27.499,-27.933],[41.499,-13.933]],\"c\":true}]},{\"t\":60,\"s\":[{\"i\":[[7.732,0],[0,7.732],[-7.732,0],[0,-7.732]],\"o\":[[-7.732,0],[0,-7.732],[7.732,0],[0,7.732]],\"v\":[[40.499,8.317],[26.499,-5.683],[40.499,-19.683],[54.499,-5.683]],\"c\":true}]}],\"ix\":2},\"nm\":\"路径1\",\"mn\":\"ADBEVectorShape-Group\",\"hd\":false},{\"ind\":1,\"ty\":\"sh\",\"ix\":2,\"ks\":{\"a\":1,\"k\":[{\"i\":{\"x\":0.833,\"y\":0.833},\"o\":{\"x\":0.167,\"y\":0.167},\"t\":0,\"s\":[{\"i\":[[7.732,0],[0,7.732],[-7.732,0],[0,-7.732]],\"o\":[[-7.732,0],[0,-7.732],[7.732,0],[0,7.732]],\"v\":[[-36.752,9.567],[-50.752,-4.433],[-36.752,-18.433],[-22.752,-4.433]],\"c\":true}]},{\"i\":{\"x\":0.833,\"y\":0.833},\"o\":{\"x\":0.167,\"y\":0.167},\"t\":30,\"s\":[{\"i\":[[7.732,0],[0,7.732],[-7.732,0],[0,-7.732]],\"o\":[[-7.732,0],[0,-7.732],[7.732,0],[0,7.732]],\"v\":[[-37.002,4.817],[-50.752,-4.433],[-36.502,-7.433],[-22.752,-4.433]],\"c\":true}]},{\"t\":60,\"s\":[{\"i\":[[7.732,0],[0,7.732],[-7.732,0],[0,-7.732]],\"o\":[[-7.732,0],[0,-7.732],[7.732,0],[0,7.732]],\"v\":[[-36.752,8.817],[-50.752,-4.433],[-37.002,-18.683],[-22.752,-4.433]],\"c\":true}]}],\"ix\":2},\"nm\":\"路径2\",\"mn\":\"ADBEVectorShape-Group\",\"hd\":false},{\"ind\":2,\"ty\":\"sh\",\"ix\":3,\"ks\":{\"a\":0,\"k\":{\"i\":[[0,0],[7.793,0],[0,0],[0,-7.792],[0,0],[-16,0],[0,0],[0,87]],\"o\":[[0,-7.792],[0,0],[-7.793,0],[0,0],[-0.5,82],[0,0],[15,0],[0,0]],\"v\":[[96.501,-41.082],[82.331,-55.25],[-82.332,-55.25],[-96.501,-41.082],[-96.501,-31.75],[-24.001,48.25],[24.001,48.25],[97.001,-31.75]],\"c\":true},\"ix\":2},\"nm\":\"路径3\",\"mn\":\"ADBEVectorShape-Group\",\"hd\":false},{\"ty\":\"mm\",\"mm\":1,\"nm\":\"合并路径1\",\"mn\":\"ADBEVectorFilter-Merge\",\"hd\":false},{\"ty\":\"fl\",\"c\":{\"a\":0,\"k\":[1,1,1,1],\"ix\":4},\"o\":{\"a\":0,\"k\":100,\"ix\":5},\"r\":1,\"bm\":0,\"nm\":\"填充1\",\"mn\":\"ADBEVectorGraphic-Fill\",\"hd\":false},{\"ty\":\"tr\",\"p\":{\"a\":0,\"k\":[127.752,131.433],\"ix\":2},\"a\":{\"a\":0,\"k\":[0,0],\"ix\":1},\"s\":{\"a\":0,\"k\":[100,100],\"ix\":3},\"r\":{\"a\":0,\"k\":0,\"ix\":6},\"o\":{\"a\":0,\"k\":100,\"ix\":7},\"sk\":{\"a\":0,\"k\":0,\"ix\":4},\"sa\":{\"a\":0,\"k\":0,\"ix\":5},\"nm\":\"变换\"}],\"nm\":\"组1\",\"np\":5,\"cix\":2,\"bm\":0,\"ix\":1,\"mn\":\"ADBEVectorGroup\",\"hd\":false}],\"ip\":0,\"op\":60,\"st\":0,\"bm\":0}],\"markers\":[]}";

myapp::myapp() {
}

myapp::~myapp() {
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

void myapp::init_ui_elements() {
    auto screen = lv_scr_act();
    lottie_ani = lv_rlottie_create_from_raw(screen, 160, 160, lottiedata);
    lv_obj_center(lottie_ani);

    esp_lv_decoder_handle_t decoder_handle = NULL;
    esp_err_t ret_fs = esp_lv_decoder_init(&decoder_handle);
    if (ret_fs != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SPNG decoder");
        return;
    }
    setting_image = lv_img_create(screen);
    ESP_LOGI(TAG,"%d",setting_png_end - setting_png_start);
    static lv_img_dsc_t img_wink_png;
    img_wink_png.header.always_zero = 0;
    img_wink_png.header.w = 77,
    img_wink_png.header.h = 78,
    img_wink_png.header.cf = LV_IMG_CF_RAW,
    img_wink_png.data_size = setting_png_end - setting_png_start,
    img_wink_png.data = setting_png_start,
    lv_img_set_src(setting_image, &img_wink_png);
    lv_img_set_zoom(setting_image, 128);
    lv_obj_align(setting_image, LV_ALIGN_TOP_LEFT, 20, 20);
    lv_obj_set_size(setting_image, 32, 32);
    esp_lv_decoder_deinit(decoder_handle);

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
    lv_rlottie_set_play_mode(lottie_ani, LV_RLOTTIE_CTRL_PAUSE);
}

void myapp::resume_ani() {
    const lv_rlottie_ctrl_t item = (lv_rlottie_ctrl_t)(8);
    lv_rlottie_set_play_mode(lottie_ani, item);
}