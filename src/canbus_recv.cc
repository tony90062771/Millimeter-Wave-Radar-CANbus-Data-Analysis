#include "canbus/canbus_recv.h"

pthread_t thread_0, thread_1;
int sockfd_can0;
struct sockaddr_can addr;
struct ifreq ifr;

void target_test_mod(void){
	static struct can_frame frame;
	frame.can_id = 0x000000C5;
	frame.can_dlc = 8;
	frame.data[0] = 0x00;
	frame.data[1] = 0x11;
	frame.data[2] = 0x22;
	frame.data[3] = 0x33;
	frame.data[4] = 0x44;
	frame.data[5] = 0x55;
	frame.data[6] = 0x66;
	frame.data[7] = 0x77;

	write(sockfd_can0, &frame, sizeof(frame));
	usleep(150000);
	printf ("RADAR Test Mode Open\n");
}
void cancel_test_mod(void){
	struct can_frame frame;
	frame.can_id = 0x000000C6;
	frame.can_dlc = 8;
	frame.data[0] = 0x00;
	frame.data[1] = 0x11;
	frame.data[2] = 0x22;
	frame.data[3] = 0x33;
	frame.data[4] = 0x44;
	frame.data[5] = 0x55;
	frame.data[6] = 0x66;
	frame.data[7] = 0x77;

	write(sockfd_can0, &frame, sizeof(frame));
	usleep(150000);
	printf ("RADAR Test Mode Close\n");
}
void close_sockfd(void){
	close(sockfd_can0);
}
void wait_pthread(void){
	pthread_join(thread_0, NULL);
	pthread_join(thread_1, NULL);
	close_sockfd();
}

void analyze_mmWave(can_frame* frame, thread_data* args){
    const static canid_t mmWave_start_can_id = 0x05900000;  // start can_id, byte[0] isstart header, always 0x73 
    const static canid_t mmWave_end_can_id = 0x05904000;
    const static canid_t can_id_mask = 0x0fffffff;
    const static uint16_t data_mask = 0x00ff;
    const static uint32_t invalid_value = 0x61A8;

    static int struct_count = 0;

    static uint16_t id_can_uint16 = 0x0000;
    static uint16_t lateral_can_uint16 = 0x0000;
    static uint16_t height_can_uint16 = 0x0000;
    static uint16_t distance_can_uint16 = 0x0000;
    static uint16_t relative_can_uint16 = 0x0000;
    static uint16_t absolute_can_uint16 = 0x0000;

    static float lateral_distance = 0.;
    static float height_distance = 0.;
    static float distance = 0.;
    static float relative_v = 0.;
    static float absolute_v = 0.;

     canid_t can_id = frame->can_id & can_id_mask;
    switch (can_id){
	    case 0x5900000:case 0x5900200:case 0x5900400:case 0x5900600:
	    case 0x5900800:case 0x5900A00:case 0x5900C00:case 0x5900E00:
	    case 0x5901000:case 0x5901200:case 0x5901400:case 0x5901600:
	    case 0x5901800:case 0x5901A00:case 0x5901C00:case 0x5901E00:
	    case 0x5902000:case 0x5902200:case 0x5902400:case 0x5902600:
	    case 0x5902800:case 0x5902A00:case 0x5902C00:case 0x5902E00:
	    case 0x5903000:case 0x5903200:case 0x5903400:case 0x5903600:
	    case 0x5903800:case 0x5903A00:case 0x5903C00:case 0x5903E00:
	    
	
		id_can_uint16 = (frame->data[2] << 8) |  frame->data[1];
		lateral_can_uint16 = (frame->data[4] << 8) |  frame->data[3];
		height_can_uint16 = (frame->data[6] << 8) |  frame->data[5];
		distance_can_uint16 = frame->data[7];
		break;

        case 0x5900100:case 0x5900300:case 0x5900500:case 0x5900700:
	    case 0x5900900:case 0x5900B00:case 0x5900D00:case 0x5900F00: 
		case 0x5901100:case 0x5901300:case 0x5901500:case 0x5901700:
	    case 0x5901900:case 0x5901B00:case 0x5901D00:case 0x5901F00:
	    case 0x5902100:case 0x5902300:case 0x5902500:case 0x5902700:
	    case 0x5902900:case 0x5902B00:case 0x5902D00:case 0x5902F00:
	    case 0x5903100:case 0x5903300:case 0x5903500:case 0x5903700:
	    case 0x5903900:case 0x5903B00:case 0x5903D00:case 0x5903F00:
    	distance_can_uint16 = (frame->data[0] << 8) | distance_can_uint16;
		relative_can_uint16 = (frame->data[2] << 8) |  frame->data[1];
		absolute_can_uint16 = (frame->data[4] << 8) |  frame->data[3];
    		lateral_distance = (lateral_can_uint16-25000)/100.0;
    		height_distance = (height_can_uint16-25000)/100.0;
    		distance = (distance_can_uint16-25000)/100.0;
    		relative_v = (relative_can_uint16-25000)/100.0;
    		absolute_v = (absolute_can_uint16-25000)/100.0;
    		
		args->data[struct_count].id = id_can_uint16;
		args->data[struct_count].lateral = lateral_distance;
		args->data[struct_count].height = height_distance;
		args->data[struct_count].distance = distance;
		args->data[struct_count].relative = relative_v;
		args->data[struct_count].absolute = absolute_v;
		if(id_can_uint16!=0x0)
	    		struct_count++;
		break;
		case 0x5904000:
		args->data_size = struct_count;
		struct_count = 0;
		break;
			
	}
}

