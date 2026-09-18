#include "NoveltyDetector.hpp"
#include <cmath>

namespace nlm {

NoveltyDetector::NoveltyDetector(size_t visionWidth, size_t visionHeight)
    : visionWidth_(visionWidth)
    , visionHeight_(visionHeight)
    , decayFactor_(0.99f)
{
}

float NoveltyDetector::computeNovelty(const std::vector<float>& currentVision,
                                      const std::vector<float>& previousVision) const {
    if (currentVision.empty()) return 0.0f;
    
    float totalDiff = 0.0f;
    size_t minSize = std::min(currentVision.size(), previousVision.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        float diff = std::abs(currentVision[i] - previousVision[i]);
        totalDiff += diff;
    }
    
    // Normalize by number of elements
    float novelty = totalDiff / std::max<size_t>(currentVision.size(), 1);
    
    // Apply decay
    novelty *= decayFactor_;
    
    return novelty;
}

} // namespace nlm