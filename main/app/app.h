#ifndef USE_COMMON
#include "common.h"
#endif

#ifndef POWER_MANAGER
#include "utils/powermanager.h"
#endif

#ifndef USE_DISP
#include "utils/dispmanager.h"
#endif

#ifndef USE_WIFI
#include "utils/wifimanager.h"
#endif

#ifndef USE_FS
#include "utils/fsmanager.h"
#endif

#ifndef MY_APP
#define MY_APP
#include "esp_lv_decoder.h"
#include "mmap_generate_resources.h"

typedef struct {
    button_driver_t base;
    int32_t gpio_num;              /**< num of gpio */
    uint8_t active_level;          /**< gpio level when press down */
} custom_gpio_obj;

class myapp {
private:
    static mmap_assets_handle_t mmap_drive_handle;
    static esp_lv_decoder_handle_t decoder_handle;
    static esp_err_t init_mmapfile();
    static esp_err_t release_mmapfile();
    static void create_image_btn(lv_obj_t *pointer, lv_obj_t *screen, myapp *app, MMAP_RESOURCES_LISTS image, lv_event_cb_t cb);

    lv_obj_t *battery_label = NULL;
    lv_obj_t *battery_bg = NULL;

    lv_obj_t *lottie_ani = NULL;
    lv_obj_t *setting_image = NULL;
    lv_style_t *battery_style = NULL;

    void init_btn();
    /// @brief 创建电池电量控件
    void create_battery_label();
public:
    powermanager *power_manager;
    dispmanager *disp_manager;
    wifimanager *wifi_manager;
    fsmanager *fs_manager;

    std::function<void(myapp* app)> pwr_click;
    std::function<void(myapp* app)> boot_click;

    myapp();
    ~myapp();
    
    void init();
    void init_ui_elements();
    void update_battery_status(powermanager *manager);

    void pause_ani();
    void resume_ani();
};

#endif