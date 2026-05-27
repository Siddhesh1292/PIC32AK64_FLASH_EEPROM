#include "tuning.h"
#include "device.h"
#include "driver_UART.h"
#include "fault.h"
#include "flash_eeprom.h"
#include "math.h"
#include "stdlib.h"
#include <string.h>


#define COPY_FLOAT_TO_TX(offset, value)                                        \
  do {                                                                         \
    float temp = (value);                                                      \
    __builtin_memcpy((uint8_t *)(send_tuning_data + offset), &temp, 4);        \
  } while (0)

float readMatrix[ROWS][COLS];
float writeMatrix[ROWS][COLS];

static uint8_t writeBuffer[EEPROM_DATA_SIZE];
static uint8_t readBuffer[EEPROM_DATA_SIZE];

static uint16_t received_rows_mask = 0;
static bool all_rows_received = false;

static uint8_t read_tuning_data[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static uint8_t send_tuning_data[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

float CONFIG_POLES = 48.0;
float CONFIG_RPH = 0.2;
float CONFIG_LD = 0.00015;
float CONFIG_LQ = 0.00015;
float CONFIG_PHIPH = 0.0204;
float CONFIG_SENSOR_DIRECTION = 0.0;    
float CONFIG_ROTATION_DIRECTION = 0.0;
float CONFIG_ANG_MARGINE = 40.0;  
float CONFIG_ZERO_ANGLE = 340.0;  
float CONFIG_OVFLT = 63.0;
float CONFIG_OVFLT_MAX = 90.0;
float CONFIG_UVFLT = 42.0;
float CONFIG_IPHFLT = 200.0;
float CONFIG_IPH_FLT_MAX = 350.0;
float CONFIG_IBATFLT = 100.0;
float CONFIG_IBAT_FLT_MAX = 200.0;
float CONFIG_ESC_TFLTC = 90.0;
float CONFIG_ESC_TFLTC_MAX = 125.0;
float CONFIG_MOTOR_TFLTC = 500.0;
float CONFIG_RPM_FLT = 800.0;
float CONFIG_RPM_TO_KMPH = 0.12;
float CONFIG_RPM_MAX = 650.0;
float CONFIG_IPH_MAX = 150.0;
float CONFIG_IPH_MAX_HLIMIT = 300.0;
float CONFIG_MOTOR_DERATEC = 500.0;
float CONFIG_ESC_DERATEC = 70.0;
float CONFIG_VBAT_MAX = 62.3;
float CONFIG_IBAT_DEFAULT = 30.3;
float CONFIG_IBAT_MAX = 40.0;
float CONFIG_IBAT_REGEN = 10.0;
float CONFIG_DRIVING_MODE = 1.0;
float CONFIG_DRIVING_INTERFACE = 0.0;
float CONFIG_L_RPM = 30.0;
float CONFIG_M_RPM = 60.0;    
float CONFIG_THROTTLE_ZERO = 1.0;
float CONFIG_THROTTLE_ZERO_LL = 0.2;  
float CONFIG_THROTTLE_ZERO_HL = 1.5;
float CONFIG_THROTTLE_MAX = 4.0;
float CONFIG_THROTTLE_MAX_LL = 1.6;
float CONFIG_THROTTLE_MAX_HL = 4.5;
float CONFIG_VBRAKE_DERATE = 54.0;
float CONFIG_K_AUTO_BRAKE = 0.0;
float CONFIG_REVERSE_RPM = 5.0;
float CONFIG_KP_RPM = 0.05;
float CONFIG_KP_RPM_MAX = 10.0;
float CONFIG_KP_RPM_MIN = 0.00001; 
float CONFIG_KI_RPM = 0.2;
float CONFIG_KI_RPM_MAX = 10.0;
float CONFIG_KI_RPM_MIN = 0.00001;
float CONFIG_L_ACCELERATION = 100.0;
float CONFIG_M_ACCELERATION = 100.0;
float CONFIG_L_IBAT = 100.0;
float CONFIG_M_IBAT = 100.0;
float CONFIG_L_IPH = 100.0;
float CONFIG_M_IPH = 100.0;
float CONFIG_BRAKE_ZERO = 2.0;
float CONFIG_BRAKE_ZERO_LL = 1.0;   
float CONFIG_BRAKE_ZERO_HL = 3.0;
float CONFIG_BRAKE_MAX = 4.0;
float CONFIG_BRAKE_MAX_LL = 3.6;
float CONFIG_BRAKE_MAX_HL = 4.5;
float CONFIG_SAFE_RPM = 5.0;
float CONFIG_BRAKE_RPM = 5.0;             
float CONFIG_REGEN_BRAKE = 10.0;          
float CONFIG_DI = 0.2;
float CONFIG_CANBAUD = 500.0;

static int ESCid = 0;
static float T_CONFIG_IPH_MAX, T_CONFIG_RPM_MAX;
static int T_poles; // FF01
static float T_CONFIG_RPH, T_Ld, T_CONFIG_LQ, T_CONFIG_PHIPH; // FF02
static float T_CONFIG_ZERO_ANGLE, T_CONFIG_ZERO_ANGLE_measured; // FF04
static int T_CONFIG_SENSOR_DIRECTION, T_CONFIG_SENSOR_DIRECTION_measured; // FF04

static int T_CONFIG_ROTATION_DIRECTION; // FF06
static float T_CONFIG_RPM_TO_KMPH, T_rpm_fault; // FF06
static float T_CONFIG_MOTOR_DERATEC, T_motor_TfaultC, T_CONFIG_ESC_DERATEC,
T_ESC_TfaultC; // FF07
static float T_CONFIG_VBAT_MAX, T_CONFIG_IBAT_DEFAULT, T_OV_fault, T_UV_fault; // FF08
static float T_CONFIG_IBAT_MAX, T_CONFIG_IBAT_REGEN, T_Iph_fault, T_Ibat_fault; // FF08

static int T_CONFIG_DRIVING_MODE; // FF11
static float T_CONFIG_L_RPM, T_CONFIG_M_RPM; // FF11
static float T_CONFIG_THROTTLE_ZERO, T_CONFIG_THROTTLE_MAX; // FF12
static float T_CONFIG_VBRAKE_DERATE, T_CONFIG_K_AUTO_BRAKE, T_CONFIG_REVERSE_RPM;
static float T_CONFIG_KP_RPM, T_CONFIG_KI_RPM, T_CONFIG_L_ACCELERATION, T_CONFIG_M_ACCELERATION; // FF13
static float T_CONFIG_L_IBAT, T_CONFIG_M_IBAT, T_CONFIG_L_IPH, T_CONFIG_M_IPH; // FF14
static int T_CAN_baud;

static int guipara_fault = 0;

void tuning_init(void) {
    memcpy(writeMatrix, readMatrix, sizeof (writeMatrix));

    received_rows_mask = 0;
    all_rows_received = false;
}

void load_read_matrix(void) {
    uint32_t temp32;

    memset(readBuffer, 0, EEPROM_DATA_SIZE);

    if (FLASH_EEPROM_Read_Page(readBuffer, sizeof (readMatrix)) ==
            FLASH_EEPROM_OK) {
        //        BLUE_LED_Set();
        //--------------------------------------------------
        // Validate all float entries
        //--------------------------------------------------
        for (uint32_t i = 0; i < (ROWS * COLS); i++) {
            __builtin_memcpy(&temp32, &readBuffer[i * 4], 4);

            //--------------------------------------------------
            // Check erased flash
            //--------------------------------------------------
            if (temp32 == 0xFFFFFFFF) {
                //                BLUE_LED_Set();
                guipara_fault = 1;
                break;
            }
        }

        //--------------------------------------------------
        // Copy to matrix only after validation
        //--------------------------------------------------
        //        BLUE_LED_Set();
        memcpy(readMatrix, readBuffer, sizeof (readMatrix));
    } else {
        guipara_fault = 1;
    }

    ESCid = R_FF01_1;

    //--------------------------------------------------
    // Check ESC ID
    //--------------------------------------------------
    if (ESCid <= 0) {
//        BLUE_LED_Set();
        copy_default_values();
    }
}

void copy_default_values(void){
    //--------------------------------------------------
    // Load default values
    //--------------------------------------------------

    // Row 1
    R_FF01_1 = R_FF01_1;
    R_FF01_2 = CONFIG_IPH_MAX;
    R_FF01_3 = CONFIG_RPM_MAX;
    R_FF01_4 = CONFIG_POLES;

    // Row 2
    R_FF02_1 = CONFIG_RPH;
    R_FF02_2 = CONFIG_LD;
    R_FF02_3 = CONFIG_LQ;
    R_FF02_4 = CONFIG_PHIPH;

    // Row 3
    R_FF04_1 = CONFIG_ZERO_ANGLE;
    R_FF04_2 = CONFIG_SENSOR_DIRECTION;
    R_FF04_3 = 0.0f;
    R_FF04_4 = 0.0f;

    // Row 4
    R_FF06_1 = CONFIG_ROTATION_DIRECTION;
    R_FF06_2 = CONFIG_CANBAUD;
    R_FF06_3 = CONFIG_RPM_TO_KMPH;
    R_FF06_4 = CONFIG_RPM_FLT;

    // Row 5
    R_FF07_1 = CONFIG_MOTOR_DERATEC;
    R_FF07_2 = CONFIG_MOTOR_TFLTC;
    R_FF07_3 = CONFIG_ESC_DERATEC;
    R_FF07_4 = CONFIG_ESC_TFLTC;

    // Row 6
    R_FF08_1 = CONFIG_VBAT_MAX;
    R_FF08_2 = CONFIG_IBAT_DEFAULT;
    R_FF08_3 = CONFIG_OVFLT;
    R_FF08_4 = CONFIG_UVFLT;

    // Row 7
    R_FF09_1 = CONFIG_IBAT_MAX;
    R_FF09_2 = CONFIG_IBAT_REGEN;
    R_FF09_3 = CONFIG_IPHFLT * 1.414213;
    R_FF09_4 = CONFIG_IBATFLT;

    // Row 8
    R_FF11_1 = CONFIG_DRIVING_MODE;
    R_FF11_2 = CONFIG_REVERSE_RPM;
    R_FF11_3 = CONFIG_L_RPM;
    R_FF11_4 = CONFIG_M_RPM;

    // Row 9
    R_FF12_1 = CONFIG_THROTTLE_ZERO;
    R_FF12_2 = CONFIG_THROTTLE_MAX;
    R_FF12_3 = CONFIG_VBRAKE_DERATE;
    R_FF12_4 = CONFIG_K_AUTO_BRAKE / 100.0;

    // Row 10
    R_FF13_1 = CONFIG_KP_RPM;
    R_FF13_2 = CONFIG_KI_RPM;
    R_FF13_3 = CONFIG_L_ACCELERATION;
    R_FF13_4 = CONFIG_M_ACCELERATION;

    // Row 11
    R_FF14_1 = CONFIG_L_IBAT;
    R_FF14_2 = CONFIG_M_IBAT;
    R_FF14_3 = CONFIG_L_IPH;
    R_FF14_4 = CONFIG_M_IPH;
}

void tune_fault_check(uint32_t *fault_code_adr) {
    if (guipara_fault == 1)
        *fault_code_adr = *fault_code_adr | GUI_PAR_FAULT;
}

void send_measured_angle(float theta, float theta0, float theta1,
        int CONFIG_SENSOR_DIRECTION_estimate) {
    R_FF04_3 = theta; // update the zero angle
    R_FF04_4 = CONFIG_SENSOR_DIRECTION_estimate; // update correct direction of sensor
    send_currently_used_values(0x04);
}

void send_currently_used_values(uint8_t adr_HB) {
    send_tuning_data[0] = 0xFF;
    send_tuning_data[1] = adr_HB;

    send_tuning_data[18] = 0xFF;
    send_tuning_data[19] = adr_HB;

    if (adr_HB == 0x01) {
        COPY_FLOAT_TO_TX(2, R_FF01_1);
        COPY_FLOAT_TO_TX(6, R_FF01_2);
        COPY_FLOAT_TO_TX(10, R_FF01_3);
        COPY_FLOAT_TO_TX(14, R_FF01_4);
    } else if (adr_HB == 0x02) {
        COPY_FLOAT_TO_TX(2, R_FF02_1);
        COPY_FLOAT_TO_TX(6, R_FF02_2);
        COPY_FLOAT_TO_TX(10, R_FF02_3);
        COPY_FLOAT_TO_TX(14, R_FF02_4);
    } else if (adr_HB == 0x04) {
        COPY_FLOAT_TO_TX(2, R_FF04_1);
        COPY_FLOAT_TO_TX(6, R_FF04_2);
        COPY_FLOAT_TO_TX(10, R_FF04_3);
        COPY_FLOAT_TO_TX(14, R_FF04_4);
    } else if (adr_HB == 0x06) {
        COPY_FLOAT_TO_TX(2, R_FF06_1);
        COPY_FLOAT_TO_TX(6, R_FF06_2);
        COPY_FLOAT_TO_TX(10, R_FF06_3);
        COPY_FLOAT_TO_TX(14, R_FF06_4);
    } else if (adr_HB == 0x07) {
        COPY_FLOAT_TO_TX(2, R_FF07_1);
        COPY_FLOAT_TO_TX(6, R_FF07_2);
        COPY_FLOAT_TO_TX(10, R_FF07_3);
        COPY_FLOAT_TO_TX(14, R_FF07_4);
    } else if (adr_HB == 0x08) {
        COPY_FLOAT_TO_TX(2, R_FF08_1);
        COPY_FLOAT_TO_TX(6, R_FF08_2);
        COPY_FLOAT_TO_TX(10, R_FF08_3);
        COPY_FLOAT_TO_TX(14, R_FF08_4);
    } else if (adr_HB == 0x09) {
        COPY_FLOAT_TO_TX(2, R_FF09_1);
        COPY_FLOAT_TO_TX(6, R_FF09_2);
        COPY_FLOAT_TO_TX(10, R_FF09_3);
        COPY_FLOAT_TO_TX(14, R_FF09_4);
    } else if (adr_HB == 0x11) {
        COPY_FLOAT_TO_TX(2, R_FF11_1);
        COPY_FLOAT_TO_TX(6, R_FF11_2);
        COPY_FLOAT_TO_TX(10, R_FF11_3);
        COPY_FLOAT_TO_TX(14, R_FF11_4);
    } else if (adr_HB == 0x12) {
        COPY_FLOAT_TO_TX(2, R_FF12_1);
        COPY_FLOAT_TO_TX(6, R_FF12_2);
        COPY_FLOAT_TO_TX(10, R_FF12_3);
        COPY_FLOAT_TO_TX(14, R_FF12_4);
    } else if (adr_HB == 0x13) {
        COPY_FLOAT_TO_TX(2, R_FF13_1);
        COPY_FLOAT_TO_TX(6, R_FF13_2);
        COPY_FLOAT_TO_TX(10, R_FF13_3);
        COPY_FLOAT_TO_TX(14, R_FF13_4);
    } else if (adr_HB == 0x14) {
        COPY_FLOAT_TO_TX(2, R_FF14_1);
        COPY_FLOAT_TO_TX(6, R_FF14_2);
        COPY_FLOAT_TO_TX(10, R_FF14_3);
        COPY_FLOAT_TO_TX(14, R_FF14_4);
    } else {
        send_tuning_data[18] = 0xF1;
    }

    send_data_uart(&send_tuning_data[0], 20);
}

static int8_t get_row_index(uint8_t guiRow) {
    switch (guiRow) {
        case 0x01:
            return 0;
        case 0x02:
            return 1;
        case 0x04:
            return 2;
        case 0x06:
            return 3;
        case 0x07:
            return 4;
        case 0x08:
            return 5;
        case 0x09:
            return 6;
        case 0x11:
            return 7;
        case 0x12:
            return 8;
        case 0x13:
            return 9;
        case 0x14:
            return 10;

        default:
            return -1;
    }
}

void update_flash_memory_values(void) {
    int8_t row;

    row = get_row_index(read_tuning_data[1]);

    if (row < 0) {
        return;
    }

    //--------------------------------------------------
    // Store 4 float values into write matrix
    //--------------------------------------------------
    memcpy(&writeMatrix[row][0], &read_tuning_data[2], 16);

    //--------------------------------------------------
    // Prepare EEPROM write buffer with the complete matrix.
    //--------------------------------------------------
    memset(writeBuffer, 0xFF, EEPROM_DATA_SIZE);

    memcpy(writeBuffer, writeMatrix, sizeof (writeMatrix));

    //--------------------------------------------------
    // Write complete matrix to EEPROM so this row survives reset.
    //--------------------------------------------------
    if (FLASH_EEPROM_Write_Page(writeBuffer, sizeof (writeMatrix)) ==
            FLASH_EEPROM_OK) {
//        GREEN_LED_Set();

        //--------------------------------------------------
        // Update runtime matrix
        //--------------------------------------------------
        memcpy(readMatrix, writeMatrix, sizeof (readMatrix));

        all_rows_received = true;

        //--------------------------------------------------
        // ACK only after the row has been committed.
        //--------------------------------------------------
        send_data_uart(&read_tuning_data[0], 20);
    } else {
        //        GREEN_LED_Set();
    }
}

void tune_command(uint8_t *command_adr) {
    //--------------------------------------------------
    // Copy full command
    //--------------------------------------------------
    memcpy(read_tuning_data, command_adr, 20);

    //--------------------------------------------------
    // WRITE COMMAND
    //--------------------------------------------------
    if (read_tuning_data[18] == 0xFF) {
        //--------------------------------------------------
        // Validate packet
        //--------------------------------------------------
        if (read_tuning_data[1] == read_tuning_data[19]) {
            //--------------------------------------------------
            // Store row
            //--------------------------------------------------
            update_flash_memory_values();
        }
    }//--------------------------------------------------
        // READ COMMAND
        //--------------------------------------------------
    else {
        send_currently_used_values(read_tuning_data[1]);
    }
}