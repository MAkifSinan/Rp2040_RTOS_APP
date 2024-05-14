#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "queue.h"
#include "pico/stdlib.h"
#include "WS2812.hpp"
#include <stdio.h>
#include <cmath>
#include "hardware/adc.h"
#include <stdint.h>
#ifdef PICO_DEFAULT_LED_PIN
#define LED_PIN PICO_DEFAULT_LED_PIN
#endif



static QueueHandle_t xQueue = NULL;

 
#define LED_PINn 0
#define LED_LENGTH_1_1 60

#define LED_PIN2 1
#define TEMPERATURE_UNITS 'C'


 WS2812 ledStrip(
        LED_PINn,            // Data line is connected to pin 0. (GP0)
        LED_LENGTH_1_1,         // Strip is 6 LEDs long.
        pio0,               // Use PIO 0 for creating the state machine.
        0,                  // Index of the state machine that will be created for controlling the LED strip
                            // You can have 4 state machines per PIO-Block up to 8 overall.
                            // See Chapter 3 in: https://datasheets.raspberrypi.org/rp2040/rp2040-datasheet.pdf
        WS2812::FORMAT_GRB  // Pixel format used by the LED strip
    );
/*
 WS2812 ledStrip2(
        LED_PIN2,            // Data line is connected to pin 0. (GP0)
        LED_LENGTH_1_1,         // Strip is 6 LEDs long.
        pio0,               // Use PIO 0 for creating the state machine.
        1,                  // Index of the state machine that will be created for controlling the LED strip
                            // You can have 4 state machines per PIO-Block up to 8 overall.
                            // See Chapter 3 in: https://datasheets.raspberrypi.org/rp2040/rp2040-datasheet.pdf
        WS2812::FORMAT_GRB  // Pixel format used by the LED strip
    );

*/
void read_onboard_temperature(void *pvParameters) {
    
    /* 12-bit conversion, assume max value == ADC_VREF == 3.3 V */
    const float conversionFactor = 3.3f / (1 << 12);

    while (true) {
        float adc = (float)adc_read() * conversionFactor;
        float tempC = 27.0f - (adc - 0.706f) / 0.001721f;
        xQueueSend(xQueue, &tempC, 0U);
        vTaskDelay(1000);
    }
}

void led_buildin(void *pvParameters){

    while (true) {
        gpio_put(LED_PIN, 1);
        vTaskDelay(500);
        gpio_put(LED_PIN, 0);
        vTaskDelay(500);
        
    }
}

void led_task1(void *pvParameters)
{   
    while (1)
    {
        uint32_t color = (uint32_t)rand();
        ledStrip.fill(color);
        ledStrip.show(color);
        vTaskDelay(1000);
}
}
/* 
void led_task2(void *pvParameters)
{   
    while (1)
    {
        uint32_t color = (uint32_t)rand();
        ledStrip2.fill(color);
        ledStrip2.show(color);
    
}

}

*/
void usb_task(void *pvParameters){
    while (1)
    {
        float uIReceivedValue;

        xQueueReceive(xQueue, &uIReceivedValue, portMAX_DELAY);
        
        printf("in read func ,temp   %.02f  \n",uIReceivedValue);

    }
    
}


int main()
{
    
    stdio_init_all();

    xQueue = xQueueCreate(1, sizeof(float));
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    /* Initialize hardware AD converter, enable onboard temperature sensor and
     *   select its channel (do this once for efficiency, but beware that this
     *   is a global operation). */
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);

    xTaskCreate(led_task1, "LED_Task1", 256, NULL, 1, NULL);
    //xTaskCreate(led_task2, "LED_Task2", 256, NULL, 1, NULL);
    //xTaskCreate(read, "read_onboardtemperature", 256, NULL, 20, NULL);

    xTaskCreate(usb_task, "USB_Task", 256, NULL, 1, NULL);
    
    xTaskCreate(led_buildin, "led_buildin", 256, NULL, 1, NULL);
    xTaskCreate(read_onboard_temperature, "read_onboard_temperature", 256, NULL, 1, NULL);

    vTaskStartScheduler();

    while(1){};


}
