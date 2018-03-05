#ifndef __TEST_H_
#define __TEST_H_
    
#include "timer.h"
#include "file_event.h"
#include "lockqueue.h"


#ifdef __cplusplus 
extern "C" {
#endif


typedef struct stTestEnv {
	struct file_event_table *fet;
	struct timer_head *th;

	struct timer step_timer;
	stLockQueue_t eq;

	int port;
	int fd;
}stTestEnv_t;


int		test_init(void *_th, void *_fet, int port);
int		test_step();
int		test_push(stEvent_t *e);
void	test_run(struct timer *timer);
void	test_in(void *arg, int fd);


int proto_make_frame(char *cmd, char *buf);
void test_do_cmd(char *load, char *cliip, int cliport);
char proto_check_sum(char *buffer, int len);
void test_send_frame(int fd, char *load, char *ip, int port);

#ifdef __cplusplus
}
#endif



#endif
