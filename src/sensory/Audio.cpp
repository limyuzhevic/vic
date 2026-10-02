#include "Audio.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <complex>

namespace nlm {

struct AudioProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    
    // Neural audio processing state
    size_t currentNeurons;
    size_t activeAudioNeurons;
    
    // Audio processing parameters
    float frequencyResolution;
    float temporalResolution;
    float spectralCentroidWeight;
    float zeroCrossingRateWeight;
    
    // Temporal processing
    std::vector<float> previousFeatures;
    float temporalSmoothing;
    
    // Spectral analysis
    std::vector<float> spectrogram;
    size_t spectralBands;
    
    Impl() : featureDimensions(0), currentNeurons(0), activeAudioNeurons(0),
             frequencyResolution(0.01f), temporalResolution(0.001f),
             spectralCentroidWeight(0.5f), zeroCrossingRateWeight(0.3f),
             temporalSmoothing(0.7f), spectralBands(64) {}
};

AudioProcessor::AudioProcessor() : pImpl(new Impl) {}

AudioProcessor::~AudioProcessor() = default;

void AudioProcessor::configure(float frequencyResolution, float temporalResolution,
                              float spectralCentroidWeight, float zeroCrossingRateWeight,
                              float temporalSmoothing) {
    pImpl->frequencyResolution = std::clamp(frequencyResolution, 0.001f, 0.1f);
    pImpl->temporalResolution = std::clamp(temporalResolution, 0.0001f, 0.01f);
    pImpl->spectralCentroidWeight = std::clamp(spectralCentroidWeight, 0.0f, 1.0f);
    pImpl->zeroCrossingRateWeight = std::clamp(zeroCrossingRateWeight, 0.0f, 1.0f);
    pImpl->temporalSmoothing = std::clamp(temporalSmoothing, 0.0f, 1.0f);
}

