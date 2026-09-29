#include "NoveltyDetector.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

void NoveltyDetector::initialize(size_t visionWidth, size_t visionHeight) {
    noveltyLevel_ = 0.0f;
    previousVision_.resize(visionWidth * visionHeight, 0.0f);
}

void NoveltyDetector::updateNovelty(
    const std::vector<float>& vision,
    float decayRate,
    float& noveltyLevel,
    std::vector<float>& previousVision) {
    
    if (!validateNoveltyParams(vision, previousVision, decayRate)) {
        return;
    }
    
    if (vision.empty()) {
        return;
    }
    
    float totalDiff = calculateNoveltyScore(vision, previousVision_);
    
    // Normalize
    noveltyLevel = totalDiff / std::max<size_t>(vision.size(), 1);
    
    // Decay and update
    noveltyLevel *= decayRate;
    
    // Store for next time
    previousVision_ = vision;
}

float NoveltyDetector::calculateNoveltyScore(
    const std::vector<float>& current,
    const std::vector<float>& previous) {
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < current.size() && i < previous.size(); ++i) {
        float diff = std::abs(current[i] - previous[i]);
        totalDiff += diff;
    }
    return totalDiff;
}

bool NoveltyDetector::validateNoveltyParams(
    const std::vector<float>& vision,
    const std::vector<float>& previous,
    float decayRate) {
    
    // Check for NaN or infinite values
    if (std::isnan(decayRate) || std::isinf(decayRate)) {
        return false;
    }
    
    // Check for NaN or infinite values in vectors
    for (float v : vision) {
        if (std::isnan(v) || std::isinf(v)) {
            return false;
        }
    }
    
    for (float v : previous) {
        if (std::isnan(v) || std::isinf(v)) {
            return false;
        }
    }
    
    return true;
}

} // namespace nlm