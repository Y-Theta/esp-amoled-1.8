#include "common.h"
#include "app/app.h"

static myapp *mainapp;

static void example_lvgl_port_task(void *arg) {
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

static bool flag = false;
static void toogle_screen(myapp *manager) {
    if (flag) {
        esp_lcd_panel_disp_on_off(manager->disp_manager->screen_handle, true);
        // esp_lcd_touch_exit_sleep(manager->touch_handle);
        manager->power_manager->wakeup();
    } else {
        esp_lcd_panel_disp_on_off(manager->disp_manager->screen_handle, false);
        // esp_lcd_touch_enter_sleep(manager->touch_handle);
        manager->power_manager->sleep();
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

static void on_power_update(powermanager *manager) {
    auto app = (myapp*) manager->power_cb_userdata;
    if (example_lvgl_lock(-1)) {
        app->update_battery_status(manager);
        example_lvgl_unlock();
    }
}

static void on_boot_click(myapp* app){
    toogle_cpu();
}

static void on_pwr_click(myapp* app){
    toogle_screen(app);
}

extern "C" void app_main(void) {
    esp_log_level_set("lcd_panel.io.i2c", ESP_LOG_NONE);
    esp_log_level_set("FT5x06", ESP_LOG_NONE);

    mainapp = new myapp();
    lvgl_mux = xSemaphoreCreateMutex();
    assert(lvgl_mux);
    mainapp->power_manager->power_cb_userdata = mainapp;
    mainapp->power_manager->power_cb = on_power_update;
    mainapp->boot_click = on_boot_click;
    mainapp->pwr_click = on_pwr_click;
    mainapp->init();

    // xTaskCreatePinnedToCore(task_btn, "app/btn", 5 * 1024, mainapp, 2, NULL, 1);
    xTaskCreatePinnedToCore(example_lvgl_port_task, "LVGL", LVGL_TASK_STACK_SIZE, mainapp, LVGL_TASK_PRIORITY, NULL, 1);
    // Lock the mutex due to the LVGL APIs are not thread-safe
    if (example_lvgl_lock(-1)) {
        mainapp->init_ui_elements();
        // Release the mutex
        example_lvgl_unlock();
    }
}