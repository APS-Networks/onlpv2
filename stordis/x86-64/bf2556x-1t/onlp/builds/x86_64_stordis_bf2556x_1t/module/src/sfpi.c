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
 ***********************************************************/
#include <onlp/platformi/base.h>
#include "vendor_driver_pool.h"
#include "vendor_i2c_device_list.h"


int onlp_sfpi_bitmap_get(onlp_sfp_bitmap_t* bmap)
{
    int i;

    AIM_BITMAP_CLR_ALL(bmap);

    for (i = sfp_start_port; i < (sfp_list_size + sfp_start_port); i++)
    {
        AIM_BITMAP_SET(bmap, i);
    }

    return ONLP_STATUS_OK;
}

/*
 * Return 1 if present.
 * Return 0 if not present.
 * Return < 0 if error.
 */
int onlp_sfpi_is_present(onlp_oid_id_t oid)
{
    int rv = 0;
    int id = ONLP_OID_ID_GET(oid) - sfp_start_port;
    int present = 0;

    rv = vendor_get_present_status(&sfp_present_list[id], &present);
    if(rv < 0) return ONLP_STATUS_E_INTERNAL;

    return present;
}

int onlp_sfpi_presence_bitmap_get(onlp_sfp_bitmap_t* dst)
{
    int rv = 0;
    int i;
    int present = 0;

    AIM_BITMAP_CLR_ALL(dst);

    for (i = 0; i < sfp_list_size; i++)
    {
        rv = vendor_get_present_status(&sfp_present_list[i], &present);
        if(rv < 0) return ONLP_STATUS_E_INTERNAL;

        if (present == 1)
            AIM_BITMAP_SET(dst, (i+sfp_start_port+1));
    }

    return ONLP_STATUS_OK;
}

int
onlp_sfpi_dev_read(onlp_oid_id_t oid, int devaddr, int addr,
                   uint8_t* dst, int size)
{
    int rv = 0, fail = 0;
    int id = ONLP_OID_ID_GET(oid) - sfp_start_port;
    int present = 0;
    uint8_t data[256] = {0};
    int copy_size = sizeof(data);
    void *busDrv = (void *)vendor_find_driver_by_name(sfp_dev_list[id].bus_drv_name);
    sfp_dev_driver_t *sfp = 
        (sfp_dev_driver_t *)vendor_find_driver_by_name(sfp_dev_list[id].dev_drv_name);

    rv = vendor_get_present_status(&sfp_present_list[id], &present);
    if(rv < 0) return ONLP_STATUS_E_INTERNAL;
    if (present != 1)
    {
        return ONLP_STATUS_E_MISSING;
    }

    if (devaddr == 0x51)
    {
        if (id >= sfp_num_of_sfp)
        {
            return ONLP_STATUS_E_UNSUPPORTED;
        }
        else
        {
            vendor_dev_do_oc(sfp_o_list[id]);
            if(sfp->eeprom_load(
                busDrv, 
                sfp_dev_list[id].bus,
                0x51, 
                data) != ONLP_STATUS_OK)
            {
                AIM_LOG_ERROR("sfp->eeprom_load failed.");
                fail = 1;
            }
            vendor_dev_do_oc(sfp_c_list[id]);

            if(fail == 1) return ONLP_STATUS_E_INTERNAL;
        }
    }
    else
    {
        vendor_dev_do_oc(sfp_o_list[id]);
        if(sfp->eeprom_load(
            busDrv, 
            sfp_dev_list[id].bus,
            sfp_dev_list[id].addr, 
            data) != ONLP_STATUS_OK)
        {
            AIM_LOG_ERROR("sfp->eeprom_load failed.");
            fail = 1;
        }
        vendor_dev_do_oc(sfp_c_list[id]);

        if(fail == 1) return ONLP_STATUS_E_INTERNAL;
    }

    copy_size -= addr;
    if (copy_size > 0)
    {
        if (size > copy_size)
            memcpy(dst, &data[addr], copy_size);
        else
            memcpy(dst, &data[addr], size);
    }

    return ONLP_STATUS_OK;
}

int
onlp_sfpi_dev_readb(onlp_oid_id_t oid, int devaddr, int addr)
{
    int rv = 0, fail = 0;
    int id = ONLP_OID_ID_GET(oid) - sfp_start_port;
    int present = 0;
    uint8_t data = 0;
    void *busDrv = (void *)vendor_find_driver_by_name(sfp_dev_list[id].bus_drv_name);
    sfp_dev_driver_t *sfp = 
        (sfp_dev_driver_t *)vendor_find_driver_by_name(sfp_dev_list[id].dev_drv_name);

    rv = vendor_get_present_status(&sfp_present_list[id], &present);
    if(rv < 0) return ONLP_STATUS_E_INTERNAL;
    if (present != 1)
    {
        return ONLP_STATUS_E_MISSING;
    }

    vendor_dev_do_oc(sfp_o_list[id]);
    if(sfp->eeprom_readb(
        busDrv, 
        sfp_dev_list[id].bus,
        sfp_dev_list[id].addr, 
        addr,
        &data) != ONLP_STATUS_OK)
    {
        AIM_LOG_ERROR("sfp->eeprom_readb failed.");
        fail = 1;
    }
    vendor_dev_do_oc(sfp_c_list[id]);

    if(fail == 1) return ONLP_STATUS_E_INTERNAL;

    return (int)data;
}

int
onlp_sfpi_dev_writeb(onlp_oid_id_t oid, int devaddr, int addr, uint8_t value)
{
    int rv = 0, fail = 0;
    int id = ONLP_OID_ID_GET(oid) - sfp_start_port;
    int present = 0;
    void *busDrv = (void *)vendor_find_driver_by_name(sfp_dev_list[id].bus_drv_name);
    sfp_dev_driver_t *sfp = 
        (sfp_dev_driver_t *)vendor_find_driver_by_name(sfp_dev_list[id].dev_drv_name);

    rv = vendor_get_present_status(&sfp_present_list[id], &present);
    if(rv < 0) return ONLP_STATUS_E_INTERNAL;
    if (present != 1)
    {
        return ONLP_STATUS_E_MISSING;
    }

    vendor_dev_do_oc(sfp_o_list[id]);
    if(sfp->eeprom_writeb(
        busDrv, 
        sfp_dev_list[id].bus,
        sfp_dev_list[id].addr, 
        addr,
        value) != ONLP_STATUS_OK)
    {
        AIM_LOG_ERROR("sfp->eeprom_readb failed.");
        fail = 1;
    }
    vendor_dev_do_oc(sfp_c_list[id]);

    if(fail == 1) return ONLP_STATUS_E_INTERNAL;

    return ONLP_STATUS_OK;
}

int onlp_sfpi_control_supported(onlp_oid_id_t oid, onlp_sfp_control_t control, int* rv)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

int onlp_sfpi_control_get(onlp_oid_id_t oid, onlp_sfp_control_t control, int* value)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

int onlp_sfpi_control_set(onlp_oid_id_t oid, onlp_sfp_control_t control, int value)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

int onlp_sfpi_rx_los_bitmap_get(onlp_sfp_bitmap_t* dst)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

int onlp_sfpi_ioctl(int port, va_list vargs)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

