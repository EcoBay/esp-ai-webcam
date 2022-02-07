#include "wifi.h"
#include "server.h"
#include "camera.h"

void app_main(void) {
    start_wifi();
    camera_init();

    init_server(camera_register_still_handler);
}
