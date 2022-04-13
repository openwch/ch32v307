# 32-bit Interconnected RISC-V MCU CH32V307
EN | [中文](README_zh.md)



### Overview
The CH32V305 and CH32V307 are interconnected microcontrollers, based on 32-bit RISC-V core, with hardware stack area and fast interrupt entry. Compared with standard RISC-V, the interrupt response speed is greatly improved. With single-precision float point instruction sets added and stack area extended, the CH32V305/7 has higher performance, the number of U(S)ART is extended to 8, and the number of motor timer is extended to 4. The CH32V305/7 provides USB2.0 high-speed interface (480Mbps) and has built-in PHY transceiver. Ethernet MAC is upgraded to GbE and integrates 10M PHY module.

### System Block Diagram
<img src="image/frame.jpg" alt="frame" style="zoom:50%;" />
 
### Features
- RISC-V4F processor, max 144MHz system clock frequency;
- Single-cycle multiplication and hardware division, hardware float point unit (FPU) ;
- 64KB SRAM，256KB Flash;
- Supply voltage: 2.5V/3.3V, GPIO unit is supplied independently;
- Multiple low-power modes: sleep/stop/standby;
- Power-on/power-down reset (POR/PDR), programmable voltage detector (PVD);
- 2 general DMA controllers, 18 channels in total;
- 4 amplifiers;
- Single true random number generator (TRNG)
- 2 x 12-bit DAC;
- 2-unit 16-channel 12-bit ADC, 16-channel TouchKey;
- 10 timers;
- USB2.0 full-speed OTG interface;
- USB2.0 high-speed host/device interface (built-in 480Mbps PHY)
- 3 USARTs, 5 UARTs;
- 2 CAN interfaces (2.0B active);
- SDIO interface, FSMC interface, DVP;
- 2 x I2C, 3 x SPI, 2 x IIS;
- 80 I/O ports, can be mapped to 16 external interrupts;
- CRC calculation unit, 96-bit unique chip ID;
- Serial 2-wire debug interface;
- Packages: LQFP64M, LQFP100.


