#include <string.h>
#include <fstream>
#include <iostream>
#include <ao/ao.h>

#include <opus/opus.h>
#include <sys/time.h>
#include<arpa/inet.h>
#include <unistd.h>

#define MAX_PACKET 1500
#define MAX_FRAME_SIZE 6 * 960
#define MAX_PACKET_SIZE (3840000)
#define COUNTERLEN 4000

#define RATE 16000
#define BITS 16
#define CHANNELS 2
#define FRAMESIZE (RATE * CHANNELS * 2 * FRAMELEN / 1000)

ao_device* ao_driver_init(int rate);

int main() {





    // 这里简单计算，其实就是2.5ms的framesize对应多少采样点
    // must be multiple times of bits
    // 2.5MS -> 48000 samples per second / 1000 MS per second * 2 Channels *
    // 2bytes per sample * 2.5MS = 480 bytes per 2.5MS 2.5MS 120 per channel
    // samples, 240 per channel size, 240 samples per 2.5MS
    
    int max_payload_bytes = MAX_PACKET;
    int len_opus[COUNTERLEN] = {0};
    // int frame_duration_ms = 2.5;
    int frame_size = RATE / 1000*20;

    //1.创建通信的套接字
    int fd =socket(AF_INET,SOCK_STREAM,0);
    if(fd ==-1){
        perror("socket error");
        return -1;
    }
    
	
//2.连接服务器的IP port
    struct sockaddr_in saddr;
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(1028);
    inet_pton(AF_INET,"192.168.252.14",&saddr.sin_addr.s_addr);
    printf("connect before\n");
    int ret =connect(fd,(struct sockaddr*)&saddr,sizeof(saddr));
    printf("connect after\n");
    if(ret == -1){
        perror("connect error");
        return -1;
    }
    else if(ret == 0){
        perror("connect success");

    }
/*
    ao_device* player = ao_driver_init(RATE);
    if (player == NULL) {
        fprintf(stderr, "Error opening device.\n");
        return -1;
    }
  */  
    int err;	
    OpusDecoder* decoder = opus_decoder_create(RATE, CHANNELS, &err);
     if (err < 0) {
         fprintf(stderr, "failed to create decoder: %s\n", opus_strerror(err));
     }

    timeval start, end,start1,end1;
    timeval start4, end4;
    unsigned char rx_buffer[640];
    char tx_buff[20]="ok";
	int count=0;
	int len;
    int done;
    opus_int16 out1[1028];
    int decodeSamples;
	sleep(3);
    FILE *fp;
    fp = fopen("data.pcm", "wb"); // 打开文件 

    for(;;){
        gettimeofday(&start1, NULL);
        int sendlen =send(fd,tx_buff,20,0);
        gettimeofday(&end1, NULL);
        std::cout << "send is" << (end1.tv_usec - start1.tv_usec)<< "us" <<std::endl; 
        if(sendlen<0){
            perror("send failed");
            break;
        } 
        else{
            printf("recving \n");
            gettimeofday(&start1, NULL);
            len=recv(fd,rx_buffer,sizeof(rx_buffer),0);
            gettimeofday(&end1, NULL);
            std::cout << "       recv is" << (end1.tv_usec - start1.tv_usec)
                  << "us" << std::endl;
            if (len>0){
                std::cout << "recvlen = "<<len<<std::endl;
                count+=1;
                std::cout << "count= "<<count<<std::endl;
            //usleep(8000);
            }
            else if(len==0){
                perror("Connection closed");
                break;
            }
            else{
                perror("receiving: errno");
                break;
            }
        }
		gettimeofday(&start1, NULL);
        decodeSamples =opus_decode(decoder, rx_buffer, len, out1, frame_size, 0);
		gettimeofday(&end1, NULL);
		std::cout << "opus_decode " << (end1.tv_usec - start1.tv_usec)<< "us" << std::endl
            <<"decodeSamples="<<decodeSamples<<std::endl;
        if (decodeSamples < 0) {
            std::cout << "failed to decode"<< std::endl;
        }
        //ao_play(player, (char*)out1, decodeSamples*3);
        //fseek(fp, 0, SEEK_END); // 将文件指针移动到文件末尾
        int fwritelen=fwrite(out1, sizeof(char), decodeSamples*2*2, fp); // 写入数据
        if(fwritelen<len)printf("fwrite error\n");
        else printf("fwritelen = %d\n",fwritelen);
		gettimeofday(&end, NULL);
		std::cout << "        one   frame " << (end.tv_usec - start.tv_usec)<< "us" << std::endl;           

    }
    fclose(fp); // 关闭文件
    //ao_close(player);
    return 0;
}

ao_device* ao_driver_init(int rate) {
    ao_device* device;
    ao_sample_format format;
    int default_driver;

    ao_initialize();

    /* -- Setup for default driver -- */
    default_driver = ao_default_driver_id();
    std::cout<<"default_driver="<<default_driver<<std::endl;

    memset(&format, 0, sizeof(format));
    format.bits = BITS;
    format.channels = 1;
    format.rate = rate;
    std::cout << "ao rate is " << rate << std::endl;
    format.byte_format = AO_FMT_LITTLE;
    //format.matrix="L,R";
    device = ao_open_live(default_driver, &format, NULL /* no options */);

    return device;
}

