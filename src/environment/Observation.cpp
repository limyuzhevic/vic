#include "Observation.hpp"
#include <algorithm>

namespace nlm {

Observation::Observation() = default;

Observation::Observation(const SensoryInput& input) {
    sensoryInput_ = input.clone();
}

Observation::~Observation() = default;

const SensoryInput* Observation::getSensoryInput() const {
    return sensoryInput_.get();
}

bool Observation::isValid() const {
    return sensoryInput_ != nullptr;
}

std::unique_ptr<Observation> Observation::clone() const {
    if (!sensoryInput_) {
        return std::make_unique<Observation>();
    }
    auto obs = std::make_unique<Observation>(*sensoryInput_);
    return obs;
}

// Additional methods for position-based reward computation
bool Observation::getPositionAvailable() const {
    if (!sensoryInput_) return false;
    
    // Check if the sensory input contains position data
    // For Vision input, we can derive position from pixel values
    if (dynamic_cast<const Vision*>(sensoryInput_.get())) {
        return true;
    }
    
    // Check if it's a different type of sensory input
    // For now, only Vision provides position information
    return false;
}

float Observation::getX() const {
    if (!sensoryInput_) return 0.0f;
    
    // For Vision, use the brightest pixel as proxy for position
    if (const auto* vision = dynamic_cast<const Vision*>(sensoryInput_.get())) {
        const auto& data = vision->getData();
        if (!data.empty()) {
            // Find maximum value and return position in normalized range
            float max_val = *std::max_element(data.begin(), data.end());
            return (max_val / 255.0f) * 10.0f - 5.0f;  // -5 to +5 range
        }
    }
    
    return 0.0f;
}

float Observation::getY() const {
    if (!sensoryInput_) return 0.0f;
    
    // For Vision, use second brightest pixel for Y coordinate
    if (const auto* vision = dynamic_cast<const Vision*>(sensoryInput_.get())) {
        const auto& data = vision->getData();
        if (data.size() >= 2) {
            // Find top two values
            float max1 = 0.0f, max2 = 0.0f;
            for (float val : data) {
                if (val > max1) {
                    max2 = max1;
                    max1 = val;
                } else if (val > max2) {
                    max2 = val;
                }
            }
            return ((max1 + max2) / 2.0f / 255.0f) * 10.0f - 5.0f;
        }
    }
    
    return 0.0f;
}

const char* Observation::getType() const {
    if (!sensoryInput_) return "empty";
    return sensoryInput_->getType();
}

} // namespace nlm
