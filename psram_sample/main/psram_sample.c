#include <stdio.h>
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// PSRAM上に static変数を配置する
#ifdef CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
#define PSRAM_BUF_SIZE  (1024 * 1024 * 5)
EXT_RAM_BSS_ATTR static uint8_t psram_buf[PSRAM_BUF_SIZE];
#endif

#define MY_TASK_STACK_SIZE  (1024 * 1024)   // 1MBをスタックサイズとして確保する

void my_task_main(void* arg)
{
    while(true) {
        ESP_LOGI("my_task", "my_task running...");
        vTaskDelay(pdMS_TO_TICKS(5000));

        #ifdef CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
        psram_buf[0]++;
        ESP_LOGI("my_task", "my_psram_buf[0] is %d", psram_buf[0]);
        #else
        ESP_LOGW("my_task", "CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY is not enable");
        #endif
    }
}

void app_main(void)
{
    // PSRAMからスタックを確保する
    #ifdef CONFIG_SPIRAM
        ESP_LOGI("MEM", "PSRAM is enable, start my_task");
        TaskHandle_t taskHandle = NULL;
        xTaskCreateWithCaps(my_task_main, "my_task", MY_TASK_STACK_SIZE, NULL, 10, &taskHandle, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    #else
        ESP_LOGW("MEM", "PSRAM is not enable");
    #endif

    multi_heap_info_t info;
    size_t freeBlkSize;
    while (true) {
        // 内蔵DRAM
        heap_caps_get_info(&info, MALLOC_CAP_INTERNAL);
        freeBlkSize = heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL);
        ESP_LOGI("MEM", "Internal DRAM: total=%u, used=%u, free=%u, freeBlk=%u",
                info.total_allocated_bytes + info.total_free_bytes,
                info.total_allocated_bytes,
                info.total_free_bytes,
                freeBlkSize);

        // PSRAM
        heap_caps_get_info(&info, MALLOC_CAP_SPIRAM);
        freeBlkSize = heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM);
        ESP_LOGI("MEM", "PSRAM: total=%u, used=%u, free=%u, freeBlk=%u",
                info.total_allocated_bytes + info.total_free_bytes,
                info.total_allocated_bytes,
                info.total_free_bytes,
                freeBlkSize);

        // 32KBずつメモリリークさせる
        void *p = malloc(32 * 1024);
        if (p == NULL) {
            ESP_LOGE("MEM", "malloc failed");
        }

        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}
