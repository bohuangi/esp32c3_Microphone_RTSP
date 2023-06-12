/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "opus.h"

#include <lwip/netdb.h>
#include "lwip/sockets.h"

#include "driver/i2s.h"
#include <sys/time.h>

#include <IAudioSource.h>
#include <AudioStreamer.h>
#include <RTSPServer.h>
#define ESP_WIFI_SSID "dududu"
#define ESP_WIFI_PASS "00000000"
#define ESP_MAXIMUM_RETRY 5
#define TAG "dudu"

static EventGroupHandle_t s_wifi_event_group;

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

#define PORT 1028
#define KEEPALIVE_IDLE 7200
#define KEEPALIVE_INTERVAL 75
#define KEEPALIVE_COUNT 10
#define CONFIG_EXAMPLE_IPV4 1

#define RATE 16000
#define BITS 16
#define CHANNELS 2
#define frame_size (RATE/1000*20)
#define encodedatasize (frame_size*(BITS/8)*CHANNELS)
#define MAX_PACKET_SIZE (640)
#define COUNTERLEN 4000

static int s_retry_num = 0;

void wifi_init_sta(void);
static void event_handler(void* arg,
                          esp_event_base_t event_base,
                          int32_t event_id,
                          void* event_data);
static void tcp_server_task(void* pvParameters);
void i2s_config_proc();
OpusEncoder* encoder_init(opus_int32 sampling_rate,
                          int channels,
                          int application);
typedef struct OpusEncoder OpusEncoder;

class AudioSource : public IAudioSource {

 public:
  AudioSource(){};

  int readBytes(void* dest, int maxBytes) override {

    unsigned char* tx_buffer = (unsigned char*)dest;
	opus_int16 in1[maxBytes]={0};
	size_t BytesRead=0;
	printf("i2s_read before \n");
    ESP_ERROR_CHECK(i2s_read(I2S_NUM_0, (char *)in1, maxBytes, &BytesRead, portMAX_DELAY));
	printf("i2s_read after \n");
    //printf("BytesRead=%d \n",BytesRead);
	
   
	int len_opus=0;
	
	if (enc != NULL) {
		printf("enc != NULL\n");
    }
	printf("opus_encode before \n");
    len_opus = opus_encode(enc,in1, frame_size, tx_buffer, MAX_PACKET_SIZE);
	
	
	printf("opus_encode after \n");
	//printf("len_opus= %d\n",len_opus);
    if (len_opus < 0) {
        printf("failed to encode:%d \n",opus_strerror(len_opus));
    }
	
	
    return len_opus;
	
  }

  void start() { 
	ESP_LOGI(TAG,"start");
	i2s_config_proc();
	ESP_LOGI(TAG, "i2s config end.\n");
	
	enc = encoder_init(RATE,CHANNELS,OPUS_APPLICATION_VOIP);
	if (enc != NULL) {
		ESP_LOGI(TAG,"opus_encoder_create success\n");
    }
  }
  void stop() { 
	vTaskDelay(20/ portTICK_PERIOD_MS);
	ESP_LOGI(TAG,"stop");
	i2s_driver_uninstall(I2S_NUM_0);
	opus_encoder_destroy(enc);
	}

 private:
  OpusEncoder* enc=NULL;
  
  
};


