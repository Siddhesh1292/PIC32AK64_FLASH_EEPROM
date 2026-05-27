#ifndef _TUNING_H    /* Guard against multiple inclusion */
#define _TUNING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes

//--------------------------------------------------
// MATRIX SIZE
//--------------------------------------------------
extern float CONFIG_POLES;  
extern float CONFIG_POLES;
extern float CONFIG_RPH;
extern float CONFIG_LD;
extern float CONFIG_LQ;
extern float CONFIG_PHIPH;
extern float CONFIG_SENSOR_DIRECTION;    
extern float CONFIG_ROTATION_DIRECTION;
extern float CONFIG_ANG_MARGINE;  
extern float CONFIG_ZERO_ANGLE;  
extern float CONFIG_OVFLT;
extern float CONFIG_OVFLT_MAX;
extern float CONFIG_UVFLT;
extern float CONFIG_IPHFLT;
extern float CONFIG_IPH_FLT_MAX;
extern float CONFIG_IBATFLT;
extern float CONFIG_IBAT_FLT_MAX;
extern float CONFIG_ESC_TFLTC;
extern float CONFIG_ESC_TFLTC_MAX;
extern float CONFIG_MOTOR_TFLTC;
extern float CONFIG_RPM_FLT;
extern float CONFIG_RPM_TO_KMPH;
extern float CONFIG_RPM_MAX;
extern float CONFIG_IPH_MAX;
extern float CONFIG_IPH_MAX_HLIMIT;
extern float CONFIG_MOTOR_DERATEC;
extern float CONFIG_ESC_DERATEC;
extern float CONFIG_VBAT_MAX;
extern float CONFIG_IBAT_DEFAULT;
extern float CONFIG_IBAT_MAX;
extern float CONFIG_IBAT_REGEN;
extern float CONFIG_DRIVING_MODE;
extern float CONFIG_DRIVING_INTERFACE;
extern float CONFIG_L_RPM;
extern float CONFIG_M_RPM;    
extern float CONFIG_THROTTLE_ZERO;
extern float CONFIG_THROTTLE_ZERO_LL;  
extern float CONFIG_THROTTLE_ZERO_HL;
extern float CONFIG_THROTTLE_MAX;
extern float CONFIG_THROTTLE_MAX_LL;
extern float CONFIG_THROTTLE_MAX_HL;
extern float CONFIG_VBRAKE_DERATE;
extern float CONFIG_K_AUTO_BRAKE;
extern float CONFIG_REVERSE_RPM;
extern float CONFIG_KP_RPM;
extern float CONFIG_KP_RPM_MAX;
extern float CONFIG_KP_RPM_MIN; 
extern float CONFIG_KI_RPM;
extern float CONFIG_KI_RPM_MAX;
extern float CONFIG_KI_RPM_MIN;
extern float CONFIG_L_ACCELERATION;
extern float CONFIG_M_ACCELERATION;
extern float CONFIG_L_IBAT;
extern float CONFIG_M_IBAT;
extern float CONFIG_L_IPH;
extern float CONFIG_M_IPH;
extern float CONFIG_BRAKE_ZERO;
extern float CONFIG_BRAKE_ZERO_LL;   
extern float CONFIG_BRAKE_ZERO_HL;
extern float CONFIG_BRAKE_MAX;
extern float CONFIG_BRAKE_MAX_LL;
extern float CONFIG_BRAKE_MAX_HL;
extern float CONFIG_SAFE_RPM;
extern float CONFIG_BRAKE_RPM;             
extern float CONFIG_REGEN_BRAKE;          
extern float CONFIG_DI;
extern float CONFIG_CANBAUD;

//--------------------------------------------------
// MATRIX SIZE
//--------------------------------------------------
#define ROWS    11
#define COLS    4

//--------------------------------------------------
// EXTERNAL MATRIX
//--------------------------------------------------
extern float readMatrix[ROWS][COLS];
extern float writeMatrix[ROWS][COLS];

//==================================================
// ROW 1
//==================================================
#define R_FF01_1                    readMatrix[0][0]
#define R_FF01_2                    readMatrix[0][1]
#define R_FF01_3                    readMatrix[0][2]
#define R_FF01_4                    readMatrix[0][3]

#define W_FF01_1                    writeMatrix[0][0]
#define W_FF01_2                    writeMatrix[0][1]
#define W_FF01_3                    writeMatrix[0][2]
#define W_FF01_4                    writeMatrix[0][3]

//==================================================
// ROW 2
//==================================================
#define R_FF02_1                    readMatrix[1][0]
#define R_FF02_2                    readMatrix[1][1]
#define R_FF02_3                    readMatrix[1][2]
#define R_FF02_4                    readMatrix[1][3]

#define W_FF02_1                    writeMatrix[1][0]
#define W_FF02_2                    writeMatrix[1][1]
#define W_FF02_3                    writeMatrix[1][2]
#define W_FF02_4                    writeMatrix[1][3]

