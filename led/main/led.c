#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define GPIO_PORT_LED       (2)

void app_main(void)
{
    // 出力ポートに設定
    gpio_set_direction(GPIO_PORT_LED, GPIO_MODE_OUTPUT);

    // LED　OFF
    gpio_set_level(GPIO_PORT_LED, 0);

    while(1)
    {
        gpio_set_level(GPIO_PORT_LED, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        gpio_set_level(GPIO_PORT_LED, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

}
