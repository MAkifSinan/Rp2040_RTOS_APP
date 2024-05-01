#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "WS2812.hpp"
#include <stdio.h>
#include <cmath>
#include "hardware/adc.h"
#include <stdint.h>



#define LED_PIN 0
#define LED_LENGTH_1_1 20

#define LED_PIN2 1
#define TEMPERATURE_UNITS 'C'
 WS2812 ledStrip(
        LED_PIN,            // Data line is connected to pin 0. (GP0)
        LED_LENGTH_1_1,         // Strip is 6 LEDs long.
        pio0,               // Use PIO 0 for creating the state machine.
        0,                  // Index of the state machine that will be created for controlling the LED strip
                            // You can have 4 state machines per PIO-Block up to 8 overall.
                            // See Chapter 3 in: https://datasheets.raspberrypi.org/rp2040/rp2040-datasheet.pdf
        WS2812::FORMAT_GRB  // Pixel format used by the LED strip
    );
 WS2812 ledStrip2(
        LED_PIN2,            // Data line is connected to pin 0. (GP0)
        LED_LENGTH_1_1,         // Strip is 6 LEDs long.
        pio0,               // Use PIO 0 for creating the state machine.
        1,                  // Index of the state machine that will be created for controlling the LED strip
                            // You can have 4 state machines per PIO-Block up to 8 overall.
                            // See Chapter 3 in: https://datasheets.raspberrypi.org/rp2040/rp2040-datasheet.pdf
        WS2812::FORMAT_GRB  // Pixel format used by the LED strip
    );

static QueueHandle_t asdsasa=NULL;
void read(void *pvParameters) {
    
    /* 12-bit conversion, assume max value == ADC_VREF == 3.3 V */
    const float conversionFactor = 3.3f / (1 << 12);

    float adc = (float)adc_read() * conversionFactor;
    float tempC = 27.0f - (adc - 0.706f) / 0.001721f;
    int b=tempC;
    xQueueSend(asdsasa,&b,0U);
    vTaskDelay(1000);

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

void led_task2(void *pvParameters)
{   
    while (1)
    {
        uint32_t color = (uint32_t)rand();
        ledStrip2.fill(color);
        ledStrip2.show(color);
    
}

}


void usb_task(void *pvParameters){
    while (1)
    {
        int recieved_temp=-99;
        xQueueReceive(asdsasa,&recieved_temp,portMAX_DELAY);
        printf("led is off \r ,temp is %d \n",recieved_temp);
    }
    
}


int main()
{
    stdio_init_all();

    asdsasa = xQueueCreate(1,sizeof(int));
    
    xTaskCreate(led_task1, "LED_Task1", 256, NULL, 1, NULL);
    //xTaskCreate(led_task2, "LED_Task2", 256, NULL, 1, NULL);
    xTaskCreate(read, "read_onboardtemperature", 256, NULL, 1, NULL);

    xTaskCreate(usb_task, "USB_Task", 256, NULL, 1, NULL);
    
    //xTaskCreate(task1, "task1", 256, NULL, 1, NULL);
    //xTaskCreate(task2, "task2", 256, NULL, 1, NULL);

    vTaskStartScheduler();

    while(1){};
}