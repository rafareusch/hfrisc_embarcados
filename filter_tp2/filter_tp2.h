//#include <noc.h>

//#include <hellfire.h>


// IMAGE DEFINES
#define splitPacketGauss          36*36
#define splitPacketSobel		  42*42

// DEFINES TO FLAG PACKET
#define REQUEST_PROCESSED_IMAGE   40
#define CORE_READY                2
#define SEND_CROPPED_IMAGE        35
#define SEND_PROCESSED_IMAGE      4


#define SEND_READY      0
#define RECEIVE_CHUNK   1
#define PROCESS_CHUNK   2
#define WAIT_CMD        3
#define SEND_PROCESSED  4 


#define WAIT_FOR_READY           0
#define SEND_DATA                1
#define CROP_AND_SEND_IMAGE      2
#define STITCH_IMAGE             3
#define REQUEST_DATA             4
#define READ_AND_APPEND_DATA     5
#define N_CORES			     	 5



#define MASTER_ID      0
#define TASK1_ID      1
#define TASK2_ID      2
#define TASK3_ID      3
#define TASK4_ID      4

#define HEIGHT 32
#define WIDTH  32


#define MASTER_CHANNEL 0
#define TASK1_CHANNEL  1
#define TASK2_CHANNEL  2
#define TASK3_CHANNEL  3
#define TASK4_CHANNEL  4

#define TIMEOUT_READY  1000000000000000

typedef struct datagram {
	uint16_t sourceCoreId;
	uint16_t flags;
	uint8_t image[splitPacketSobel];
	int16_t lineOffset;
	int16_t columnOffset;
	uint8_t runType;
} commDatagram;

#define DO_GAUSS 1
#define DO_SOBEL 0

uint8_t gaussian(uint8_t buffer[5][5]);
void do_gaussian(uint8_t *input, uint8_t *output, int32_t width, int32_t height);
void do_sobel(uint8_t *input, uint8_t *output, int32_t width, int32_t height);
void sender(uint8_t id, uint8_t sourcechannel, int8_t * data);
void receive(uint16_t listenChannel,  uint8_t * buff);
void splitGauss(uint8_t *input, uint8_t *output, int32_t l, int32_t k );
void appendBuffer(uint8_t *newMatriz, uint8_t *buff, int32_t l, int32_t k, uint8_t type);
void taskMaster(void);
void task1(void);
void task2(void);
void task3(void);
void task4(void);
void app_main(void);
uint8_t sobel(uint8_t buffer[3][3]);
void showImg(uint8_t *img, int32_t width, int32_t height);
void cutImage(uint8_t *output, uint8_t *newMatriz, uint8_t filter);
