#include "SensoryPercept.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace nlm {

SensoryPercept::SensoryPercept(const SensoryConfig& config)
    : config_(config), timestamp_(0.0), currentTime_(0.0) {
    if (!config_.isValid()) {
        NLM_LOG_ERROR("Invalid SensoryConfig provided - will use defaults");
        config_ = SensoryConfig(); // Reset to safe defaults
    }
    
    updateSignalBuffers();
    NLM_LOG_INFO("SensoryPercept initialized: " + config_.toString());
}

void SensoryPercept::updateSignalBuffers() {
    // Resize vision buffer
    if (config_.visionEnabled) {
        vision_.resize(config_.visionWidth * config_.visionHeight, 0.0f);
    } else {
        vision_.clear();
    }
    
    // Resize touch buffer
    if (config_.touchEnabled) {
        touch_.resize(config_.touchSensors, 0.0f);
    } else {
        touch_.clear();
    }
    
    // Resize internal buffer
    if (config_.internalEnabled) {
        internal_.resize(config_.internalSensors, 0.0f);
    } else {
        internal_.clear();
    }
    
    // Resize proprioception buffer
    if (config_.proprioceptionEnabled) {
        proprioception_.resize(config_.proprioceptionSensors, 0.0f);
    } else {
        proprioception_.clear();
    }
    
    // Resize audio buffer
    if (config_.audioEnabled) {
        audio_.resize(config_.audioSamples, 0.0f);
    } else {
        audio_.clear();
    }
}

std::vector<float> SensoryPercept::getAllSignals() const {
    std::vector<float> all;
    
    // Vision (flattened)
    if (config_.visionEnabled) {
        all.insert(all.end(), vision_.begin(), vision_.end());
    }
    
    // Touch
    if (config_.touchEnabled) {
        all.insert(all.end(), touch_.begin(), touch_.end());
    }
    
    // Internal
    if (config_.internalEnabled) {
        all.insert(all.end(), internal_.begin(), internal_.end());
    }
    
    // Proprioception
    if (config_.proprioceptionEnabled) {
        all.insert(all.end(), proprioception_.begin(), proprioception_.end());
    }
    
    // Audio
    if (config_.audioEnabled) {
        all.insert(all.end(), audio_.begin(), audio_.end());
    }
    
    return all;
}

bool SensoryPercept::validateSignalDimensions() const {
    bool isValid = true;
    
    // Check vision dimensions
    if (config_.visionEnabled) {
        if (vision_.size() != config_.visionWidth * config_.visionHeight) {
            NLM_LOG_WARNING("Vision signal size mismatch: expected " + 
                          std::to_string(config_.visionWidth * config_.visionHeight) +
                          ", got " + std::to_string(vision_.size()));
            isValid = false;
        }
    }
    
    // Check touch dimensions
    if (config_.touchEnabled) {
        if (touch_.size() != config_.touchSensors) {
            NLM_LOG_WARNING("Touch signal size mismatch: expected " + 
                          std::to_string(config_.touchSensors) +
                          ", got " + std::to_string(touch_.size()));
            isValid = false;
        }
    }
    
    // Check internal dimensions
    if (config_.internalEnabled) {
        if (internal_.size() != config_.internalSensors) {
            NLM_LOG_WARNING("Internal signal size mismatch: expected " + 
                          std::to_string(config_.internalSensors) +
                          ", got " + std::to_string(internal_.size()));
            isValid = false;
        }
    }
    
    // Check proprioception dimensions
    if (config_.proprioceptionEnabled) {
        if (proprioception_.size() != config_.proprioceptionSensors) {
            NLM_LOG_WARNING("Proprioception signal size mismatch: expected " + 
                          std::to_string(config_.proprioceptionSensors) +
                          ", got " + std::to_string(proprioception_.size()));
            isValid = false;
        }
    }
    
    // Check audio dimensions
    if (config_.audioEnabled) {
        if (audio_.size() != config_.audioSamples) {
            NLM_LOG_WARNING("Audio signal size mismatch: expected " + 
                          std::to_string(config_.audioSamples) +
                          ", got " + std::to_string(audio_.size()));
            isValid = false;
        }
    }
    
    return isValid;
}

