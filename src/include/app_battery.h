#ifndef SRC_INCLUDE_APP_BATTERY_H_
#define SRC_INCLUDE_APP_BATTERY_H_

#define MAX_VBAT_MV             9000            /* 9000 mV - > battery = 100%         */
#define MIN_VBAT_MV             7000            /* 7000 mV - > battery = 0%           */
#define BATTERY_TIMER_INTERVAL  TIMEOUT_1HOUR

#define ADC_CALIBRATION_ID      0x1965

typedef struct __attribute__((packed)) _adc_calibration_t {
    uint16_t    id;                             /* 0x1965                             */
    uint8_t     voltage_factor;                 /* 1-255; default - 0x78 (120)        */
    uint8_t     reserve;
} adc_calibration_t;

int32_t batteryCb(void *arg);
void battery_init(bool isRetention);

#endif /* SRC_INCLUDE_APP_BATTERY_H_ */
