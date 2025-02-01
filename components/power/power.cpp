#include <stdio.h>

#include "power.h"
#include "defines.h"
#include "M5Unified.h"

extern "C" uint32_t power_get_batt_volt(void)
{
    return M5.Power.getBatteryVoltage();
}

extern "C" uint32_t power_get_batt_curr(void)
{
    return M5.Power.getBatteryCurrent();
}

extern "C" uint32_t power_get_batt_soc(void)
{
    return M5.Power.getBatteryLevel();
}
