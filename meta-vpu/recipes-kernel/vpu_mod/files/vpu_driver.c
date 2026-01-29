#include <linux/kernel.h>
#include <linux/module.h>

static int __init vpu_init(void) {
    pr_info("VPU: Virtual Accelerator Driver loaded.\n");
    return 0;
}

static void __exit vpu_exit(void) {
    pr_info("VPU: Virtual Accelerator Driver unloaded.\n");
}

module_init(vpu_init);
module_exit(vpu_exit);
MODULE_LICENSE("GPL");
