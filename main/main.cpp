#include "common.h"

extern esp_err_t pmu_init();
extern esp_err_t i2c_init(void);
extern void pmu_isr_handler();

static void pmu_hander_task(void *);
static QueueHandle_t gpio_evt_queue = NULL;

const char lottiedata[] = "{ \"v\": \"5.7.1\", \"fr\": 60, \"ip\": 0, \"op\": 240, \"w\": 256, \"h\": 256, \"nm\": \"合成 1\", \"ddd\": 0, \"assets\": [], \"layers\": [{ \"ddd\": 0, \"ind\": 1, \"ty\": 4, \"nm\": \"“图层 2/未标题-1”轮廓\", \"sr\": 1, \"ks\": { \"o\": { \"a\": 0, \"k\": 100, \"ix\": 11 }, \"r\": { \"a\": 0, \"k\": 0, \"ix\": 10 }, \"p\": { \"a\": 0, \"k\": [128.345, 160.5, 0], \"ix\": 2 }, \"a\": { \"a\": 0, \"k\": [15.5, 13.5, 0], \"ix\": 1 }, \"s\": { \"a\": 0, \"k\": [144.494, 144.494, 100], \"ix\": 6 } }, \"ao\": 0, \"shapes\": [{ \"ty\": \"gr\", \"it\": [{ \"ind\": 0, \"ty\": \"sh\", \"ix\": 1, \"ks\": { \"a\": 0, \"k\": { \"i\": [[0, 0], [0, 0], [0, 0]], \"o\": [[0, 0], [0, 0], [0, 0]], \"v\": [[0, -13], [-15.011, 13], [15.011, 13]], \"c\": true }, \"ix\": 2 }, \"nm\": \"路径 1\", \"mn\": \"ADBE Vector Shape - Group\", \"hd\": false }, { \"ty\": \"fl\", \"c\": { \"a\": 0, \"k\": [0.968627510819, 0.576470588235, 0.117647066303, 1], \"ix\": 4 }, \"o\": { \"a\": 0, \"k\": 100, \"ix\": 5 }, \"r\": 1, \"bm\": 0, \"nm\": \"填充 1\", \"mn\": \"ADBE Vector Graphic - Fill\", \"hd\": false }, { \"ty\": \"tr\", \"p\": { \"a\": 0, \"k\": [15.261, 13.25], \"ix\": 2 }, \"a\": { \"a\": 0, \"k\": [0, 0], \"ix\": 1 }, \"s\": { \"a\": 0, \"k\": [100, 100], \"ix\": 3 }, \"r\": { \"a\": 0, \"k\": 0, \"ix\": 6 }, \"o\": { \"a\": 0, \"k\": 100, \"ix\": 7 }, \"sk\": { \"a\": 0, \"k\": 0, \"ix\": 4 }, \"sa\": { \"a\": 0, \"k\": 0, \"ix\": 5 }, \"nm\": \"变换\" }], \"nm\": \"组 1\", \"np\": 2, \"cix\": 2, \"bm\": 0, \"ix\": 1, \"mn\": \"ADBE Vector Group\", \"hd\": false }], \"ip\": 0, \"op\": 240, \"st\": 0, \"bm\": 0 }, { \"ddd\": 0, \"ind\": 2, \"ty\": 4, \"nm\": \"“图层 3/未标题-1”轮廓\", \"sr\": 1, \"ks\": { \"o\": { \"a\": 0, \"k\": 100, \"ix\": 11 }, \"r\": { \"a\": 1, \"k\": [{ \"i\": { \"x\": [0.833], \"y\": [0.833] }, \"o\": { \"x\": [0.167], \"y\": [0.167] }, \"t\": 0, \"s\": [-12] }, { \"i\": { \"x\": [0.833], \"y\": [0.833] }, \"o\": { \"x\": [0.167], \"y\": [0.167] }, \"t\": 120, \"s\": [12] }, { \"t\": 240, \"s\": [-12] }], \"ix\": 10 }, \"p\": { \"a\": 0, \"k\": [128.572, 133.25, 0], \"ix\": 2 }, \"a\": { \"a\": 0, \"k\": [50, 5, 0], \"ix\": 1 }, \"s\": { \"a\": 0, \"k\": [248.223, 158.824, 100], \"ix\": 6 } }, \"ao\": 0, \"shapes\": [{ \"ty\": \"gr\", \"it\": [{ \"ind\": 0, \"ty\": \"sh\", \"ix\": 1, \"ks\": { \"a\": 0, \"k\": { \"i\": [[0, 0], [0, 0], [0, 0], [0, 0]], \"o\": [[0, 0], [0, 0], [0, 0], [0, 0]], \"v\": [[49.5, 4.5], [-49.5, 4.5], [-49.5, -4.5], [49.5, -4.5]], \"c\": true }, \"ix\": 2 }, \"nm\": \"路径 1\", \"mn\": \"ADBE Vector Shape - Group\", \"hd\": false }, { \"ty\": \"fl\", \"c\": { \"a\": 0, \"k\": [0.968627510819, 0.576470588235, 0.117647066303, 1], \"ix\": 4 }, \"o\": { \"a\": 0, \"k\": 100, \"ix\": 5 }, \"r\": 1, \"bm\": 0, \"nm\": \"填充 1\", \"mn\": \"ADBE Vector Graphic - Fill\", \"hd\": false }, { \"ty\": \"tr\", \"p\": { \"a\": 0, \"k\": [49.75, 4.75], \"ix\": 2 }, \"a\": { \"a\": 0, \"k\": [0, 0], \"ix\": 1 }, \"s\": { \"a\": 0, \"k\": [100, 100], \"ix\": 3 }, \"r\": { \"a\": 0, \"k\": 0, \"ix\": 6 }, \"o\": { \"a\": 0, \"k\": 100, \"ix\": 7 }, \"sk\": { \"a\": 0, \"k\": 0, \"ix\": 4 }, \"sa\": { \"a\": 0, \"k\": 0, \"ix\": 5 }, \"nm\": \"变换\" }], \"nm\": \"组 1\", \"np\": 2, \"cix\": 2, \"bm\": 0, \"ix\": 1, \"mn\": \"ADBE Vector Group\", \"hd\": false }], \"ip\": 0, \"op\": 240, \"st\": 0, \"bm\": 0 }, { \"ddd\": 0, \"ind\": 3, \"ty\": 4, \"nm\": \"“图层 1/未标题-1”轮廓\", \"parent\": 2, \"sr\": 1, \"ks\": { \"o\": { \"a\": 0, \"k\": 100, \"ix\": 11 }, \"r\": { \"a\": 0, \"k\": 18.377, \"ix\": 10 }, \"p\": { \"a\": 1, \"k\": [{ \"i\": { \"x\": 0.833, \"y\": 0.833 }, \"o\": { \"x\": 0.333, \"y\": 0 }, \"t\": 0, \"s\": [97.07, -6.544, 0], \"to\": [0, 0, 0], \"ti\": [15.244, -0.003, 0] }, { \"i\": { \"x\": 0.833, \"y\": 0.833 }, \"o\": { \"x\": 0.167, \"y\": 0.167 }, \"t\": 38, \"s\": [56.053, -6.539, 0], \"to\": [-4.708, 0.001, 0], \"ti\": [-1.196, -0.001, 0] }, { \"i\": { \"x\": 0.667, \"y\": 1 }, \"o\": { \"x\": 0.167, \"y\": 0.167 }, \"t\": 86, \"s\": [13.475, -6.537, 0], \"to\": [2.677, 0.002, 0], \"ti\": [0, 0.011, 0] }, { \"i\": { \"x\": 0.833, \"y\": 0.896 }, \"o\": { \"x\": 0.333, \"y\": 0 }, \"t\": 121, \"s\": [4.07, -6.544, 0], \"to\": [0, -0.016, 0], \"ti\": [-15.057, -0.002, 0] }, { \"i\": { \"x\": 0.833, \"y\": 0.858 }, \"o\": { \"x\": 0.167, \"y\": 0.406 }, \"t\": 180, \"s\": [77.823, -6.549, 0], \"to\": [14.211, 0.001, 0], \"ti\": [-3.826, 0, 0] }, { \"t\": 240, \"s\": [97.07, -6.544, 0] }], \"ix\": 2 }, \"a\": { \"a\": 0, \"k\": [11.5, 11.5, 0], \"ix\": 1 }, \"s\": { \"a\": 0, \"k\": [41.523, 61.087, 100], \"ix\": 6 } }, \"ao\": 0, \"shapes\": [{ \"ty\": \"gr\", \"it\": [{ \"ind\": 0, \"ty\": \"sh\", \"ix\": 1, \"ks\": { \"a\": 0, \"k\": { \"i\": [[0, -6.075], [6.075, 0], [0, 6.075], [-6.075, 0]], \"o\": [[0, 6.075], [-6.075, 0], [0, -6.075], [6.075, 0]], \"v\": [[11, 0], [0, 11], [-11, 0], [0, -11]], \"c\": true }, \"ix\": 2 }, \"nm\": \"路径 1\", \"mn\": \"ADBE Vector Shape - Group\", \"hd\": false }, { \"ty\": \"fl\", \"c\": { \"a\": 0, \"k\": [0.968627510819, 0.576470588235, 0.117647066303, 1], \"ix\": 4 }, \"o\": { \"a\": 0, \"k\": 100, \"ix\": 5 }, \"r\": 1, \"bm\": 0, \"nm\": \"填充 1\", \"mn\": \"ADBE Vector Graphic - Fill\", \"hd\": false }, { \"ty\": \"tr\", \"p\": { \"a\": 0, \"k\": [11.25, 11.25], \"ix\": 2 }, \"a\": { \"a\": 0, \"k\": [0, 0], \"ix\": 1 }, \"s\": { \"a\": 0, \"k\": [100, 100], \"ix\": 3 }, \"r\": { \"a\": 0, \"k\": 0, \"ix\": 6 }, \"o\": { \"a\": 0, \"k\": 100, \"ix\": 7 }, \"sk\": { \"a\": 0, \"k\": -15, \"ix\": 4 }, \"sa\": { \"a\": 0, \"k\": 0, \"ix\": 5 }, \"nm\": \"变换\" }], \"nm\": \"组 1\", \"np\": 2, \"cix\": 2, \"bm\": 0, \"ix\": 1, \"mn\": \"ADBE Vector Group\", \"hd\": false }], \"ip\": 0, \"op\": 240, \"st\": 0, \"bm\": 0 }], \"markers\": [] }";

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

