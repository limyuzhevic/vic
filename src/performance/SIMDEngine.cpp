#include "SIMDEngine.hpp"
#include <iostream>
#include <cstring>
#include <thread>
#include <immintrin.h>

namespace nlm {

SIMDEngine::SIMDEngine()
    : hasAVX512_(false)
    , hasAVX2_(false)
    , hasSSE_(false)
    , preferAVX512_(true)
    , preferAVX2_(true)
    , preferSSE_(true)
    , simdOperations_(0)
    , avx512Detector_([]() {
        // Check AVX512 availability
        uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
        __asm__ __volatile__ ("cpuid"
                              : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
                              : "0"(7), "2"(0));
        return (ecx & 0x10) != 0;
    })
    , avx2Detector_([]() {
        // Check AVX2 availability
        uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
        __asm__ __volatile__ ("cpuid"
                              : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
                              : "0"(7), "2"(0));
        return (ebx & 0x18) != 0;
    })
    , sseDetector_([]() {
        // Check SSE availability
        uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
        __asm__ __volatile__ ("cpuid"
                              : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
                              : "0"(1));
        return (edx & 0x2000) != 0;
    }) {}

void SIMDEngine::detectSIMDCapabilities() {
    hasAVX512_ = avx512Detector_();
    hasAVX2_ = avx2Detector_();
    hasSSE_ = sseDetector_();
    
    if (SIMDEngine::getInstance().isEnabled()) {
        std::cout << "[SIMD] Detected SIMD capabilities:" << std::endl;
        std::cout << "[SIMD]   AVX-512: " << (hasAVX512_ ? "Yes" : "No") << std::endl;
        std::cout << "[SIMD]   AVX2: " << (hasAVX2_ ? "Yes" : "No") << std::endl;
        std::cout << "[SIMD]   SSE: " << (hasSSE_ ? "Yes" : "No") << std::endl;
    }
}

const char* SIMDEngine::getBestSIMD() const {
    if (preferAVX512_ && hasAVX512_) return "AVX-512";
    if (preferAVX2_ && hasAVX2_) return "AVX2";
    if (preferSSE_ && hasSSE_) return "SSE";
    return "Scalar";
}

void SIMDEngine::setAVX512Detector(std::function<bool()> detector) {
    avx512Detector_ = detector;
}

void SIMDEngine::setAVX2Detector(std::function<bool()> detector) {
    avx2Detector_ = detector;
}

void SIMDEngine::setSSEDetector(std::function<bool()> detector) {
    sseDetector_ = detector;
}

template<typename ScalarFunc, typename VectorFunc>
auto SIMDEngine::dispatchSIMD(ScalarFunc scalarFunc, VectorFunc vectorFunc) {
    const char* simdType = getBestSIMD();
    
    if (strcmp(simdType, "AVX-512") == 0) {
        incrementSIMDOperations();
        return vectorFunc();
    } else if (strcmp(simdType, "AVX2") == 0) {
        incrementSIMDOperations();
        return vectorFunc();
    } else if (strcmp(simdType, "SSE") == 0) {
        incrementSIMDOperations();
        return vectorFunc();
    } else {
        return scalarFunc();
    }
}

const char* SIMDEngine::getSIMDType() const {
    const char* bestSIMD = getBestSIMD();
    if (strcmp(bestSIMD, "AVX-512") == 0) return "AVX-512";
    if (strcmp(bestSIMD, "AVX2") == 0) return "AVX2";
    if (strcmp(bestSIMD, "SSE") == 0) return "SSE";
    return "Scalar";
}

} // namespace nlm