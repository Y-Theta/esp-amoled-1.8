#ifndef USE_COMMON
#include "common.h"
#endif

#define XPOWERS_CHIP_AXP2101
#include "XPowersLib.h"
#include <cstring>
#include "sdkconfig.h"

#ifndef POWER_MANAGER
#define POWER_MANAGER

class powermanager
{
private:
    XPowersPMU PMU;
    TaskHandle_t monitor_handle;

public:
    powermanager(/* args */);
    ~powermanager();

    std::function<void(powermanager* manager)> power_cb;

    esp_err_t init();
    int get_percent();
    bool is_charging();

    void start_power_monitor();
    void sleep();
    void wakeup();
};

#endif