
 #include <linux/fs.h>
 #include <linux/gpio.h>
 #include <linux/interrupt.h>
 #include <linux/module.h>

// #include <rtdm/driver.h>


 static unsigned int irq_num;
 // bbb gpios that work on PREEMPT_RT linux 4.19.59 and 5.4.5 with default dts
 static unsigned int gpio_out = 7; // PIN9_42
static unsigned int gpio_in = 44; // PIN8_12
 static bool value = true;
//static rtdm_irq_t irq_handle;
 static int num_of_intr = 0;


 static int gpio_irq_handler(unsigned long ptr, void* arg)
 {
      value = !value;
      gpio_set_value(gpio_out, value); // toggle the led everytime irq handler is invoked
      trace_printk("GPIO interrupt \n");
      num_of_intr++;
      return IRQ_HANDLED;
 }


 static int __init gene_init (void)
 {
      int err;

      if ((err = gpio_request(gpio_in, THIS_MODULE->name)) != 0) {
          printk(" gpio_request gpio_in failed ! \n");
          return err;
      }

      if ((err = gpio_direction_input(gpio_in)) != 0) {
          printk(" gpio_direction_input gpio_in failed ! \n");
          gpio_free(gpio_in);

          return err;
      }

      irq_num = gpio_to_irq(gpio_in);

      printk(" IRQ number %d !  \n",irq_num);

      if ((err = gpio_request(gpio_out, THIS_MODULE->name)) != 0) {
          printk(" gpio_request gpio_out failed ! \n");
          gpio_free(gpio_in);
          return err;
      }

      if ((err = gpio_direction_output(gpio_out, 0)) != 0) {
          printk(" gpio_direction_input gpio_out failed ! \n");
          gpio_free(gpio_out);
          gpio_free(gpio_in);
          return err;
      }

      disable_irq(irq_num);
      err = irq_set_irq_type(irq_num,  IRQF_TRIGGER_RISING);

      if(err) {
          gpio_free(gpio_out);
          gpio_free(gpio_in);
          printk(" irq_set_irq_type failed ! \n");
          return err;
      }

      err = request_any_context_irq(irq_num,(irq_handler_t)gpio_irq_handler, IRQ_TYPE_EDGE_FALLING,THIS_MODULE->name, NULL);

      printk("after request irq = %d \n",irq_num);

      if(err) {
          gpio_free(gpio_out);
          gpio_free(gpio_in);
          printk(" rtdm_irq_request failed ! \n");
          return err;
      }

     enable_irq(irq_num);
      
     /*if (err < 0) {
          printk("rtdm_irq_enable failed \n");
          return err;
	  }*/
      gpio_set_value(gpio_out, 1);

      return 0;


 }



 static void __exit gene_exit (void)
 {
   gpio_set_value(gpio_out, 0);

   free_irq(irq_num,NULL);
   gpio_free(gpio_out);
   gpio_free(gpio_in);
   
   printk("The number of intr is %d \n",num_of_intr);
 }


 module_init(gene_init);
 module_exit(gene_exit);
 MODULE_LICENSE("GPL");
