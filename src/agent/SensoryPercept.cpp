#include "SensoryPercept.hpp"
#include <algorithm>
#include <stdexcept>

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

void SensoryPercept::setVision(const std::vector<float>& vision) {
    if (vision.size() > vision_.capacity()) {
        NLM_LOG_WARNING("SensoryPercept::setVision: Vision data exceeds allocated capacity. Resizing...");
        vision_.resize(vision.size(), 0.0f);
    }
    vision_ = vision;
}

void SensoryPercept::setTouch(const std::vector<float>& touch) {
    if (touch.size() > touch_.capacity()) {
        NLM_LOG_WARNING("SensoryPercept::setTouch: Touch data exceeds allocated capacity. Resizing...");
        touch_.resize(touch.size(), 0.0f);
    }
    touch_ = touch;
}

void SensoryPercept::setInternal(const std::vector<float>& internal) {
    if (internal.size() > internal_.capacity()) {
        NLM_LOG_WARNING("SensoryPercept::setInternal: Internal data exceeds allocated capacity. Resizing...");
        internal_.resize(internal.size(), 0.0f);
    }
    internal_ = internal;
}

void SensoryPercept::setProprioception(const std::vector<float>& proprioception) {
    if (proprioception.size() > proprioception_.capacity()) {
        NLM_LOG_WARNING("SensoryPercept::setProprioception: Proprioception data exceeds allocated capacity. Resizing...");
        proprioception_.resize(proprioception.size(), 0.0f);
    }
    proprioception_ = proprioception;
}

void SensoryPercept::setAudio(const std::vector<float>& audio) {
    if (audio.size() > audio_.capacity()) {
        NLM_LOG_WARNING("SensoryPercept::setAudio: Audio data exceeds allocated capacity. Resizing...");
        audio_.resize(audio.size(), 0.0f);
    }
    audio_ = audio;
}

std::vector<float> SensoryPercept::normalizeVision() const {
    if (vision_.empty()) {
        return {};
    }
    
    float max_val = *std::max_element(vision_.begin(), vision_.end());
    float min_val = *std::min_element(vision_.begin(), vision_.end());
    
    if (std::abs(max_val - min_val) < 1e-6f) {
        return vision_;  // All values are the same
    }
    
    std::vector<float> normalized;
    normalized.reserve(vision_.size());
    
    for (float val : vision_) {
        normalized.push_back((val - min_val) / (max_val - min_val));
    }
    
    return normalized;
}

float SensoryPercept::getVisionMax() const {
    if (vision_.empty()) {
        return 0.0f;
    }
    return *std::max_element(vision_.begin(), vision_.end());
}

float SensoryPercept::getVisionMin() const {
    if (vision_.empty()) {
        return 0.0f;
    }
    return *std::min_element(vision_.begin(), vision_.end());
}

float SensoryPercept::getVisionMean() const {
    if (vision_.empty()) {
        return 0.0f;
    }
    float sum = std::accumulate(vision_.begin(), vision_.end(), 0.0f);
    return sum / vision_.size();
}

float SensoryPercept::getVisionVariance() const {
    if (vision_.empty()) {
        return 0.0f;
    }
    
    float mean = getVisionMean();
    float sum_sq = 0.0f;
    for (float val : vision_) {
        sum_sq += (val - mean) * (val - mean);
    }
    return sum_sq / vision_.size();
}

std::vector<float> SensoryPercept::getNormalizedSignals() const {
    std::vector<float> all = getAllSignals();
    
    if (all.empty()) {
        return all;
    }
    
    float max_val = *std::max_element(all.begin(), all.end());
    float min_val = *std::min_element(all.begin(), all.end());
    
    if (std::abs(max_val - min_val) < 1e-6f) {
        return all;  // All values are the same
    }
    
    std::vector<float> normalized;
    normalized.reserve(all.size());
    
    for (float val : all) {
        normalized.push_back((val - min_val) / (max_val - min_val));
    }
    
    return normalized;
}

bool SensoryPercept::isValid() const {
    // Check if data sizes are within expected ranges
    if (vision_.size() > 1024) {  // Allow some flexibility
        return false;
    }
    
    if (touch_.size() > 32) {
        return false;
    }
    
    if (internal_.size() > 16) {
        return false;
    }
    
    if (proprioception_.size() > 32) {
        return false;
    }
    
    // Check for NaN or infinite values
    for (float val : vision_) {
        if (std::isnan(val) || std::isinf(val)) {
            return false;
        }
    }
    
    for (float val : touch_) {
        if (std::isnan(val) || std::isinf(val)) {
            return false;
        }
    }
    
    for (float val : internal_) {
        if (std::isnan(val) || std::isinf(val)) {
            return false;
        }
    }
    
    for (float val : proprioception_) {
        if (std::isnan(val) || std::isinf(val)) {
            return false;
        }
    }
    
    return true;
}