/**
 * @brief i2c master initialization
 */
esp_err_t i2c_init(i2c_config_t i2c_conf)
{
    i2c_param_config(I2C_MASTER_NUM, &i2c_conf);
    return i2c_driver_install(I2C_MASTER_NUM, i2c_conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

static void IRAM_ATTR pmu_irq_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void irq_init()
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = PMU_INPUT_PIN_SEL;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);
    gpio_set_intr_type(PMU_INPUT_PIN, GPIO_INTR_NEGEDGE);
    // install gpio isr service
    gpio_install_isr_service(0);
    // hook isr handler for specific gpio pin
    gpio_isr_handler_add(PMU_INPUT_PIN, pmu_irq_handler, (void *)PMU_INPUT_PIN);
}

int pmu_register_read(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len)
{
    if (len == 0)
    {
        return ESP_OK;
    }
    if (data == NULL)
    {
        return ESP_FAIL;
    }
    i2c_cmd_handle_t cmd;

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (devAddr << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, regAddr, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdTICKS_TO_MS(1000));
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "PMU i2c_master_cmd_begin FAILED! > ");
        return ESP_FAIL;
    }
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (devAddr << 1) | READ_BIT, ACK_CHECK_EN);
    if (len > 1)
    {
        i2c_master_read(cmd, data, len - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, &data[len - 1], NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdTICKS_TO_MS(1000));
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "PMU READ FAILED! > ");
    }
    return ret == ESP_OK ? 0 : -1;
}


