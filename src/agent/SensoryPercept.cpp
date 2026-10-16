#include "SensoryPercept.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>

namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
    , signalsProcessed_(false)
{
    vision_.resize(16 * 16, 0.0f);
    touch_.resize(8, 0.0f);
    internal_.resize(4, 0.0f);
    proprioception_.resize(6, 0.0f);
    audio_.resize(0, 0.0f);
}

void SensoryPercept::configureVision(size_t width, size_t height) {
    visionWidth_ = width;
    visionHeight_ = height;
    vision_.resize(width * height, 0.0f);
    processedVision_.resize(width * height, 0.0f);
}

void SensoryPercept::configureTouch(size_t count) {
    touch_.resize(count, 0.0f);
    processedTouch_.resize(count, 0.0f);
}

void SensoryPercept::configureInternal(size_t count) {
    internal_.resize(count, 0.0f);
    processedInternal_.resize(count, 0.0f);
}

void SensoryPercept::configureProprioception(size_t count) {
    proprioception_.resize(count, 0.0f);
    processedProprioception_.resize(count, 0.0f);
}

void SensoryPercept::configureAudio(size_t count) {
    audio_.resize(count, 0.0f);
    processedAudio_.resize(count, 0.0f);
}

std::vector<float> SensoryPercept::getAllSignals() const {
    std::vector<float> all;
    
    // Vision (flattened)
    all.insert(all.end(), signalsProcessed_ ? processedVision_.begin() : vision_.begin(), 
               signalsProcessed_ ? processedVision_.end() : vision_.end());
    
    // Touch
    all.insert(all.end(), signalsProcessed_ ? processedTouch_.begin() : touch_.begin(),
               signalsProcessed_ ? processedTouch_.end() : touch_.end());
    
    // Internal
    all.insert(all.end(), signalsProcessed_ ? processedInternal_.begin() : internal_.begin(),
               signalsProcessed_ ? processedInternal_.end() : internal_.end());
    
    // Proprioception
    all.insert(all.end(), signalsProcessed_ ? processedProprioception_.begin() : proprioception_.begin(),
               signalsProcessed_ ? processedProprioception_.end() : proprioception_.end());
    
    // Audio
    all.insert(all.end(), signalsProcessed_ ? processedAudio_.begin() : audio_.begin(),
               signalsProcessed_ ? processedAudio_.end() : audio_.end());
    
    return all;
}

void SensoryPercept::processSignals() {
    if (!signalProcessor_.getConfig().enableSignalValidation && 
        !signalProcessor_.getConfig().enableCompression &&
        !signalProcessor_.getConfig().enableFiltering &&
        !signalProcessor_.getConfig().enableFeatureExtraction &&
        !signalProcessor_.getConfig().enableTemporalSmoothing &&
        !signalProcessor_.getConfig().applyNoise) {
        // If no processing enabled, just copy original signals
        processedVision_ = vision_;
        processedTouch_ = touch_;
        processedInternal_ = internal_;
        processedProprioception_ = proprioception_;
        processedAudio_ = audio_;
        signalsProcessed_ = true;
        return;
    }
    
    // Validate signals first
    if (!validateAllSignals()) {
        NLM_LOG_WARNING("Signal validation failed - processing may be unreliable");
    }
    
    // Process vision signal
    processedVision_ = signalProcessor_.processVision(vision_);
    
    // Process touch signal
    processedTouch_ = signalProcessor_.processTouch(touch_);
    
    // Process internal signal
    processedInternal_ = signalProcessor_.processInternal(internal_);
    
    // Process proprioception signal
    processedProprioception_ = signalProcessor_.processProprioception(proprioception_);
    
    // Process audio signal
    processedAudio_ = signalProcessor_.processAudio(audio_);
    
    signalsProcessed_ = true;
    
    NLM_LOG_DEBUG("Sensory signals processed with " + 
                 std::to_string(getAllSignals().size()) + " total signal values");
}

bool SensoryPercept::validateAllSignals() const {
    bool visionValid = signalProcessor_.validateSignal(vision_);
    bool touchValid = signalProcessor_.validateSignal(touch_);
    bool internalValid = signalProcessor_.validateSignal(internal_);
    bool proprioValid = signalProcessor_.validateSignal(proprioception_);
    bool audioValid = signalProcessor_.validateSignal(audio_);
    
    if (!visionValid) NLM_LOG_WARNING("Vision signal validation failed");
    if (!touchValid) NLM_LOG_WARNING("Touch signal validation failed");
    if (!internalValid) NLM_LOG_WARNING("Internal signal validation failed");
    if (!proprioValid) NLM_LOG_WARNING("Proprioception signal validation failed");
    if (!audioValid) NLM_LOG_WARNING("Audio signal validation failed");
    
    return visionValid && touchValid && internalValid && proprioValid && audioValid;
}

