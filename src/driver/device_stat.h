
#ifndef _DEVICE_STAT_H    /* Guard against multiple inclusion */
#define _DEVICE_STAT_H


#ifdef __cplusplus
extern "C" {
#endif
    
    int code_authentication(void);
    void CODESN_send(void);
    void DEVSN_send(void);
    void device_state_send(void);

#ifdef __cplusplus
}
#endif

#endif /* _DEVICE_STAT_H */

