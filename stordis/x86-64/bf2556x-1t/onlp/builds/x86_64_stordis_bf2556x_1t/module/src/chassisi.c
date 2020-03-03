#include <onlp/platformi/chassisi.h>
#include "vendor_i2c_device_list.h"


int
onlp_chassisi_hdr_get(onlp_oid_id_t id, onlp_oid_hdr_t* hdr)
{
    int i;
    onlp_oid_t* e = hdr->coids;

    memset(hdr->coids, 0, sizeof(hdr->coids));

    ONLP_OID_STATUS_FLAG_SET(hdr, PRESENT);
    ONLP_OID_STATUS_FLAG_SET(hdr, OPERATIONAL);

    for (i = 1; i <= psu_list_size; i++)
    {
        *e++ = ONLP_PSU_ID_CREATE(i);
    }

    for (i = 1; i <= fan_list_size; i++)
    {
        *e++ = ONLP_FAN_ID_CREATE(i);
    }
    
    for (i = 1; i <= thermal_list_size; i++)
    {
        *e++ = ONLP_THERMAL_ID_CREATE(i);
    }

    for(i = (1 + sfp_start_port); i <= (sfp_list_size + sfp_start_port); i++) 
    {
        *e++ = ONLP_SFP_ID_CREATE(i);
    }

    return 0;
}

int
onlp_chassisi_info_get(onlp_oid_id_t id, onlp_chassis_info_t* info)
{
    return onlp_chassisi_hdr_get(id, &info->hdr);
}
