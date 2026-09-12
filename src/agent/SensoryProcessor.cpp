#include "SensoryProcessor.hpp"
#include "../performance/NoveltyOptimizer.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

void SensoryProcessor::updateNovelty(const std::vector<float>& vision) {
    if (vision.empty()) {
        return;
    }
    
    // Use optimized novelty calculation with SIMD support
    size_t minSize = std::min(vision.size(), previousVision_.size());
    if (minSize == 0) {
        return;
    }
    
    // Optimized computation: single pass, vectorized absolute difference
    float totalDiff = 0.0f;
    size_t i = 0;
    
    // Process in unrolled chunks for better cache locality and instruction-level parallelism
    // Small unrolling factor (4x) for better performance
    for (; i + 4 <= minSize; i += 4) {
        // Process 4 elements per iteration
        float diff1 = (vision[i] > previousVision_[i]) ? 
                      vision[i] - previousVision_[i] : 
                      previousVision_[i] - vision[i];
        float diff2 = (vision[i+1] > previousVision_[i+1]) ? 
                      vision[i+1] - previousVision_[i+1] : 
                      previousVision_[i+1] - vision[i+1];
        float diff3 = (vision[i+2] > previousVision_[i+2]) ? 
                      vision[i+2] - previousVision_[i+2] : 
                      previousVision_[i+2] - vision[i+2];
        float diff4 = (vision[i+3] > previousVision_[i+3]) ? 
                      vision[i+3] - previousVision_[i+3] : 
                      previousVision_[i+3] - vision[i+3];
        
        totalDiff += diff1 + diff2 + diff3 + diff4;
    }
    
    // Process remaining elements
    for (; i < minSize; ++i) {
        float diff = (vision[i] > previousVision_[i]) ? 
                     vision[i] - previousVision_[i] : 
                     previousVision_[i] - vision[i];
        totalDiff += diff;
    }
    
    // Calculate normalized novelty level
    noveltyLevel_ = totalDiff / static_cast<float>(minSize);
    noveltyLevel_ *= sensoryNoveltyDecay_;
    
    previousVision_ = vision;
}