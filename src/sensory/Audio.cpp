#include "Audio.hpp"
#include "../brain/Brain.hpp"
#include <numeric>
#include <cmath>
#include <algorithm>

namespace nlm {

struct AudioProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    std::vector<float> spectralFeatures;
    std::vector<float> temporalFeatures;
    float spectralCentroid;
    float rmsEnergy;
    float zeroCrossingRate;
    float spectralRollOff;
    
    Impl() : featureDimensions(0), spectralCentroid(0.0f), rmsEnergy(0.0f), 
             zeroCrossingRate(0.0f), spectralRollOff(0.0f) {}
};

AudioProcessor::AudioProcessor() : pImpl(new Impl) {}

AudioProcessor::~AudioProcessor() = default;

void AudioProcessor::process(const Audio& input, Brain* brain) {
    const auto& rawData = input.getData();
    size_t dimensions = input.getDimensions();
    
    if (rawData.empty() || brain == nullptr) {
        pImpl->features.clear();
        pImpl->featureDimensions = 0;
        return;
    }
    
    // Extract audio features using neural-inspired processing
    extractAudioFeatures(rawData);
    
    // For now, basic integration with brain's plasticity system
    // Note: This is a simplified version of neural integration
    if (brain) {
        // Get plasticity systems from brain
        auto* hebbian = brain->getHebbian();
        
        // Apply Hebbian learning based on audio features
        if (hebbian && pImpl->features.size() > 1) {
            float totalCorrelation = 0.0f;
            for (size_t i = 0; i < pImpl->features.size() - 1; ++i) {
                for (size_t j = i + 1; j < pImpl->features.size(); ++j) {
                    float correlation = pImpl->features[i] * pImpl->features[j];
                    totalCorrelation += correlation;
                }
            }
            if (totalCorrelation > 0.1f) {
                float delta = 0.001f * totalCorrelation;
                hebbian->applyWeightChange(nullptr, delta);
            }
        }
    }
}

void AudioProcessor::extractAudioFeatures(const std::vector<float>& data) {
    size_t windowSize = 1024;
    size_t hopSize = 512;
    
    pImpl->features.clear();
    pImpl->spectralFeatures.clear();
    pImpl->temporalFeatures.clear();
    
    if (data.size() < windowSize) {
        // Short audio - use full data as features
        pImpl->features = data;
        pImpl->featureDimensions = data.size();
        computeBasicStats(data);
        return;
    }
    
    // Process audio in overlapping windows
    size_t numFrames = (data.size() - windowSize) / hopSize + 1;
    
    // Compute spectral features
    computeSpectralFeatures(data, windowSize, hopSize, numFrames);
    
    // Compute temporal features
    computeTemporalFeatures(data, windowSize, hopSize, numFrames);
    
    // Aggregate features
    aggregateFeatures();
    
    pImpl->featureDimensions = pImpl->features.size();
}

void AudioProcessor::computeBasicStats(const std::vector<float>& data) {
    // RMS energy
    float sum = std::accumulate(data.begin(), data.end(), 0.0f);
    float mean = sum / data.size();
    
    float sumSquares = 0.0f;
    for (float val : data) {
        float diff = val - mean;
        sumSquares += diff * diff;
    }
    pImpl->rmsEnergy = std::sqrt(sumSquares / data.size());
    
    // Zero crossing rate
    int crossings = 0;
    for (size_t i = 1; i < data.size(); ++i) {
        if ((data[i-1] < 0.0f && data[i] >= 0.0f) || 
            (data[i-1] >= 0.0f && data[i] < 0.0f)) {
            crossings++;
        }
    }
    pImpl->zeroCrossingRate = static_cast<float>(crossings) / data.size();
    
    // Add basic features
    pImpl->features = {
        pImpl->rmsEnergy,
        pImpl->zeroCrossingRate,
        mean,
        pImpl->spectralCentroid,
        pImpl->spectralRollOff
    };
}