void SensoryPercept::validateAndNormalize() {
    // Validate and normalize all signals
    vision_ = normalizeVision();
    
    // Normalize other signals
    if (!touch_.empty()) {
        float max_val = *std::max_element(touch_.begin(), touch_.end());
        float min_val = *std::min_element(touch_.begin(), touch_.end());
        
        if (std::abs(max_val - min_val) >= 1e-6f) {
            for (size_t i = 0; i < touch_.size(); ++i) {
                touch_[i] = (touch_[i] - min_val) / (max_val - min_val);
            }
        }
    }
    
    if (!internal_.empty()) {
        float max_val = *std::max_element(internal_.begin(), internal_.end());
        float min_val = *std::min_element(internal_.begin(), internal_.end());
        
        if (std::abs(max_val - min_val) >= 1e-6f) {
            for (size_t i = 0; i < internal_.size(); ++i) {
                internal_[i] = (internal_[i] - min_val) / (max_val - min_val);
            }
        }
    }
    
    if (!proprioception_.empty()) {
        float max_val = *std::max_element(proprioception_.begin(), proprioception_.end());
        float min_val = *std::min_element(proprioception_.begin(), proprioception_.end());
        
        if (std::abs(max_val - min_val) >= 1e-6f) {
            for (size_t i = 0; i < proprioception_.size(); ++i) {
                proprioception_[i] = (proprioception_[i] - min_val) / (max_val - min_val);
            }
        }
    }
    
    if (!audio_.empty()) {
        float max_val = *std::max_element(audio_.begin(), audio_.end());
        float min_val = *std::min_element(audio_.begin(), audio_.end());
        
        if (std::abs(max_val - min_val) >= 1e-6f) {
            for (size_t i = 0; i < audio_.size(); ++i) {
                audio_[i] = (audio_[i] - min_val) / (max_val - min_val);
            }
        }
    }
}

void SensoryPercept::clampValues(float min_val, float max_val) {
    // Clamp all values to specified range
    for (float& val : vision_) {
        val = std::clamp(val, min_val, max_val);
    }
    
    for (float& val : touch_) {
        val = std::clamp(val, min_val, max_val);
    }
    
    for (float& val : internal_) {
        val = std::clamp(val, min_val, max_val);
    }
    
    for (float& val : proprioception_) {
        val = std::clamp(val, min_val, max_val);
    }
    
    for (float& val : audio_) {
        val = std::clamp(val, min_val, max_val);
    }
}

std::vector<float> SensoryPercept::extractFeatures() const {
    std::vector<float> features;
    
    // Basic statistical features
    features.push_back(getVisionMax());
    features.push_back(getVisionMin());
    features.push_back(getVisionMean());
    features.push_back(getVisionVariance());
    
    features.push_back(*std::max_element(touch_.begin(), touch_.end()));
    features.push_back(*std::min_element(touch_.begin(), touch_.end()));
    
    features.push_back(*std::max_element(internal_.begin(), internal_.end()));
    features.push_back(*std::min_element(internal_.begin(), internal_.end()));
    
    features.push_back(*std::max_element(proprioception_.begin(), proprioception_.end()));
    features.push_back(*std::min_element(proprioception_.begin(), proprioception_.end()));
    
    features.push_back(*std::max_element(audio_.begin(), audio_.end()));
    features.push_back(*std::min_element(audio_.begin(), audio_.end()));
    
    // Texture features (simple approximations)
    float vision_contrast = getVisionVariance();
    features.push_back(vision_contrast);
    
    float touch_variance = 0.0f;
    if (!touch_.empty()) {
        float touch_mean = std::accumulate(touch_.begin(), touch_.end(), 0.0f) / touch_.size();
        for (float val : touch_) {
            touch_variance += (val - touch_mean) * (val - touch_mean);
        }
        touch_variance /= touch_.size();
    }
    features.push_back(touch_variance);
    
    return features;
}

void SensoryPercept::addNoise(float noise_level) {
    // Add Gaussian noise to all signals
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> distribution(0.0f, noise_level);
    
    for (float& val : vision_) {
        val += distribution(gen);
    }
    
    for (float& val : touch_) {
        val += distribution(gen);
    }
    
    for (float& val : internal_) {
        val += distribution(gen);
    }
    
    for (float& val : proprioception_) {
        val += distribution(gen);
    }
    
    for (float& val : audio_) {
        val += distribution(gen);
    }
}

void SensoryPercept::scaleAll(float scale_factor) {
    // Scale all signals by a factor
    for (float& val : vision_) {
        val *= scale_factor;
    }
    
    for (float& val : touch_) {
        val *= scale_factor;
    }
    
    for (float& val : internal_) {
        val *= scale_factor;
    }
    
    for (float& val : proprioception_) {
        val *= scale_factor;
    }
    
    for (float& val : audio_) {
        val *= scale_factor;
    }
}

void SensoryPercept::offsetAll(float offset) {
    // Offset all signals by a value
    for (float& val : vision_) {
        val += offset;
    }
    
    for (float& val : touch_) {
        val += offset;
    }
    
    for (float& val : internal_) {
        val += offset;
    }
    
    for (float& val : proprioception_) {
        val += offset;
    }
    
    for (float& val : audio_) {
        val += offset;
    }
}

std::string SensoryPercept::toString() const {
    std::string result = "SensoryPercept(\n";
    result += "  Vision: [";
    for (size_t i = 0; i < vision_.size(); ++i) {
        result += std::to_string(vision_[i]);
        if (i < vision_.size() - 1) {
            result += ", ";
        }
    }
    result += "]\n";
    
    result += "  Touch: [";
    for (size_t i = 0; i < touch_.size(); ++i) {
        result += std::to_string(touch_[i]);
        if (i < touch_.size() - 1) {
            result += ", ";
        }
    }
    result += "]\n";
    
    result += "  Internal: [";
    for (size_t i = 0; i < internal_.size(); ++i) {
        result += std::to_string(internal_[i]);
        if (i < internal_.size() - 1) {
            result += ", ";
        }
    }
    result += "]\n";
    
    result += "  Proprioception: [";
    for (size_t i = 0; i < proprioception_.size(); ++i) {
        result += std::to_string(proprioception_[i]);
        if (i < proprioception_.size() - 1) {
            result += ", ";
        }
    }
    result += "]\n";
    
    result += "  Timestamp: " + std::to_string(timestamp_) + "\n";
    result += ")";
    
    return result;
}

} // namespace nlm
