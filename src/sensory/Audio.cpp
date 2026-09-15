#include "Audio.hpp"
#include "../brain/Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <vector>
#include <map>

namespace nlm {

struct AudioProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    size_t sampleRate;
    size_t numSamples;
    std::vector<float> rawInput;
    
    // Auditory processing stages
    std::vector<float> cochleaResponses;      // Cochlear frequency analysis
    std::vector<float> nerveResponses;        // Auditory nerve encoding
    std::vector<float> brainstemResponses;    // Temporal and rhythmic processing
    std::vector<float> rewardSignals;         // Reward-modulated processing
    
    // Sound localization
    std::vector<float> interauralLevelDifferences;  // ILD for low frequencies
    std::vector<float> interauralTimeDifferences;   // ITD for high frequencies
    
    // Statistics
    size_t totalProcessed;
    float temporalIntegration;                // Adaptation time constant
    float soundSalience;                       // For reward learning
    float localizationConfidence;              // Sound source reliability
    
    Impl() : featureDimensions(0), sampleRate(0), numSamples(0),
             totalProcessed(0), temporalIntegration(0.1f),
             soundSalience(0.0f), localizationConfidence(0.0f) {}
};

AudioProcessor::AudioProcessor() : pImpl(new Impl) {}

AudioProcessor::~AudioProcessor() = default;

void AudioProcessor::process(const Audio& input) {
    // Store input parameters
    pImpl->sampleRate = input.getSampleRate();
    pImpl->numSamples = input.getNumSamples();
    pImpl->rawInput = input.getData();
    
    // Reset feature vectors
    pImpl->features.clear();
    pImpl->cochleaResponses.clear();
    pImpl->nerveResponses.clear();
    pImpl->brainstemResponses.clear();
    pImpl->interauralLevelDifferences.clear();
    pImpl->interauralTimeDifferences.clear();
    
    // Process audio through biologically plausible stages
    processCochlea();      // Frequency analysis via basilar membrane
    processAuditoryNerve();  // Temporal coding and phase locking
    processBrainstem();    // Sound localization and rhythmic processing
    extractFeatures();     // Temporal and spectral feature extraction
    
    pImpl->totalProcessed++;
}

void AudioProcessor::processCochlea() {
    // Simulate cochlear frequency analysis
    // Model basilar membrane as tonotopic map
    // Different frequencies peak at different locations
    
    const size_t channels = 64;  // Frequency channels (place coding)
    pImpl->cochleaResponses.resize(channels, 0.0f);
    
    // Convert sound pressure to neural activity in each frequency channel
    for (size_t channel = 0; channel < channels; ++channel) {
        float frequency = 100.0f * (channel + 1);  // 100Hz to 6.4kHz
        
        // Filter response: Gaussian-shaped frequency tuning
        float peakFrequency = 0.0f;
        if (!pImpl->rawInput.empty()) {
            // Estimate dominant frequency using FFT-like analysis
            float maxAmplitude = 0.0f;
            for (size_t i = 0; i < pImpl->rawInput.size() / 4; i += 4) {
                if (i + 3 < pImpl->rawInput.size()) {
                    // Simple frequency bin calculation
                    float binCenter = static_cast<float>(i) / pImpl->sampleRate;
                    float targetBin = frequency / pImpl->sampleRate * pImpl->rawInput.size();
                    float binDistance = std::abs(static_cast<float>(i) - targetBin);
                    
                    float amplitude = pImpl->rawInput[i];
                    float tuning = std::exp(-binDistance * binDistance / (frequency * 0.1f));
                    float response = amplitude * tuning;
                    
                    if (std::abs(response) > maxAmplitude) {
                        maxAmplitude = std::abs(response);
                    }
                }
            }
            peakFrequency = maxAmplitude;
        }
        
        // Apply non-linear transformation (logarithmic compression)
        float response = std::log(1.0f + std::abs(peakFrequency));
        
        // Add biological noise and spontaneous activity
        static RandomGenerator rng(45);
        float noise = rng.normal(0.0f, 0.05f);
        float spontaneous = rng.bernoulli(0.05f) * 0.1f;
        
        pImpl->cochleaResponses[channel] = std::clamp(response + noise + spontaneous, 0.0f, 1.0f);
    }
}

