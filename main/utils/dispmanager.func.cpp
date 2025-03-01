#include "dispmanager.h"

#define LCD_OPCODE_WRITE_CMD (0x02ULL)
#define LCD_OPCODE_READ_CMD (0x03ULL)
#define LCD_OPCODE_WRITE_COLOR (0x32ULL)
static esp_err_t tx_param(esp_lcd_panel_io_handle_t io, int lcd_cmd, const void *param, size_t param_size) {
    lcd_cmd &= 0xff;
    lcd_cmd <<= 8;
    lcd_cmd |= LCD_OPCODE_WRITE_CMD << 24;
    return esp_lcd_panel_io_tx_param(io, lcd_cmd, param, param_size);
}

void dispmanager::set_brightness(int bright) {
    uint8_t *cmd = new uint8_t[]{(uint8_t)bright};
    tx_param(screen_spi_handle, 0x51, cmd, sizeof(cmd));
}

int dispmanager::pwr_click() {
    uint32_t pin_levels = 0;
    int count = 1;
    esp_io_expander_get_level(io_expander_handle, IO_EXPANDER_PIN_NUM_4, &pin_levels);
    if (pin_levels) {
        while (true) {
            vTaskDelay(pdMS_TO_TICKS(50));
            esp_io_expander_get_level(io_expander_handle, IO_EXPANDER_PIN_NUM_4, &pin_levels);
            if (!pin_levels) {
                
                return count;
                break;
            }
        }
    }
    return 0;
}

int dispmanager::boot_click() {
    return 0;
}