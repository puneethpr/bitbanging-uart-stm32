#include <stdint.h>
	#define RCC_BASE 0x40021000 //Base address of clock
	#define GPIOA_BASE 0x50000000 //Base address of PORT A
	#define TIM6_BASE 0x40001000 //Timer Base
	#define NVIC_ISER0 ((volatile uint32_t*)0xE000E100)
	#define RCC_IOPENR (*(volatile uint32_t*)(RCC_BASE + 0x34))
	#define RCC_APB1ENR (*(volatile uint32_t*)(RCC_BASE + 0x3C))
	#define GPIOA_MODER (*(volatile uint32_t*)(GPIOA_BASE + 0x00)) //GPIOA mode
	#define GPIOA_ODR (*(volatile uint32_t*)(GPIOA_BASE + 0x14)) //data reg port a
	#define TIM6_CR1 (*(volatile uint32_t*)(TIM6_BASE + 0x00)) //TIM6 control register 1
	#define TIM6_DIER (*(volatile uint32_t*)(TIM6_BASE + 0x0C)) //TIM6 interrupt enable register
	#define TIM6_SR (*(volatile uint32_t*)(TIM6_BASE + 0x10))  // TIM6 status register
	#define TIM6_CNT (*(volatile uint32_t*)(TIM6_BASE + 0x24))  // counter value
	#define TIM6_ARR (*(volatile uint32_t*)(TIM6_BASE + 0x2C))   // auto reload value
	#define TIM6_PSC (*(volatile uint32_t*)(TIM6_BASE + 0x28))  //prescaler for TIM6

	volatile uint8_t count = 10;
	uint16_t frame_buf=0;


	void init_gpio(void)
	{
		RCC_IOPENR = RCC_IOPENR | (1 << 0); // Enable GPIOA clock
		GPIOA_MODER = GPIOA_MODER  & ~(0x3 << (6 * 2)); // Clear MODER6
		GPIOA_MODER = GPIOA_MODER | (0x1 << (6 * 2)); // Set PA6 as output
	}
	void init_tim6(void)
	{
		RCC_APB1ENR = RCC_APB1ENR | (1 << 4); // Enable TIM6 clock
		TIM6_ARR = 103; // Auto-reload value for overflow
		TIM6_CNT = 0; // Reset counter
		TIM6_PSC = 15; // setting prescaler to 16. 16Mhz (clock for tim6) to 1Mhz
		TIM6_DIER = TIM6_DIER | (1 << 0); // Enable update interrupt
		*NVIC_ISER0 = *NVIC_ISER0 | (1 << 17); // Enable TIM6 interrupt in NVIC
		TIM6_CR1 = TIM6_CR1 & ~1; // stop timer
	}
	void TIM6_DAC_LPTIM1_IRQHandler(void)
	{
		TIM6_SR &= ~(1 << 0); // Clear update interrupt flag
		if (frame_buf & 1)
			GPIOA_ODR = GPIOA_ODR | (1<<6); //make the GPIOA pin HIGH
		else
			GPIOA_ODR = GPIOA_ODR & ~(1<<6); // make GPIOA pin LOW
		frame_buf=frame_buf>>1;    //shifting the Bit to right
		count++;

		if(count == 10)   // if 10 bits are sent then stop the timer
		{
			TIM6_CR1 = TIM6_CR1 & ~1; // stop timer
		}
	}
	int main(void) {
		init_gpio();
		init_tim6();
		char *str = "Hello World";
		char *ptr = str;
		while (*ptr)
		{
			if(count==10) //change data in buff only if 10 bits are sent
			{
				frame_buf = 0; // start bit = 0
				frame_buf = frame_buf | (*ptr << 1); // data bits
				frame_buf = frame_buf | (1 << 9); // stop bit = 1 count = 0;
				ptr++;
				count=0;
				TIM6_CR1 = TIM6_CR1 | 1; // start timer
			}
		}
		while(1)
		{

		}
	}

