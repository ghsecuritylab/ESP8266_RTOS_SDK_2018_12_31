#include "csro_common.h"

void csro_task_sc(void *pvParameters)
{
    while(1)
    {
        vTaskDelay(1000 / portTICK_RATE_MS);
        debug("SC TASK!\n");
    }
}