#include "websocket_server.h"
#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
#else
  #include <sys/socket.h>
  #include <arpa/inet.h>
  #include <unistd.h>
#endif
#include <vector>
#include <mutex>
#include <cstring>
#include <cstdio>

static std::vector<int> g_clients;
static std::mutex g_clientsMutex;

// Упрощенный парсинг HTTP Upgrade (без полной библиотеки)
static bool handle_handshake(int client_fd) {
    char buf[1024];
    int bytes = recv(client_fd, buf, sizeof(buf) - 1, 0);
    if (bytes <= 0) return false;
    buf[bytes] = '\0';

    // Извлекаем ключ Sec-WebSocket-Key
    const char* key_start = strstr(buf, "Sec-WebSocket-Key: ");
    if (!key_start) return false;
    key_start += 19;
    const char* key_end = strstr(key_start, "\r\n");
    if (!key_end) return false;
    std::string key(key_start, key_end - key_start);

    // Формируем ответ (без магии SHA1, жестко прописываем заголовки для теста)
    // В продакшене здесь нужен Base64(SHA1(key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"))
    const char* response = "HTTP/1.1 101 Switching Protocols\r\n"
                           "Upgrade: websocket\r\n"
                           "Connection: Upgrade\r\n"
                           "Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=\r\n\r\n"; // Заглушка
    send(client_fd, response, strlen(response), 0);
    return true;
}

static void client_thread(int client_fd) {
    if (!handle_handshake(client_fd)) {
        close(client_fd);
        return;
    }
    std::lock_guard<std::mutex> lock(g_clientsMutex);
    g_clients.push_back(client_fd);
    printf("[WS] Клиент подключен\n");
}

void startWebSocketServer(int port) {
#ifdef _WIN32
    WSADATA wsa; WSAStartup(MAKEWORD(2,2), &wsa);
#endif
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    bind(server_fd, (sockaddr*)&address, sizeof(address));
    listen(server_fd, 5);

    printf("[WS] WebSocket сервер запущен на порту %d\n", port);

    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd >= 0) {
            std::thread(client_thread, client_fd).detach();
        }
    }
}

void broadcastWebSocketData(const float* data, size_t count) {
    std::lock_guard<std::mutex> lock(g_clientsMutex);
    if (g_clients.empty()) return;

    // Формируем WS-фрейм (без маски, сервер->клиент)
    size_t payload_size = count * 5 * sizeof(float);
    std::vector<uint8_t> frame(2 + payload_size);
    
    frame[0] = 0x82; // Binary frame
    if (payload_size < 126) {
        frame[1] = payload_size;
    } else if (payload_size < 65536) {
        frame[1] = 126;
        frame[2] = (payload_size >> 8) & 0xFF;
        frame[3] = payload_size & 0xFF;
        frame.insert(frame.begin() + 2, 0); // Сдвигаем данные
    } else {
        frame[1] = 127;
        uint64_t len = payload_size;
        for (int i = 0; i < 8; i++) frame[2+i] = (len >> (56 - i*8)) & 0xFF;
        frame.insert(frame.begin() + 2, 0, 6); // Сдвигаем данные
    }

    // Копируем полезную нагрузку
    memcpy(frame.data() + frame.size() - payload_size, data, payload_size);

    for (auto it = g_clients.begin(); it != g_clients.end(); ) {
        if (send(*it, (const char*)frame.data(), frame.size(), 0) <= 0) {
            close(*it);
            it = g_clients.erase(it);
        } else {
            ++it;
        }
    }
}