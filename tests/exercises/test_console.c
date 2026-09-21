#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "platform.h"
/* Include actual sources to inspect private counters and inject ISR bytes. */
#include TRAINING_CONSOLE_SOURCE
#include "../../platform/stm32f4/src/timebase.c"
#include "../../platform/stm32f4/src/adc_input.c"

static HAL_StatusTypeDef rx_result = HAL_OK, tx_result = HAL_OK;
void platform_panic(void) { assert(0); }
uint32_t HAL_GetTick(void) { return 0; }
void HAL_GPIO_Init(void *p, GPIO_InitTypeDef *g) { (void)p; (void)g; }
void HAL_NVIC_SetPriority(int n, unsigned p, unsigned s) { (void)n; (void)p; (void)s; }
void HAL_NVIC_EnableIRQ(int n) { (void)n; }
HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *u) { (void)u; return HAL_OK; }
HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *u, uint8_t *b, uint16_t n, uint32_t t)
{ (void)u; (void)b; (void)n; assert(t == 100U); return tx_result; }
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *u, uint8_t *b, uint16_t n)
{ (void)u; (void)b; (void)n; return rx_result; }
HAL_StatusTypeDef HAL_UART_AbortReceive(UART_HandleTypeDef *u) { (void)u; return HAL_OK; }
static void feed(const char *s)
{ for (; *s; s++) { rx_byte = (uint8_t)*s; HAL_UART_RxCpltCallback(&huart4); } }

int main(void)
{
    char line[64];
    console_init(); console_rx_start();
    feed("help\r\nstatus\n");
    assert(console_read_line(line, sizeof line) && strcmp(line, "help") == 0);
    assert(console_read_line(line, sizeof line) && strcmp(line, "status") == 0);
    feed("helx\bp\n");
    assert(console_read_line(line, sizeof line) && strcmp(line, "help") == 0);
    feed("sta"); assert(!console_read_line(line, sizeof line));
    feed("tus\n"); assert(console_read_line(line, sizeof line) && strcmp(line,"status") == 0);
    for (unsigned i=0; i<80; i++) { feed("a"); }
    feed("\n"); assert(!console_read_line(line,sizeof line));
    feed("help\n"); assert(console_read_line(line,sizeof line));
    feed("status\n"); assert(!console_read_line(line,3));
    for (unsigned i=0; i<200; i++) { feed("a"); }
    assert(!console_read_line(line,sizeof line) && rx_overflows > 0);
    feed("\nhelp\n"); assert(console_read_line(line,sizeof line) && strcmp(line,"help")==0);
    feed("hel"); assert(!console_read_line(line,sizeof line));
    HAL_UART_ErrorCallback(&huart4);
    assert(!console_read_line(line,sizeof line) && !rx_recover);
    feed("\nhelp\n"); assert(console_read_line(line,sizeof line));
    rx_result = HAL_ERROR; feed("x"); assert(rx_recover);
    rx_result = HAL_OK; assert(!console_read_line(line,sizeof line));
    feed("\n"); assert(!console_read_line(line,sizeof line));
    tx_result = HAL_ERROR; console_write("x"); assert(tx_errors == 1);
    uint32_t last=0, missed=0;
    assert(!periodic_due(9,&last,10,&missed));
    assert(periodic_due(35,&last,10,&missed) && last==30 && missed==2);
    assert(!periodic_due(35,&last,10,&missed));
    last=UINT32_MAX-4U; missed=0;
    assert(periodic_due(5,&last,10,&missed) && last==5 && missed==0);
    assert(!periodic_due(5,&last,0,&missed));
    assert(time_elapsed(5,UINT32_MAX-4U,10));
    uint16_t raw;
    assert(adc_input_read(&raw)==ADC_INPUT_OK && raw==310);
    adc_input_sim_set_running(false); assert(adc_input_read(&raw)==ADC_INPUT_NO_UPDATE);
    adc_input_sim_set_valid(false); assert(adc_input_read(&raw)==ADC_INPUT_INVALID);
    adc_input_sim_set(4096); assert(adc_input_read(&raw)==ADC_INPUT_INVALID);
    adc_input_sim_set(931); assert(adc_input_read(&raw)==ADC_INPUT_OK && raw==931);
    assert(adc_input_read(NULL)==ADC_INPUT_INVALID);
    puts("actual console/timebase/simulated acquisition host tests: PASS");
}