/*

class AudioSource : public IAudioSource {

 public:
  AudioSource(){};

  int readBytes(void* dest, int maxBytes) override {
    int16_t* destSamples = (int16_t*)dest;
    for (int i = 0; i < maxBytes/2; i++) {
      destSamples[i] = testData[index];
      index++;
      if (index >= testDataSamples) index = 0;
    }
    return maxBytes;
  }

  void start() { ESP_LOGI(TAG,"start"); }
  void stop() { ESP_LOGI(TAG,"stop"); }

 private:
  int index = 0;
  static const int testDataSamples = 1024;

  const int16_t testData[testDataSamples] = {
      0,      6398,   12551,  18220,  23188,  27263,  30288,  32146,  32767,
      32127,  30249,  27207,  23117,  18136,  12458,  6300,   -100,   -6497,
      -12644, -18304, -23259, -27318, -30326, -32166, -32767, -32107, -30210,
      -27150, -23045, -18052, -12365, -6201,  201,    6596,   12737,  18387,
      23330,  27374,  30364,  32185,  32767,  32087,  30171,  27094,  22973,
      17968,  12271,  6102,   -301,   -6694,  -12829, -18470, -23400, -27429,
      -30402, -32204, -32766, -32066, -30132, -27037, -22902, -17884, -12178,
      -6003,  402,    6793,   12922,  18553,  23470,  27484,  30439,  32222,
      32764,  32045,  30092,  26980,  22830,  17800,  12085,  5904,   -503,
      -6891,  -13014, -18636, -23541, -27538, -30476, -32240, -32763, -32024,
      -30052, -26923, -22757, -17715, -11991, -5805,  603,    6989,   13106,
      18719,  23610,  27593,  30513,  32258,  32761,  32003,  30012,  26866,
      22685,  17630,  11897,  5706,   -704,   -7088,  -13199, -18801, -23680,
      -27647, -30549, -32275, -32759, -31981, -29971, -26808, -22612, -17546,
      -11804, -5607,  804,    7186,   13291,  18884,  23750,  27701,  30586,
      32293,  32756,  31959,  29930,  26750,  22539,  17461,  11710,  5508,
      -905,   -7284,  -13383, -18966, -23819, -27755, -30622, -32310, -32754,
      -31936, -29889, -26692, -22466, -17375, -11616, -5409,  1006,   7382,
      13474,  19048,  23888,  27808,  30657,  32326,  32750,  31914,  29848,
      26633,  22393,  17290,  11521,  5309,   -1106,  -7480,  -13566, -19130,
      -23956, -27861, -30693, -32343, -32747, -31891, -29806, -26574, -22319,
      -17204, -11427, -5210,  1207,   7578,   13658,  19211,  24025,  27914,
      30728,  32359,  32743,  31867,  29764,  26515,  22245,  17119,  11333,
      5111,   -1307,  -7676,  -13749, -19293, -24093, -27966, -30763, -32374,
      -32739, -31844, -29722, -26456, -22171, -17033, -11238, -5011,  1408,
      7774,   13840,  19374,  24161,  28019,  30797,  32390,  32735,  31820,
      29680,  26397,  22097,  16947,  11144,  4912,   -1508,  -7871,  -13931,
      -19455, -24229, -28071, -30831, -32405, -32730, -31796, -29637, -26337,
      -22023, -16860, -11049, -4812,  1609,   7969,   14022,  19536,  24297,
      28123,  30865,  32419,  32725,  31771,  29594,  26277,  21948,  16774,
      10954,  4713,   -1709,  -8067,  -14113, -19616, -24364, -28174, -30899,
      -32434, -32720, -31747, -29550, -26217, -21873, -16688, -10859, -4613,
      1810,   8164,   14204,  19697,  24432,  28225,  30932,  32448,  32715,
      31721,  29507,  26156,  21798,  16601,  10764,  4513,   -1910,  -8262,
      -14295, -19777, -24498, -28276, -30965, -32462, -32709, -31696, -29463,
      -26095, -21723, -16514, -10669, -4414,  2011,   8359,   14385,  19857,
      24565,  28327,  30998,  32476,  32703,  31670,  29419,  26034,  21647,
      16427,  10574,  4314,   -2111,  -8456,  -14475, -19937, -24632, -28377,
      -31031, -32489, -32696, -31644, -29374, -25973, -21572, -16340, -10479,
      -4214,  2212,   8553,   14566,  20017,  24698,  28428,  31063,  32502,
      32689,  31618,  29330,  25912,  21496,  16253,  10383,  4114,   -2312,
      -8650,  -14656, -20097, -24764, -28478, -31095, -32514, -32682, -31592,
      -29285, -25850, -21420, -16165, -10288, -4015,  2412,   8747,   14746,
      20176,  24830,  28527,  31126,  32527,  32675,  31565,  29239,  25788,
      21344,  16078,  10192,  3915,   -2513,  -8844,  -14835, -20255, -24895,
      -28577, -31157, -32539, -32667, -31538, -29194, -25726, -21267, -15990,
      -10097, -3815,  2613,   8941,   14925,  20334,  24961,  28626,  31189,
      32550,  32659,  31510,  29148,  25663,  21191,  15902,  10001,  3715,
      -2713,  -9038,  -15015, -20413, -25026, -28674, -31219, -32562, -32651,
      -31482, -29102, -25601, -21114, -15814, -9905,  -3615,  2814,   9135,
      15104,  20492,  25090,  28723,  31250,  32573,  32642,  31454,  29055,
      25538,  21037,  15726,  9809,   3515,   -2914,  -9231,  -15193, -20570,
      -25155, -28771, -31280, -32584, -32633, -31426, -29009, -25475, -20959,
      -15637, -9713,  -3415,  3014,   9328,   15282,  20648,  25219,  28819,
      31310,  32594,  32624,  31397,  28962,  25411,  20882,  15549,  9617,
      3315,   -3114,  -9424,  -15371, -20726, -25284, -28867, -31339, -32604,
      -32614, -31368, -28915, -25347, -20804, -15460, -9521,  -3214,  3214,
      9521,   15460,  20804,  25347,  28915,  31368,  32614,  32604,  31339,
      28867,  25284,  20726,  15371,  9424,   3114,   -3315,  -9617,  -15549,
      -20882, -25411, -28962, -31397, -32624, -32594, -31310, -28819, -25219,
      -20648, -15282, -9328,  -3014,  3415,   9713,   15637,  20959,  25475,
      29009,  31426,  32633,  32584,  31280,  28771,  25155,  20570,  15193,
      9231,   2914,   -3515,  -9809,  -15726, -21037, -25538, -29055, -31454,
      -32642, -32573, -31250, -28723, -25090, -20492, -15104, -9135,  -2814,
      3615,   9905,   15814,  21114,  25601,  29102,  31482,  32651,  32562,
      31219,  28674,  25026,  20413,  15015,  9038,   2713,   -3715,  -10001,
      -15902, -21191, -25663, -29148, -31510, -32659, -32550, -31189, -28626,
      -24961, -20334, -14925, -8941,  -2613,  3815,   10097,  15990,  21267,
      25726,  29194,  31538,  32667,  32539,  31157,  28577,  24895,  20255,
      14835,  8844,   2513,   -3915,  -10192, -16078, -21344, -25788, -29239,
      -31565, -32675, -32527, -31126, -28527, -24830, -20176, -14746, -8747,
      -2412,  4015,   10288,  16165,  21420,  25850,  29285,  31592,  32682,
      32514,  31095,  28478,  24764,  20097,  14656,  8650,   2312,   -4114,
      -10383, -16253, -21496, -25912, -29330, -31618, -32689, -32502, -31063,
      -28428, -24698, -20017, -14566, -8553,  -2212,  4214,   10479,  16340,
      21572,  25973,  29374,  31644,  32696,  32489,  31031,  28377,  24632,
      19937,  14475,  8456,   2111,   -4314,  -10574, -16427, -21647, -26034,
      -29419, -31670, -32703, -32476, -30998, -28327, -24565, -19857, -14385,
      -8359,  -2011,  4414,   10669,  16514,  21723,  26095,  29463,  31696,
      32709,  32462,  30965,  28276,  24498,  19777,  14295,  8262,   1910,
      -4513,  -10764, -16601, -21798, -26156, -29507, -31721, -32715, -32448,
      -30932, -28225, -24432, -19697, -14204, -8164,  -1810,  4613,   10859,
      16688,  21873,  26217,  29550,  31747,  32720,  32434,  30899,  28174,
      24364,  19616,  14113,  8067,   1709,   -4713,  -10954, -16774, -21948,
      -26277, -29594, -31771, -32725, -32419, -30865, -28123, -24297, -19536,
      -14022, -7969,  -1609,  4812,   11049,  16860,  22023,  26337,  29637,
      31796,  32730,  32405,  30831,  28071,  24229,  19455,  13931,  7871,
      1508,   -4912,  -11144, -16947, -22097, -26397, -29680, -31820, -32735,
      -32390, -30797, -28019, -24161, -19374, -13840, -7774,  -1408,  5011,
      11238,  17033,  22171,  26456,  29722,  31844,  32739,  32374,  30763,
      27966,  24093,  19293,  13749,  7676,   1307,   -5111,  -11333, -17119,
      -22245, -26515, -29764, -31867, -32743, -32359, -30728, -27914, -24025,
      -19211, -13658, -7578,  -1207,  5210,   11427,  17204,  22319,  26574,
      29806,  31891,  32747,  32343,  30693,  27861,  23956,  19130,  13566,
      7480,   1106,   -5309,  -11521, -17290, -22393, -26633, -29848, -31914,
      -32750, -32326, -30657, -27808, -23888, -19048, -13474, -7382,  -1006,
      5409,   11616,  17375,  22466,  26692,  29889,  31936,  32754,  32310,
      30622,  27755,  23819,  18966,  13383,  7284,   905,    -5508,  -11710,
      -17461, -22539, -26750, -29930, -31959, -32756, -32293, -30586, -27701,
      -23750, -18884, -13291, -7186,  -804,   5607,   11804,  17546,  22612,
      26808,  29971,  31981,  32759,  32275,  30549,  27647,  23680,  18801,
      13199,  7088,   704,    -5706,  -11897, -17630, -22685, -26866, -30012,
      -32003, -32761, -32258, -30513, -27593, -23610, -18719, -13106, -6989,
      -603,   5805,   11991,  17715,  22757,  26923,  30052,  32024,  32763,
      32240,  30476,  27538,  23541,  18636,  13014,  6891,   503,    -5904,
      -12085, -17800, -22830, -26980, -30092, -32045, -32764, -32222, -30439,
      -27484, -23470, -18553, -12922, -6793,  -402,   6003,   12178,  17884,
      22902,  27037,  30132,  32066,  32766,  32204,  30402,  27429,  23400,
      18470,  12829,  6694,   301,    -6102,  -12271, -17968, -22973, -27094,
      -30171, -32087, -32767, -32185, -30364, -27374, -23330, -18387, -12737,
      -6596,  -201,   6201,   12365,  18052,  23045,  27150,  30210,  32107,
      32767,  32166,  30326,  27318,  23259,  18304,  12644,  6497,   100,
      -6300,  -12458, -18136, -23117, -27207, -30249, -32127, -32767, -32146,
      -30288, -27263, -23188, -18220, -12551, -6398,  0,
  };
};

*/

