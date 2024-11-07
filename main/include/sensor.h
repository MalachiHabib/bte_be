#ifndef SENSOR_H
#define SENSOR_H

/* Define GPIO for DHT22 */
#define DHT22_GPIO GPIO_NUM_4
#define TEMP_TASK_PERIOD (2000 / portTICK_PERIOD_MS)

/* Declare global temperature variable */
extern float current_temperature;

/* Function to get current temperature */
float get_temperature(void);
void temperature_task(void *param);

#endif // SENSOR_H