std::vector<SignalFeature> SensoryPercept::extractFeatures() const {
    std::vector<SignalFeature> allFeatures;
    
    // Extract features from each signal type
    std::vector<SignalFeature> visionFeatures = signalProcessor_.extractFeatures(vision_);
    std::vector<SignalFeature> touchFeatures = signalProcessor_.extractFeatures(touch_);
    std::vector<SignalFeature> internalFeatures = signalProcessor_.extractFeatures(internal_);
    std::vector<SignalFeature> proprioFeatures = signalProcessor_.extractFeatures(proprioception_);
    std::vector<SignalFeature> audioFeatures = signalProcessor_.extractFeatures(audio_);
    
    // Add prefix to feature names
    auto addPrefix = [](std::vector<SignalFeature>& features, const std::string& prefix) {
        for (auto& feature : features) {
            feature.name = prefix + "_" + feature.name;
        }
    };
    
    addPrefix(visionFeatures, "vision");
    addPrefix(touchFeatures, "touch");
    addPrefix(internalFeatures, "internal");
    addPrefix(proprioFeatures, "proprio");
    addPrefix(audioFeatures, "audio");
    
    // Combine all features
    allFeatures.reserve(visionFeatures.size() + touchFeatures.size() + 
                       internalFeatures.size() + proprioFeatures.size() + 
                       audioFeatures.size());
    
    allFeatures.insert(allFeatures.end(), visionFeatures.begin(), visionFeatures.end());
    allFeatures.insert(allFeatures.end(), touchFeatures.begin(), touchFeatures.end());
    allFeatures.insert(allFeatures.end(), internalFeatures.begin(), internalFeatures.end());
    allFeatures.insert(allFeatures.end(), proprioFeatures.begin(), proprioFeatures.end());
    allFeatures.insert(allFeatures.end(), audioFeatures.begin(), audioFeatures.end());
    
    return allFeatures;
}

float SensoryPercept::getVisionAverage() const {
    if (vision_.empty()) return 0.0f;
    float sum = std::accumulate(vision_.begin(), vision_.end(), 0.0f);
    return sum / vision_.size();
}

float SensoryPercept::getTouchAverage() const {
    if (touch_.empty()) return 0.0f;
    float sum = std::accumulate(touch_.begin(), touch_.end(), 0.0f);
    return sum / touch_.size();
}

float SensoryPercept::getInternalAverage() const {
    if (internal_.empty()) return 0.0f;
    float sum = std::accumulate(internal_.begin(), internal_.end(), 0.0f);
    return sum / internal_.size();
}

float SensoryPercept::getProprioceptionAverage() const {
    if (proprioception_.empty()) return 0.0f;
    float sum = std::accumulate(proprioception_.begin(), proprioception_.end(), 0.0f);
    return sum / proprioception_.size();
}

float SensoryPercept::getAudioAverage() const {
    if (audio_.empty()) return 0.0f;
    float sum = std::accumulate(audio_.begin(), audio_.end(), 0.0f);
    return sum / audio_.size();
}

void SensoryPercept::enableSignalCompression(float factor) {
    SignalProcessingConfig config = signalProcessor_.getConfig();
    config.enableCompression = true;
    config.compressionFactor = factor;
    signalProcessor_.setConfig(config);
    signalsProcessed_ = false;
}

void SensoryPercept::disableSignalCompression() {
    SignalProcessingConfig config = signalProcessor_.getConfig();
    config.enableCompression = false;
    signalProcessor_.setConfig(config);
    signalsProcessed_ = false;
}

void SensoryPercept::enableFiltering(float alpha) {
    SignalProcessingConfig config = signalProcessor_.getConfig();
    config.enableFiltering = true;
    config.filterAlpha = alpha;
    signalProcessor_.setConfig(config);
    signalsProcessed_ = false;
}

void SensoryPercept::disableFiltering() {
    SignalProcessingConfig config = signalProcessor_.getConfig();
    config.enableFiltering = false;
    signalProcessor_.setConfig(config);
    signalsProcessed_ = false;
}

void SensoryPercept::enableFeatureExtraction() {
    SignalProcessingConfig config = signalProcessor_.getConfig();
    config.enableFeatureExtraction = true;
    signalProcessor_.setConfig(config);
    signalsProcessed_ = false;
}

