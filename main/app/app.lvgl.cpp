#include "app.h"

static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map) {
    auto panel_handle = (devicemanager *)drv->user_data;
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
    auto panel_handle = (devicemanager *)drv->user_data;

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
    auto tp = (devicemanager *)drv->user_data;
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
        // ESP_LOGI(TAG, "Touch position: %d,%d", tp_x, tp_y);
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}
#endif

static void increase_lvgl_tick(void *arg) {
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(LVGL_TICK_PERIOD_MS);
}

static bool lvgl_port_flush_io_ready_callback(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
    lv_disp_drv_t *disp_drv = (lv_disp_drv_t *)user_ctx;
    assert(disp_drv != NULL);
    lv_disp_flush_ready(disp_drv);
    return false;
}

void myapp::init_framework() {

    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();

    void *buf1 = heap_caps_malloc(SCREEN_H_RES * LVGL_BUF_HEIGHT * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    assert(buf1);
    // void *buf2 = heap_caps_malloc(SCREEN_H_RES * LVGL_BUF_HEIGHT * sizeof(lv_color_t), MALLOC_CAP_DMA);
    // assert(buf2);

    lv_disp_draw_buf_init(&disp_buf, buf1, NULL, SCREEN_H_RES * LVGL_BUF_HEIGHT);

    ESP_LOGI(TAG, "Register display driver to LVGL");
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = SCREEN_H_RES;
    disp_drv.ver_res = SCREEN_V_RES;
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
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, LVGL_TICK_PERIOD_MS * 1000));

    const esp_lcd_panel_io_callbacks_t cbs = {
        .on_color_trans_done = lvgl_port_flush_io_ready_callback,
    };
    esp_lcd_panel_io_register_event_callbacks(device_manager->screen_spi_handle, &cbs, &disp_drv);
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