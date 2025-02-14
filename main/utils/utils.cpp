#include "utils.h"

utils::utils() {
    lvgl_mux = xSemaphoreCreateMutex();
}

utils::~utils() {
}

bool utils::lvgl_lock(int timeout_ms) {
    assert(lvgl_mux && "bsp_display_start must be called first");
    const TickType_t timeout_ticks = (timeout_ms == -1) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    return xSemaphoreTake(lvgl_mux, timeout_ticks) == pdTRUE;
}

void utils::lvgl_unlock() {
    assert(lvgl_mux && "bsp_display_start must be called first");
    xSemaphoreGive(lvgl_mux);
}