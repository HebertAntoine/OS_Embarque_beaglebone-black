#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/kthread.h>
#include <linux/delay.h>

#define NUM_GPIO 4

// GPIO BeagleBone compatibles GPIO (fonctionnent)
static int gpios[NUM_GPIO] = {68, 67, 69, 61};

static struct task_struct *blink_thread;

// Thread de clignotement
static int blink_fn(void *data)
{
    bool state = false;

    pr_info("gpio_blink: thread démarré\n");

    while (!kthread_should_stop()) {
        int i;

        // ON ou OFF
        state = !state;

        for (i = 0; i < NUM_GPIO; i++)
            gpio_set_value(gpios[i], state);

        pr_info("gpio_blink: GPIO = %d\n", state);

        ssleep(1);   // 1 seconde
    }

    return 0;
}

static int __init gpio_blink_init(void)
{
    int i, ret;

    pr_info("gpio_blink: init\n");

    // Demande + direction out
    for (i = 0; i < NUM_GPIO; i++) {
        ret = gpio_request(gpios[i], "gpio_blink");
        if (ret) {
            pr_err("gpio_blink: erreur gpio_request %d\n", gpios[i]);
            return ret;
        }

        ret = gpio_direction_output(gpios[i], 0);
        if (ret) {
            pr_err("gpio_blink: erreur direction_output %d\n", gpios[i]);
            return ret;
        }
    }

    // Démarrage du thread
    blink_thread = kthread_run(blink_fn, NULL, "gpio_blink_thread");
    if (IS_ERR(blink_thread)) {
        pr_err("gpio_blink: impossible démarrer thread\n");
        return PTR_ERR(blink_thread);
    }

    return 0;
}

static void __exit gpio_blink_exit(void)
{
    int i;

    pr_info("gpio_blink: exit\n");

    if (blink_thread)
        kthread_stop(blink_thread);

    for (i = 0; i < NUM_GPIO; i++) {
        gpio_set_value(gpios[i], 0);
        gpio_free(gpios[i]);
    }
}

module_init(gpio_blink_init);
module_exit(gpio_blink_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Antoine");
MODULE_DESCRIPTION("Module noyau : 4 GPIO blink 1s");