void AudioProcessor::processAuditoryNerve() {
    // Auditory nerve encoding: rate coding and phase locking
    const size_t size = pImpl->cochleaResponses.size();
    pImpl->nerveResponses.resize(size, 0.0f);
    
    for (size_t i = 0; i < size; ++i) {
        float cochlearOutput = pImpl->cochleaResponses[i];
        
        // Phase locking to temporal envelope
        // High frequencies: phase locking degrades with increasing frequency
        float frequency = 100.0f * (i + 1);
        float maxPhaseLockFrequency = (frequency < 1000.0f) ? 800.0f : 200.0f;
        
        float phaseLock = std::exp(-frequency / maxPhaseLockFrequency);
        
        // Rate coding: spike probability proportional to stimulus intensity
        float spikeRate = cochlearOutput * phaseLock * 50.0f;  // up to 50 Hz
        
        // Temporal integration with adaptation
        pImpl->temporalIntegration *= 0.95f;
        pImpl->temporalIntegration += (1.0f - pImpl->temporalIntegration) * spikeRate * 0.01f;
        
        // Convert rate to neural response with refractory period
        float neuralResponse = spikeRate * (1.0f - pImpl->temporalIntegration * 0.1f);
        
        // Add biological noise
        static RandomGenerator rng(46);
        float noise = rng.normal(0.0f, 0.03f);
        
        pImpl->nerveResponses[i] = std::clamp(neuralResponse + noise, 0.0f, 1.0f);
    }
}

void AudioProcessor::processBrainstem() {
    // Brainstem processing: sound localization and rhythmic integration
    const size_t size = pImpl->nerveResponses.size();
    pImpl->brainstemResponses.resize(size, 0.0f);
    
    // Sound localization processing
    pImpl->interauralLevelDifferences.resize(size, 0.0f);
    pImpl->interauralTimeDifferences.resize(size, 0.0f);
    
    for (size_t i = 0; i < size; ++i) {
        float nerveOutput = pImpl->nerveResponses[i];
        float frequency = 100.0f * (i + 1);
        
        // Interaural Level Difference (ILD) - dominant for low frequencies (<1500 Hz)
        float ild = 0.0f;
        if (frequency < 1500.0f) {
            ild = (nerveOutput - 0.5f) * 0.3f;  // Asymmetric response to level differences
        }
        pImpl->interauralLevelDifferences[i] = std::clamp(ild, -1.0f, 1.0f);
        
        // Interaural Time Difference (ITD) - dominant for high frequencies (>1500 Hz)
        float itd = 0.0f;
        if (frequency >= 1500.0f) {
            itd = std::sin(2.0f * frequency * pImpl->temporalIntegration * 0.001f) * 0.2f;
        }
        pImpl->interauralTimeDifferences[i] = std::clamp(itd, -1.0f, 1.0f);
        
        // Integrate ILD and ITD for source localization confidence
        float localizationStrength = std::sqrt(ild * ild + itd * itd);
        pImpl->localizationConfidence = (pImpl->localizationConfidence * 0.9f) + 
                                       localizationStrength * 0.1f;
        
        // Rhythm generation and temporal processing
        float rhythmResponse = nerveOutput * pImpl->temporalIntegration;
        
        // Burst detection for salient sounds
        if (nerveOutput > 0.7f) {
            // Salient sound detection
            pImpl->soundSalience = std::max(pImpl->soundSalience, nerveOutput * 0.5f);
        }
        
        // Reset salience decay
        pImpl->soundSalience *= 0.99f;
        
        pImpl->brainstemResponses[i] = std::clamp(rhythmResponse, 0.0f, 1.0f);
    }
}

