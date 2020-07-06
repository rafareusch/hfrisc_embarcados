#include <hellfire.h>
#include "image.h"
//#include <noc.h>

void do_gaussian(uint8_t *input, uint8_t *output, int32_t width, int32_t height){
	int32_t i = 0, j = 0, k, l;
	uint8_t image_buf[5][5];
	
	for(i = 0; i < height; i++){
		if (i > 1 && i < height-2){
			for(j = 0; j < width; j++){
				if (j > 1 && j < width-2){
					for (k = 0; k < 5; k++)
						for(l = 0; l < 5; l++)
							image_buf[k][l] = input[(((i + l-2) * width) + (j + k-2))];

					output[((i * width) + j)] = gaussian(image_buf);
				}else{
					output[((i * width) + j)] = input[((i * width) + j)];
				}
			}
		}else{
			output[((i * width) + j)] = input[((i * width) + j)];
		}
	}
}

void do_sobel(uint8_t *input, uint8_t *output, int32_t width, int32_t height){
	int32_t i = 0, j = 0, k, l;
	uint8_t image_buf[3][3];
	
	for(i = 0; i < height; i++){
		if (i > 2 && i < height-3){
			for(j = 0; j < width-1; j++){
				if (j > 2 && j < width-3){
					for (k = 0; k < 3; k++)
						for(l = 0; l < 3; l++)
							image_buf[k][l] = input[(((i + l-1) * width) + (j + k-1))];

					output[((i * width) + j)] = sobel(image_buf);
				}else{
					output[((i * width) + j)] = 0;
				}
			}
		}else{
			output[((i * width) + j)] = 0;
		}
	}
}


#define WAIT_FOR_READY   0
#define SEND_DATA        1
#define CROP_IMAGE       2
#define STITCH_IMAGE     3
#define REQUEST_DATA     4
#define READ_DATA        5


void taskMaster(void){

    int state = 0;

    switch (state){
        case WAIT_FOR_READY:
            // if task N is ready, go to crop image, if not do nothing
            break;
        case CROP_IMAGE:
            // crop image from 256 in a small block of 36x36 in which 32x32 is valid data
            break;
        case SEND_DATA:
            // send cropped image to recpective core if theres still data to be sent, 
            break;
        case REQUEST_DATA:
            // send request package
            break;
        case READ_DATA:
            // hf_recv to receive data from core.
            break;
        case STITCH_IMAGE:
            // final state, stitch all cropped images together.
            break;
        case 
        
        
    }

}

#define SEND_READY      0
#define RECEIVE_CHUNK   1
#define PROCESS_CHUNK   2
#define WAIT_CMD        3
#define SEND_PROCESSED  4 

void task1(void){


    
}
void task2(void){

	


    
}
void task3(void){

	if(hf_comm_create(hf_selfid(),5000,0))
		panic(0xff);

	int state = 0;
    switch (state){

        case SEND_READY:
            /// send ready to master and go to receive
            break;
        case RECEIVE_CHUNK:
            // receive chunk, if nothing is received, timeout to send_ready again.
            break;
        case PROCESS_CHUNK:
            // FILTER CHUNK
            break;
        case WAIT_CMD:
            // wait for request command from master
            break;
        case SEND_PROCESSED:
            break;
    }
    
}
void task4(void){


    
}



void app_main(void) {

    if (hf_cpuid() == 0)
	    hf_spawn(taskMaster, 0, 0, 0, "coreMAster",4096);
    if (hf_cpuid() == 1)
	    hf_spawn(task1, 0, 0, 0, "core1", 4096);
    if (hf_cpuid() == 2)
	    hf_spawn(task2, 0, 0, 0, "core2", 4096);
    if (hf_cpuid() == 3)
	    hf_spawn(task3, 0, 0, 0, "core3", 4096);
    if (hf_cpuid() == 4)
	    hf_spawn(task4, 0, 0, 0, "core4", 4096);
    
}
