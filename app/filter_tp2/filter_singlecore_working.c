#include <hellfire.h>
#include "image.h"
#include "filter_tp2.h"





uint8_t gaussian(uint8_t buffer[5][5]){
	int32_t sum = 0, mpixel;
	uint8_t i, j;

	int16_t kernel[5][5] =	{	{2, 4, 5, 4, 2},
					{4, 9, 12, 9, 4},
					{5, 12, 15, 12, 5},
					{4, 9, 12, 9, 4},
					{2, 4, 5, 4, 2}
				};
	for (i = 0; i < 5; i++)
		for (j = 0; j < 5; j++)
			sum += ((int32_t)buffer[i][j] * (int32_t)kernel[i][j]);
	mpixel = (int32_t)(sum / 159);

	return (uint8_t)mpixel;
}


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


void showImg(uint8_t *img, int32_t width, int32_t height) {
    int32_t i, j, k = 0;
    int32_t this_width = width, this_height = height;

    printf("int32_t width = %d, height = %d;\n", this_width, this_height);
		printf("uint8_t image[] = {\n");
		for (i = 0; i < this_height; i++){
			for (j = 0; j < this_width; j++){
				printf("0x%x", img[i * this_width + j]);
				if ((i < this_height-1) || (j < this_width-1)) printf(", ");
				if ((++k % 16) == 0) printf("\n");
			}
		}
	printf("};\n");
}


