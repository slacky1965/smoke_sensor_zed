#ifndef SRC_INCLUDE_APP_BATTERY_H_
#define SRC_INCLUDE_APP_BATTERY_H_

#define MAX_VBAT_MV             9000            /* 9000 mV - > battery = 100%         */
#define MIN_VBAT_MV             7000            /* 7000 mV - > battery = 0%           */
#define BATTERY_TIMER_INTERVAL  TIMEOUT_1HOUR

int32_t batteryCb(void *arg);
void battery_init();

#endif /* SRC_INCLUDE_APP_BATTERY_H_ */
