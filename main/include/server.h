#ifndef SERVER_H
#define SERVER_H

#include "esp_http_server.h"

typedef void (*callback) (httpd_handle_t server);
void init_server(callback cbk);

#endif
