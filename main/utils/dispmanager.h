#ifndef USE_COMMON
#include "common.h"
#endif

class dispmanager
{
private:
    /* data */
    i2c_config_t i2c_conf;
    spi_bus_config_t buscfg;
    esp_lcd_panel_io_spi_config_t io_config;

    lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
    lv_disp_drv_t disp_drv;      // contains callback functions

    int btn_click_timeout = 1000; // btn click times count interval 

protected:
    virtual void init_lvgl();

public:
    dispmanager(/* args */);
    ~dispmanager();

    esp_io_expander_handle_t io_expander_handle;
    esp_lcd_panel_io_handle_t screen_spi_handle;
    esp_lcd_panel_handle_t screen_handle;
    esp_lcd_touch_handle_t touch_handle;

    void init_i2c();
    void init_io_expander();
    void init_screen();
    void init_touch();
    void framework_init();

    void set_brightness(int bright);
    int pwr_click();
    int boot_click();
};

