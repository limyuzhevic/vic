#include "Audio.hpp"
#include "../brain/Brain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../core/Types/Types.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/STDP.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include <cmath>
#include <vector>
#include <algorithm>
#include <complex>
#include <unordered_map>
#include <random>

namespace nlm {

// Audio processing parameters
constexpr float DEFAULT_SAMPLE_RATE = 44100.0f;
constexpr float COCHLEAR_SCALING_FACTOR = 0.25f;
constexpr float PHASE_LOCKING_THRESHOLD = 0.6f;
constexpr float FILTER_COEFFICIENT = 0.5f;
constexpr size_t COCHLEAR_BASELINE_RATE = 80;
constexpr size_t COCHLEAR_MAX_RATE = 350;

// Gammatone filter parameters for frequency mapping
struct GammatoneFilter {
    float centerFreq;
    float bandwidth;
    float phi;
    float a;  // gain factor
    std::vector<float> history;
    
    GammatoneFilter(float cf, float sr = DEFAULT_SAMPLE_RATE) {
        centerFreq = cf;
        bandwidth = 1.019f * 2.0f * 3.14159f * cf * COCHLEAR_SCALING_FACTOR;
        phi = 2.0f * 3.14159f * cf * 0.005f;  // phase offset
        a = std::exp(-bandwidth);
        history.resize(4, 0.0f);
    }
};

// Binaural processing for sound localization
struct BinauralProcessor {
    std::vector<float> leftInput;
    std::vector<float> rightInput;
    std::vector<float> interauralLevelDifference;
    std::vector<float> interauralTimeDifference;
    std::vector<float> coherence;
    
    BinauralProcessor() {}
    
    void process(const std::vector<float>& left, const std::vector<float>& right) {
        leftInput = left;
        rightInput = right;
        interauralLevelDifference.resize(std::min(left.size(), right.size()));
        interauralTimeDifference.resize(std::min(left.size(), right.size()));
        coherence.resize(std::min(left.size(), right.size()));
        
        for (size_t i = 0; i < interauralLevelDifference.size(); ++i) {
            interauralLevelDifference[i] = std::log10(
                std::abs(left[i]) / (std::abs(right[i]) + 1e-8f) + 1e-8f);
            
            // Simple ITD calculation
            interauralTimeDifference[i] = (left[i] > right[i]) ? 0.001f : -0.001f;
            
            // Coherence calculation
            float leftPower = left[i] * left[i];
            float rightPower = right[i] * right[i];
            float crossPower = left[i] * right[i];
            float denominator = std::sqrt((leftPower + rightPower) * (leftPower + rightPower));
            coherence[i] = (denominator > 0.0f) ? (crossPower / denominator) : 0.0f;
        }
    }
    
    float getAzimuth() const {
        if (interauralLevelDifference.empty()) return 0.0f;
        float sumILD = 0.0f;
        for (float ild : interauralLevelDifference) {
            sumILD += ild;
        }
        return std::atan2(sumILD, 1.0f) * 180.0f / 3.14159f;
    }
    
    float getElevation() const {
        if (interauralTimeDifference.empty()) return 0.0f;
        float sumITD = 0.0f;
        for (float itd : interauralTimeDifference) {
            sumITD += itd;
        }
        return std::asin(sumITD / 0.001f) * 180.0f / 3.14159f;
    }
};

// Auditory transduction model (hair cell mechanotransduction)
class AuditoryTransduction {
public:
    struct HairCellState {
        float basilarMembraneDisplacement;
        float transductionCurrent;
        float adaptationFactor;
        float phaseLockingPhase;
        float phaseLockingRate;
    };
    
    AuditoryTransduction() {
        rng.seed(12345);
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    }
    
