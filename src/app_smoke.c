#include "tl_common.h"
#include "zcl_include.h"

#include "app_main.h"

#define DEBOUNCE_SMOKE    32                          /* number of polls for debounce       */

static uint8_t smoke_debounce = 1;

static void cmdOnOff(uint8_t endpoint, uint8_t smoke) {
    epInfo_t dstEpInfo;
    TL_SETSTRUCTCONTENT(dstEpInfo, 0);

    dstEpInfo.profileId = HA_PROFILE_ID;
#if FIND_AND_BIND_SUPPORT
        dstEpInfo.dstAddrMode = APS_DSTADDR_EP_NOTPRESETNT;
#else
    dstEpInfo.dstAddrMode = APS_SHORT_DSTADDR_WITHEP;
    dstEpInfo.dstEp = endpoint;
    dstEpInfo.dstAddr.shortAddr = 0xfffc;
#endif

    if (smoke) {
        zcl_onOff_onCmd(endpoint, &dstEpInfo, FALSE);
    } else {
        zcl_onOff_offCmd(endpoint, &dstEpInfo, FALSE);
    }
}


static void fillIASAddress(epInfo_t* pdstEpInfo) {
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

    if (!drv_gpio_read(SMOKE_GPIO)) {
        if (smoke_debounce != DEBOUNCE_SMOKE) {
            smoke_debounce++;
            if (smoke_debounce == DEBOUNCE_SMOKE) {
#if UART_PRINTF_MODE && DEBUG_SMOKE
                printf("Smoke detected\r\n");
#endif /* UART_PRINTF_MODE */
                if(zb_isDeviceJoinedNwk()) {
                    cmdOnOff(APP_ENDPOINT1, true);
                    fillIASAddress(&dstEpInfo);

                    zoneStatusChangeNoti_t statusChangeNotification;

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

    if (smoke_idle()) {
        sleep_ms(1);
    }
}

uint8_t smoke_idle() {
    if (smoke_debounce != 1 && smoke_debounce != DEBOUNCE_SMOKE) {
        return true;
    }
    return false;
}

