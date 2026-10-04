#include "SensoryPercept.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>

namespace nlm {

SensoryPercept::SensoryPercept(const Config& config)
    : config_(config)
    , timestamp_(0.0)
{
    // Initialize signal vectors with config values
    vision_.resize(config_.VISION_WIDTH * config_.VISION_HEIGHT, 0.0f);
    touch_.resize(config_.TOUCH_COUNT, 0.0f);
    internal_.resize(config_.INTERNAL_COUNT, 0.0f);
    proprioception_.resize(config_.PROPRIOCEPT_COUNT, 0.0f);
    audio_.resize(config_.AUDIO_COUNT, 0.0f);
    
    // Initialize signal history
    signalHistory_.initialize(config_.HISTORY_SIZE);
}

SensoryPercept::~SensoryPercept() = default;

// Copy constructor for deep copy semantics
SensoryPercept::SensoryPercept(const SensoryPercept& other)
    : config_(other.config_)
    , timestamp_(other.timestamp_)
{
    vision_ = other.vision_;
    touch_ = other.touch_;
    internal_ = other.internal_;
    proprioception_ = other.proprioception_;
    audio_ = other.audio_;
    signalHistory_ = other.signalHistory_;
}

// Assignment operator for deep copy semantics
SensoryPercept& SensoryPercept::operator=(const SensoryPercept& other) {
    if (this != &other) {
        config_ = other.config_;
        timestamp_ = other.timestamp_;
        vision_ = other.vision_;
        touch_ = other.touch_;
        internal_ = other.internal_;
        proprioception_ = other.proprioception_;
        audio_ = other.audio_;
        signalHistory_ = other.signalHistory_;
    }
    return *this;
}

// Move constructor for efficient transfer
SensoryPercept::SensoryPercept(SensoryPercept&& other) noexcept
    : config_(std::move(other.config_))
    , timestamp_(other.timestamp_)
{
    vision_ = std::move(other.vision_);
    touch_ = std::move(other.touch_);
    internal_ = std::move(other.internal_);
    proprioception_ = std::move(other.proprioception_);
    audio_ = std::move(other.audio_);
    signalHistory_ = std::move(other.signalHistory_);
    allSignalsCache_ = std::move(other.allSignalsCache_);
    allSignalsCacheValid_ = other.allSignalsCacheValid_;
    other.allSignalsCacheValid_ = false;
}

// Move assignment operator for efficient transfer
SensoryPercept& SensoryPercept::operator=(SensoryPercept&& other) noexcept {
    if (this != &other) {
        config_ = std::move(other.config_);
        timestamp_ = other.timestamp_;
        vision_ = std::move(other.vision_);
        touch_ = std::move(other.touch_);
        internal_ = std::move(other.internal_);
        proprioception_ = std::move(other.proprioception_);
        audio_ = std::move(other.audio_);
        signalHistory_ = std::move(other.signalHistory_);
        allSignalsCache_ = std::move(other.allSignalsCache_);
        allSignalsCacheValid_ = other.allSignalsCacheValid_;
        other.allSignalsCacheValid_ = false;
    }
    return *this;
}

void SensoryPercept::setVision(const std::vector<float>& v) {
    ensureCapacity(vision_, v);
    vision_ = v;
    invalidateCache();
}

void SensoryPercept::setVision(std::vector<float>&& v) noexcept {
    ensureCapacity(vision_, v);
    vision_ = std::move(v);
    invalidateCache();
}

void SensoryPercept::setTouch(const std::vector<float>& t) {
    ensureCapacity(touch_, t);
    touch_ = t;
    invalidateCache();
}

void SensoryPercept::setTouch(std::vector<float>&& t) noexcept {
    ensureCapacity(touch_, t);
    touch_ = std::move(t);
    invalidateCache();
}

void SensoryPercept::setInternal(const std::vector<float>& i) {
    ensureCapacity(internal_, i);
    internal_ = i;
    invalidateCache();
}

void SensoryPercept::setInternal(std::vector<float>&& i) noexcept {
    ensureCapacity(internal_, i);
    internal_ = std::move(i);
    invalidateCache();
}

void SensoryPercept::setProprioception(const std::vector<float>& p) {
    ensureCapacity(proprioception_, p);
    proprioception_ = p;
    invalidateCache();
}

void SensoryPercept::setProprioception(std::vector<float>&& p) noexcept {
    ensureCapacity(proprioception_, p);
    proprioception_ = std::move(p);
    invalidateCache();
}

void SensoryPercept::setAudio(const std::vector<float>& a) {
    ensureCapacity(audio_, a);
    audio_ = a;
    invalidateCache();
}

void SensoryPercept::setAudio(std::vector<float>&& a) noexcept {
    ensureCapacity(audio_, a);
    audio_ = std::move(a);
    invalidateCache();
}

std::vector<float> SensoryPercept::getAllSignals() const {
    if (!allSignalsCacheValid_) {
        updateCache();
    }
    return allSignalsCache_;
}

std::array<float, SensoryPercept::Config::VISION_WIDTH * SensoryPercept::Config::VISION_HEIGHT> 
SensoryPercept::getVisionArray() const {
    std::array<float, Config::VISION_WIDTH * Config::VISION_HEIGHT> result{};
    size_t copySize = std::min(vision_.size(), result.size());
    std::copy(vision_.begin(), vision_.begin() + copySize, result.begin());
    return result;
}

std::array<float, SensoryPercept::Config::TOUCH_COUNT> SensoryPercept::getTouchArray() const {
    std::array<float, Config::TOUCH_COUNT> result{};
    size_t copySize = std::min(touch_.size(), result.size());
    std::copy(touch_.begin(), touch_.begin() + copySize, result.begin());
    return result;
}

std::array<float, SensoryPercept::Config::INTERNAL_COUNT> SensoryPercept::getInternalArray() const {
    std::array<float, Config::INTERNAL_COUNT> result{};
    size_t copySize = std::min(internal_.size(), result.size());
    std::copy(internal_.begin(), internal_.begin() + copySize, result.begin());
    return result;
}

std::array<float, SensoryPercept::Config::PROPRIOCEPT_COUNT> 
SensoryPercept::getProprioceptionArray() const {
    std::array<float, Config::PROPRIOCEPT_COUNT> result{};
    size_t copySize = std::min(proprioception_.size(), result.size());
    std::copy(proprioception_.begin(), proprioception_.begin() + copySize, result.begin());
    return result;
}

bool SensoryPercept::validateVision() const {
    return validateSignalRange(vision_, config_.visionMin, config_.visionMax);
}

bool SensoryPercept::validateTouch() const {
    return validateSignalRange(touch_, config_.touchMin, config_.touchMax);
}

bool SensoryPercept::validateInternal() const {
    return validateSignalRange(internal_, config_.internalMin, config_.internalMax);
}

bool SensoryPercept::validateProprioception() const {
    return validateSignalRange(proprioception_, config_.proprioceptionMin, config_.proprioceptionMax);
}

void SensoryPercept::normalizeVision() {
    normalizeSignal(vision_, config_.visionMin, config_.visionMax);
    invalidateCache();
}

void SensoryPercept::normalizeTouch() {
    normalizeSignal(touch_, config_.touchMin, config_.touchMax);
    invalidateCache();
}

void SensoryPercept::normalizeInternal() {
    normalizeSignal(internal_, config_.internalMin, config_.internalMax);
    invalidateCache();
}

void SensoryPercept::normalizeProprioception() {
    normalizeSignal(proprioception_, config_.proprioceptionMin, config_.proprioceptionMax);
    invalidateCache();
}

void SensoryPercept::applySmoothing(float alpha) {
    if (alpha <= 0.0f || alpha >= 1.0f) return;
    
    // Apply exponential moving average smoothing
    if (!vision_.empty()) {
        float smoothed = vision_[0] * alpha;
        for (size_t i = 1; i < vision_.size(); ++i) {
            smoothed = vision_[i] * alpha + (1.0f - alpha) * smoothed;
            vision_[i] = smoothed;
        }
    }
    
    applySignalFilter(touch_, alpha);
    applySignalFilter(internal_, alpha);
    applySignalFilter(proprioception_, alpha);
    
    invalidateCache();
}

void SensoryPercept::applyGain(float factor) {
    if (factor <= 0.0f) return;
    
    std::transform(vision_.begin(), vision_.end(), vision_.begin(),
                   [factor](float val) { return val * factor; });
    std::transform(touch_.begin(), touch_.end(), touch_.begin(),
                   [factor](float val) { return val * factor; });
    std::transform(internal_.begin(), internal_.end(), internal_.begin(),
                   [factor](float val) { return val * factor; });
    std::transform(proprioception_.begin(), proprioception_.end(), proprioception_.begin(),
                   [factor](float val) { return val * factor; });
    
    invalidateCache();
}

void SensoryPercept::applyBias(float bias) {
    if (bias == 0.0f) return;
    
    std::transform(vision_.begin(), vision_.end(), vision_.begin(),
                   [bias](float val) { return val + bias; });
    std::transform(touch_.begin(), touch_.end(), touch_.begin(),
                   [bias](float val) { return val + bias; });
    std::transform(internal_.begin(), internal_.end(), internal_.begin(),
                   [bias](float val) { return val + bias; });
    std::transform(proprioception_.begin(), proprioception_.end(), proprioception_.begin(),
                   [bias](float val) { return val + bias; });
    
    invalidateCache();
}

float SensoryPercept::computeVisionDifference(const SensoryPercept& other) const {
    return vectorDifference(vision_, other.vision_);
}

float SensoryPercept::computeTotalDifference(const SensoryPercept& other) const {
    float totalDiff = 0.0f;
    totalDiff += computeVisionDifference(other);
    totalDiff += vectorDifference(touch_, other.touch_);
    totalDiff += vectorDifference(internal_, other.internal_);
    totalDiff += vectorDifference(proprioception_, other.proprioception_);
    totalDiff += vectorDifference(audio_, other.audio_);
    return totalDiff;
}

void SensoryPercept::clear() {
    vision_.clear();
    touch_.clear();
    internal_.clear();
    proprioception_.clear();
    audio_.clear();
    invalidateCache();
}

void SensoryPercept::resizeToFit() {
    vision_.shrink_to_fit();
    touch_.shrink_to_fit();
    internal_.shrink_to_fit();
    proprioception_.shrink_to_fit();
    audio_.shrink_to_fit();
}

size_t SensoryPercept::memoryUsage() const {
    size_t total = 0;
    total += vision_.capacity() * sizeof(float);
    total += touch_.capacity() * sizeof(float);
    total += internal_.capacity() * sizeof(float);
    total += proprioception_.capacity() * sizeof(float);
    total += audio_.capacity() * sizeof(float);
    return total;
}

void SensoryPercept::reset() {
    std::fill(vision_.begin(), vision_.end(), 0.0f);
    std::fill(touch_.begin(), touch_.end(), 0.0f);
    std::fill(internal_.begin(), internal_.end(), 0.0f);
    std::fill(proprioception_.begin(), proprioception_.end(), 0.0f);
    std::fill(audio_.begin(), audio_.end(), 0.0f);
    
    signalHistory_.visionHistory.clear();
    signalHistory_.touchHistory.clear();
    signalHistory_.internalHistory.clear();
    signalHistory_.proprioceptionHistory.clear();
    
    invalidateCache();
}

void SensoryPercept::signalHistory_.update(const std::vector<float>& vision, const std::vector<float>& touch,
                                         const std::vector<float>& internal, const std::vector<float>& proprioception) {
    if (visionHistory.size() < historySize) {
        visionHistory.push_back(vision);
        touchHistory.push_back(touch);
        internalHistory.push_back(internal);
        proprioceptionHistory.push_back(proprioception);
    } else {
        // Rotate buffer
        for (size_t i = 1; i < visionHistory.size(); ++i) {
            visionHistory[i-1] = visionHistory[i];
            touchHistory[i-1] = touchHistory[i];
            internalHistory[i-1] = internalHistory[i];
            proprioceptionHistory[i-1] = proprioceptionHistory[i];
        }
        visionHistory.back() = vision;
        touchHistory.back() = touch;
        internalHistory.back() = internal;
        proprioceptionHistory.back() = proprioception;
    }
}

float SensoryPercept::signalHistory_.computeTemporalDifference(const std::vector<float>& current, 
                                                             const std::vector<float>& previous) const {
    if (current.empty() || previous.empty()) return 0.0f;
    
    float totalDiff = 0.0f;
    size_t minSize = std::min(current.size(), previous.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        totalDiff += std::abs(current[i] - previous[i]);
    }
    
    return totalDiff / static_cast<float>(minSize);
}

void SensoryPercept::ensureCapacity(std::vector<float>& container, const std::vector<float>& signal) {
    if (signal.size() > container.capacity()) {
        container.reserve(signal.size() * 1.2f); // Reserve extra capacity for future growth
    }
}

void SensoryPercept::updateCache() const {
    allSignalsCache_.clear();
    
    // Efficient concatenation using insert
    allSignalsCache_.insert(allSignalsCache_.end(), vision_.begin(), vision_.end());
    allSignalsCache_.insert(allSignalsCache_.end(), touch_.begin(), touch_.end());
    allSignalsCache_.insert(allSignalsCache_.end(), internal_.begin(), internal_.end());
    allSignalsCache_.insert(allSignalsCache_.end(), proprioception_.begin(), proprioception_.end());
    allSignalsCache_.insert(allSignalsCache_.end(), audio_.begin(), audio_.end());
    
    allSignalsCacheValid_ = true;
}

void SensoryPercept::invalidateCache() {
    allSignalsCacheValid_ = false;
}

bool SensoryPercept::validateSignalRange(const std::vector<float>& signal, float min, float max) const {
    if (min >= max) return true; // Invalid range, skip validation
    
    for (float val : signal) {
        if (val < min || val > max) {
            return false;
        }
    }
    return true;
}

float SensoryPercept::computeSignalQuality(const std::vector<float>& signal, float threshold) const {
    if (signal.empty()) return 0.0f;
    
    float variance = 0.0f;
    float mean = std::accumulate(signal.begin(), signal.end(), 0.0f) / signal.size();
    
    for (float val : signal) {
        variance += (val - mean) * (val - mean);
    }
    variance /= signal.size();
    
    // Quality based on variance and threshold
    float quality = std::min(1.0f, variance / (threshold * threshold + 1e-6f));
    return quality;
}

void SensoryPercept::normalizeSignal(std::vector<float>& signal, float min, float max) {
    if (signal.empty()) return;
    
    float actualMin = *std::min_element(signal.begin(), signal.end());
    float actualMax = *std::max_element(signal.begin(), signal.end());
    
    if (actualMax - actualMin < 1e-6f) {
        // All values are the same, don't normalize
        return;
    }
    
    for (float& val : signal) {
        val = (val - actualMin) / (actualMax - actualMin);
    }
}

void SensoryPercept::applySignalFilter(std::vector<float>& signal, float alpha) {
    if (signal.empty() || alpha <= 0.0f || alpha >= 1.0f) return;
    
    float filtered = signal[0] * alpha;
    for (size_t i = 1; i < signal.size(); ++i) {
        filtered = signal[i] * alpha + (1.0f - alpha) * filtered;
        signal[i] = filtered;
    }
}

float SensoryPercept::vectorDifference(const std::vector<float>& a, const std::vector<float>& b) {
    if (a.empty() || b.empty()) return 0.0f;
    
    float totalDiff = 0.0f;
    size_t minSize = std::min(a.size(), b.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        totalDiff += std::abs(a[i] - b[i]);
    }
    
    return totalDiff;
}

} // namespace nlm