void AudioProcessor::computeSpectralFeatures(const std::vector<float>& data, 
                                           size_t windowSize, size_t hopSize, 
                                           size_t numFrames) {
    pImpl->spectralFeatures.reserve(numFrames * 8);
    
    for (size_t frame = 0; frame < numFrames; ++frame) {
        size_t start = frame * hopSize;
        
        // Extract frame
        std::vector<float> frameData(windowSize, 0.0f);
        size_t copySize = std::min(windowSize, data.size() - start);
        for (size_t i = 0; i < copySize; ++i) {
            frameData[i] = data[start + i];
        }
        
        // Compute FFT magnitude spectrum (simplified)
        std::vector<float> spectrum(windowSize, 0.0f);
        computeFFTMagnitude(frameData, spectrum);
        
        // Compute spectral features for this frame
        float centroid = computeSpectralCentroid(spectrum);
        float rollOff = computeSpectralRollOff(spectrum);
        
        // Store spectral features
        pImpl->spectralFeatures.push_back(centroid);
        pImpl->spectralFeatures.push_back(rollOff);
        pImpl->spectralFeatures.push_back(computeSpectralFlux(spectrum, frame == 0 ? spectrum : pImpl->spectralFeatures));
        pImpl->spectralFeatures.push_back(computeSpectralEntropy(spectrum));
        
        // Update global spectral centroid
        pImpl->spectralCentroid = (pImpl->spectralCentroid * frame + centroid) / (frame + 1);
    }
}

void AudioProcessor::computeTemporalFeatures(const std::vector<float>& data, 
                                           size_t windowSize, size_t hopSize,
                                           size_t numFrames) {
    pImpl->temporalFeatures.reserve(numFrames * 4);
    
    for (size_t frame = 0; frame < numFrames; ++frame) {
        size_t start = frame * hopSize;
        
        // Extract frame
        std::vector<float> frameData(windowSize, 0.0f);
        size_t copySize = std::min(windowSize, data.size() - start);
        for (size_t i = 0; i < copySize; ++i) {
            frameData[i] = data[start + i];
        }
        
        // Compute temporal features
        float pitch = estimatePitch(frameData);
        float formant = estimateFormant(frameData);
        float attack = computeAttackTime(frameData);
        float decay = computeDecayTime(frameData);
        
        pImpl->temporalFeatures.push_back(pitch);
        pImpl->temporalFeatures.push_back(formant);
        pImpl->temporalFeatures.push_back(attack);
        pImpl->temporalFeatures.push_back(decay);
    }
}

void AudioProcessor::aggregateFeatures() {
    // Combine all feature types
    pImpl->features.clear();
    
    // Add spectral features (first 2 per frame, then aggregated)
    for (size_t i = 0; i < std::min(static_cast<size_t>(100), pImpl->spectralFeatures.size()); ++i) {
        pImpl->features.push_back(pImpl->spectralFeatures[i]);
    }
    
    // Add temporal features (first 2 per frame)
    for (size_t i = 0; i < std::min(static_cast<size_t>(50), pImpl->temporalFeatures.size()); ++i) {
        pImpl->features.push_back(pImpl->temporalFeatures[i]);
    }
    
    // Add global statistics
    pImpl->features.push_back(pImpl->rmsEnergy);
    pImpl->features.push_back(pImpl->zeroCrossingRate);
    pImpl->features.push_back(pImpl->spectralCentroid);
    pImpl->features.push_back(pImpl->spectralRollOff);
    
    // Add normalized features for consistency
    if (!pImpl->features.empty()) {
        float maxVal = *std::max_element(pImpl->features.begin(), pImpl->features.end());
        if (maxVal > 0.0f) {
            for (float& val : pImpl->features) {
                val /= maxVal;
            }
        }
    }
}

float AudioProcessor::computeSpectralCentroid(const std::vector<float>& spectrum) {
    float numerator = 0.0f, denominator = 0.0f;
    size_t size = spectrum.size();
    
    for (size_t i = 0; i < size; ++i) {
        float freq = static_cast<float>(i) * 22050.0f / static_cast<float>(size); // Assuming 22.05kHz sample rate
        float magnitude = spectrum[i];
        
        numerator += freq * magnitude;
        denominator += magnitude;
    }
    
    return denominator > 0.0f ? numerator / denominator : 0.0f;
}

float AudioProcessor::computeSpectralRollOff(const std::vector<float>& spectrum) {
    float totalEnergy = 0.0f;
    for (float mag : spectrum) {
        totalEnergy += mag * mag;
    }
    
    float accumulated = 0.0f;
    float rollOff = 0.0f;
    float threshold = 0.85f * totalEnergy;
    
    for (size_t i = 0; i < spectrum.size(); ++i) {
        accumulated += spectrum[i] * spectrum[i];
        if (accumulated >= threshold) {
            rollOff = static_cast<float>(i) / static_cast<float>(spectrum.size());
            break;
        }
    }
    
    return rollOff;
}

