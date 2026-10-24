#include "SensoryPerceptImproved.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <chrono>
#include <sstream>

namespace nlm {

void SensoryPerceptConfig::loadFromConfig(const Config& config) {
    // Vision configuration
    if (config.has("sensory.vision_width")) {
        visionWidth = config.getOr<size_t>("sensory.vision_width", visionWidth);
    }
    if (config.has("sensory.vision_height")) {
        visionHeight = config.getOr<size_t>("sensory.vision_height", visionHeight);
    }
    if (config.has("sensory.vision_scale")) {
        visionScale = config.getOr<float>("sensory.vision_scale", visionScale);
    }
    if (config.has("sensory.vision_enabled")) {
        visionEnabled = config.getOr<bool>("sensory.vision_enabled", visionEnabled);
    }
    
    // Touch configuration
    if (config.has("sensory.touch_count")) {
        touchCount = config.getOr<size_t>("sensory.touch_count", touchCount);
    }
    if (config.has("sensory.touch_sensitivity")) {
        touchSensitivity = config.getOr<float>("sensory.touch_sensitivity", touchSensitivity);
    }
    if (config.has("sensory.touch_enabled")) {
        touchEnabled = config.getOr<bool>("sensory.touch_enabled", touchEnabled);
    }
    
    // Internal signals configuration
    if (config.has("sensory.internal_count")) {
        internalSignalCount = config.getOr<size_t>("sensory.internal_count", internalSignalCount);
    }
    if (config.has("sensory.internal_scale")) {
        internalScale = config.getOr<float>("sensory.internal_scale", internalScale);
    }
    if (config.has("sensory.internal_enabled")) {
        internalEnabled = config.getOr<bool>("sensory.internal_enabled", internalEnabled);
    }
    
    // Proprioception configuration
    if (config.has("sensory.proprioception_count")) {
        proprioceptionCount = config.getOr<size_t>("sensory.proprioception_count", proprioceptionCount);
    }
    if (config.has("sensory.proprioception_scale")) {
        proprioceptionScale = config.getOr<float>("sensory.proprioception_scale", proprioceptionScale);
    }
    if (config.has("sensory.proprioception_enabled")) {
        proprioceptionEnabled = config.getOr<bool>("sensory.proprioception_enabled", proprioceptionEnabled);
    }
    
    // Audio configuration
    if (config.has("sensory.audio_sample_rate")) {
        audioSampleRate = config.getOr<size_t>("sensory.audio_sample_rate", audioSampleRate);
    }
    if (config.has("sensory.audio_samples")) {
        audioNumSamples = config.getOr<size_t>("sensory.audio_samples", audioNumSamples);
    }
    if (config.has("sensory.audio_scale")) {
        audioScale = config.getOr<float>("sensory.audio_scale", audioScale);
    }
    if (config.has("sensory.audio_enabled")) {
        audioEnabled = config.getOr<bool>("sensory.audio_enabled", audioEnabled);
    }
    
    // Signal processing
    if (config.has("sensory.novelty_threshold")) {
        noveltyThreshold = config.getOr<float>("sensory.novelty_threshold", noveltyThreshold);
    }
    if (config.has("sensory.novelty_decay")) {
        noveltyDecay = config.getOr<float>("sensory.novelty_decay", noveltyDecay);
    }
    if (config.has("sensory.signal_compression")) {
        signalCompression = config.getOr<float>("sensory.signal_compression", signalCompression);
    }
    if (config.has("sensory.enable_compression")) {
        enableCompression = config.getOr<bool>("sensory.enable_compression", enableCompression);
    }
    
    // Memory and performance
    if (config.has("sensory.max_vision_buffer")) {
        maxVisionBufferSize = config.getOr<size_t>("sensory.max_vision_buffer", maxVisionBufferSize);
    }
    if (config.has("sensory.max_touch_buffer")) {
        maxTouchBufferSize = config.getOr<size_t>("sensory.max_touch_buffer", maxTouchBufferSize);
    }
    if (config.has("sensory.max_internal_buffer")) {
        maxInternalBufferSize = config.getOr<size_t>("sensory.max_internal_buffer", maxInternalBufferSize);
    }
    if (config.has("sensory.max_proprioception_buffer")) {
        maxProprioceptionBufferSize = config.getOr<size_t>("sensory.max_proprioception_buffer", maxProprioceptionBufferSize);
    }
    
    // Timestamp
    if (config.has("sensory.enable_timestamps")) {
        enableTimestamps = config.getOr<bool>("sensory.enable_timestamps", enableTimestamps);
    }
    if (config.has("sensory.timestamp_interval")) {
        timestampUpdateInterval = config.getOr<double>("sensory.timestamp_interval", timestampUpdateInterval);
    }
}

SensoryPercept::SensoryPercept(const SensoryPerceptConfig& config)
    : config_(config)
    , timestamp_(0.0)
{
    resizeBuffers();
    NLM_LOG_INFO("SensoryPercept initialized with configurable parameters:");
    NLM_LOG_INFO("  Vision: " + std::to_string(config_.visionWidth) + "x" + std::to_string(config_.visionHeight) +
                 " (enabled: " + std::to_string(config_.visionEnabled) + ")");
    NLM_LOG_INFO("  Touch: " + std::to_string(config_.touchCount) +
                 " (enabled: " + std::to_string(config_.touchEnabled) + ")");
    NLM_LOG_INFO("  Internal: " + std::to_string(config_.internalSignalCount) +
                 " (enabled: " + std::to_string(config_.internalEnabled) + ")");
    NLM_LOG_INFO("  Proprioception: " + std::to_string(config_.proprioceptionCount) +
                 " (enabled: " + std::to_string(config_.proprioceptionEnabled) + ")");
    if (config_.audioEnabled) {
        NLM_LOG_INFO("  Audio: " + std::to_string(config_.audioSampleRate) + "Hz, " + 
                     std::to_string(config_.audioNumSamples) + " samples (enabled: true)");
    }
}

void SensoryPercept::setVision(size_t width, size_t height, const std::vector<float>& v) {
    config_.visionWidth = width;
    config_.visionHeight = height;
    vision_.resize(width * height, 0.0f);
    size_t copySize = std::min(v.size(), vision_.size());
    for (size_t i = 0; i < copySize; ++i) {
        vision_[i] = v[i] * config_.visionScale;
    }
    if (config_.enableTimestamps) {
        timestamp_ = getCurrentTime();
    }
}

double SensoryPercept::getCurrentTime() const {
    if (!config_.enableTimestamps) return 0.0;
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::duration<double>>(duration).count();
}

void SensoryPercept::resizeBuffers() {
    // Resize vision buffer with memory limits
    size_t visionTargetSize = config_.visionWidth * config_.visionHeight;
    resizeVector(vision_, visionTargetSize, 0.0f);
    
    // Resize touch buffer
    resizeVector(touch_, config_.touchCount, 0.0f);
    
    // Resize internal buffer
    resizeVector(internal_, config_.internalSignalCount, 0.0f);
    
    // Resize proprioception buffer
    resizeVector(proprioception_, config_.proprioceptionCount, 0.0f);
    
    // Resize audio buffer
    resizeVector(audio_, config_.audioNumSamples, 0.0f);
}

void SensoryPercept::resizeVector(std::vector<float>& vec, size_t targetSize, float defaultValue) {
    if (targetSize > config_.maxVisionBufferSize) {
        NLM_LOG_WARNING("Requested buffer size " + std::to_string(targetSize) + 
                       " exceeds maximum " + std::to_string(config_.maxVisionBufferSize) +
                       ". Clamping to maximum.");
        targetSize = config_.maxVisionBufferSize;
    }
    
    if (vec.size() != targetSize) {
        vec.resize(targetSize, defaultValue);
    }
}

std::vector<float> SensoryPercept::getAllSignals() const {
    std::vector<float> all;
    
    if (config_.visionEnabled) {
        // Vision (flattened)
        all.insert(all.end(), vision_.begin(), vision_.end());
    }
    
    if (config_.touchEnabled) {
        // Touch
        all.insert(all.end(), touch_.begin(), touch_.end());
    }
    
    if (config_.internalEnabled) {
        // Internal
        all.insert(all.end(), internal_.begin(), internal_.end());
    }
    
    if (config_.proprioceptionEnabled) {
        // Proprioception
        all.insert(all.end(), proprioception_.begin(), proprioception_.end());
    }
    
    if (config_.audioEnabled) {
        // Audio
        all.insert(all.end(), audio_.begin(), audio_.end());
    }
    
    return all;
}

std::vector<float> SensoryPercept::getAllSignalsCompressed() const {
    std::vector<float> all = getAllSignals();
    
    if (config_.enableCompression && config_.signalCompression != 1.0f) {
        applyCompression(all);
    }
    
    return all;
}

bool SensoryPercept::validateVision() const {
    if (!config_.visionEnabled) return true;
    
    bool valid = true;
    for (float val : vision_) {
        if (std::isnan(val) || std::isinf(val)) {
            valid = false;
            break;
        }
    }
    
    return valid;
}

bool SensoryPercept::validateTouch() const {
    if (!config_.touchEnabled) return true;
    
    bool valid = true;
    for (float val : touch_) {
        if (std::isnan(val) || std::isinf(val)) {
            valid = false;
            break;
        }
    }
    
    return valid;
}

bool SensoryPercept::validateInternal() const {
    if (!config_.internalEnabled) return true;
    
    bool valid = true;
    for (float val : internal_) {
        if (std::isnan(val) || std::isinf(val)) {
            valid = false;
            break;
        }
    }
    
    return valid;
}

bool SensoryPercept::validateProprioception() const {
    if (!config_.proprioceptionEnabled) return true;
    
    bool valid = true;
    for (float val : proprioception_) {
        if (std::isnan(val) || std::isinf(val)) {
            valid = false;
            break;
        }
    }
    
    return valid;
}

bool SensoryPercept::validateAudio() const {
    if (!config_.audioEnabled) return true;
    
    bool valid = true;
    for (float val : audio_) {
        if (std::isnan(val) || std::isinf(val)) {
            valid = false;
            break;
        }
    }
    
    return valid;
}

bool SensoryPercept::validateAll() const {
    return validateVision() && validateTouch() && 
           validateInternal() && validateProprioception() && 
           validateAudio();
}

void SensoryPercept::applyCompression(std::vector<float>& signal) const {
    if (config_.signalCompression <= 0.0f) return;
    
    float scale = 1.0f / config_.signalCompression;
    for (float& val : signal) {
        val = std::tanh(val * scale);
    }
}

float SensoryPercept::calculateMean(const std::vector<float>& signal) const {
    if (signal.empty()) return 0.0f;
    float sum = std::accumulate(signal.begin(), signal.end(), 0.0f);
    return sum / signal.size();
}

float SensoryPercept::calculateStdDev(const std::vector<float>& signal, float mean) const {
    if (signal.size() <= 1) return 0.0f;
    
    float sumSq = 0.0f;
    for (float val : signal) {
        float diff = val - mean;
        sumSq += diff * diff;
    }
    
    return std::sqrt(sumSq / (signal.size() - 1));
}

float SensoryPercept::getVisionMean() const {
    if (!config_.visionEnabled || vision_.empty()) return 0.0f;
    return calculateMean(vision_);
}

float SensoryPercept::getVisionStdDev() const {
    if (!config_.visionEnabled || vision_.empty()) return 0.0f;
    float mean = getVisionMean();
    return calculateStdDev(vision_, mean);
}

float SensoryPercept::getTouchMean() const {
    if (!config_.touchEnabled || touch_.empty()) return 0.0f;
    return calculateMean(touch_);
}

float SensoryPercept::getInternalMean() const {
    if (!config_.internalEnabled || internal_.empty()) return 0.0f;
    return calculateMean(internal_);
}

float SensoryPercept::getProprioceptionMean() const {
    if (!config_.proprioceptionEnabled || proprioception_.empty()) return 0.0f;
    return calculateMean(proprioception_);
}

float SensoryPercept::getAudioMean() const {
    if (!config_.audioEnabled || audio_.empty()) return 0.0f;
    return calculateMean(audio_);
}

void SensoryPercept::normalizeVision() {
    if (!config_.visionEnabled || vision_.empty()) return;
    
    float mean = getVisionMean();
    float stddev = getVisionStdDev();
    
    if (stddev > 0.0f) {
        for (float& val : vision_) {
            val = (val - mean) / stddev;
        }
    }
}

void SensoryPercept::normalizeTouch() {
    if (!config_.touchEnabled || touch_.empty()) return;
    
    float mean = getTouchMean();
    float stddev = calculateStdDev(touch_, mean);
    
    if (stddev > 0.0f) {
        for (float& val : touch_) {
            val = (val - mean) / stddev;
        }
    }
}

void SensoryPercept::normalizeInternal() {
    if (!config_.internalEnabled || internal_.empty()) return;
    
    float mean = getInternalMean();
    float stddev = calculateStdDev(internal_, mean);
    
    if (stddev > 0.0f) {
        for (float& val : internal_) {
            val = (val - mean) / stddev;
        }
    }
}

void SensoryPercept::normalizeProprioception() {
    if (!config_.proprioceptionEnabled || proprioception_.empty()) return;
    
    float mean = getProprioceptionMean();
    float stddev = calculateStdDev(proprioception_, mean);
    
    if (stddev > 0.0f) {
        for (float& val : proprioception_) {
            val = (val - mean) / stddev;
        }
    }
}

void SensoryPercept::normalizeAudio() {
    if (!config_.audioEnabled || audio_.empty()) return;
    
    float mean = getAudioMean();
    float stddev = calculateStdDev(audio_, mean);
    
    if (stddev > 0.0f) {
        for (float& val : audio_) {
            val = (val - mean) / stddev;
        }
    }
}

void SensoryPercept::scaleAllSignals(float factor) {
    if (config_.visionEnabled) {
        for (float& val : vision_) val *= factor;
    }
    if (config_.touchEnabled) {
        for (float& val : touch_) val *= factor;
    }
    if (config_.internalEnabled) {
        for (float& val : internal_) val *= factor;
    }
    if (config_.proprioceptionEnabled) {
        for (float& val : proprioception_) val *= factor;
    }
    if (config_.audioEnabled) {
        for (float& val : audio_) val *= factor;
    }
}

void SensoryPercept::offsetAllSignals(float offset) {
    if (config_.visionEnabled) {
        for (float& val : vision_) val += offset;
    }
    if (config_.touchEnabled) {
        for (float& val : touch_) val += offset;
    }
    if (config_.internalEnabled) {
        for (float& val : internal_) val += offset;
    }
    if (config_.proprioceptionEnabled) {
        for (float& val : proprioception_) val += offset;
    }
    if (config_.audioEnabled) {
        for (float& val : audio_) val += offset;
    }
}

std::vector<float> SensoryPercept::compressSignal(const std::vector<float>& signal) const {
    std::vector<float> compressed = signal;
    applyCompression(compressed);
    return compressed;
}

std::vector<float> SensoryPercept::decompressSignal(const std::vector<float>& signal) const {
    std::vector<float> decompressed = signal;
    float scale = config_.signalCompression;
    for (float& val : decompressed) {
        val = std::tanh(val) * scale;
    }
    return decompressed;
}

size_t SensoryPercept::getMemoryUsage() const {
    size_t total = 0;
    total += vision_.capacity() * sizeof(float);
    total += touch_.capacity() * sizeof(float);
    total += internal_.capacity() * sizeof(float);
    total += proprioception_.capacity() * sizeof(float);
    total += audio_.capacity() * sizeof(float);
    return total;
}

void SensoryPercept::clear() {
    vision_.clear();
    touch_.clear();
    internal_.clear();
    proprioception_.clear();
    audio_.clear();
    timestamp_ = 0.0;
}

float SensoryPercept::compareVision(const SensoryPercept& other) const {
    if (vision_.size() != other.vision_.size()) return 0.0f;
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < vision_.size(); ++i) {
        float diff = std::abs(vision_[i] - other.vision_[i]);
        totalDiff += diff;
    }
    
