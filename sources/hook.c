#include "hook.h"
#include "cr0.h"
#include "kallsyms_lookup.h"

#include <linux/kprobes.h>
#include <linux/module.h>
#include <linux/nospec.h>
#include <linux/string.h>

static unsigned long **old_syscall_table;
static unsigned long **syscall_table;
static unsigned long **crazydriver_syscall_table;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 8, 0)

static void *symbol_x64_sys_call;
static unsigned char x64_sys_call_recovery[20] = {};

const ERR hook_sys_call_table_x64(void) {  
  pr_info("crazydriver: Adding sys_call_table kernel patch\n");
  symbol_x64_sys_call = (void *)kallsyms_lookup_name("x64_sys_call");

  if (!symbol_x64_sys_call) {
    pr_info("crazydriver: Symbol 'x64_sys_call' not found\n");
    return ERR_FAILURE;
  }
  
  memcpy(x64_sys_call_recovery, symbol_x64_sys_call, 20);

  disable_register_cr0_wp();
  // Write the modified bytes into x64_sys_call memory
  memcpy((char *)symbol_x64_sys_call, "\x48\xB8",
          2); // movq syscall_table, %rax

  *(unsigned long *)(symbol_x64_sys_call + 2) =
      (unsigned long)syscall_table;
  memcpy((char *)symbol_x64_sys_call + 10, "\x48\x8d\x04\xF0", 4); // leaq  (%rax, %rsi, 8), %rax
  memcpy((char *)symbol_x64_sys_call + 14, "\x48\x8B\x00", 3); // movq  (%rax), %rax
  memcpy((char *)symbol_x64_sys_call + 17, "\xFF\xE0", 2); // jmp *%rax
  
  enable_register_cr0_wp();

  return ERR_SUCCESS;
}

void hook_remove_sys_call_table_x64(void) {
  pr_warn("crazydriver: Removing sys_call_table kernel patch ...\n");
  disable_register_cr0_wp();
  memcpy(symbol_x64_sys_call, x64_sys_call_recovery, 20);
  enable_register_cr0_wp();
}

#endif

ERR hook_init(void) {
  unsigned int i;

  syscall_table = (unsigned long **)kallsyms_lookup_name("sys_call_table");

  if (syscall_table == NULL) {
    pr_info("crazydriver: Failed to get syscall table\n");
    return ERR_FAILURE;
  }

  old_syscall_table = kmalloc(sizeof(void *) * __NR_syscalls, __GFP_HIGH);

  if (old_syscall_table == NULL)
    return ERR_FAILURE;

  crazydriver_syscall_table = kmalloc(sizeof(void *) * __NR_syscalls, __GFP_HIGH);

  if (crazydriver_syscall_table == NULL)
    return ERR_FAILURE;

  memcpy(old_syscall_table, syscall_table, sizeof(void *) * __NR_syscalls);

  memcpy(crazydriver_syscall_table, syscall_table,
         sizeof(void *) * __NR_syscalls);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 8, 0)

  pr_info("crazydriver: This version of the kernel was identified as no longer "
          "using sys_call_table, patching the kernel...\n");
  if (IS_ERR_FAILURE(hook_sys_call_table_x64())) {
    pr_warn(
        "crazydriver: Error in kernel patching, sys_call_table not restored\n");
    return ERR_FAILURE;
  }

#endif

  return ERR_SUCCESS;
}

void hook_end(void) {
  pr_warn("crazydriver: Hook syscalls shutdown ...\n");

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 8, 0)
  hook_remove_sys_call_table_x64();
#endif

  kfree(old_syscall_table);
  kfree(crazydriver_syscall_table);
}
