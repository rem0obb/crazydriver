#pragma once

#include "err.h"
#include <linux/kprobes.h>
#include <linux/version.h>

const __must_check ERR hook_init(void) notrace;
void hook_end(void) notrace;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 8, 0)

const __must_check ERR hook_sys_call_table_x64(void) notrace;
void hook_remove_sys_call_table_x64(void) notrace;

#endif