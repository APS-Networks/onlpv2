/************************************************************
 * <bsn.cl fy=2014 v=onl>
 *
 *           Copyright 2014 Big Switch Networks, Inc.
 *           Copyright 2014 Accton Technology Corporation.
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
 *
 *
 ***********************************************************/
#include <onlp/platformi/base.h>
#include "vendor_driver_pool.h"
#include "vendor_i2c_device_list.h"


enum onlp_led_id
{
    LED_RESERVED = 0,
    LED_SYS
};

#define LED_INFO_ENTRY_INIT(_id, _desc, _caps, _defaultm)      \
    {                                               \
        {                                           \
            .id = ONLP_LED_ID_CREATE(_id),          \
            .description = _desc,                   \
            .poid = ONLP_OID_CHASSIS,               \
            .status = ONLP_OID_STATUS_FLAG_PRESENT, \
        },                                          \
        .caps = _caps,                              \
        .mode = _defaultm,                          \
    }

static onlp_led_info_t onlp_led_info[] =
{
    { }, /* Not used */
    LED_INFO_ENTRY_INIT(LED_SYS, "sysled-system",
        (ONLP_LED_CAPS_OFF | ONLP_LED_CAPS_RED | ONLP_LED_CAPS_GREEN | ONLP_LED_CAPS_GREEN_BLINKING ), ONLP_LED_MODE_GREEN)
};

int
onlp_ledi_hdr_get(onlp_oid_t oid, onlp_oid_hdr_t* hdr)
{
    *hdr = onlp_led_info[ONLP_OID_ID_GET(oid)].hdr;
    return ONLP_STATUS_OK;
}

int
onlp_ledi_info_get(onlp_oid_t oid, onlp_led_info_t* info)
{
    //AIM_LOG_ERROR("Function: %s, instance: %d \n", __FUNCTION__, ONLP_OID_ID_GET(oid) - 1);
    int rv = 0, id = ONLP_OID_ID_GET(oid) - 1, cpld_idx = 0;
    vendor_dev_led_pin_t *curr_mode;
    uint8_t mode = 0;
    ONLP_OID_INFO_ASSIGN(ONLP_OID_ID_GET(oid), onlp_led_info, info);

    void *busDrv = (void *)vendor_find_driver_by_name(sysled_dev_list[id].bus_drv_name);
    cpld_dev_driver_t *cpld = 
        (cpld_dev_driver_t *)vendor_find_driver_by_name(sysled_dev_list[id].dev_drv_name);

    cpld_idx = vendor_find_cpld_idx(sysled_color_list[id]->addr);
    if(cpld_idx < 0) return ONLP_STATUS_E_INTERNAL;

    vendor_dev_do_oc(cpld_o_list[cpld_idx]);
    rv = cpld->readb(
        busDrv,
        sysled_color_list[id]->bus,
        sysled_color_list[id]->addr,
        sysled_color_list[id]->offset,
        &mode);
    vendor_dev_do_oc(cpld_c_list[cpld_idx]);

    if(rv < 0) return ONLP_STATUS_E_INTERNAL;
    curr_mode = sysled_color_list[id];

    while (curr_mode->mode != ONLP_LED_MODE_INVALID)
    {
        if(((mode & curr_mode->mask) == curr_mode->match) ? 1 : 0)
        {
            info->mode = curr_mode->mode;
            return ONLP_STATUS_OK;
        }
        curr_mode++;
    }
    
    AIM_LOG_ERROR("Function: %s, Unknown LED mode.", __FUNCTION__);
    return ONLP_STATUS_E_INTERNAL;
}

int
onlp_ledi_mode_set(onlp_oid_t oid, onlp_led_mode_t mode)
{
    //AIM_LOG_ERROR("Function: %s, instance: %d \n", __FUNCTION__, ONLP_OID_ID_GET(oid) - 1);
    int rv = 0, id = ONLP_OID_ID_GET(oid) - 1, cpld_idx = 0;
    uint8_t curr_data = 0;

    void *busDrv = (void *)vendor_find_driver_by_name(sysled_dev_list[id].bus_drv_name);
    cpld_dev_driver_t *cpld = 
        (cpld_dev_driver_t *)vendor_find_driver_by_name(sysled_dev_list[id].dev_drv_name);

    cpld_idx = vendor_find_cpld_idx(sysled_color_list[id]->addr);
    if(cpld_idx < 0) return ONLP_STATUS_E_INTERNAL;

    vendor_dev_do_oc(cpld_o_list[cpld_idx]);
    rv = cpld->readb(
        busDrv,
        sysled_color_list[id]->bus,
        sysled_color_list[id]->addr,
        sysled_color_list[id]->offset,
        &curr_data);

    curr_data &= ~sysled_color_list[id]->mask;
    curr_data |= (sysled_color_list[id]->match & sysled_color_list[id]->mask);

    rv = cpld->writeb(
        busDrv,
        sysled_color_list[id]->bus,
        sysled_color_list[id]->addr,
        sysled_color_list[id]->offset,
        curr_data);
    vendor_dev_do_oc(cpld_c_list[cpld_idx]);

    if(rv < 0) return ONLP_STATUS_E_INTERNAL;
    
    return ONLP_STATUS_OK;
}
