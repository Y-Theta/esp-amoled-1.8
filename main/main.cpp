#include "common.h"
#include "utils/dispmanager.h"
#include "utils/powermanager.h"
#include "app/app.h"

static powermanager *power_manager;
static dispmanager *disp_manager;
static app* mainapp;
static SemaphoreHandle_t lvgl_mux = NULL;


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
    auto handle = (dispmanager *)arg;
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
        esp_io_expander_get_level(handle->io_expander_handle, IO_EXPANDER_PIN_NUM_4, &pin_levels);
        if (pin_levels) {
            while (true) {
                vTaskDelay(pdMS_TO_TICKS(50));
                esp_io_expander_get_level(handle->io_expander_handle, IO_EXPANDER_PIN_NUM_4, &pin_levels);
                if (!pin_levels) {
                    toogle_screen(handle->screen_handle);
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

static void on_power_update(powermanager* manager){
    mainapp->update_battery_status(manager);
}

extern "C" void app_main(void) {
    esp_log_level_set("lcd_panel.io.i2c", ESP_LOG_NONE);
    esp_log_level_set("FT5x06", ESP_LOG_NONE);

    mainapp = new app();
    power_manager = new powermanager();
    disp_manager = new dispmanager();

    disp_manager->init_i2c();

    power_manager->init();
    power_manager->power_cb = on_power_update;

    disp_manager->init_io_expander();

#if EXAMPLE_PIN_NUM_BK_LIGHT >= 0
    ESP_LOGI(TAG, "Turn off LCD backlight");
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << EXAMPLE_PIN_NUM_BK_LIGHT};
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));
#endif
    disp_manager->init_screen();
    disp_manager->set_brightness(128);

#if EXAMPLE_USE_TOUCH
    disp_manager->init_touch();
#endif

#if EXAMPLE_PIN_NUM_BK_LIGHT >= 0
    ESP_LOGI(TAG, "Turn on LCD backlight");
    gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, EXAMPLE_LCD_BK_LIGHT_ON_LEVEL);
#endif
    disp_manager->framework_init();
    power_manager->start_power_monitor();

    lvgl_mux = xSemaphoreCreateMutex();
    assert(lvgl_mux);
    xTaskCreate(example_lvgl_port_task, "LVGL", EXAMPLE_LVGL_TASK_STACK_SIZE,disp_manager, EXAMPLE_LVGL_TASK_PRIORITY, NULL);
    // Lock the mutex due to the LVGL APIs are not thread-safe
    if (example_lvgl_lock(-1)) {
        mainapp->init_ui_elements();
        // Release the mutex
        example_lvgl_unlock();
    }
}