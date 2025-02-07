#include "common.h"
#include "utils/dispmanager.h"
#include "utils/powermanager.h"
#include "app/app.h"

static powermanager *power_manager;
static dispmanager *disp_manager;
static myapp* mainapp;
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

    mainapp = new myapp();
    power_manager = new powermanager();
    disp_manager = new dispmanager(power_manager,mainapp);

    disp_manager->init_i2c();
    power_manager->init();
    power_manager->power_cb = on_power_update;
    disp_manager->init_io_expander();
    disp_manager->init_screen();
    disp_manager->set_brightness(128);
    disp_manager->init_touch();
    disp_manager->framework_init();

    lvgl_mux = xSemaphoreCreateMutex();
    assert(lvgl_mux);
    xTaskCreate(example_lvgl_port_task, "LVGL", EXAMPLE_LVGL_TASK_STACK_SIZE, disp_manager, EXAMPLE_LVGL_TASK_PRIORITY, NULL);
    // Lock the mutex due to the LVGL APIs are not thread-safe
    if (example_lvgl_lock(-1)) {
        mainapp->init_ui_elements();
        power_manager->start_power_monitor();
        // Release the mutex
        example_lvgl_unlock();
    }
}