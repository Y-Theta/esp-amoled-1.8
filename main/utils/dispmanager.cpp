#include "dispmanager.h"

dispmanager::dispmanager(powermanager* manager, myapp* app) {
    io_expander_handle = NULL;
    screen_spi_handle = NULL;
    screen_handle = NULL;
    touch_handle = NULL;
    power_manager = manager;
    this->app = app;
}

dispmanager::~dispmanager() {
}

void dispmanager::init_i2c() {
    ESP_LOGI(TAG, "Initialize I2C bus");
    i2c_conf.mode = I2C_MODE_MASTER;
    i2c_conf.sda_io_num = EXAMPLE_PIN_NUM_TOUCH_SDA;
    i2c_conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_conf.scl_io_num = EXAMPLE_PIN_NUM_TOUCH_SCL;
    i2c_conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_conf.clk_flags = 0;
    i2c_conf.master.clk_speed = 200 * 1000;

    ESP_ERROR_CHECK(i2c_param_config(TOUCH_HOST, &i2c_conf));
    ESP_ERROR_CHECK(i2c_driver_install(TOUCH_HOST, i2c_conf.mode, 0, 0, 0));
}

void dispmanager::init_io_expander() {
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

static bool example_notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx) {
    lv_disp_drv_t *disp_driver = (lv_disp_drv_t *)user_ctx;
    lv_disp_flush_ready(disp_driver);
    return false;
}

