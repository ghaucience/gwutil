#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "log.h"
#include "parse.h"
#include "file_event.h"
#include "test.h"
#include "jansson.h"
#include "json_parser.h"
#include "timer.h"
#include "udp.h"


static stTestEnv_t ee;

int		test_init(void *_th, void *_fet, int port) {
	ee.th = _th;
	ee.fet = _fet;

	ee.port = port;
	
	timer_init(&ee.step_timer, test_run);
	lockqueue_init(&ee.eq);
	
	ee.fd = udp_create(ee.port);
	file_event_reg(ee.fet, ee.fd, test_in, NULL, NULL);

	return 0;
}

int		test_step() {
	timer_cancel(ee.th, &ee.step_timer);
	timer_set(ee.th, &ee.step_timer, 10);
	return 0;
}

int		test_push(stEvent_t *e) {
	lockqueue_push(&ee.eq, e);
	test_step();
	return 0;
}

void	test_run(struct timer *timer) {
	stEvent_t *e;

	if (!lockqueue_pop(&ee.eq, (void**)&e)) {
		return;
	}

	if (e == NULL) {
		return;
	}

	/* Deal The Event */
	//state_machine_step(&smApp, e);

	FREE(e);

	test_step();
	return;
}

void	test_in(void *arg, int fd) {
	char buf[1024];	
	int  size = 0;
	char cliip[32];
	int  cliport;

		
	int ret = udp_recv(fd, buf, sizeof(buf), cliip, &cliport, 4, 800);
	if (ret < 0) {
		log_debug("what happend?");
		return;
	}

	if (ret == 0) {
		log_debug("error!");
		return;
	}

	size = ret;
	buf[size] = 0;

	char load[1024];
	int load_len = ret - 4;
	strncpy(load, buf + 3, ret - 4);
	load[load_len] = 0;
	
	log_info("[RECV]:%s", load);

	test_do_cmd(load, cliip, cliport);

	return;
}

