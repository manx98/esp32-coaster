//
// Created by wenyiyu on 2024/12/28.
//

#ifndef CONFIG_H
#define CONFIG_H
#include  <u8g2.h>

#define I2C_SDA_PIN GPIO_NUM_1
#define I2C_SCL_PIN GPIO_NUM_2
#define OLED_I2C_ADDRESS 0x3c

#define HX711_SCK_PIN GPIO_NUM_4
#define HX711_DI_PIN GPIO_NUM_3
#define HX711_AVG_SAMPLES   10

#define ENCODER_B_PIN GPIO_NUM_7
#define ENCODER_S_PIN GPIO_NUM_5
#define ENCODER_A_PIN GPIO_NUM_6

#define CHARGE_LED GPIO_NUM_8
#define CHARGE_FULL_LED GPIO_NUM_10

static u8g2_t u8g2;

#endif //CONFIG_H
