#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "switch_task.h"
#include "logging_task.h"
#include "sensor_task.h"

void app_main(void)
{
    // 各タスクの機能を開始
    start_switch_task();
    start_sensor_task();
    start_logging_task();
}
 
