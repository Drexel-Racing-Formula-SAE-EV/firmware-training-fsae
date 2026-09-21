#include "platform.h"
#include "stm32f4xx_hal.h"

#include <string.h>

#define CAN_RX_CAPACITY 8U

CAN_HandleTypeDef hcan1;
static volatile can_frame_t rx_ring[CAN_RX_CAPACITY];
static volatile uint8_t rx_head;
static volatile uint8_t rx_tail;
static volatile can_bus_stats_t bus_stats;

void can_bus_init(void)
{
    GPIO_InitTypeDef gpio = {0};
    CAN_FilterTypeDef filter = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_CAN1_CLK_ENABLE();

    gpio.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOB, &gpio);

    hcan1.Instance = CAN1;
    hcan1.Init.Prescaler = 6U;
    hcan1.Init.Mode = CAN_MODE_NORMAL;
    hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
    hcan1.Init.TimeSeg1 = CAN_BS1_9TQ;
    hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
    hcan1.Init.TimeTriggeredMode = DISABLE;
    hcan1.Init.AutoBusOff = ENABLE;
    hcan1.Init.AutoWakeUp = DISABLE;
    hcan1.Init.AutoRetransmission = ENABLE;
    hcan1.Init.ReceiveFifoLocked = DISABLE;
    hcan1.Init.TransmitFifoPriority = DISABLE;
#if TRAINING_RENODE
    /* Renode's bxCAN reset state starts in sleep mode and requires SLEEP to
       be cleared before INRQ can be acknowledged. Real bxCAN accepts the HAL
       sequence that clears SLEEP after entering initialization mode. */
    CLEAR_BIT(CAN1->MCR, CAN_MCR_SLEEP);
#endif
    if (HAL_CAN_Init(&hcan1) != HAL_OK) {
        platform_panic();
    }

    /* Accept all frames here so the training protocol can demonstrate and
       count identifier, frame-type and DLC rejection in application code. */
    filter.FilterBank = 0U;
    filter.FilterMode = CAN_FILTERMODE_IDMASK;
    filter.FilterScale = CAN_FILTERSCALE_32BIT;
    filter.FilterIdHigh = 0U;
    filter.FilterIdLow = 0U;
    filter.FilterMaskIdHigh = 0U;
    filter.FilterMaskIdLow = 0U;
    filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    filter.FilterActivation = ENABLE;
    filter.SlaveStartFilterBank = 14U;
    if (HAL_CAN_ConfigFilter(&hcan1, &filter) != HAL_OK) {
        platform_panic();
    }

    rx_head = 0U;
    rx_tail = 0U;
    memset((void *)&bus_stats, 0, sizeof(bus_stats));
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 6U, 0U);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
    if ((HAL_CAN_Start(&hcan1) != HAL_OK) ||
        (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)) {
        platform_panic();
    }
}

bool can_bus_send(const can_frame_t *frame)
{
    if ((frame == NULL) || (frame->dlc > 8U) ||
        ((!frame->extended) && (frame->id > 0x7FFU)) ||
        (frame->extended && (frame->id > 0x1FFFFFFFU))) {
        bus_stats.tx_errors++;
        return false;
    }

    CAN_TxHeaderTypeDef header = {0};
    uint32_t mailbox;
    header.StdId = frame->extended ? 0U : frame->id;
    header.ExtId = frame->extended ? frame->id : 0U;
    header.IDE = frame->extended ? CAN_ID_EXT : CAN_ID_STD;
    header.RTR = frame->remote ? CAN_RTR_REMOTE : CAN_RTR_DATA;
    header.DLC = frame->dlc;
    header.TransmitGlobalTime = DISABLE;
    if (HAL_CAN_AddTxMessage(&hcan1, &header, (uint8_t *)frame->data, &mailbox) != HAL_OK) {
        bus_stats.tx_errors++;
        return false;
    }
    bus_stats.tx_frames++;
    return true;
}

bool can_bus_receive(can_frame_t *frame)
{
    if (frame == NULL) {
        return false;
    }
    uint32_t mask = __get_PRIMASK();
    __disable_irq();
    if (rx_tail == rx_head) {
        __set_PRIMASK(mask);
        return false;
    }
    *frame = rx_ring[rx_tail];
    rx_tail = (uint8_t)((rx_tail + 1U) % CAN_RX_CAPACITY);
    __set_PRIMASK(mask);
    return true;
}

void can_bus_get_stats(can_bus_stats_t *stats)
{
    if (stats == NULL) {
        return;
    }
    uint32_t mask = __get_PRIMASK();
    __disable_irq();
    *stats = bus_stats;
    __set_PRIMASK(mask);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *can)
{
    if (can->Instance != CAN1) {
        return;
    }

    CAN_RxHeaderTypeDef header;
    uint8_t data[8];
    if (HAL_CAN_GetRxMessage(can, CAN_RX_FIFO0, &header, data) != HAL_OK) {
        bus_stats.rx_errors++;
        return;
    }

    uint8_t next = (uint8_t)((rx_head + 1U) % CAN_RX_CAPACITY);
    if (next == rx_tail) {
        bus_stats.rx_overflows++;
        return;
    }
    can_frame_t *slot = (can_frame_t *)&rx_ring[rx_head];
    slot->id = (header.IDE == CAN_ID_EXT) ? header.ExtId : header.StdId;
    slot->dlc = (uint8_t)header.DLC;
    memcpy(slot->data, data, sizeof(slot->data));
    slot->extended = header.IDE == CAN_ID_EXT;
    slot->remote = header.RTR == CAN_RTR_REMOTE;
    __DMB();
    rx_head = next;
    bus_stats.rx_frames++;
}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *can)
{
    if (can->Instance == CAN1) {
        bus_stats.rx_errors++;
    }
}