    std::vector<float> process(const std::vector<float>& audioSamples, float sampleRate) {
        std::vector<float> transducedSamples(audioSamples.size());
        
        for (size_t i = 0; i < audioSamples.size(); ++i) {
            // Basilar membrane displacement (place theory - frequency mapping)
            float frequency = calculateFrequencyFromPosition(i, audioSamples.size(), sampleRate);
            float displacement = audioSamples[i] * getFrequencyTuning(frequency);
            
            // Hair cell transduction
            HairCellState state = hairCellStates[i % hairCellStates.size()];
            state.basilarMembraneDisplacement = displacement;
            
            // Transduction current with adaptation
            float transductionCurrent = displacement * 
                                       (1.0f + state.adaptationFactor * std::sin(state.phaseLockingPhase)) *
                                       (1.0f - std::exp(-0.1f * std::abs(displacement)));
            
            // Phase locking for temporal precision
            float phaseAdvance = 2.0f * 3.14159f * frequency * (1.0f / sampleRate);
            state.phaseLockingPhase = std::fmod(state.phaseLockingPhase + phaseAdvance, 2.0f * 3.14159f);
            
            if (std::abs(std::sin(state.phaseLockingPhase)) > PHASE_LOCKING_THRESHOLD) {
                state.phaseLockingRate = COCHLEAR_BASELINE_RATE + 
                                       (COCHLEAR_MAX_RATE - COCHLEAR_BASELINE_RATE) * 
                                       std::abs(std::sin(state.phaseLockingPhase));
            }
            
            // Update hair cell state
            state.adaptationFactor = std::max(0.0f, state.adaptationFactor - 0.01f + displacement * 0.05f);
            hairCellStates[i % hairCellStates.size()] = state;
            
            // Generate neural firing based on phase locking
            float neuralFiring = generateAuditoryNerveFiring(state.phaseLockingRate, sampleRate);
            transducedSamples[i] = neuralFiring * 0.1f;  // Scale down for neural integration
        }
        
        return transducedSamples;
    }
    
private:
    std::vector<HairCellState> hairCellStates;
    std::mt19937 rng;
    
    float calculateFrequencyFromPosition(size_t position, size_t totalLength, float sampleRate) {
        // Place theory: different positions along cochlea respond to different frequencies
        float positionRatio = static_cast<float>(position) / static_cast<float>(totalLength);
        // Cochlear map: high frequencies at base, low at apex
        return sampleRate * 20.0f * std::pow(1000.0f / sampleRate, positionRatio);
    }
    
    float getFrequencyTuning(float frequency) {
        // Frequency-dependent gain (sharper tuning at higher frequencies)
        return std::exp(-std::pow(frequency / (sampleRate / 8.0f), 2.0f)) * 2.0f;
    }
    
    float generateAuditoryNerveFiring(float rate, float sampleRate) {
        // Phase-locked firing with refractory period
        static std::vector<float> spikeHistory;
        float dt = 1.0f / sampleRate;
        
        // Generate spikes based on rate with refractory period
        float probability = rate * dt * 0.01f;  // Small probability per timestep
        probability = std::min(1.0f, probability);
        
        float spike = (rng() % 10000 < probability * 10000) ? 1.0f : 0.0f;
        
        // Simple refractory period
        if (spike > 0.5f) {
            spikeHistory.push_back(spike);
            if (spikeHistory.size() > 10) {
                spikeHistory.erase(spikeHistory.begin());
            }
            return 1.0f;
        }
        
        return spike;
    }
};

// Spectral processing using gammatone filter banks
class SpectralProcessor {
public:
    SpectralProcessor() {
        // Create gammatone filter bank (typical human hearing range: 20Hz - 20kHz)
        createFilterBank(20.0f, 20000.0f, 32);
    }
    