///////////////////////////////////////////////////////////////////////////////////
// cmd
void test_do_cmd(char *load, char *cliip, int cliport) {
	json_error_t error;
  json_t *jmsg = json_loads(load, 0, &error);
	if (jmsg == NULL) {
		return;
	}
	
	const char *cmd = json_get_string(jmsg, "cmd");
	
	if (strcmp(cmd, "Query") == 0) {
		json_t *jres = json_object();
		json_object_set_new(jres, "cmd",	json_string("QueryResponse"));
		json_object_set_new(jres, "ret",	json_integer(0));

		json_t *jdata = json_object();
		json_object_set_new(jdata, "mac",			json_string("FE:01:02:03:04:FF"));
		json_object_set_new(jdata, "LanIp",		json_string("192.168.66.1"));
		json_object_set_new(jdata, "WanIp",		json_string("192.168.10.100"));
		json_object_set_new(jdata, "Model",		json_string("DSI0134"));
		json_object_set_new(jdata, "Version", json_string("V3.18.20"));
		json_object_set_new(jres, "data",	jdata);

		char *sres= json_dumps(jres, 0);
    if (sres != NULL) {
			log_debug("Send %s to %s, %d", sres, cliip, cliport);
			test_send_frame(ee.fd, sres, cliip, cliport);
      free(sres);
		}

		json_decref(jres);
	} else if (strcmp(cmd, "QueryWifi") == 0) {
		json_t *jres = json_object();
		json_object_set_new(jres, "cmd",	json_string("QueryWifiResponse"));
		json_object_set_new(jres, "ret",	json_integer(0));

		json_t *jdata = json_object();
		json_object_set_new(jdata, "onoff",			json_integer(0));
		json_object_set_new(jdata, "mode",			json_string("ap"));
		json_object_set_new(jdata, "ssid",			json_string("AuGwWifi"));
		json_object_set_new(jdata, "encryption",json_string("psk2"));
		json_object_set_new(jdata, "key",				json_string("dl123456"));
		json_object_set_new(jres, "data",	jdata);

		char *sres= json_dumps(jres, 0);
    if (sres != NULL) {
			log_debug("Send %s to %s, %d", sres, cliip, cliport);
			test_send_frame(ee.fd, sres, cliip, cliport);
      free(sres);
		}

		json_decref(jres);

	} else if (strcmp(cmd, "QueryWan") == 0) {
		json_t *jres = json_object();
		json_object_set_new(jres, "cmd",	json_string("QueryWanResponse"));
		json_object_set_new(jres, "ret",	json_integer(0));

		json_t *jdata = json_object();
		json_object_set_new(jdata, "proto",			json_string("dhcp"));
		json_object_set_new(jdata, "ip",				json_string("192.168.100.100"));
		json_object_set_new(jdata, "gw",				json_string("192.168.100.1"));
		json_object_set_new(jdata, "mask",			json_string("255.255.255.0"));
		json_object_set_new(jdata, "dns1",			json_string("192.168.100.1"));
		json_object_set_new(jdata, "dns2",			json_string("8.8.8.8"));
		json_object_set_new(jres, "data",	jdata);

		char *sres= json_dumps(jres, 0);
    if (sres != NULL) {
			log_debug("Send %s to %s, %d", sres, cliip, cliport);
			test_send_frame(ee.fd, sres, cliip, cliport);
      free(sres);
		}

		json_decref(jres);

	} else if (strcmp(cmd, "QueryMqttServer") == 0) {
		json_t *jres = json_object();
		json_object_set_new(jres, "cmd",	json_string("QueryMqttServerResponse"));
		json_object_set_new(jres, "ret",	json_integer(0));

		json_t *jdata = json_object();
		json_object_set_new(jdata, "server",			json_string("mqtt.dsaas.cc"));
		json_object_set_new(jdata, "port",				json_integer(3100));
		json_object_set_new(jdata, "heartbeat",		json_integer(20));
		json_object_set_new(jres, "data",	jdata);

		char *sres= json_dumps(jres, 0);
    if (sres != NULL) {
			log_debug("Send %s to %s, %d", sres, cliip, cliport);
			test_send_frame(ee.fd, sres, cliip, cliport);
      free(sres);
		}

		json_decref(jres);
		json_decref(jmsg);
	} else if (strcmp(cmd, "ConfigWifi") == 0) {
		json_t *jres = json_deep_copy(jmsg);
		json_object_del(jres, "cmd");
		json_object_set_new(jres, "cmd", json_string("ConfigWifiResponse"));
		json_object_set_new(jres, "ret",	json_integer(0));
		char *sres= json_dumps(jres, 0);
		if (sres != NULL) {
			log_debug("Send %s to %s, %d", sres, cliip, cliport);
			test_send_frame(ee.fd, sres, cliip, cliport);
			free(sres);
		}
		json_decref(jres);
	} else if (strcmp(cmd, "ConfigWan") == 0) {
		json_t *jres = json_deep_copy(jmsg);
		json_object_del(jres, "cmd");
		json_object_set_new(jres, "cmd", json_string("ConfigWifiResponse"));
		json_object_set_new(jres, "ret",	json_integer(0));
		char *sres= json_dumps(jres, 0);
		if (sres != NULL) {
			log_debug("Send %s to %s, %d", sres, cliip, cliport);
			test_send_frame(ee.fd, sres, cliip, cliport);
			free(sres);
		}
		json_decref(jres);
	} else if (strcmp(cmd, "ConfigMqttServer") == 0) {
		json_t *jres = json_deep_copy(jmsg);
		json_object_del(jres, "cmd");
		json_object_set_new(jres, "cmd", json_string("ConfigWifiResponse"));
		json_object_set_new(jres, "ret",	json_integer(0));
		char *sres= json_dumps(jres, 0);
		if (sres != NULL) {
			log_debug("Send %s to %s, %d", sres, cliip, cliport);
			test_send_frame(ee.fd, sres, cliip, cliport);
			free(sres);
		}
		json_decref(jres);
	} 
 
	json_decref(jmsg);
}

//////////////////////////////////////////////////////////////////////////////////
// util
void test_send_frame(int fd, char *load, char *ip, int port) {
	char buf[512];
	int len = proto_make_frame(load, buf);
	udp_send(fd, buf, len, ip, port, 0, 800);
}

char proto_check_sum(char *buffer, int len) {
  int i = 0;
  char sum = 0;

  for (i = 0; i < len - 1; i++) {
    sum ^= buffer[i]&0xff;
  }
  
  return sum;
}

int proto_make_frame(char *cmd, char *buf) {
  int cmdlen = strlen(cmd);

  buf[0] = (char)0xfe;
  buf[1] = (char)((cmdlen>>8)&0xff);
  buf[2] = (char)((cmdlen>>0)&0xff);
  strcpy(buf + 3, cmd);

  int len = cmdlen + 4;  
  buf[3 + cmdlen] = proto_check_sum(buf, len);

  return len;
}