uint8_t sobel(uint8_t buffer[3][3]){
	int32_t sum = 0, gx = 0, gy = 0;
	uint8_t i, j;

	int16_t kernelx[3][3] =	{	{-1, 0, 1},
					{-2, 0, 2},
					{-1, 0, 1},
				};
	int16_t kernely[3][3] =	{	{-1, -2, -1},
					{0, 0, 0},
					{1, 2, 1},
				};
	for (i = 0; i < 3; i++){
		for (j = 0; j < 3; j++){
			gx += ((int32_t)buffer[i][j] * (int32_t)kernelx[i][j]);
			gy += ((int32_t)buffer[i][j] * (int32_t)kernely[i][j]);
		}
	}
	
	sum = isqrt(gy * gy + gx * gx);

	if (sum > 255) sum = 255;
	if (sum < 0) sum = 0;

	return (uint8_t)sum;
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



void sender(uint8_t id, uint8_t sourcechannel, uint8_t * data) {
	int16_t val;
	

	printf(" Sending data from %d to %d\n", sourcechannel, id );
	delay_ms(50);
	val = hf_send(id, 5000, data, sizeof(commDatagram), sourcechannel);

	if (val)
		printf("hf_send(): error %d\n", val);
	return;
}


void receive(int32_t listenChannel,  uint8_t * buff){

	uint16_t cpu, port, size; //  will be written on
	int16_t val;

    
	val = hf_recv(&cpu, &port, (int8_t *)buff, &size, listenChannel);
    printf(" Receiving data on channel %d  from cpu %d\n", listenChannel, cpu);
	if (val)
		printf("hf_recv(): error %d\n", val);
}




uint8_t buffAux[260*260];

//              INPUT 256*256    OUTPUT 36X36      COLUMN    NEW LINE
void splitGauss(uint8_t *input, uint8_t *output, int32_t l, int32_t k ) {
	
    int32_t i, j;
	int32_t colunm;

    //memset(buffAux, 0, sizeof(buffAux));

    colunm = (l * HEIGHT) > 0 ? (l * HEIGHT) : 0;
    
    for (i = 0; i < 256; i++) {
        for (j = 0; j < 256; j++) {
            buffAux[(((i+2) * 260) + j) + 2] =  input[((i * 256) + j)];
        }    
    }

    for(i = 0; i < 36; i++) {
        for (j = 0; j < 36; j++) {
            output[((i * 36) + j)] = buffAux[(((i+k) * 260) + (j+colunm))];
        }
    }

	
}


void appendBuffer(uint8_t *newMatriz, uint8_t *buff, int32_t l, int32_t k, uint8_t filter) {
    int32_t i = 0, j = 0, colunm = 0;

    colunm = (l * HEIGHT) > 0 ? (l * HEIGHT) : 0;

    switch (filter) {
    case 1:
        for(i = 0; i < 32; i++) {
            for (j = 0; j < 32; j++) {
                newMatriz[(((i+k) * 260) + (j+colunm))] = buff[(((i+2) * 36) + j+2)]; 
            }
        }

        break;
    case 0:
        for(i = 0; i < 32; i++) {
            for (j = 0; j < 32; j++) {
                newMatriz[(((i+k+4) * 266) + (j+colunm+4))] = buff[(((i+4) * 42) + j+4)];
            }
        }

        break;
    }
}


void cutImage(uint8_t *output, uint8_t *newMatriz, uint8_t filter) {
    int32_t i = 0, j = 0;
 
    switch (filter) {
    case 1:
        for (i = 0; i < 256; i++) {
            for (j = 0; j < 256; j++) {  
                output[((i * 256) + j)] = newMatriz[(((i+2) * 260) + j) + 2];
            }    
        }

        break;
    case 0:
        for (i = 0; i < 256; i++) {
            for (j = 0; j < 256; j++) {  
                output[((i * 256) + j)] = newMatriz[(((i+5) * 266) + j) + 5];
            }    
        }

        break;
    }
}

uint8_t bigPicture [260*260];
uint8_t mediumPicture [256*256];

void taskMaster(void){
    
	if(hf_comm_create(hf_selfid(),5000,0))
		panic(0xff);

	printf(" \n MASTER CORE started ... \n");
    int state = 0;

	commDatagram receiveChunk,sendChunk;

	int32_t queuedChannel = 0;
	int32_t channelReceived = 0;
	int32_t columnOffset = 0, lineOffset = 0;
	uint8_t sendCount = 0;
	uint8_t cpuIndex = 1;
	uint8_t sourceCpu = 0;


	// vectors
	//uint8_t splitGaussOut[36*36];

	// initialize vectors
	memset(splitGauss,0,sizeof(splitGauss));

	while(1){
		switch (state){

			case WAIT_FOR_READY:

				memset((uint8_t*)&receiveChunk,0,sizeof(receiveChunk));

				queuedChannel= hf_recvprobe();
				//printf(" after recvprobe %d\n",queuedChannel);

				if(queuedChannel >= 0) {  
					
					receive(queuedChannel,(uint8_t *)&receiveChunk);

					printf(">>>>>>>>>>>>>>>>>> new state: CROP IMAGE\n");
					state = CROP_AND_SEND_IMAGE;
					sourceCpu = receiveChunk.sourceCoreId;
					sendCount++; 
				} 
				// if task N is ready, go to crop image, if not do nothing
				break;

			case CROP_AND_SEND_IMAGE:

				

				splitGauss(image,sendChunk.image,columnOffset, lineOffset);
				printf(" End of splitGauss task \n\n\n\n" ) ;
				
				

				sendChunk.flags = SEND_CROPPED_IMAGE; // SET DATA TYPE
				sendChunk.lineOffset = lineOffset;
				sendChunk.columnOffset = columnOffset;
				sender(sourceCpu, MASTER_CHANNEL, (uint8_t*)&sendChunk);
				printf(" \nlineOffset: %d, columnOffset %d \n\n", lineOffset, columnOffset);

				// calculate new offsets
				lineOffset =   !((columnOffset+1) % 8) && columnOffset > 0 ? lineOffset+32 : lineOffset;
                columnOffset = !((columnOffset+1) % 8) && columnOffset > 0 ? 0 : columnOffset+1;
				
				// after all cores have data to process, go to request data from them
				
				if ( sendCount == 1){ // N_CORES - 1
					state = REQUEST_DATA;
					printf(">>>>>>>>>>>>>>> New state: REQUEST_DATA \n");
					sendCount = 0;
				} else { 
					//state = WAIT_FOR_READY;
					printf(">>>>>>>>>>>>>>> New state: REQUEST_DATA \n");
				}
				break;

			case REQUEST_DATA:
				memset((uint8_t*)&sendChunk,0,sizeof(sendChunk));

				sendChunk.flags = REQUEST_PROCESSED_IMAGE;
				sender(cpuIndex,MASTER_CHANNEL,(uint8_t*)&sendChunk); // need to cycle thru cpu index
				printf(">>>>>>>>>>>>>>> New state: READ_AND_APPEND_DATA \n");
				state = READ_AND_APPEND_DATA;
				break;

			case READ_AND_APPEND_DATA:
				memset((uint8_t*)&receiveChunk,0,sizeof(receiveChunk));

				channelReceived = -208;

				channelReceived = hf_recvprobe();
				//printf(" after recvprobe %d\n",queuedChannel);

				if(channelReceived >= 0 && channelReceived < N_CORES) {  
					
					receive(queuedChannel,(uint8_t *)&receiveChunk);
					printf(" Processing received data from core... %d \n",channelReceived);
				
					if(receiveChunk.flags == SEND_PROCESSED_IMAGE){
						// APPEND
						// IMAGEM ESTA OK ATE AQUI

						printf("Append debug line:%d  column%d\n",receiveChunk.lineOffset,receiveChunk.columnOffset);
						appendBuffer(bigPicture,receiveChunk.image,receiveChunk.columnOffset,receiveChunk.lineOffset,1);
						

						//showImg(mediumPicture,256,256);
					

						if (lineOffset == 256){
							state = STITCH_IMAGE;
							printf(" END OF COMM \n\n>>>>>>>>>>> New state: stitch \n\n");
						} else {
							state = WAIT_FOR_READY;
							printf(" END OF COMM \n\n>>>>>>>>>>> New state: WAIT FOR READY \n");
						}
					}

				} 

				
				// hf_recv to receive data from core.
				break;
			case STITCH_IMAGE:
				cutImage(mediumPicture,bigPicture,1);
				showImg(mediumPicture,256,256);
				while(1);
				// final state, stitch all cropped images together.
				break;
			
			
			
		}
	}



	while(1);

}




void task1(void){

    

	if(hf_comm_create(hf_selfid(),5000,0))
		panic(0xff);

	printf("\n CORE 1 STARTED ... \n");

	
	commDatagram receiveChunk, sendChunk;
	sendChunk.sourceCoreId = 1;
	int32_t queuedChannel = 0;
	uint8_t state = 0;
	uint8_t processedChunk[36*36];
	uint16_t lineOffset =0, columnOffset = 0;

	memset((uint8_t*)&sendChunk,0,sizeof(sendChunk));

	while(1){
    switch (state){

        case SEND_READY:
			sendChunk.flags = CORE_READY;
			sendChunk.sourceCoreId = 1;
            sender(MASTER_ID,TASK1_CHANNEL,(uint8_t*)&sendChunk);
			printf(">>>>>>> New state: RECEIVE CHUNK \n");
			state = RECEIVE_CHUNK;
            break;
        case RECEIVE_CHUNK:

			queuedChannel= hf_recvprobe();
			
			if(queuedChannel == MASTER_CHANNEL) {  
				
				receive(queuedChannel, (uint8_t *)&receiveChunk);
				if(receiveChunk.flags = SEND_CROPPED_IMAGE ){
					lineOffset = receiveChunk.lineOffset;
					columnOffset = receiveChunk.columnOffset;
					printf(">>>>>>> New state: PROCESS CHUNK \n");
					state = PROCESS_CHUNK;
				}
				
				
			} 
            // receive chunk, if nothing is received, timeout to send_ready again.
            break;
        case PROCESS_CHUNK:
			memset((uint8_t*)&processedChunk,0,sizeof(processedChunk));

			do_gaussian(receiveChunk.image,processedChunk,36,36);
			printf(" Gaussian complete \n");

			printf(">>>>>>> New state: WAIT CMD \n");
			state = WAIT_CMD;
            // FILTER CHUNK
            break;
        case WAIT_CMD:
			memset((uint8_t*)&receiveChunk,0,sizeof(receiveChunk));
			queuedChannel = -1;
			queuedChannel= hf_recvprobe();
			
			if(queuedChannel == MASTER_CHANNEL) {  
				
				receive(queuedChannel, (uint8_t *)&receiveChunk);
				if (receiveChunk.flags == REQUEST_PROCESSED_IMAGE){
					printf(" Master asks for processed image.. \n");
					printf(">>>>>>> New state: SEND_PROCESSED IMAGE \n");
					state = SEND_PROCESSED;
				}
				
				
			} 
            // wait for request command from master
            break;
        case SEND_PROCESSED:
			delay_ms(50);
			sendChunk.flags = SEND_PROCESSED_IMAGE;
			memcpy(sendChunk.image,processedChunk,sizeof(processedChunk));
			sendChunk.columnOffset = columnOffset;
			sendChunk.lineOffset = lineOffset;
			sendChunk.sourceCoreId = 1;
			
			sender(MASTER_ID,TASK1_CHANNEL,(uint8_t*)&sendChunk);

			printf(" Processing ended... \n ");
            printf(" >>>>>>>>> New state : SEND READY \n \n  CORE RESTART \n");
			state = SEND_READY;
			break;
		case 8:
			break;

    }
	}


	
    
}
void task2(void){

	

	while(1);
}
void task3(void){





	while(1);
}
void task4(void){


    
	while(1);
}



void app_main(void) {

    if (hf_cpuid() == 0)
	    hf_spawn(taskMaster, 0, 0, 0, "coreMaster",  100000);
    if (hf_cpuid() == 1)
	    hf_spawn(task1, 0, 0, 0, "core1", 100000);
    if (hf_cpuid() == 2)
	    hf_spawn(task2, 0, 0, 0, "core2", 100000);
    if (hf_cpuid() == 3)
	    hf_spawn(task3, 0, 0, 0, "core3", 100000);
    if (hf_cpuid() == 4)
	    hf_spawn(task4, 0, 0, 0, "core4", 100000);
    
}
