#include "SIMDVectorization.hpp"
#include <chrono>
#include <iostream>
#include <numeric>
#include <algorithm>

namespace nlm {

// VectorizedLIF implementation - update() method

template<typename VArray, typename IArray, typename RArray, typename FArray>
size_t VectorizedLIF::update(VArray& V_inout, const IArray& I, RArray& refractoryRemaining,
                           FArray& fired, size_t count) {
    // Fallback implementation when SIMD is not available
    return updateScalar(V_inout, I, refractoryRemaining, fired, count);
}

// Scalar fallback for VectorizedLIF

template<typename VArray, typename IArray, typename RArray, typename FArray>
size_t VectorizedLIF::updateScalar(VArray& V_inout, const IArray& I, RArray& refractoryRemaining,
                                 FArray& fired, size_t count) {
    size_t numFired = 0;
    
    for (size_t i = 0; i < count; ++i) {
        if (refractoryRemaining[i] > 0) {
            refractoryRemaining[i]--;
            V_inout[i] = V_rest;
            fired[i] = 0;
        } else {
            float leak = (V_rest - V_inout[i]) / tau;
            float input = I[i] / C;
            V_inout[i] += dt * 1000.0f * (leak + input);
            
            if (V_inout[i] >= V_threshold) {
                V_inout[i] = V_reset;
                refractoryRemaining[i] = static_cast<uint32_t>(refractoryDecay);
                fired[i] = 1;
                ++numFired;
            } else {
                fired[i] = 0;
            }
        }
    }
    
    return numFired;
}

// VectorizedSynapse implementation - updateSTD()

template<typename DArray, typename FArray>
void VectorizedSynapse::updateSTD(DArray& depression, FArray& facilitation,
                                 const float* lastPreTime, const float* lastPostTime,
                                 float currentTime, size_t count) {
    // Scalar fallback implementation
    for (size_t i = 0; i < count; ++i) {
        if (lastPreTime[i] > 0.0f) {
            float dt = currentTime - lastPreTime[i];
            depression[i] = std::exp(-dt / tau_depression);
            facilitation[i] = U_max * (1.0f - std::exp(-dt / tau_facilitation));
        } else {
            depression[i] = 0.0f;
            facilitation[i] = 0.0f;
        }
    }
}

// VectorizedSTDP implementation - updateWeights()

template<typename WArray, typename PreArray, typename PostArray>
void VectorizedSTDP::updateWeights(WArray& weights, const PreArray& preSpikes,
                                  const PostArray& postSpikes, size_t numPre, size_t numPost) {
    // Scalar fallback implementation
    for (size_t i = 0; i < numPre; ++i) {
        float preSpikeTime = preSpikes[i];
        for (size_t j = 0; j < numPost; ++j) {
            float postSpikeTime = postSpikes[j];
            float dt = postSpikeTime - preSpikeTime;
            float deltaW = calculateDeltaW(dt);
            
            // Find corresponding synapse weight
            size_t index = i * numPost + j;
            if (index < weights.size()) {
                weights[index] += deltaW;
            }
        }
    }
}

} // namespace nlm

// Benchmarking

inline void benchmarkSIMDVectorization() {
    std::cout << "SIMDVectorization benchmark not yet implemented" << std::endl;
}

inline void logSIMDVectorizationStats() {
    std::cout << "SIMDVectorization Stats:" << std::endl;
    std::cout << "  SIMD Width: " << nlm::SIMDUtils::SIMD_WIDTH << std::endl;
    std::cout << "  SIMD Available: " << nlm::SIMDUtils::isSIMDAvailable() << std::endl;
    std::cout << "  SIMD Type: " << nlm::SIMDUtils::getSIMDType() << std::endl;
    std::cout << "  Alignment: " << nlm::SIMDUtils::alignment() << std::endl;
}

// VectorizedLIF explicit template instantiations (not in header for header-only)

template size_t nlm::VectorizedLIF::update<std::vector<float>, std::vector<float>, std::vector<uint32_t>, std::vector<uint32_t> >(
    std::vector<float>&, const std::vector<float>&, std::vector<uint32_t>&, 
    std::vector<uint32_t>&, size_t);

// VectorizedSynapse explicit template instantiations

template void nlm::VectorizedSynapse::updateSTD<std::vector<float>, std::vector<float> >(
    std::vector<float>&, std::vector<float>&, const float*, const float*, float, size_t);

// VectorizedSTDP explicit template instantiations

template void nlm::VectorizedSTDP::updateWeights<std::vector<float>, std::vector<float>, std::vector<float> >(
    std::vector<float>&, const std::vector<float>&, const std::vector<float>&, size_t, size_t);

// VectorizedStats explicit template instantiations

template float nlm::VectorizedStats::sum<std::vector<float> >(const std::vector<float>&, size_t);
template float nlm::VectorizedStats::mean<std::vector<float> >(const std::vector<float>&, size_t);
template float nlm::VectorizedStats::variance<std::vector<float> >(const std::vector<float>&, size_t);
template float nlm::VectorizedStats::max<std::vector<float> >(const std::vector<float>&, size_t);
template void nlm::VectorizedStats::clamp<std::vector<float> >(std::vector<float>&, size_t, float, float);

// Additional SIMD implementations (if needed)

inline void initializeSIMD() {
#ifdef NLM_SIMD_AVX512
    std::cout << "Initializing AVX-512 SIMD support" << std::endl;
#endif
#ifdef NLM_SIMD_AVX2
    std::cout << "Initializing AVX2 SIMD support" << std::endl;
#endif
#ifdef NLM_SIMD_SSE
    std::cout << "Initializing SSE SIMD support" << std::endl;
#endif
}

inline void cleanupSIMD() {
    std::cout << "Cleaning up SIMD resources" << std::endl;
}

} // namespace nlm