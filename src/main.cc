#include <iostream>
#include <iomanip> // 引入 iomanip 庫以使用 std::setw 和 std::setfill
#include <cstring>
#include <unistd.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include "canbus/canbus_recv.h"
int main() {
	canbus_recv();
    while (true) {
    }
	wait_pthread();
	
    return 0;
}