    std::vector<float> process(const std::vector<float>& audioSamples, float sampleRate) {
        std::vector<float> spectrogram(audioSamples.size());
        
        // Apply filter bank
        std::vector<std::vector<float>> filterOutputs;
        for (auto& filter : filters) {
            std::vector<float> output = applyFilter(audioSamples, filter, sampleRate);
            filterOutputs.push_back(output);
        }
        
        // Compute spectral features
        std::vector<float> spectralFeatures;
        spectralFeatures.reserve(filterOutputs.size() * 10);  // Include multiple frames
        
        for (size_t frame = 0; frame < filterOutputs[0].size(); frame += 10) {
            std::vector<float> frameFeatures;
            for (size_t i = 0; i < filterOutputs.size(); ++i) {
                if (frame + 9 < filterOutputs[i].size()) {
                    float frameEnergy = 0.0f;
                    float frameFlux = 0.0f;
                    float frameCentroid = 0.0f;
                    
                    for (size_t j = 0; j < 10; ++j) {
                        float sample = filterOutputs[i][frame + j];
                        frameEnergy += sample * sample;
                        if (j > 0) {
                            frameFlux += std::abs(sample - filterOutputs[i][frame + j - 1]);
                        }
                        frameCentroid += static_cast<float>(j) * sample;
                    }
                    
                    frameCentroid = (frameEnergy > 0.0f) ? (frameCentroid / frameEnergy) : 0.0f;
                    
                    // Normalize features
                    float normalizedEnergy = std::log10(frameEnergy + 1e-6f);
                    float normalizedFlux = frameFlux / 10.0f;
                    
                    frameFeatures.push_back(normalizedEnergy);
                    frameFeatures.push_back(normalizedFlux);
                    frameFeatures.push_back(frameCentroid);
                }
            }
            
            spectralFeatures.insert(spectralFeatures.end(), frameFeatures.begin(), frameFeatures.end());
        }
        
        return spectralFeatures;
    }
    
    // Extract pitch (fundamental frequency) from spectral features
    float extractPitch(const std::vector<float>& audioSamples, float sampleRate) {
        if (audioSamples.size() < 10) return 0.0f;
        
        // Simple autocorrelation-based pitch detection
        float bestCorrelation = 0.0f;
        float bestLag = 0.0f;
        
        for (float lag = 1.0f; lag < static_cast<float>(audioSamples.size()) / 2.0f; lag += 0.1f) {
            float correlation = 0.0f;
            for (size_t i = 0; i < audioSamples.size() - lag; i += 10) {
                correlation += audioSamples[i] * audioSamples[i + static_cast<size_t>(lag)];
            }
            correlation /= audioSamples.size();
            
            if (correlation > bestCorrelation) {
                bestCorrelation = correlation;
                bestLag = lag;
            }
        }
        
        return (bestLag > 0.0f) ? (sampleRate / bestLag) : 0.0f;
    }
    
    // Extract timbre (spectral shape)
    std::vector<float> extractTimbre(const std::vector<float>& spectralFeatures) {
        std::vector<float> timbreFeatures;
        
        if (spectralFeatures.size() < 6) return timbreFeatures;
        
        // Extract spectral envelope and shape characteristics
        for (size_t i = 0; i < spectralFeatures.size(); i += 3) {
            if (i + 2 < spectralFeatures.size()) {
                float energy = spectralFeatures[i];
                float flux = spectralFeatures[i + 1];
                float centroid = spectralFeatures[i + 2];
                
                // Timbre descriptors
                timbreFeatures.push_back(std::tanh(energy));  // Normalized energy
                timbreFeatures.push_back(std::tanh(flux));    // Normalized spectral flux
                timbreFeatures.push_back(std::tanh(centroid)); // Normalized spectral centroid
            }
        }
        
        return timbreFeatures;
    }
    
    // Extract speech-related features
    std::vector<float> extractSpeechFeatures(const std::vector<float>& audioSamples, float sampleRate) {
        std::vector<float> speechFeatures;
        
        // Zero-crossing rate
        float zeroCrossings = 0.0f;
        for (size_t i = 1; i < audioSamples.size(); ++i) {
            if ((audioSamples[i - 1] < 0.0f && audioSamples[i] >= 0.0f) ||
                (audioSamples[i - 1] >= 0.0f && audioSamples[i] < 0.0f)) {
                zeroCrossings++;
            }
        }
        speechFeatures.push_back(zeroCrossings / audioSamples.size());
        
        // Energy envelope
        float totalEnergy = 0.0f;
        for (float sample : audioSamples) {
            totalEnergy += sample * sample;
        }
        speechFeatures.push_back(std::log10(totalEnergy + 1e-6f));
        
        // Estimate fundamental frequency (rough)
        float pitch = extractPitch(audioSamples, sampleRate);
        speechFeatures.push_back(pitch / sampleRate);  // Normalized pitch
        
        return speechFeatures;
    }
    
private:
    std::vector<GammatoneFilter> filters;
    