//==================================================
// ROW 3
//==================================================
#define R_FF04_1                    readMatrix[2][0]
#define R_FF04_2                    readMatrix[2][1]
#define R_FF04_3                    readMatrix[2][2]
#define R_FF04_4                    readMatrix[2][3]

#define W_FF04_1                    writeMatrix[2][0]
#define W_FF04_2                    writeMatrix[2][1]
#define W_FF04_3                    writeMatrix[2][2]
#define W_FF04_4                    writeMatrix[2][3]

//==================================================
// ROW 4
//==================================================
#define R_FF06_1                    readMatrix[3][0]
#define R_FF06_2                    readMatrix[3][1]   //(NA)
#define R_FF06_3                    readMatrix[3][2]
#define R_FF06_4                    readMatrix[3][3]

#define W_FF06_1                    writeMatrix[3][0]
#define W_FF06_2                    writeMatrix[3][1]   //(NA)
#define W_FF06_3                    writeMatrix[3][2]
#define W_FF06_4                    writeMatrix[3][3]

//==================================================
// ROW 5
//==================================================
#define R_FF07_1                    readMatrix[4][0]
#define R_FF07_2                    readMatrix[4][1]
#define R_FF07_3                    readMatrix[4][2]
#define R_FF07_4                    readMatrix[4][3]

#define W_FF07_1                    writeMatrix[4][0]
#define W_FF07_2                    writeMatrix[4][1]
#define W_FF07_3                    writeMatrix[4][2]
#define W_FF07_4                    writeMatrix[4][3]

//==================================================
// ROW 6
//==================================================
#define R_FF08_1                    readMatrix[5][0]
#define R_FF08_2                    readMatrix[5][1]
#define R_FF08_3                    readMatrix[5][2]
#define R_FF08_4                    readMatrix[5][3]

#define W_FF08_1                    writeMatrix[5][0]
#define W_FF08_2                    writeMatrix[5][1]
#define W_FF08_3                    writeMatrix[5][2]
#define W_FF08_4                    writeMatrix[5][3]

//==================================================
// ROW 7
//==================================================
#define R_FF09_1                    readMatrix[6][0]
#define R_FF09_2                    readMatrix[6][1]
#define R_FF09_3                    readMatrix[6][2]
#define R_FF09_4                    readMatrix[6][3]

#define W_FF09_1                    writeMatrix[6][0]
#define W_FF09_2                    writeMatrix[6][1]
#define W_FF09_3                    writeMatrix[6][2]
#define W_FF09_4                    writeMatrix[6][3]

//==================================================
// ROW 8
//==================================================
#define R_FF11_1                    readMatrix[7][0]
#define R_FF11_2                    readMatrix[7][1]
#define R_FF11_3                    readMatrix[7][2]
#define R_FF11_4                    readMatrix[7][3]

#define W_FF11_1                    writeMatrix[7][0]
#define W_FF11_2                    writeMatrix[7][1]
#define W_FF11_3                    writeMatrix[7][2]
#define W_FF11_4                    writeMatrix[7][3]

//==================================================
// ROW 9
//==================================================
#define R_FF12_1                    readMatrix[8][0]
#define R_FF12_2                    readMatrix[8][1]
#define R_FF12_3                    readMatrix[8][2]
#define R_FF12_4                    readMatrix[8][3]

#define W_FF12_1                    writeMatrix[8][0]
#define W_FF12_2                    writeMatrix[8][1]
#define W_FF12_3                    writeMatrix[8][2]
#define W_FF12_4                    writeMatrix[8][3]

//==================================================
// ROW 10
//==================================================
#define R_FF13_1                    readMatrix[9][0]
#define R_FF13_2                    readMatrix[9][1]
#define R_FF13_3                    readMatrix[9][2]
#define R_FF13_4                    readMatrix[9][3]

#define W_FF13_1                    writeMatrix[9][0]
#define W_FF13_2                    writeMatrix[9][1]
#define W_FF13_3                    writeMatrix[9][2]
#define W_FF13_4                    writeMatrix[9][3]

//==================================================
// ROW 11
//==================================================
#define R_FF14_1                    readMatrix[10][0]
#define R_FF14_2                    readMatrix[10][1]
#define R_FF14_3                    readMatrix[10][2]
#define R_FF14_4                    readMatrix[10][3]

#define W_FF14_1                    writeMatrix[10][0]
#define W_FF14_2                    writeMatrix[10][1]
#define W_FF14_3                    writeMatrix[10][2]
#define W_FF14_4                    writeMatrix[10][3]

    
void tuning_init(void);
void load_read_matrix(void);
void copy_default_values(void);
void update_flash_memory_values(void); 
void send_currently_used_values(uint8_t); 
void tune_command(uint8_t *); 
void send_measured_angle(float, float, float, int); 
void tune_fault_check(uint32_t *);


#ifdef __cplusplus
}
#endif

#endif /* _TUNING_H */

