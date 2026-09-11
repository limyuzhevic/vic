#include "SIMDVectorization.hpp"
#include <immintrin.h>
#include <xmmintrin.h>

namespace nlm {

struct SIMDVectorization::Impl {
    size_t vectorWidth;
    bool avxSupported;
    bool sseSupported;
    bool avx2Supported;
};

SIMDVectorization::SIMDVectorization() : pImpl(std::make_unique<Impl>()) {
    pImpl->vectorWidth = 8; // Default for AVX
    pImpl->avxSupported = isSupported();
    pImpl->sseSupported = true;
    pImpl->avx2Supported = false;
}

SIMDVectorization::~SIMDVectorization() = default;

bool SIMDVectorization::isSupported() {
    int cpuInfo[4];
    
    // Check AVX support
    __cpuid(1, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    bool avx = (cpuInfo[2] & 0x18) == 0x18;
    
    // Check AVX2 support
    __cpuid(7, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
    bool avx2 = cpuInfo[1] & 0x20;
    
    return avx || avx2;
}

SIMDVectorization& SIMDVectorization::getInstance() {
    static SIMDVectorization instance;
    return instance;
}

void SIMDVectorization::vectorAdd(const float* a, const float* b, float* result, size_t n) {
    size_t i = 0;
    
    if (pImpl->avx2Supported) {
        // AVX2 implementation
        for (; i + 8 <= n; i += 8) {
            __m256 va = _mm256_loadu_ps(a + i);
            __m256 vb = _mm256_loadu_ps(b + i);
            __m256 vresult = _mm256_add_ps(va, vb);
            _mm256_storeu_ps(result + i, vresult);
        }
    } else if (pImpl->avxSupported) {
        // AVX implementation
        for (; i + 8 <= n; i += 8) {
            __m256 va = _mm256_loadu_ps(a + i);
            __m256 vb = _mm256_loadu_ps(b + i);
            __m256 vresult = _mm256_add_ps(va, vb);
            _mm256_storeu_ps(result + i, vresult);
        }
    }
    
    // Scalar fallback
    for (; i < n; ++i) {
        result[i] = a[i] + b[i];
    }
}

void SIMDVectorization::vectorMultiply(const float* a, const float* b, float* result, size_t n) {
    size_t i = 0;
    
    if (pImpl->avx2Supported) {
        for (; i + 8 <= n; i += 8) {
            __m256 va = _mm256_loadu_ps(a + i);
            __m256 vb = _mm256_loadu_ps(b + i);
            __m256 vresult = _mm256_mul_ps(va, vb);
            _mm256_storeu_ps(result + i, vresult);
        }
    } else if (pImpl->avxSupported) {
        for (; i + 8 <= n; i += 8) {
            __m256 va = _mm256_loadu_ps(a + i);
            __m256 vb = _mm256_loadu_ps(b + i);
            __m256 vresult = _mm256_mul_ps(va, vb);
            _mm256_storeu_ps(result + i, vresult);
        }
    }
    
    for (; i < n; ++i) {
        result[i] = a[i] * b[i];
    }
}

void SIMDVectorization::vectorDotProduct(const float* a, const float* b, float& result, size_t n) {
    float sum = 0.0f;
    size_t i = 0;
    
    if (pImpl->avx2Supported) {
        __m256 sumVec = _mm256_setzero_ps();
        for (; i + 8 <= n; i += 8) {
            __m256 va = _mm256_loadu_ps(a + i);
            __m256 vb = _mm256_loadu_ps(b + i);
            sumVec = _mm256_add_ps(sumVec, _mm256_mul_ps(va, vb));
        }
        
        // Horizontal sum
        alignas(32) float temp[8];
        _mm256_store_ps(temp, sumVec);
        sum = temp[0] + temp[1] + temp[2] + temp[3] +
              temp[4] + temp[5] + temp[6] + temp[7];
    } else if (pImpl->avxSupported) {
        __m256 sumVec = _mm256_setzero_ps();
        for (; i + 8 <= n; i += 8) {
            __m256 va = _mm256_loadu_ps(a + i);
            __m256 vb = _mm256_loadu_ps(b + i);
            sumVec = _mm256_add_ps(sumVec, _mm256_mul_ps(va, vb));
        }
        
        alignas(32) float temp[8];
        _mm256_store_ps(temp, sumVec);
        sum = temp[0] + temp[1] + temp[2] + temp[3] +
              temp[4] + temp[5] + temp[6] + temp[7];
    }
    
    for (; i < n; ++i) {
        sum += a[i] * b[i];
    }
    
    result = sum;
}

void SIMDVectorization::neuralUpdateWeights(const float* input, const float* error, float* weights, size_t size) {
    // Placeholder implementation
}

void SIMDVectorization::neuralForwardPropagate(const float* input, float* output, size_t size) {
    // Placeholder implementation
}

void SIMDVectorization::neuralBackPropagate(const float* output, const float* target, float* error, size_t size) {
    // Placeholder implementation
}

void SIMDVectorization::neuronUpdateMembranePotential(float* v, float* I, float* tau, size_t n) {
    // Placeholder implementation
}

void SIMDVectorization::neuronUpdateSynapticWeights(float* w, float* delta, float learningRate, size_t n) {
    // Placeholder implementation
}

size_t SIMDVectorization::getVectorWidth() {
    return 8; // Default for AVX
}

std::string SIMDVectorization::getCapabilities() {
    std::string caps = "SSE:";
    caps += (pImpl->sseSupported ? "1" : "0");
    caps += ", AVX:";
    caps += (pImpl->avxSupported ? "1" : "0");
    caps += ", AVX2:";
    caps += (pImpl->avx2Supported ? "1" : "0");
    return caps;
}

} // namespace nlm
