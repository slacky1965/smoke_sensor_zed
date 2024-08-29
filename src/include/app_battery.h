#ifndef SRC_INCLUDE_APP_BATTERY_H_
#define SRC_INCLUDE_APP_BATTERY_H_

#define MAX_VBAT_MV             9000            /* 9000 mV - > battery = 100%         */
#define MIN_VBAT_MV             7000            /* 7000 mV - > battery = 0%           */
#define BATTERY_TIMER_INTERVAL  TIMEOUT_1HOUR

#define ADC_CALIBRATION         0x19657800
#define ADC_CALIB_FLAG1         0x19
#define ADC_CALIB_FLAG2         0x65

int32_t batteryCb(void *arg);
void battery_init(bool isRetention);

#endif /* SRC_INCLUDE_APP_BATTERY_H_ */