    void createFilterBank(float minFreq, float maxFreq, size_t numFilters) {
        for (size_t i = 0; i < numFilters; ++i) {
            float freq = minFreq * std::pow(maxFreq / minFreq, static_cast<float>(i) / static_cast<float>(numFilters - 1));
            filters.emplace_back(freq);
        }
    }
    
    std::vector<float> applyFilter(const std::vector<float>& input, 
                                 const GammatoneFilter& filter, float sampleRate) {
        std::vector<float> output(input.size(), 0.0f);
        
        for (size_t i = 0; i < input.size(); ++i) {
            // Simplified gammatone filter implementation
            float inputSample = input[i];
            
            // Update filter state
            float delayed1 = filter.history[0];
            float delayed2 = filter.history[1];
            float delayed3 = filter.history[2];
            float delayed4 = filter.history[3];
            
            // Difference equation for gammatone filter
            float outputSample = inputSample - 
                               filter.a * delayed1 +
                               filter.a * delayed2 -
                               filter.a * delayed3 +
                               filter.a * delayed4;
            
            // Update history
            filter.history[0] = outputSample;
            filter.history[1] = delayed1;
            filter.history[2] = delayed2;
            filter.history[3] = delayed3;
            
            output[i] = outputSample;
        }
        
        return output;
    }
};

struct AudioProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    float sampleRate;
    
    // Neural integration components
    std::unique_ptr<AuditoryTransduction> transductionModel;
    std::unique_ptr<SpectralProcessor> spectralProcessor;
    std::unique_ptr<BinauralProcessor> binauralProcessor;
    
    // Temporal processing
    std::vector<float> envelopeHistory;
    std::vector<float> phaseHistory;
    std::vector<float> spectralFluxHistory;
    
    // Neural plasticity integration
    float temporalPlasticity;
    float spectralPlasticity;
    float binauralPlasticity;
    
    Impl() : featureDimensions(0), sampleRate(DEFAULT_SAMPLE_RATE),
             temporalPlasticity(1.0f), spectralPlasticity(1.0f), binauralPlasticity(1.0f) {
        transductionModel = std::make_unique<AuditoryTransduction>();
        spectralProcessor = std::make_unique<SpectralProcessor>();
        binauralProcessor = std::make_unique<BinauralProcessor>();
        envelopeHistory.resize(1024, 0.0f);
        phaseHistory.resize(1024, 0.0f);
        spectralFluxHistory.resize(1024, 0.0f);
    }
};

AudioProcessor::AudioProcessor() : pImpl(new Impl) {}

AudioProcessor::~AudioProcessor() = default;

