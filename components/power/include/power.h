#ifndef POWER_H
#define POWER_H

#ifdef __cplusplus
extern "C"
{
#endif

    uint32_t power_get_batt_volt(void);

    uint32_t power_get_batt_curr(void);

    uint32_t power_get_batt_soc(void);
    
    void power_shut_down(void);

#ifdef __cplusplus
}
#endif

#endif // POWER_H