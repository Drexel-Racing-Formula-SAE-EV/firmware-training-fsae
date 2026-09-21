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
static volatile bool rx_corrupt, rx_recover;
static volatile uint32_t rx_errors, rx_overflows;
static uint32_t tx_errors;
static size_t line_length;
static bool line_discard;
static char pending_line[64];

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
    size_t length = strlen(text);
    if (length > UINT16_MAX) { tx_errors++; return; }
    /* Requires a working SysTick; linked vector entries are checked at build time. */
    if (HAL_UART_Transmit(&huart4, (uint8_t *)text, (uint16_t)length, 100U) != HAL_OK) {
        tx_errors++;
    }
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
    line_length = 0U;
    line_discard = false;
    rx_corrupt = false;
    rx_recover = false;
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
        __DMB();
        rx_head = next;
    } else {
        rx_overflows++;
        rx_corrupt = true;
    }
    if (HAL_UART_Receive_IT(&huart4, &rx_byte, 1U) != HAL_OK) {
        rx_recover = true;
        rx_corrupt = true;
        rx_errors++;
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *uart)
{
    if (uart->Instance == UART4) {
        rx_errors++;
        rx_corrupt = true;
        rx_recover = true;
    }
}

void console_report_errors(void)
{
    console_printf("uart tx_errors=%lu rx_errors=%lu rx_overflows=%lu\r\n",
                   (unsigned long)tx_errors, (unsigned long)rx_errors,
                   (unsigned long)rx_overflows);
}

bool console_read_line(char *line, size_t capacity)
{
    if ((line == NULL) || (capacity < 2U)) {
        return false;
    }

    for (;;) {
        uint32_t mask = __get_PRIMASK();
        __disable_irq();
        if (rx_recover) {
            HAL_StatusTypeDef status = HAL_UART_AbortReceive(&huart4);
            if (status == HAL_OK) { status = HAL_UART_Receive_IT(&huart4, &rx_byte, 1U); }
            rx_recover = status != HAL_OK;
        }
        if (rx_corrupt) {
            rx_tail = rx_head;
            line_length = 0U;
            line_discard = true;
            rx_corrupt = false;
        }
        if (rx_tail == rx_head) {
            __set_PRIMASK(mask);
            break;
        }
        char ch = (char)rx_ring[rx_tail];
        rx_tail = (uint16_t)((rx_tail + 1U) % RX_RING_CAPACITY);
        __set_PRIMASK(mask);
        if ((ch == '\r') || (ch == '\n')) {
            if (line_discard) {
                line_discard = false;
                line_length = 0U;
                console_write("error: input lost or line too long; re-enter command\r\n> ");
                continue;
            }
            if (line_length == 0U) {
                continue;
            }
            if (line_length >= capacity) {
                line_length = 0U;
                console_write("error: output buffer too small\r\n> ");
                continue;
            }
            memcpy(line, pending_line, line_length);
            line[line_length] = '\0';
            line_length = 0U;
            return true;
        }
        if (line_discard) { continue; }
        if ((ch == '\b') || (ch == 0x7f)) {
            if (line_length > 0U) {
                line_length--;
            }
        } else if ((unsigned char)ch < 32U || (unsigned char)ch > 126U) {
            line_discard = true;
        } else if (line_length < sizeof(pending_line) - 1U) {
            pending_line[line_length++] = ch;
        } else {
            line_discard = true;
        }
    }
    return false;
}
