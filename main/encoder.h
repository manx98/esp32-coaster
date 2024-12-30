//
// Created by manx98 on 2024/12/30.
//
#ifndef _Encoder_H_
#define _Encoder_H_

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "sdkconfig.h"

extern QueueHandle_t encoder_gpio_event_queue;

void Encoder_init(void);

void Encoder_Test(void);

#endif

