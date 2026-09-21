#include "platform.h"
#include "stm32f4xx_hal.h"

static uint16_t sim_raw = 310U; /* about 25 C for a 10 mV/C sensor */
static bool sim_valid = true;
static bool sim_running = true;

#if !TRAINING_RENODE
static ADC_HandleTypeDef hadc1;
#endif

void adc_input_init(void)
{
#if !TRAINING_RENODE
    GPIO_InitTypeDef gpio = {0};
    ADC_ChannelConfTypeDef channel = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_ADC1_CLK_ENABLE();
    gpio.Pin = GPIO_PIN_1;
    gpio.Mode = GPIO_MODE_ANALOG;
    gpio.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &gpio);

    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1U;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        platform_panic();
    }
    channel.Channel = ADC_CHANNEL_1;
    channel.Rank = 1U;
    channel.SamplingTime = ADC_SAMPLETIME_84CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc1, &channel) != HAL_OK) {
        platform_panic();
    }
#endif
}

bool adc_input_read(uint16_t *raw_counts)
{
    if (raw_counts == NULL) {
        return false;
    }
#if TRAINING_RENODE
    if (!sim_valid || !sim_running) {
        return false;
    }
    *raw_counts = sim_raw;
    return true;
#else
    if (HAL_ADC_Start(&hadc1) != HAL_OK) {
        return false;
    }
    if (HAL_ADC_PollForConversion(&hadc1, 5U) != HAL_OK) {
        (void)HAL_ADC_Stop(&hadc1);
        return false;
    }
    *raw_counts = (uint16_t)HAL_ADC_GetValue(&hadc1);
    (void)HAL_ADC_Stop(&hadc1);
    return true;
#endif
}

void adc_input_sim_set(uint16_t raw_counts)
{
    sim_raw = raw_counts;
    sim_valid = raw_counts <= 4095U;
    sim_running = true;
}

void adc_input_sim_set_valid(bool valid)
{
    sim_valid = valid;
    if (valid) {
        sim_running = true;
    }
}

void adc_input_sim_set_running(bool running)
{
    sim_running = running;
}

