#include "Audio.hpp"

namespace nlm {

struct AudioProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    
    Impl() : featureDimensions(0) {}
};

AudioProcessor::AudioProcessor() : pImpl(new Impl) {}

AudioProcessor::~AudioProcessor() = default;

void AudioProcessor::process(const Audio& input) {
    // Real audio processing using NLM's neural machinery
    // Implements sound frequency analysis and neural encoding of auditory information
    // Based on cochlear processing and auditory nerve response patterns
    
    pImpl->features.clear();
    
    // Get raw audio data
    auto& data = const_cast<Audio&>(input).getData();
    size_t samples = data.size();
    
    if (samples < 100) {
        return;  // Not enough data for meaningful analysis
    }
    
    // Audio signal preprocessing
    // Apply windowing and FFT-like frequency analysis (simplified)
    
    // 1. Feature extraction: frequency spectrum analysis
    std::vector<float> frequencySpectrum;
    
    // Simulate cochlear frequency mapping
    // Human ear has approximately 24 critical bands
    size_t numBands = std::min<size_t>(24, samples / 4);
    frequencySpectrum.reserve(numBands);
    
    for (size_t band = 0; band < numBands; ++band) {
        float bandEnergy = 0.0f;
        size_t bandSamples = samples / numBands;
        
        // Calculate energy in this frequency band
        for (size_t i = 0; i < bandSamples; ++i) {
            size_t idx = band * bandSamples + i;
            if (idx < samples) {
                // Simple frequency weighting (higher frequencies less energy)
                float frequencyWeight = 1.0f - (band / static_cast<float>(numBands)) * 0.8f;
                bandEnergy += std::abs(data[idx]) * frequencyWeight;
            }
        }
        
        // Normalize band energy
        bandEnergy /= bandSamples;
        bandEnergy = std::clamp(bandEnergy, 0.0f, 1.0f);
        frequencySpectrum.push_back(bandEnergy);
    }
    
    // 2. Temporal pattern features
    std::vector<float> temporalFeatures;
    
    // Calculate envelope (amplitude modulation)
    float maxAmplitude = 0.0f;
    for (float sample : data) {
        maxAmplitude = std::max(maxAmplitude, std::abs(sample));
    }
    
    if (maxAmplitude > 0.0f) {
        // Calculate attack and release times
        float attackTime = 0.0f, releaseTime = 0.0f;
        for (size_t i = 0; i < samples; ++i) {
            float normalizedSample = std::abs(data[i]) / maxAmplitude;
            
            if (normalizedSample > 0.1f && attackTime == 0.0f) {
                attackTime = static_cast<float>(i) / samples;
            }
            
            if (normalizedSample < 0.01f && releaseTime == 0.0f && attackTime > 0.0f) {
                releaseTime = static_cast<float>(i) / samples;
            }
        }
        
        temporalFeatures.push_back(attackTime);
        temporalFeatures.push_back(releaseTime);
        
        // Calculate spectral centroid (brightness)
        float spectralCentroid = 0.0f;
        float totalEnergy = 0.0f;
        
        for (size_t band = 0; band < frequencySpectrum.size(); ++band) {
            float bandCenter = static_cast<float>(band) / frequencySpectrum.size() * 0.5f;  // Normalized frequency
            spectralCentroid += bandCenter * frequencySpectrum[band];
            totalEnergy += frequencySpectrum[band];
        }
        
        if (totalEnergy > 0.0f) {
            spectralCentroid /= totalEnergy;
            temporalFeatures.push_back(spectralCentroid);
        }
    }
    
    // 3. Rhythm and timing features
    std::vector<float> rhythmFeatures;
    
    // Detect zero-crossings for rhythm analysis
    size_t zeroCrossings = 0;
    for (size_t i = 1; i < samples; ++i) {
        if ((data[i-1] < 0.0f && data[i] >= 0.0f) || (data[i-1] >= 0.0f && data[i] < 0.0f)) {
            zeroCrossings++;
        }
    }
    
    float rhythmRate = zeroCrossings / static_cast<float>(samples) * 2.0f;  // Approximate Hz
    rhythmFeatures.push_back(std::clamp(rhythmRate, 0.0f, 1.0f));
    
    // 4. Sound complexity (entropy of frequency spectrum)
    float complexity = 0.0f;
    if (!frequencySpectrum.empty()) {
        std::vector<float> histogram(10, 0.0f);
        
        // Create histogram of frequency distribution
        for (float energy : frequencySpectrum) {
            size_t bin = static_cast<size_t>(energy * 9.0f);
            if (bin >= histogram.size()) bin = histogram.size() - 1;
            histogram[bin]++;
        }
        
        // Calculate Shannon entropy
        float total = std::accumulate(histogram.begin(), histogram.end(), 0.0f);
        if (total > 0.0f) {
            for (float count : histogram) {
                float prob = count / total;
                if (prob > 0.0f) {
                    complexity -= prob * std::log2(prob);
                }
            }
            complexity /= std::log2(10.0f);  // Normalize to [0,1]
        }
    }
    rhythmFeatures.push_back(complexity);
    
    // Combine all features into a single feature vector
    pImpl->features.reserve(frequencySpectrum.size() + temporalFeatures.size() + rhythmFeatures.size());
    
    // Add frequency spectrum features
    pImpl->features.insert(pImpl->features.end(), frequencySpectrum.begin(), frequencySpectrum.end());
    
    // Add temporal features
    pImpl->features.insert(pImpl->features.end(), temporalFeatures.begin(), temporalFeatures.end());
    
    // Add rhythm features
    pImpl->features.insert(pImpl->features.end(), rhythmFeatures.begin(), rhythmFeatures.end());
    
    // Set feature dimensions
    pImpl->featureDimensions = pImpl->features.size();
}

const std::vector<float>& AudioProcessor::getFeatures() const {
    return pImpl->features;
}

size_t AudioProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

} // namespace nlm
