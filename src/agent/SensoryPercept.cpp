#include "SensoryPercept.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
    , audioEnabled_(false)
    , audioProcessed_(false)
    , audioChannels_(0)
    , audioScale_(1.0f)
    , audioNormalized_(false)
    , audioDuration_(0.0)
    , audioSampleRate_(44100)
    , visionProcessed_(false)
    , touchProcessed_(false)
    , internalProcessed_(false)
    , proprioceptionProcessed_(false)
    , configFilePath_("")
    , useConfigFile_(false)
{
    vision_.resize(16 * 16, 0.0f);
    previousVision_.resize(16 * 16, 0.0f);
    touch_.resize(8, 0.0f);
    internal_.resize(4, 0.0f);
    proprioception_.resize(6, 0.0f);
    audio_.resize(0, 0.0f);
    processedAudio_.resize(0, 0.0f);
    audioFeatures_.resize(0, 0.0f);
    processedVision_.resize(16 * 16, 0.0f);
    processedTouch_.resize(8, 0.0f);
    processedInternal_.resize(4, 0.0f);
    processedProprioception_.resize(6, 0.0f);
}

void SensoryPercept::setVisionConfig(float width, float height, float scale, bool normalize) {
    visionWidth_ = static_cast<size_t>(width);
    visionHeight_ = static_cast<size_t>(height);
    vision_.resize(visionWidth_ * visionHeight_, 0.0f);
    previousVision_.resize(visionWidth_ * visionHeight_, 0.0f);
    processedVision_.resize(visionWidth_ * visionHeight_, 0.0f);
    visionProcessed_ = !normalize;
}

void SensoryPercept::setTouchConfig(float scale, bool normalize) {
    touch_.resize(8, 0.0f);
    processedTouch_.resize(8, 0.0f);
    touchProcessed_ = !normalize;
}

void SensoryPercept::setInternalConfig(float scale, bool normalize) {
    internal_.resize(4, 0.0f);
    processedInternal_.resize(4, 0.0f);
    internalProcessed_ = !normalize;
}

void SensoryPercept::setProprioceptionConfig(float scale, bool normalize) {
    proprioception_.resize(6, 0.0f);
    processedProprioception_.resize(6, 0.0f);
    proprioceptionProcessed_ = !normalize;
}

void SensoryPercept::setAudioConfig(size_t channels, float scale, bool normalize) {
    audioEnabled_ = true;
    audioChannels_ = channels;
    audioScale_ = scale;
    audioNormalized_ = normalize;
    audio_.resize(channels, 0.0f);
    processedAudio_.resize(channels, 0.0f);
    audioFeatures_.resize(0, 0.0f);
}

void SensoryPercept::adjustVisionDimensions(size_t width, size_t height) {
    visionWidth_ = width;
    visionHeight_ = height;
    size_t newSize = width * height;
    vision_.resize(newSize, 0.0f);
    previousVision_.resize(newSize, 0.0f);
    processedVision_.resize(newSize, 0.0f);
    visionProcessed_ = false;
}

void SensoryPercept::adjustTouchDimensions(size_t points) {
    touch_.resize(points, 0.0f);
    processedTouch_.resize(points, 0.0f);
    touchProcessed_ = false;
}

void SensoryPercept::adjustInternalDimensions(size_t signals) {
    internal_.resize(signals, 0.0f);
    processedInternal_.resize(signals, 0.0f);
    internalProcessed_ = false;
}

void SensoryPercept::adjustProprioceptionDimensions(size_t points) {
    proprioception_.resize(points, 0.0f);
    processedProprioception_.resize(points, 0.0f);
    proprioceptionProcessed_ = false;
}

void SensoryPercept::normalizeVision() {
    if (!visionProcessed_ && !vision_.empty()) {
        float maxVal = *std::max_element(vision_.begin(), vision_.end());
        if (maxVal > 0.0f) {
            float scale = 1.0f / maxVal;
            for (float& val : vision_) val *= scale;
        }
        visionProcessed_ = true;
    }
}

void SensoryPercept::normalizeTouch() {
    if (!touchProcessed_ && !touch_.empty()) {
        float maxVal = *std::max_element(touch_.begin(), touch_.end());
        if (maxVal > 0.0f) {
            float scale = 1.0f / maxVal;
            for (float& val : touch_) val *= scale;
        }
        touchProcessed_ = true;
    }
}

