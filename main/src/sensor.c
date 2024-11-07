/*
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/* Includes */
#include "common.h"
#include "dht.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sensor.h"

/* Define global variable */
float current_temperature = 0.0f;

/* Function to get current temperature */
float get_temperature(void)
{
    return current_temperature;
}

/* Temperature Task */
void temperature_task(void *param)
{
    float temperature = 0.0f, humidity = 0.0f;
    esp_err_t res = dht_read_float_data(DHT_TYPE_AM2301, DHT22_GPIO, &humidity, &temperature);
    if (res == ESP_OK)
    {
        current_temperature = temperature;
        ESP_LOGI("SENSOR", "Temperature: %.1f°C, Humidity: %.1f%%", temperature, humidity);
    }
    else
    {
        ESP_LOGI("SENSOR", "Error occured :(");
        ESP_LOGE("SENSOR", "Could not read data from DHT22 sensor, error: %d", res);
    }
}
