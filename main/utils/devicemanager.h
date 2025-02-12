#ifndef USE_COMMON
#include "common.h"
#endif

#ifndef POWER_MANAGER
#include "powermanager.h"
#endif

#ifndef USE_DISP
#define USE_DISP

class devicemanager
{
private:
    /* data */
    i2c_config_t i2c_conf;
    spi_bus_config_t buscfg;
    esp_lcd_panel_io_spi_config_t io_config;

public:
    devicemanager();
    ~devicemanager();

    esp_io_expander_handle_t io_expander_handle;
    esp_lcd_panel_io_handle_t screen_spi_handle;
    esp_lcd_panel_handle_t screen_handle;
    esp_lcd_touch_handle_t touch_handle;

    void init_i2c();
    void init_io_expander();
    void init_screen();
    void init_touch();

    void set_brightness(int bright);
};

#endif