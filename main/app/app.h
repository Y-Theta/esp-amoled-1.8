#ifndef USE_COMMON
#include "common.h"
#endif

#ifndef POWER_MANAGER
#include "utils/powermanager.h"
#endif

#ifndef USE_DISP
#include "utils/devicemanager.h"
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


class myapp {
private:
    static mmap_assets_handle_t mmap_drive_handle;
    static esp_lv_decoder_handle_t decoder_handle;
    static std::map<MMAP_RESOURCES_LISTS ,COMMON::assets_info_t> assets_map;

    static esp_err_t init_mmapfile();
    static esp_err_t release_mmapfile();
    static void create_image_btn(lv_obj_t *pointer, lv_obj_t *screen, myapp *app, MMAP_RESOURCES_LISTS image, lv_event_cb_t cb);

    lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
    lv_disp_drv_t disp_drv;      // contains callback functions

    lv_style_t header_bar_style;
    lv_obj_t *header_bar = NULL;

    lv_style_t body_style;
    lv_obj_t *body = NULL;

    lv_obj_t *battery_label = NULL;
    lv_obj_t *battery_bg = NULL;

    lv_obj_t *wifi_icon = NULL;

    lv_obj_t *setting_btn = NULL;

    lv_obj_t *lottie_ani = NULL;

    /// @brief 初始化硬件按钮
    void init_btn();
    /// @brief 初始化 lvgl
    void init_framework();
    /// @brief 初始化界面布局
    /// @return 
    lv_obj_t* init_layout();
    /// @brief 创建电池电量控件
    void create_battery_label(lv_obj_t *baselayout);
    /// @brief 创建 wifi 标识
    void create_wifi_label(lv_obj_t *baselayout);
    /// @brief 创建上部区域控件
    void create_header_bar(lv_obj_t *baselayout);
    /// @brief 
    void wifi_window();
public:

    static COMMON::assets_info_t* get_mmap_assets(MMAP_RESOURCES_LISTS assets);

    powermanager *power_manager;
    devicemanager *device_manager;
    wifimanager *wifi_manager;
    fsmanager *fs_manager;
    
    COMMON::global_config config;
    COMMON::main_view_model view_model;

    std::function<void(myapp* app)> pwr_click;
    std::function<void(myapp* app)> boot_click;

    myapp();
    ~myapp();
    
    void init();
    void init_ui_elements();
    void update_battery_status(powermanager *manager);

    void set_wifi_status(bool flag);
    void open_wifi_setting();

    void pause_ani();
    void resume_ani();
};

#endif