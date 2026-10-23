#pragma once
#include <thread>
#include <atomic>

void startWebSocketServer(int port = 9006);
void broadcastWebSocketData(const float* data, size_t count);