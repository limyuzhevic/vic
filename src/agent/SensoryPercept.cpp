#include "SensoryPercept.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <queue>
#include <unordered_map>

namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
{
    // Initialize perceptual buffers with enhanced features
    vision_.resize(visionWidth_ * visionHeight_, 0.0f);
    touch_.resize(8, 0.0f);
    internal_.resize(4, 0.0f);
    proprioception_.resize(6, 0.0f);
    audio_.resize(32, 0.0f); // Increased audio feature dimensionality
    
    // Add prediction error buffers
    visionPredictionError_.resize(visionWidth_ * visionHeight_, 0.0f);
    audioPredictionError_.resize(32, 0.0f);
    internalPredictionError_.resize(4, 0.0f);
    
    // Add cross-modal integration buffers
    crossModalIntegration_.resize(4, std::vector<float>(visionWidth_ * visionHeight_, 0.0f));
    attentionWeights_.resize(4, 0.0f); // Vision, Touch, Internal, Audio
    
    // Initialize with placeholder enhanced values
    initializeEnhancedPerception();
}

void SensoryPercept::initializeEnhancedPerception() {
    // Initialize with biologically plausible baseline values
    // Vision: central field higher resolution
    float centralWeight = 2.0f;
    float peripheralWeight = 0.5f;
    
    for (size_t y = 0; y < visionHeight_; ++y) {
        for (size_t x = 0; x < visionWidth_; ++x) {
            float distance = std::sqrt(std::pow(static_cast<float>(x) - static_cast<float>(visionWidth_) / 2.0f, 2) +
                                      std::pow(static_cast<float>(y) - static_cast<float>(visionHeight_) / 2.0f, 2));
            float maxDist = std::sqrt(std::pow(static_cast<float>(visionWidth_) / 2.0f, 2) +
                                     std::pow(static_cast<float>(visionHeight_) / 2.0f, 2));
            
            float weight = (distance < maxDist / 2.0f) ? centralWeight : peripheralWeight;
            vision_[y * visionWidth_ + x] = weight / maxDist;
        }
    }
    
    // Touch: include pressure and proximity
    touch_[0] = 0.8f; // Pressure
    touch_[1] = 0.3f; // Light touch
    touch_[2] = 0.6f; // Vibration
    touch_[3] = 0.2f; // Temperature
    touch_[4] = 0.4f; // Pain
    touch_[5] = 0.1f; // Itch
    touch_[6] = 0.7f; // Proprioceptive feedback
    touch_[7] = 0.5f; // Joint position
    
    // Internal: include homeostatic signals
    internal_[0] = 0.7f; // Energy level
    internal_[1] = 0.5f; // Hunger
    internal_[2] = 0.3f; // Thirst
    internal_[3] = 0.8f; // Alertness
    
    // Proprioception: body position and velocity
    proprioception_[0] = 0.0f; // Head x
    proprioception_[1] = 0.0f; // Head y
    proprioception_[2] = 0.0f; // Head z
    proprioception_[3] = 0.0f; // Torso roll
    proprioception_[4] = 0.0f; // Torso pitch
    proprioception_[5] = 0.0f; // Limb velocity
    
    // Audio: include spectral features
    audio_[0] = 0.0f;  // Low frequency energy
    audio_[1] = 0.3f;  // Mid frequency energy
    audio_[2] = 0.7f;  // High frequency energy
    audio_[3] = 0.5f;  // Speech formants
    audio_[4] = 0.2f;  // Environmental sounds
    audio_[5] = 0.1f;  // Music
    audio_[6] = 0.0f;  // Silence
    audio_[7] = 0.4f;  // Directional cues
    audio_[8] = 0.6f;  // Temporal patterns
    audio_[9] = 0.3f;  // Rhythm
    audio_[10] = 0.1f; // Melody
    audio_[11] = 0.2f; // Harmony
    audio_[12] = 0.0f; // Drone
    audio_[13] = 0.5f; // Binaural cues
    audio_[14] = 0.3f; // Interaural level difference
    audio_[15] = 0.2f; // Interaural time difference
    audio_[16] = 0.4f; // Pitch
    audio_[17] = 0.1f; // Timbre
    audio_[18] = 0.3f; // Attack
    audio_[19] = 0.2f; // Decay
    audio_[20] = 0.1f; // Sustain
    audio_[21] = 0.0f; // Release
    audio_[22] = 0.5f; // loudness
    audio_[23] = 0.3f; // Sharpness
    audio_[24] = 0.2f; // Warmth
    audio_[25] = 0.1f; // Brightness
    audio_[26] = 0.4f; // Complex
    audio_[27] = 0.3f; // Simple
    audio_[28] = 0.2f; // Fast
    audio_[29] = 0.1f; // Slow
    audio_[30] = 0.5f; // Regular
    audio_[31] = 0.0f; // Irregular
}

