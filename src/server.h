#pragma once

#ifdef _WIN32
  #include <winsock2.h>
#else
  #include "linux_compat.h"
#endif

#include "config.h"
#include "app_state.h"
#include "ring_buffer.h"
#include "websocket_server.h"

void runHttpServer();