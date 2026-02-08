#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

#define AHT10_ADDR                  0x38        // AHT10 アドレス
#define I2C_MASTER_SCL_IO           21          // SCL
#define I2C_MASTER_SDA_IO           47          // SDA
#define I2C_MASTER_FREQ_HZ          100000      // 100KHzモード

static const char *TAG = "AHT10";

void app_main(void)
{
    esp_err_t err;

    // I2C マスターバス設定
    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = false,
    };

    // バスハンドル
    i2c_master_bus_handle_t bus_handle;
    err = i2c_new_master_bus(&bus_config, &bus_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "i2c_new_master_bus failed: %s", esp_err_to_name(err));
        return;
    }

    // デバイス設定
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = AHT10_ADDR,
        .scl_speed_hz = I2C_MASTER_FREQ_HZ,
    };

    // デバイスハンドル
    i2c_master_dev_handle_t dev_handle;
    err = i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "i2c_master_bus_add_device failed: %s", esp_err_to_name(err));
        return;
    }

    // 初期化コマンド送信
    const uint8_t initCmd[1] = {0xE1};
    err = i2c_master_transmit(dev_handle, initCmd, sizeof(initCmd), pdMS_TO_TICKS(100));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "init transmit failed: %s", esp_err_to_name(err));
        return;
    }

    vTaskDelay(pdMS_TO_TICKS(100));

    while (1) {
        // 測定コマンド送信
        const uint8_t startCmd[3] = {0xAC, 0x33, 0x00};
        err = i2c_master_transmit(dev_handle, startCmd, sizeof(startCmd), pdMS_TO_TICKS(100));
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "start measurement failed: %s", esp_err_to_name(err));
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        // 測定待ち
        vTaskDelay(pdMS_TO_TICKS(100));

        // 測定データ読み取り
        uint8_t data[6];
        err = i2c_master_receive(dev_handle, data, sizeof(data), pdMS_TO_TICKS(100));
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "read measurement failed: %s", esp_err_to_name(err));
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        if (data[0] & 0x80) {
            ESP_LOGW(TAG, "Measurement not ready");
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        // データ変換
        uint32_t raw_humi = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | (data[3] >> 4);
        uint32_t raw_temp = (((uint32_t)data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | data[5];

        float humi = (raw_humi / 1048576.0f) * 100.0f;
        float temp = ((raw_temp / 1048576.0f) * 200.0f) - 50.0f;

        ESP_LOGI(TAG, "Temperature: %.2f°C, Humidity: %.2f%%", temp, humi);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // 終了処理（通常は到達しない）
    i2c_master_bus_rm_device(dev_handle);
    i2c_del_master_bus(bus_handle);
}
