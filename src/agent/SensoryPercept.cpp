#include "SensoryPercept.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
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

// New utility methods
std::vector<float> SensoryPercept::getNormalizedVision() const {
    if (vision_.empty()) return {};
    
    std::vector<float> normalized = vision_;
    float min_val = *std::min_element(normalized.begin(), normalized.end());
    float max_val = *std::max_element(normalized.begin(), normalized.end());
    
    if (max_val - min_val > std::numeric_limits<float>::epsilon()) {
        float scale = 1.0f / (max_val - min_val);
        for (auto& val : normalized) {
            val = (val - min_val) * scale;
        }
    }
    
    return normalized;
}

float SensoryPercept::getVisionMean() const {
    if (vision_.empty()) return 0.0f;
    float sum = std::accumulate(vision_.begin(), vision_.end(), 0.0f);
    return sum / vision_.size();
}

float SensoryPercept::getVisionStdDev() const {
    if (vision_.empty()) return 0.0f;
    
    float mean = getVisionMean();
    float sum_sq = 0.0f;
    for (float val : vision_) {
        sum_sq += (val - mean) * (val - mean);
    }
    
    return std::sqrt(sum_sq / vision_.size());
}

float SensoryPercept::getTouchMean() const {
    if (touch_.empty()) return 0.0f;
    float sum = std::accumulate(touch_.begin(), touch_.end(), 0.0f);
    return sum / touch_.size();
}

float SensoryPercept::getInternalMean() const {
    if (internal_.empty()) return 0.0f;
    float sum = std::accumulate(internal_.begin(), internal_.end(), 0.0f);
    return sum / internal_.size();
}

float SensoryPercept::getProprioceptionMean() const {
    if (proprioception_.empty()) return 0.0f;
    float sum = std::accumulate(proprioception_.begin(), proprioception_.end(), 0.0f);
    return sum / proprioception_.size();
}

float SensoryPercept::getMaxSignal() const {
    float max_val = std::numeric_limits<float>::lowest();
    
    max_val = std::max(max_val, *std::max_element(vision_.begin(), vision_.end()));
    max_val = std::max(max_val, *std::max_element(touch_.begin(), touch_.end()));
    max_val = std::max(max_val, *std::max_element(internal_.begin(), internal_.end()));
    max_val = std::max(max_val, *std::max_element(proprioception_.begin(), proprioception_.end()));
    
    return max_val;
}

float SensoryPercept::getMinSignal() const {
    float min_val = std::numeric_limits<float>::max();
    
    if (!vision_.empty()) {
        min_val = std::min(min_val, *std::min_element(vision_.begin(), vision_.end()));
    }
    if (!touch_.empty()) {
        min_val = std::min(min_val, *std::min_element(touch_.begin(), touch_.end()));
    }
    if (!internal_.empty()) {
        min_val = std::min(min_val, *std::min_element(internal_.begin(), internal_.end()));
    }
    if (!proprioception_.empty()) {
        min_val = std::min(min_val, *std::min_element(proprioception_.begin(), proprioception_.end()));
    }
    
    return min_val;
}

std::vector<float> SensoryPercept::getTouchSignalIndices() const {
    std::vector<size_t> active_indices;
    for (size_t i = 0; i < touch_.size(); ++i) {
        if (touch_[i] > 0.5f) {
            active_indices.push_back(i);
        }
    }
    return active_indices;
}

std::vector<float> SensoryPercept::getHighActivationVision(float threshold) const {
    std::vector<size_t> high_indices;
    for (size_t i = 0; i < vision_.size(); ++i) {
        if (vision_[i] > threshold) {
            high_indices.push_back(i);
        }
    }
    return high_indices;
}

SensoryPercept SensoryPercept::clone() const {
    SensoryPercept cloned;
    cloned.vision_ = vision_;
    cloned.touch_ = touch_;
    cloned.internal_ = internal_;
    cloned.proprioception_ = proprioception_;
    cloned.audio_ = audio_;
    cloned.timestamp_ = timestamp_;
    return cloned;
}

bool SensoryPercept::validate() const {
    // Check for NaN values
    auto check_nan = [](const std::vector<float>& vec, const std::string& name) {
        for (size_t i = 0; i < vec.size(); ++i) {
            if (std::isnan(vec[i])) {
                return false;
            }
        }
        return true;
    };
    
    if (!check_nan(vision_, "vision")) return false;
    if (!check_nan(touch_, "touch")) return false;
    if (!check_nan(internal_, "internal")) return false;
    if (!check_nan(proprioception_, "proprioception")) return false;
    
    // Check for valid ranges
    auto check_range = [](const std::vector<float>& vec, const std::string& name) {
        for (size_t i = 0; i < vec.size(); ++i) {
            if (vec[i] < 0.0f || vec[i] > 1.0f) {
                return false;
            }
        }
        return true;
    };
    
    if (!check_range(vision_, "vision")) return false;
    if (!check_range(touch_, "touch")) return false;
    if (!check_range(internal_, "internal")) return false;
    if (!check_range(proprioception_, "proprioception")) return false;
    
    return true;
}

std::string SensoryPercept::to_string() const {
    std::stringstream ss;
    ss << "SensoryPercept(";
    ss << "vision=[" << vision_.size() << " values], ";
    ss << "touch=[" << touch_.size() << " values], ";
    ss << "internal=[" << internal_.size() << " values], ";
    ss << "proprioception=[" << proprioception_.size() << " values], ";
    ss << "timestamp=" << timestamp_ << ")";
    return ss.str();
}

} // namespace nlm