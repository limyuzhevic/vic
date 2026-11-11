#include "MemoryPool.hpp"
#include <stdexcept>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <chrono>

namespace nlm {

MemoryPool::MemoryPool() : growthFactor(2.0), alignment(8) {
    currentSize = 0;
}

MemoryPool::~MemoryPool() {
    reset();
}

void* MemoryPool::allocate(size_t size, size_t align) {
    if (size == 0) return nullptr;
    
    // Round up size to alignment
    size = (size + align - 1) & ~(align - 1);
    
    // Find appropriate free list or create new block
    auto it = freeBlocks.find(size);
    if (it != freeBlocks.end() && !it->second.empty()) {
        void* ptr = it->second.back();
        it->second.pop_back();
        currentSize--;
        return ptr;
    }
    
    // Allocate new block
    return allocateBlock(size);
}

void MemoryPool::deallocate(void* ptr, size_t size) {
    if (!ptr) return;
    
    auto it = freeBlocks.find(size);
    if (it != freeBlocks.end()) {
        it->second.push_back(ptr);
    } else {
        freeBlocks[size] = {ptr};
    }
    
    currentSize++;
}

MemoryPool::Statistics MemoryPool::getStatistics() const {
    Statistics stats;
    stats.allocations = currentSize + freeBlocks.size();
    stats.deallocations = stats.allocations - currentSize;
    stats.currentSize = currentSize;
    stats.peakSize = currentSize; // TODO: Track peak properly
    stats.bytesAllocated = 0; // TODO: Track bytes properly
    stats.fragmentation = 0.0; // TODO: Calculate fragmentation
    return stats;
}

void MemoryPool::reset() {
    // Deallocate all blocks
    for (Block* block : blocks) {
        ::free(block->ptr);
        delete block;
    }
    blocks.clear();
    freeBlocks.clear();
    currentSize = 0;
}

void MemoryPool::setGrowthFactor(double factor) {
    growthFactor = std::max(1.0, factor);
}

void* MemoryPool::allocateBlock(size_t size) {
    // Calculate block size (power of 2 for efficiency)
    size_t blockSize = size;
    if (blockSize & (blockSize - 1)) {
        blockSize = 1;
        while (blockSize < size) {
            blockSize <<= 1;
        }
    }
    
    // Allocate memory aligned to alignment
    void* ptr = nullptr;
    if (posix_memalign(&ptr, alignment, blockSize) != 0) {
        throw std::bad_alloc();
    }
    
    Block* block = createBlock(blockSize);
    block->ptr = ptr;
    blocks.push_back(block);
    currentSize++;
    
    return ptr;
}

void MemoryPool::deallocateBlock(void* ptr) {
    // Find block containing ptr
    for (Block* block : blocks) {
        if (block->ptr == ptr) {
            destroyBlock(block);
            return;
        }
    }
}

MemoryPool::Block* MemoryPool::createBlock(size_t size) {
    Block* block = new Block;
    block->ptr = nullptr;
    block->size = size;
    block->next = nullptr;
    return block;
}

void MemoryPool::destroyBlock(Block* block) {
    ::free(block->ptr);
    delete block;
}

namespace simd {

void add(const float* a, const float* b, float* result, size_t size) {
#ifdef __AVX__
    // AVX implementation
    size_t i = 0;
    for (; i + 8 <= size; i += 8) {
        __m256 va = _mm256_loadu_ps(a + i);
        __m256 vb = _mm256_loadu_ps(b + i);
        __m256 vresult = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(result + i, vresult);
    }
#endif
    // Scalar fallback
    for (; i < size; ++i) {
        result[i] = a[i] + b[i];
    }
}

void multiply(const float* a, const float* b, float* result, size_t size) {
#ifdef __AVX__
    // AVX implementation
    size_t i = 0;
    for (; i + 8 <= size; i += 8) {
        __m256 va = _mm256_loadu_ps(a + i);
        __m256 vb = _mm256_loadu_ps(b + i);
        __m256 vresult = _mm256_mul_ps(va, vb);
        _mm256_storeu_ps(result + i, vresult);
    }
#endif
    // Scalar fallback
    for (; i < size; ++i) {
        result[i] = a[i] * b[i];
    }
}

void sigmoid(const float* input, float* output, size_t size) {
    size_t i = 0;
    for (; i + 8 <= size; i += 8) {
#ifdef __AVX__
        __m256 vx = _mm256_loadu_ps(input + i);
        __m256 vresult = _mm256_div_ps(_mm256_set1_ps(1.0f), _mm256_add_ps(_mm256_set1_ps(1.0f), _mm256_exp_ps(vx)));
        _mm256_storeu_ps(output + i, vresult);
#else
        for (int j = 0; j < 8; ++j) {
            output[i + j] = 1.0f / (1.0f + std::exp(input[i + j]));
        }
#endif
    }
    for (; i < size; ++i) {
        output[i] = 1.0f / (1.0f + std::exp(input[i]));
    }
}

void relu(const float* input, float* output, size_t size) {
    size_t i = 0;
    for (; i + 8 <= size; i += 8) {
#ifdef __AVX__
        __m256 vx = _mm256_loadu_ps(input + i);
        __m256 vresult = _mm256_max_ps(vx, _mm256_setzero_ps());
        _mm256_storeu_ps(output + i, vresult);
#else
        for (int j = 0; j < 8; ++j) {
            output[i + j] = std::max(input[i + j], 0.0f);
        }
#endif
    }
    for (; i < size; ++i) {
        output[i] = std::max(input[i], 0.0f);
    }
}

void softmax(const float* input, float* output, size_t size) {
    // Find maximum value
    float maxVal = input[0];
    for (size_t i = 1; i < size; ++i) {
        if (input[i] > maxVal) maxVal = input[i];
    }
    
    // Calculate exp(x - max) and sum
    float sum = 0.0f;
    for (size_t i = 0; i < size; ++i) {
        output[i] = std::exp(input[i] - maxVal);
        sum += output[i];
    }
    
    // Normalize
    float invSum = 1.0f / sum;
    for (size_t i = 0; i < size; ++i) {
        output[i] *= invSum;
    }
}

void matrixMultiply(const float* a, const float* b, float* result,
                   size_t m, size_t n, size_t p) {
    // Initialize result to zero
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < p; ++j) {
            result[i * p + j] = 0.0f;
        }
    }
    
