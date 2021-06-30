#ifndef DRV8825_H_
#define DRV8825_H_

#include "main.h"

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


#endif /* DRV8825_H_ */