#pragma once

#include "../core/Types/Types.hpp"
#include "AgentBrainConstants.h"
#include <vector>
#include <algorithm>
#include <cmath>

namespace nlm {

class NoveltyDetector {
public:
    NoveltyDetector() = default;
    ~NoveltyDetector() = default;
    
    // Initialize novelty detector with sensory resolution
    void initialize(size_t visionWidth, size_t visionHeight);
    
    // Update novelty based on current vision and previous state
    void updateNovelty(
        const std::vector<float>& vision,
        float decayRate,
        float& noveltyLevel,
        std::vector<float>& previousVision);
    
    // Calculate novelty score from vision difference
    static float calculateNoveltyScore(
        const std::vector<float>& current,
        const std::vector<float>& previous);
    
    // Validate novelty calculation parameters
    static bool validateNoveltyParams(
        const std::vector<float>& vision,
        const std::vector<float>& previous,
        float decayRate);
    
    // Get novelty statistics
    float getNoveltyLevel() const { return noveltyLevel_; }
    
private:
    float noveltyLevel_ = 0.0f;
    std::vector<float> previousVision_;
};

} // namespace nlm