extern "C" void app_main(void) {
	
	
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    // wifi_connect();

    /* Print chip information */
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), WiFi%s%s, ", CONFIG_IDF_TARGET,
           chip_info.cores, (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    if (esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        printf("Get flash size failed");
        return;
    }

    wifi_init_sta();

    printf("%uMB %s flash\n", flash_size / (1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded"
                                                         : "external");

    printf("Minimum free heap size: %d bytes\n",
           esp_get_minimum_free_heap_size());

#ifdef CONFIG_EXAMPLE_IPV4
 
	xTaskCreate(tcp_server_task, "tcp_server", 100000, (void*)AF_INET, 5, NULL);

		
#endif
#ifdef CONFIG_EXAMPLE_IPV6

	xTaskCreate(tcp_server_task, "tcp_server", 100000, (void*)AF_INET6, 5, NULL);

#endif

}

void wifi_init_sta(void) {
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta =
            {
                .ssid = ESP_WIFI_SSID,
                .password = ESP_WIFI_PASS,
                /* Authmode threshold resets to WPA2 as default if password
                 * matches WPA2 standards (pasword len => 8). If you want to
                 * connect the device to deprecated WEP/WPA networks, Please set
                 * the threshold value to WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK and
                 * set the password with length and format matching to
                 * WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK standards.
                 */
                .threshold={
					.authmode = WIFI_AUTH_WPA2_PSK
					},
                .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
            },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT)
     * or connection failed for the maximum number of re-tries (WIFI_FAIL_BIT).
     * The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE, pdFALSE, portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we
     * can test which event actually happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s", ESP_WIFI_SSID,
                 ESP_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 ESP_WIFI_SSID, ESP_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
}

