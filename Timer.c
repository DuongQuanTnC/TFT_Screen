#include "Timer.h"
#include "RCC.h"

void Timer4_Init(void) {
    RCC->APB1ENR |= (1 << 2);  // B?t clock cho TIM4

    TIM4->PSC = 8399;  // Prescaler: 10 kHz (84MHz / 8399 + 1)
    TIM4->ARR = 999;   // Auto-reload (100ms ng?t)
    TIM4->DIER |= TIM_DIER_UIE; // B?t ng?t Update
    TIM4->CR1 |= TIM_CR1_CEN;  // B?t Timer

    NVIC_EnableIRQ(TIM4_IRQn);  // B?t ng?t trong NVIC
    NVIC_SetPriority(TIM4_IRQn, 0);
}

void TIM3_PinConfig()
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	GPIO_SetMode(GPIOA, 6, GPIO_INPUT_MODE_FLOAT);
	GPIO_SetMode(GPIOA, 7, GPIO_INPUT_MODE_FLOAT);
}

void TIM3_PWM_IC_EM_Init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    
	TIM3->CCMR1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0; // Ch?n input IC1 & IC2
    TIM3->SMCR |= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;  // Ch?n Encoder Mode 3
    TIM3->CCMR1 |= (TIM_CCMR1_IC1F_2 | TIM_CCMR1_IC2F_2); 
    TIM3->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E;
    
	TIM3->PSC = 0;
    TIM3->ARR = 0xFFFF;        // Giá tr? t?i da (16-bit = 65535)
    TIM_ResetCNT(TIM3);
    TIM3->CR1 |= TIM_CR1_CEN;
}

void TIM1_PinConfig() {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    GPIO_SetMode(GPIOA, 8, GPIO_ALT_MODE_50M_PP);
}

// Initialize PWM output for Timer 1 function
void set_PWM(TIM_TypeDef* TIMx, uint32_t Duty_Cycle) 
{   
    if (Duty_Cycle > 100) Duty_Cycle = 100;
    if (Duty_Cycle < 0) Duty_Cycle = 0;
    TIMx->CCR1 = (Duty_Cycle * ((float)(TIMx->ARR + 1) / 100));
    
}

void set_Frequency(TIM_TypeDef* TIMx, uint32_t Frequency) {
    
    uint32_t RCC_Clock; 
    uint32_t Reload;
    uint16_t Prescaler;
	RCC_Clock = 0; 
    Reload = 0;
    Prescaler = 0;
    
    if (Frequency == 0) {
        TIMx->CR1 &= ~TIM_CR1_CEN; // D?ng Timer
        return;
    }

    
	//Enable Timer Clock
	if(TIMx == TIM1)
	{
		RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
		RCC_Clock = RCC_GetAbp2Clk();
	}
	else if(TIMx == TIM2 || TIMx == TIM3 || TIMx == TIM4)
	{
		RCC->APB1ENR |= (1 << (((uint32_t)TIMx - (uint32_t)TIM2_BASE) >>  0xA));
		RCC_Clock = RCC_GetAbp1Clk();
	}
	
	/* Calculate Reload Value */
	Reload = (RCC_Clock/Frequency) - 1;
	while(Reload > 0xFFFF) 
	{
		Prescaler++;
		Reload = (RCC_Clock/(Frequency*(Prescaler + 1))) - 1;
	}
		
	//Reset Timer
	TIMx->CR1 = 0;
	TIMx->CNT = 0;
	
	//Initialize Timer
	TIMx->ARR = Reload;
	TIMx->PSC = Prescaler;
    TIMx->CR1 |= TIM_CR1_CEN;
    //TIM_ResetCNT(TIMx);
}


