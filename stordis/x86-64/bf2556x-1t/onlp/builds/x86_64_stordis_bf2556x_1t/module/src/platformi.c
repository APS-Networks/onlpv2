#include <onlp/platformi/base.h>
#include "vendor_driver_pool.h"
#include "vendor_i2c_device_list.h"

#include "x86_64_stordis_bf2556x_1t_int.h"
#include "x86_64_stordis_bf2556x_1t_log.h"


const char*
onlp_platformi_get(void)
{
    return "x86-64-stordis-bf2556x-1t-r0";
}

int
onlp_platformi_sw_init(void)
{
    vendor_driver_init();

    return 0;
}

int
onlp_platformi_manage_fans(void)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

int
onlp_platformi_manage_leds(void)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}
