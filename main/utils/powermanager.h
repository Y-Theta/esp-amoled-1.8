#ifndef USE_COMMON
#include "common.h"
#endif

#define XPOWERS_CHIP_AXP2101
#include "XPowersLib.h"
#include <cstring>
#include "sdkconfig.h"

class powermanager
{
private:
    XPowersPMU PMU;

public:
    powermanager(/* args */);
    ~powermanager();

    esp_err_t init();
    int get_percent();
    bool is_charging();
};
