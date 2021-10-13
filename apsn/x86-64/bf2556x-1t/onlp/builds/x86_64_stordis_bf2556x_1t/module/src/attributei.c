#include <onlp/platformi/base.h>
#include "vendor_driver_pool.h"
#include "vendor_i2c_device_list.h"


int
onlp_attributei_onie_info_get(onlp_oid_t oid, onlp_onie_info_t* rp)
{
    int rv = -1;
    int id = ONLP_OID_ID_GET(oid) - 1;
    uint8_t data[256] = {0};
    void *busDrv = (void *)vendor_find_driver_by_name(eeprom_dev_list[id].bus_drv_name);
    eeprom_dev_driver_t *eeprom =
        (eeprom_dev_driver_t *)vendor_find_driver_by_name(eeprom_dev_list[id].dev_drv_name);

    if (!ONLP_OID_IS_TYPE(ONLP_OID_TYPE_CHASSIS, oid))
    {
        return ONLP_STATUS_E_UNSUPPORTED;
    }

    /* 
     * Return OK when rp is a NULL pointer.
     * Do not make sense, but just try to adapt onlp_attribute_onie_info_get().
     */
    if (rp == NULL)
    {
        return ONLP_STATUS_OK;
    }

    vendor_dev_do_oc(eeprom_o_list[id]);
    if (eeprom_dev_list[id].id == 2)
    {
        rv = eeprom->load(
            NULL,
            eeprom_dev_list[id].bus,
            eeprom_dev_list[id].addr,
            data);
    }
    else
    {
        rv = eeprom->load(
            busDrv,
            eeprom_dev_list[id].bus,
            eeprom_dev_list[id].addr,
            data);
    }
    vendor_dev_do_oc(eeprom_c_list[id]);

    rv = onlp_onie_decode(rp, data, 256);
    if(rv < 0) return ONLP_STATUS_E_INVALID;

    return ONLP_STATUS_OK;
}

int
onlp_attributei_asset_info_get(onlp_oid_t oid, onlp_asset_info_t* rp)
{
    if (!ONLP_OID_IS_TYPE(ONLP_OID_TYPE_CHASSIS, oid))
    {
        return ONLP_STATUS_E_UNSUPPORTED;
    }

    /*
     * Return OK when rp is a NULL pointer.
     * Do not make sense, but just try to adapt onlp_attribute_asset_info_get().
     */
    if (rp == NULL)
    {
        return ONLP_STATUS_OK;
    }

    return ONLP_STATUS_E_UNSUPPORTED;
}
