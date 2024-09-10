#include "pinctrl.h"
#include "soc_osal.h"
#include "pwm.h"
#include "gpio.h"
#include "osal_debug.h"
#include "cmsis_os2.h"
#include "app_init.h"

#define PWM_GPIO                   2
#define PWM_PIN_MODE               1
#define PWM_CHANNEL                2
#define PWM_GROUP_ID               0
#define PWM_FREQUENCY              1000  // 1kHz
#define PWM_TASK_STACK_SIZE        0x1000
#define PWM_TASK_DURATION_MS       1000
#define PWM_TASK_PRIO              (osPriority_t)(17)

static volatile uint32_t g_pwm_duty_cycle = 30;  // Initial duty cycle 30%

static void app_pwm_init_pin(void)
{
    uapi_pin_set_mode(PWM_GPIO, PWM_PIN_MODE);
}

static void app_pwm_init_config(void)
{
    pwm_config_t config = {0};
    uint32_t pwm_clock = uapi_pwm_get_frequency(PWM_CHANNEL);
    uint32_t pwm_period = pwm_clock / PWM_FREQUENCY;
    uint32_t pwm_high_time = (pwm_period * g_pwm_duty_cycle) / 100;
    uint32_t pwm_low_time = pwm_period - pwm_high_time;

    config.low_time = pwm_low_time;
    config.high_time = pwm_high_time;
    config.offset_time = 0;
    config.cycles = 0;
    config.repeat = true;

    errcode_t ret = uapi_pwm_init();
    if (ret != ERRCODE_SUCC) {
        osal_printk("Failed to initialize PWM, error code: %d\r\n", ret);
        return;
    }

    ret = uapi_pwm_open(PWM_CHANNEL, &config);
    if (ret != ERRCODE_SUCC) {
        osal_printk("Failed to open PWM channel, error code: %d\r\n", ret);
        return;
    }

    uint8_t channel_id = PWM_CHANNEL;
    ret = uapi_pwm_set_group(PWM_GROUP_ID, &channel_id, 1);
    if (ret != ERRCODE_SUCC) {
        osal_printk("Failed to set PWM group, error code: %d\r\n", ret);
        return;
    }
}

static errcode_t pwm_update_duty_cycle(uint32_t duty_cycle)
{
    uint32_t pwm_clock = uapi_pwm_get_frequency(PWM_CHANNEL);
    uint32_t pwm_period = pwm_clock / PWM_FREQUENCY;
    uint32_t pwm_high_time = (pwm_period * duty_cycle) / 100;
    uint32_t pwm_low_time = pwm_period - pwm_high_time;

    pwm_config_t cfg = {
        pwm_low_time,
        pwm_high_time,
        0,
        0,
        true
    };

    errcode_t ret = uapi_pwm_open(PWM_CHANNEL, &cfg);
    if (ret != ERRCODE_SUCC) {
        osal_printk("Failed to reconfigure PWM, error code: %d\r\n", ret);
        return ret;
    }

    osal_printk("PWM duty cycle updated to %lu%%\r\n", duty_cycle);
    return ERRCODE_SUCC;
}

static void *pwm_task(const char *arg)
{
    unused(arg);

    /* PWM pinmux. */
    app_pwm_init_pin();

    /* PWM init config. */
    app_pwm_init_config();

    errcode_t ret = uapi_pwm_start_group(PWM_GROUP_ID);
    if (ret != ERRCODE_SUCC) {
        osal_printk("Failed to start PWM group, error code: %d\r\n", ret);
        return NULL;
    }

    osal_printk("PWM started with %lu%% duty cycle\r\n", g_pwm_duty_cycle);

    uint32_t last_duty_cycle = g_pwm_duty_cycle;

    while (1) {
        osal_msleep(PWM_TASK_DURATION_MS);
        
        if (g_pwm_duty_cycle != last_duty_cycle) {
            ret = pwm_update_duty_cycle(g_pwm_duty_cycle);
            if (ret == ERRCODE_SUCC) {
                last_duty_cycle = g_pwm_duty_cycle;
            }
        }
        
        osal_printk("PWM running, current duty cycle: %lu%%\r\n", last_duty_cycle);
    }

    return NULL;
}

static void pwm_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "PWMTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = PWM_TASK_STACK_SIZE;
    attr.priority = PWM_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)pwm_task, NULL, &attr) == NULL) {
        osal_printk("Failed to create PWM task\r\n");
    } else {
        osal_printk("PWM task created successfully\r\n");
    }
}

/* Run the pwm_entry. */
app_run(pwm_entry);