#include "Audio.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iostream>

namespace nlm {

struct AudioProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    
    // Previous frame for temporal analysis
    std::vector<float> previousFrame;
    
    // Sample rate for frequency analysis
    size_t sampleRate;
    
    Impl() : featureDimensions(0), sampleRate(44100) {}
};

AudioProcessor::AudioProcessor() : pImpl(new Impl) {}

AudioProcessor::~AudioProcessor() = default;

void AudioProcessor::process(const Audio& input) {
    const auto& rawSamples = input.getData();
    size_t numSamples = input.getNumSamples();
    pImpl->sampleRate = input.getSampleRate();
    
#include "Audio.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iostream>

namespace nlm {

struct AudioProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    
    // Previous frame for temporal analysis
    std::vector<float> previousFrame;
    
    // Sample rate for frequency analysis
    size_t sampleRate;
    
    Impl() : featureDimensions(0), sampleRate(44100) {}
};

AudioProcessor::AudioProcessor() : pImpl(new Impl) {}

AudioProcessor::~AudioProcessor() = default;

void AudioProcessor::process(const Audio& input) {
    const auto& rawSamples = input.getData();
    size_t numSamples = input.getNumSamples();
    pImpl->sampleRate = input.getSampleRate();
    
    // Check if this is a valid audio input
    if (numSamples == 0) {
        pImpl->features.clear();
        pImpl->featureDimensions = 0;
        return;
    }
    
    // Generate 16 compressed features from raw audio samples
    pImpl->features.clear();
    pImpl->featureDimensions = 16;
    
    // 1. Basic audio statistics
    float minSample = 1e10f, maxSample = -1e10f, sumSample = 0.0f, sumSq = 0.0f;
    size_t samplesToAnalyze = std::min(numSamples, static_cast<size_t>(10000)); // Limit for performance
    
    for (size_t i = 0; i < samplesToAnalyze; ++i) {
        float sample = rawSamples[i];
        minSample = std::min(minSample, sample);
        maxSample = std::max(maxSample, sample);
        sumSample += sample;
        sumSq += sample * sample;
    }
    
    // Volume and intensity features
    float volume = std::max(std::abs(minSample), std::abs(maxSample));
    float avgSample = (samplesToAnalyze > 0) ? sumSample / samplesToAnalyze : 0.0f;
    float rms = (samplesToAnalyze > 0) ? std::sqrt(sumSq / samplesToAnalyze) : 0.0f;
    
    pImpl->features.push_back(volume);      // Peak volume
    pImpl->features.push_back(rms);         // Root mean square
    pImpl->features.push_back(avgSample);   // Average sample value
    
    // 2. Spectral features (simple FFT approximation)
    // Use only first 1024 samples for DFT for performance
    size_t dftSize = std::min(static_cast<size_t>(1024), numSamples);
    std::vector<float> magnitudes(dftSize, 0.0f);
    
    if (dftSize > 1) {
        for (size_t k = 0; k < dftSize; ++k) {
            float real = 0.0f, imag = 0.0f;
            for (size_t n = 0; n < dftSize; ++n) {
                float angle = 2 * M_PI * k * n / dftSize;
                real += rawSamples[n] * std::cos(angle);
                imag -= rawSamples[n] * std::sin(angle);
            }
            magnitudes[k] = std::sqrt(real * real + imag * imag);
        }
        
        // Find dominant frequency
        float maxMag = *std::max_element(magnitudes.begin(), magnitudes.end());
        float totalMag = std::accumulate(magnitudes.begin(), magnitudes.end(), 0.0f);
        
        // Normalize frequencies
        float dominantFreq = (maxMag > 0) ? (magnitudes.size() / 2 * maxMag / totalMag) : 0.0f;
        pImpl->features.push_back(dominantFreq);
        
        // Spectral flatness (how "noisy" the signal is)
        float geometricMean = 1.0f;
        float arithmeticMean = totalMag / magnitudes.size();
        for (float mag : magnitudes) {
            geometricMean *= (mag + 1e-10f);
        }
        geometricMean = std::pow(geometricMean, 1.0f / magnitudes.size());
        float spectralFlatness = (arithmeticMean > 0) ? (geometricMean / arithmeticMean) : 0.0f;
        pImpl->features.push_back(spectralFlatness);
    } else {
        pImpl->features.push_back(0.0f);
        pImpl->features.push_back(0.0f);
    }
    
    // 3. Rhythm/pattern features
    // Simple onset detection
    float energy = 0.0f;
    for (size_t i = 0; i < std::min(static_cast<size_t>(100), numSamples - 1); ++i) {
        float diff = rawSamples[i + 1] - rawSamples[i];
        energy += diff * diff;
    }
    energy = std::sqrt(energy);
    pImpl->features.push_back(energy);
    
    // 4. Temporal features
    // Zero crossing rate
    size_t zeroCrossings = 0;
    for (size_t i = 1; i < std::min(static_cast<size_t>(1000), numSamples); ++i) {
        if ((rawSamples[i-1] > 0 && rawSamples[i] <= 0) || (rawSamples[i-1] <= 0 && rawSamples[i] > 0)) {
            zeroCrossings++;
        }
    }
    float zcr = (numSamples > 1) ? (zeroCrossings / static_cast<float>(numSamples - 1)) : 0.0f;
    pImpl->features.push_back(zcr);
    
    // 5. Timbre/colour features (using frequency band energy)
    size_t bandSize = dftSize / 8;
    float bandEnergy[8] = {0.0f};
    for (size_t k = 0; k < dftSize && k < magnitudes.size(); ++k) {
        size_t band = k / bandSize;
        if (band < 8) {
            bandEnergy[band] += magnitudes[k] * magnitudes[k];
        }
    }
    
    // Normalize and add to features
    for (int b = 0; b < 8; ++b) {
        pImpl->features.push_back(bandEnergy[b]);
    }
    
    // 6. Temporal trend (difference from previous frame)
    float temporalEnergy = 0.0f;
    if (!pImpl->previousFrame.empty() && pImpl->previousFrame.size() == rawSamples.size()) {
        for (size_t i = 0; i < std::min(static_cast<size_t>(1000), rawSamples.size()); ++i) {
            float diff = rawSamples[i] - pImpl->previousFrame[i];
            temporalEnergy += diff * diff;
        }
        temporalEnergy = std::sqrt(temporalEnergy / 1000.0f);
    }
    pImpl->features.push_back(temporalEnergy);
    
    // 7. Sample rate normalized features
    float freqToSampleRatio = (pImpl->sampleRate > 0) ? dominantFreq / pImpl->sampleRate : 0.0f;
    pImpl->features.push_back(freqToSampleRatio);
    
    // 8. Stereo separation (if we had stereo, but for mono we just repeat)
    pImpl->features.push_back(volume * 0.5f);
    
    // 9. Temporal center of mass
    float temporalCenter = 0.0f, totalEnergy = 0.0f;
    for (size_t i = 0; i < std::min(static_cast<size_t>(1000), numSamples); ++i) {
        totalEnergy += std::abs(rawSamples[i]);
        temporalCenter += i * std::abs(rawSamples[i]);
    }
    float temporalCM = (totalEnergy > 0) ? (temporalCenter / totalEnergy) : 0.0f;
    pImpl->features.push_back(temporalCM);
    
    // 10. Compact representation (downsampling)
    // Create a compact representation by averaging groups of samples
    size_t numGroups = std::min(static_cast<size_t>(8), numSamples);
    float groupSums[8] = {0.0f};
    size_t samplesPerGroup = numSamples / numGroups;
    
    for (size_t i = 0; i < numGroups; ++i) {
        size_t start = i * samplesPerGroup;
        size_t count = std::min(samplesPerGroup, numSamples - start);
        float groupSum = 0.0f;
        for (size_t j = 0; j < count; ++j) {
            groupSum += rawSamples[start + j];
        }
        groupSums[i] = groupSum / count;
    }
    
    // Add group means as features
    for (size_t i = 0; i < numGroups; ++i) {
        pImpl->features.push_back(groupSums[i]);
    }
    
    // Store current frame for temporal analysis
    pImpl->previousFrame = rawSamples;
    
    // Print debug info
    std::cout << "AudioProcessor: Processed " << numSamples << " samples into " 
              << pImpl->featureDimensions << " features" << std::endl;
    std::cout << "  Volume: " << volume << ", RMS: " << rms << ", Energy: " << energy 
              << ", ZCR: " << zcr << ", Max Freq: " << dominantFreq << std::endl;
    std::cout << "  Spectral Flatness: " << spectralFlatness << ", Temporal Energy: " << temporalEnergy 
              << ", Temporal CM: " << temporalCM << std::endl;
}
    
    // 2. Spectral features (simple FFT approximation)
    // Use only first 1024 samples for DFT for performance
    size_t dftSize = std::min(static_cast<size_t>(1024), numSamples);
    std::vector<float> magnitudes(dftSize, 0.0f);
    
    if (dftSize > 1) {
        for (size_t k = 0; k < dftSize; ++k) {
            float real = 0.0f, imag = 0.0f;
            for (size_t n = 0; n < dftSize; ++n) {
                float angle = 2 * M_PI * k * n / dftSize;
                real += rawSamples[n] * std::cos(angle);
                imag -= rawSamples[n] * std::sin(angle);
            }
            magnitudes[k] = std::sqrt(real * real + imag * imag);
        }
        
        // Find dominant frequency
        float maxMag = *std::max_element(magnitudes.begin(), magnitudes.end());
        float totalMag = std::accumulate(magnitudes.begin(), magnitudes.end(), 0.0f);
        
        // Normalize frequencies
        float dominantFreq = (maxMag > 0) ? (magnitudes.size() / 2 * maxMag / totalMag) : 0.0f;
        pImpl->features.push_back(dominantFreq);
        
        // Spectral flatness (how "noisy" the signal is)
        float geometricMean = 1.0f;
        float arithmeticMean = totalMag / magnitudes.size();
        for (float mag : magnitudes) {
            geometricMean *= (mag + 1e-10f);
        }
        geometricMean = std::pow(geometricMean, 1.0f / magnitudes.size());
        float spectralFlatness = (arithmeticMean > 0) ? (geometricMean / arithmeticMean) : 0.0f;
        pImpl->features.push_back(spectralFlatness);
    } else {
        pImpl->features.push_back(0.0f);
        pImpl->features.push_back(0.0f);
    }
    
    // 3. Rhythm/pattern features
    // Simple onset detection
    float energy = 0.0f;
    for (size_t i = 0; i < std::min(static_cast<size_t>(100), numSamples - 1); ++i) {
        float diff = rawSamples[i + 1] - rawSamples[i];
        energy += diff * diff;
    }
    energy = std::sqrt(energy);
    pImpl->features.push_back(energy);
    
    // 4. Temporal features
    // Zero crossing rate
    size_t zeroCrossings = 0;
    for (size_t i = 1; i < std::min(static_cast<size_t>(1000), numSamples); ++i) {
        if ((rawSamples[i-1] > 0 && rawSamples[i] <= 0) || (rawSamples[i-1] <= 0 && rawSamples[i] > 0)) {
            zeroCrossings++;
        }
    }
    float zcr = (numSamples > 1) ? (zeroCrossings / static_cast<float>(numSamples - 1)) : 0.0f;
    pImpl->features.push_back(zcr);
    
    // 5. Timbre/colour features (using frequency band energy)
    size_t bandSize = dftSize / 8;
    float bandEnergy[8] = {0.0f};
    for (size_t k = 0; k < dftSize && k < magnitudes.size(); ++k) {
        size_t band = k / bandSize;
        if (band < 8) {
            bandEnergy[band] += magnitudes[k] * magnitudes[k];
        }
    }
    
    // Normalize and add to features
    for (int b = 0; b < 8; ++b) {
        pImpl->features.push_back(bandEnergy[b]);
    }
    
    // 6. Temporal trend (difference from previous frame)
    float temporalEnergy = 0.0f;
    if (!pImpl->previousFrame.empty() && pImpl->previousFrame.size() == rawSamples.size()) {
        for (size_t i = 0; i < std::min(static_cast<size_t>(1000), rawSamples.size()); ++i) {
            float diff = rawSamples[i] - pImpl->previousFrame[i];
            temporalEnergy += diff * diff;
        }
        temporalEnergy = std::sqrt(temporalEnergy / 1000.0f);
    }
    pImpl->features.push_back(temporalEnergy);
    
    // 7. Sample rate normalized features
    float freqToSampleRatio = (pImpl->sampleRate > 0) ? dominantFreq / pImpl->sampleRate : 0.0f;
    pImpl->features.push_back(freqToSampleRatio);
    
    // 8. Stereo separation (if we had stereo, but for mono we just repeat)
    pImpl->features.push_back(volume * 0.5f);
    
    // 9. Temporal center of mass
    float temporalCenter = 0.0f, totalEnergy = 0.0f;
    for (size_t i = 0; i < std::min(static_cast<size_t>(1000), numSamples); ++i) {
        totalEnergy += std::abs(rawSamples[i]);
        temporalCenter += i * std::abs(rawSamples[i]);
    }
    float temporalCM = (totalEnergy > 0) ? (temporalCenter / totalEnergy) : 0.0f;
    pImpl->features.push_back(temporalCM);
    
    // 10. Compact representation (downsampling)
    // Create a compact representation by averaging groups of samples
    size_t numGroups = std::min(static_cast<size_t>(8), numSamples);
    float groupSums[8] = {0.0f};
    size_t samplesPerGroup = numSamples / numGroups;
    
    for (size_t i = 0; i < numGroups; ++i) {
        size_t start = i * samplesPerGroup;
        size_t count = std::min(samplesPerGroup, numSamples - start);
        float groupSum = 0.0f;
        for (size_t j = 0; j < count; ++j) {
            groupSum += rawSamples[start + j];
        }
        groupSums[i] = groupSum / count;
    }
    
    // Add group means as features
    for (size_t i = 0; i < numGroups; ++i) {
        pImpl->features.push_back(groupSums[i]);
    }
    
    // Store current frame for temporal analysis
    pImpl->previousFrame = rawSamples;
    
    // Print debug info
    std::cout << "AudioProcessor: Processed " << numSamples << " samples into " 
              << pImpl->featureDimensions << " features" << std::endl;
    std::cout << "  Volume: " << volume << ", RMS: " << rms << ", Energy: " << energy 
              << ", ZCR: " << zcr << ", Max Freq: " << dominantFreq << std::endl;
    std::cout << "  Spectral Flatness: " << spectralFlatness << ", Temporal Energy: " << temporalEnergy 
              << ", Temporal CM: " << temporalCM << std::endl;
}

const std::vector<float>& AudioProcessor::getFeatures() const {
    return pImpl->features;
}

size_t AudioProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

} // namespace nlm