/**
 * @brief Write a byte to a pmu register
 */
int pmu_register_write_byte(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len)
{
    if (data == NULL)
    {
        return ESP_FAIL;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (devAddr << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, regAddr, ACK_CHECK_EN);
    i2c_master_write(cmd, data, len, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdTICKS_TO_MS(1000));
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "PMU WRITE FAILED! < ");
    }
    return ret == ESP_OK ? 0 : -1;
}

static bool example_notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx) {
    lv_disp_drv_t *disp_driver = (lv_disp_drv_t *)user_ctx;
    lv_disp_flush_ready(disp_driver);
    return false;
}

static void example_lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map) {
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)drv->user_data;
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
    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
}

/* Rotate display and touch, when rotated screen in LVGL. Called when driver parameters are updated. */
static void example_lvgl_update_cb(lv_disp_drv_t *drv) {
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)drv->user_data;

    switch (drv->rotated) {
    case LV_DISP_ROT_NONE:
        // Rotate LCD display
        esp_lcd_panel_swap_xy(panel_handle, false);
        esp_lcd_panel_mirror(panel_handle, true, false);
        break;
    case LV_DISP_ROT_90:
        // Rotate LCD display
        esp_lcd_panel_swap_xy(panel_handle, true);
        esp_lcd_panel_mirror(panel_handle, true, true);
        break;
    case LV_DISP_ROT_180:
        // Rotate LCD display
        esp_lcd_panel_swap_xy(panel_handle, false);
        esp_lcd_panel_mirror(panel_handle, false, true);
        break;
    case LV_DISP_ROT_270:
        // Rotate LCD display
        esp_lcd_panel_swap_xy(panel_handle, true);
        esp_lcd_panel_mirror(panel_handle, false, false);
        break;
    }
}

