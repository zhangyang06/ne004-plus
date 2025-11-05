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

// 1ms 节拍计时
static volatile uint32_t g_tick_ms = 0;

void SysTick_Handler(void)
{
    g_tick_ms++;
    lv_tick_inc(1);
}

static inline uint32_t millis(void)
{
    return g_tick_ms;
}
#define test_addr         (0x80010000U)//0x80000000;
int main(void)
{
    /* Board init: clocks + debug UART for printf */
    board_init();
    printf("\r\n[S300][DisplayDemo] Booting...\r\n");

    SystemCoreClockUpdate();
    if (SysTick_Config(SystemCoreClock / 1000U) != 0U) { printf("[S300][DisplayDemo][ERR] SysTick_Config failed!\r\n"); }

    init_psram(4,1);
    rcc_init_mm_pll(8, 400, 0, 3, 2); /* 100MHz */
    rcc_init_dsp_pll(6, 768, 0, 2, 2); /* 300MHz */

//    uint8_t *byte_ptr = (uint8_t *)test_addr;
//    int j = 0;

   // 写入数据
    // for (int i = 0; i < 320; i++)
    // {
    //     byte_ptr[i] = (uint8_t)j;
    //     if(j == 127)
    //     {
    //         j = 0;
    //     }
    //     else
    //     {
    //         j++;
    //     }
    // }

    // // 读取验证
    // for(int i = 0; i < 320; i++)
    // {
    //     printf("addr: 0x%08X, val: 0x%02X\r\n", 
    //         (unsigned int)(byte_ptr + i), 
    //         byte_ptr[i]);
    // }

// while (1)
// {
//     /* code */
// }


    /* App init: camera/video/mailbox/ui/eyes/face_tracker */
    display_demo_app_init(millis);

    while (1)
    {
        display_demo_app_tick();
        uint32_t t0 = millis();
        while ((uint32_t)(millis() - t0) < 5u) { /* spin */ }
    }
}
 