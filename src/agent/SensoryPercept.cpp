#include "SensoryPercept.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <stdexcept>

namespace nlm {

namespace SensoryPerceptConfig {
    constexpr size_t DEFAULT_VISION_WIDTH = 16;
    constexpr size_t DEFAULT_VISION_HEIGHT = 16;
    constexpr size_t DEFAULT_TOUCH_SENSORS = 8;
    constexpr size_t DEFAULT_INTERNAL_SENSORS = 4;
    constexpr size_t DEFAULT_PROPRIOCEPTORS = 6;
    constexpr size_t MAX_SENSOR_SIZE = 1024;
    constexpr float MAX_SENSOR_VALUE = 1.0f;
    constexpr double DEFAULT_TIMESTAMP = 0.0;
} // namespace SensoryPerceptConfig

// Validation helper functions
namespace SensoryValidation {
    
    bool validateSensorSize(size_t size, const char* sensorName) {
        if (size > SensoryPerceptConfig::MAX_SENSOR_SIZE) {
            NLM_LOG_WARNING(std::string(sensorName) + " size " + std::to_string(size) + 
                          " exceeds maximum of " + std::to_string(SensoryPerceptConfig::MAX_SENSOR_SIZE));
            return false;
        }
        return true;
    }
    
    bool validateSensorValues(const std::vector<float>& values, const char* sensorName) {
        for (size_t i = 0; i < values.size(); ++i) {
            if (values[i] < 0.0f || values[i] > SensoryPerceptConfig::MAX_SENSOR_VALUE) {
                NLM_LOG_WARNING(std::string(sensorName) + " value at index " + std::to_string(i) + 
                              " (" + std::to_string(values[i]) + ") is out of range [0.0, 1.0]");
                return false;
            }
        }
        return true;
    }
    
