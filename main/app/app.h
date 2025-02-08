#ifndef USE_COMMON
#include "common.h"
#endif

#ifndef POWER_MANAGER
#include "utils/powermanager.h"
#endif

#ifndef MY_APP
#define MY_APP
#include "esp_lv_decoder.h"
#include "mmap_generate_spiffs.h"

class myapp {
private:

    mmap_assets_handle_t mmap_drive_handle;
    esp_lv_decoder_handle_t decoder_handle;
    lv_obj_t *battery_label = NULL;
    lv_obj_t *lottie_ani = NULL;
    lv_obj_t *setting_image = NULL;
    lv_style_t *battery_style = NULL;

    esp_err_t init_mmapfile();
    esp_err_t release_mmapfile();

public:
    myapp();
    ~myapp();

    void init_ui_elements();
    void update_battery_status(powermanager *manager);

    void pause_ani();
    void resume_ani();
};

#endif