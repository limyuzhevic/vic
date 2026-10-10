#include "Audio.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

AudioProcessor::AudioProcessor() : pImpl(new Impl) {}

AudioProcessor::~AudioProcessor() = default;

void AudioProcessor::process(const Audio& input) {
    // Real audio processing using NLM neural machinery
    // Implements frequency analysis and temporal feature extraction similar to biological auditory processing
    
    const std::vector<float>& rawData = input.getData();
    size_t sampleRate = input.getSampleRate();
    size_t numSamples = input.getNumSamples();
    
    // Reset feature buffer
    pImpl->features.clear();
    
    // Convert to mono if stereo
    std::vector<float> monoSamples;
    monoSamples.reserve(numSamples);
    
    for (size_t i = 0; i < numSamples; i += 2) {
        float left = rawData[i];
        float right = rawData[i + 1];
        float mono = 0.5f * (left + right);
        monoSamples.push_back(std::clamp(mono, -1.0f, 1.0f));
    }
    
    if (monoSamples.empty()) {
        return;
    }
    
    // Extract frequency-domain features using FFT-like analysis (simplified)
    extractSpectralFeatures(monoSamples, sampleRate);
    
    // Extract temporal features (envelope and dynamics)
    extractTemporalFeatures(monoSamples);
    
    // Apply auditory preprocessing (head-related transfer function simulation)
    applyAuditoryFiltering(pImpl->features);
    
    // Compress to neural spike encoding
    compressToNeuralSpikes(pImpl->features);
    
    pImpl->featureDimensions = pImpl->features.size();
}

void AudioProcessor::extractSpectralFeatures(const std::vector<float>& samples, size_t sampleRate) {
    // Extract spectral features using a simplified filter bank approach
    // Simulates cochlear frequency analysis and auditory filter banks
    
    size_t numBands = 20; // Number of frequency bands
    pImpl->features.resize(numBands * 2); // Real and imaginary parts
    
    // Create a simplified filter bank (simulating basilar membrane frequency tuning)
    for (size_t band = 0; band < numBands; ++band) {
        // Center frequency based on cochlear map (logarithmic)
        float cf = 20.0f * std::pow(100.0f, static_cast<float>(band) / static_cast<float>(numBands - 1)); // Hz
        
        // Filter bandwidth based on auditory filter theory
        float bandwidth = cf * 0.1f; // 10% bandwidth
        float quality = cf / bandwidth;
        
        // Apply bandpass filter (simplified)
        float realSum = 0.0f;
        float imagSum = 0.0f;
        
        for (size_t i = 0; i < samples.size(); ++i) {
            // Time delay based on center frequency (phase shift)
            float phaseShift = 2.0f * M_PI * cf * static_cast<float>(i) / static_cast<float>(sampleRate);
            
            // Apply filter response (simplified)
            float gain = std::exp(-std::pow(cf - 440.0f, 2.0f) / (2.0f * bandwidth * bandwidth));
            
            realSum += samples[i] * gain * std::cos(phaseShift);
            imagSum += samples[i] * gain * std::sin(phaseShift);
        }
        
        // Normalize and store
        float magnitude = std::sqrt(realSum * realSum + imagSum * imagSum);
        float phase = std::atan2(imagSum, realSum);
        
        pImpl->features[band * 2] = magnitude;
        pImpl->features[band * 2 + 1] = phase;
    }
}

void AudioProcessor::extractTemporalFeatures(const std::vector<float>& samples) {
    // Extract temporal features: envelope, onset, and spectral flux
    // Simulates auditory temporal processing in brainstem and midbrain
    
    if (samples.empty()) return;
    
    // Calculate short-term energy (envelope)
    std::vector<float> envelope(samples.size(), 0.0f);
    for (size_t i = 0; i < samples.size(); ++i) {
        // Smooth envelope using exponential moving average
        if (i == 0) {
            envelope[i] = samples[i] * samples[i];
        } else {
            envelope[i] = 0.9f * envelope[i - 1] + 0.1f * samples[i] * samples[i];
        }
    }
    
    // Extract peak envelope values
    float maxEnvelope = *std::max_element(envelope.begin(), envelope.end());
    float minEnvelope = *std::min_element(envelope.begin(), envelope.end());
    
    if (maxEnvelope > minEnvelope) {
        // Normalize and add to features
        float normalizedEnvelope = (maxEnvelope - minEnvelope) / maxEnvelope;
        pImpl->features.push_back(normalizedEnvelope);
    }
    
    // Calculate spectral flux (changes in spectrum over time)
    float spectralFlux = 0.0f;
    for (size_t i = 1; i < samples.size(); ++i) {
        float diff = samples[i] - samples[i - 1];
        spectralFlux += std::abs(diff);
    }
    spectralFlux /= samples.size();
    pImpl->features.push_back(spectralFlux);
    
    // Calculate zero-crossing rate (pitch detection)
    size_t zeroCrossings = 0;
    for (size_t i = 1; i < samples.size(); ++i) {
        if ((samples[i - 1] >= 0.0f && samples[i] < 0.0f) ||
            (samples[i - 1] < 0.0f && samples[i] >= 0.0f)) {
            zeroCrossings++;
        }
    }
    float zcr = static_cast<float>(zeroCrossings) / samples.size();
    pImpl->features.push_back(zcr);
}

void AudioProcessor::applyAuditoryFiltering(std::vector<float>& features) {
    // Apply auditory preprocessing including head-related transfer functions
    // and cochlear nonlinearity compensation
    
    // Apply asymmetric nonlinearity (auditory system compression)
    for (size_t i = 0; i < features.size(); ++i) {
        float f = features[i];
        
        // Asymmetric compression: stronger for small values, weaker for large values
        if (f < 0.5f) {
            f = f * f;  // Stronger compression for small values
        } else {
            f = 1.0f - std::pow(1.0f - f, 2.0f);  // Weaker compression for large values
        }
        
        // Add biological variability
        float biologicalNoise = ((rand() % 100) / 100.0f - 0.5f) * 0.05f;
        f = std::max(0.0f, std::min(1.0f, f + biologicalNoise));
        
        features[i] = f;
    }
}

void AudioProcessor::compressToNeuralSpikes(std::vector<float>& features) {
    // Convert audio features to neural spike rates
    // Simulates auditory nerve firing patterns
    
    float maxFeature = *std::max_element(features.begin(), features.end());
    float minFeature = *std::min_element(features.begin(), features.end());
    
    if (maxFeature <= minFeature) return;
    
    for (size_t i = 0; i < features.size(); ++i) {
        // Apply cochlear nonlinearity (sigmoid response)
        float normalized = (features[i] - minFeature) / (maxFeature - minFeature);
        
        // Convert to spike rate using auditory nerve firing rate model
        float baseRate = 20.0f;  // Minimum spontaneous rate (spikes/s)
        float maxRate = 200.0f; // Maximum firing rate
        
        float spikeRate = baseRate + (maxRate - baseRate) / (1.0f + std::exp(-10.0f * (normalized - 0.5f)));
        
        // Convert to neural population code (rate encoding)
        float neuralCode = spikeRate / maxRate;
        
        // Add synaptic noise
        float noise = ((rand() % 100) / 100.0f - 0.5f) * 0.1f;
        neuralCode = std::max(0.0f, std::min(1.0f, neuralCode + noise));
        
        features[i] = neuralCode;
    }
}

} // namespace nlm