/* gpio_btn_softpwm.c
 *
 * Module noyau BBB:
 * - Bouton entrée : P8_11 -> GPIO45
 * - 4 GPIO sorties LED : {68,67,69,61} (ON/OFF constant)
 * - 4 PWM logiciels sur 4 GPIO (bit-banging à 50Hz)
 *
 * Séquence par appui (front montant sur bouton):
 *  0: tout OFF
 *  1: LED ON constant, PWM OFF
 *  2: LED ON constant, PWM = 20%
 *  3: LED ON constant, PWM = 40%
 *  puis retour à 0
 */

 #include <linux/module.h>
 #include <linux/kernel.h>
 #include <linux/gpio.h>
 #include <linux/kthread.h>
 #include <linux/delay.h>
 #include <linux/err.h>
 
 MODULE_LICENSE("GPL");
 MODULE_AUTHOR("Antoine");
 MODULE_DESCRIPTION("BBB: bouton -> LED ON + PWM logiciel 20/40% + OFF");
 
 /* === Bouton === */
 static int btn_gpio = 45; /* P8_11 */
 module_param(btn_gpio, int, 0444);
 
 /* === LED GPIO (ON constant) === */
 static int led_gpios[4] = { 68, 67, 69, 61 };
 
 /* === PWM logiciel sur 4 GPIO ===
  * METS ICI les vrais GPIO globaux correspondant à tes pins PWM.
  * (Tes valeurs 50,51,3,2 sont à confirmer)
  */
 static int pwm_gpios[4] = { 50, 51, 3, 2 };
 module_param_array(pwm_gpios, int, NULL, 0444);
 
 /* PWM */
 #define PWM_FREQ_HZ 50
 #define PERIOD_MS   (1000 / PWM_FREQ_HZ) /* 20ms */
 
 static int duty_pct = 0; /* 0,20,40 */
 static int mode = 0;     /* 0..3 */
 
 static struct task_struct *thread;
 
 /* Appliquer mode */
 static void apply_mode(int m)
 {
     int i;
 
     if (m == 0) {
         /* tout OFF */
         for (i = 0; i < 4; i++)
             gpio_set_value(led_gpios[i], 0);
         duty_pct = 0;
         pr_info("softpwm: mode=0 -> LED OFF, PWM OFF\n");
         return;
     }
 
     /* LED ON constant (m=1,2,3) */
     for (i = 0; i < 4; i++)
         gpio_set_value(led_gpios[i], 1);
 
     if (m == 1) {
         duty_pct = 0;
         pr_info("softpwm: mode=1 -> LED ON, PWM OFF\n");
     } else if (m == 2) {
         duty_pct = 20;
         pr_info("softpwm: mode=2 -> LED ON, PWM 20%%\n");
     } else {
         duty_pct = 40;
         pr_info("softpwm: mode=3 -> LED ON, PWM 40%%\n");
     }
 }
 
 /* Thread: PWM + bouton */
 static int worker_fn(void *data)
 {
     int last = 0, now;
     int on_ms, off_ms;
     int i;
 
     apply_mode(mode);
 
     while (!kthread_should_stop()) {
 
         /* --- lecture bouton + front montant --- */
         now = gpio_get_value(btn_gpio);
         if (last == 0 && now == 1) {
             msleep(50); /* anti-rebond */
             now = gpio_get_value(btn_gpio);
             if (now == 1) {
                 mode = (mode + 1) % 4;
                 pr_info("softpwm: bouton press -> new mode=%d\n", mode);
                 apply_mode(mode);
             }
         }
         last = now;
 
         /* --- PWM logiciel (si duty>0) --- */
         if (duty_pct > 0) {
             on_ms  = (PERIOD_MS * duty_pct) / 100;
             off_ms = PERIOD_MS - on_ms;
 
             for (i = 0; i < 4; i++)
                 gpio_set_value(pwm_gpios[i], 1);
             msleep(on_ms);
 
             for (i = 0; i < 4; i++)
                 gpio_set_value(pwm_gpios[i], 0);
             msleep(off_ms);
         } else {
             /* PWM OFF => forcer bas */
             for (i = 0; i < 4; i++)
                 gpio_set_value(pwm_gpios[i], 0);
 
             msleep(20); /* boucle légère */
         }
     }
 
     return 0;
 }
 
 static int __init mod_init(void)
 {
     int i, ret;
 
     pr_info("softpwm: init (btn GPIO%d)\n", btn_gpio);
 
     /* bouton */
     ret = gpio_request(btn_gpio, "softpwm_btn");
     if (ret) return ret;
     ret = gpio_direction_input(btn_gpio);
     if (ret) { gpio_free(btn_gpio); return ret; }
 
     /* LED GPIO */
     for (i = 0; i < 4; i++) {
         ret = gpio_request(led_gpios[i], "softpwm_led");
         if (ret) return ret;
         gpio_direction_output(led_gpios[i], 0);
     }
 
     /* PWM GPIO */
     for (i = 0; i < 4; i++) {
         ret = gpio_request(pwm_gpios[i], "softpwm_pwm");
         if (ret) return ret;
         gpio_direction_output(pwm_gpios[i], 0);
     }
 
     thread = kthread_run(worker_fn, NULL, "softpwm_thread");
     if (IS_ERR(thread))
         return PTR_ERR(thread);
 
     return 0;
 }
 
 static void __exit mod_exit(void)
 {
     int i;
 
     if (thread)
         kthread_stop(thread);
 
     /* tout OFF */
     for (i = 0; i < 4; i++) {
         gpio_set_value(led_gpios[i], 0);
         gpio_free(led_gpios[i]);
     }
 
     for (i = 0; i < 4; i++) {
         gpio_set_value(pwm_gpios[i], 0);
         gpio_free(pwm_gpios[i]);
     }
 
     gpio_free(btn_gpio);
 
     pr_info("softpwm: exit\n");
 }
 
 module_init(mod_init);
 module_exit(mod_exit);