void SensoryPercept::reset() {
    std::fill(vision_.begin(), vision_.end(), 0.0f);
    std::fill(touch_.begin(), touch_.end(), 0.0f);
    std::fill(internal_.begin(), internal_.end(), 0.0f);
    std::fill(proprioception_.begin(), proprioception_.end(), 0.0f);
    std::fill(audio_.begin(), audio_.end(), 0.0f);
    
    timestamp_ = 0.0;
    
    NLM_LOG_INFO("SensoryPercept reset - all signals zeroed");
}

void SensoryPercept::updateSignals(const std::vector<float>& visionData,
                                   const std::vector<float>& touchData,
                                   const std::vector<float>& internalData,
                                   const std::vector<float>& proprioceptionData,
                                   const std::vector<float>& audioData) {
    bool updated = false;
    
    // Update vision
    if (config_.visionEnabled && !visionData.empty()) {
        if (visionData.size() == vision_.size()) {
            vision_ = visionData;
            updated = true;
        } else if (config_.enableDebugLogging) {
            NLM_LOG_WARNING("Vision data size mismatch during update");
        }
    }
    
    // Update touch
    if (config_.touchEnabled && !touchData.empty()) {
        if (touchData.size() == touch_.size()) {
            touch_ = touchData;
            updated = true;
        } else if (config_.enableDebugLogging) {
            NLM_LOG_WARNING("Touch data size mismatch during update");
        }
    }
    
    // Update internal
    if (config_.internalEnabled && !internalData.empty()) {
        if (internalData.size() == internal_.size()) {
            internal_ = internalData;
            updated = true;
        } else if (config_.enableDebugLogging) {
            NLM_LOG_WARNING("Internal data size mismatch during update");
        }
    }
    
    // Update proprioception
    if (config_.proprioceptionEnabled && !proprioceptionData.empty()) {
        if (proprioceptionData.size() == proprioception_.size()) {
            proprioception_ = proprioceptionData;
            updated = true;
        } else if (config_.enableDebugLogging) {
            NLM_LOG_WARNING("Proprioception data size mismatch during update");
        }
    }
    
    // Update audio
    if (config_.audioEnabled && !audioData.empty()) {
        if (audioData.size() == audio_.size()) {
            audio_ = audioData;
            updated = true;
        } else if (config_.enableDebugLogging) {
            NLM_LOG_WARNING("Audio data size mismatch during update");
        }
    }
    
    if (updated && config_.enableTimestamps) {
        setTimestamp(currentTime_);
    }
}

std::string SensoryPercept::getSignalInfo() const {
    std::string info = "SensoryPercept: ";
    
    if (config_.visionEnabled) {
        info += "Vision:" + std::to_string(config_.visionWidth) + "x" + 
                std::to_string(config_.visionHeight) + "( " + 
                std::to_string(vision_.size()) + " values), ";
    }
    
    if (config_.touchEnabled) {
        info += "Touch:" + std::to_string(touch_.size()) + " values, ";
    }
    
    if (config_.internalEnabled) {
        info += "Internal:" + std::to_string(internal_.size()) + " values, ";
    }
    
    if (config_.proprioceptionEnabled) {
        info += "Proprioception:" + std::to_string(proprioception_.size()) + " values, ";
    }
    
    if (config_.audioEnabled) {
        info += "Audio:" + std::to_string(config_.audioSampleRate) + "Hz, " +
                std::to_string(audio_.size()) + " samples, ";
    }
    
    info += "Total:" + std::to_string(getAllSignals().size()) + " signals";
    return info;
}

} // namespace nlm