void example_lvgl_rounder_cb(struct _lv_disp_drv_t *disp_drv, lv_area_t *area) {
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
static void example_lvgl_touch_cb(lv_indev_drv_t *drv, lv_indev_data_t *data) {
    esp_lcd_touch_handle_t tp = (esp_lcd_touch_handle_t)drv->user_data;
    assert(tp);

    uint16_t tp_x;
    uint16_t tp_y;
    uint8_t tp_cnt = 0;
    /* Read data from touch controller into memory */
    esp_lcd_touch_read_data(tp);
    /* Read data from touch controller */
    bool tp_pressed = esp_lcd_touch_get_coordinates(tp, &tp_x, &tp_y, NULL, &tp_cnt, 1);
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

static void example_increase_lvgl_tick(void *arg) {
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}

static bool example_lvgl_lock(int timeout_ms) {
    assert(lvgl_mux && "bsp_display_start must be called first");

    const TickType_t timeout_ticks = (timeout_ms == -1) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    return xSemaphoreTake(lvgl_mux, timeout_ticks) == pdTRUE;
}

static void example_lvgl_unlock(void) {
    assert(lvgl_mux && "bsp_display_start must be called first");
    xSemaphoreGive(lvgl_mux);
}

static bool flag = false;

static void toogle_screen(esp_lcd_panel_handle_t handle) {
    if (flag) {
        esp_lcd_panel_disp_on_off(handle, true);
    } else {
        esp_lcd_panel_disp_on_off(handle, false);
    }
    flag = !flag;
}

static void example_lvgl_port_task(void *arg) {
    Utils::PanelParam *handle = (Utils::PanelParam *)arg;
    ESP_LOGI(TAG, "Starting LVGL task");
    uint32_t task_delay_ms = EXAMPLE_LVGL_TASK_MAX_DELAY_MS;
    while (1) {
        // Lock the mutex due to the LVGL APIs are not thread-safe
        if (example_lvgl_lock(-1)) {
            task_delay_ms = lv_timer_handler();
            // Release the mutex
            example_lvgl_unlock();
        }

        uint32_t pin_levels = 0;
        esp_io_expander_get_level(handle->iohandle, IO_EXPANDER_PIN_NUM_4, &pin_levels);
        if (pin_levels) {
            while (true)
            {
                vTaskDelay(pdMS_TO_TICKS(50));
                esp_io_expander_get_level(handle->iohandle, IO_EXPANDER_PIN_NUM_4, &pin_levels);
                if(!pin_levels){
                    toogle_screen(handle->panelhandle);
                    break;
                }
            }
        }

        if (task_delay_ms > EXAMPLE_LVGL_TASK_MAX_DELAY_MS) {
            task_delay_ms = EXAMPLE_LVGL_TASK_MAX_DELAY_MS;
        } else if (task_delay_ms < EXAMPLE_LVGL_TASK_MIN_DELAY_MS) {
            task_delay_ms = EXAMPLE_LVGL_TASK_MIN_DELAY_MS;
        }
        vTaskDelay(pdMS_TO_TICKS(task_delay_ms));
    }
}

#define LCD_OPCODE_WRITE_CMD (0x02ULL)
#define LCD_OPCODE_READ_CMD (0x03ULL)
#define LCD_OPCODE_WRITE_COLOR (0x32ULL)
static esp_err_t tx_param(esp_lcd_panel_io_handle_t io, int lcd_cmd, const void *param, size_t param_size) {
    lcd_cmd &= 0xff;
    lcd_cmd <<= 8;
    lcd_cmd |= LCD_OPCODE_WRITE_CMD << 24;
    return esp_lcd_panel_io_tx_param(io, lcd_cmd, param, param_size);
}

static void pmu_hander_task(void *args)
{
    while (1)
    {
        pmu_isr_handler();
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

extern "C" void app_main(void) {
    esp_log_level_set("lcd_panel.io.i2c", ESP_LOG_NONE);
    esp_log_level_set("FT5x06", ESP_LOG_NONE);
    static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
    static lv_disp_drv_t disp_drv;      // contains callback functions

    ESP_LOGI(TAG, "Initialize I2C bus");
    i2c_config_t i2c_conf;
    i2c_conf.mode = I2C_MODE_MASTER;
    i2c_conf.sda_io_num = EXAMPLE_PIN_NUM_TOUCH_SDA;
    i2c_conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_conf.scl_io_num = EXAMPLE_PIN_NUM_TOUCH_SCL;
    i2c_conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_conf.clk_flags = 0;
    i2c_conf.master.clk_speed = 200 * 1000;

    ESP_ERROR_CHECK(i2c_param_config(TOUCH_HOST, &i2c_conf));
    ESP_ERROR_CHECK(i2c_driver_install(TOUCH_HOST, i2c_conf.mode, 0, 0, 0));

    esp_io_expander_handle_t io_expander = NULL;
    esp_io_expander_new_i2c_tca9554(TOUCH_HOST, ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000, &io_expander);

    ESP_ERROR_CHECK(esp_io_expander_new_i2c_tca9554(TOUCH_HOST, ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000, &io_expander));

    esp_io_expander_set_dir(io_expander, IO_EXPANDER_PIN_NUM_0 | IO_EXPANDER_PIN_NUM_1 | IO_EXPANDER_PIN_NUM_2, IO_EXPANDER_OUTPUT);
    esp_io_expander_set_dir(io_expander, IO_EXPANDER_PIN_NUM_4, IO_EXPANDER_INPUT);
    esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_0, 0);
    esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_1, 0);
    esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_2, 0);
    vTaskDelay(pdMS_TO_TICKS(200));
    esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_0, 1);
    esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_1, 1);
    esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_2, 1);

