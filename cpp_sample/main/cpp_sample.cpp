#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <iostream>
#include "person.h"

extern "C" void app_main(void)
{
    Person *person = new Person("Taro", 20);
    while(true) {
        person->Greet();
        person->IncrementAge();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    delete person;
}