void SensoryPercept::normalizeInternal() {
    if (!internalProcessed_ && !internal_.empty()) {
        float maxVal = *std::max_element(internal_.begin(), internal_.end());
        if (maxVal > 0.0f) {
            float scale = 1.0f / maxVal;
            for (float& val : internal_) val *= scale;
        }
        internalProcessed_ = true;
    }
}

void SensoryPercept::normalizeProprioception() {
    if (!proprioceptionProcessed_ && !proprioception_.empty()) {
        float maxVal = *std::max_element(proprioception_.begin(), proprioception_.end());
        if (maxVal > 0.0f) {
            float scale = 1.0f / maxVal;
            for (float& val : proprioception_) val *= scale;
        }
        proprioceptionProcessed_ = true;
    }
}

void SensoryPercept::normalizeAudio() {
    if (!audioNormalized_ && !audio_.empty()) {
        float maxVal = *std::max_element(audio_.begin(), audio_.end());
        if (maxVal > 0.0f) {
            float scale = 1.0f / maxVal;
            for (float& val : audio_) val *= scale;
        }
        audioNormalized_ = true;
    }
}

void SensoryPercept::scaleVision(float scale) {
    for (float& val : vision_) val *= scale;
    visionProcessed_ = false;
}

void SensoryPercept::scaleTouch(float scale) {
    for (float& val : touch_) val *= scale;
    touchProcessed_ = false;
}

void SensoryPercept::scaleInternal(float scale) {
    for (float& val : internal_) val *= scale;
    internalProcessed_ = false;
}

void SensoryPercept::scaleProprioception(float scale) {
    for (float& val : proprioception_) val *= scale;
    proprioceptionProcessed_ = false;
}

void SensoryPercept::scaleAudio(float scale) {
    for (float& val : audio_) val *= scale;
    audioNormalized_ = false;
}

void SensoryPercept::processAudioInternal() {
    if (!audioEnabled_ || audio_.empty()) return;
    
    audioProcessed_ = true;
    processedAudio_ = audio_;
    
    applyEnvelopeDetection(processedAudio_);
    applySpectralFeatures(processedAudio_);
    applyTemporalSmoothing(processedAudio_);
    
    audioFeatures_ = extractFrequencyFeatures(audio_);
}

void SensoryPercept::applyEnvelopeDetection(std::vector<float>& signal) {
    if (signal.empty()) return;
    
    float maxVal = *std::max_element(signal.begin(), signal.end());
    if (maxVal > 0.0f) {
        for (size_t i = 1; i < signal.size() - 1; ++i) {
            float left = std::abs(signal[i] - signal[i-1]);
            float right = std::abs(signal[i] - signal[i+1]);
            signal[i] = (signal[i] + left + right) / 3.0f;
        }
    }
}

void SensoryPercept::applySpectralFeatures(std::vector<float>& signal) {
    if (signal.size() < 2) return;
    
    std::vector<float> fftFeatures(signal.size() / 2);
    size_t halfSize = std::min(size_t(256), signal.size() / 2);
    
    for (size_t i = 0; i < halfSize; ++i) {
        float magnitude = 0.0f;
        for (size_t j = 0; j < signal.size(); j += 2) {
            float angle = 2.0f * M_PI * i * j / signal.size();
            magnitude += signal[j] * std::cos(angle);
            magnitude += signal[j+1] * std::sin(angle);
        }
        fftFeatures[i] = magnitude / signal.size();
    }
    
    signal = fftFeatures;
}

void SensoryPercept::applyTemporalSmoothing(std::vector<float>& signal) {
    if (signal.size() < 3) return;
    
    std::vector<float> smoothed(signal.size());
    float weight = 0.3f;
    
    smoothed[0] = signal[0];
    for (size_t i = 1; i < signal.size() - 1; ++i) {
        smoothed[i] = (1.0f - weight) * signal[i] + weight * smoothed[i-1];
    }
    smoothed[signal.size() - 1] = signal[signal.size() - 1];
    
    signal = smoothed;
}

void SensoryPercept::applyNoiseReduction(std::vector<float>& signal, float threshold) {
    if (signal.empty()) return;
    
    float median = 0.0f;
    std::vector<float> sorted(signal);
    std::sort(sorted.begin(), sorted.end());
    size_t mid = sorted.size() / 2;
    median = sorted[mid];
    
    for (float& val : signal) {
        if (std::abs(val - median) > threshold) {
            val = median;
        }
    }
}