float AudioProcessor::computeSpectralFlux(const std::vector<float>& spectrum, const std::vector<float>& prevSpectrum) {
    float flux = 0.0f;
    size_t size = std::min(spectrum.size(), prevSpectrum.size());
    
    for (size_t i = 0; i < size; ++i) {
        float diff = spectrum[i] - prevSpectrum[i];
        flux += diff * diff;
    }
    
    return std::sqrt(flux);
}

float AudioProcessor::computeSpectralEntropy(const std::vector<float>& spectrum) {
    float entropy = 0.0f;
    float total = 0.0f;
    
    for (float mag : spectrum) {
        total += mag;
    }
    
    if (total > 0.0f) {
        for (float mag : spectrum) {
            float prob = mag / total;
            if (prob > 0.0f) {
                entropy -= prob * std::log(std::max(prob, 1e-6f));
            }
        }
        entropy /= std::log(static_cast<float>(spectrum.size()));
    }
    
    return entropy;
}

float AudioProcessor::estimatePitch(const std::vector<float>& frame) {
    // Simple autocorrelation-based pitch estimation
    float maxCorrelation = 0.0f;
    float bestLag = 0.0f;
    size_t searchLimit = std::min(static_cast<size_t>(200), frame.size() / 2);
    
    for (size_t lag = 1; lag < searchLimit; ++lag) {
        float correlation = 0.0f;
        for (size_t i = 0; i < frame.size() - lag; ++i) {
            correlation += frame[i] * frame[i + lag];
        }
        
        if (correlation > maxCorrelation) {
            maxCorrelation = correlation;
            bestLag = static_cast<float>(lag);
        }
    }
    
    return 22050.0f / bestLag; // Convert lag to pitch frequency
}

float AudioProcessor::estimateFormant(const std::vector<float>& frame) {
    // Estimate fundamental frequency as proxy for formant
    float maxAmplitude = 0.0f;
    size_t peakIdx = 0;
    
    for (size_t i = 0; i < frame.size(); ++i) {
        if (std::abs(frame[i]) > maxAmplitude) {
            maxAmplitude = std::abs(frame[i]);
            peakIdx = i;
        }
    }
    
    // Simple formant approximation
    return 220.0f + 2000.0f * (static_cast<float>(peakIdx) / static_cast<float>(frame.size()));
}

float AudioProcessor::computeAttackTime(const std::vector<float>& frame) {
    float maxVal = 0.0f;
    size_t maxIdx = 0;
    
    for (size_t i = 0; i < frame.size(); ++i) {
        if (std::abs(frame[i]) > maxVal) {
            maxVal = std::abs(frame[i]);
            maxIdx = i;
        }
    }
    
    return static_cast<float>(maxIdx) / static_cast<float>(frame.size());
}

float AudioProcessor::computeDecayTime(const std::vector<float>& frame) {
    float envelope = 0.0f;
    float decay = 0.0f;
    
    for (size_t i = 0; i < frame.size(); ++i) {
        envelope = std::max(envelope, std::abs(frame[i]));
        decay = envelope * (1.0f - static_cast<float>(i) / static_cast<float>(frame.size()));
    }
    
    return decay;
}

void AudioProcessor::computeFFTMagnitude(const std::vector<float>& input, std::vector<float>& output) {
    // Simplified FFT magnitude computation
    // In real implementation, would use proper FFT library
    size_t size = input.size();
    
    for (size_t i = 0; i < size; ++i) {
        float real = 0.0f, imag = 0.0f;
        
        for (size_t j = 0; j < size; ++j) {
            float angle = -2.0f * M_PI * i * j / static_cast<float>(size);
            real += input[j] * std::cos(angle);
            imag -= input[j] * std::sin(angle);
        }
        
        output[i] = std::sqrt(real * real + imag * imag) / size;
    }
}

const std::vector<float>& AudioProcessor::getFeatures() const {
    return pImpl->features;
}

float AudioProcessor::getSpectralCentroid() const {
    return pImpl->spectralCentroid;
}

float AudioProcessor::getRMSEnergy() const {
    return pImpl->rmsEnergy;
}

float AudioProcessor::getZeroCrossingRate() const {
    return pImpl->zeroCrossingRate;
}

float AudioProcessor::getSpectralRollOff() const {
    return pImpl->spectralRollOff;
}

} // namespace nlm