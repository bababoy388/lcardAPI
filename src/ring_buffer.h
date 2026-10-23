#pragma once
#include <vector>
#include <atomic>
#include <cstddef>

struct Sample {
    long long timestamp_ns;
    double values[4];
};

const size_t MAX_SAMPLES = 1 << 21; 
const size_t BUF_MASK    = MAX_SAMPLES - 1;

extern std::vector<Sample>    g_ringBuffer;
extern std::atomic<size_t>    g_write_idx;
extern std::atomic<long long> g_totalSamples;

void      addSamples(const double* data, int points, int channels, double fs);
void      clearBuffer();
size_t    bufferSize();
long long totalSamples();