void AudioProcessor::process(const Audio& input) {
    // Get audio data
    std::vector<float> audioSamples = input.getData();
    float sampleRate = input.getSampleRate() > 0 ? input.getSampleRate() : pImpl->sampleRate;
    
    // Reset features
    pImpl->features.clear();
    
    if (audioSamples.empty()) {
        pImpl->featureDimensions = 0;
        return;
    }
    
    // 1. Real auditory transduction (hair cell mechanotransduction)
    std::vector<float> transducedSamples = pImpl->transductionModel->process(audioSamples, sampleRate);
    
    // 2. Binaural processing for sound localization
    if (audioSamples.size() >= 2) {
        // Create stereo representation (if not already stereo)
        std::vector<float> leftChannel, rightChannel;
        if (audioSamples.size() % 2 == 0) {
            // Assume interleaved stereo
            leftChannel.resize(audioSamples.size() / 2);
            rightChannel.resize(audioSamples.size() / 2);
            for (size_t i = 0; i < audioSamples.size() / 2; ++i) {
                leftChannel[i] = audioSamples[2 * i];
                rightChannel[i] = audioSamples[2 * i + 1];
            }
        } else {
            // Mono to stereo
            leftChannel = audioSamples;
            rightChannel = audioSamples;
        }
        
        pImpl->binauralProcessor->process(leftChannel, rightChannel);
        
        // Add binaural features to output
        pImpl->features.push_back(pImpl->binauralProcessor->getAzimuth());  // Sound location (azimuth)
        pImpl->features.push_back(pImpl->binauralProcessor->getElevation());  // Sound location (elevation)
    }
    
    // 3. Spectral processing using filter banks
    std::vector<float> spectralFeatures = pImpl->spectralProcessor->process(transducedSamples, sampleRate);
    pImpl->features.insert(pImpl->features.end(), spectralFeatures.begin(), spectralFeatures.end());
    
    // 4. Temporal processing for sound envelope and timing
    std::vector<float> temporalFeatures;
    
    // Envelope extraction
    float currentEnvelope = 0.0f;
    for (float sample : transducedSamples) {
        currentEnvelope = std::sqrt(currentEnvelope * currentEnvelope * 0.95f + 
                                  sample * sample * 0.05f);
    }
    pImpl->features.push_back(currentEnvelope);
    
    // Spectral flux
    float spectralFlux = 0.0f;
    if (!pImpl->spectralFluxHistory.empty()) {
        for (size_t i = 0; i < pImpl->spectralFluxHistory.size(); ++i) {
            spectralFlux += std::abs(currentEnvelope - pImpl->spectralFluxHistory[i]);
        }
        spectralFlux /= pImpl->spectralFluxHistory.size();
    }
    pImpl->features.push_back(spectralFlux);
    pImpl->spectralFluxHistory[0] = currentEnvelope;
    
    // Timbre features
    std::vector<float> timbreFeatures = pImpl->spectralProcessor->extractTimbre(spectralFeatures);
    pImpl->features.insert(pImpl->features.end(), timbreFeatures.begin(), timbreFeatures.end());
    
    // Speech features
    std::vector<float> speechFeatures = pImpl->spectralProcessor->extractSpeechFeatures(transducedSamples, sampleRate);
    pImpl->features.insert(pImpl->features.end(), speechFeatures.begin(), speechFeatures.end());
    
    // Extract pitch
    float pitch = pImpl->spectralProcessor->extractPitch(transducedSamples, sampleRate);
    pImpl->features.push_back(pitch);
    
    // 5. Update neural plasticity based on audio input
    updateNeuralPlasticity(transducedSamples, spectralFeatures);
    
    // Calculate final feature dimensions
    pImpl->featureDimensions = pImpl->features.size();
}

const std::vector<float>& AudioProcessor::getFeatures() const {
    return pImpl->features;
}

size_t AudioProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

void AudioProcessor::Impl::updateNeuralPlasticity(const std::vector<float>& audioSamples, 
                                                 const std::vector<float>& spectralFeatures) {
    // Temporal plasticity (Hebbian learning based on temporal patterns)
    if (!audioSamples.empty()) {
        float temporalPattern = 0.0f;
        for (size_t i = 1; i < audioSamples.size(); ++i) {
            temporalPattern += std::abs(audioSamples[i] - audioSamples[i - 1]);
        }
        temporalPattern /= audioSamples.size();
        
        // Modulate plasticity based on temporal regularity
        temporalPlasticity = 0.9f + 0.1f * std::tanh(temporalPattern * 10.0f);
    }
    
    // Spectral plasticity (STDP-like learning for spectral patterns)
    if (!spectralFeatures.empty()) {
        float spectralVariation = 0.0f;
        for (size_t i = 1; i < spectralFeatures.size(); ++i) {
            spectralVariation += std::abs(spectralFeatures[i] - spectralFeatures[i - 1]);
        }
        spectralVariation /= spectralFeatures.size();
        
        // Enhance plasticity for novel spectral patterns
        spectralPlasticity = 0.5f + 0.5f * std::tanh(spectralVariation * 5.0f);
    }
    
    // Binaural plasticity (for sound localization learning)
    float binauralCoherence = 0.0f;
    if (binauralProcessor) {
        // Simple coherence calculation from binaural processor
        float azimuth = binauralProcessor->getAzimuth();
        float elevation = binauralProcessor->getElevation();
        binauralCoherence = 1.0f - std::sqrt(azimuth * azimuth + elevation * elevation) / 180.0f;
    }
    
    binauralPlasticity = 0.3f + 0.7f * binauralCoherence;
}

} // namespace nlm
