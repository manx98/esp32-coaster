#include <esp_log.h>
#include <hx711.h>
#include <lm75a.h>
#include <stdio.h>
#include <u8g2_esp32_hal.h>

#include "config.h"

static char* TAG = "main";

static void init_u8g2_esp32()
{
    u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
    u8g2_esp32_hal.bus.i2c.sda = I2C_SDA_PIN;
    u8g2_esp32_hal.bus.i2c.scl = I2C_SCL_PIN;
    u8g2_esp32_hal_init(u8g2_esp32_hal);
    // 针对不同的屏幕使用不同的初始化函数
    u8g2_Setup_ssd1306_i2c_128x32_univision_f(
        &u8g2, U8G2_R2,
        u8g2_esp32_i2c_byte_cb,
        u8g2_esp32_gpio_and_delay_cb); // init u8g2 structure
    u8x8_SetI2CAddress(&u8g2.u8x8, OLED_I2C_ADDRESS << 1);
    u8g2_InitDisplay(&u8g2);     // send init sequence to the display, display is in
    // sleep mode after this,
    u8g2_SetPowerSave(&u8g2, 0); // wake up display
    u8g2_ClearBuffer(&u8g2);
}

void i2c_scanner() {
    ESP_LOGI(TAG, "Scanning I2C bus...");
    // Iterate over all possible I2C addresses
    for (int addr = 1; addr < 127; addr++) {
        // Create I2C command link
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        // Send I2C address with write bit
        i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);

        // Execute I2C command and check for response
        esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));
        i2c_cmd_link_delete(cmd);

        // If device responds, print the address
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "Found device at address 0x%02x", addr);
        }
    }
    ESP_LOGI(TAG, "I2C scan complete.");
}

void print_sensors()
{
    init_u8g2_esp32();
    i2c_scanner();
    HX711_init(HX711_DI_PIN, HX711_SCK_PIN, eGAIN_128);
    HX711_tare();
    lm75a_set_interrupt(0);
    float temp;
    char buf[100];
    while (true)
    {
        esp_err_t ret = lm75a_read_temperature(&temp);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Error reading temperature: %s", esp_err_to_name(ret));
        }
        u8g2_ClearBuffer(&u8g2);
        u8g2_SetFont(&u8g2, u8g2_font_5x7_tf);
        sprintf(buf, "TMP: %.2f", temp);
        u8g2_DrawStr(&u8g2, 0, 10, buf);
        float weight = HX711_get_units(HX711_AVG_SAMPLES);
        float real_weight = weight / (float)306 * 5;
        sprintf(buf, "W: %.2f", real_weight);
        u8g2_DrawStr(&u8g2, 0, 20, buf);
        u8g2_SendBuffer(&u8g2);
        ESP_LOGI(TAG, "W: %.2f", weight);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    print_sensors();
}
