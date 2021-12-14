#include "tim.h"
#include "motor.h"
void arm_init(void);
void arm_set(const int rad[6]);
void printf_rad(const int arm_rad[6]);
void get_rad(void);
void arm_task(void);
void left_task(void);
void right_task(void);
void back_task(void);
extern const int arm_begin[6];
extern const int arm_clamp[6];
extern const int arm_doing[6];
extern int arm_state[6];