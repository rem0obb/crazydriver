#include <linux/kernel.h> /* We are doing kernel work */
#include <linux/module.h> /* Specifically, a module  */

#include "err.h"
#include "hook.h"

int __init init_module(void) {
  pr_info("crazydriver: Starting driver crazydriver ...\n");

    if (!IS_ERR_FAILURE(hook_init())) {
      /*...*/
    }

  return 0;
}

void __exit cleanup_module(void) {
  pr_warn("crazydriver: Shutdown driver crazydriver ...\n");
  hook_end();
}

MODULE_AUTHOR("VitorMob");
MODULE_DESCRIPTION("Crazy Driver");
MODULE_LICENSE("GPL");