    return totalDiff / vision_.size();
}

bool SensoryPercept::isNovel(const SensoryPercept& previous, float threshold) const {
    float thresholdValue = (threshold > 0.0f) ? threshold : config_.noveltyThreshold;
    return compareVision(previous) > thresholdValue;
}

void SensoryPercept::logSignalStats() const {
    NLM_LOG_INFO("SensoryPercept Signal Statistics:");
    
    if (config_.visionEnabled) {
        NLM_LOG_INFO("  Vision: mean=" + std::to_string(getVisionMean()) + 
                     ", stddev=" + std::to_string(getVisionStdDev()) +
                     ", size=" + std::to_string(vision_.size()));
    }
    
    if (config_.touchEnabled) {
        NLM_LOG_INFO("  Touch: mean=" + std::to_string(getTouchMean()) + ", size=" + std::to_string(touch_.size()));
    }
    
    if (config_.internalEnabled) {
        NLM_LOG_INFO("  Internal: mean=" + std::to_string(getInternalMean()) + ", size=" + std::to_string(internal_.size()));
    }
    
    if (config_.proprioceptionEnabled) {
        NLM_LOG_INFO("  Proprioception: mean=" + std::to_string(getProprioceptionMean()) + ", size=" + std::to_string(proprioception_.size()));
    }
    
    if (config_.audioEnabled) {
        NLM_LOG_INFO("  Audio: mean=" + std::to_string(getAudioMean()) + ", size=" + std::to_string(audio_.size()));
    }
    
    NLM_LOG_INFO("  Memory usage: " + std::to_string(getMemoryUsage()) + " bytes");
    NLM_LOG_INFO("  Timestamps enabled: " + std::to_string(config_.enableTimestamps));
}

std::string SensoryPercept::toString() const {
    std::stringstream ss;
    ss << "SensoryPercept: ";
    
    if (config_.visionEnabled) {
        ss << "Vision(" << vision_.size() << ")";
        if (!config_.touchEnabled) ss << ", ";
    }
    
    if (config_.touchEnabled) {
        ss << "Touch(" << touch_.size() << ")";
        if (!config_.internalEnabled) ss << ", ";
    }
    
    if (config_.internalEnabled) {
        ss << "Internal(" << internal_.size() << ")";
        if (!config_.proprioceptionEnabled) ss << ", ";
    }
    
    if (config_.proprioceptionEnabled) {
        ss << "Proprioception(" << proprioception_.size() << ")";
        if (!config_.audioEnabled) ss << ", ";
    }
    
    if (config_.audioEnabled) {
        ss << "Audio(" << audio_.size() << ")";
    }
    
    return ss.str();
}

} // namespace nlm