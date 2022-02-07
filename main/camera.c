#include "camera.h"

#include "esp_camera.h"
#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "sensor.h"

#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

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
            .frame_size     = FRAMESIZE_XGA,
            .jpeg_quality   = 10,
            .fb_count       = 2,
            .grab_mode      = CAMERA_GRAB_LATEST,
    };

    ESP_ERROR_CHECK(esp_camera_init(&cfg));
    ESP_LOGI(TAG, "Camera started");
}

static esp_err_t still_get_handler(httpd_req_t *req) {
    camera_fb_t *fb = NULL;
    esp_err_t res = ESP_OK;

    fb = esp_camera_fb_get();
    if (!fb) {
        ESP_LOGE(TAG, "Failed to get capture frame buffer");
        res = ESP_FAIL;
    }

    if (fb -> format != PIXFORMAT_JPEG) {
        ESP_LOGE(TAG, "Incompatible capture format");
        res = ESP_FAIL;
    }

    if (res == ESP_OK)
        res = httpd_resp_set_type(req, "image/jpeg");

    if (res == ESP_OK)
        res = httpd_resp_set_hdr(req, "Content-Disposition", 
                                    "inline; filename=capture.jpg");

    if (res == ESP_OK) 
        res = httpd_resp_send(req, (const char*) fb -> buf, fb -> len);

    if (res != ESP_OK) {
        ESP_LOGE(TAG, "Failed sending capture");
        httpd_resp_send_500(req);
    }

    esp_camera_fb_return(fb);
    return res;
}

static esp_err_t stream_get_handler(httpd_req_t *req) {
    camera_fb_t *fb = NULL;
    char part_buf[64];
    esp_err_t res = ESP_OK;

    if (httpd_resp_set_type(req, _STREAM_CONTENT_TYPE)) {
        ESP_LOGE(TAG, "Failed to set stream content type");
        return ESP_FAIL;
    }

    while (1) {
        fb = esp_camera_fb_get();
        if (!fb) {
            ESP_LOGE(TAG, "Failed to get capture frame buffer");
            res = ESP_FAIL;
            break;
        }

        if (fb -> format != PIXFORMAT_JPEG) {
            ESP_LOGE(TAG, "Incompatible capture format");
            res = ESP_FAIL;
        }

        int hlen = snprintf(part_buf, 64, _STREAM_PART, fb -> len);
        if (hlen < 0 || hlen >= 64) {
            ESP_LOGE(TAG, "Failed to generate part header");
            res = ESP_FAIL;
        }

        if (res == ESP_OK)
            res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));


        if (res == ESP_OK)
            res = httpd_resp_send_chunk(req, part_buf, hlen);

        if (res == ESP_OK)
            res = httpd_resp_send_chunk(req, (const char*) fb -> buf, fb -> len);

        esp_camera_fb_return(fb);

        if (res != ESP_OK)
            break;
    }

    return res;
}

void camera_register_still_handler(httpd_handle_t server, const char *uri) {
    const httpd_uri_t capture_uri = {
        .uri        = uri,
        .method     = HTTP_GET,
        .handler    = still_get_handler,
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &capture_uri));
}

void camera_register_stream_handler(httpd_handle_t server, const char *uri) {
    const httpd_uri_t stream_uri = {
        .uri        = uri,
        .method     = HTTP_GET,
        .handler    = stream_get_handler,
    };
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &stream_uri));
}
