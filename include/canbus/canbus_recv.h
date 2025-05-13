#ifndef _CANBUS_RECV_H
#define _CANBUS_RECV_H
#include <vector>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <memory.h>
#include <iostream>
#include <time.h>

typedef struct _mmWave_data{
	uint16_t id;
	float lateral;
	float height;
	float distance;
	float relative;
	float absolute;
}mmWave_data;

typedef struct _thread_data{
	mmWave_data data[32];
	clock_t radar_fetch_time;
	int data_size = 0;
}thread_data;

extern void canbus_recv (void);
extern void wait_pthread(void);
#endif
