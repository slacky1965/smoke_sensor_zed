#include "tl_common.h"
#include "zcl_include.h"

#include "app_main.h"

static void voltage_divisor_en(uint8_t on) {
    if (on) drv_gpio_write(VOLTAGE_DIVISOR_PIN_EN, ON);
    else drv_gpio_write(VOLTAGE_DIVISOR_PIN_EN, OFF);
}
// 2200..3100 mv - 0..100%
static uint8_t get_battery_level(uint16_t battery_mv) {
    /* Zigbee 0% - 0x0, 50% - 0x64, 100% - 0xc8 */
    uint16_t battery_level = 0;
    if (battery_mv > MIN_VBAT_MV) {
        battery_level = (battery_mv - MIN_VBAT_MV) / ((MAX_VBAT_MV - MIN_VBAT_MV) / 0xC8);
        if (battery_level > 0xC8)
            battery_level = 0xC8;
    }
    return battery_level;
}

int32_t batteryCb(void *arg) {

    voltage_divisor_en(ON);
    drv_adc_enable(ON);

    uint16_t voltage_raw = drv_get_adc_data();
    uint8_t voltage = (uint8_t)(voltage_raw/100);
    uint8_t level = get_battery_level(voltage_raw);

#if UART_PRINTF_MODE && DEBUG_BATTERY
    printf("Voltage_raw: %d\r\n", voltage_raw);
    printf("Voltage:     %d\r\n", voltage);
    printf("Level:       %d\r\n", level);
#endif

    zcl_setAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_GEN_POWER_CFG, ZCL_ATTRID_BATTERY_VOLTAGE, &voltage);
    zcl_setAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_GEN_POWER_CFG, ZCL_ATTRID_BATTERY_PERCENTAGE_REMAINING, &level);

    drv_adc_enable(OFF);
    voltage_divisor_en(OFF);

    return 0;
}

void battery_init() {

    voltage_divisor_en(OFF);
    drv_adc_init();
    drv_adc_mode_pin_set(DRV_ADC_BASE_MODE, VOLTAGE_DETECT_ADC_PIN);
}
