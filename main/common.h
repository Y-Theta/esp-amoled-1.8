#pragma once

#ifndef USE_COMMON
#define USE_COMMON

#ifndef USE_STYLE
#include "style.h"
#endif

#include <map>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "cJSON.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_flash.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "spi_flash_mmap.h"

#include "button_adc.h"
#include "button_gpio.h"
#include "esp_http_server.h"
#include "iot_button.h"
#include "nvs_flash.h"

#include "esp_lcd_sh8601.h"
#include "esp_lcd_touch_ft5x06.h"
#include "esp_littlefs.h"
#include "lv_lottie.h"
#include "lvgl.h"
#include "thorvg_capi.h"

#include "esp_io_expander_tca9554.h"
#include "functional"

static const char *TAG = "AIChat";

#define PMU_INPUT_PIN (gpio_num_t) CONFIG_PMU_INTERRUPT_PIN /*!< axp power chip interrupt Pin*/
#define PMU_INPUT_PIN_SEL (1ULL << PMU_INPUT_PIN)

#define WIFI_SCAN_LIST_SIZE 8

#define I2C_MASTER_NUM (i2c_port_t) I2C_NUM_0
#define I2C_MASTER_FREQ_HZ CONFIG_I2C_MASTER_FREQUENCY /*!< I2C master clock frequency */
#define I2C_MASTER_SDA_IO (gpio_num_t) CONFIG_PMU_I2C_SDA
#define I2C_MASTER_SCL_IO (gpio_num_t) CONFIG_PMU_I2C_SCL

#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS 1000

#define LCD_HOST SPI3_HOST
#define TOUCH_HOST I2C_NUM_0
#define LCD_BIT_PER_PIXEL (16)

#define WRITE_BIT I2C_MASTER_WRITE   /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ     /*!< I2C master read */
#define ACK_CHECK_EN 0x1             /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0            /*!< I2C master will not check ack from slave */
#define ACK_VAL (i2c_ack_type_t)0x0  /*!< I2C ack value */
#define NACK_VAL (i2c_ack_type_t)0x1 /*!< I2C nack value */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your LCD spec //////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SCREEN_BK_LIGHT_ON_LEVEL 1
#define SCREEN_BK_LIGHT_OFF_LEVEL !SCREEN_BK_LIGHT_ON_LEVEL
#define PIN_NUM_LCD_CS (GPIO_NUM_12)
#define PIN_NUM_LCD_PCLK (GPIO_NUM_11)
#define PIN_NUM_LCD_DATA0 (GPIO_NUM_4)
#define PIN_NUM_LCD_DATA1 (GPIO_NUM_5)
#define PIN_NUM_LCD_DATA2 (GPIO_NUM_6)
#define PIN_NUM_LCD_DATA3 (GPIO_NUM_7)
#define PIN_NUM_LCD_RST (-1)
#define PIN_NUM_BK_LIGHT (-1)

#define PIN_NUM_BOOT 0

// The pixel number in horizontal and vertical

#define EXAMPLE_USE_TOUCH 1

#if EXAMPLE_USE_TOUCH
#define PIN_NUM_TOUCH_SCL (GPIO_NUM_14)
#define PIN_NUM_TOUCH_SDA (GPIO_NUM_15)
#define PIN_NUM_TOUCH_RST ((gpio_num_t) - 1)
#define PIN_NUM_TOUCH_INT (GPIO_NUM_21)

#endif

#define LVGL_BUF_HEIGHT (SCREEN_V_RES / 8)
#define LVGL_TICK_PERIOD_MS 2
#define LVGL_TASK_MAX_DELAY_MS 384
#define LVGL_TASK_MIN_DELAY_MS 25
#define LVGL_TASK_STACK_SIZE (48 * 1024)
#define LVGL_TASK_PRIORITY 2

typedef void (*gpio_isr_t)(void *arg);

// static volatile SemaphoreHandle_t lvgl_mux = NULL;

// static bool lvgl_lock(int timeout_ms) {
//     assert(lvgl_mux && "bsp_display_start must be called first");
//     const TickType_t timeout_ticks = (timeout_ms == -1) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
//     return xSemaphoreTake(lvgl_mux, timeout_ticks) == pdTRUE;
// }

// static void lvgl_unlock(void) {
//     assert(lvgl_mux && "bsp_display_start must be called first");
//     xSemaphoreGive(lvgl_mux);
// }

static void set_text_style(lv_obj_t *obj) {
    lv_obj_set_style_text_color(obj, lv_color_hex(WIN_WIFI_LIST_TITLE_FG), 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(WIN_WIFI_LIST_TITLE_BG), 0);
}

namespace COMMON {
typedef struct {
    button_driver_t base;
    int32_t gpio_num;     /**< num of gpio */
    uint8_t active_level; /**< gpio level when press down */
} custom_gpio_obj;

typedef struct {
    char *wifi_ssid;
    char *wifi_pass;

    char *server_url;
    char *api_key_token;
} global_config;

typedef struct {
    bool is_wifi_connect = 0;

} main_view_model;

typedef struct {
    int32_t size;
    const uint8_t *buf;
} assets_info_t;

typedef struct {

} image_btn_info_t;

typedef enum {
    connected = 0,
    ap_started = 1,
} wifi_connect_result;

typedef struct {
    uint8_t bssid[6];          /**< MAC address of AP */
    uint8_t ssid[33];          /**< SSID of AP */
    uint8_t primary;           /**< Channel of AP */
    wifi_second_chan_t second; /**< Secondary channel of AP */
    int8_t rssi;
} wifi_ap_info;

typedef struct {
    wifi_ap_record_t *infos;
    int count;
} wifi_scan_result;

struct SsidItem {
    char *ssid;
    char *password;
};

} // namespace COMMON

#endif