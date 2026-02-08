#include <stdio.h>
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LED_PIN     (2)
void app_main(void)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .duty_resolution  = LEDC_TIMER_8_BIT,
        .timer_num        = LEDC_TIMER_0,
        .freq_hz          = 5000,
        .clk_cfg          = LEDC_USE_APB_CLK,
        .deconfigure      = false
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_t channel = LEDC_CHANNEL_0;
    ledc_channel_config_t ledc_channel = {
        .gpio_num   = LED_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = channel,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = LEDC_TIMER_0,
        .duty       = 0,
        .hpoint     = 0,
    };
    ledc_channel_config(&ledc_channel);

    ledc_fade_func_install(0);

    while(1)
    {
        ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, channel, 255, 1000);
        ledc_fade_start(LEDC_LOW_SPEED_MODE, channel, LEDC_FADE_WAIT_DONE);
        vTaskDelay(500 / portTICK_PERIOD_MS);

        ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, channel, 0, 1000);
        ledc_fade_start(LEDC_LOW_SPEED_MODE, channel, LEDC_FADE_WAIT_DONE);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