#if EXAMPLE_PIN_NUM_BK_LIGHT >= 0
    ESP_LOGI(TAG, "Turn off LCD backlight");
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << EXAMPLE_PIN_NUM_BK_LIGHT};
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));
#endif

    ESP_LOGI(TAG, "Initialize SPI bus");
    spi_bus_config_t buscfg;
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
    buscfg.max_transfer_sz = EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES * LCD_BIT_PER_PIXEL / 8;
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

    ESP_LOGI(TAG, "Install panel IO");
    esp_lcd_panel_io_handle_t io_handle = NULL;
    const esp_lcd_panel_io_spi_config_t io_config = SH8601_PANEL_IO_QSPI_CONFIG(EXAMPLE_PIN_NUM_LCD_CS,
                                                                                example_notify_lvgl_flush_ready,
                                                                                &disp_drv);
    sh8601_vendor_config_t vendor_config = {
        .init_cmds = lcd_init_cmds,
        .init_cmds_size = sizeof(lcd_init_cmds) / sizeof(lcd_init_cmds[0]),
        .flags = {
            .use_qspi_interface = 1,
        },
    };
    // Attach the LCD to the SPI bus
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle));

    esp_lcd_panel_handle_t panel_handle = NULL;
    const esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = EXAMPLE_PIN_NUM_LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = LCD_BIT_PER_PIXEL,
        .vendor_config = &vendor_config,
    };
    ESP_LOGI(TAG, "Install SH8601 panel driver");
    ESP_ERROR_CHECK(esp_lcd_new_panel_sh8601(io_handle, &panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    // user can flush pre-defined pattern to the screen before we turn on the screen or backlight
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    uint8_t *cmd = new uint8_t{128};
    tx_param(io_handle, 0x51, cmd, sizeof(cmd));

#if EXAMPLE_USE_TOUCH

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
    ESP_ERROR_CHECK(esp_lcd_touch_new_i2c_ft5x06(tp_io_handle, &tp_cfg, &tp));

#endif

#if EXAMPLE_PIN_NUM_BK_LIGHT >= 0
    ESP_LOGI(TAG, "Turn on LCD backlight");
    gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, EXAMPLE_LCD_BK_LIGHT_ON_LEVEL);
#endif

    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();
    // alloc draw buffers used by LVGL
    // it's recommended to choose the size of the draw buffer(s) to be at least 1/10 screen sized
    void *buf1 = heap_caps_malloc(EXAMPLE_LCD_H_RES * EXAMPLE_LVGL_BUF_HEIGHT * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf1);
    void *buf2 = heap_caps_malloc(EXAMPLE_LCD_H_RES * EXAMPLE_LVGL_BUF_HEIGHT * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf2);
    // initialize LVGL draw buffers
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, EXAMPLE_LCD_H_RES * EXAMPLE_LVGL_BUF_HEIGHT);

    ESP_LOGI(TAG, "Register display driver to LVGL");
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = EXAMPLE_LCD_H_RES;
    disp_drv.ver_res = EXAMPLE_LCD_V_RES;
    disp_drv.flush_cb = example_lvgl_flush_cb;
    disp_drv.rounder_cb = example_lvgl_rounder_cb;
    disp_drv.drv_update_cb = example_lvgl_update_cb;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.user_data = panel_handle;
    lv_disp_t *disp = lv_disp_drv_register(&disp_drv);

    ESP_LOGI(TAG, "Install LVGL tick timer");
    // Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &example_increase_lvgl_tick,
        .name = "lvgl_tick"};
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000));

