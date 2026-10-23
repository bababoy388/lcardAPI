#include "ring_buffer.h"
#include "app_state.h"
#include "websocket_server.h"
#include <cstring>
#include <vector>

std::vector<Sample>    g_ringBuffer(MAX_SAMPLES);
std::atomic<size_t>    g_write_idx{0};
std::atomic<long long> g_totalSamples{0};

void addSamples(const double* data, int points, int channels, double fs) {
    long long t0ns = g_recordStartEpochNs.load();
    long long startIdx = g_totalSamples.load();
    
    std::vector<float> ws_buf(points * 5);
    int ws_idx = 0;

    for (int k = 0; k < points; k++) {
        size_t idx = g_write_idx.load(std::memory_order_relaxed);
        Sample& s = g_ringBuffer[idx];
        
        // Время считается от глобального индекса
        long long offsetNs = (long long)((long double)(startIdx + k) * 1e9L / fs);
        s.timestamp_ns = t0ns + offsetNs;
        
        for (int ch = 0; ch < 4; ch++) s.values[ch] = 0.0;
        for (int ch = 0; ch < channels; ch++) {
            s.values[ch] = data[k * channels + ch];
        }
        
        g_write_idx.store((idx + 1) & BUF_MASK, std::memory_order_release);
        g_totalSamples.fetch_add(1, std::memory_order_relaxed);

        ws_buf[ws_idx++] = (float)(s.timestamp_ns / 1e9);
        for (int ch = 0; ch < 4; ch++) ws_buf[ws_idx++] = (float)s.values[ch];
    }
    
    broadcastWebSocketData(ws_buf.data(), points);
}

void clearBuffer() {
    g_write_idx = 0;
    g_totalSamples = 0;
}

size_t bufferSize() {
    long long t = g_totalSamples.load();
    return (size_t)(t > MAX_SAMPLES ? MAX_SAMPLES : t);
}

long long totalSamples() {
    return g_totalSamples.load();
}