void TIM_PWM_OCM_Init(TIM_TypeDef* TIMx)
{	
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    //TIM_ResetCNT(TIMx);
	/* Config Timer for PWM Mode */
	//OC1M = 110, CC1  channel is configure as PWM Mode 1
	TIMx->CCMR1 |= (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1);	
	//Preload register on TIMx_CCR1 enabled
	TIMx->CCMR1 |= TIM_CCMR1_OC1PE;
	// TIMx_ARR register is buffered
	TIMx->CR1 |= TIM_CR1_ARPE;
	//OC1 signal is output on the corresponding output pin
	TIMx->CCER |= TIM_CCER_CC1E;
	
    if (TIMx == TIM1 || TIMx == TIM8 ) {
        TIMx->BDTR |= TIM_BDTR_OSSI | TIM_BDTR_OSSR;
        TIMx->BDTR |= TIM_BDTR_MOE; // Main output enable
    }
	//Calculate CCR1 for Duty Cycle
	//TIMx->CCR1 = (Duty_Cycle * ((float)(TIMx->ARR + 1) / 100));
	//Reinitialize the counter and generates an update of the registers
	TIMx->EGR  |= TIM_EGR_UG;
	//OC and OCN outputs are enabled
	//TIMx->BDTR |= TIM_BDTR_MOE;
	//Enable TIMx
    TIMx->CR1 |= TIM_CR1_CEN;
}

void TIM_ResetCNT(TIM_TypeDef* TIMx)
{
	TIMx->CR1 &= ~TIM_CR1_CEN;
	TIMx->CNT = 0;
}

void TIM_PWM_ICM_Init(TIM_TypeDef* TIMx)
{
	//Initialize Timer
	RCC->APB1ENR |= (1 << (((uint32_t)TIMx - (uint32_t)TIM2_BASE) >>  0xA));
	TIMx->PSC = 1000;
	/* Config Timer for Input Capture PWM Input Mode */
	
	//TIMx_ARR register is buffered
	TIMx->CR1 |= TIM_CR1_ARPE;
	//Enable update event, set UG bit
	TIMx->CR1 &= ~TIM_CR1_UDIS;
	TIMx->EGR |= TIM_EGR_UG;
	//Slave mode: 100 - Reset mode - Rising edge of the selected trigger input (TRGI) reinitializes the counter and generates an update of the registers.
	TIMx->SMCR |= TIM_SMCR_SMS_2 ;
	//101 - Filtered Timer Input 1 (TI1FP1)
	TIMx->SMCR |= TIM_SMCR_TS_2 | TIM_SMCR_TS_0;
	// Configure Channel 1 in PWM input mode
	TIMx->CCMR1 |= TIM_CCMR1_CC1S_0;              // CC1S = 01, CC1 channel is configured as input, IC1 is mapped on TI1
	TIMx->CCER &= ~TIM_CCER_CC1P;                 // Capture is done on a rising edge of IC1
	// Configure Channel 2 in PWM input mode
	TIMx->CCMR1 |= TIM_CCMR1_CC2S_1;              // CC2S = 10, CC2 channel is configured as input, IC2 is mapped on TI1
	TIMx->CCER |= TIM_CCER_CC2P;                  // Capture is done on a falling edge of IC1	
	// Capture/Compare 1 interrupt enable
	TIMx->DIER  |= TIM_DIER_CC1IE | TIM_DIER_CC2IE|TIM_DIER_UIE ;	
	//Enable CH1 & CH2
	TIMx->CCER  |= TIM_CCER_CC1E | TIM_CCER_CC2E;	
	//Enable TIMx
	TIMx->CR1 |= TIM_CR1_CEN;
}


/*void TIM_OC2_PWMM1_Init(TIM_TypeDef* TIMx, uint32_t Frequency, uint32_t Duty_Cycle)
{
	//Initialize Timer
	//TIM_Init(TIMx, Frequency);
	TIM_ResetCNT(TIMx);
	
	 //Config Timer for PWM Mode 
	//OC2M = 110, CC2  channel is configure as PWM Mode 1
	TIMx->CCMR1 |= (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1);	
	//Preload register on TIMx_CCR2 enabled
	TIMx->CCMR1 |= TIM_CCMR1_OC2PE;
	// TIMx_ARR register is buffered
	TIMx->CR1   |= TIM_CR1_ARPE;
	//OC2 signal is output on the corresponding output pin
	TIMx->CCER  |= TIM_CCER_CC2E;
	
	TIMx->BDTR  |= TIM_BDTR_OSSI|TIM_BDTR_OSSR;
	//Calculate CCR1 for Duty Cycle
	TIMx->CCR2 = (Duty_Cycle * ((TIMx->ARR + 1) / 100));
	//Reinitialize the counter and generates an update of the registers
	TIMx->EGR  |= TIM_EGR_UG;
	//OC and OCN outputs are enabled
	TIMx->BDTR |= TIM_BDTR_MOE;
	//Enable TIMx
	TIMx->CR1 |= TIM_CR1_CEN;
}*/


