#include "SensoryPercept.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>

namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
{
    vision_.resize(16 * 16, 0.0f);
    touch_.resize(8, 0.0f);
    internal_.resize(4, 0.0f);
    proprioception_.resize(6, 0.0f);
    audio_.resize(0, 0.0f);
}

void SensoryPercept::setVision(const std::vector<float>& v, size_t width, size_t height) {
    if (v.size() < width * height) {
        Logger::getGlobal().error("Vision data size doesn't match dimensions");
        return;
    }
    vision_ = v;
    visionWidth_ = width;
    visionHeight_ = height;
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

std::vector<float> SensoryPercept::getSignalByType(const std::string& type) const {
    std::string lowerType = type;
    for (char& c : lowerType) c = tolower(c);
    
    if (lowerType == "vision") return vision_;
    if (lowerType == "touch") return touch_;
    if (lowerType == "internal") return internal_;
    if (lowerType == "proprioception") return proprioception_;
    if (lowerType == "audio") return audio_;
    
    Logger::getGlobal().warning("Unknown sensory type: " + type);
    return std::vector<float>();
}

bool SensoryPercept::validate() const {
    // Check if all arrays have consistent sizes
    bool valid = true;
    
    if (vision_.size() != visionWidth_ * visionHeight_) {
        Logger::getGlobal().error("Vision array size doesn't match dimensions");
        valid = false;
    }
    
    // Check if all values are in reasonable ranges
    for (float v : vision_) {
        if (v < 0.0f || v > 1.0f) {
            Logger::getGlobal().warning("Vision value out of range [0,1]: " + std::to_string(v));
        }
    }
    
    return valid;
}

std::string SensoryPercept::toString() const {
    std::stringstream ss;
    ss << "SensoryPercept [timestamp=" << timestamp_ << "]\n";
    ss << "  Vision: " << visionWidth_ << "x" << visionHeight_ << " (" << vision_.size() << " values)\n";
    ss << "  Touch: " << touch_.size() << " values\n";
    ss << "  Internal: " << internal_.size() << " values\n";
    ss << "  Proprioception: " << proprioception_.size() << " values\n";
    ss << "  Audio: " << audio_.size() << " values\n";
    return ss.str();
}

} // namespace nlm