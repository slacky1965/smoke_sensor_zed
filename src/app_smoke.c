#include "tl_common.h"
#include "zcl_include.h"

#include "app_main.h"

#define DEBOUNCE_SMOKE    32                          /* number of polls for debounce       */
#define DEBOUNCE_TAMPER   DEBOUNCE_SMOKE

static uint8_t smoke_debounce = 1;
static uint8_t tamper_debounce = 1;

static void cmdOnOff(uint8_t endpoint, uint8_t smoke_state) {

    u16 len;
    u8 switch_action;

    epInfo_t dstEpInfo;
    TL_SETSTRUCTCONTENT(dstEpInfo, 0);

    dstEpInfo.profileId = HA_PROFILE_ID;

//#if FIND_AND_BIND_SUPPORT
//        dstEpInfo.dstAddrMode = APS_DSTADDR_EP_NOTPRESETNT;
//#else
//    dstEpInfo.dstAddrMode = APS_SHORT_DSTADDR_WITHEP;
//    dstEpInfo.dstEp = endpoint;
//    dstEpInfo.dstAddr.shortAddr = 0xfffc;
//#endif

    dstEpInfo.dstAddrMode = APS_DSTADDR_EP_NOTPRESETNT;


    zcl_getAttrVal(endpoint, ZCL_CLUSTER_GEN_ON_OFF_SWITCH_CONFIG, ZCL_ATTRID_SWITCH_ACTION, &len, (u8*)&switch_action);
    //printf("sw config action:%d, state:%d \r\n", switch_action, smoke_state);
    switch (switch_action) {
        case ZCL_SWITCH_ACTION_ON_OFF:
            if (smoke_state)
                zcl_onOff_onCmd(endpoint, &dstEpInfo, FALSE);
            else
                zcl_onOff_offCmd(endpoint, &dstEpInfo, FALSE);
            break;
        case ZCL_SWITCH_ACTION_OFF_ON:
            if (smoke_state)
                zcl_onOff_offCmd(endpoint, &dstEpInfo, FALSE);
            else
                zcl_onOff_onCmd(endpoint, &dstEpInfo, FALSE);
            break;
        case ZCL_SWITCH_ACTION_TOGGLE:
            zcl_onOff_toggleCmd(endpoint, &dstEpInfo, FALSE);
    };

//    if (smoke_state) {
//        zcl_onOff_onCmd(endpoint, &dstEpInfo, FALSE);
//    } else {
//        zcl_onOff_offCmd(endpoint, &dstEpInfo, FALSE);
//    }
}


void fillIASAddress(epInfo_t* pdstEpInfo) {
    u16 len;
    u8 zoneState;

    memset((u8 *)pdstEpInfo, 0, sizeof(epInfo_t));

    zcl_getAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATE, &len, &zoneState);

    pdstEpInfo->dstEp = APP_ENDPOINT1;
    pdstEpInfo->profileId = HA_PROFILE_ID;

    if (zoneState&ZONE_STATE_ENROLLED) { //device enrolled
        pdstEpInfo->dstAddrMode = APS_LONG_DSTADDR_WITHEP;
        zcl_getAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_IAS_CIE_ADDR, &len, (u8*)&pdstEpInfo->dstAddr.extAddr);
    }
    else {
        pdstEpInfo->dstAddrMode = APS_SHORT_DSTADDR_WITHEP;
        pdstEpInfo->dstAddr.shortAddr = 0x0000;
    }
}


