#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "dht.h"

#define DHT22_GPIO GPIO_NUM_4

extern "C" void app_main()
{
    while (1)
    {
        float temperature = 0.0f, humidity = 0.0f;
        esp_err_t res = dht_read_float_data(DHT_TYPE_AM2301, DHT22_GPIO, &humidity, &temperature);
        if (res == ESP_OK)
        {
            printf("Temperature: %.1f°C, Humidity: %.1f%%\n", temperature, humidity);
        }
        else
        {
            printf("Could not read data from sensor, error: %d\n", res);
        }

        vTaskDelay(pdMS_TO_TICKS(2000)); // Wait 2 seconds before next reading
    }
}
