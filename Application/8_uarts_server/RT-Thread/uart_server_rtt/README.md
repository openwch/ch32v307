# CH32V307V-R0开发板 BSP 说明

## 简介

主要内容如下：

- 开发板资源介绍
- BSP 快速上手
- 进阶使用方法

通过阅读快速上手章节开发者可以快速地上手该 BSP，将 RT-Thread 运行在开发板上。在进阶使用指南章节，将会介绍更多高级功能，帮助开发者利用 RT-Thread 驱动更多板载资源。

## 开发板介绍

CH32V307V-R0 是 南京沁恒微电子(WCH) 推出的一款基于 RISC-V 内核的开发板，最高主频为 120Mhz，该开发板芯片为 CH32V307-R0。

开发板外观如下图所示：

![board](figures/board.png)

该开发板常用 **板载资源** 如下：

- MCU：CH32V307VCT6，主频 144MHz，FLASH和RAM四种配置
  288KB FLASH ，32KB RAM
  256KB FLASH ，64KB RAM
  224KB FLASH ，96KB RAM
  192KB FLASH ，128KB RAM
- 常用外设
  - LED：2个用户 LEDs， LED1（blue），LED2(red)。
  - LED：power LED， D1。
  - 按键，3个，Reset， User 和 Download。
  - USB，2个，Tpye-C; 2个， USB-A
  - 网口，1个，内置 10M PHY

开发板更多详细信息请参考 WCH 官方[开发板介绍]EVT包->PUB->CH32V307评估板说明书.pdf (http://www.wch.cn/downloads/CH32V307EVT_ZIP.html)。

## 外设支持

本 BSP 目前对外设的支持情况如下：

| **片上外设** | **支持情况** | **备注**                                  |
| GPIO         |     支持     | PA PB PC PD PE                               |
| UART         |     支持     | UART0/1/2/3/4/5/6/7/8                                 |
| SDIO         |     即将支持     |                                           |
| ADC          |     支持     | 10bit ADC1/2                                 |
| DAC          |     支持     | 10bit DAC channel 1/2                                 |
| ETH          |     支持     | 10M                                 |
| SPI          |   即将支持   |                                  |
| I2C          |     即将支持     |                                   |
| RTC          |     即将支持     |                                           |
| WDT          |     即将支持     |                                           |
| FLASH        |   即将支持   |                                   |
| TIMER        |     即将支持     |                                           |
| PWM          |     即将支持     | |
| USB Device   |   即将支持   |                                           |
| USB Host     |   即将支持   |                                           |

## 使用说明

使用说明分为如下两个章节：

- 快速上手

    本章节是为刚接触 RT-Thread 的新手准备的使用说明，遵循简单的步骤即可将 RT-Thread 操作系统运行在该开发板上，看到实验效果 。

- 进阶使用

    本章节是为需要在 RT-Thread 操作系统上使用更多开发板资源的开发者准备的。通过使用 ENV 工具对 BSP 进行配置，可以开启更多板载资源，实现更多高级功能。


### 快速上手

本 BSP 为开发者提供 GCC 开发环境。下面介绍如何将系统运行起来。

#### 硬件连接

使用数据线连接开发板到 PC，打开电源开关。

#### 编译下载

通过 `RT-Thread Studio` 或者 `scons` 编译得到 `.dcf` 固件，通过 `Downloader` 进行下载

#### 运行结果

下载程序成功之后，系统会自动运行，观察开发板上 LED 的运行效果，红色 LED 会周期性闪烁。

连接开发板对应串口到 PC , 在终端工具里打开相应的串口（115200-8-1-N），复位设备后，可以看到 RT-Thread 的输出信息:

```bash
 \ | /
- RT -     Thread Operating System
 / | \     4.0.4 build Sep  1 2021
 2006 - 2021 Copyright by rt-thread team
adc1 init successdacc1 init success

 MCU: CH32V307

 SysClk: 144000000Hz

 www.wch.cn

msh >
```
### 进阶使用

此 BSP 默认只开启了 GPIO 和 串口1 的功能，如果需使用 ETH、ADC 等更多高级功能，需要利用 ENV 工具对BSP 进行配置，步骤如下：

1. 在 bsp 下打开 env 工具。

2. 输入`menuconfig`命令配置工程，配置好之后保存退出。

3. 输入`pkgs --update`命令更新软件包。

4. 输入`scons` 命令重新编译工程。

## 注意事项

波特率默认为 115200Kbps，烧录代码需去我司官网(http://www.wch.cn/downloads/WCHISPTool_Setup_exe.html)下载ISP工具进行烧录，之后版本会支持我司WCH-Link-两线SWD下载。


## 维护人信息

-