std::vector<float> SensoryPercept::getAllSignals() const {
    std::vector<float> all;
    
    // Vision: multi-scale representation (flattened)
    all.insert(all.end(), vision_.begin(), vision_.end());
    
    // Add enhanced vision features (edges, saliency, etc.)
    for (size_t i = 0; i < visionPredictionError_.size() && i < vision_.size(); ++i) {
        all.push_back(visionPredictionError_[i]);
    }
    
    // Touch
    all.insert(all.end(), touch_.begin(), touch_.end());
    
    // Internal (enhanced)
    all.insert(all.end(), internal_.begin(), internal_.end());
    for (size_t i = 0; i < internalPredictionError_.size() && i < internal_.size(); ++i) {
        all.push_back(internalPredictionError_[i]);
    }
    
    // Proprioception
    all.insert(all.end(), proprioception_.begin(), proprioception_.end());
    
    // Audio (enhanced)
    all.insert(all.end(), audio_.begin(), audio_.end());
    
    // Add audio prediction errors
    for (size_t i = 0; i < audioPredictionError_.size() && i < audio_.size(); ++i) {
        all.push_back(audioPredictionError_[i]);
    }
    
    return all;
}

void SensoryPercept::updateCrossModalIntegration() {
    // Update cross-modal integration weights
    attentionWeights_.clear();
    
    // Calculate attention based on signal variance (more variance = more attention)
    float visionAttention = calculateSignalVariance(vision_) * 0.4f;
    float touchAttention = calculateSignalVariance(touch_) * 0.2f;
    float internalAttention = calculateSignalVariance(internal_) * 0.2f;
    float audioAttention = calculateSignalVariance(audio_) * 0.2f;
    
    // Normalize attention weights
    float totalAttention = visionAttention + touchAttention + internalAttention + audioAttention;
    if (totalAttention > 0.0f) {
        attentionWeights_.push_back(visionAttention / totalAttention);
        attentionWeights_.push_back(touchAttention / totalAttention);
        attentionWeights_.push_back(internalAttention / totalAttention);
        attentionWeights_.push_back(audioAttention / totalAttention);
    } else {
        attentionWeights_ = {0.4f, 0.2f, 0.2f, 0.2f}; // Default weights
    }
}

void SensoryPercept::updatePredictionErrors() {
    // Update prediction errors for each modality
    // This simulates forward model prediction errors
    
    // Vision prediction errors (compare with memory)
    for (size_t i = 0; i < vision_.size(); ++i) {
        // Simple prediction: average of neighbors in history
        float predicted = computeVisionPrediction(i);
        visionPredictionError_[i] = std::abs(vision_[i] - predicted);
    }
    
    // Audio prediction errors
    for (size_t i = 0; i < audio_.size(); ++i) {
        // Predict based on temporal patterns
        float predicted = computeAudioPrediction(i);
        audioPredictionError_[i] = std::abs(audio_[i] - predicted);
    }
    
    // Internal prediction errors
    for (size_t i = 0; i < internal_.size(); ++i) {
        // Predict based on homeostatic needs
        float predicted = computeInternalPrediction(i);
        internalPredictionError_[i] = std::abs(internal_[i] - predicted);
    }
}

