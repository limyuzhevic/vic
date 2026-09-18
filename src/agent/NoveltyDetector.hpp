#pragma once

#include <vector>
#include <string>

namespace nlm {

class NoveltyDetector {
public:
    NoveltyDetector(size_t visionWidth = 16, size_t visionHeight = 16);
    
    float computeNovelty(const std::vector<float>& currentVision,
                        const std::vector<float>& previousVision) const;
    
    void setDecayFactor(float decay) { decayFactor_ = decay; }
    float getDecayFactor() const { return decayFactor_; }
    
    void setVisionDimensions(size_t width, size_t height) {
        visionWidth_ = width;
        visionHeight_ = height;
    }
    
private:
    size_t visionWidth_;
    size_t visionHeight_;
    float decayFactor_;
};

} // namespace nlm