std::vector<float> SensoryPercept::extractFrequencyFeatures(const std::vector<float>& audio) const {
    std::vector<float> features;
    
    if (audio.size() < 4) return features;
    
    // Simple frequency domain features
    float mean = std::accumulate(audio.begin(), audio.end(), 0.0f) / audio.size();
    float variance = 0.0f;
    for (float val : audio) variance += (val - mean) * (val - mean);
    variance /= audio.size();
    
    float rms = std::sqrt(variance);
    features.push_back(mean);
    features.push_back(variance);
    features.push_back(rms);
    
    // Zero-crossing rate
    size_t zeroCrossings = 0;
    for (size_t i = 1; i < audio.size(); ++i) {
        if ((audio[i-1] > 0.0f && audio[i] < 0.0f) || (audio[i-1] < 0.0f && audio[i] > 0.0f)) {
            zeroCrossings++;
        }
    }
    features.push_back(static_cast<float>(zeroCrossings) / audio.size());
    
    // Spectral centroid approximation
    float spectralCentroid = 0.0f;
    float spectralWeight = 0.0f;
    for (size_t i = 0; i < audio.size(); ++i) {
        spectralWeight += audio[i];
        spectralCentroid += static_cast<float>(i) * audio[i];
    }
    if (spectralWeight > 0.0f) {
        spectralCentroid /= spectralWeight;
    }
    features.push_back(spectralCentroid / audio.size());
    
    return features;
}

std::vector<float> SensoryPercept::extractTemporalFeatures(const std::vector<float>& audio) const {
    std::vector<float> features;
    
    if (audio.size() < 4) return features;
    
    // Temporal derivatives
    float maxSlope = 0.0f;
    for (size_t i = 1; i < audio.size(); ++i) {
        float slope = std::abs(audio[i] - audio[i-1]);
        maxSlope = std::max(maxSlope, slope);
    }
    features.push_back(maxSlope);
    
    // Autocorrelation at lag 1
    float autocorr = 0.0f;
    if (audio.size() > 1) {
        float sum = 0.0f;
        float sumSq = 0.0f;
        for (size_t i = 0; i < audio.size() - 1; ++i) {
            sum += audio[i] * audio[i+1];
            sumSq += audio[i] * audio[i];
        }
        autocorr = sum / std::sqrt(sumSq * (audio.size() - 1));
    }
    features.push_back(autocorr);
    
    // Energy in different frequency bands (approximate)
    float lowFreq = 0.0f, midFreq = 0.0f, highFreq = 0.0f;
    for (size_t i = 0; i < audio.size(); ++i) {
        float freq = static_cast<float>(i) / audio.size();
        if (freq < 0.33f) lowFreq += audio[i];
        else if (freq < 0.66f) midFreq += audio[i];
        else highFreq += audio[i];
    }
    features.push_back(lowFreq);
    features.push_back(midFreq);
    features.push_back(highFreq);
    
    return features;
}

std::vector<float> SensoryPercept::extractSpectralFeatures(const std::vector<float>& audio) const {
    std::vector<float> features;
    
    if (audio.size() < 4) return features;
    
    // Power spectral density approximation
    std::vector<float> psd(64, 0.0f);
    for (size_t i = 0; i < audio.size(); ++i) {
        size_t bin = std::min(i % psd.size(), psd.size() - 1);
        psd[bin] += audio[i] * audio[i];
    }
    
    float totalPower = std::accumulate(psd.begin(), psd.end(), 0.0f);
    float maxPower = *std::max_element(psd.begin(), psd.end());
    
    features.push_back(totalPower);
    features.push_back(maxPower);
    
    if (totalPower > 0.0f) {
        float entropy = 0.0f;
        for (float power : psd) {
            if (power > 0.0f) {
                float p = power / totalPower;
                entropy -= p * std::log(p);
            }
        }
        features.push_back(entropy);
    } else {
        features.push_back(0.0f);
    }
    
    // Spectral flux
    float spectralFlux = 0.0f;
    for (size_t i = 1; i < psd.size(); ++i) {
        spectralFlux += std::abs(psd[i] - psd[i-1]);
    }
    features.push_back(spectralFlux);
    
    return features;
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
