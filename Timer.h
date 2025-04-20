#ifndef __TIMER_H__
#define __TIMER_H__

#include "stm32f10x.h" // Device header
#include "GPIO.h"

void TIM3_PinConfig();
void TIM1_PinConfig();


void TIM3_PWM_IC_EM_Init(void);
void Timer4_Init(void);
void TIM_ResetCNT(TIM_TypeDef* TIMx);

void TIM_PWM_OCM_Init(TIM_TypeDef* TIMx);
void set_PWM(TIM_TypeDef* TIMx, uint32_t Duty_Cycle);
void set_Frequency(TIM_TypeDef* TIMx, uint32_t Frequency);
#endif /* __TIM_H__ */