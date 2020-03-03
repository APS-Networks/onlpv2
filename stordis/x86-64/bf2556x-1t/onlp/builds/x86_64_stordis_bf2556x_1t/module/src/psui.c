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
 *
 *
 ***********************************************************/
#include <onlp/platformi/base.h>
#include "vendor_driver_pool.h"
#include "vendor_i2c_device_list.h"


enum onlp_psu_id
{
    PSU_RESERVED = 0,
    PSU_1,
    PSU_2
};

#define PSU_INFO_ENTRY_INIT(_id, _desc)    \
    {                                      \
        {                                  \
            .id = ONLP_PSU_ID_CREATE(_id), \
            .description = _desc,          \
            .poid = ONLP_OID_CHASSIS,      \
            .status = 0,                   \
        },                                 \
        .caps = (ONLP_PSU_CAPS_GET_TYPE | ONLP_PSU_CAPS_GET_VOUT | ONLP_PSU_CAPS_GET_POUT), \
        .type = ONLP_PSU_TYPE_DC12, \
    }


static onlp_psu_info_t onlp_psu_info[] = {
    { }, /* Not used */
    PSU_INFO_ENTRY_INIT(PSU_1, "PSU 1"),
    PSU_INFO_ENTRY_INIT(PSU_2, "PSU 2"),
};


int onlp_psui_hdr_get(onlp_oid_t oid, onlp_oid_hdr_t* hdr)
{
    int rv = 0;
    int id = ONLP_OID_ID_GET(oid) - 1;
    int present = 0;

    *hdr = onlp_psu_info[ONLP_OID_ID_GET(oid)].hdr;

    rv = vendor_get_present_status(&psu_present_list[id], &present);
    if(rv < 0) return ONLP_STATUS_E_INVALID;

    if(present)
    {
        ONLP_OID_STATUS_FLAGS_CLR(hdr);
        ONLP_OID_STATUS_FLAG_SET(hdr, PRESENT);
    }
    else
    {
        ONLP_OID_STATUS_FLAGS_CLR(hdr);
    }

    return ONLP_STATUS_OK;
}

int onlp_psui_info_get(onlp_oid_t oid, onlp_psu_info_t* info)
{
    int rv = 0, fail = 0;
    int id = ONLP_OID_ID_GET(oid) - 1;
    onlp_oid_hdr_t *hdr;
    void *busDrv = (void *)vendor_find_driver_by_name(psu_dev_list[id].bus_drv_name);
    psu_dev_driver_t *psu = 
        (psu_dev_driver_t *)vendor_find_driver_by_name(psu_dev_list[id].dev_drv_name);
    int present = 0;
    int mvout = 0, mpout = 0;

    ONLP_OID_INFO_ASSIGN(ONLP_OID_ID_GET(oid), onlp_psu_info, info);
    hdr = &info->hdr;

    rv = vendor_get_present_status(&psu_present_list[id], &present);
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

    vendor_dev_do_oc(psu_o_list[id]);
    if(psu->model_get(
        busDrv, 
        psu_dev_list[id].bus,
        psu_dev_list[id].addr,
        (char *) &info->model) != ONLP_STATUS_OK)
    {
        AIM_LOG_ERROR("psu->model_get failed.");
        fail = 1;
    }

    if(psu->serial_get(
        busDrv, 
        psu_dev_list[id].bus,
        psu_dev_list[id].addr,
        (char *) &info->serial) != ONLP_STATUS_OK)
    {
        AIM_LOG_ERROR("psu->serial_get failed.");
        fail = 1;
    }

    if(psu->volt_get(
        busDrv, 
        psu_dev_list[id].bus,
        psu_dev_list[id].addr,
        &mvout) != ONLP_STATUS_OK)
    {
        AIM_LOG_ERROR("psu->volt_get failed.");
        fail = 1;
    }

    info->mvout = mvout;

    if(psu->watt_get(
        busDrv, 
        psu_dev_list[id].bus,
        psu_dev_list[id].addr,
        &mpout) != ONLP_STATUS_OK)
    {
        AIM_LOG_ERROR("psu->watt_get failed.");
        fail = 1;
    }

    info->mpout = mpout;
    vendor_dev_do_oc(psu_o_list[id]);

    if(fail == 1) return ONLP_STATUS_E_INVALID;

    return ONLP_STATUS_OK;
}
