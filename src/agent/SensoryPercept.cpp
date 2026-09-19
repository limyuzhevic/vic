#include "SensoryPercept.hpp"
#include "../sensory/Vision.hpp"
#include "../sensory/Audio.hpp"
#include "../sensory/InternalSignals.hpp"
#include <algorithm>

namespace nlm {

SensoryPercept::SensoryPercept() : timestamp_(0.0) {}

const Vision& SensoryPercept::getVision() const {
    return vision_;
}

void SensoryPercept::setVision(const Vision& vision) {
    vision_ = vision;
}

const Audio& SensoryPercept::getTouch() const {
    return touch_;
}

void SensoryPercept::setTouch(const Audio& touch) {
    touch_ = touch;
}

const InternalSignals& SensoryPercept::getInternal() const {
    return internal_;
}

void SensoryPercept::setInternal(const InternalSignals& internal) {
    internal_ = internal;
}

const std::vector<float>& SensoryPercept::getProprioception() const {
    return proprioception_;
}

void SensoryPercept::setProprioception(const std::vector<float>& proprioception) {
    proprioception_ = proprioception;
}

const Audio& SensoryPercept::getAudio() const {
    return audio_;
}

void SensoryPercept::setAudio(const Audio& audio) {
    audio_ = audio;
}

std::vector<float> SensoryPercept::getAllSignals() const {
    std::vector<float> allSignals;
    
    // Add vision data
    auto visionData = vision_.getData();
    allSignals.insert(allSignals.end(), visionData.begin(), visionData.end());
    
    // Add touch data
    auto touchData = touch_.getData();
    allSignals.insert(allSignals.end(), touchData.begin(), touchData.end());
    
    // Add internal signals
    internal_.getAllSignals(allSignals);
    
    // Add proprioception
    allSignals.insert(allSignals.end(), proprioception_.begin(), proprioception_.end());
    
    // Add audio data
    auto audioData = audio_.getData();
    allSignals.insert(allSignals.end(), audioData.begin(), audioData.end());
    
    return allSignals;
}

Timestamp SensoryPercept::getTimestamp() const {
    return timestamp_;
}

void SensoryPercept::setTimestamp(Timestamp timestamp) {
    timestamp_ = timestamp;
}

void SensoryPercept::reset() {
    vision_.reset();
    touch_.reset();
    internal_.reset();
    proprioception_.clear();
    audio_.reset();
    timestamp_ = 0.0;
}

size_t SensoryPercept::getVisionWidth() const {
    return vision_.getDimensions();
}

size_t SensoryPercept::getVisionHeight() const {
    return 1; // Simplified for now, should be actual height
}

} // namespace nlm