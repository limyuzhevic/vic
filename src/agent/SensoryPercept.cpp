#include "SensoryPercept.hpp"

namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
{
    // Pre-allocate vectors with optimal sizes to avoid reallocations
    vision_.reserve(16 * 16);  // Reserve 256 elements
    vision_.resize(16 * 16, 0.0f);
    touch_.reserve(8);
    touch_.resize(8, 0.0f);
    internal_.reserve(4);
    internal_.resize(4, 0.0f);
    proprioception_.reserve(6);
    proprioception_.resize(6, 0.0f);
    // Audio initially empty, but reserve some space for potential future use
    audio_.reserve(0);
}

// Configure vision dimensions (useful for different environments)
void SensoryPercept::configureVision(size_t width, size_t height) {
    vision_.resize(width * height, 0.0f);
    visionWidth_ = width;
    visionHeight_ = height;
}

// Get all sensory signals as a flat vector (for brain input)
// This provides a consistent interface for the brain to receive sensory data
std::vector<float> SensoryPercept::getAllSignals() const {
    // Pre-allocate result vector with exact size for optimal performance
    size_t totalSize = vision_.size() + touch_.size() + internal_.size() + 
                     proprioception_.size() + audio_.size();
    std::vector<float> all;
    all.reserve(totalSize);
    all.resize(totalSize);
    
    // Use iterators for efficient copying
    size_t pos = 0;
    
    // Copy vision data
    if (!vision_.empty()) {
        std::copy(vision_.begin(), vision_.end(), all.begin() + pos);
        pos += vision_.size();
    }
    
    // Copy touch data
    if (!touch_.empty()) {
        std::copy(touch_.begin(), touch_.end(), all.begin() + pos);
        pos += touch_.size();
    }
    
    // Copy internal data
    if (!internal_.empty()) {
        std::copy(internal_.begin(), internal_.end(), all.begin() + pos);
        pos += internal_.size();
    }
    
    // Copy proprioception data
    if (!proprioception_.empty()) {
        std::copy(proprioception_.begin(), proprioception_.end(), all.begin() + pos);
        pos += proprioception_.size();
    }
    
    // Copy audio data (if enabled)
    if (!audio_.empty()) {
        std::copy(audio_.begin(), audio_.end(), all.begin() + pos);
        pos += audio_.size();
    }
    
    return all;
}

// Get signal indices for advanced sensory processing
// Returns mapping of signal type to index range for efficient access
// Uses static cache for performance optimization - returns reference to cached map
const std::map<std::string, std::pair<size_t, size_t>>& SensoryPercept::getSignalRanges() const {
    static std::map<std::string, std::pair<size_t, size_t>> ranges;
    ranges.clear();
    
    size_t currentPos = 0;
    
    // Vision range
    if (!vision_.empty()) {
        ranges["vision"] = {currentPos, currentPos + vision_.size()};
        currentPos += vision_.size();
    }
    
    // Touch range
    if (!touch_.empty()) {
        ranges["touch"] = {currentPos, currentPos + touch_.size()};
        currentPos += touch_.size();
    }
    
    // Internal range
    if (!internal_.empty()) {
        ranges["internal"] = {currentPos, currentPos + internal_.size()};
        currentPos += internal_.size();
    }
    
    // Proprioception range
    if (!proprioception_.empty()) {
        ranges["proprioception"] = {currentPos, currentPos + proprioception_.size()};
        currentPos += proprioception_.size();
    }
    
    // Audio range (if present)
    if (!audio_.empty()) {
        ranges["audio"] = {currentPos, currentPos + audio_.size()};
    }
    
    return ranges;
}

// Validate sensory data ranges
// Ensures all vectors have reasonable sizes for the system
bool SensoryPercept::validateData() const {
    // Vision should have positive dimensions
    if (visionWidth_ == 0 || visionHeight_ == 0 || vision_.empty()) {
        return false;
    }
    
    // Check that vision size matches dimensions
    if (vision_.size() != visionWidth_ * visionHeight_) {
        return false;
    }
    
    // Touch should have reasonable size (2-16 sensors)
    if (touch_.size() > 16) {
        return false;
    }
    
    // Other sensory modalities should have reasonable sizes
    if (internal_.size() > 8 || proprioception_.size() > 12) {
        return false;
    }
    
    // Audio, if present, should be reasonable
    if (!audio_.empty() && audio_.size() > 1024) {
        return false;
    }
    
    return true;
}

} // namespace nlm
