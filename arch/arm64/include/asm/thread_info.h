/*
 * Based on arch/arm/include/asm/thread_info.h
 *
 * Copyright (C) 2002 Russell King.
 * Copyright (C) 2012 ARM Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __ASM_THREAD_INFO_H
#define __ASM_THREAD_INFO_H

#ifdef __KERNEL__

#include <linux/compiler.h>

#ifndef CONFIG_ARM64_64K_PAGES
#define THREAD_SIZE_ORDER	2
#endif

#define THREAD_SIZE		16384
#define THREAD_START_SP		(THREAD_SIZE - 16)

#ifndef __ASSEMBLY__

struct task_struct;
struct exec_domain;

#include <asm/stack_pointer.h>
#include <asm/types.h>

typedef unsigned long mm_segment_t;

/*
 * low level task data that entry.S needs immediate access to.
 */
struct thread_info {
	unsigned long		flags;		/* low level flags */
	mm_segment_t		addr_limit;	/* address limit */
	struct task_struct	*task;		/* main task structure */
	struct exec_domain	*exec_domain;	/* execution domain */
#ifdef CONFIG_ARM64_SW_TTBR0_PAN
	u64			ttbr0;		/* saved TTBR0_EL1 */
#endif
	int			preempt_count;	/* 0 => preemptable, <0 => bug */
	int			cpu;		/* cpu */
#ifdef CONFIG_ARCH_THREAD_INFO_ALLOCATOR
	phys_addr_t		phys_addr;	/* set if vmalloc */
#endif
};

#define INIT_THREAD_INFO(tsk)						\
{									\
	.task		= &tsk,						\
	.exec_domain	= &default_exec_domain,				\
	.flags		= 0,						\
	.preempt_count	= INIT_PREEMPT_COUNT,				\
	.addr_limit	= KERNEL_DS,					\
}

#ifndef CONFIG_THREAD_INFO_IN_TASK
#define init_thread_info	(init_thread_union.thread_info)
#endif

#define init_stack		(init_thread_union.stack)

/*
 * how to get the thread information struct from C
 */
static inline struct thread_info *current_thread_info(void) __attribute_const__;

/*
 * struct thread_info can be accessed directly via sp_el0.
 */
static inline struct thread_info *current_thread_info(void)
{
	unsigned long sp_el0;

	asm ("mrs %0, sp_el0" : "=r" (sp_el0));

	return (struct thread_info *)sp_el0;
}

#define thread_saved_pc(tsk)	\
	((unsigned long)(tsk->thread.cpu_context.pc))
#define thread_saved_sp(tsk)	\
	((unsigned long)(tsk->thread.cpu_context.sp))
#define thread_saved_fp(tsk)	\
	((unsigned long)(tsk->thread.cpu_context.fp))

#ifdef CONFIG_ARCH_THREAD_INFO_ALLOCATOR
struct thread_info *alloc_thread_info_node(struct task_struct *tsk,
						  int node);
void free_thread_info(struct thread_info *ti);
#endif

#endif

/*
 * thread information flags:
 *  TIF_SYSCALL_TRACE	- syscall trace active
 *  TIF_SYSCALL_TRACEPOINT - syscall tracepoint for ftrace
 *  TIF_SYSCALL_AUDIT	- syscall auditing
 *  TIF_SECOMP		- syscall secure computing
 *  TIF_SIGPENDING	- signal pending
 *  TIF_NEED_RESCHED	- rescheduling necessary
 *  TIF_NOTIFY_RESUME	- callback before returning to user
 *  TIF_USEDFPU		- FPU was used by this task this quantum (SMP)
 */
#define TIF_SIGPENDING		0
#define TIF_NEED_RESCHED	1
#define TIF_NOTIFY_RESUME	2	/* callback before returning to user */
#define TIF_FOREIGN_FPSTATE	3	/* CPU's FP state is not current's */
#define TIF_FSCHECK		4	/* Check FS is USER_DS on return */
#define TIF_NOHZ		7
#define TIF_SYSCALL_TRACE	8
#define TIF_SYSCALL_AUDIT	9
#define TIF_SYSCALL_TRACEPOINT	10
#define TIF_SECCOMP		11
#define TIF_MEMDIE		18	/* is terminating due to OOM killer */
#define TIF_FREEZE		19
#define TIF_RESTORE_SIGMASK	20
#define TIF_SINGLESTEP		21
#define TIF_32BIT		22	/* 32bit process */
#define TIF_SWITCH_MM		23	/* deferred switch_mm */
#define TIF_MM_RELEASED		24

#define _TIF_SIGPENDING		(1 << TIF_SIGPENDING)
#define _TIF_NEED_RESCHED	(1 << TIF_NEED_RESCHED)
#define _TIF_NOTIFY_RESUME	(1 << TIF_NOTIFY_RESUME)
#define _TIF_FOREIGN_FPSTATE	(1 << TIF_FOREIGN_FPSTATE)
#define _TIF_NOHZ		(1 << TIF_NOHZ)
#define _TIF_SYSCALL_TRACE	(1 << TIF_SYSCALL_TRACE)
#define _TIF_SYSCALL_AUDIT	(1 << TIF_SYSCALL_AUDIT)
#define _TIF_SYSCALL_TRACEPOINT	(1 << TIF_SYSCALL_TRACEPOINT)
#define _TIF_SECCOMP		(1 << TIF_SECCOMP)
#define _TIF_FSCHECK		(1 << TIF_FSCHECK)
#define _TIF_32BIT		(1 << TIF_32BIT)

#define _TIF_WORK_MASK		(_TIF_NEED_RESCHED | _TIF_SIGPENDING | \
				 _TIF_NOTIFY_RESUME | _TIF_FOREIGN_FPSTATE | \
				 _TIF_FSCHECK)

#define _TIF_SYSCALL_WORK	(_TIF_SYSCALL_TRACE | _TIF_SYSCALL_AUDIT | \
				 _TIF_SYSCALL_TRACEPOINT | _TIF_SECCOMP | \
				 _TIF_NOHZ)

#endif /* __KERNEL__ */
#endif /* __ASM_THREAD_INFO_H */