void dispmanager::init_screen() {

    ESP_LOGI(TAG, "Initialize Screen");
    buscfg.sclk_io_num = EXAMPLE_PIN_NUM_LCD_PCLK;
    buscfg.data0_io_num = EXAMPLE_PIN_NUM_LCD_DATA0;
    buscfg.data1_io_num = EXAMPLE_PIN_NUM_LCD_DATA1;
    buscfg.data2_io_num = EXAMPLE_PIN_NUM_LCD_DATA2;
    buscfg.data3_io_num = EXAMPLE_PIN_NUM_LCD_DATA3;
    buscfg.data4_io_num = -1;
    buscfg.data5_io_num = -1;
    buscfg.data6_io_num = -1;
    buscfg.data7_io_num = -1;
    buscfg.isr_cpu_id = ESP_INTR_CPU_AFFINITY_AUTO;
    buscfg.intr_flags = 0;
    buscfg.flags = 0;
    buscfg.max_transfer_sz = EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES * LCD_BIT_PER_PIXEL / 8;
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

    ESP_LOGI(TAG, "Install panel IO");
    // esp_lcd_panel_io_handle_t io_handle = NULL;
    io_config = SH8601_PANEL_IO_QSPI_CONFIG(EXAMPLE_PIN_NUM_LCD_CS, example_notify_lvgl_flush_ready, &disp_drv);
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
        .reset_gpio_num = EXAMPLE_PIN_NUM_LCD_RST,
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

void dispmanager::init_touch() {
    ESP_LOGI(TAG, "Initialize Touch");
    esp_lcd_panel_io_handle_t tp_io_handle = NULL;
    const esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_FT5x06_CONFIG();
    // Attach the TOUCH to the I2C bus
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)TOUCH_HOST, &tp_io_config, &tp_io_handle));

    const esp_lcd_touch_config_t tp_cfg = {
        .x_max = EXAMPLE_LCD_H_RES,
        .y_max = EXAMPLE_LCD_V_RES,
        .rst_gpio_num = EXAMPLE_PIN_NUM_TOUCH_RST,
        .int_gpio_num = EXAMPLE_PIN_NUM_TOUCH_INT,
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

static bool flag = false;
static void toogle_screen(dispmanager* manager) {
    if (flag) {
        esp_lcd_panel_disp_on_off(manager->screen_handle, true);
        // esp_lcd_touch_exit_sleep(manager->touch_handle);
        manager->power_manager->wakeup();
    } else {
        esp_lcd_panel_disp_on_off(manager->screen_handle, false);
        // esp_lcd_touch_enter_sleep(manager->touch_handle);
        manager->power_manager->sleep();
    }
    flag = !flag;
}

static bool flag1 = false;
static void toogle_cpu(dispmanager* manager) {
    if (flag1) {
        manager->app->pause_ani();
    } else {
        manager->app->resume_ani();
    }
    flag1 = !flag1;
}

static void task_btn(void *param) {
    auto handle = (dispmanager*) param;
    while (true)
    {
        uint32_t pin_levels = 0;
        esp_io_expander_get_level(handle->io_expander_handle, IO_EXPANDER_PIN_NUM_4, &pin_levels);
        if (pin_levels) {
            while (true) {
                vTaskDelay(pdMS_TO_TICKS(50));
                esp_io_expander_get_level(handle->io_expander_handle, IO_EXPANDER_PIN_NUM_4, &pin_levels);
                if (!pin_levels) {
                    toogle_screen(handle);
                    break;
                }
            }
        }
        
        int level = gpio_get_level(GPIO_NUM_0);
        if(!level){
              while (true) {
                vTaskDelay(pdMS_TO_TICKS(50));
                level = gpio_get_level(GPIO_NUM_0);
                if (level) {
                    ESP_LOGI(TAG,"BOOT Click");
                    toogle_cpu(handle);
                    break;
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void dispmanager::framework_init() {
    init_lvgl();
    xTaskCreate(task_btn, "app/btn", 3 * 1024, this, 2, NULL);
}

static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map) {
    auto panel_handle = (dispmanager *)drv->user_data;
    const int offsetx1 = area->x1;
    const int offsetx2 = area->x2;
    const int offsety1 = area->y1;
    const int offsety2 = area->y2;

#if LCD_BIT_PER_PIXEL == 24
    uint8_t *to = (uint8_t *)color_map;
    uint8_t temp = 0;
    uint16_t pixel_num = (offsetx2 - offsetx1 + 1) * (offsety2 - offsety1 + 1);

    // Special dealing for first pixel
    temp = color_map[0].ch.blue;
    *to++ = color_map[0].ch.red;
    *to++ = color_map[0].ch.green;
    *to++ = temp;
    // Normal dealing for other pixels
    for (int i = 1; i < pixel_num; i++) {
        *to++ = color_map[i].ch.red;
        *to++ = color_map[i].ch.green;
        *to++ = color_map[i].ch.blue;
    }
#endif

    // copy a buffer's content to a specific area of the display
    esp_lcd_panel_draw_bitmap(panel_handle->screen_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
}
/* Rotate display and touch, when rotated screen in LVGL. Called when driver parameters are updated. */
static void lvgl_update_cb(lv_disp_drv_t *drv) {
    auto panel_handle = (dispmanager *)drv->user_data;

    switch (drv->rotated) {
    case LV_DISP_ROT_NONE:
        // Rotate LCD display
        esp_lcd_panel_swap_xy(panel_handle->screen_handle, false);
        esp_lcd_panel_mirror(panel_handle->screen_handle, true, false);
        break;
    case LV_DISP_ROT_90:
        // Rotate LCD display
        esp_lcd_panel_swap_xy(panel_handle->screen_handle, true);
        esp_lcd_panel_mirror(panel_handle->screen_handle, true, true);
        break;
    case LV_DISP_ROT_180:
        // Rotate LCD display
        esp_lcd_panel_swap_xy(panel_handle->screen_handle, false);
        esp_lcd_panel_mirror(panel_handle->screen_handle, false, true);
        break;
    case LV_DISP_ROT_270:
        // Rotate LCD display
        esp_lcd_panel_swap_xy(panel_handle->screen_handle, true);
        esp_lcd_panel_mirror(panel_handle->screen_handle, false, false);
        break;
    }
}

static void lvgl_rounder_cb(struct _lv_disp_drv_t *disp_drv, lv_area_t *area) {
    uint16_t x1 = area->x1;
    uint16_t x2 = area->x2;

    uint16_t y1 = area->y1;
    uint16_t y2 = area->y2;

    // round the start of coordinate down to the nearest 2M number
    area->x1 = (x1 >> 1) << 1;
    area->y1 = (y1 >> 1) << 1;
    // round the end of coordinate up to the nearest 2N+1 number
    area->x2 = ((x2 >> 1) << 1) + 1;
    area->y2 = ((y2 >> 1) << 1) + 1;
}

#if EXAMPLE_USE_TOUCH
static void lvgl_touch_cb(lv_indev_drv_t *drv, lv_indev_data_t *data) {
    auto tp = (dispmanager *)drv->user_data;
    assert(tp);

    uint16_t tp_x;
    uint16_t tp_y;
    uint8_t tp_cnt = 0;
    /* Read data from touch controller into memory */
    esp_lcd_touch_read_data(tp->touch_handle);
    /* Read data from touch controller */
    bool tp_pressed = esp_lcd_touch_get_coordinates(tp->touch_handle, &tp_x, &tp_y, NULL, &tp_cnt, 1);
    if (tp_pressed && tp_cnt > 0) {
        data->point.x = tp_x;
        data->point.y = tp_y;
        data->state = LV_INDEV_STATE_PRESSED;
        ESP_LOGD(TAG, "Touch position: %d,%d", tp_x, tp_y);
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}
#endif

static void increase_lvgl_tick(void *arg) {
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}

void dispmanager::init_lvgl() {

    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();

    void *buf1 = heap_caps_malloc(EXAMPLE_LCD_H_RES * EXAMPLE_LVGL_BUF_HEIGHT * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf1);
    void *buf2 = heap_caps_malloc(EXAMPLE_LCD_H_RES * EXAMPLE_LVGL_BUF_HEIGHT * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf2);

    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, EXAMPLE_LCD_H_RES * EXAMPLE_LVGL_BUF_HEIGHT);

    ESP_LOGI(TAG, "Register display driver to LVGL");
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = EXAMPLE_LCD_H_RES;
    disp_drv.ver_res = EXAMPLE_LCD_V_RES;
    disp_drv.flush_cb = lvgl_flush_cb;
    disp_drv.rounder_cb = lvgl_rounder_cb;
    disp_drv.drv_update_cb = lvgl_update_cb;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.user_data = this;
    lv_disp_t *disp = lv_disp_drv_register(&disp_drv);

    ESP_LOGI(TAG, "Install LVGL tick timer");
    // Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &increase_lvgl_tick,
        .name = "lvgl_tick"};
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000));

#if EXAMPLE_USE_TOUCH
    static lv_indev_drv_t indev_drv; // Input device driver (Touch)
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.disp = disp;
    indev_drv.read_cb = lvgl_touch_cb;
    indev_drv.user_data = this;
    lv_indev_drv_register(&indev_drv);
#endif
}