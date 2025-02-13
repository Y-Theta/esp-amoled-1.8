#include "app.h"


bool flag = false;
static void on_setting_click(lv_event_t *event) {
    auto app = (myapp *)event->user_data;
    ESP_LOGI(TAG, "btn tap %d", flag);
    app->open_setting();
    // app->wifi_manager->scan();
}

void myapp::create_status_bar(lv_obj_t *baselayout) {
    setting_btn = create_image_btn(this, baselayout, MMAP_RESOURCES_SETTING_SPNG, ICON_ACTIVE_H, on_setting_click);
}