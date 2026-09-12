// NLM SensoryPercept Implementation
// Improved version with better utility methods, memory management, and type safety

#include "SensoryPercept.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <stdexcept>
#include <random>

namespace nlm {

// Configuration constants for SensoryPercept
namespace SensoryPerceptConstants {
    constexpr size_t DEFAULT_VISION_DIMENSION = 256;  // 16x16 vision grid
    constexpr size_t DEFAULT_TOUCH_DIMENSION = 8;
    constexpr size_t DEFAULT_INTERNAL_DIMENSION = 4;
    constexpr size_t DEFAULT_PROPRIOCEPTION_DIMENSION = 6;
    constexpr size_t DEFAULT_AUDIO_DIMENSION = 0;
    
    // Signal normalization and scaling
    constexpr float MIN_VISION_VALUE = 0.0f;
    constexpr float MAX_VISION_VALUE = 1.0f;
    constexpr float VISION_SCALE_FACTOR = 1.0f;
    
    // Memory and performance optimization
    constexpr size_t INITIAL_VECTOR_CAPACITY = 256;
    constexpr float DEFAULT_TIMESTAMP = 0.0;
    
    // Statistical and signal processing constants
    constexpr float DEFAULT_NOISE_THRESHOLD = 0.01f;
    constexpr float DEFAULT_CONTRAST_THRESHOLD = 0.1f;
    constexpr float DEFAULT_MAGNITUDE_WEIGHT = 1.0f;
}

SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(SensoryPerceptConstants::DEFAULT_TIMESTAMP)
{
    // Initialize with capacity to avoid reallocations
    vision_.reserve(SensoryPerceptConstants::INITIAL_VECTOR_CAPACITY);
    vision_.resize(visionWidth_ * visionHeight_, SensoryPerceptConstants::MIN_VISION_VALUE);
    
    touch_.reserve(SensoryPerceptConstants::DEFAULT_TOUCH_DIMENSION);
    touch_.resize(SensoryPerceptConstants::DEFAULT_TOUCH_DIMENSION, 0.0f);
    
    internal_.reserve(SensoryPerceptConstants::DEFAULT_INTERNAL_DIMENSION);
    internal_.resize(SensoryPerceptConstants::DEFAULT_INTERNAL_DIMENSION, 0.0f);
    
    proprioception_.reserve(SensoryPerceptConstants::DEFAULT_PROPRIOCEPTION_DIMENSION);
    proprioception_.resize(SensoryPerceptConstants::DEFAULT_PROPRIOCEPTION_DIMENSION, 0.0f);
    
    audio_.reserve(SensoryPerceptConstants::DEFAULT_AUDIO_DIMENSION);
    audio_.resize(SensoryPerceptConstants::DEFAULT_AUDIO_DIMENSION, 0.0f);
}

void SensoryPercept::validateIndex(size_t index, size_t dimension, const std::string& fieldName) const {
    if (index >= dimension) {
        throw std::out_of_range("SensoryPercept::" + fieldName + " access: index " + std::to_string(index) +
                              " out of range [0, " + std::to_string(dimension) + ")");
    }
}

float SensoryPercept::getVisionValue(size_t x, size_t y) const {
    if (x >= visionWidth_ || y >= visionHeight_) {
        return SensoryPerceptConstants::MIN_VISION_VALUE;  // Safe default
    }
    size_t index = y * visionWidth_ + x;
    return std::clamp(vision_[index], SensoryPerceptConstants::MIN_VISION_VALUE,
                      SensoryPerceptConstants::MAX_VISION_VALUE);
}

void SensoryPercept::setVisionValue(size_t x, size_t y, float value) {
    if (x >= visionWidth_ || y >= visionHeight_) {
        return;  // Ignore out-of-bounds coordinates
    }
    size_t index = y * visionWidth_ + x;
    vision_[index] = std::clamp(value, SensoryPerceptConstants::MIN_VISION_VALUE,
                                SensoryPerceptConstants::MAX_VISION_VALUE);
}

float SensoryPercept::getTotalVisionMagnitude() const {
    float sum = std::accumulate(vision_.begin(), vision_.end(), 0.0f);
    return sum / vision_.size();
}

float SensoryPercept::getVisionContrast() const {
    if (vision_.empty()) return 0.0f;
    
    float mean = getTotalVisionMagnitude();
    float sumSquares = 0.0f;
    
    for (float value : vision_) {
        float diff = value - mean;
        sumSquares += diff * diff;
    }
    
    float variance = sumSquares / vision_.size();
    return std::sqrt(variance);
}

float SensoryPercept::getVisionMaxIntensity() const {
    return *std::max_element(vision_.begin(), vision_.end());
}

float SensoryPercept::getVisionMinIntensity() const {
    return *std::min_element(vision_.begin(), vision_.end());
}

std::vector<float> SensoryPercept::getVisionRegion(size_t x, size_t y, size_t radius) const {
    std::vector<float> region;
    region.reserve((2 * radius + 1) * (2 * radius + 1));
    
    size_t startX = std::max(0, static_cast<int>(x) - static_cast<int>(radius));
    size_t endX = std::min(visionWidth_, x + radius + 1);
    size_t startY = std::max(0, static_cast<int>(y) - static_cast<int>(radius));
    size_t endY = std::min(visionHeight_, y + radius + 1);
    
    for (size_t y = startY; y < endY; ++y) {
        for (size_t x = startX; x < endX; ++x) {
            region.push_back(getVisionValue(x, y));
        }
    }
    
    return region;
}

std::vector<float> SensoryPercept::getTouchForDirection(size_t direction) const {
    if (direction >= 8) {
        return {};  // Invalid direction
    }
    
    // Touch sensors are typically in 8 directions (N, NE, E, SE, S, SW, W, NW)
    std::vector<float> result;
    result.reserve(touch_.size() / 8);
    
    // Simple distribution - in a real implementation this would depend on sensor layout
    size_t index = direction % touch_.size();
    
    // Return a small region around this touch sensor
    size_t start = std::max(0, static_cast<int>(index) - 1);
    size_t end = std::min(static_cast<size_t>(8), index + 2);
    
    for (size_t i = start; i < end; ++i) {
        result.push_back(touch_[i % touch_.size()]);
    }
    
    return result;
}

float SensoryPercept::getInternalSignal(size_t index) const {
    if (index >= internal_.size()) {
        return 0.0f;  // Safe default
    }
    return internal_[index];
}

void SensoryPercept::setInternalSignal(size_t index, float value) {
    if (index < internal_.size()) {
        internal_[index] = value;
    }
}

float SensoryPercept::getProprioceptionValue(size_t index) const {
    if (index >= proprioception_.size()) {
        return 0.0f;  // Safe default
    }
    return proprioception_[index];
}

void SensoryPercept::setProprioceptionValue(size_t index, float value) {
    if (index < proprioception_.size()) {
        proprioception_[index] = value;
    }
}

float SensoryPercept::getAudioSignal(size_t index) const {
    if (index >= audio_.size()) {
        return 0.0f;  // Safe default
    }
    return audio_[index];
}

void SensoryPercept::setAudioSignal(size_t index, float value) {
    if (index < audio_.size()) {
        audio_[index] = value;
    }
}

float SensoryPercept::getEnergyLevel() const {
    // Energy is typically in internal[0]
    return getInternalSignal(0);
}

float SensoryPercept::getHealthLevel() const {
    // Health is typically in internal[1]
    return getInternalSignal(1);
}

float SensoryPercept::getTotalInternalState() const {
    float sum = std::accumulate(internal_.begin(), internal_.end(), 0.0f);
    return sum / internal_.size();
}

std::vector<float> SensoryPercept::getAllSignals() const {
    std::vector<float> all;
    all.reserve(getTotalSignalCount());
    
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

float SensoryPercept::getSignalMagnitude(const std::vector<float>& signals) const {
    if (signals.empty()) return 0.0f;
    
    float sumSquares = 0.0f;
    for (float signal : signals) {
        sumSquares += signal * signal;
    }
    
    return std::sqrt(sumSquares / signals.size());
}

float SensoryPercept::getSignalThresholdedMagnitude(const std::vector<float>& signals, float threshold) const {
    float sumSquares = 0.0f;
    size_t count = 0;
    
    for (float signal : signals) {
        if (std::abs(signal) > threshold) {
            sumSquares += signal * signal;
            count++;
        }
    }
    
    if (count == 0) return 0.0f;
    return std::sqrt(sumSquares / count);
}

float SensoryPercept::getSignalVariance(const std::vector<float>& signals) const {
    if (signals.empty()) return 0.0f;
    
    float mean = std::accumulate(signals.begin(), signals.end(), 0.0f) / signals.size();
    float sumSquares = 0.0f;
    
    for (float signal : signals) {
        float diff = signal - mean;
        sumSquares += diff * diff;
    }
    
    return sumSquares / signals.size();
}

float SensoryPercept::getSignalSkewness(const std::vector<float>& signals) const {
    if (signals.size() < 3) return 0.0f;
    
    float mean = std::accumulate(signals.begin(), signals.end(), 0.0f) / signals.size();
    float variance = getSignalVariance(signals);
    
    if (variance < 1e-6f) return 0.0f;  // Avoid division by zero
    
    float stdDev = std::sqrt(variance);
    float sumCubed = 0.0f;
    
    for (float signal : signals) {
        float diff = (signal - mean) / stdDev;
        sumCubed += diff * diff * diff;
    }
    
    return sumCubed / signals.size();
}

bool SensoryPercept::isVisionDark() const {
    float threshold = (getVisionMaxIntensity() + getVisionMinIntensity()) / 2.0f;
    return getVisionMaxIntensity() < threshold * 1.1f;
}

bool SensoryPercept::isVisionBright() const {
    float threshold = (getVisionMaxIntensity() + getVisionMinIntensity()) / 2.0f;
    return getVisionMaxIntensity() > threshold * 1.1f;
}

bool SensoryPercept::hasTouch() const {
    for (float touch : touch_) {
        if (touch > 0.0f) return true;
    }
    return false;
}

float SensoryPercept::getMaxTouchValue() const {
    return touch_.empty() ? 0.0f : *std::max_element(touch_.begin(), touch_.end());
}

bool SensoryPercept::isHealthy() const {
    return getHealthLevel() > 0.5f;
}

bool SensoryPercept::isLowEnergy() const {
    return getEnergyLevel() < 0.2f;
}

bool SensoryPercept::isMoving() const {
    return getInternalSignal(2) > 0.5f;
}

bool SensoryPercept::isTurning() const {
    return getInternalSignal(3) > 0.5f;
}

void SensoryPercept::normalizeSignals() {
    // Normalize vision to [0, 1] range
    if (!vision_.empty()) {
        float minVal = *std::min_element(vision_.begin(), vision_.end());
        float maxVal = *std::max_element(vision_.begin(), vision_.end());
        
        if (maxVal > minVal) {
            float range = maxVal - minVal;
            for (float& value : vision_) {
                value = (value - minVal) / range;
            }
        }
    }
    
    // Normalize other signals to [-1, 1] range
    for (float& value : internal_) {
        value = std::clamp(value, -1.0f, 1.0f);
    }
    
    for (float& value : proprioception_) {
        value = std::clamp(value, -1.0f, 1.0f);
    }
}

void SensoryPercept::addNoise(float noiseLevel) {
    if (noiseLevel <= 0.0f) return;
    
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(-noiseLevel, noiseLevel);
    
    for (float& value : vision_) {
        value += dist(rng);
        value = std::clamp(value, 0.0f, 1.0f);
    }
    
    for (float& value : touch_) {
        value += dist(rng);
        value = std::clamp(value, 0.0f, 1.0f);
    }
    
    for (float& value : internal_) {
        value += dist(rng);
        value = std::clamp(value, -1.0f, 1.0f);
    }
    
    for (float& value : proprioception_) {
        value += dist(rng);
        value = std::clamp(value, -1.0f, 1.0f);
    }
}

void SensoryPercept::applySigmoid(float gain) {
    float k = gain / 4.0f;  // Adjust to get reasonable outputs
    
    for (float& value : vision_) {
        value = 1.0f / (1.0f + std::exp(-k * (value - 0.5f)));
    }
}

void SensoryPercept::applyGaussianBlur(size_t radius) {
    if (radius == 0) return;
    
    size_t originalWidth = visionWidth_;
    size_t originalHeight = visionHeight_;
    std::vector<float> original = vision_;
    
    vision_.clear();
    vision_.resize(originalHeight * originalWidth, 0.0f);
    
    // Simple box blur (Gaussian would be more computationally expensive)
    for (size_t y = 0; y < originalHeight; ++y) {
        for (size_t x = 0; x < originalWidth; ++x) {
            float sum = 0.0f;
            size_t count = 0;
            
            for (size_t yy = std::max(0, static_cast<int>(y) - static_cast<int>(radius)); 
                 yy < std::min(originalHeight, y + radius + 1); ++yy) {
                for (size_t xx = std::max(0, static_cast<int>(x) - static_cast<int>(radius)); 
                     xx < std::min(originalWidth, x + radius + 1); ++xx) {
                    sum += original[yy * originalWidth + xx];
                    count++;
                }
            }
            
            vision_[y * originalWidth + x] = sum / count;
        }
    }
}

void SensoryPercept::applyEdgeDetection() {
    // Simple Sobel-like edge detection
    std::vector<float> blurred = vision_;
    
    // Apply blur
    std::vector<float> blurredCopy = blurred;
    for (size_t y = 1; y < visionHeight_ - 1; ++y) {
        for (size_t x = 1; x < visionWidth_ - 1; ++x) {
            size_t idx = y * visionWidth_ + x;
            blurred[idx] = (blurredCopy[idx - visionWidth_] + blurredCopy[idx + visionWidth_] +
                           blurredCopy[idx - 1] + blurredCopy[idx + 1] + blurredCopy[idx]) / 5.0f;
        }
    }
    
    // Apply edge detection
    for (size_t y = 1; y < visionHeight_ - 1; ++y) {
        for (size_t x = 1; x < visionWidth_ - 1; ++x) {
            size_t idx = y * visionWidth_ + x;
            
            // Compute gradient
            float gx = blurred[idx + 1] - blurred[idx - 1];
            float gy = blurred[idx + visionWidth_] - blurred[idx - visionWidth_];
            
            // Compute magnitude
            vision_[idx] = std::sqrt(gx * gx + gy * gy);
        }
    }
    
    // Clamp to [0, 1] range
    for (float& value : vision_) {
        value = std::clamp(value, 0.0f, 1.0f);
    }
}

void SensoryPercept::applyContrastEnhancement(float factor) {
    if (factor <= 0.0f) return;
    
    float mean = getTotalVisionMagnitude();
    float stdDev = getVisionContrast();
    
    if (stdDev < 1e-6f) return;  // Avoid division by zero
    
    for (float& value : vision_) {
        // Enhance contrast
        value = mean + factor * (value - mean);
        value = std::clamp(value, 0.0f, 1.0f);
    }
}

void SensoryPercept::applyGammaCorrection(float gamma) {
    if (gamma <= 0.0f) return;
    
    for (float& value : vision_) {
        value = std::pow(value, gamma);
    }
}

void SensoryPercept::resizeVision(size_t newWidth, size_t newHeight) {
    visionWidth_ = newWidth;
    visionHeight_ = newHeight;
    
    // Smooth resizing (simple average for now)
    std::vector<float> resized(newWidth * newHeight, 0.0f);
    
    float xRatio = static_cast<float>(visionWidth_ - 1) / static_cast<float>(newWidth - 1);
    float yRatio = static_cast<float>(visionHeight_ - 1) / static_cast<float>(newHeight - 1);
    
    for (size_t y = 0; y < newHeight; ++y) {
        float fy = y * yRatio;
        size_t y0 = static_cast<size_t>(fy);
        size_t y1 = std::min(y0 + 1, visionHeight_ - 1);
        float ty = fy - y0;
        
        for (size_t x = 0; x < newWidth; ++x) {
            float fx = x * xRatio;
            size_t x0 = static_cast<size_t>(fx);
            size_t x1 = std::min(x0 + 1, visionWidth_ - 1);
            float tx = fx - x0;
            
            size_t idx = y * newWidth + x;
            float val = (1.0f - ty) * ((1.0f - tx) * vision_[y0 * visionWidth_ + x0] +
                                       tx * vision_[y0 * visionWidth_ + x1]) +
                        ty * ((1.0f - tx) * vision_[y1 * visionWidth_ + x0] +
                               tx * vision_[y1 * visionWidth_ + x1]);
            resized[idx] = val;
        }
    }
    
    vision_.swap(resized);
}

void SensoryPercept::addVisionOffset(float offset) {
    for (float& value : vision_) {
        value += offset;
        value = std::clamp(value, 0.0f, 1.0f);
    }
}

void SensoryPercept::addVisionNoise(float stdDev) {
    if (stdDev <= 0.0f) return;
    
    std::mt19937 rng(std::random_device{}());
    std::normal_distribution<float> dist(0.0f, stdDev);
    
    for (float& value : vision_) {
        value += dist(rng);
        value = std::clamp(value, 0.0f, 1.0f);
    }
}

void SensoryPercept::smoothVision(float alpha) {
    if (alpha <= 0.0f || alpha >= 1.0f) return;
    
    // Simple exponential moving average
    float avg = getTotalVisionMagnitude();
    
    for (float& value : vision_) {
        value = alpha * value + (1.0f - alpha) * avg;
    }
}

float SensoryPercept::getVisionCorrelation(const SensoryPercept& other) const {
    if (vision_.size() != other.vision_.size()) return 0.0f;
    
    float sum1 = std::accumulate(vision_.begin(), vision_.end(), 0.0f);
    float sum2 = std::accumulate(other.vision_.begin(), other.vision_.end(), 0.0f);
    
    float mean1 = sum1 / vision_.size();
    float mean2 = sum2 / other.vision_.size();
    
    float numerator = 0.0f;
    float denom1 = 0.0f;
    float denom2 = 0.0f;
    
    for (size_t i = 0; i < vision_.size(); ++i) {
        float diff1 = vision_[i] - mean1;
        float diff2 = other.vision_[i] - mean2;
        numerator += diff1 * diff2;
        denom1 += diff1 * diff1;
        denom2 += diff2 * diff2;
    }
    
    if (denom1 < 1e-6f || denom2 < 1e-6f) return 0.0f;
    
    return numerator / std::sqrt(denom1 * denom2);
}

void SensoryPercept::copyFrom(const SensoryPercept& other) {
    vision_ = other.vision_;
    visionWidth_ = other.visionWidth_;
    visionHeight_ = other.visionHeight_;
    
    touch_ = other.touch_;
    internal_ = other.internal_;
    proprioception_ = other.proprioception_;
    audio_ = other.audio_;
    
    timestamp_ = other.timestamp_;
}

bool SensoryPercept::equals(const SensoryPercept& other) const {
    return vision_ == other.vision_ &&
           touch_ == other.touch_ &&
           internal_ == other.internal_ &&
           proprioception_ == other.proprioception_ &&
           audio_ == other.audio_ &&
           std::abs(timestamp_ - other.timestamp_) < 1e-6f;
}

} // namespace nlm
