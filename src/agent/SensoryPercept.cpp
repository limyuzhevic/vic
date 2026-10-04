#include "SensoryPercept.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
    , configuredVisionWidth_(16)
    , configuredVisionHeight_(16)
    , configuredTouchSize_(8)
    , configuredInternalSize_(4)
    , configuredProprioceptionSize_(6)
    , configuredAudioSize_(0)
{
    initializeDefaultDimensions();
}

SensoryPercept::SensoryPercept(size_t visionWidth, size_t visionHeight, 
                               size_t touchSize, size_t internalSize,
                               size_t proprioceptionSize, size_t audioSize)
    : visionWidth_(visionWidth)
    , visionHeight_(visionHeight)
    , timestamp_(0.0)
    , configuredVisionWidth_(visionWidth)
    , configuredVisionHeight_(visionHeight)
    , configuredTouchSize_(touchSize)
    , configuredInternalSize_(internalSize)
    , configuredProprioceptionSize_(proprioceptionSize)
    , configuredAudioSize_(audioSize)
{
    resizeVectors();
}

void SensoryPercept::configure(size_t visionWidth, size_t visionHeight,
                               size_t touchSize, size_t internalSize,
                               size_t proprioceptionSize, size_t audioSize) {
    configuredVisionWidth_ = visionWidth;
    configuredVisionHeight_ = visionHeight;
    configuredTouchSize_ = touchSize;
    configuredInternalSize_ = internalSize;
    configuredProprioceptionSize_ = proprioceptionSize;
    configuredAudioSize_ = audioSize;
    
    resizeVectors();
    
    NLM_LOG_INFO("SensoryPercept configured: vision=" + std::to_string(visionWidth) + "x" + std::to_string(visionHeight) +
                 ", touch=" + std::to_string(touchSize) + ", internal=" + std::to_string(internalSize) +
                 ", proprioception=" + std::to_string(proprioceptionSize) + ", audio=" + std::to_string(audioSize));
}

void SensoryPercept::initializeDefaultDimensions() {
    vision_.resize(16 * 16, 0.0f);
    touch_.resize(8, 0.0f);
    internal_.resize(4, 0.0f);
    proprioception_.resize(6, 0.0f);
    audio_.resize(0, 0.0f);
}

size_t SensoryPercept::getTotalSensoryInputSize() const {
    return getVisionArea() + getTouchSize() + getInternalSize() + getProprioceptionSize() + getAudioSize();
}

bool SensoryPercept::validate() const {
    bool valid = true;
    
    if (vision_.size() != getVisionArea()) {
        NLM_LOG_WARNING("Vision size mismatch: expected " + std::to_string(getVisionArea()) + 
                       ", got " + std::to_string(vision_.size()));
        valid = false;
    }
    
    if (touch_.size() != getTouchSize()) {
        NLM_LOG_WARNING("Touch size mismatch: expected " + std::to_string(getTouchSize()) + 
                       ", got " + std::to_string(touch_.size()));
        valid = false;
    }
    
    if (internal_.size() != getInternalSize()) {
        NLM_LOG_WARNING("Internal size mismatch: expected " + std::to_string(getInternalSize()) + 
                       ", got " + std::to_string(internal_.size()));
        valid = false;
    }
    
    if (proprioception_.size() != getProprioceptionSize()) {
        NLM_LOG_WARNING("Proprioception size mismatch: expected " + std::to_string(getProprioceptionSize()) + 
                       ", got " + std::to_string(proprioception_.size()));
        valid = false;
    }
    
    if (audio_.size() != getAudioSize()) {
        NLM_LOG_WARNING("Audio size mismatch: expected " + std::to_string(getAudioSize()) + 
                       ", got " + std::to_string(audio_.size()));
        valid = false;
    }
    
    return valid;
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

} // namespace nlm
