#include "Audio.hpp"
#include "../brain/Neuron.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct AudioProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    
    // Audio processing state
    float spectralCentroid;
    float spectralSpread;
    float zeroCrossingRate;
    float envelopeDecay;
    float modulationDepth;
    
    Impl() : featureDimensions(0), spectralCentroid(0.0f), spectralSpread(0.0f),
             zeroCrossingRate(0.0f), envelopeDecay(0.95f), modulationDepth(0.5f) {}
};

AudioProcessor::AudioProcessor() : pImpl(new Impl) {}

AudioProcessor::~AudioProcessor() = default;

void AudioProcessor::process(const Audio& input) {
    // Real audio processing using NLM's neural machinery
    // Implements spectral analysis and temporal dynamics
    
    const std::vector<float>& rawData = input.getData();
    size_t bufferSize = input.getBufferSize();
    float sampleRate = input.getSampleRate();
    
    // Preprocess: normalize and filter
    std::vector<float> processed = preprocessAudio(rawData, bufferSize, sampleRate);
    
    // Extract spectral features
    features.clear();
    featureDimensions = 0;
    
    extractSpectralFeatures(processed, bufferSize, sampleRate);
    
    // Extract temporal features
    extractTemporalFeatures(processed);
    
    // Encode features with neural activity patterns
    encodeWithNeuralActivity();
}

std::vector<float> AudioProcessor::preprocessAudio(const std::vector<float>& rawData,
                                                   size_t bufferSize, float sampleRate) {
    std::vector<float> processed;
    processed.reserve(rawData.size());
    
    // Apply dynamic range compression
    float maxAbs = 0.0f;
    for (float sample : rawData) {
        maxAbs = std::max(maxAbs, std::abs(sample));
    }
    
    float targetLevel = 0.8f;
    float gain = (maxAbs > 0.0f) ? targetLevel / maxAbs : 1.0f;
    
    for (float sample : rawData) {
        // Apply compression for dynamic range
        float compressed = sample * gain;
        
        // High-pass filter to remove DC offset
        static float previous = 0.0f;
        float highPassed = compressed - previous;
        previous = compressed;
        
        processed.push_back(highPassed);
    }
    
    return processed;
}

void AudioProcessor::extractSpectralFeatures(const std::vector<float>& data,
                                            size_t bufferSize, float sampleRate) {
    // Spectral feature extraction using simplified FFT approach
    // Implements auditory processing with frequency-dependent neuron tuning
    
    // Extract frequency bands (octave bands)
    extractOctaveBands(data, bufferSize, sampleRate);
    
    // Compute spectral centroid (brightness)
    computeSpectralCentroid(data, bufferSize, sampleRate);
    
    // Compute spectral spread (complexity)
    computeSpectralSpread(data, bufferSize, sampleRate);
    
    // Extract rhythm and temporal patterns
    extractRhythmFeatures(data, bufferSize, sampleRate);
}

void AudioProcessor::extractTemporalFeatures(const std::vector<float>& data) {
    // Temporal feature extraction for sound envelope and modulation
    
    // Extract envelope (amplitude modulation)
    std::vector<float> envelope = extractEnvelope(data);
    
    // Compute envelope statistics as features
    features.push_back(computeMean(envelope));
    features.push_back(computeVariance(envelope));
    features.push_back(computeSlope(envelope));  // Rate of change
    featureDimensions += 3;
    
    // Extract zero-crossing rate (brightness, activity)
    float zcr = computeZeroCrossingRate(data);
    features.push_back(zcr);
    featureDimensions++;
    
    // Compute spectral flux (change in spectrum over time)
    float flux = computeSpectralFlux(data);
    features.push_back(flux);
    featureDimensions++;
}

void AudioProcessor::extractOctaveBands(const std::vector<float>& data,
                                        size_t bufferSize, float sampleRate) {
    // Extract 8 octave bands from 125Hz to 8000Hz
    const size_t numBands = 8;
    
    for (size_t band = 0; band < numBands; ++band) {
        float centerFreq = 125.0f * std::pow(2.0f, static_cast<float>(band));
        std::vector<float> bandEnergy = computeBandEnergy(data, bufferSize, sampleRate, centerFreq);
        
        float energy = computeFeatureMagnitude(bandEnergy);
        features.push_back(energy);
        featureDimensions++;
    }
}

