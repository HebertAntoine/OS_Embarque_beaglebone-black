#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/kthread.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Antoine");
MODULE_DESCRIPTION("PWM logiciel 4 pins - 50Hz, 50% duty");

static int gpios[4] = {50, 51, 3, 2};   // P9_14, P9_16, P9_21, P9_22

#define PWM_FREQ_HZ     50
#define PERIOD_MS       (1000 / PWM_FREQ_HZ)     // 20 ms
#define DUTY_PCT        50
#define ON_MS           (PERIOD_MS * DUTY_PCT / 100)
#define OFF_MS          (PERIOD_MS - ON_MS)

static struct task_struct *thread;

static int pwm_thread_fn(void *data)
{
    pr_info("pwm_kernel: RUNNING (freq=%dHz duty=%d%%)\n", PWM_FREQ_HZ, DUTY_PCT);

    while (!kthread_should_stop()) {
        
        /* ON : 3.3V */
        gpio_set_value(gpios[0], 1);
        gpio_set_value(gpios[1], 1);
        gpio_set_value(gpios[2], 1);
        gpio_set_value(gpios[3], 1);
        msleep(ON_MS);

        /* OFF : 0V */
        gpio_set_value(gpios[0], 0);
        gpio_set_value(gpios[1], 0);
        gpio_set_value(gpios[2], 0);
        gpio_set_value(gpios[3], 0);
        msleep(OFF_MS);
    }

    pr_info("pwm_kernel: thread stopped\n");
    return 0;
}

static int __init pwm_init(void)
{
    int i, ret;

    pr_info("pwm_kernel: init\n");

    for (i = 0; i < 4; i++) {
        ret = gpio_request(gpios[i], "pwm_kernel");
        if (ret) return ret;

        gpio_direction_output(gpios[i], 0);
    }

    thread = kthread_run(pwm_thread_fn, NULL, "pwm_kernel_thread");
    if (IS_ERR(thread))
        return PTR_ERR(thread);

    return 0;
}

static void __exit pwm_exit(void)
{
    int i;

    if (thread)
        kthread_stop(thread);

    for (i = 0; i < 4; i++) {
        gpio_set_value(gpios[i], 0);
        gpio_free(gpios[i]);
    }

    pr_info("pwm_kernel: exit\n");
}

module_init(pwm_init);
module_exit(pwm_exit);