void smoke_handler() {

    uint16_t len;
    epInfo_t dstEpInfo;
    zoneStatusChangeNoti_t statusChangeNotification;

#if (BOARD == BOARD_8258_DIY)
    if (drv_gpio_read(ALARM_GPIO)) {
#else
    if (!drv_gpio_read(ALARM_GPIO)) {
#endif
        if (smoke_debounce != DEBOUNCE_SMOKE) {
            smoke_debounce++;
            if (smoke_debounce == DEBOUNCE_SMOKE) {
#if UART_PRINTF_MODE && DEBUG_SMOKE
                printf("Smoke detected\r\n");
#endif /* UART_PRINTF_MODE */
                if(zb_isDeviceJoinedNwk()) {
                    cmdOnOff(APP_ENDPOINT1, true);
                    fillIASAddress(&dstEpInfo);

                    zcl_getAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, &len, (u8*)&statusChangeNotification.zoneStatus);
                    zcl_getAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_ID, &len, &statusChangeNotification.zoneId);

                    statusChangeNotification.zoneStatus |= ZONE_STATUS_BIT_ALARM1;
                    zcl_setAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, (u8*)&statusChangeNotification.zoneStatus);
                    statusChangeNotification.extStatus = 0;
                    statusChangeNotification.delay = 0;

                    zcl_iasZone_statusChangeNotificationCmd(APP_ENDPOINT1, &dstEpInfo, TRUE, &statusChangeNotification);
                }
            }
        }
    } else {
        if (smoke_debounce != 1) {
            smoke_debounce--;
            if (smoke_debounce == 1) {
#if UART_PRINTF_MODE && DEBUG_SMOKE
                printf("Smoke clear\r\n");
#endif /* UART_PRINTF_MODE */
                if(zb_isDeviceJoinedNwk()){
                    cmdOnOff(APP_ENDPOINT1, false);
                    fillIASAddress(&dstEpInfo);

                    zcl_getAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, &len, (u8*)&statusChangeNotification.zoneStatus);
                    zcl_getAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_ID, &len, &statusChangeNotification.zoneId);

                    statusChangeNotification.zoneStatus &= ~ZONE_STATUS_BIT_ALARM1;
                    zcl_setAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, (u8*)&statusChangeNotification.zoneStatus);
                    statusChangeNotification.extStatus = 0;
                    statusChangeNotification.delay = 0;

                    zcl_iasZone_statusChangeNotificationCmd(APP_ENDPOINT1, &dstEpInfo, TRUE, &statusChangeNotification);
                }
            }
        }
    }

#if (BOARD == BOARD_8258_DIY)
  #if (TAMPER_SWITCH_NC == 1)
    if (!drv_gpio_read(TAMPER_GPIO)) {
  #else
    if (drv_gpio_read(TAMPER_GPIO)) {
  #endif
#else
    if (!drv_gpio_read(TAMPER_GPIO)) {
#endif
        if (tamper_debounce != DEBOUNCE_SMOKE) {
            tamper_debounce++;
            if (tamper_debounce == DEBOUNCE_SMOKE) {
#if UART_PRINTF_MODE && DEBUG_SMOKE
                printf("Tamper detected\r\n");
#endif /* UART_PRINTF_MODE */
                if(zb_isDeviceJoinedNwk()) {
                    fillIASAddress(&dstEpInfo);

                    zcl_getAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, &len, (u8*)&statusChangeNotification.zoneStatus);
                    zcl_getAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_ID, &len, &statusChangeNotification.zoneId);

                    statusChangeNotification.zoneStatus |= ZONE_STATUS_BIT_TAMPER;
                    zcl_setAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, (u8*)&statusChangeNotification.zoneStatus);
                    statusChangeNotification.extStatus = 0;
                    statusChangeNotification.delay = 0;

                    zcl_iasZone_statusChangeNotificationCmd(APP_ENDPOINT1, &dstEpInfo, TRUE, &statusChangeNotification);
                }
            }
        }
    } else {
        if (tamper_debounce != 1) {
            tamper_debounce--;
            if (tamper_debounce == 1) {
#if UART_PRINTF_MODE && DEBUG_SMOKE
                printf("Tamper clear\r\n");
#endif /* UART_PRINTF_MODE */
                if(zb_isDeviceJoinedNwk()){
                    fillIASAddress(&dstEpInfo);

                    zcl_getAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, &len, (u8*)&statusChangeNotification.zoneStatus);
                    zcl_getAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_ID, &len, &statusChangeNotification.zoneId);

                    statusChangeNotification.zoneStatus &= ~ZONE_STATUS_BIT_TAMPER;
                    zcl_setAttrVal(APP_ENDPOINT1, ZCL_CLUSTER_SS_IAS_ZONE, ZCL_ATTRID_ZONE_STATUS, (u8*)&statusChangeNotification.zoneStatus);
                    statusChangeNotification.extStatus = 0;
                    statusChangeNotification.delay = 0;

                    zcl_iasZone_statusChangeNotificationCmd(APP_ENDPOINT1, &dstEpInfo, TRUE, &statusChangeNotification);
                }
            }
        }
    }

    if (smoke_idle()) {
        sleep_ms(1);
    }
}

uint8_t smoke_idle() {
    if ((smoke_debounce != 1 && smoke_debounce != DEBOUNCE_SMOKE) ||
        (tamper_debounce != 1 && tamper_debounce != DEBOUNCE_TAMPER)) {
        return true;
    }
    return false;
}

