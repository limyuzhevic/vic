#include "SensoryPercept.hpp"

namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
{
    initialize();
}

void SensoryPercept::initialize() {
    vision_.resize(16 * 16, 0.0f);
    touch_.resize(8, 0.0f);
    internal_.resize(4, 0.0f);
    proprioception_.resize(6, 0.0f);
    audio_.resize(0, 0.0f);
}

std::vector<float> SensoryPercept::getAllSignals() const {
    std::vector<float> all;
    
    // Validate dimensions before proceeding
    validateDimensions();
    
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

void SensoryPercept::validateDimensions() const {
    static const size_t EXPECTED_VISION_SIZE = 16 * 16;  // 256
    static const size_t EXPECTED_TOUCH_SIZE = 8;
    static const size_t EXPECTED_INTERNAL_SIZE = 4;
    static const size_t EXPECTED_PROPrioception_SIZE = 6;
    
    if (vision_.size() != EXPECTED_VISION_SIZE) {
        throw std::runtime_error("Vision vector size mismatch: expected " +
                                 std::to_string(EXPECTED_VISION_SIZE) +
                                 ", got " + std::to_string(vision_.size()));
    }
    if (touch_.size() != EXPECTED_TOUCH_SIZE) {
        throw std::runtime_error("Touch vector size mismatch: expected " +
                                 std::to_string(EXPECTED_TOUCH_SIZE) +
                                 ", got " + std::to_string(touch_.size()));
    }
    if (internal_.size() != EXPECTED_INTERNAL_SIZE) {
        throw std::runtime_error("Internal vector size mismatch: expected " +
                                 std::to_string(EXPECTED_INTERNAL_SIZE) +
                                 ", got " + std::to_string(internal_.size()));
    }
    if (proprioception_.size() != EXPECTED_PROPrioception_SIZE) {
        throw std::runtime_error("Proprioception vector size mismatch: expected " +
                                 std::to_string(EXPECTED_PROPrioception_SIZE) +
                                 ", got " + std::to_string(proprioception_.size()));
    }
}

void SensoryPercept::reset() {
    initialize();
    timestamp_ = 0.0;
}

std::string SensoryPercept::getTypeDescription() const {
    std::string desc = "SensoryPercept";
    
    if (!vision_.empty()) desc += "[Vision:" + std::to_string(vision_.size()) + "]";
    if (!touch_.empty()) desc += "[Touch:" + std::to_string(touch_.size()) + "]";
    if (!internal_.empty()) desc += "[Internal:" + std::to_string(internal_.size()) + "]";
    if (!proprioception_.empty()) desc += "[Proprioception:" + std::to_string(proprioception_.size()) + "]";
    if (!audio_.empty()) desc += "[Audio:" + std::to_string(audio_.size()) + "]";
    
    if (isEmpty()) {
        desc += "[Empty]";
    }
    
    return desc;
}

bool SensoryPercept::isEmpty() const {
    return vision_.empty() && touch_.empty() && internal_.empty() && 
           proprioception_.empty() && audio_.empty();
}

SensoryPercept& SensoryPercept::operator=(const SensoryPercept& other) {
    if (this != &other) {
        vision_ = other.vision_;
        touch_ = other.touch_;
        internal_ = other.internal_;
        proprioception_ = other.proprioception_;
        audio_ = other.audio_;
        timestamp_ = other.timestamp_;
    }
    return *this;
}

SensoryPercept& SensoryPercept::operator=(SensoryPercept&& other) {
    if (this != &other) {
        vision_ = std::move(other.vision_);
        touch_ = std::move(other.touch_);
        internal_ = std::move(other.internal_);
        proprioception_ = std::move(other.proprioception_);
        audio_ = std::move(other.audio_);
        timestamp_ = other.timestamp_;
    }
    return *this;
}

size_t SensoryPercept::getTotalSignalCount() const {
    return vision_.size() + touch_.size() + internal_.size() + 
           proprioception_.size() + audio_.size();
}