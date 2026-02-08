#pragma once

#define RECORD_COUNT_MAX        (30)         // センサデータ保持件数

typedef struct _SENSOR_RECORD
{
    float temp; // 温度
    float humi; // 湿度
} SENSOR_RECORD;

extern void start_sensor_task(void);
extern void get_sensor_records(SENSOR_RECORD *records, uint32_t *count);
