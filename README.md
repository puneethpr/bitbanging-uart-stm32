[README.md](https://github.com/user-attachments/files/23833416/README.md)
Bit-Banged UART on STM32 (Bare-Metal)

This project implements a fully software-based UART transmitter on an STM32G070 (bare-metal, no HAL).  
Transmission timing is generated purely using TIM6 interrupts, and bits are shifted manually through GPIO.



Features
- Software UART (TX only)
- 9600 baud implementation
- TIM6 configured for precise 104us intervals
- Manual UART framing:
  - Start bit
  - 8 data bits (LSB first)
  - Stop bit
- Verified using logic analyzer (PulseView)
- Data received successfully on ESP8266 (UART RX)
- ESP8266 forwards received bytes to PC over USB serial (TeraTerm / RealTerm)


Hardware Used
- STM32G070RBT6 (Bare-metal programming)
- ESP8266 (UART receiver)
- Logic analyzer (24 MHz, 8-channel) for debugging
- USB serial tools (TeraTerm / RealTerm)



Project Summary
A custom UART protocol was implemented without using the hardware USART peripheral.  
Instead, TIM6 generates fixed-time slices (104us for 9600 baud), and each interrupt outputs one bit of the UART frame on GPIOA pin.

A logic analyzer was used to confirm that:
- Bit timing is accurate
- Framing is correct
- ESP8266 decodes the data correctly



Working (Short Explanation)
1. Initialize GPIOA as output for TX  
2. Configure TIM6:
   - PSC = 15 (16Mhz/15+1 = 1 MHz timer tick) 
   - ARR = 103 - 104 �s per interrupt  
3. On each interrupt:
   - Output frame_buf & 1 to TX pin
   - Shift frame_buf >>= 1
4. After 10 bits (start + 8 data + stop), stop timer  
5. Load next byte and restart timer


Verification
Captured waveforms using a 24 MHz logic analyzer showed accurate 104 �s bit timing and stable UART frames.

ESP8266 RX successfully decoded all characters and printed them to the PC. 

