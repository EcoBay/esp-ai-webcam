#include "server.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_event.h"
#include <string.h>

static httpd_handle_t server = NULL;

static const char *TAG = "HTTP Server";

static void start_server(void) {
    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();

    ESP_LOGI(TAG, "Starting server on port: '%d'", cfg.server_port);

    if (httpd_start(&server, &cfg)) {
        ESP_LOGE(TAG, "Error starting server!");
        server = NULL;
    }
}

static void stop_server(void) {
    httpd_stop(server);
    server = NULL;
}

static void connect_handler(void* arg, esp_event_base_t event_base, 
        int32_t event_id, void* event_dat) {
    if (server) return;

    ESP_LOGI(TAG, "Starting webserver");
    start_server();

    callback cbk = (callback) arg;
    if (cbk)
        cbk(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base,
        int32_t event_id, void* event_dat) {
    if (!server) return;

    callback cbk = (callback) arg;
    if (cbk)
        cbk(server);

    ESP_LOGI(TAG, "Stopping webserver");
    stop_server();
}

void init_server(callback connect_callback, callback disconnect_callback) {
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                IP_EVENT_STA_GOT_IP,
                &connect_handler,
                (void*) connect_callback,
                NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, 
                WIFI_EVENT_STA_DISCONNECTED, 
                &disconnect_handler, 
                (void*) disconnect_callback,
                NULL));
}
