#include "SensoryPercept.hpp"
#include <algorithm>
#include <numeric>
#include <stdexcept>

namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
{
    // Initialize with proper sizes
    vision_.resize(visionWidth_ * visionHeight_, 0.0f);
    touch_.resize(8, 0.0f);
    internal_.resize(4, 0.0f);
    proprioception_.resize(6, 0.0f);
    audio_.resize(0, 0.0f);  // Empty by default - can be resized if audio is enabled
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

// Validation and helper functions
void SensoryPercept::validateVisionSize() const {
    if (vision_.size() != visionWidth_ * visionHeight_) {
        throw std::runtime_error("Vision vector size mismatch with vision dimensions");
    }
}

void SensoryPercept::validateTouchSize() const {
    if (touch_.size() != 8) {
        throw std::runtime_error("Touch vector size should be exactly 8");
    }
}

void SensoryPercept::validateInternalSize() const {
    if (internal_.size() != 4) {
        throw std::runtime_error("Internal vector size should be exactly 4");
    }
}

void SensoryPercept::validateProprioceptionSize() const {
    if (proprioception_.size() != 6) {
        throw std::runtime_error("Proprioception vector size should be exactly 6");
    }
}

// Enhanced interface methods with validation
void SensoryPercept::setVisionWithValidation(const std::vector<float>& v) {
    if (v.size() != visionWidth_ * visionHeight_) {
        throw std::invalid_argument("Vision vector size must match vision dimensions");
    }
    vision_ = v;
}

void SensoryPercept::setVisionGrid(size_t width, size_t height, const std::vector<float>& grid) {
    visionWidth_ = width;
    visionHeight_ = height;
    vision_.resize(width * height, 0.0f);
    
    size_t copySize = std::min(grid.size(), vision_.size());
    std::copy(grid.begin(), grid.begin() + copySize, vision_.begin());
}

void SensoryPercept::addNoise(float noiseLevel) {
    if (noiseLevel < 0.0f || noiseLevel > 1.0f) {
        throw std::invalid_argument("Noise level must be between 0.0 and 1.0");
    }
    
    // Add Gaussian noise to vision data
    std::vector<float> noisyVision = vision_;
    for (size_t i = 0; i < noisyVision.size(); ++i) {
        // Simple noise generation
        float noise = static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f;
        noisyVision[i] += noise * noiseLevel;
        // Clamp to valid range
        noisyVision[i] = std::max(0.0f, std::min(1.0f, noisyVision[i]));
    }
    vision_ = noisyVision;
}

float SensoryPercept::getVisionSum() const {
    return std::accumulate(vision_.begin(), vision_.end(), 0.0f);
}

float SensoryPercept::getVisionMax() const {
    return *std::max_element(vision_.begin(), vision_.end());
}

float SensoryPercept::getVisionAverage() const {
    if (vision_.empty()) return 0.0f;
    return getVisionSum() / vision_.size();
}

std::vector<float> SensoryPercept::getVisionHistogram(size_t bins) const {
    std::vector<float> histogram(bins, 0.0f);
    if (vision_.empty()) return histogram;
    
    float binWidth = 1.0f / bins;
    for (float val : vision_) {
        size_t binIndex = static_cast<size_t>(val / binWidth);
        if (binIndex >= bins) binIndex = bins - 1;
        histogram[binIndex]++;
    }
    
    // Normalize to probabilities
    float total = std::accumulate(histogram.begin(), histogram.end(), 0.0f);
    if (total > 0) {
        for (auto& count : histogram) {
            count /= total;
        }
    }
    
    return histogram;
}

// Audio-related methods (when audio is enabled)
void SensoryPercept::enableAudio(size_t samples, float sampleRate) {
    audio_.resize(samples, 0.0f);
    audioSampleRate_ = sampleRate;
}

void SensoryPercept::addAudioSample(float sample) {
    if (!audio_.empty()) {
        static size_t writePos = 0;
        audio_[writePos++] = sample;
        if (writePos >= audio_.size()) writePos = 0;
    }
}

std::vector<float> SensoryPercept::getAudioSamples() const {
    return audio_;
}

size_t SensoryPercept::getAudioSampleRate() const {
    return audioSampleRate_;
}

// Reset functionality
void SensoryPercept::clear() {
    std::fill(vision_.begin(), vision_.end(), 0.0f);
    std::fill(touch_.begin(), touch_.end(), 0.0f);
    std::fill(internal_.begin(), internal_.end(), 0.0f);
    std::fill(proprioception_.begin(), proprioception_.end(), 0.0f);
    std::fill(audio_.begin(), audio_.end(), 0.0f);
    timestamp_ = 0.0;
}

void SensoryPercept::validateAll() const {
    validateVisionSize();
    validateTouchSize();
    validateInternalSize();
    validateProprioceptionSize();
}