static void event_handler(void* arg,
                          esp_event_base_t event_base,
                          int32_t event_id,
                          void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT &&
               event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}
static void tcp_server_task(void* pvParameters) {

	ESP_LOGI(TAG, "tcp_server_task create success");
	

	int port = 1028;
	AudioSource testSource = AudioSource();
	AudioStreamer streamer = AudioStreamer(&testSource);
	RTSPServer rtsp(&streamer, port,0);
	rtsp.runAsync();
   

	vTaskDelete( NULL );

    }






void i2s_config_proc() {
    // i2s config for writing both channels of I2S
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,//I2S_CHANNEL_FMT_ONLY_RIGHT,
        .communication_format =I2S_COMM_FORMAT_STAND_I2S, 
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 3,
        .dma_buf_len = 1024,
        .use_apll = 0,
        .tx_desc_auto_clear = 0,
        //.fixed_mclk = 4096000
    };
    // i2s pinout
    static const i2s_pin_config_t pin_config = {
        .bck_io_num = 3,
        .ws_io_num = 4,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = 8
    };


    // install and start i2s driver
    ESP_ERROR_CHECK(i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL));
    ESP_ERROR_CHECK(i2s_set_pin(I2S_NUM_0, &pin_config));
    //ESP_ERROR_CHECK(i2s_set_clk(I2S_NUM_0, RATE, I2S_BITS_PER_SAMPLE_16BIT,I2S_CHANNEL_STEREO));// I2S_CHANNEL_MONO));
    // enable the DAC channels
    // i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);
    // clear the DMA buffers
    ESP_ERROR_CHECK(i2s_zero_dma_buffer(I2S_NUM_0));

    ESP_ERROR_CHECK(i2s_start(I2S_NUM_0));
}