    void clampSensorValues(std::vector<float>& values) {
        for (float& value : values) {
            value = std::clamp(value, 0.0f, SensoryPerceptConfig::MAX_SENSOR_VALUE);
        }
    }
    
} // namespace SensoryValidation

SensoryPercept::SensoryPercept()
    : visionWidth_(SensoryPerceptConfig::DEFAULT_VISION_WIDTH)
    , visionHeight_(SensoryPerceptConfig::DEFAULT_VISION_HEIGHT)
    , timestamp_(SensoryPerceptConfig::DEFAULT_TIMESTAMP)
{
    initializeSensors();
}

void SensoryPercept::initializeSensors() {
    // Initialize vision with proper dimensions
    if (!SensoryValidation::validateSensorSize(
        SensoryPerceptConfig::DEFAULT_VISION_WIDTH * SensoryPerceptConfig::DEFAULT_VISION_HEIGHT,
        "Vision")) {
        vision_.resize(0);
        NLM_LOG_ERROR("Failed to initialize vision sensor - invalid dimensions");
        return;
    }
    
    vision_.resize(SensoryPerceptConfig::DEFAULT_VISION_WIDTH * 
                   SensoryPerceptConfig::DEFAULT_VISION_HEIGHT, 0.0f);
    
    // Initialize other sensors with validation
    if (SensoryValidation::validateSensorSize(
        SensoryPerceptConfig::DEFAULT_TOUCH_SENSORS, "Touch")) {
        touch_.resize(SensoryPerceptConfig::DEFAULT_TOUCH_SENSORS, 0.0f);
    }
    
    if (SensoryValidation::validateSensorSize(
        SensoryPerceptConfig::DEFAULT_INTERNAL_SENSORS, "Internal")) {
        internal_.resize(SensoryPerceptConfig::DEFAULT_INTERNAL_SENSORS, 0.0f);
    }
    
    if (SensoryValidation::validateSensorSize(
        SensoryPerceptConfig::DEFAULT_PROPRIOCEPTORS, "Proprioception")) {
        proprioception_.resize(SensoryPerceptConfig::DEFAULT_PROPRIOCEPTORS, 0.0f);
    }
    
    // Audio is intentionally empty for now, but the infrastructure exists for future implementation
    audio_.clear();
}

std::vector<float> SensoryPercept::getAllSignals() const {
    std::vector<float> all;
    
    // Reserve space for performance
    size_t totalSize = vision_.size() + touch_.size() + 
                      internal_.size() + proprioception_.size() + audio_.size();
    all.reserve(totalSize);
    
    // Vision (flattened) - validated during access
    all.insert(all.end(), vision_.begin(), vision_.end());
    
    // Touch - validated during access
    all.insert(all.end(), touch_.begin(), touch_.end());
    
    // Internal - validated during access
    all.insert(all.end(), internal_.begin(), internal_.end());
    
    // Proprioception - validated during access
    all.insert(all.end(), proprioception_.begin(), proprioception_.end());
    
    // Audio - future extension point
    all.insert(all.end(), audio_.begin(), audio_.end());
    
    return all;
}

// Safe sensor access methods with validation
const std::vector<float>& SensoryPercept::getVision() const {
    return vision_;
}

void SensoryPercept::setVision(const std::vector<float>& v) {
    if (!SensoryValidation::validateSensorSize(
        v.size(), "Vision input")) {
        throw std::invalid_argument("Invalid vision sensor size");
    }
    
    if (!SensoryValidation::validateSensorValues(v, "Vision input")) {
        NLM_LOG_WARNING("Vision sensor values out of range, clamping to valid range");
        // Create copy and clamp values
        std::vector<float> clamped = v;
        SensoryValidation::clampSensorValues(clamped);
        vision_ = clamped;
    } else {
        vision_ = v;
    }
}

size_t SensoryPercept::getVisionWidth() const {
    return visionWidth_;
}

size_t SensoryPercept::getVisionHeight() const {
    return visionHeight_;
}

const std::vector<float>& SensoryPercept::getTouch() const {
    return touch_;
}

void SensoryPercept::setTouch(const std::vector<float>& t) {
    if (!SensoryValidation::validateSensorSize(
        t.size(), "Touch input")) {
        throw std::invalid_argument("Invalid touch sensor size");
    }
    
    if (!SensoryValidation::validateSensorValues(t, "Touch input")) {
        NLM_LOG_WARNING("Touch sensor values out of range, clamping to valid range");
        std::vector<float> clamped = t;
        SensoryValidation::clampSensorValues(clamped);
        touch_ = clamped;
    } else {
        touch_ = t;
    }
}

const std::vector<float>& SensoryPercept::getInternal() const {
    return internal_;
}

void SensoryPercept::setInternal(const std::vector<float>& i) {
    if (!SensoryValidation::validateSensorSize(
        i.size(), "Internal input")) {
        throw std::invalid_argument("Invalid internal sensor size");
    }
    
    if (!SensoryValidation::validateSensorValues(i, "Internal input")) {
        NLM_LOG_WARNING("Internal sensor values out of range, clamping to valid range");
        std::vector<float> clamped = i;
        SensoryValidation::clampSensorValues(clamped);
        internal_ = clamped;
    } else {
        internal_ = i;
    }
}

const std::vector<float>& SensoryPercept::getProprioception() const {
    return proprioception_;
}

void SensoryPercept::setProprioception(const std::vector<float>& p) {
    if (!SensoryValidation::validateSensorSize(
        p.size(), "Proprioception input")) {
        throw std::invalid_argument("Invalid proprioception sensor size");
    }
    
    if (!SensoryValidation::validateSensorValues(p, "Proprioception input")) {
        NLM_LOG_WARNING("Proprioception sensor values out of range, clamping to valid range");
        std::vector<float> clamped = p;
        SensoryValidation::clampSensorValues(clamped);
        proprioception_ = clamped;
    } else {
        proprioception_ = p;
    }
}

const std::vector<float>& SensoryPercept::getAudio() const {
    return audio_;
}

void SensoryPercept::setAudio(const std::vector<float>& a) {
    if (!SensoryValidation::validateSensorSize(
        a.size(), "Audio input")) {
        throw std::invalid_argument("Invalid audio sensor size");
    }
    
    if (!SensoryValidation::validateSensorValues(a, "Audio input")) {
        NLM_LOG_WARNING("Audio sensor values out of range, clamping to valid range");
        std::vector<float> clamped = a;
        SensoryValidation::clampSensorValues(clamped);
        audio_ = clamped;
    } else {
        audio_ = a;
    }
}

double SensoryPercept::getTimestamp() const {
    return timestamp_;
}

void SensoryPercept::setTimestamp(double t) {
    timestamp_ = t;
}

} // namespace nlm
