#include "camera.h"

#include "esp_camera.h"
#include "esp_err.h"
#include "esp_log.h"
#include "sensor.h"

static const char* TAG = "Camera";

void camera_init(void) {
    const camera_config_t cfg = {
            .pin_pwdn       = PWDN_GPIO_NUM,
            .pin_reset      = RESET_GPIO_NUM,
            .pin_xclk       = XCLK_GPIO_NUM,
            .pin_sscb_sda   = SIOD_GPIO_NUM,
            .pin_sscb_scl   = SIOC_GPIO_NUM,

            .pin_d7     = D7_GPIO_NUM,
            .pin_d6     = D6_GPIO_NUM,
            .pin_d5     = D5_GPIO_NUM,
            .pin_d4     = D4_GPIO_NUM,
            .pin_d3     = D3_GPIO_NUM,
            .pin_d2     = D2_GPIO_NUM,
            .pin_d1     = D1_GPIO_NUM,
            .pin_d0     = D0_GPIO_NUM,
            .pin_vsync  = VSYNC_GPIO_NUM,
            .pin_href   = HREF_GPIO_NUM,
            .pin_pclk   = PCLK_GPIO_NUM,

            .xclk_freq_hz   = 16500000,
            .ledc_timer     = LEDC_TIMER_0,
            .ledc_channel   = LEDC_CHANNEL_0,

            .pixel_format   = PIXFORMAT_JPEG,
            .frame_size     = FRAMESIZE_SVGA,
            .jpeg_quality   = 12,
            .fb_count       = 1,
            .grab_mode      = CAMERA_GRAB_LATEST,
    };

    ESP_ERROR_CHECK(esp_camera_init(&cfg));
    ESP_LOGI(TAG, "Camera started");
}

#define RESP_FAIL(msg) {        \
    ESP_LOGE(TAG, msg);         \
    httpd_resp_send_500(req);   \
    return ESP_FAIL;            \
}

static esp_err_t still_get_handler(httpd_req_t *req) {
    camera_fb_t *fb = NULL;

    if (!(fb = esp_camera_fb_get()))
        RESP_FAIL("Image capture failed");

    if (httpd_resp_set_type(req, "image/jpeg"))
        RESP_FAIL("Failed to set 'Content-typeimage/jpeg'");

    if (httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg"))
        RESP_FAIL("Failed to set 'Content-Disposition'");

    if (fb -> format != PIXFORMAT_JPEG)
        RESP_FAIL("Incompatible capture format");

    if (httpd_resp_send(req, (const char*) fb -> buf, fb -> len))
        RESP_FAIL("Failed to send response");

    esp_camera_fb_return(fb);
    return ESP_OK;
}


void camera_register_still_handler(httpd_handle_t server, const char *uri) {
    const httpd_uri_t capture_uri = {
        .uri        = uri,
        .method     = HTTP_GET,
        .handler    = still_get_handler,
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &capture_uri));
}
