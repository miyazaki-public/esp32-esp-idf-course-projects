#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

static const char *TAG = "error-handling-sample";

void app_main(void)
{
    ESP_LOGI(TAG, "app_main started...");

    // サイズの大きなローカル変数を確保してスタックオーバーフローを発生させる
    #if 1
    char buf[4096];
    memset(buf, 0, 4096);
    #endif

    // NVS未初期化の状態でパーティションをオープンする(戻り値がESP_OK以外になる)
    #if 1
    nvs_handle_t handle;
    esp_err_t err = nvs_open_from_partition("mynvs", "storage", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error nvs_open_from_partition %s", esp_err_to_name(err));
    }
    #endif

    while(true) {
        ESP_LOGI(TAG, "task running...");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
