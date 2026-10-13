#include "SensoryPercept.hpp"

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

std::vector<float> SensoryPercept::getAllSignals() const {
    std::vector<float> all;
    all.reserve(getTotalSignalCount());
    
    // Vision (flattened) - validated size
    all.insert(all.end(), vision_.begin(), vision_.end());
    
    // Touch - validated size
    all.insert(all.end(), touch_.begin(), touch_.end());
    
    // Internal - validated size
    all.insert(all.end(), internal_.begin(), internal_.end());
    
    // Proprioception - validated size
    all.insert(all.end(), proprioception_.begin(), proprioception_.end());
    
    // Audio - validated size
    all.insert(all.end(), audio_.begin(), audio_.end());
    
    return all;
}

// Helper method to calculate total expected signal count
size_t SensoryPercept::getTotalSignalCount() const {
    return vision_.size() + touch_.size() + internal_.size() + 
           proprioception_.size() + audio_.size();
}

// Setters with bounds checking
void SensoryPercept::setVision(const std::vector<float>& v) {
    if (v.size() != visionWidth_ * visionHeight_) {
        throw std::invalid_argument("Vision data size mismatch. "
            "Expected " + std::to_string(visionWidth_ * visionHeight_) +
            " values, got " + std::to_string(v.size()));
    }
    vision_ = v;
}

void SensoryPercept::setTouch(const std::vector<float>& t) {
    if (t.size() != 8) {
        throw std::invalid_argument("Touch data size mismatch. "
            "Expected 8 values, got " + std::to_string(t.size()));
    }
    touch_ = t;
}

void SensoryPercept::setInternal(const std::vector<float>& i) {
    if (i.size() != 4) {
        throw std::invalid_argument("Internal data size mismatch. "
            "Expected 4 values, got " + std::to_string(i.size()));
    }
    internal_ = i;
}

void SensoryPercept::setProprioception(const std::vector<float>& p) {
    if (p.size() != 6) {
        throw std::invalid_argument("Proprioception data size mismatch. "
            "Expected 6 values, got " + std::to_string(p.size()));
    }
    proprioception_ = p;
}

void SensoryPercept::setAudio(const std::vector<float>& a) {
    audio_ = a;
}

// Validation methods
bool SensoryPercept::isVisionValid(const std::vector<float>& v) const {
    return v.size() == visionWidth_ * visionHeight_;
}

bool SensoryPercept::isTouchValid(const std::vector<float>& t) const {
    return t.size() == 8;
}

bool SensoryPercept::isInternalValid(const std::vector<float>& i) const {
    return i.size() == 4;
}

bool SensoryPercept::isProprioceptionValid(const std::vector<float>& p) const {
    return p.size() == 6;
}

// Configuration methods
void SensoryPercept::setVisionDimensions(size_t width, size_t height) {
    visionWidth_ = width;
    visionHeight_ = height;
    vision_.resize(width * height, 0.0f);
}

void SensoryPercept::setTouchCount(size_t count) {
    touch_.resize(count, 0.0f);
}

void SensoryPercept::setInternalCount(size_t count) {
    internal_.resize(count, 0.0f);
}

void SensoryPercept::setProprioceptionCount(size_t count) {
    proprioception_.resize(count, 0.0f);
}

} // namespace nlm
