#ifndef CAMERA_H
#define CAMERA_H

#include "esp_err.h"
#include "esp_http_server.h"

// AI Thinkerer ESP32-CAM
#define PWDN_GPIO_NUM  32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM  0
#define SIOD_GPIO_NUM  26
#define SIOC_GPIO_NUM  27

#define D7_GPIO_NUM    35
#define D6_GPIO_NUM    34
#define D5_GPIO_NUM    39
#define D4_GPIO_NUM    36
#define D3_GPIO_NUM    21
#define D2_GPIO_NUM    19
#define D1_GPIO_NUM    18
#define D0_GPIO_NUM    5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM  23
#define PCLK_GPIO_NUM  22

void camera_init(void);

void camera_register_still_handler(httpd_handle_t server);
void camera_register_stream_handler(httpd_handle_t server);

#endif