void AudioProcessor::extractFeatures() {
    // Extract temporal and spectral features for working memory
    // Encode audio features as neural population responses
    
    // Spectral features
    std::vector<float> spectralFeatures;
    spectralFeatures.reserve(16);
    
    // Calculate spectral centroid, bandwidth, and flux
    float spectralCentroid = 0.0f, spectralFlux = 0.0f, spectralRolloff = 0.0f;
    float totalPower = 0.0f;
    
    for (size_t i = 0; i < pImpl->cochleaResponses.size(); ++i) {
        float power = pImpl->cochleaResponses[i] * pImpl->cochleaResponses[i];
        totalPower += power;
        
        float frequency = 100.0f * (i + 1);
        spectralCentroid += power * frequency;
    }
    
    if (totalPower > 0.0f) {
        spectralCentroid /= totalPower;
    }
    
    // Calculate spectral flux (rate of change)
    spectralFlux = pImpl->soundSalience * 0.5f;
    
    // Spectral roll-off (85% of total energy)
    float cumulativeEnergy = 0.0f;
    float energyThreshold = 0.85f * totalPower;
    for (size_t i = 0; i < pImpl->cochleaResponses.size(); ++i) {
        float power = pImpl->cochleaResponses[i] * pImpl->cochleaResponses[i];
        cumulativeEnergy += power;
        if (cumulativeEnergy >= energyThreshold) {
            spectralRolloff = 100.0f * (i + 1);
            break;
        }
    }
    
    spectralFeatures.push_back(spectralCentroid / 1000.0f);  // Normalize
    spectralFeatures.push_back(spectralFlux);
    spectralFeatures.push_back(spectralRolloff / 10000.0f);  // Normalize
    
    // Temporal features
    std::vector<float> temporalFeatures;
    temporalFeatures.reserve(16);
    
    // Calculate zero-crossing rate, tempo, and rhythm
    float zeroCrossings = 0.0f;
    float tempo = pImpl->temporalIntegration * 120.0f;  // BPM
    float rhythmRegularity = 1.0f - std::abs(pImpl->temporalIntegration - 0.5f);
    
    // Estimate from cochlear output
    for (size_t i = 1; i < pImpl->cochleaResponses.size(); ++i) {
        if ((pImpl->cochleaResponses[i] > 0.5f) != (pImpl->cochleaResponses[i-1] > 0.5f)) {
            zeroCrossings++;
        }
    }
    
    temporalFeatures.push_back(zeroCrossings / static_cast<float>(pImpl->cochleaResponses.size()));
    temporalFeatures.push_back(tempo / 200.0f);  // Normalize
    temporalFeatures.push_back(rhythmRegularity);
    
    // Localization features
    std::vector<float> localizationFeatures;
    localizationFeatures.reserve(8);
    
    // Combine ILD and ITD for source direction
    float avgILD = 0.0f, avgITD = 0.0f;
    for (size_t i = 0; i < pImpl->interauralLevelDifferences.size(); ++i) {
        avgILD += pImpl->interauralLevelDifferences[i];
        avgITD += pImpl->interauralTimeDifferences[i];
    }
    
    if (!pImpl->interauralLevelDifferences.empty()) {
        avgILD /= pImpl->interauralLevelDifferences.size();
        avgITD /= pImpl->interauralTimeDifferences.size();
    }
    
    localizationFeatures.push_back(avgILD);
    localizationFeatures.push_back(avgITD);
    localizationFeatures.push_back(pImpl->localizationConfidence);
    
    // Reward modulation features
    std::vector<float> rewardFeatures;
    rewardFeatures.reserve(8);
    
    rewardFeatures.push_back(pImpl->soundSalience);
    rewardFeatures.push_back(pImpl->temporalIntegration);
    
    // Combine all features
    pImpl->features.clear();
    
    // Add spectral features
    pImpl->features.insert(pImpl->features.end(), spectralFeatures.begin(), spectralFeatures.end());
    
    // Add temporal features
    pImpl->features.insert(pImpl->features.end(), temporalFeatures.begin(), temporalFeatures.end());
    
    // Add localization features
    pImpl->features.insert(pImpl->features.end(), localizationFeatures.begin(), localizationFeatures.end());
    
    // Add reward modulation features
    pImpl->features.insert(pImpl->features.end(), rewardFeatures.begin(), rewardFeatures.end());
    
    // Add global audio properties
    pImpl->features.push_back(static_cast<float>(pImpl->sampleRate));
    pImpl->features.push_back(pImpl->soundSalience);
    
    pImpl->featureDimensions = pImpl->features.size();
}

void AudioProcessor::updateReward(float reward) {
    // Update reward-modulated processing for audio
    // Implement reward prediction error for sound salience
    
    // Update salience based on reward prediction error
    float predictedSalience = pImpl->soundSalience * 0.9f;
    float predictionError = std::abs(reward - predictedSalience);
    
    // Enhance salience for prediction errors
    pImpl->soundSalience += predictionError * 0.1f;
    pImpl->soundSalience = std::min(pImpl->soundSalience, 1.0f);
    
    // Update temporal integration with reward
    pImpl->temporalIntegration = (pImpl->temporalIntegration * 0.9f) + 
                                (reward * 0.05f);
}

const std::vector<float>& AudioProcessor::getFeatures() const {
    return pImpl->features;
}

void AudioProcessor::setTemporalIntegration(float integration) {
    pImpl->temporalIntegration = integration;
}

} // namespace nlm
