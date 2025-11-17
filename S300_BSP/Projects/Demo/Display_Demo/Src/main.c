/*
 * 文件：main.c
 * 职责：
 *  - 板级与时钟初始化、SysTick 1ms 节拍；
 *  - 将毫秒节拍以回调形式交给应用编排层 display_demo_app；
 *  - 主循环仅做 app tick 调度与简易节拍控制，保持 main 简洁清晰。
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
/* Platform */
#include "s300.h"
/* Board and subsystems */
#include "rcc.h"
#include "board.h"
#include "video.h"
/* LVGL */
#include "lvgl.h"
/* App orchestration */
#include "display_demo_app.h"
#include "psram.h"
#include "perf.h"
#include "uart.h"
#include "uart_buffer.h"

// 1ms 节拍计时
static volatile uint32_t g_tick_ms = 0;
volatile uint32_t g_cpu_total_ticks = 0;
volatile uint32_t g_cpu_idle_ticks = 0;
volatile uint8_t  g_cpu_in_idle = 0;

void SysTick_Handler(void)
{
    g_tick_ms++;
    lv_tick_inc(1);
    /* CPU usage metering: 1ms tick granularity */
    g_cpu_total_ticks++;
    if (g_cpu_in_idle) g_cpu_idle_ticks++;
}

static inline uint32_t millis(void)
{
    return g_tick_ms;
}

extern uart_ring_buffer_t uart3_rx_buffer;
int main(void)
{
    /* Board init: clocks + debug UART for printf */
    board_init();
    printf("\r\n[S300][DisplayDemo] Booting...\r\n");

    SystemCoreClockUpdate();
    if (SysTick_Config(SystemCoreClock / 1000U) != 0U) { printf("[S300][DisplayDemo][ERR] SysTick_Config failed!\r\n"); }





    uart3_buffer_init();

    char line[128] = {0};

    while (1)
    {
        /* 方式1：字节读 */
        // uint8_t byte;
        // if (uart_buffer_read_byte(&uart3_rx_buffer, &byte)) {
        //     uart_write(3, UARTTYPE_STD_SERIAL, byte);  // 回显
        // }

        // if (uart_buffer_has_line(&uart3_rx_buffer)) 
        {
            // 有完整行
            /* 方式2：行读（推荐用于命令） */
            int len = uart_buffer_read_line(&uart3_rx_buffer, line, sizeof(line));
            if (len > 0) {
                printf("Recv: %s [%d]\r\n", line, len);
                for(int i = 0; i < len; i++) {
                    uart_write(3, UARTTYPE_STD_SERIAL, (uint8_t)line[i]);
                }
    
                if (strcmp(line, "clear") == 0) {
                    uart_buffer_clear(&uart3_rx_buffer);
                    printf("Buffer cleared!\r\n");
                }
            }
    
            /* 方式3：查空 */
            if (uart_buffer_is_empty(&uart3_rx_buffer)) {
                // 无数据
            }
        }

        delay_ms(10);
    }





    // init_psram(4,1);
    // rcc_init_mm_pll(8, 400, 0, 3, 2); /* 100MHz */
    // rcc_init_dsp_pll(6, 768, 0, 2, 2); /* 300MHz */

    // /* App init: camera/video/mailbox/ui/eyes/face_tracker */
    // display_demo_app_init(millis);

    while (1)
    {
        // display_demo_app_tick();
        // /* Mark idle section for CPU usage metering (spin-wait ~5ms) */
        // g_cpu_in_idle = 1;
        // uint32_t t0 = millis();
        // while ((uint32_t)(millis() - t0) < 5u) { /* idle spin */ }
        // g_cpu_in_idle = 0;
    }
}
 

void UART3_IRQHandler(void)
{
    uart_interupt_fun();
}
