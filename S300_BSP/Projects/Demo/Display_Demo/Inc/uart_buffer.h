#ifndef UART_BUFFER_H
#define UART_BUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define UART_BUFFER_SIZE    32   // 可根据 RAM 调整

typedef struct {
    uint8_t  buffer[UART_BUFFER_SIZE];
    volatile uint16_t head;   // 写指针（中断写）
    volatile uint16_t tail;   // 读指针（主循环读）
    volatile uint16_t count;  // 当前数据量
} uart_ring_buffer_t;

/* 初始化缓冲区 */
void uart_buffer_init(uart_ring_buffer_t *rb);

/* 写入一个字节（中断安全） */
bool uart_buffer_write(uart_ring_buffer_t *rb, uint8_t data);

/* 读取一个字节（返回 true 表示成功） */
bool uart_buffer_read_byte(uart_ring_buffer_t *rb, uint8_t *data);

bool uart_buffer_has_line(const uart_ring_buffer_t *rb);

/* 读取一行（以 \n 或 \r\n 结尾），返回实际长度（不含换行符） */
int uart_buffer_read_line(uart_ring_buffer_t *rb, char *line, int max_len);

/* 清空缓冲区 */
void uart_buffer_clear(uart_ring_buffer_t *rb);

/* 查询状态 */
bool uart_buffer_is_empty(const uart_ring_buffer_t *rb);
bool uart_buffer_is_full(const uart_ring_buffer_t *rb);
uint16_t uart_buffer_available(const uart_ring_buffer_t *rb);

#endif