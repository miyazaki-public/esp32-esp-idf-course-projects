#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

int add(int a, int b)
{
    return a + b;
}

void app_main(void)
{
    int cnt = 0;
    printf("app_main start...\n");
    while(1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        cnt = add(cnt, 1);
        printf("cnt: %d\n", cnt);
    }
}
