#include "app/app.h"
#include "common.hpp"
#include "utils/dispmanager.h"
#include "utils/powermanager.h"
#include "utils/wifimanager.h"
#include "utils/fsmanager.h"

static powermanager *power_manager;
static dispmanager *disp_manager;
static wifimanager *wifi_manager;
static fsmanager *fs_manager;
static myapp *mainapp;

static void example_lvgl_port_task(void *arg) {
    auto handle = (dispmanager *)arg;
    ESP_LOGI(TAG, "Starting LVGL task");
    uint32_t task_delay_ms = LVGL_TASK_MAX_DELAY_MS;
    while (1) {
        // Lock the mutex due to the LVGL APIs are not thread-safe
        if (example_lvgl_lock(-1)) {
            task_delay_ms = lv_timer_handler();
            // Release the mutex
            example_lvgl_unlock();
        }

        if (task_delay_ms > LVGL_TASK_MAX_DELAY_MS) {
            task_delay_ms = LVGL_TASK_MAX_DELAY_MS;
        } else if (task_delay_ms < LVGL_TASK_MIN_DELAY_MS) {
            task_delay_ms = LVGL_TASK_MIN_DELAY_MS;
        }
        vTaskDelay(pdMS_TO_TICKS(task_delay_ms));
    }
}

static void on_power_update(powermanager *manager) {
    if (example_lvgl_lock(-1)) {
        mainapp->update_battery_status(manager);
        example_lvgl_unlock();
    }
}

static bool flag = false;
static void toogle_screen(dispmanager *manager) {
    if (flag) {
        esp_lcd_panel_disp_on_off(manager->screen_handle, true);
        // esp_lcd_touch_exit_sleep(manager->touch_handle);
        power_manager->wakeup();
    } else {
        esp_lcd_panel_disp_on_off(manager->screen_handle, false);
        // esp_lcd_touch_enter_sleep(manager->touch_handle);
        power_manager->sleep();
    }
    flag = !flag;
}

static bool flag1 = false;
static void toogle_cpu() {
    if (example_lvgl_lock(10)) {
        if (flag1) {
            mainapp->pause_ani();
        } else {
            mainapp->resume_ani();
        }
        flag1 = !flag1;
        example_lvgl_unlock();
    }
}

static void task_btn(void *param) {
    auto handle = (dispmanager *)param;
    while (true) {
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
        if (!level) {
            while (true) {
                vTaskDelay(pdMS_TO_TICKS(50));
                level = gpio_get_level(GPIO_NUM_0);
                if (level) {
                    ESP_LOGI(TAG, "BOOT Click");
                    toogle_cpu();
                    break;
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

extern "C" void app_main(void) {
    esp_log_level_set("lcd_panel.io.i2c", ESP_LOG_NONE);
    esp_log_level_set("FT5x06", ESP_LOG_NONE);

    vTaskDelay(pdMS_TO_TICKS(4000));
    ESP_LOGI(TAG, "free heap: %d,\nfree internal: %d ", esp_get_free_heap_size(), esp_get_free_internal_heap_size());

    mainapp = new myapp();
    fs_manager = new fsmanager();
    power_manager = new powermanager();
    wifi_manager = new wifimanager();
    disp_manager = new dispmanager();

    lvgl_mux = xSemaphoreCreateMutex();
    assert(lvgl_mux);

    disp_manager->init_i2c();
    power_manager->init();
    power_manager->power_cb = on_power_update;
    disp_manager->init_io_expander();
    disp_manager->init_screen();
    disp_manager->set_brightness(64);
    disp_manager->init_touch();
    disp_manager->framework_init();
    power_manager->start_power_monitor();

    xTaskCreatePinnedToCore(task_btn, "app/btn", 5 * 1024, disp_manager, 2, NULL, 1);
    xTaskCreatePinnedToCore(example_lvgl_port_task, "LVGL", LVGL_TASK_STACK_SIZE, disp_manager, LVGL_TASK_PRIORITY, NULL, 1);
    // Lock the mutex due to the LVGL APIs are not thread-safe
    if (example_lvgl_lock(-1)) {
        mainapp->init_ui_elements();
        // Release the mutex
        example_lvgl_unlock();
    }
}