OpusEncoder* encoder_init(opus_int32 sampling_rate,
                          int channels,
                          int application) {
    int enc_err;
    printf("Here the rate is %ld \n", sampling_rate);
    OpusEncoder* enc =
        opus_encoder_create(sampling_rate, channels, application, &enc_err);
	if(enc_err==OPUS_ALLOC_FAIL){printf("OPUS_ALLOC_FAIL error\n");}
	else if(enc_err==OPUS_BAD_ARG){printf("OPUS_BAD_ARG error\n");}
	else if(enc_err==OPUS_BUFFER_TOO_SMALL){printf("OPUS_BUFFER_TOO_SMALL error\n");}
	else if(enc_err==OPUS_INTERNAL_ERROR){printf("OPUS_INTERNAL_ERROR error\n");}
	else if(enc_err==OPUS_INVALID_PACKET){printf("OPUS_INVALID_PACKET error\n");}	
	else if(enc_err==OPUS_INVALID_STATE){printf("OPUS_INVALID_STATE error\n");}	
	else if(enc_err==OPUS_UNIMPLEMENTED){printf("OPUS_UNIMPLEMENTED error\n");}	
	
    if (enc_err != OPUS_OK) {
		printf("opus_encoder_create error\n");
        fprintf(stderr, "Cannot create encoder: %s\n", opus_strerror(enc_err));
        return NULL;
    }
	

    int bitrate_bps = OPUS_AUTO;//sampling_rate*channels*BITS;
    int bandwidth = OPUS_BANDWIDTH_WIDEBAND;
    int use_vbr = 1;
        int cvbr = 0;
    int complexity = 1;
    int use_inbandfec = 1;
    int forcechannels = 2;
    int use_dtx = 1;
    int packet_loss_perc = 0;

    opus_encoder_ctl(enc, OPUS_SET_BITRATE(bitrate_bps));
    /*
    opus_int32 a=0;
    opus_encoder_ctl(enc,OPUS_GET_BITRATE(&a));
    std::cout<<"complexity="<<a<<std::endl; */  
    opus_encoder_ctl(enc, OPUS_SET_BANDWIDTH(bandwidth));
    opus_encoder_ctl(enc, OPUS_SET_MAX_BANDWIDTH(bandwidth));
    opus_encoder_ctl(enc, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE));//
    opus_encoder_ctl(enc, OPUS_SET_VBR(use_vbr));       //使用动态比特率
    opus_encoder_ctl(enc, OPUS_SET_VBR_CONSTRAINT(cvbr));//不启用约束VBR，启用可以降低延迟
        /*
    opus_int32 a=0;
    opus_encoder_ctl(enc,OPUS_GET_COMPLEXITY(&a));
    std::cout<<"complexity="<<a<<std::endl;*/   //获取到的是9
    opus_encoder_ctl(enc, OPUS_SET_COMPLEXITY(complexity)); //复杂度0-10，在 CPU 复杂性和质量/比特率之间进行取舍
    opus_encoder_ctl(enc, OPUS_SET_INBAND_FEC(use_inbandfec));   //不使用前向纠错，只适用于LPC
    opus_encoder_ctl(enc, OPUS_SET_FORCE_CHANNELS(forcechannels));//强制双声道
    opus_encoder_ctl(enc, OPUS_SET_DTX(use_dtx));               //不使用不连续传输 (DTX)，在静音或背景噪音期间降低比特率，主要适用于voip
    opus_encoder_ctl(enc, OPUS_SET_PACKET_LOSS_PERC(packet_loss_perc));//预期丢包，用降低比特率，来防丢包
    //opus_encoder_ctl(OPUS_SET_PREDICTION_DISABLED (0))    //默认启用预测，LPC线性预测？不启用好像每一帧都有帧头，且会降低质量

    // opus_encoder_ctl(enc, OPUS_GET_LOOKAHEAD(&skip));
    opus_encoder_ctl(enc, OPUS_SET_LSB_DEPTH(BITS));//被编码信号的深度，是一个提示，低于该数量的信号包含可忽略的量化或其他噪声，帮助编码器识别静音

    // IMPORTANT TO CONFIGURE DELAY
    int variable_duration = OPUS_FRAMESIZE_20_MS;
    opus_encoder_ctl(enc, OPUS_SET_EXPERT_FRAME_DURATION(variable_duration));//帧时长

    return enc;
}




