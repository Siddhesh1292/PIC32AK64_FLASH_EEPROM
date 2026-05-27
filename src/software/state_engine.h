#ifndef STATE_ENGINE_H
#define STATE_ENGINE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

//--------------------------------------------------
// STATE ENUM
//--------------------------------------------------
typedef enum
{
    STATE_INIT = 0,
    STATE_RUN

} STATE_ENGINE_STATES;

//--------------------------------------------------
// PUBLIC FUNCTIONS
//--------------------------------------------------
void STATE_ENGINE_Tasks(void);
void drive_init(int * p_adr, float * CONFIG_THROTTLE_ZERO_adr, float * CONFIG_THROTTLE_MAX_adr, float motor_TfaultC, float ESC_TfaultC, float * rpm_to_speed_adr, float *CONFIG_BRAKE_ZERO_adr, float *CONFIG_BRAKE_MAX_adr);
void state_init(void);

#ifdef __cplusplus
}
#endif
 
#endif // STATE_ENGINE_H