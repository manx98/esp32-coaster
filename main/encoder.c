//
// Created by manx98 on 2024/12/30.
//
#include "encoder.h"
#include "config.h"

#define ESP_INTR_FLAG_DEFAULT 0

#define     Encoder_A     ENCODER_A_PIN    //编码器通道一
#define     Encoder_B     ENCODER_B_PIN    //编码器通道二
#define     Encoder_K     ENCODER_S_PIN    //编码器按键通道

#define     Encoder_CHA_GPIO_INPUT_PIN_SEL      ((1ULL<<Encoder_A))
#define     Encoder_CHB_GPIO_INPUT_PIN_SEL      ((1ULL<<Encoder_B))
#define     Encoder_KEY_GPIO_INPUT_PIN_SEL      ((1ULL<<Encoder_K))

QueueHandle_t encoder_gpio_event_queue = NULL;   //编码器队列
static unsigned int Value_count = 0;            //状态计数
static int Encoder_A_Last_Value = 0;            //第一次A项的值
static int Encoder_B_Last_Value = 0;            //第一次B项的值
static int Encoder_A_Value = 0;                 //第二次A项的值
static int Encoder_B_Value = 0;                 //第二次B项的值
#define TAG "Encoder"

/*****************************************
 * @brief GPIO中断处理
 * @author wsp
 * @date  2022-2-21
 * ***************************************/
static void IRAM_ATTR encoder_gpio_isr_handler(void *arg) {
    uint32_t GPIO_Queue_Data = 0;       //发送队列变量
    if (Value_count == 0) {               //边缘计数值，计数两次边缘值
        Encoder_A_Last_Value = gpio_get_level(Encoder_A);   //捕获A项的值
        Encoder_B_Last_Value = gpio_get_level(Encoder_B);   //捕获B项的值
        Value_count = 1;               //开始第一次计数
    } else if (Value_count == 1) {         //完成一个边缘捕获
        Encoder_A_Value = gpio_get_level(Encoder_A);        //捕获A项的值
        Encoder_B_Value = gpio_get_level(Encoder_B);        //捕获B项的值
        //状态判断处理
        if (((Encoder_A_Last_Value == 0 && Encoder_A_Value == 1) &&
             (Encoder_B_Last_Value == 1 && Encoder_B_Value == 0)) ||
            ((Encoder_A_Last_Value == 1 && Encoder_A_Value == 0) &&
             (Encoder_B_Last_Value == 0 && Encoder_B_Value == 1))) {        //顺时针旋转
            GPIO_Queue_Data = 1;        //右一
        } else if (((Encoder_A_Last_Value == 0 && Encoder_A_Value == 1) &&
                    (Encoder_B_Last_Value == 0 && Encoder_B_Value == 1)) ||
                   ((Encoder_A_Last_Value == 1 && Encoder_A_Value == 0) &&
                    (Encoder_B_Last_Value == 1 && Encoder_B_Value == 0))) {  //逆时针旋转
            GPIO_Queue_Data = 2;        //左二
        }
        Encoder_B_Last_Value = 2;       //清除状态值，不初始化0原因是在全局第一次初始化就是0，为了区别
        Encoder_A_Last_Value = 2;       //清除状态值
        Value_count = 0;               //清除状态值
    }
    if (GPIO_Queue_Data != 0)            //状态改变的时候 发送队列
        xQueueSendFromISR(encoder_gpio_event_queue, &GPIO_Queue_Data, NULL);
}

/*****************************************
 * @brief 编码器初始化
 * @author wsp
 * @date  2022-2-21
 * ***************************************/
void Encoder_init(void) {
    gpio_config_t io_conf;                                  //配置GPIO结构体
    io_conf.intr_type = GPIO_INTR_DISABLE;                  //不使能GPIO中断
    io_conf.mode = GPIO_MODE_INPUT;                         //GPIO输入模式
    io_conf.pull_down_en = 0;                               //下拉使能
    io_conf.pull_up_en = 1;                                 //上拉不使能
    io_conf.pin_bit_mask = Encoder_CHB_GPIO_INPUT_PIN_SEL;  //GPIO输入引脚选择
    gpio_config(&io_conf);                                  //配置IO参数

    io_conf.intr_type = GPIO_INTR_ANYEDGE;                  //边沿触发中断
    io_conf.pin_bit_mask = Encoder_CHA_GPIO_INPUT_PIN_SEL;  //GPIO输入引脚选择
    gpio_config(&io_conf);                                  //配置IO参数

    io_conf.intr_type = GPIO_INTR_DISABLE;                  //不使能GPIO中断
    io_conf.pin_bit_mask = Encoder_KEY_GPIO_INPUT_PIN_SEL;  //GPIO输入引脚选择
    gpio_config(&io_conf);                                  //配置IO参数

    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);        //安装GPIO中断服务
    gpio_isr_handler_add(Encoder_A, encoder_gpio_isr_handler, (void *) Encoder_A);//添加中断服务

    encoder_gpio_event_queue = xQueueCreate(1, sizeof(uint32_t));                //创建队列
}

void Encoder_Test(void) {
    char Capure_Enconder_State = 0;
    while (1) {
        //获取队列信息
        if (pdTRUE == xQueueReceive(encoder_gpio_event_queue, &Capure_Enconder_State, portMAX_DELAY)) {
            printf("Capure_Enconder_State:%d\n\r", Capure_Enconder_State);
        }
    }
}
