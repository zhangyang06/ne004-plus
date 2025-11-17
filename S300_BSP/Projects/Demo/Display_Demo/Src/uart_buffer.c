#include "uart_buffer.h"
#include "uart.h"

/* 初始化 */
void uart_buffer_init(uart_ring_buffer_t *rb)
{
    if (!rb) return;
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

/* 写入一个字节（中断中调用） */
bool uart_buffer_write(uart_ring_buffer_t *rb, uint8_t data)
{
    if (!rb || rb->count >= UART_BUFFER_SIZE) {
        return false;  // 满，丢弃
    }

    rb->buffer[rb->head] = data;
    rb->head = (rb->head + 1) % UART_BUFFER_SIZE;
    rb->count++;

    return true;
}

/* 读取一个字节（主循环调用） */
bool uart_buffer_read_byte(uart_ring_buffer_t *rb, uint8_t *data)
{
    if (!rb || !data || rb->count == 0) {
        return false;
    }

    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % UART_BUFFER_SIZE;
    rb->count--;

    return true;
}

bool uart_buffer_has_line(const uart_ring_buffer_t *rb)
{
    if (!rb || rb->count == 0) return false;

    uint16_t index = rb->count -1;
    uint16_t checked = 0;

    while (checked < rb->count) {
        uint8_t ch = rb->buffer[index];
        if (ch == '\r' || ch == '\n') {
            return true;
        }
        index = (index + 1) % UART_BUFFER_SIZE;
        checked++;
    }
    return false;
}

// 在 uart_buffer.c 中添加：
static bool uart_buffer_peek_byte(uart_ring_buffer_t *rb, uint8_t *data)
{
    if (!rb || !data || rb->count == 0) return false;
    *data = rb->buffer[rb->tail];
    return true;
}

int uart_buffer_read_line(uart_ring_buffer_t *rb, char *line, int max_len)
{
    if (!rb || !line || max_len <= 1 || rb->count == 0) return 0;

    int len = 0;
    uint8_t ch;
    bool line_complete = false;
    
    // 先检查缓冲区中是否有完整的行
    uart_ring_buffer_t temp_rb = *rb; // 创建临时副本进行检查
    int chars_before_eol = 0;
    
    while (uart_buffer_read_byte(&temp_rb, &ch) && chars_before_eol < max_len - 1) {
        chars_before_eol++;
        if (ch == '\r' || ch == '\n') {
            line_complete = true;
            break;
        }
    }
    
    // 如果没有完整的行，返回0
    if (!line_complete && rb->count < max_len - 1) {
        return 0;
    }

    // 读取数据直到行结束符或缓冲区满
    while (uart_buffer_read_byte(rb, &ch) && len < max_len - 1) {
        if (ch == '\r') {
            // 检查下一个字符是否是\n
            uint8_t next_ch;
            if (uart_buffer_peek_byte(rb, &next_ch) && next_ch == '\n') {
                uart_buffer_read_byte(rb, &next_ch); // 消耗掉\n
            }
            break;
        } else if (ch == '\n') {
            break;
        } else {
            line[len++] = (char)ch;
        }
    }

    line[len] = '\0';
    return len;
}

/* 清空缓冲区 */
void uart_buffer_clear(uart_ring_buffer_t *rb)
{
    if (!rb) return;
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

/* 查询状态 */
bool uart_buffer_is_empty(const uart_ring_buffer_t *rb)
{
    return (!rb || rb->count == 0);
}

bool uart_buffer_is_full(const uart_ring_buffer_t *rb)
{
    return (rb && rb->count >= UART_BUFFER_SIZE);
}

uint16_t uart_buffer_available(const uart_ring_buffer_t *rb)
{
    return rb ? rb->count : 0;
}