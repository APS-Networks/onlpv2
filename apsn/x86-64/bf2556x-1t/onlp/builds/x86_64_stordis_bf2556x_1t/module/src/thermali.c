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
 * Thermal Sensor Platform Implementation.
 *
 ***********************************************************/
#include <onlp/platformi/base.h>
#include "vendor_driver_pool.h"
#include "vendor_i2c_device_list.h"


enum onlp_thermal_id
{
    THERMAL_RESERVED = 0,
    THERMAL_1_ON_MAIN_BOARD,
    THERMAL_2_ON_MAIN_BOARD,
    THERMAL_3_ON_MAIN_BOARD,
    THERMAL_4_ON_MAIN_BOARD
};

#define THERMAL_INFO_ENTRY_INIT(_id, _desc)          \
    {                                                \
        {                                            \
            .id = ONLP_THERMAL_ID_CREATE(_id),       \
            .description = _desc,                    \
            .poid = ONLP_OID_CHASSIS,                \
            .status = ONLP_OID_STATUS_FLAG_PRESENT,  \
        },                                           \
        .caps = (ONLP_THERMAL_CAPS_GET_TEMPERATURE), \
    }


static onlp_thermal_info_t onlp_thermal_info[] = {
    { }, /* Not used */
    THERMAL_INFO_ENTRY_INIT(THERMAL_1_ON_MAIN_BOARD, "Temp_L0"),
    THERMAL_INFO_ENTRY_INIT(THERMAL_2_ON_MAIN_BOARD, "Temp_L1"),
    THERMAL_INFO_ENTRY_INIT(THERMAL_3_ON_MAIN_BOARD, "Temp_L2"),
    THERMAL_INFO_ENTRY_INIT(THERMAL_4_ON_MAIN_BOARD, "Temp_L3")
};


int onlp_thermali_hdr_get(onlp_oid_t oid, onlp_oid_hdr_t* hdr)
{
    *hdr = onlp_thermal_info[ONLP_OID_ID_GET(oid)].hdr;
    return ONLP_STATUS_OK;
}

int onlp_thermali_info_get(onlp_oid_t oid, onlp_thermal_info_t* info)
{
    int fail = 0;
    int id = ONLP_OID_ID_GET(oid) - 1;
    void *busDrv = (void *)vendor_find_driver_by_name(thermal_dev_list[id].bus_drv_name);
    thermal_dev_driver_t *thermal = 
        (thermal_dev_driver_t *)vendor_find_driver_by_name(thermal_dev_list[id].dev_drv_name);

    ONLP_OID_INFO_ASSIGN(ONLP_OID_ID_GET(oid), onlp_thermal_info, info);

    vendor_dev_do_oc(thermal_o_list[id]);
    if(thermal->temp_get(
        busDrv, 
        thermal_dev_list[id].bus,
        thermal_dev_list[id].addr,
        thermal_dev_list[id].id, 
        &info->mcelsius) != ONLP_STATUS_OK)
    {
        info->mcelsius = 0;
        fail = 1;
    }
    vendor_dev_do_oc(thermal_c_list[id]);

    if(fail == 1) return ONLP_STATUS_E_INVALID;

    return ONLP_STATUS_OK;
}
