#include "common.h"
#include "utils/dispmanager.h"
#include "utils/powermanager.h"

static powermanager *power_manager;
static dispmanager *disp_manager;

static SemaphoreHandle_t lvgl_mux = NULL;

static void pmu_hander_task(void *);
static QueueHandle_t gpio_evt_queue = NULL;
static lv_obj_t *batteryLabel = NULL;

static lv_style_t style_bat;

const char lottiedata[] = "{\"v\":\"5.9.4\",\"fr\":29.9700012207031,\"ip\":0,\"op\":84.0000034213901,\"w\":1000,\"h\":1000,\"nm\":\"Gif 2\",\"ddd\":0,\"assets\":[],\"layers\":[{\"ddd\":0,\"ind\":1,\"ty\":4,\"nm\":\"Shape Layer 3\",\"sr\":1,\"ks\":{\"o\":{\"a\":0,\"k\":100,\"ix\":11},\"r\":{\"a\":0,\"k\":0,\"ix\":10},\"p\":{\"a\":0,\"k\":[512,518,0],\"ix\":2,\"l\":2},\"a\":{\"a\":0,\"k\":[0,0,0],\"ix\":1,\"l\":2},\"s\":{\"a\":0,\"k\":[100,100,100],\"ix\":6,\"l\":2}},\"ao\":0,\"shapes\":[{\"ty\":\"gr\",\"it\":[{\"ind\":0,\"ty\":\"sh\",\"ix\":1,\"ks\":{\"a\":0,\"k\":{\"i\":[[0,0],[0,0],[0,0]],\"o\":[[0,0],[0,0],[0,0]],\"v\":[[-168,-36],[-38,44],[102,-105]],\"c\":false},\"ix\":2},\"nm\":\"Path 1\",\"mn\":\"ADBE Vector Shape - Group\",\"hd\":false},{\"ty\":\"st\",\"c\":{\"a\":0,\"k\":[1,1,1,1],\"ix\":3},\"o\":{\"a\":0,\"k\":100,\"ix\":4},\"w\":{\"a\":0,\"k\":66,\"ix\":5},\"lc\":2,\"lj\":2,\"bm\":0,\"nm\":\"Stroke 1\",\"mn\":\"ADBE Vector Graphic - Stroke\",\"hd\":false},{\"ty\":\"tr\",\"p\":{\"a\":0,\"k\":[0,0],\"ix\":2},\"a\":{\"a\":0,\"k\":[0,0],\"ix\":1},\"s\":{\"a\":0,\"k\":[100,100],\"ix\":3},\"r\":{\"a\":0,\"k\":0,\"ix\":6},\"o\":{\"a\":0,\"k\":100,\"ix\":7},\"sk\":{\"a\":0,\"k\":0,\"ix\":4},\"sa\":{\"a\":0,\"k\":0,\"ix\":5},\"nm\":\"Transform\"}],\"nm\":\"Shape 1\",\"np\":3,\"cix\":2,\"bm\":0,\"ix\":1,\"mn\":\"ADBE Vector Group\",\"hd\":false},{\"ty\":\"tm\",\"s\":{\"a\":1,\"k\":[{\"i\":{\"x\":[0.833],\"y\":[1]},\"o\":{\"x\":[0.333],\"y\":[0]},\"t\":37,\"s\":[0]},{\"t\":56.0000022809268,\"s\":[14]}],\"ix\":1},\"e\":{\"a\":1,\"k\":[{\"i\":{\"x\":[0.667],\"y\":[1]},\"o\":{\"x\":[0.333],\"y\":[0]},\"t\":27,\"s\":[0]},{\"t\":54.0000021994651,\"s\":[100]}],\"ix\":2},\"o\":{\"a\":0,\"k\":0,\"ix\":3},\"m\":1,\"ix\":2,\"nm\":\"Trim Paths 1\",\"mn\":\"ADBE Vector Filter - Trim\",\"hd\":false}],\"ip\":27.0000010997325,\"op\":893.000036372636,\"st\":-7.00000028511585,\"ct\":1,\"bm\":0},{\"ddd\":0,\"ind\":2,\"ty\":4,\"nm\":\"Shape Layer 1\",\"sr\":1,\"ks\":{\"o\":{\"a\":0,\"k\":100,\"ix\":11},\"r\":{\"a\":0,\"k\":-91,\"ix\":10},\"p\":{\"a\":0,\"k\":[497,488,0],\"ix\":2,\"l\":2},\"a\":{\"a\":0,\"k\":[0,0,0],\"ix\":1,\"l\":2},\"s\":{\"a\":0,\"k\":[100,100,100],\"ix\":6,\"l\":2}},\"ao\":0,\"shapes\":[{\"ty\":\"gr\",\"it\":[{\"ind\":0,\"ty\":\"sh\",\"ix\":1,\"ks\":{\"a\":0,\"k\":{\"i\":[[88.347,0],[0,-88.347],[-88.347,0],[0,88.347]],\"o\":[[-88.347,0],[0,88.347],[88.347,0],[0,-88.347]],\"v\":[[-105,-240.967],[-264.967,-81],[-105,78.967],[54.967,-81]],\"c\":true},\"ix\":2},\"nm\":\"Path 1\",\"mn\":\"ADBE Vector Shape - Group\",\"hd\":false},{\"ty\":\"st\",\"c\":{\"a\":0,\"k\":[1,1,1,1],\"ix\":3},\"o\":{\"a\":0,\"k\":100,\"ix\":4},\"w\":{\"a\":0,\"k\":59,\"ix\":5},\"lc\":2,\"lj\":1,\"ml\":4,\"bm\":0,\"nm\":\"Stroke 1\",\"mn\":\"ADBE Vector Graphic - Stroke\",\"hd\":false},{\"ty\":\"tr\",\"p\":{\"a\":0,\"k\":[115.967,82.967],\"ix\":2},\"a\":{\"a\":0,\"k\":[0,0],\"ix\":1},\"s\":{\"a\":0,\"k\":[100,100],\"ix\":3},\"r\":{\"a\":0,\"k\":0,\"ix\":6},\"o\":{\"a\":0,\"k\":100,\"ix\":7},\"sk\":{\"a\":0,\"k\":0,\"ix\":4},\"sa\":{\"a\":0,\"k\":0,\"ix\":5},\"nm\":\"Transform\"}],\"nm\":\"Ellipse 1\",\"np\":3,\"cix\":2,\"bm\":0,\"ix\":1,\"mn\":\"ADBE Vector Group\",\"hd\":false},{\"ty\":\"tm\",\"s\":{\"a\":1,\"k\":[{\"i\":{\"x\":[0.667],\"y\":[1]},\"o\":{\"x\":[0.333],\"y\":[0]},\"t\":0,\"s\":[100]},{\"t\":27.0000010997325,\"s\":[0]}],\"ix\":1},\"e\":{\"a\":1,\"k\":[{\"i\":{\"x\":[0.667],\"y\":[1]},\"o\":{\"x\":[0.333],\"y\":[0]},\"t\":3,\"s\":[100]},{\"t\":35.0000014255792,\"s\":[0]}],\"ix\":2},\"o\":{\"a\":0,\"k\":0,\"ix\":3},\"m\":1,\"ix\":2,\"nm\":\"Trim Paths 1\",\"mn\":\"ADBE Vector Filter - Trim\",\"hd\":false}],\"ip\":0,\"op\":900.000036657751,\"st\":0,\"ct\":1,\"bm\":0},{\"ddd\":0,\"ind\":3,\"ty\":4,\"nm\":\"Shape Layer 4\",\"sr\":1,\"ks\":{\"o\":{\"a\":0,\"k\":100,\"ix\":11},\"r\":{\"a\":0,\"k\":-91,\"ix\":10},\"p\":{\"a\":0,\"k\":[497,488,0],\"ix\":2,\"l\":2},\"a\":{\"a\":0,\"k\":[0,0,0],\"ix\":1,\"l\":2},\"s\":{\"a\":0,\"k\":[100,100,100],\"ix\":6,\"l\":2}},\"ao\":0,\"shapes\":[{\"ty\":\"gr\",\"it\":[{\"ind\":0,\"ty\":\"sh\",\"ix\":1,\"ks\":{\"a\":0,\"k\":{\"i\":[[88.347,0],[0,-88.347],[-88.347,0],[0,88.347]],\"o\":[[-88.347,0],[0,88.347],[88.347,0],[0,-88.347]],\"v\":[[-105,-240.967],[-264.967,-81],[-105,78.967],[54.967,-81]],\"c\":true},\"ix\":2},\"nm\":\"Path 1\",\"mn\":\"ADBE Vector Shape - Group\",\"hd\":false},{\"ty\":\"st\",\"c\":{\"a\":0,\"k\":[0.301960784314,0.937254961799,0.188235309077,1],\"ix\":3},\"o\":{\"a\":0,\"k\":100,\"ix\":4},\"w\":{\"a\":0,\"k\":82,\"ix\":5},\"lc\":2,\"lj\":1,\"ml\":4,\"bm\":0,\"nm\":\"Stroke 1\",\"mn\":\"ADBE Vector Graphic - Stroke\",\"hd\":false},{\"ty\":\"fl\",\"c\":{\"a\":0,\"k\":[0.302594951555,0.935661764706,0.19014229868,1],\"ix\":4},\"o\":{\"a\":0,\"k\":100,\"ix\":5},\"r\":1,\"bm\":0,\"nm\":\"Fill 1\",\"mn\":\"ADBE Vector Graphic - Fill\",\"hd\":false},{\"ty\":\"tr\",\"p\":{\"a\":0,\"k\":[115.967,82.967],\"ix\":2},\"a\":{\"a\":0,\"k\":[0,0],\"ix\":1},\"s\":{\"a\":0,\"k\":[100,100],\"ix\":3},\"r\":{\"a\":0,\"k\":0,\"ix\":6},\"o\":{\"a\":0,\"k\":100,\"ix\":7},\"sk\":{\"a\":0,\"k\":0,\"ix\":4},\"sa\":{\"a\":0,\"k\":0,\"ix\":5},\"nm\":\"Transform\"}],\"nm\":\"Ellipse 1\",\"np\":3,\"cix\":2,\"bm\":0,\"ix\":1,\"mn\":\"ADBE Vector Group\",\"hd\":false},{\"ty\":\"tm\",\"s\":{\"a\":1,\"k\":[{\"i\":{\"x\":[0.667],\"y\":[1]},\"o\":{\"x\":[0.333],\"y\":[0]},\"t\":0,\"s\":[100]},{\"t\":28.0000011404634,\"s\":[0]}],\"ix\":1},\"e\":{\"a\":0,\"k\":100,\"ix\":2},\"o\":{\"a\":0,\"k\":0,\"ix\":3},\"m\":1,\"ix\":2,\"nm\":\"Trim Paths 1\",\"mn\":\"ADBE Vector Filter - Trim\",\"hd\":false}],\"ip\":29.0000011811942,\"op\":900.000036657751,\"st\":0,\"ct\":1,\"bm\":0},{\"ddd\":0,\"ind\":4,\"ty\":4,\"nm\":\"Shape Layer 2\",\"sr\":1,\"ks\":{\"o\":{\"a\":0,\"k\":100,\"ix\":11},\"r\":{\"a\":0,\"k\":-91,\"ix\":10},\"p\":{\"a\":0,\"k\":[497,488,0],\"ix\":2,\"l\":2},\"a\":{\"a\":0,\"k\":[0,0,0],\"ix\":1,\"l\":2},\"s\":{\"a\":0,\"k\":[100,100,100],\"ix\":6,\"l\":2}},\"ao\":0,\"shapes\":[{\"ty\":\"gr\",\"it\":[{\"ind\":0,\"ty\":\"sh\",\"ix\":1,\"ks\":{\"a\":0,\"k\":{\"i\":[[88.347,0],[0,-88.347],[-88.347,0],[0,88.347]],\"o\":[[-88.347,0],[0,88.347],[88.347,0],[0,-88.347]],\"v\":[[-105,-240.967],[-264.967,-81],[-105,78.967],[54.967,-81]],\"c\":true},\"ix\":2},\"nm\":\"Path 1\",\"mn\":\"ADBE Vector Shape - Group\",\"hd\":false},{\"ty\":\"st\",\"c\":{\"a\":0,\"k\":[0.301960784314,0.937254961799,0.188235309077,1],\"ix\":3},\"o\":{\"a\":0,\"k\":100,\"ix\":4},\"w\":{\"a\":0,\"k\":82,\"ix\":5},\"lc\":2,\"lj\":1,\"ml\":4,\"bm\":0,\"nm\":\"Stroke 1\",\"mn\":\"ADBE Vector Graphic - Stroke\",\"hd\":false},{\"ty\":\"tr\",\"p\":{\"a\":0,\"k\":[115.967,82.967],\"ix\":2},\"a\":{\"a\":0,\"k\":[0,0],\"ix\":1},\"s\":{\"a\":0,\"k\":[100,100],\"ix\":3},\"r\":{\"a\":0,\"k\":0,\"ix\":6},\"o\":{\"a\":0,\"k\":100,\"ix\":7},\"sk\":{\"a\":0,\"k\":0,\"ix\":4},\"sa\":{\"a\":0,\"k\":0,\"ix\":5},\"nm\":\"Transform\"}],\"nm\":\"Ellipse 1\",\"np\":3,\"cix\":2,\"bm\":0,\"ix\":1,\"mn\":\"ADBE Vector Group\",\"hd\":false},{\"ty\":\"tm\",\"s\":{\"a\":1,\"k\":[{\"i\":{\"x\":[0.667],\"y\":[1]},\"o\":{\"x\":[0.333],\"y\":[0]},\"t\":0,\"s\":[100]},{\"t\":28.0000011404634,\"s\":[0]}],\"ix\":1},\"e\":{\"a\":0,\"k\":100,\"ix\":2},\"o\":{\"a\":0,\"k\":0,\"ix\":3},\"m\":1,\"ix\":2,\"nm\":\"Trim Paths 1\",\"mn\":\"ADBE Vector Filter - Trim\",\"hd\":false}],\"ip\":0,\"op\":29.0000011811942,\"st\":0,\"ct\":1,\"bm\":0}],\"markers\":[]}";

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
            while (true) {
                vTaskDelay(pdMS_TO_TICKS(50));
                esp_io_expander_get_level(handle->iohandle, IO_EXPANDER_PIN_NUM_4, &pin_levels);
                if (!pin_levels) {
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

static void pmu_hander_task(void *args) {
    char post[6];
    while (1) {
        // pmu_isr_handler();
        if (batteryLabel) {
            auto p = power_manager->get_percent();
            auto ischarge = power_manager->is_charging();
            snprintf(post, sizeof(post),"%d %%",p);
            lv_label_set_text(batteryLabel, post);
            if (ischarge) {
                lv_obj_set_style_text_color(batteryLabel, lv_color_hex(0x00ff22), 0);
            } else {
                lv_obj_set_style_text_color(batteryLabel, lv_color_hex(0xffffff), 0);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

extern "C" void app_main(void) {
    esp_log_level_set("lcd_panel.io.i2c", ESP_LOG_NONE);
    esp_log_level_set("FT5x06", ESP_LOG_NONE);
    static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
    static lv_disp_drv_t disp_drv;      // contains callback functions
    power_manager = new powermanager();
    disp_manager = new dispmanager();

    disp_manager->init_i2c();
    power_manager->init();
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

    lvgl_mux = xSemaphoreCreateMutex();
    assert(lvgl_mux);
    static Utils::PanelParam param;
    param.iohandle = disp_manager->io_expander_handle;
    param.panelhandle = disp_manager->screen_handle;
    xTaskCreate(example_lvgl_port_task, "LVGL", EXAMPLE_LVGL_TASK_STACK_SIZE, &param, EXAMPLE_LVGL_TASK_PRIORITY, NULL);
    xTaskCreate(pmu_hander_task, "App/pwr", 4 * 1024, NULL, 2, NULL);
    // Lock the mutex due to the LVGL APIs are not thread-safe
    if (example_lvgl_lock(-1)) {

        // lv_demo_widgets(); /* A widgets example */
        // lv_demo_music(); /* A modern, smartphone-like music player demo. */
        // lv_demo_stress();       /* A stress test for LVGL. */
        // lv_demo_benchmark(); /* A demo to measure the performance of LVGL or to compare different settings. */
        auto screen = lv_scr_act();
        lv_obj_t *lottie = lv_rlottie_create_from_raw(screen, 160, 160, lottiedata);
        lv_obj_t *label = lv_label_create(screen);

        lv_style_init(&style_bat);
        lv_style_set_text_font(&style_bat, &lv_font_montserrat_14);
        lv_style_set_text_color(&style_bat, lv_color_hex(0xffffff));
        lv_label_set_text(label, "BAT");
        lv_obj_add_style(label, &style_bat, 0);

        batteryLabel = label;
        lv_obj_align(label, LV_ALIGN_TOP_RIGHT, -20, 20);
        lv_obj_center(lottie);

        lv_obj_set_style_bg_color(screen, LV_COLOR_MAKE(0, 0, 0), LV_STATE_DEFAULT);
        // Release the mutex
        example_lvgl_unlock();
    }
}