#ifndef USE_COMMON
#include "common.h"
#endif

#ifndef POWER_MANAGER
#include "utils/powermanager.h"
#endif

class myapp {
private:
    lv_obj_t *battery_label = NULL;
    lv_obj_t *lottie_ani = NULL;
    lv_style_t *battery_style = NULL;

public:
    myapp();
    ~myapp();

    void init_ui_elements();
    void update_battery_status(powermanager *manager);
};