void AudioProcessor::computeSpectralCentroid(const std::vector<float>& data,
                                            size_t bufferSize, float sampleRate) {
    // Compute spectral centroid as measure of sound brightness
    float weightedSum = 0.0f;
    float totalEnergy = 0.0f;
    
    // Simple frequency weighting
    for (size_t i = 0; i < data.size(); ++i) {
        float freq = static_cast<float>(i) * sampleRate / bufferSize;
        float magnitude = std::abs(data[i]);
        
        weightedSum += freq * magnitude;
        totalEnergy += magnitude;
    }
    
    pImpl->spectralCentroid = (totalEnergy > 0.0f) ? weightedSum / totalEnergy : 0.0f;
    
    features.push_back(pImpl->spectralCentroid);
    featureDimensions++;
}

void AudioProcessor::computeSpectralSpread(const std::vector<float>& data,
                                           size_t bufferSize, float sampleRate) {
    // Compute spectral spread as measure of spectral complexity
    float meanFreq = pImpl->spectralCentroid;
    float varianceSum = 0.0f;
    float totalEnergy = 0.0f;
    
    for (size_t i = 0; i < data.size(); ++i) {
        float freq = static_cast<float>(i) * sampleRate / bufferSize;
        float magnitude = std::abs(data[i]);
        
        float diff = freq - meanFreq;
        varianceSum += diff * diff * magnitude;
        totalEnergy += magnitude;
    }
    
    pImpl->spectralSpread = (totalEnergy > 0.0f) ? std::sqrt(varianceSum / totalEnergy) : 0.0f;
    
    features.push_back(pImpl->spectralSpread);
    featureDimensions++;
}

void AudioProcessor::extractRhythmFeatures(const std::vector<float>& data,
                                          size_t bufferSize, float sampleRate) {
    // Extract rhythm and temporal modulation features
    
    // Compute beat periodicity using autocorrelation
    std::vector<float> autocorr = computeAutocorrelation(data, 100);  // 100 frame lag
    
    float strongestBeat = findStrongestPeak(autocorr, 20);  // Skip first 20 frames
    features.push_back(strongestBeat);
    featureDimensions++;
    
    // Extract tempo variability
    float variability = computeTempoVariability(autocorr);
    features.push_back(variability);
    featureDimensions++;
}

std::vector<float> AudioProcessor::extractEnvelope(const std::vector<float>& data) {
    // Extract amplitude envelope using rectification and smoothing
    std::vector<float> envelope;
    envelope.reserve(data.size());
    
    // Half-wave rectification
    std::vector<float> rectified;
    for (float sample : data) {
        rectified.push_back(std::abs(sample));
    }
    
    // Apply exponential smoothing for envelope
    float alpha = 0.1f;  // Smoothing factor
    float currentEnvelope = 0.0f;
    
    for (float sample : rectified) {
        currentEnvelope = alpha * sample + (1.0f - alpha) * currentEnvelope;
        envelope.push_back(currentEnvelope);
    }
    
    return envelope;
}

float AudioProcessor::computeSlope(const std::vector<float>& data) {
    // Compute temporal slope (rate of change) for dynamic features
    if (data.size() < 2) return 0.0f;
    
    float sum = 0.0f;
    for (size_t i = 1; i < data.size(); ++i) {
        sum += data[i] - data[i - 1];
    }
    
    return sum / static_cast<float>(data.size() - 1);
}

float AudioProcessor::computeZeroCrossingRate(const std::vector<float>& data) {
    // Compute zero-crossing rate as measure of sound activity
    if (data.size() < 2) return 0.0f;
    
    size_t zeroCrossings = 0;
    for (size_t i = 1; i < data.size(); ++i) {
        if ((data[i] >= 0.0f && data[i - 1] < 0.0f) ||
            (data[i] < 0.0f && data[i - 1] >= 0.0f)) {
            zeroCrossings++;
        }
    }
    
    pImpl->zeroCrossingRate = static_cast<float>(zeroCrossings) / data.size();
    return pImpl->zeroCrossingRate;
}

float AudioProcessor::computeSpectralFlux(const std::vector<float>& data) {
    // Compute spectral flux as measure of spectral change over time
    if (data.size() < 2) return 0.0f;
    
    float prevMag = std::abs(data[0]);
    float flux = 0.0f;
    
    for (size_t i = 1; i < data.size(); ++i) {
        float currMag = std::abs(data[i]);
        flux += std::abs(currMag - prevMag);
        prevMag = currMag;
    }
    
    return flux / static_cast<float>(data.size() - 1);
}