void *mmWave_can_receive(void *arg)   // receive
{
    printf("in mmWave pthread\n");
    struct can_frame frame;
    thread_data * data = (thread_data*)arg;

    printf("start pthread while\n");
    while (true){
        ssize_t bytesRead = read(sockfd_can0, &frame, sizeof(frame));
	data->radar_fetch_time = clock();
        if (bytesRead < 0)
        {
            std::cerr<<"\nread error from mmWave_can_receive"<<std::endl;
            break;
        }
        else if (bytesRead == 0)
        {
            std::cerr<<"\nNo more data from mmWave_can_receive"<<std::endl;
            break;
        }
        analyze_mmWave(&frame, data);
    }
    pthread_exit(NULL);
}

void* mmWave_can_show(void * args){
	thread_data* data = (thread_data*) args;
	while(true){
		if ((double)(clock() - data->radar_fetch_time) / CLOCKS_PER_SEC > 0.5){
			printf("RADAR not working !!! \r");
			usleep(10000);
			continue;
		}
		int data_size = data->data_size;
		for (int i = 0;i < data_size; i++){
			printf("\nID : %d\n", data->data[i].id);
			printf("Lateral\t\t:%f\n", data->data[i].lateral);
			printf("Height\t\t:%f\n", data->data[i].height);
			printf("Distance\t:%f\n", data->data[i].distance);
			printf("Relative\t:%f\n", data->data[i].relative);
			printf("Absolute\t:%f\n", data->data[i].absolute);
		}
		printf("\nTotal Frame Packet : %d\n", data_size);
		usleep(10000);
		system("clear");
	}
	pthread_exit(NULL);
}

void canbus_recv(void){
    
	// 建立 CAN 套接字
	sockfd_can0 = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (sockfd_can0 < 0) {
	    perror("Error while opening socket");
	    exit(-1);
	}
	
	// 設定 can0 介面
	strcpy(ifr.ifr_name, "can0");
	if (ioctl(sockfd_can0, SIOCGIFINDEX, &ifr) < 0) {
	    perror("Error in ioctl");
	    close(sockfd_can0);
	    exit(-1);
	}
	
	// 綁定套接字到 can0
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
	if (bind(sockfd_can0, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
	    perror("Error in socket bind");
	    close(sockfd_can0);
	    exit(-1);
	}
	//for (int i = 0; i < 10000; i++)
	std::cout << "Listening for CAN messages on can0..." << std::endl;

	static thread_data data;
	memset(&data.data[0], 0x00, sizeof(mmWave_data));
	pthread_create(&thread_0, NULL, mmWave_can_receive, &data); // car to wheel
	pthread_create(&thread_1, NULL, mmWave_can_show, &data); // car to wheel
}