    // Matrix multiplication
    for (size_t i = 0; i < m; ++i) {
        for (size_t k = 0; k < n; ++k) {
            float a_ik = a[i * n + k];
            for (size_t j = 0; j < p; ++j) {
                result[i * p + j] += a_ik * b[k * p + j];
            }
        }
    }
}

void elementWise(const float* a, const float* b, float* result,
                size_t size, int op) {
    switch (op) {
        case 0: // Add
            for (size_t i = 0; i < size; ++i) {
                result[i] = a[i] + b[i];
            }
            break;
        case 1: // Multiply
            for (size_t i = 0; i < size; ++i) {
                result[i] = a[i] * b[i];
            }
            break;
        case 2: // Max
            for (size_t i = 0; i < size; ++i) {
                result[i] = std::max(a[i], b[i]);
            }
            break;
        case 3: // Min
            for (size_t i = 0; i < size; ++i) {
                result[i] = std::min(a[i], b[i]);
            }
            break;
        default:
            break;
    }
}

} // namespace simd

void PerformanceMonitor::start(const std::string& name) {
    for (auto& measurement : measurements) {
        if (!measurement.active && measurement.name == name) {
            measurement.active = true;
            measurement.startTime = std::chrono::high_resolution_clock::now().time_since_epoch().count();
            return;
        }
    }
    measurements.push_back({name, 0.0, 0.0, true});
}

double PerformanceMonitor::stop(const std::string& name) {
    auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    
    for (auto& measurement : measurements) {
        if (measurement.active && measurement.name == name) {
            measurement.active = false;
            measurement.endTime = now;
            double elapsed = (measurement.endTime - measurement.startTime) * 1e-9; // Convert to seconds
            stats.timings[name].push_back(elapsed);
            return elapsed;
        }
    }
    
    return 0.0;
}

void PerformanceMonitor::recordAllocation(size_t size) {
    stats.totalAllocations++;
    stats.currentMemoryUsage += size;
    if (stats.currentMemoryUsage > stats.peakMemoryUsage) {
        stats.peakMemoryUsage = stats.currentMemoryUsage;
    }
}

void PerformanceMonitor::recordDeallocation(size_t size) {
    stats.totalDeallocations++;
    stats.currentMemoryUsage -= size;
}

PerformanceMonitor::Statistics PerformanceMonitor::getStatistics() const {
    return stats;
}

std::string PerformanceMonitor::generateReport() const {
    std::string report = "=== Performance Report ===\n";
    report += "Total allocations: " + std::to_string(stats.totalAllocations) + "\n";
    report += "Total deallocations: " + std::to_string(stats.totalDeallocations) + "\n";
    report += "Current memory usage: " + std::to_string(stats.currentMemoryUsage) + " bytes\n";
    report += "Peak memory usage: " + std::to_string(stats.peakMemoryUsage) + " bytes\n";
    report += "Total operations: " + std::to_string(stats.totalOperations) + "\n\n";
    
    report += "=== Measurement Details ===\n";
    for (const auto& timing : stats.timings) {
        report += "Measurement: " + timing.first + "\n";
        report += "  Count: " + std::to_string(timing.second.size()) + "\n";
        if (!timing.second.empty()) {
            double sum = 0.0, min = timing.second[0], max = timing.second[0];
            for (double t : timing.second) {
                sum += t;
                if (t < min) min = t;
                if (t > max) max = t;
            }
            double avg = sum / timing.second.size();
            report += "  Average: " + std::to_string(avg) + " s\n";
            report += "  Minimum: " + std::to_string(min) + " s\n";
            report += "  Maximum: " + std::to_string(max) + " s\n";
        }
    }
    
    return report;
}

void PerformanceMonitor::clear() {
    stats.timings.clear();
    stats.totalAllocations = 0;
    stats.totalDeallocations = 0;
    stats.currentMemoryUsage = 0;
    stats.peakMemoryUsage = 0;
    stats.totalOperations = 0;
}

} // namespace nlm