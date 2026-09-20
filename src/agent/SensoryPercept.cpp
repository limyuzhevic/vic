#include "SensoryPercept.hpp"

namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(VISION_WIDTH)
    , visionHeight_(VISION_HEIGHT)
    , timestamp_(0.0)
{
    // Validate and initialize vision buffer
    if (VISION_WIDTH * VISION_HEIGHT != VISION_SIZE) {
        throw std::runtime_error("Invalid vision dimensions");
    }
    vision_.resize(VISION_SIZE, 0.0f);
    
    // Initialize touch buffer
    touch_.resize(TOUCH_SIZE, 0.0f);
    
    // Initialize internal signals
    internal_.resize(INTERNAL_SIZE, 0.0f);
    
    // Initialize proprioception
    proprioception_.resize(PROPRIOCEPTION_SIZE, 0.0f);
    
    // Audio is intentionally not initialized - reserved for future extension
}

void SensoryPercept::setVision(const std::vector<float>& v) {
    validateVectorSize(v, VISION_SIZE, "vision");
    vision_ = v;
}

void SensoryPercept::setTouch(const std::vector<float>& t) {
    validateVectorSize(t, TOUCH_SIZE, "touch");
    touch_ = t;
}

void SensoryPercept::setInternal(const std::vector<float>& i) {
    validateVectorSize(i, INTERNAL_SIZE, "internal");
    internal_ = i;
}

void SensoryPercept::setProprioception(const std::vector<float>& p) {
    validateVectorSize(p, PROPRIOCEPTION_SIZE, "proprioception");
    proprioception_ = p;
}

void SensoryPercept::setAudio(const std::vector<float>& a) {
    // Audio is optional - accept any size
    audio_ = a;
}

std::vector<float> SensoryPercept::getAllSignals() const {
    // Reserve space for performance
    std::vector<float> all;
    all.reserve(VISION_SIZE + TOUCH_SIZE + INTERNAL_SIZE + PROPRIOCEPTION_SIZE + audio_.size());
    
    // Vision (flattened)
    all.insert(all.end(), vision_.begin(), vision_.end());
    
    // Touch
    all.insert(all.end(), touch_.begin(), touch_.end());
    
    // Internal
    all.insert(all.end(), internal_.begin(), internal_.end());
    
    // Proprioception
    all.insert(all.end(), proprioception_.begin(), proprioception_.end());
    
    // Audio (if present)
    if (!audio_.empty()) {
        all.insert(all.end(), audio_.begin(), audio_.end());
    }
    
    return all;
}

bool SensoryPercept::isValid() const {
    try {
        validateSignalBounds();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

void SensoryPercept::validateSignalBounds() const {
    if (vision_.size() != VISION_SIZE) {
        throw std::runtime_error("Vision buffer size mismatch");
    }
    if (touch_.size() != TOUCH_SIZE) {
        throw std::runtime_error("Touch buffer size mismatch");
    }
    if (internal_.size() != INTERNAL_SIZE) {
        throw std::runtime_error("Internal buffer size mismatch");
    }
    if (proprioception_.size() != PROPRIOCEPTION_SIZE) {
        throw std::runtime_error("Proprioception buffer size mismatch");
    }
}

float SensoryPercept::getVisionIntensity(size_t x, size_t y) const {
    if (x >= VISION_WIDTH || y >= VISION_HEIGHT) {
        throw std::out_of_range("Vision coordinate out of bounds");
    }
    return vision_[y * VISION_WIDTH + x];
}

void SensoryPercept::clear() {
    std::fill(vision_.begin(), vision_.end(), 0.0f);
    std::fill(touch_.begin(), touch_.end(), 0.0f);
    std::fill(internal_.begin(), internal_.end(), 0.0f);
    std::fill(proprioception_.begin(), proprioception_.end(), 0.0f);
    audio_.clear();
    timestamp_ = 0.0;
}

void SensoryPercept::validateVectorSize(const std::vector<float>& vec, size_t expectedSize, 
                                       const std::string& fieldName) const {
    if (vec.size() != expectedSize) {
        throw std::invalid_argument("Vector size mismatch for " + fieldName + ": " +
                                   std::to_string(vec.size()) + " != " + std::to_string(expectedSize));
    }
}

} // namespace nlm