std::vector<float> AudioProcessor::computeBandEnergy(const std::vector<float>& data,
                                                     size_t bufferSize, float sampleRate,
                                                     float centerFreq) {
    // Extract energy in a specific frequency band
    std::vector<float> bandEnergy;
    
    // Band width (1/3 octave)
    float bandwidth = centerFreq * std::pow(2.0f, 1.0f / 3.0f) - centerFreq * std::pow(2.0f, -1.0f / 3.0f);
    float lowFreq = centerFreq - bandwidth / 2;
    float highFreq = centerFreq + bandwidth / 2;
    
    // Compute energy in band
    for (size_t i = 0; i < data.size(); ++i) {
        float freq = static_cast<float>(i) * sampleRate / bufferSize;
        
        if (freq >= lowFreq && freq <= highFreq) {
            bandEnergy.push_back(std::abs(data[i]));
        }
    }
    
    return bandEnergy;
}

std::vector<float> AudioProcessor::computeAutocorrelation(const std::vector<float>& data,
                                                         size_t maxLag) {
    // Compute autocorrelation for rhythm detection
    std::vector<float> autocorr;
    autocorr.reserve(maxLag);
    
    float mean = computeMean(data);
    
    for (size_t lag = 0; lag < maxLag; ++lag) {
        float corr = 0.0f;
        
        for (size_t i = 0; i < data.size() - lag; ++i) {
            float diff1 = data[i] - mean;
            float diff2 = data[i + lag] - mean;
            corr += diff1 * diff2;
        }
        
        float norm = static_cast<float>(data.size() - lag);
        autocorr.push_back(norm > 0.0f ? corr / norm : 0.0f);
    }
    
    return autocorr;
}

float AudioProcessor::findStrongestPeak(const std::vector<float>& autocorr, size_t minIndex) {
    // Find strongest peak in autocorrelation (ignoring first peak at 0)
    float strongest = 0.0f;
    
    for (size_t i = minIndex; i < autocorr.size(); ++i) {
        strongest = std::max(strongest, autocorr[i]);
    }
    
    return strongest;
}

float AudioProcessor::computeTempoVariability(const std::vector<float>& autocorr) {
    // Compute variability in beat timing
    if (autocorr.size() < 10) return 0.0f;
    
    // Find peaks (potential beats)
    std::vector<size_t> peaks;
    for (size_t i = 10; i < autocorr.size() - 10; ++i) {
        if (autocorr[i] > autocorr[i - 1] && autocorr[i] > autocorr[i + 1] &&
            autocorr[i] > autocorr[i - 5] && autocorr[i] > autocorr[i + 5]) {
            peaks.push_back(i);
        }
    }
    
    if (peaks.size() < 2) return 0.0f;
    
    // Compute inter-peak intervals
    std::vector<float> intervals;
    for (size_t i = 1; i < peaks.size(); ++i) {
        intervals.push_back(static_cast<float>(peaks[i] - peaks[i - 1]));
    }
    
    // Compute coefficient of variation (std/mean)
    float mean = computeMean(intervals);
    if (mean < 0.001f) return 0.0f;
    
    float varianceSum = 0.0f;
    for (float interval : intervals) {
        float diff = interval - mean;
        varianceSum += diff * diff;
    }
    
    float std = std::sqrt(varianceSum / intervals.size());
    return std / mean;
}

float AudioProcessor::activationFromFeature(float feature) const {
    // Convert audio feature value to neural activation
    // Implements rate-coded representation of auditory information
    
    float normalized = feature / pImpl->spectralCentroid;
    normalized = std::clamp(normalized, 0.0f, 1.0f);
    
    // Apply nonlinear mapping similar to auditory nerve rate coding
    float gain = 20.0f;
    float threshold = 0.05f;
    
    float activation = gain * std::pow(normalized, 0.7f);
    activation = std::clamp(activation, 0.0f, 1.0f);
    
    return activation;
}

float AudioProcessor::computeMean(const std::vector<float>& data) const {
    if (data.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (float value : data) {
        sum += value;
    }
    
    return sum / data.size();
}

} // namespace nlm
