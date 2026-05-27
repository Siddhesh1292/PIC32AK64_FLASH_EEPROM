

#ifndef FAULT_H
#define	FAULT_H

#ifdef	__cplusplus
extern "C" {
#endif

    void fault(uint32_t);
//fault id
    
#define OV_FAULT 1
#define IPH_FAULT 2
#define IBAT_FAULT 4
#define TMCU_FAULT 8
#define UV_FAULT 16
#define TMOT_FAULT 32
#define HALL_FAULT 64
#define MCUID_FAULT 128
#define THROTTLE_FAULT 256
#define SPEED_FAULT 512
#define BRAKE_FAULT 1024
#define STALL_FAULT 2048
#define OVERLOAD_FAULT 4096
#define AUXV_FAULT 8192
#define OPENT_FAULT 16384
#define REVERSET_FAULT 32768
#define FET_FAULT 65536
#define SOFTWARE_FAULT 131072
#define FOC_FAULT 262144
#define ISAMPLE_FAULT 524288
#define GUI_PAR_FAULT 1048576  
#define FOCBOOT_FAULT 2097152
#define EEPROM_FAULT 4194304
#define MSTART_FAULT 8388608
#define SELFLEARN_FAULT 16777216     
    
#ifdef	__cplusplus
}
#endif

#endif	/* FAULT_H */

