#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define SWITCH_GPIO      (1)
#define GPIO_PORT_LED    (2)

static const char *TAG = "SWITCH";

// 通知を受け取るタスクのハンドル
static TaskHandle_t switch_task_handle = NULL;

// 割り込みハンドラ
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // switch_task に通知
    vTaskNotifyGiveFromISR(switch_task_handle, &xHigherPriorityTaskWoken);

    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

void switch_task(void *arg)
{
    // 出力ポートに設定
    gpio_set_direction(GPIO_PORT_LED, GPIO_MODE_OUTPUT);

    // LED　OFF
    gpio_set_level(GPIO_PORT_LED, 0);

    TickType_t press_start = 0;

    while (1) {
        // 割込みからの通知を待つ
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        ESP_LOGI(TAG, "taskNotify received...");

        int level = gpio_get_level(SWITCH_GPIO);
        if (level == 0) {
            // 押された瞬間
            press_start = xTaskGetTickCount();
            ESP_LOGI(TAG, "★★★★★★★★★★★★ Switch Pressed ★★★★★★★★★★★★");
            gpio_set_level(GPIO_PORT_LED, 1);
        } else {
            ESP_LOGI(TAG, "★★★★★★★★★★★★ Switch Released ★★★★★★★★★★★★");
            TickType_t diff = xTaskGetTickCount() - press_start;
            if (diff >= pdMS_TO_TICKS(3000)) {
                ESP_LOGI(TAG, "★★★★★★★★★★★★ LONG PRESS DETECTED! ★★★★★★★★★★★★");
            }
            gpio_set_level(GPIO_PORT_LED, 0);
        }
    }
}

void start_switch_task(void)
{
    // GPIO 設定
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << SWITCH_GPIO,
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_ANYEDGE,
        .pull_up_en = true,
        .pull_down_en = false,
    };
    gpio_config(&io_conf);

    // ISR インストール
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    gpio_isr_handler_add(SWITCH_GPIO, gpio_isr_handler, NULL);

    // タスク起動
    xTaskCreate(switch_task, "switch_task", 4096, NULL, 10, &switch_task_handle);
}