void SensoryPercept::disableFeatureExtraction() {
    SignalProcessingConfig config = signalProcessor_.getConfig();
    config.enableFeatureExtraction = false;
    signalProcessor_.setConfig(config);
    signalsProcessed_ = false;
}

void SensoryPercept::enableTemporalSmoothing(float factor) {
    SignalProcessingConfig config = signalProcessor_.getConfig();
    config.enableTemporalSmoothing = true;
    config.temporalSmoothingFactor = factor;
    signalProcessor_.setConfig(config);
    signalsProcessed_ = false;
}

void SensoryPercept::disableTemporalSmoothing() {
    SignalProcessingConfig config = signalProcessor_.getConfig();
    config.enableTemporalSmoothing = false;
    signalProcessor_.setConfig(config);
    signalsProcessed_ = false;
}

void SensoryPercept::enableSignalValidation(bool enable) {
    SignalProcessingConfig config = signalProcessor_.getConfig();
    config.enableSignalValidation = enable;
    signalProcessor_.setConfig(config);
}

void SensoryPercept::setSignalRange(float min, float max) {
    SignalProcessingConfig config = signalProcessor_.getConfig();
    config.minValidSignal = min;
    config.maxValidSignal = max;
    signalProcessor_.setConfig(config);
}

void SensoryPercept::enableNoiseInjection(float level) {
    SignalProcessingConfig config = signalProcessor_.getConfig();
    config.applyNoise = true;
    config.noiseLevel = level;
    signalProcessor_.setConfig(config);
    signalsProcessed_ = false;
}

void SensoryPercept::disableNoiseInjection() {
    SignalProcessingConfig config = signalProcessor_.getConfig();
    config.applyNoise = false;
    signalProcessor_.setConfig(config);
    signalsProcessed_ = false;
}

void SensoryPercept::addNoiseToSignals(float level) {
    enableNoiseInjection(level);
    processSignals();
}

void SensoryPercept::applyGain(float gain) {
    vision_ = applyGainToVector(vision_, gain);
    touch_ = applyGainToVector(touch_, gain);
    internal_ = applyGainToVector(internal_, gain);
    proprioception_ = applyGainToVector(proprioception_, gain);
    audio_ = applyGainToVector(audio_, gain);
    signalsProcessed_ = false;
}

void SensoryPercept::applyOffset(float offset) {
    vision_ = applyOffsetToVector(vision_, offset);
    touch_ = applyOffsetToVector(touch_, offset);
    internal_ = applyOffsetToVector(internal_, offset);
    proprioception_ = applyOffsetToVector(proprioception_, offset);
    audio_ = applyOffsetToVector(audio_, offset);
    signalsProcessed_ = false;
}

void SensoryPercept::scaleSignals(float factor) {
    applyGain(factor);
}

SensoryPercept::SignalStats SensoryPercept::computeSignalStats(const std::vector<float>& signal) const {
    SignalStats stats;
    stats.count = signal.size();
    
    if (signal.empty()) {
        return stats;
    }
    
    stats.mean = std::accumulate(signal.begin(), signal.end(), 0.0f) / signal.size();
    
    float sumSq = 0.0f;
    for (float val : signal) {
        float diff = val - stats.mean;
        sumSq += diff * diff;
    }
    stats.variance = sumSq / signal.size();
    
    stats.min = *std::min_element(signal.begin(), signal.end());
    stats.max = *std::max_element(signal.begin(), signal.end());
    stats.range = stats.max - stats.min;
    
    return stats;
}

std::unordered_map<std::string, SensoryPercept::SignalStats> SensoryPercept::computeAllSignalStats() const {
    std::unordered_map<std::string, SignalStats> stats;
    
    stats["vision"] = computeSignalStats(vision_);
    stats["touch"] = computeSignalStats(touch_);
    stats["internal"] = computeSignalStats(internal_);
    stats["proprioception"] = computeSignalStats(proprioception_);
    stats["audio"] = computeSignalStats(audio_);
    
    return stats;
}

std::vector<float> SensoryPercept::applyGainToVector(const std::vector<float>& signal, float gain) const {
    std::vector<float> result(signal.size());
    for (size_t i = 0; i < signal.size(); ++i) {
        result[i] = signal[i] * gain;
    }
    return result;
}

std::vector<float> SensoryPercept::applyOffsetToVector(const std::vector<float>& signal, float offset) const {
    std::vector<float> result(signal.size());
    for (size_t i = 0; i < signal.size(); ++i) {
        result[i] = signal[i] + offset;
    }
    return result;
}

} // namespace nlm