#if EXAMPLE_USE_TOUCH
    static lv_indev_drv_t indev_drv; // Input device driver (Touch)
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.disp = disp;
    indev_drv.read_cb = example_lvgl_touch_cb;
    indev_drv.user_data = tp;
    lv_indev_drv_register(&indev_drv);
#endif

    lvgl_mux = xSemaphoreCreateMutex();
    assert(lvgl_mux);
    static Utils::PanelParam param;
    param.iohandle = io_expander;
    param.panelhandle = panel_handle;
    xTaskCreate(example_lvgl_port_task, "LVGL", EXAMPLE_LVGL_TASK_STACK_SIZE, &param, EXAMPLE_LVGL_TASK_PRIORITY, NULL);
    // create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(5, sizeof(uint32_t));

    // Register PMU interrupt pins
    // irq_init();

    // ESP_ERROR_CHECK(i2c_init(i2c_conf));

    ESP_LOGI(TAG, "I2C initialized successfully");

    ESP_ERROR_CHECK(pmu_init());

    xTaskCreate(pmu_hander_task, "App/pwr", 4 * 1024, NULL, 2, NULL);
    // Lock the mutex due to the LVGL APIs are not thread-safe
    if (example_lvgl_lock(-1)) {

        // lv_demo_widgets(); /* A widgets example */
        // lv_demo_music(); /* A modern, smartphone-like music player demo. */
        // lv_demo_stress();       /* A stress test for LVGL. */
        // lv_demo_benchmark(); /* A demo to measure the performance of LVGL or to compare different settings. */
        auto screen = lv_scr_act();
        lv_obj_t* lottie = lv_rlottie_create_from_raw(screen,200,200,lottiedata);
        lv_obj_center(lottie);
        
        lv_obj_set_style_bg_color(screen, LV_COLOR_MAKE(0, 0, 0), LV_STATE_DEFAULT);
        // Release the mutex
        example_lvgl_unlock();
    }
}