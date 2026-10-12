// SIMDEngine.hpp - New runtime SIMD engine
#pragma once

#include "Performance.hpp"
#include <string>
#include <functional>
#include <thread>

namespace nlm {

class SIMDEngine {
public:
    SIMDEngine();
    ~SIMDEngine() = default;
    
    // Disable copying
    SIMDEngine(const SIMDEngine&) = delete;
    SIMDEngine& operator=(const SIMDEngine&) = delete;
    
    // Detect available SIMD instruction sets
    void detectSIMDCapabilities();
    
    // Check if specific SIMD is available
    bool hasAVX512() const { return hasAVX512_; }
    bool hasAVX2() const { return hasAVX2_; }
    bool hasSSE() const { return hasSSE_; }
    
    // Get best available SIMD level
    const char* getBestSIMD() const;
    
    // SIMD detection callbacks
    void setAVX512Detector(std::function<bool()> detector);
    void setAVX2Detector(std::function<bool()> detector);
    void setSSEDetector(std::function<bool()> detector);
    
    // Runtime SIMD dispatch
    template<typename ScalarFunc, typename VectorFunc>
    auto dispatchSIMD(ScalarFunc scalarFunc, VectorFunc vectorFunc);
    
    // SIMD operation counters
    uint64_t getSIMDOperationsCount() const { return simdOperations_; }
    void incrementSIMDOperations() { ++simdOperations_; }
    
    // SIMD configuration
    void setPreferAVX512(bool prefer) { preferAVX512_ = prefer; }
    void setPreferAVX2(bool prefer) { preferAVX2_ = prefer; }
    void setPreferSSE(bool prefer) { preferSSE_ = prefer; }
    
    // Get SIMD type
    const char* getSIMDType() const;
    
    // Get singleton instance
    static SIMDEngine& getInstance() {
        static SIMDEngine instance;
        return instance;
    }
    
private:
    bool hasAVX512_;
    bool hasAVX2_;
    bool hasSSE_;
    bool preferAVX512_;
    bool preferAVX2_;
    bool preferSSE_;
    uint64_t simdOperations_;
    std::function<bool()> avx512Detector_;
    std::function<bool()> avx2Detector_;
    std::function<bool()> sseDetector_;
};

} // namespace nlm