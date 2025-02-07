#include "powermanager.h"

powermanager::powermanager(/* args */) {
}

powermanager::~powermanager() {
}

static int pmu_register_read(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len) {
    if (len == 0) {
        return ESP_OK;
    }
    if (data == NULL) {
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
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "PMU i2c_master_cmd_begin FAILED! > ");
        return ESP_FAIL;
    }
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (devAddr << 1) | READ_BIT, ACK_CHECK_EN);
    if (len > 1) {
        i2c_master_read(cmd, data, len - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, &data[len - 1], NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdTICKS_TO_MS(1000));
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "PMU READ FAILED! > ");
    }
    return ret == ESP_OK ? 0 : -1;
}

/**
 * @brief Write a byte to a pmu register
 */
static int pmu_register_write_byte(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len) {
    if (data == NULL) {
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
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "PMU WRITE FAILED! < ");
    }
    return ret == ESP_OK ? 0 : -1;
}

esp_err_t powermanager::init() {
    if (PMU.begin(AXP2101_SLAVE_ADDRESS, pmu_register_read, pmu_register_write_byte)) {
        ESP_LOGI(TAG, "Init PMU SUCCESS!");
    } else {
        ESP_LOGE(TAG, "Init PMU FAILED!");
        return ESP_FAIL;
    }

    // Turn off not use power channel
    PMU.disableDC2();
    PMU.disableDC3();
    PMU.disableDC4();
    PMU.disableDC5();

    PMU.disableALDO1();
    PMU.disableALDO2();
    PMU.disableALDO3();
    PMU.disableALDO4();
    PMU.disableBLDO1();
    PMU.disableBLDO2();

    PMU.disableCPUSLDO();
    PMU.disableDLDO1();
    PMU.disableDLDO2();

    // ESP32s3 Core VDD
    PMU.setDC3Voltage(3300);
    PMU.enableDC3();

    // Extern 3.3V VDD
    PMU.setDC1Voltage(3300);
    PMU.enableDC1();

    // CAM DVDD  1500~1800
    PMU.setALDO1Voltage(1800);
    // PMU.setALDO1Voltage(1500);
    PMU.enableALDO1();

    // CAM DVDD 2500~2800
    PMU.setALDO2Voltage(2800);
    PMU.enableALDO2();

    // CAM AVDD 2800~3000
    PMU.setALDO4Voltage(3000);
    PMU.enableALDO4();

    // PIR VDD 3300
    PMU.setALDO3Voltage(3300);
    PMU.enableALDO3();

    // OLED VDD 3300
    PMU.setBLDO1Voltage(3300);
    PMU.enableBLDO1();

    // MIC VDD 33000
    PMU.setBLDO2Voltage(3300);
    PMU.enableBLDO2();

    // ESP_LOGI(TAG, "DCDC=======================================================================\n");
    // ESP_LOGI(TAG, "DC1  : %s   Voltage:%u mV \n", PMU.isEnableDC1() ? "+" : "-", PMU.getDC1Voltage());
    // ESP_LOGI(TAG, "DC2  : %s   Voltage:%u mV \n", PMU.isEnableDC2() ? "+" : "-", PMU.getDC2Voltage());
    // ESP_LOGI(TAG, "DC3  : %s   Voltage:%u mV \n", PMU.isEnableDC3() ? "+" : "-", PMU.getDC3Voltage());
    // ESP_LOGI(TAG, "DC4  : %s   Voltage:%u mV \n", PMU.isEnableDC4() ? "+" : "-", PMU.getDC4Voltage());
    // ESP_LOGI(TAG, "DC5  : %s   Voltage:%u mV \n", PMU.isEnableDC5() ? "+" : "-", PMU.getDC5Voltage());
    // ESP_LOGI(TAG, "ALDO=======================================================================\n");
    // ESP_LOGI(TAG, "ALDO1: %s   Voltage:%u mV\n", PMU.isEnableALDO1() ? "+" : "-", PMU.getALDO1Voltage());
    // ESP_LOGI(TAG, "ALDO2: %s   Voltage:%u mV\n", PMU.isEnableALDO2() ? "+" : "-", PMU.getALDO2Voltage());
    // ESP_LOGI(TAG, "ALDO3: %s   Voltage:%u mV\n", PMU.isEnableALDO3() ? "+" : "-", PMU.getALDO3Voltage());
    // ESP_LOGI(TAG, "ALDO4: %s   Voltage:%u mV\n", PMU.isEnableALDO4() ? "+" : "-", PMU.getALDO4Voltage());
    // ESP_LOGI(TAG, "BLDO=======================================================================\n");
    // ESP_LOGI(TAG, "BLDO1: %s   Voltage:%u mV\n", PMU.isEnableBLDO1() ? "+" : "-", PMU.getBLDO1Voltage());
    // ESP_LOGI(TAG, "BLDO2: %s   Voltage:%u mV\n", PMU.isEnableBLDO2() ? "+" : "-", PMU.getBLDO2Voltage());
    // ESP_LOGI(TAG, "CPUSLDO====================================================================\n");
    // ESP_LOGI(TAG, "CPUSLDO: %s Voltage:%u mV\n", PMU.isEnableCPUSLDO() ? "+" : "-", PMU.getCPUSLDOVoltage());
    // ESP_LOGI(TAG, "DLDO=======================================================================\n");
    // ESP_LOGI(TAG, "DLDO1: %s   Voltage:%u mV\n", PMU.isEnableDLDO1() ? "+" : "-", PMU.getDLDO1Voltage());
    // ESP_LOGI(TAG, "DLDO2: %s   Voltage:%u mV\n", PMU.isEnableDLDO2() ? "+" : "-", PMU.getDLDO2Voltage());
    // ESP_LOGI(TAG, "===========================================================================\n");

    PMU.clearIrqStatus();

    PMU.enableVbusVoltageMeasure();
    PMU.enableBattVoltageMeasure();
    PMU.enableSystemVoltageMeasure();
    PMU.enableTemperatureMeasure();
    // It is necessary to disable the detection function of the TS pin on the board
    // without the battery temperature detection function, otherwise it will cause abnormal charging
    PMU.disableTSPinMeasure();

    // Disable all interrupts
    PMU.disableIRQ(XPOWERS_AXP2101_ALL_IRQ);
    // Clear all interrupt flags
    PMU.clearIrqStatus();
    // Enable the required interrupt function
    PMU.enableIRQ(
        XPOWERS_AXP2101_BAT_INSERT_IRQ | XPOWERS_AXP2101_BAT_REMOVE_IRQ |    // BATTERY
        XPOWERS_AXP2101_VBUS_INSERT_IRQ | XPOWERS_AXP2101_VBUS_REMOVE_IRQ |  // VBUS
        XPOWERS_AXP2101_PKEY_SHORT_IRQ | XPOWERS_AXP2101_PKEY_LONG_IRQ |     // POWER KEY
        XPOWERS_AXP2101_BAT_CHG_DONE_IRQ | XPOWERS_AXP2101_BAT_CHG_START_IRQ // CHARGE
        // XPOWERS_AXP2101_PKEY_NEGATIVE_IRQ | XPOWERS_AXP2101_PKEY_POSITIVE_IRQ   |   //POWER KEY
    );

    // Set the precharge charging current
    PMU.setPrechargeCurr(XPOWERS_AXP2101_PRECHARGE_50MA);
    // Set constant current charge current limit
    PMU.setChargerConstantCurr(XPOWERS_AXP2101_CHG_CUR_200MA);
    // Set stop charging termination current
    PMU.setChargerTerminationCurr(XPOWERS_AXP2101_CHG_ITERM_25MA);

    // Set charge cut-off voltage
    PMU.setChargeTargetVoltage(XPOWERS_AXP2101_CHG_VOL_4V1);

    // Read battery percentage
    ESP_LOGI(TAG, "battery percentage:%d %%", PMU.getBatteryPercent());

    // Set the watchdog trigger event type
    // PMU.setWatchdogConfig(XPOWERS_AXP2101_WDT_IRQ_TO_PIN);
    // Set watchdog timeout
    // PMU.setWatchdogTimeout(XPOWERS_AXP2101_WDT_TIMEOUT_4S);
    // Enable watchdog to trigger interrupt event
    // PMU.enableWatchdog();
    return ESP_OK;
}

static void pmu_hander_task(void *args) {
    auto manager = (powermanager *)args;
    while (1) {
        if (manager->power_cb != NULL) {
            manager->power_cb(manager);
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void powermanager::start_power_monitor() {
    xTaskCreate(pmu_hander_task, "App/pwr", 4 * 1024, this, 2, NULL);
}

int powermanager::get_percent() {
    if (PMU.isBatteryConnect()) {
        return PMU.getBatteryPercent();
    }
    return 0;
}

bool powermanager::is_charging() {
    return PMU.isCharging();
}

void powermanager::sleep(){
    const esp_pm_config_t pm_config = {
        .max_freq_mhz = 80,
        .min_freq_mhz = 40,
        #if CONFIG_FREERTOS_USE_TICKLESS_IDLE
        .light_sleep_enable = true
         #endif
        };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
    PMU.enableSleep();
}

void powermanager::wakeup(){
    const esp_pm_config_t pm_config = {
        .max_freq_mhz = 240,
        .min_freq_mhz = 40,
        #if CONFIG_FREERTOS_USE_TICKLESS_IDLE
        .light_sleep_enable = true
         #endif
        };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
    PMU.disableSleep();
}