/*
 * created by Le MIn 2017/12/09
 */

#ifndef _MINOS_SCHED_H_
#define _MINOS_SCHED_H_

#include <minos/vcpu.h>
#include <minos/percpu.h>
#include <minos/list.h>
#include <minos/timer.h>
#include <minos/sched_class.h>
#include <minos/atomic.h>

#define PCPU_AFFINITY_FAIL	(0xffff)

DECLARE_PER_CPU(struct pcpu *, pcpu);
DECLARE_PER_CPU(struct vcpu *, percpu_current_vcpu);
DECLARE_PER_CPU(struct vcpu *, percpu_next_vcpu);
DECLARE_PER_CPU(int, need_resched);

#define SCHED_REASON_IRQ	(0x0)
#define SCHED_FLAGS_PREEMPT	(1 << 0)

typedef enum _pcpu_state_t {
	PCPU_STATE_RUNNING	= 0x0,
	PCPU_STATE_IDLE,
	PCPU_STATE_OFFLINE,
} pcpu_state_t;

struct pcpu {
	uint32_t pcpu_id;
	int state;
	spinlock_t lock;

	struct sched_class *sched_class;
	void *sched_data;

	int nr_vcpus;
	int nr_running_vcpus;

	struct list_head vcpu_list;
};

#define pcpu_to_sched_data(pcpu)	(pcpu->sched_data)

void pcpus_init(void);
void sched(void);
int pcpu_add_vcpu(int cpu, struct vcpu *vcpu);
int pcpu_remove_vcpu(int cpu, struct vcpu *vcpu);
void set_vcpu_state(struct vcpu *vcpu, int state);
void kick_vcpu(struct vcpu *vcpu);
int sched_init(void);
int local_sched_init(void);
void sched_new(void);
void pcpu_resched(int pcpu_id);
int sched_reset_vcpu(struct vcpu *vcpu);
int sched_can_idle(struct pcpu *pcpu);

static inline void set_vcpu_ready(struct vcpu *vcpu)
{
	set_vcpu_state(vcpu, VCPU_STAT_READY);
}

static inline void set_vcpu_suspend(struct vcpu *vcpu)
{
	set_vcpu_state(vcpu, VCPU_STAT_SUSPEND);
}

static inline struct vcpu *get_current_vcpu(void)
{
	return get_cpu_var(percpu_current_vcpu);
}

static inline struct vm *get_current_vm(void)
{
	struct vcpu *vcpu = get_current_vcpu();
	return vcpu->vm;
}

static inline struct vcpu *get_next_vcpu(void)
{
	return get_cpu_var(percpu_next_vcpu);
}

static int inline need_resched(void)
{
	return get_cpu_var(need_resched);
}

static int inline get_vcpu_state(struct vcpu *vcpu)
{
	return vcpu->state;
}

static void inline set_next_vcpu(struct vcpu *vcpu)
{
	get_cpu_var(percpu_next_vcpu) = vcpu;
}

static void inline set_current_vcpu(struct vcpu *vcpu)
{
	get_cpu_var(percpu_current_vcpu) = vcpu;
}

static void inline set_need_resched(int n)
{
	get_cpu_var(need_resched) = n;
}

#endif
