#include "SensoryPercept.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <numeric>

namespace nlm {

void SensoryPercept::initializeArrays() {
    vision_.resize(visionWidth_ * visionHeight_, 0.0f);
    touch_.resize(DEFAULT_TOUCH_SIZE, 0.0f);
    internal_.resize(DEFAULT_INTERNAL_SIZE, 0.0f);
    proprioception_.resize(DEFAULT_PROPRIOCEPTION_SIZE, 0.0f);
    // audio_ is intentionally left empty (0 elements)
}

void SensoryPercept::validateArraySize(const std::vector<float>& array, size_t expectedSize, 
                                      const std::string& fieldName) const {
    if (array.size() != expectedSize) {
        NLM_LOG_WARNING("Invalid " + fieldName + " size: expected " + std::to_string(expectedSize) + 
                      ", got " + std::to_string(array.size()));
    }
}

bool SensoryPercept::validate() const {
    bool isValid = true;
    
    // Check vision array
    if (vision_.size() != visionWidth_ * visionHeight_) {
        NLM_LOG_ERROR("Vision array size mismatch");
        isValid = false;
    }
    
    // Check values are within valid range
    for (float v : vision_) {
        if (v < 0.0f || v > 1.0f) {
            NLM_LOG_WARNING("Vision value out of range: " + std::to_string(v));
            isValid = false;
        }
    }
    
    return isValid;
}

bool SensoryPercept::isEmpty() const {
    return std::all_of(vision_.begin(), vision_.end(), [](float v) { return v == 0.0f; }) &&
           std::all_of(touch_.begin(), touch_.end(), [](float t) { return t == 0.0f; }) &&
           std::all_of(internal_.begin(), internal_.end(), [](float i) { return i == 0.0f; }) &&
           std::all_of(proprioception_.begin(), proprioception_.end(), [](float p) { return p == 0.0f; });
}

size_t SensoryPercept::getTotalSignalCount() const {
    return vision_.size() + touch_.size() + internal_.size() + 
           proprioception_.size() + audio_.size();
}

std::string SensoryPercept::toString() const {
    std::stringstream ss;
    ss << "SensoryPercept {\n";
    ss << "  Vision: " << vision_.size() << "x" << visionWidth_ << " values\n";
    ss << "  Touch: " << touch_.size() << " sensors\n";
    ss << "  Internal: " << internal_.size() << " signals\n";
    ss << "  Proprioception: " << proprioception_.size() << " signals\n";
    ss << "  Audio: " << audio_.size() << " samples\n";
    ss << "  Timestamp: " << timestamp_ << "\n";
    ss << "}";
    return ss.str();
}

SensoryPercept::PerceptualStats SensoryPercept::getStats() const {
    PerceptualStats stats;
    
    // Calculate vision statistics
    if (!vision_.empty()) {
        float sum = std::accumulate(vision_.begin(), vision_.end(), 0.0f);
        stats.visionMean = sum / vision_.size();
        
        float sqSum = 0.0f;
        for (float v : vision_) {
            float diff = v - stats.visionMean;
            sqSum += diff * diff;
        }
        stats.visionStdDev = std::sqrt(sqSum / vision_.size());
    }
    
    // Find maxima for other signals
    if (!touch_.empty()) {
        stats.touchMax = *std::max_element(touch_.begin(), touch_.end());
    }
    if (!internal_.empty()) {
        stats.internalMax = *std::max_element(internal_.begin(), internal_.end());
    }
    if (!proprioception_.empty()) {
        stats.proprioceptionMax = *std::max_element(proprioception_.begin(), proprioception_.end());
    }
    
    return stats;
}

std::vector<float> SensoryPercept::getAllSignals() const {
    std::vector<float> all;
    
    // Vision (flattened)
    all.insert(all.end(), vision_.begin(), vision_.end());
    
    // Touch
    all.insert(all.end(), touch_.begin(), touch_.end());
    
    // Internal
    all.insert(all.end(), internal_.begin(), internal_.end());
    
    // Proprioception
    all.insert(all.end(), proprioception_.begin(), proprioception_.end());
    
    // Audio
    all.insert(all.end(), audio_.begin(), audio_.end());
    
    return all;
}

} // namespace nlm
