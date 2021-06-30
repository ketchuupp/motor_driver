#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>
#include "gpio.h"

#define EN_Pin GPIO_PIN_10
#define EN_PORT GPIOA

// step driver
extern TIM_HandleTypeDef htim2;

#define RESET_PIN   GPIO_PIN_1
#define RESET_PORT  GPIOA
#define SLEEP_PIN   GPIO_PIN_2
#define SLEEP_PORT  GPIOA
#define ENABLE_PIN  GPIO_PIN_3
#define ENABLE_PORT GPIOA
#define DIR_PIN     GPIO_PIN_4
#define DIR_PORT    GPIOA
#define TIM         &htim2

typedef enum {disp_lv_0 = 0, disp_lv_1 = 1} disp_lv;
typedef enum {motor_DIS = 0, motor_EN = 1} motor_state;
typedef enum {pin_DIS = 0, pin_EN = 1} pin_state;


typedef struct motor_cluster {
    int8_t speed;
    motor_state m_state;
    pin_state p_state;
    float gear_ratio;
    
} MotorCluster;

void Motor_Init();
void UpdateSpeed(void);
void UpdateState(void);
void StartMotor(void);
void StopMotor(void);
void ChangeSpeed(void);
void ReloadDisplay(disp_lv lv);
int8_t get_speed(void);
motor_state get_state(void);

#endif /* COMMON_H_ */