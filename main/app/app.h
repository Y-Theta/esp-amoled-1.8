#ifndef USE_COMMON
#include "common.h"
#endif

#ifndef POWER_MANAGER
#include "utils/powermanager.h"
#endif

#ifndef MY_APP
#define MY_APP

class myapp {
private:
    lv_obj_t *battery_label = NULL;
    lv_obj_t *lottie_ani = NULL;
    lv_obj_t *setting_image = NULL;
    lv_style_t *battery_style = NULL;

public:
    myapp();
    ~myapp();

    void init_ui_elements();
    void update_battery_status(powermanager *manager);

    void pause_ani();
    void resume_ani();
};

#endif