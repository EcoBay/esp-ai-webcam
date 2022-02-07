#include "wifi.h"
#include "server.h"
#include "camera.h"

void connect_callback (httpd_handle_t server) {
    if (!server) return;
    camera_register_still_handler(server, "/capture.jpg");
    camera_register_stream_handler(server, "/");
}

void app_main(void) {
    start_wifi();
    camera_init();

    init_server(connect_callback, NULL);
}
