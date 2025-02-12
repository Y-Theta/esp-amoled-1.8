#include "devicemanager.h"

devicemanager::devicemanager() {
    io_expander_handle = NULL;
    screen_spi_handle = NULL;
    screen_handle = NULL;
    touch_handle = NULL;
}

devicemanager::~devicemanager() {
}

void devicemanager::init_i2c() {
    ESP_LOGI(TAG, "Initialize I2C bus");
    i2c_conf.mode = I2C_MODE_MASTER;
    i2c_conf.sda_io_num = PIN_NUM_TOUCH_SDA;
    i2c_conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_conf.scl_io_num = PIN_NUM_TOUCH_SCL;
    i2c_conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_conf.clk_flags = 0;
    i2c_conf.master.clk_speed = 200 * 1000;

    ESP_ERROR_CHECK(i2c_param_config(TOUCH_HOST, &i2c_conf));
    ESP_ERROR_CHECK(i2c_driver_install(TOUCH_HOST, i2c_conf.mode, 0, 0, 0));
}

void devicemanager::init_io_expander() {
    ESP_LOGI(TAG, "Initialize IO Expander");

    esp_io_expander_new_i2c_tca9554(TOUCH_HOST, ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000, &io_expander_handle);

    ESP_ERROR_CHECK(esp_io_expander_new_i2c_tca9554(TOUCH_HOST, ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000, &io_expander_handle));

    esp_io_expander_set_dir(io_expander_handle, IO_EXPANDER_PIN_NUM_0 | IO_EXPANDER_PIN_NUM_1 | IO_EXPANDER_PIN_NUM_2, IO_EXPANDER_OUTPUT);
    esp_io_expander_set_dir(io_expander_handle, IO_EXPANDER_PIN_NUM_4, IO_EXPANDER_INPUT);
    esp_io_expander_set_level(io_expander_handle, IO_EXPANDER_PIN_NUM_0, 0);
    esp_io_expander_set_level(io_expander_handle, IO_EXPANDER_PIN_NUM_1, 0);
    esp_io_expander_set_level(io_expander_handle, IO_EXPANDER_PIN_NUM_2, 0);
    vTaskDelay(pdMS_TO_TICKS(200));
    esp_io_expander_set_level(io_expander_handle, IO_EXPANDER_PIN_NUM_0, 1);
    esp_io_expander_set_level(io_expander_handle, IO_EXPANDER_PIN_NUM_1, 1);
    esp_io_expander_set_level(io_expander_handle, IO_EXPANDER_PIN_NUM_2, 1);

    gpio_set_direction(GPIO_NUM_0, GPIO_MODE_INPUT);
}

static const sh8601_lcd_init_cmd_t lcd_init_cmds[] = {
    {0x11, (uint8_t[]){0x00}, 0, 120},
    {0x44, (uint8_t[]){0x01, 0xD1}, 2, 0},
    {0x35, (uint8_t[]){0x00}, 1, 0},
    {0x53, (uint8_t[]){0x20}, 1, 10},
    {0x2A, (uint8_t[]){0x00, 0x00, 0x01, 0x6F}, 4, 0},
    {0x2B, (uint8_t[]){0x00, 0x00, 0x01, 0xBF}, 4, 0},
    {0x51, (uint8_t[]){0x00}, 1, 10},
    {0x29, (uint8_t[]){0x00}, 0, 10},
    {0x51, (uint8_t[]){0xFF}, 1, 0},
};

void devicemanager::init_screen() {

    ESP_LOGI(TAG, "Initialize Screen");
    buscfg.sclk_io_num = PIN_NUM_LCD_PCLK;
    buscfg.data0_io_num = PIN_NUM_LCD_DATA0;
    buscfg.data1_io_num = PIN_NUM_LCD_DATA1;
    buscfg.data2_io_num = PIN_NUM_LCD_DATA2;
    buscfg.data3_io_num = PIN_NUM_LCD_DATA3;
    buscfg.data4_io_num = -1;
    buscfg.data5_io_num = -1;
    buscfg.data6_io_num = -1;
    buscfg.data7_io_num = -1;
    buscfg.isr_cpu_id = ESP_INTR_CPU_AFFINITY_AUTO;
    buscfg.intr_flags = 0;
    buscfg.flags = 0;
    buscfg.max_transfer_sz = SCREEN_H_RES * SCREEN_V_RES * LCD_BIT_PER_PIXEL / 8;
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

    ESP_LOGI(TAG, "Install panel IO");
    // esp_lcd_panel_io_handle_t io_handle = NULL;
    io_config = SH8601_PANEL_IO_QSPI_CONFIG(PIN_NUM_LCD_CS, NULL, NULL);
    sh8601_vendor_config_t vendor_config = {
        .init_cmds = lcd_init_cmds,
        .init_cmds_size = sizeof(lcd_init_cmds) / sizeof(lcd_init_cmds[0]),
        .flags = {
            .use_qspi_interface = 1,
        },
    };
    // Attach the LCD to the SPI bus
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &screen_spi_handle));

    // esp_lcd_panel_handle_t panel_handle = NULL;
    const esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_NUM_LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = LCD_BIT_PER_PIXEL,
        .vendor_config = &vendor_config,
    };
    ESP_LOGI(TAG, "Install SH8601 panel driver");
    ESP_ERROR_CHECK(esp_lcd_new_panel_sh8601(screen_spi_handle, &panel_config, &screen_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(screen_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(screen_handle));
    // user can flush pre-defined pattern to the screen before we turn on the screen or backlight
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(screen_handle, true));
}

void devicemanager::init_touch() {
    ESP_LOGI(TAG, "Initialize Touch");
    esp_lcd_panel_io_handle_t tp_io_handle = NULL;
    const esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_FT5x06_CONFIG();
    // Attach the TOUCH to the I2C bus
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)TOUCH_HOST, &tp_io_config, &tp_io_handle));

    const esp_lcd_touch_config_t tp_cfg = {
        .x_max = SCREEN_H_RES,
        .y_max = SCREEN_V_RES,
        .rst_gpio_num = PIN_NUM_TOUCH_RST,
        .int_gpio_num = PIN_NUM_TOUCH_INT,
        .levels = {
            .reset = 0,
            .interrupt = 0,
        },
        .flags = {
            .swap_xy = 0,
            .mirror_x = 0,
            .mirror_y = 0,
        },
    };

    ESP_LOGI(TAG, "Initialize touch controller");
    ESP_ERROR_CHECK(esp_lcd_touch_new_i2c_ft5x06(tp_io_handle, &tp_cfg, &touch_handle));
}
