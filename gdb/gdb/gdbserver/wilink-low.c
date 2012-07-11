/*
 *
 *  Created on: Jul 8, 2012
 *      Author: itamarb@ti.com
 */

#include "server.h"
#include "regdef.h"
#include <stdio.h>
#include "wilink-low.h"

extern struct wilink_target_ops the_low_target;

int using_threads = 0;
static int attached_pid = -1;

int wilink_create_inferior(char *program, char **args) {
	fprintf(stderr, "%s\n", __func__);
	return -1; // not supported
}

int wilink_attach(unsigned long pid) {
	ptid_t ptid;

	fprintf(stderr, "%s\n", __func__);

	attached_pid = pid;

	ptid.pid = pid;
	ptid.lwp = 0;
	ptid.tid = 1;
	add_process(pid, 1);
	add_thread(ptid, NULL);

	return 0;
}

int wilink_kill(int pid) {
	fprintf(stderr, "%s\n", __func__);
	return -1; // not supported
}

int wilink_detach(int pid) {
	fprintf(stderr, "%s\n", __func__);
	attached_pid = -1;
	return 0;
}

void wilink_mourn(struct process_info *proc) {
	fprintf(stderr, "%s\n", __func__);
}

void wilink_join(int pid) {
	fprintf(stderr, "%s\n", __func__);
}

int wilink_thread_alive(ptid_t pid) {
	fprintf(stderr, "%s\n", __func__);
	return attached_pid == pid.pid;
}

void wilink_resume(struct thread_resume *resume_info, size_t n) {
	fprintf(stderr, "%s\n", __func__);
}

ptid_t wilink_wait(ptid_t ptid, struct target_waitstatus *status, int options) {
	ptid_t child;

	fprintf(stderr, "%s ptid=(%d,%ld,%ld) options=%d\n",
			__func__, ptid.pid, ptid.lwp, ptid.tid, options);

	//tbd really stop FW

	/* Architecture-specific setup after inferior is running.
	     This needs to happen after we have attached to the inferior
	     and it is stopped for the first time, but before we access
	     any inferior registers.  */
	the_low_target.arch_setup();

	child = ptid;
	status->kind = TARGET_WAITKIND_STOPPED;
	status->value.sig = 30;
	return child;
}

void wilink_fetch_registers(struct regcache *regcache, int regno) {
	int regfirst, regnum, i;
	const char dummy_reg[] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07};

	fprintf(stderr, "%s regno=%d\n", __func__, regno);

	if (regno==-1) {
		// if regno==-1, do for all registers
		regfirst = 0;
		regnum = the_low_target.num_regs;
	} else {
		// otherwise, do just for regno
		regfirst = regno;
		regnum = 1;
	}

	for (i=regfirst; i<regfirst+regnum; i++) {
		supply_register(regcache, i, &dummy_reg);
	}
}

void wilink_store_registers(struct regcache *regcache, int regno) {
	fprintf(stderr, "%s\n", __func__);
}

int wilink_prepare_to_access_memory(void) {
	return 0;
}

void wilink_done_accessing_memory(void) {
}

int wilink_read_memory(CORE_ADDR memaddr, unsigned char *myaddr, int len) {
	fprintf(stderr, "%s\n", __func__);

	//todo
	memset(myaddr,0,len);
	return 0;
}

int wilink_write_memory(CORE_ADDR memaddr, const unsigned char *myaddr, int len) {
	fprintf(stderr, "%s\n", __func__);

	//todo
	return 0;
}

void wilink_request_interrupt(void) {
}

static struct target_ops wilink_target_ops = {
  wilink_create_inferior,
  wilink_attach,
  wilink_kill,
  wilink_detach,
  wilink_mourn,
  wilink_join,
  wilink_thread_alive,
  wilink_resume,
  wilink_wait,
  wilink_fetch_registers,
  wilink_store_registers,
  wilink_prepare_to_access_memory,
  wilink_done_accessing_memory,
  wilink_read_memory,
  wilink_write_memory,
  NULL, //wilink_look_up_symbols,
  wilink_request_interrupt,
  NULL, //wilink_read_auxv,
  NULL, //wilink_insert_point,
  NULL, //wilink_remove_point,
  NULL, //wilink_stopped_by_watchpoint,
  NULL, //wilink_stopped_data_address,
  NULL, //read_offsets
  NULL, //get_tls_address
  NULL, //wilink_qxfer_spu,
  NULL, //wilink_hostio_last_error,
  NULL, //wilink_qxfer_osdata,
  NULL, //wilink_xfer_siginfo,
  NULL, //wilink_supports_non_stop,
  NULL, //wilink_async,
  NULL, //wilink_start_non_stop,
  NULL, //wilink_supports_multi_process,
  NULL, //handle_monitor_command
  NULL, //wilink_common_core_of_thread,
  NULL, //wilink_read_loadmap,
  NULL, //wilink_process_qsupported,
  NULL, //wilink_supports_tracepoints,
  NULL, //wilink_read_pc,
  NULL, //wilink_write_pc,
  NULL, //wilink_thread_stopped,
  NULL, //get_tib_address
  NULL, //wilink_pause_all,
  NULL, //wilink_unpause_all,
  NULL, //wilink_cancel_breakpoints,
  NULL, //wilink_stabilize_threads,
  NULL, //wilink_install_fast_tracepoint_jump_pad,
  NULL, //wilink_emit_ops,
  NULL, //wilink_supports_disable_randomization,
  NULL, //wilink_get_min_fast_tracepoint_insn_len,
  NULL, //wilink_qxfer_libraries_svr4,
};

void
initialize_low (void)
{
  set_target_ops (&wilink_target_ops);
  set_breakpoint_data (the_low_target.breakpoint,
		       the_low_target.breakpoint_len);
}
