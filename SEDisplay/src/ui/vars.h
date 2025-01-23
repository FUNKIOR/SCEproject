#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// enum declarations



// Flow global variables

enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_POWER = 0,
    FLOW_GLOBAL_VARIABLE_RPM = 1
};

// Native global variables

extern int32_t get_var_power();
extern void set_var_power(int32_t value);
extern int32_t get_var_rpm();
extern void set_var_rpm(int32_t value);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/