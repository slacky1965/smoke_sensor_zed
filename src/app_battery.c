#include "tl_common.h"
#include "zcl_include.h"

#include "app_main.h"

#define VOLTAGE_FACTOR  120

static uint8_t voltage_factor = VOLTAGE_FACTOR;

/*
 * 9v--[200k]-----[100k]--
 *             |         |
 *            ADC       GND
 *
 *            3040-3060 - 9.0V
 *            3010-3030 - 8.9V
 *            2970-2990 - 8.8V
 *            2940-2960 - 8.7V
 *            2910-2930 - 8.6V
 *            2870-2900 - 8.5V
 *            2840-2860 - 8.4V
 *            2800-2830 - 8.3V
 *            2770-2790 - 8.2V
 *            2740-2760 - 8.1V
 *            2700-2730 - 8.0V
 *            2670-2690 - 7.9V
 *            2630-2660 - 7.8V
 *            2600-2620 - 7.7V
 *            2570-2590 - 7.6V
 *            2530-2560 - 7.5V
 *            2500-2520 - 7.4V
 *            2470-2490 - 7.3V
 *            2430-2460 - 7.2V
 *            2400-2420 - 7.1V
 *            2370-2390 - 7.0V
 *            2330-2350 - 6.9V
 *            2290-2310 - 6.8V
 *            2250-2280 - 6.7V
 *            2220-2240 - 6.6V
 *            2190-2210 - 6.5V
 *            2160-2180 - 6.4V
 *            2120-2140 - 6.3V
 *            2090-2110 - 6.2V
 *            2050-2070 - 6.1V
 *            2020-2040 - 6.0V
 *            1990-2010 - 5.9V
 *
 *  VOLTAGE_FACTOR = 120
 *
 *  ADC  * 3 - VOLTAGE_FACTOR = 5...9V
 *  2190 * 3 - VOLTAGE_FACTOR = 6450
 *  2210 * 3 - VOLTAGE_FACTOR = 6510
 */



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

    uint16_t voltage_raw, voltage_9v;
    uint8_t voltage, level;

    uint16_t len;
    epInfo_t dstEpInfo;
    zoneStatusChangeNoti_t statusChangeNotification;

    voltage_divisor_en(ON);
    drv_adc_enable(ON);

    voltage_raw = drv_get_adc_data();

    drv_adc_enable(OFF);
    voltage_divisor_en(OFF);

    if ((voltage_raw * 3) < voltage_factor) {
        voltage_9v = 0;
    } else {
        voltage_9v = voltage_raw*3-voltage_factor;
    }

    if (voltage_9v % 100 >= 50) {
        voltage_9v /= 100;
        voltage_9v++;
        voltage = voltage_9v;
        voltage_9v *= 100;
    } else {
        voltage_9v /= 100;
        voltage = voltage_9v;
        voltage_9v *= 100;
    }

    level = get_battery_level(voltage_9v);

#if UART_PRINTF_MODE && DEBUG_BATTERY
    printf("Voltage_raw: %d\r\n", voltage_raw);
    printf("Voltage_9v:  %d\r\n", voltage_9v);
    printf("Voltage:     %d\r\n", voltage);
    printf("Level:       %d\r\n", level);
#endif

    zcl_setAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_GEN_POWER_CFG, ZCL_ATTRID_BATTERY_VOLTAGE, &voltage);
    zcl_setAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_GEN_POWER_CFG, ZCL_ATTRID_BATTERY_PERCENTAGE_REMAINING, &level);

    fillIASAddress(&dstEpInfo);

    zcl_getAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, &len, (uint8_t*)&statusChangeNotification.zoneStatus);
    zcl_getAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_ID, &len, &statusChangeNotification.zoneId);

    if (voltage_9v <= MIN_VBAT_MV) {

        if (!(statusChangeNotification.zoneStatus & ZONE_STATUS_BIT_BATTERY)) {

            statusChangeNotification.zoneStatus |= ZONE_STATUS_BIT_BATTERY;
            zcl_setAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, (uint8_t*)&statusChangeNotification.zoneStatus);
            statusChangeNotification.extStatus = 0;
            statusChangeNotification.delay = 0;

            zcl_iasZone_statusChangeNotificationCmd(APP_ENDPOINT1, &dstEpInfo, TRUE, &statusChangeNotification);
        }

    } else {

       if (statusChangeNotification.zoneStatus & ZONE_STATUS_BIT_BATTERY) {

           statusChangeNotification.zoneStatus &= ~ZONE_STATUS_BIT_BATTERY;
           zcl_setAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, (uint8_t*)&statusChangeNotification.zoneStatus);
           statusChangeNotification.extStatus = 0;
           statusChangeNotification.delay = 0;

           zcl_iasZone_statusChangeNotificationCmd(APP_ENDPOINT1, &dstEpInfo, TRUE, &statusChangeNotification);
       }
    }

    return 0;
}

void battery_init() {

    uint8_t adc_calibration[4] = {0};

    flash_read(CFG_ADC_CALIBRATION, 4, adc_calibration);

    if (adc_calibration[0] == 0x19 && adc_calibration[1] == 0x65) {
        printf("voltage_factor from flash\r\n");
        voltage_factor = adc_calibration[2];
        /* voltage_factor should not be 0 */
        if (voltage_factor == 0) voltage_factor++;
    } else {
        printf("voltage_factor from #define\r\n");
        voltage_factor = VOLTAGE_FACTOR;
    }

    voltage_divisor_en(OFF);
    drv_adc_init();
    drv_adc_mode_pin_set(DRV_ADC_BASE_MODE, VOLTAGE_DETECT_ADC_PIN);
}
