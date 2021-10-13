/************************************************************
 * <bsn.cl fy=2014 v=onl>
 *
 *        Copyright 2014, 2015 Big Switch Networks, Inc.
 *        Copyright 2017 Delta Networks, Inc.
 *
 * Licensed under the Eclipse Public License, Version 1.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *        http://www.eclipse.org/legal/epl-v10.html
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the
 * License.
 *
 * </bsn.cl>
 ************************************************************
 *
 * Fan Platform Implementation Defaults.
 *
 ***********************************************************/
#include <onlp/platformi/base.h>
#include "vendor_driver_pool.h"
#include "vendor_i2c_device_list.h"


enum onlp_fan_id
{
    FAN_RESERVED = 0,
    FAN_1_ON_FAN_BOARD,
    FAN_2_ON_FAN_BOARD,
    FAN_3_ON_FAN_BOARD,
    FAN_4_ON_FAN_BOARD,
    FAN_5_ON_FAN_BOARD,
    FAN_6_ON_FAN_BOARD,
    FAN_7_ON_FAN_BOARD,
    FAN_8_ON_FAN_BOARD
};

#define FAN_INFO_ENTRY_INIT(_id, _desc)         \
    {                                           \
        {                                       \
            .id = ONLP_FAN_ID_CREATE(_id),      \
            .description = _desc,               \
            .poid = 0,                          \
            .status = 0,                        \
        },                                      \
        .caps = (ONLP_FAN_CAPS_GET_PERCENTAGE), \
    }


static onlp_fan_info_t onlp_fan_info[] = {
    { }, /* Not used */
    FAN_INFO_ENTRY_INIT(FAN_1_ON_FAN_BOARD, "FanPWM_0"),
    FAN_INFO_ENTRY_INIT(FAN_2_ON_FAN_BOARD, "FanPWM_0_1"),
    FAN_INFO_ENTRY_INIT(FAN_3_ON_FAN_BOARD, "FanPWM_1"),
    FAN_INFO_ENTRY_INIT(FAN_4_ON_FAN_BOARD, "FanPWM_1_1"),
    FAN_INFO_ENTRY_INIT(FAN_5_ON_FAN_BOARD, "FanPWM_2"),
    FAN_INFO_ENTRY_INIT(FAN_6_ON_FAN_BOARD, "FanPWM_2_1"),
    FAN_INFO_ENTRY_INIT(FAN_7_ON_FAN_BOARD, "FanPWM_3"),
    FAN_INFO_ENTRY_INIT(FAN_8_ON_FAN_BOARD, "FanPWM_3_1"),
};

static int fan_max_rpm_list[] = {
    23000,
    20500,
    23000,
    20500,
    23000,
    20500,
    23000,
    20500
};


int onlp_fani_hdr_get(onlp_oid_t oid, onlp_oid_hdr_t* hdr)
{
    int rv = 0;
    int id = ONLP_OID_ID_GET(oid) - 1;
    int present = 0;

    *hdr = onlp_fan_info[ONLP_OID_ID_GET(oid)].hdr;

    rv = vendor_get_present_status(&fan_present_list[id], &present);
    if(rv < 0) return ONLP_STATUS_E_INVALID;

    if(present)
    {
        ONLP_OID_STATUS_FLAGS_CLR(hdr);
        ONLP_OID_STATUS_FLAG_SET(hdr, PRESENT);
    }
    else
    {
        ONLP_OID_STATUS_FLAGS_CLR(hdr);
        return ONLP_STATUS_OK;
    }

    return ONLP_STATUS_OK;
}

int onlp_fani_info_get(onlp_oid_t oid, onlp_fan_info_t* info)
{
    int rv = 0, fail = 0;
    int id = ONLP_OID_ID_GET(oid) - 1;
    onlp_oid_hdr_t *hdr;
    void *busDrv = (void *)vendor_find_driver_by_name(fan_dev_list[id].bus_drv_name);
    fan_dev_driver_t *fan = 
        (fan_dev_driver_t *)vendor_find_driver_by_name(fan_dev_list[id].dev_drv_name);
    int present = 0;
    int rpm;

    ONLP_OID_INFO_ASSIGN(ONLP_OID_ID_GET(oid), onlp_fan_info, info);
    hdr = &info->hdr;

    rv = vendor_get_present_status(&fan_present_list[id], &present);
    if(rv < 0) return ONLP_STATUS_E_INVALID;

    if(present)
    {
        ONLP_OID_STATUS_FLAGS_CLR(hdr);
        ONLP_OID_STATUS_FLAG_SET(hdr, PRESENT);
    }
    else
    {
        ONLP_OID_STATUS_FLAGS_CLR(hdr);
        return ONLP_STATUS_OK;
    }

    vendor_dev_do_oc(fan_o_list[id]);
    if(fan->rpm_get(
        busDrv, 
        fan_dev_list[id].bus,
        fan_dev_list[id].addr,
        fan_dev_list[id].id, 
        &rpm) != ONLP_STATUS_OK)
    {
        info->percentage = 0;
        fail = 1;
    }
    vendor_dev_do_oc(fan_c_list[id]);

    if(fail == 1) return ONLP_STATUS_E_INVALID;

    info->percentage = (rpm * 100) / fan_max_rpm_list[id];
    return ONLP_STATUS_OK;
}

int onlp_fani_rpm_set(onlp_oid_t oid, int rpm)
{
    if (oid || rpm) {}

    return ONLP_STATUS_E_UNSUPPORTED;
}

