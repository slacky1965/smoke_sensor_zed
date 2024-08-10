#ifndef SRC_INCLUDE_BOARD_8258_DIY_H_
#define SRC_INCLUDE_BOARD_8258_DIY_H_

/************************* Configure SMOKE GPIO **************************************/
#define ALARM_GPIO              GPIO_PC2
#define PC2_FUNC                AS_GPIO
#define PC2_OUTPUT_ENABLE       OFF
#define PC2_INPUT_ENABLE        ON
#define PULL_WAKEUP_SRC_PC2     PM_PIN_PULLDOWN_100K

#define TAMPER_GPIO             GPIO_PC3
#define PC3_FUNC                AS_GPIO
#define PC3_OUTPUT_ENABLE       OFF
#define PC3_INPUT_ENABLE        ON
#define PULL_WAKEUP_SRC_PC3     PM_PIN_PULLUP_1M

/* tamper switch type 1 - NC, 0 - NO */
#define TAMPER_SWITCH_NC        0

/********************* Configure External Battery GPIO ******************************/
#define VOLTAGE_DETECT_PIN      GPIO_PB1
#define VOLTAGE_DIVISOR_PIN_EN  GPIO_PA1
#define PA1_FUNC                AS_GPIO
#define PA1_OUTPUT_ENABLE       ON
#define PA1_INPUT_ENABLE        OFF
#define PULL_WAKEUP_SRC_PA1     PM_PIN_PULLDOWN_100K

/************************* Configure KEY GPIO ***************************************/
#define MAX_BUTTON_NUM  1

#define BUTTON1                 GPIO_PC1
#define PC1_FUNC                AS_GPIO
#define PC1_OUTPUT_ENABLE       OFF
#define PC1_INPUT_ENABLE        ON
#define PULL_WAKEUP_SRC_PC1     PM_PIN_PULLUP_1M

//#define PM_WAKEUP_LEVEL         PM_WAKEUP_LEVEL_LOW
//#define PM_WAKEUP_LEVEL_ALARM   PM_WAKEUP_LEVEL_HIGH

enum {
    VK_SW1 = 0x01,
};

#define KB_MAP_NORMAL   {\
        {VK_SW1,}}

#define KB_MAP_NUM      KB_MAP_NORMAL
#define KB_MAP_FN       KB_MAP_NORMAL

#define KB_DRIVE_PINS  {NULL }
#define KB_SCAN_PINS   {BUTTON1}


/**************************** Configure UART ***************************************
*    UART_TX_PA2 = GPIO_PA2,
*    UART_TX_PB1 = GPIO_PB1,
*    UART_TX_PC2 = GPIO_PC2,
*    UART_TX_PD0 = GPIO_PD0,
*    UART_TX_PD3 = GPIO_PD3,
*    UART_TX_PD7 = GPIO_PD7,
*    UART_RX_PA0 = GPIO_PA0,
*    UART_RX_PB0 = GPIO_PB0,
*    UART_RX_PB7 = GPIO_PB7,
*    UART_RX_PC3 = GPIO_PC3,
*    UART_RX_PC5 = GPIO_PC5,
*    UART_RX_PD6 = GPIO_PD6,
*/

#if UART_PRINTF_MODE
#define DEBUG_INFO_TX_PIN       UART_TX_PD7//print
#define BAUDRATE                115200
#endif /* UART_PRINTF_MODE */

/**************************** Configure LED ******************************************/

#define LED1                    GPIO_PB5
#define PB5_FUNC                AS_GPIO
#define PB5_OUTPUT_ENABLE       ON
#define PB5_INPUT_ENABLE        OFF

/**************************************************************************************/

#endif /* SRC_INCLUDE_BOARD_8258_DIY_H_ */