float SensoryPercept::calculateSignalVariance(const std::vector<float>& signal) const {
    if (signal.size() < 2) return 0.0f;
    
    float mean = std::accumulate(signal.begin(), signal.end(), 0.0f) / signal.size();
    float variance = 0.0f;
    
    for (float val : signal) {
        variance += (val - mean) * (val - mean);
    }
    
    return variance / signal.size();
}

float SensoryPercept::computeVisionPrediction(size_t index) const {
    // Simple prediction based on spatial context
    size_t x = index % visionWidth_;
    size_t y = index / visionWidth_;
    
    // Average of surrounding pixels (with weighting)
    float sum = 0.0f;
    float weightSum = 0.0f;
    
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            size_t nx = x + dx;
            size_t ny = y + dy;
            
            if (nx < visionWidth_ && ny < visionHeight_) {
                size_t nIndex = ny * visionWidth_ + nx;
                float weight = 1.0f / (std::sqrt(static_cast<float>(dx * dx + dy * dy)) + 1.0f);
                sum += vision_[nIndex] * weight;
                weightSum += weight;
            }
        }
    }
    
    return weightSum > 0.0f ? sum / weightSum : 0.5f;
}

float SensoryPercept::computeAudioPrediction(size_t index) const {
    // Predict based on temporal patterns (simplified)
    // In a real implementation, this would use a proper forward model
    
    // Use spectral and temporal context
    float prediction = 0.0f;
    size_t contextSize = std::min(static_cast<size_t>(4), audio_.size());
    
    // Combine current and previous values
    for (size_t i = 0; i < contextSize && index >= i; ++i) {
        prediction += audio_[index - i] * 0.5f; // Weight decay over time
    }
    
    // Add spectral context (even/odd frequency bands)
    if (index < audio_.size()) {
        size_t freqBand = index % 4;
        if (freqBand == 0) prediction += audio_[index] * 0.3f; // Low freq
        else if (freqBand == 1) prediction += audio_[index] * 0.6f; // Mid freq
        else if (freqBand == 2) prediction += audio_[index] * 0.9f; // High freq
        else prediction += audio_[index] * 0.1f; // Very high freq
    }
    
    return prediction / contextSize;
}

float SensoryPercept::computeInternalPrediction(size_t index) const {
    // Predict internal state based on homeostatic drives
    // and prediction error history
    
    float prediction = 0.5f; // Default neutral value
    
    // Modulate based on index
    switch (index % 4) {
        case 0: // Energy level
            prediction = 0.7f; // Usually high energy
            break;
        case 1: // Hunger
            prediction = 0.4f; // Moderate hunger
            break;
        case 2: // Thirst
            prediction = 0.3f; // Moderate thirst
            break;
        case 3: // Alertness
            prediction = 0.8f; // Usually alert
            break;
    }
    
    return prediction;
}

const std::vector<float>& SensoryPercept::getVision() const { return vision_; }
void SensoryPercept::setVision(const std::vector<float>& v) { vision_ = v; }
size_t SensoryPercept::getVisionWidth() const { return visionWidth_; }
size_t SensoryPercept::getVisionHeight() const { return visionHeight_; }

const std::vector<float>& SensoryPercept::getTouch() const { return touch_; }
void SensoryPercept::setTouch(const std::vector<float>& t) { touch_ = t; }

const std::vector<float>& SensoryPercept::getInternal() const { return internal_; }
void SensoryPercept::setInternal(const std::vector<float>& i) { internal_ = i; }

const std::vector<float>& SensoryPercept::getProprioception() const { return proprioception_; }
void SensoryPercept::setProprioception(const std::vector<float>& p) { proprioception_ = p; }

const std::vector<float>& SensoryPercept::getAudio() const { return audio_; }
void SensoryPercept::setAudio(const std::vector<float>& a) { audio_ = a; }

double SensoryPercept::getTimestamp() const { return timestamp_; }
void SensoryPercept::setTimestamp(double t) { timestamp_ = t; }

} // namespace nlm
