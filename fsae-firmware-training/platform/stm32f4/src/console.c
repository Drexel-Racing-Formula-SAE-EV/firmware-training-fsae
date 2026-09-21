#include "platform.h"
#include "stm32f4xx_hal.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define RX_RING_CAPACITY 128U

UART_HandleTypeDef huart4;
static volatile uint8_t rx_ring[RX_RING_CAPACITY];
static volatile uint16_t rx_head;
static volatile uint16_t rx_tail;
static uint8_t rx_byte;

void console_init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_UART4_CLK_ENABLE();

    gpio.Pin = GPIO_PIN_10 | GPIO_PIN_11;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.Alternate = GPIO_AF8_UART4;
    HAL_GPIO_Init(GPIOC, &gpio);

    huart4.Instance = UART4;
    huart4.Init.BaudRate = 115200U;
    huart4.Init.WordLength = UART_WORDLENGTH_8B;
    huart4.Init.StopBits = UART_STOPBITS_1;
    huart4.Init.Parity = UART_PARITY_NONE;
    huart4.Init.Mode = UART_MODE_TX_RX;
    huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart4.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart4) != HAL_OK) {
        platform_panic();
    }
}

void console_write(const char *text)
{
    if (text == NULL) {
        return;
    }
    (void)HAL_UART_Transmit(&huart4, (uint8_t *)text, (uint16_t)strlen(text), HAL_MAX_DELAY);
}

void console_printf(const char *format, ...)
{
    char buffer[160];
    va_list args;
    va_start(args, format);
    int length = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    if (length <= 0) {
        return;
    }
    buffer[sizeof(buffer) - 1U] = '\0';
    console_write(buffer);
}

void console_rx_start(void)
{
    rx_head = 0U;
    rx_tail = 0U;
    HAL_NVIC_SetPriority(UART4_IRQn, 5U, 0U);
    HAL_NVIC_EnableIRQ(UART4_IRQn);
    if (HAL_UART_Receive_IT(&huart4, &rx_byte, 1U) != HAL_OK) {
        platform_panic();
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *uart)
{
    if (uart->Instance != UART4) {
        return;
    }
    uint16_t next = (uint16_t)((rx_head + 1U) % RX_RING_CAPACITY);
    if (next != rx_tail) {
        rx_ring[rx_head] = rx_byte;
        rx_head = next;
    }
    (void)HAL_UART_Receive_IT(&huart4, &rx_byte, 1U);
}

bool console_read_line(char *line, size_t capacity)
{
    static size_t length;
    if ((line == NULL) || (capacity < 2U)) {
        return false;
    }

    while (rx_tail != rx_head) {
        char ch = (char)rx_ring[rx_tail];
        rx_tail = (uint16_t)((rx_tail + 1U) % RX_RING_CAPACITY);
        if ((ch == '\r') || (ch == '\n')) {
            if (length == 0U) {
                continue;
            }
            line[length] = '\0';
            length = 0U;
            return true;
        }
        if ((ch == '\b') || (ch == 0x7f)) {
            if (length > 0U) {
                length--;
            }
        } else if (length < (capacity - 1U)) {
            line[length++] = ch;
        }
    }
    return false;
}

