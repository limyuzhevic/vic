#include "SensoryPercept.hpp"

namespace nlm {

SensoryPercept::SensoryPercept(size_t visionWidth, size_t visionHeight)
    : visionWidth_(visionWidth)
    , visionHeight_(visionHeight)
    , timestamp_(0.0)
{
    vision_.resize(visionWidth_ * visionHeight_, 0.0f);
    touch_.resize(8, 0.0f);
    internal_.resize(4, 0.0f);
    proprioception_.resize(6, 0.0f);
    audio_.resize(0, 0.0f);
}

// Default constructor for backward compatibility
SensoryPercept::SensoryPercept()
    : SensoryPercept(16, 16) {}

SensoryPerceptBuilder::SensoryPerceptBuilder& SensoryPerceptBuilder::addVision(const std::vector<float>& data) {
    visionData_ = data;
    return *this;
}

SensoryPerceptBuilder::SensoryPerceptBuilder& SensoryPerceptBuilder::addTouch(const std::vector<float>& data) {
    touchData_ = data;
    return *this;
}

SensoryPerceptBuilder::SensoryPerceptBuilder& SensoryPerceptBuilder::addInternal(const std::vector<float>& data) {
    internalData_ = data;
    return *this;
}

SensoryPerceptBuilder::SensoryPerceptBuilder& SensoryPerceptBuilder::addProprioception(const std::vector<float>& data) {
    proprioceptionData_ = data;
    return *this;
}

SensoryPerceptBuilder::SensoryPerceptBuilder& SensoryPerceptBuilder::addAudio(const std::vector<float>& data) {
    audioData_ = data;
    return *this;
}

SensoryPerceptBuilder::SensoryPerceptBuilder& SensoryPerceptBuilder::setDimensions(size_t width, size_t height) {
    visionWidth_ = width;
    visionHeight_ = height;
    return *this;
}

SensoryPercept SensoryPerceptBuilder::build() {
    if (!validate()) {
        throw std::runtime_error("Cannot build SensoryPercept: invalid builder state");
    }
    
    SensoryPercept percept(visionWidth_, visionHeight_);
    
    if (!visionData_.empty()) {
        percept.setVision(visionData_);
    }
    if (!touchData_.empty()) {
        percept.setTouch(touchData_);
    }
    if (!internalData_.empty()) {
        percept.setInternal(internalData_);
    }
    if (!proprioceptionData_.empty()) {
        percept.setProprioception(proprioceptionData_);
    }
    if (!audioData_.empty()) {
        percept.setAudio(audioData_);
    }
    
    return percept;
}

bool SensoryPerceptBuilder::validate() const {
    if (visionData_.size() != visionWidth_ * visionHeight_) {
        return false;
    }
    if (touchData_.size() > 8) return false;
    if (internalData_.size() > 4) return false;
    if (proprioceptionData_.size() > 6) return false;
    return true;
}

SensoryPercept SensoryPercept::create() {
    return SensoryPerceptBuilder().build();
}

bool SensoryPercept::validate() const {
    if (validateVision() && validateTouch() && validateInternal() && 
        validateProprioception() && validateAudio()) {
        return true;
    }
    return false;
}

bool SensoryPercept::validateVision() const {
    size_t expectedSize = visionWidth_ * visionHeight_;
    if (vision_.size() != expectedSize) {
        return false;
    }
    for (float value : vision_) {
        if (std::isnan(value) || value < 0.0f || value > 1.0f) {
            return false;
        }
    }
    return true;
}

bool SensoryPercept::validateTouch() const {
    if (touch_.size() > 8) return false;
    for (float value : touch_) {
        if (std::isnan(value) || value < 0.0f || value > 1.0f) {
            return false;
        }
    }
    return true;
}

bool SensoryPercept::validateInternal() const {
    if (internal_.size() > 4) return false;
    for (float value : internal_) {
        if (std::isnan(value) || value < -1.0f || value > 1.0f) {
            return false;
        }
    }
    return true;
}

bool SensoryPercept::validateProprioception() const {
    if (proprioception_.size() > 6) return false;
    for (float value : proprioception_) {
        if (std::isnan(value) || value < -1.0f || value > 1.0f) {
            return false;
        }
    }
    return true;
}

bool SensoryPercept::validateAudio() const {
    for (float value : audio_) {
        if (std::isnan(value) || value < 0.0f || value > 1.0f) {
            return false;
        }
    }
    return true;
}

std::string SensoryPercept::to_string() const {
    std::ostringstream oss;
    oss << "SensoryPercept[width=" << visionWidth_ << ", height=" << visionHeight_ 
        << ", vision_size=" << vision_.size() 
        << ", touch_size=" << touch_.size() 
        << ", internal_size=" << internal_.size() 
        << ", proprioception_size=" << proprioception_.size() 
        << ", audio_size=" << audio_.size() 
        << ", timestamp=" << timestamp_ << "]";
    return oss.str();
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

void SensoryPercept::setVision(const std::vector<float>& v) {
    if (v.size() != visionWidth_ * visionHeight_) {
        throw std::invalid_argument("Vision data size does not match vision dimensions");
    }
    vision_ = v;
}

} // namespace nlm