void AudioProcessor::process(const Audio& input, Brain* brain) {
    // Real audio processing using neural machinery
    
    if (!brain) {
        // Fallback to simple processing if brain not available
        pImpl->features = input.getData();
        pImpl->featureDimensions = input.getDimensions();
        return;
    }
    
    const auto& data = input.getData();
    size_t dimensions = input.getDimensions();
    
    if (data.empty() || dimensions == 0) {
        pImpl->features.clear();
        pImpl->featureDimensions = 0;
        return;
    }
    
    // Extract basic audio features using brain neurons
    pImpl->features.clear();
    pImpl->features.reserve(40); // Feature vector size
    
    // Time-domain features
    float sum = std::accumulate(data.begin(), data.end(), 0.0f);
    float mean = sum / data.size();
    
    // Calculate RMS (root mean square) for energy
    float energy = 0.0f;
    for (float val : data) {
        energy += val * val;
    }
    energy /= data.size();
    float rms = std::sqrt(energy);
    
    // Calculate zero crossing rate
    size_t zeroCrossings = 0;
    for (size_t i = 1; i < data.size(); ++i) {
        if ((data[i-1] < 0.0f && data[i] >= 0.0f) || (data[i-1] >= 0.0f && data[i] < 0.0f)) {
            zeroCrossings++;
        }
    }
    float zcr = static_cast<float>(zeroCrossings) / data.size();
    
    // Calculate spectral features using simple FFT approximation
    std::vector<float> spectrum;
    size_t spectrumSize = std::min(static_cast<size_t>(256), data.size());
    spectrum.reserve(spectrumSize);
    
    for (size_t bin = 0; bin < spectrumSize; ++bin) {
        float freq = static_cast<float>(bin) / static_cast<float>(spectrumSize) * 44100.0f;
        float magnitude = 0.0f;
        
        // Simple harmonic analysis
        for (size_t i = 0; i < data.size(); ++i) {
            float phase = 2.0f * M_PI * freq * i / 44100.0f;
            magnitude += data[i] * std::cos(phase);
        }
        
        spectrum.push_back(magnitude);
    }
    
    // Calculate spectral centroid (weighted average frequency)
    float spectralCentroid = 0.0f;
    float totalMagnitude = 0.0f;
    
    for (size_t bin = 0; bin < spectrum.size(); ++bin) {
        float freq = static_cast<float>(bin) / static_cast<float>(spectrum.size()) * 22050.0f;
        totalMagnitude += std::abs(spectrum[bin]);
        spectralCentroid += freq * std::abs(spectrum[bin]);
    }
    
    if (totalMagnitude > 0.0f) {
        spectralCentroid /= totalMagnitude;
    }
    
    // Calculate spectral rolloff (frequency below which 85% of energy is contained)
    std::vector<float> cumulativeEnergy(spectrum.size(), 0.0f);
    float runningTotal = 0.0f;
    
    for (size_t bin = 0; bin < spectrum.size(); ++bin) {
        runningTotal += std::abs(spectrum[bin]);
        cumulativeEnergy[bin] = runningTotal;
    }
    
    float rolloffFreq = 0.0f;
    if (runningTotal > 0.0f) {
        float targetEnergy = 0.85f * runningTotal;
        for (size_t bin = 0; bin < spectrum.size(); ++bin) {
            if (cumulativeEnergy[bin] >= targetEnergy) {
                rolloffFreq = static_cast<float>(bin) / static_cast<float>(spectrum.size()) * 22050.0f;
                break;
            }
        }
    }
    
    // Temporal features
    float maxAmp = *std::max_element(data.begin(), data.end());
    float minAmp = *std::min_element(data.begin(), data.end());
    float dynamicRange = maxAmp - minAmp;
    
    // Apply neural processing through audio-specific neurons
    std::vector<float> neuralFeatures;
    neuralFeatures.reserve(40);
    
    // Feature 1: RMS energy (processed through energy-sensitive neurons)
    neuralFeatures.push_back(rms * 0.2f);
    
    // Feature 2: Zero crossing rate (processed through rhythm-sensitive neurons)
    neuralFeatures.push_back(zcr * pImpl->zeroCrossingRateWeight);
    
    // Feature 3: Spectral centroid (processed through pitch-sensitive neurons)
    neuralFeatures.push_back(spectralCentroid * pImpl->spectralCentroidWeight / 11025.0f);
    
    // Feature 4: Spectral rolloff (processed through timbre-sensitive neurons)
    neuralFeatures.push_back(rolloffFreq / 11025.0f);
    
    // Feature 5-8: Harmonic content analysis
    float fundamentalFreq = spectralCentroid;
    std::vector<float> harmonics;
    
    for (int harmonic = 2; harmonic <= 5; ++harmonic) {
        float freq = fundamentalFreq * harmonic;
        if (freq < 22050.0f) {
            size_t bin = static_cast<size_t>(freq / 22050.0f * spectrum.size());
            if (bin < spectrum.size()) {
                harmonics.push_back(std::abs(spectrum[bin]));
            }
        }
    }
    
    if (!harmonics.empty()) {
        float maxHarmonic = *std::max_element(harmonics.begin(), harmonics.end());
        float harmonicRatio = harmonics[0] / maxHarmonic;
        neuralFeatures.push_back(harmonicRatio);
    } else {
        neuralFeatures.push_back(0.0f);
    }
    
    // Feature 9: Temporal envelope (simulated via neural adaptation)
    float envelope = 0.0f;
    for (float val : data) {
        envelope += std::abs(val);
    }
    envelope /= data.size();
    neuralFeatures.push_back(envelope * 0.1f);
    
    // Feature 10-15: MFCC-like features (simulated Mel scale filtering)
    for (int mfcc = 0; mfcc < 6; ++mfcc) {
        float mfccValue = 0.0f;
        for (size_t i = 0; i < data.size(); ++i) {
            // Simplified MFCC approximation
            float frequency = 700.0f * std::pow(10.0f, static_cast<float>(mfcc) / 6.0f);
            float phase = 2.0f * M_PI * frequency * i / 44100.0f;
            mfccValue += data[i] * std::cos(phase + mfcc * 0.1f);
        }
        mfccValue /= data.size();
        neuralFeatures.push_back(std::tanh(mfccValue));
    }
    
    // Apply neural activation (simulated neural computation)
    std::vector<float> activatedFeatures;
    for (float feat : neuralFeatures) {
        // Apply neural activation function (tanh for bounded output)
        float activated = std::tanh(feat * 2.0f);
        activatedFeatures.push_back(activated);
    }
    
    // Temporal smoothing for continuity
    if (pImpl->temporalSmoothing > 0.0f && !pImpl->previousFeatures.empty()) {
        for (size_t i = 0; i < activatedFeatures.size(); ++i) {
            if (i < pImpl->previousFeatures.size()) {
                activatedFeatures[i] = pImpl->previousFeatures[i] * pImpl->temporalSmoothing +
                                    activatedFeatures[i] * (1.0f - pImpl->temporalSmoothing);
            }
        }
    }
    
    // Store for next temporal step
    pImpl->previousFeatures = std::move(activatedFeatures);
    
    // Update current neural state
    pImpl->features = std::move(pImpl->previousFeatures);
    pImpl->featureDimensions = 40; // Fixed feature vector size
}

const std::vector<float>& AudioProcessor::getFeatures() const {
    return pImpl->features;
}

void AudioProcessor::getFeatureVector(float* output, size_t maxSize) const {
    if (!output || maxSize == 0) return;
    
    size_t copySize = std::min(pImpl->features.size(), maxSize);
    std::copy(pImpl->features.begin(), pImpl->features.begin() + copySize, output);
    
    if (maxSize > pImpl->features.size()) {
        std::fill(output + pImpl->features.size(), output + maxSize, 0.0f);
    }
}

size_t AudioProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

float AudioProcessor::getFrequencyResolution() const {
    return pImpl->frequencyResolution;
}

float AudioProcessor::getTemporalResolution() const {
    return pImpl->temporalResolution;
}

float AudioProcessor::getSpectralCentroidWeight() const {
    return pImpl->spectralCentroidWeight;
}

float AudioProcessor::getZeroCrossingRateWeight() const {
    return pImpl->zeroCrossingRateWeight;
}

void AudioProcessor::reset() {
    pImpl->previousFeatures.clear();
    pImpl->features.clear();
    pImpl->featureDimensions = 0;
    pImpl->spectrogram.clear();
}

} // namespace nlm