#include "SensoryPercept.hpp"
#include <stdexcept>
#include <cmath>
#include <iostream>
#include <limits>

#include "constants.hpp"

namespace nlm {

// Helper function for float validation
bool isValidFloat(float value) {
    return !std::isnan(value) && !std::isinf(value) &&
           value >= MIN_VALID_VALUE && value <= MAX_VALID_VALUE;
}

// Helper function for validation error logging
void logValidationError(const std::string& operation, const std::string& details) {
    std::cerr << "[SensoryPercept ERROR] " << operation << ": " << details << std::endl;
}

} // namespace nlm

// Define local constants that could be moved to constants.hpp
namespace {
    constexpr size_t MAX_VISION_WIDTH = 16;
    constexpr size_t MAX_VISION_HEIGHT = 16;
    constexpr size_t MAX_VISION_SIZE = MAX_VISION_WIDTH * MAX_VISION_HEIGHT;
    constexpr float MIN_VALID_VALUE = std::numeric_limits<float>::lowest() * 10.0f;
    constexpr float MAX_VALID_VALUE = std::numeric_limits<float>::max() * 0.1f;
}

SensoryPercept::SensoryPercept()
    : visionWidth_(MAX_VISION_WIDTH)
    , visionHeight_(MAX_VISION_HEIGHT)
    , timestamp_(0.0)
{
    // Validate and allocate memory efficiently
    if (visionWidth_ * visionHeight_ > MAX_VISION_SIZE) {
        throw std::invalid_argument("SensoryPercept: Vision dimensions exceed maximum allowed size");
    }
    
    size_t visionSize = visionWidth_ * visionHeight_;
    vision_.reserve(visionSize);
    vision_.resize(visionSize, 0.0f);
    
    touch_.reserve(MAX_TOUCH_SENSORS);
    touch_.resize(TOUCH_SENSORS, 0.0f);
    
    internal_.reserve(MAX_INTERNAL_SENSORS);
    internal_.resize(INTERNAL_SENSORS, 0.0f);
    
    proprioception_.reserve(MAX_PROPRIOCEPTION_SENSORS);
    proprioception_.resize(PROPRIOCEPTION_SENSORS, 0.0f);
    
    audio_.reserve(MAX_AUDIO_SENSORS);
    audio_.resize(0, 0.0f);
    
    // Validate initial values
    for (float val : vision_) {
        if (!isValidFloat(val)) {
            logValidationError("SensoryPercept constructor", "Invalid vision pixel value in initialization");
        }
    }
}

std::vector<float> SensoryPercept::getAllSignals() const {
    std::vector<float> all;
    
    // Validate output vector size against reasonable limits
    size_t totalExpectedSize = vision_.size() + touch_.size() + internal_.size() + 
                              proprioception_.size() + audio_.size();
    if (totalExpectedSize > MAX_ACTION_HISTORY_SIZE) {
        logValidationError("getAllSignals", "Total signal size exceeds reasonable limit");
        throw std::runtime_error("SensoryPercept: Signal size exceeds acceptable limit");
    }
    
    // Reserve memory for better performance
    all.reserve(totalExpectedSize);
    
    // Vision (flattened) - with bounds checking
    if (vision_.size() > 0) {
        // Check for NaN values in vision data
        for (float val : vision_) {
            if (!isValidFloat(val)) {
                logValidationError("getAllSignals", "Invalid vision pixel value detected");
                // Instead of failing, continue but log the error
            }
        }
        all.insert(all.end(), vision_.begin(), vision_.end());
    }
    
    // Touch - with bounds checking
    if (touch_.size() > 0) {
        for (float val : touch_) {
            if (!isValidFloat(val)) {
                logValidationError("getAllSignals", "Invalid touch sensor value detected");
            }
        }
        all.insert(all.end(), touch_.begin(), touch_.end());
    }
    
    // Internal - with bounds checking
    if (internal_.size() > 0) {
        for (float val : internal_) {
            if (!isValidFloat(val)) {
                logValidationError("getAllSignals", "Invalid internal sensor value detected");
            }
        }
        all.insert(all.end(), internal_.begin(), internal_.end());
    }
    
    // Proprioception - with bounds checking
    if (proprioception_.size() > 0) {
        for (float val : proprioception_) {
            if (!isValidFloat(val)) {
                logValidationError("getAllSignals", "Invalid proprioception sensor value detected");
            }
        }
        all.insert(all.end(), proprioception_.begin(), proprioception_.end());
    }
    
    // Audio - with bounds checking
    if (audio_.size() > 0) {
        for (float val : audio_) {
            if (!isValidFloat(val)) {
                logValidationError("getAllSignals", "Invalid audio sensor value detected");
            }
        }
        all.insert(all.end(), audio_.begin(), audio_.end());
    }
    
    return all;
}
