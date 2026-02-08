#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sensor_task.h"
static const char *TAG = "logging_task";

void logging_task(void *arg)
{
    SENSOR_RECORD sensor_records[RECORD_COUNT_MAX];
    memset(sensor_records, 0, sizeof(SENSOR_RECORD) * RECORD_COUNT_MAX);
    while(1) {
        // メインループの処理時間を計測
        TickType_t start_tick = xTaskGetTickCount();

        uint32_t record_count = 0;
        get_sensor_records(sensor_records, &record_count);
        ESP_LOGI(TAG, "******** START output records count: %d ********", record_count);
        ESP_LOGI(TAG, "▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼ OUTPUT SENSOR RECORDS ▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼");

        for (int i = 0; i < record_count; i++) {
            ESP_LOGI(TAG, "record[%d]: temp:[%.2f], humi:[%.2f]", i, sensor_records[i].temp, sensor_records[i].humi);

            // logging_taskの処理時間を長くするためのダミー処理
            for(int j = 0; j < 4000000; j++);
        }
        ESP_LOGI(TAG, "▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲ OUTPUT SENSOR RECORDS ▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲");

        // メインループの処理時間をログに出力
        TickType_t end_tick = xTaskGetTickCount();
        TickType_t diff = end_tick - start_tick;
        ESP_LOGI(TAG, "******** ELAPSED TIME = %lu ms ********", diff * portTICK_PERIOD_MS);

        // 10秒周期で起動
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

void start_logging_task(void)
{
    xTaskCreate(logging_task, "logging_task", 4096, NULL, 5, NULL);
}
