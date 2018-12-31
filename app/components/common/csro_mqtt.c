#include "csro_common.h"

void csro_task_mqtt(void *pvParameters)
{
    while(1)
    {
        vTaskDelay(1000 / portTICK_RATE_MS);
        debug("MQTT TASK!\n");
    }    
}