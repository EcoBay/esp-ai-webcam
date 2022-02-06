#include "wifi.h"
#include "server.h"

void app_main(void) {
    start_wifi();
    init_server();
}
