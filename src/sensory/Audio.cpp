#include "Audio.hpp"
./brain/Neuron.hpp"
./brain/NeuralPopulation.hpp"
./brain/NeuralRegion.hpp"
./memory/NeuralWorkingMemory.hpp"
./memory/AdvancedWorkingMemory.hpp"
./memory/Memory.hpp"
./prediction/PredictionSystem.hpp"
./neuromodulation/Neuromodulator.hpp"
./neuromodulation/Curiosity.hpp"
./neuromodulation/Dopamine.hpp"
./core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <unordered_map>
#include <functional>
#include <complex>

namespace nlm {

struct AudioProcessor::Impl {
    // Neural computation components
    Brain* brain;
    
    // Audio processing neural populations
    std::vector<std::unique_ptr<NeuralRegion>> auditoryRegions;
    
    // Neural oscillators for frequency analysis
    struct NeuralOscillator {
        NeuronId id;
        float centerFrequency;  // Hz
        float bandwidth;  // Hz
        float phase;  // Phase in radians
        float amplitude;  // Amplitude of oscillation
        float preferredModulation;  // Preferred rate modulation
        float coherence;  // Phase coherence with input
        float activation;  // Current activation level
        
        NeuralOscillator() : id(), centerFrequency(440.0f), bandwidth(50.0f), phase(0.0f),
            amplitude(0.0f), preferredModulation(0.0f), coherence(0.0f), activation(0.0f) {}
    };
    
    std::vector<NeuralOscillator> oscillators;
    
    // Binaural processing for sound localization
    struct BinauralProcessor {
        NeuronId leftId;
        NeuronId rightId;
        
        float interauralLevelDifference;  // ILD
        float interauralTimeDifference;  // ITD
        float soundSourceAzimuth;  // Azimuth angle (-90 to 90 degrees)
        float soundSourceElevation;  // Elevation angle (-90 to 90 degrees)
        float confidence;  // Localization confidence
        
        BinauralProcessor() : leftId(), rightId(), interauralLevelDifference(0.0f),
            interauralTimeDifference(0.0f), soundSourceAzimuth(0.0f), soundSourceElevation(0.0f),
            confidence(0.0f) {}
    };
    
    BinauralProcessor binauralProcessor;
    
    // Echoic memory for temporal processing
    struct EchoicMemoryTrace {
        std::vector<float> waveform;  // Auditory waveform
        float startTime;
        float duration;
        float salience;
        float encodingStrength;
        bool consolidated;
        
        EchoicMemoryTrace() : startTime(0), duration(0), salience(0), encodingStrength(0), consolidated(false) {}
    };
    
    std::vector<EchoicMemoryTrace> echoicMemory;
    
    // Pattern recognition for auditory events
    enum class AuditoryPattern {
        Tone, Noise, ComplexTone, Speech, Music, EnvironmentalSound
    };
    
    struct PatternPopulation {
        std::vector<NeuronId> neurons;
        AuditoryPattern patternType;
        float activationThreshold;
        float tuningWidth;
        std::vector<float> patternFeatures;
    };
    
    std::vector<PatternPopulation> patternPopulations;
    
    // Sound localization using binaural processing
    struct SoundSource {
        float azimuth;  // Horizontal angle (-180 to 180 degrees)
        float elevation;  // Vertical angle (-90 to 90 degrees)
        float distance;  // Distance in meters
        float level;  // Sound level in dB
        float timeOfArrival;  // Time of arrival
        float confidence;  // Localization confidence
        bool tracked;  // Whether source is being tracked
        
        SoundSource() : azimuth(0), elevation(0), distance(0), level(0),
            timeOfArrival(0), confidence(0), tracked(false) {}
    };
    
    std::vector<SoundSource> soundSources;
    
    // Auditory working memory
    std::vector<float> auditoryWorkingMemory;
    std::vector<float> auditoryTemporalFeatures;
    std::vector<float> auditorySpectralFeatures;
    
    // Cross-modal integration with visual
    std::vector<float> crossModalIntegrationWeights;
    
    // Auditory gating and attention
    std::vector<float> auditoryGatingSignals;
    std::vector<float> auditoryAttentionSignals;
    
    // Motor planning integration
    std::vector<float> motorPlanningSignals;
    
    // Statistics
    std::vector<float> spectralFeatures;
    std::vector<float> temporalFeatures;
    std::vector<float> binauralFeatures;
    float processingEfficiency;
    
    // Random generator for biological variability
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    std::normal_distribution<float> normalDist;
    std::exponential_distribution<float> exponentialDist;
    
    Impl() : brain(nullptr), processingEfficiency(0.0f),
        rng(std::random_device{}()), uniformDist(0.0f, 1.0f), normalDist(0.0f, 1.0f),
        exponentialDist(1.0f) {
        // Initialize neural substrate components
        oscillators.reserve(32);
        echoicMemory.reserve(10);
        soundSources.reserve(8);
    }
};

AudioProcessor::AudioProcessor() : pImpl(new Impl) {}

AudioProcessor::~AudioProcessor() = default;

void AudioProcessor::process(const Audio& input) {
    if (!pImpl->brain) {
        NLM_LOG_WARNING("AudioProcessor: Brain not initialized");
        return;
    }
    
    const auto& inputData = input.getData();
    if (inputData.empty()) {
        return;
    }
    
    // Reset processing state
    pImpl->features.clear();
    pImpl->spectralFeatures.clear();
    pImpl->temporalFeatures.clear();
    pImpl->binauralFeatures.clear();
    pImpl->auditoryWorkingMemory.clear();
    pImpl->auditoryTemporalFeatures.clear();
    pImpl->auditorySpectralFeatures.clear();
    pImpl->auditoryGatingSignals.clear();
    pImpl->auditoryAttentionSignals.clear();
    pImpl->motorPlanningSignals.clear();
    pImpl->crossModalIntegrationWeights.clear();
    pImpl->soundSources.clear();
    
    size_t numSamples = input.getNumSamples();
    float sampleRate = static_cast<float>(input.getSampleRate());
    
    // Initialize binaural processing if input contains stereo data
    if (inputData.size() >= 2) {
        initializeBinauralProcessing(inputData);
    }
    
    // Initialize neural oscillators
    initializeNeuralOscillators(sampleRate);
    
    // Process audio through neural oscillators
    processThroughOscillators(inputData, numSamples, sampleRate);
    
    // Extract spectral features using neural computation
    extractSpectralFeatures(inputData, numSamples, sampleRate);
    
    // Extract temporal features using echoic memory
    extractTemporalFeatures(inputData, numSamples, sampleRate);
    
    // Perform binaural processing and sound localization
    performBinauralProcessing(inputData, numSamples, sampleRate);
    
    // Pattern recognition
    recognizeAuditoryPatterns(inputData, numSamples, sampleRate);
    
    // Update echoic memory
    updateEchoicMemory(inputData, numSamples, sampleRate);
    
    // Integrate with working memory
    integrateWithWorkingMemory();
    
    // Apply attention and auditory gating
    applyAttentionAndGating();
    
    // Cross-modal integration with visual
    crossModalIntegration();
    
    // Integrate with motor planning
    integrateWithMotorPlanning();
    
    // Update episodic memory
    updateEpisodicMemory(input);
    
    // Store features for output
    pImpl->features = extractSummaryFeatures();
    pImpl->featureDimensions = pImpl->features.size();
}

void AudioProcessor::initializeBinauralProcessing(const std::vector<float>& data) {
    // Initialize binaural processing for sound localization
    // Analyze interaural level and time differences
    
    size_t numChannels = 2;
    if (data.size() < numChannels) return;
    
    // Simple stereo to mono processing for initialization
    float leftSum = 0.0f, rightSum = 0.0f;
    size_t samplesPerChannel = data.size() / numChannels;
    
    for (size_t i = 0; i < samplesPerChannel; ++i) {
        leftSum += data[i * numChannels];
        rightSum += data[i * numChannels + 1];
    }
    
    float leftAvg = leftSum / static_cast<float>(samplesPerChannel);
    float rightAvg = rightSum / static_cast<float>(samplesPerChannel);
    
    // Compute interaural level difference (ILD)
    pImpl->binauralProcessor.interauralLevelDifference = 
        20.0f * std::log10(std::abs(rightAvg) / (std::abs(leftAvg) + 0.0001f));
    
    // Simple interaural time difference estimation
    pImpl->binauralProcessor.interauralTimeDifference = 0.0f;
    
    // Estimate sound source location using ILD
    // Positive ILD (right louder) -> source on right side
    // Negative ILD (left louder) -> source on left side
    pImpl->binauralProcessor.soundSourceAzimuth = 
        std::atan2(pImpl->binauralProcessor.interauralLevelDifference, 10.0f) * 180.0f / M_PI;
    
    pImpl->binauralProcessor.confidence = 
        std::min(1.0f, std::abs(pImpl->binauralProcessor.interauralLevelDifference) / 20.0f);
    
    // Create binaural neurons
    pImpl->binauralProcessor.leftId = NeuronId(40000);
    pImpl->binauralProcessor.rightId = NeuronId(40001);
}

void AudioProcessor::initializeNeuralOscillators(float sampleRate) {
    // Initialize biologically plausible neural oscillators
    // Based on Hodgkin-Huxley models for neural oscillations
    
    pImpl->oscillators.clear();
    
    // Create oscillators for different frequency bands
    std::vector<float> centerFrequencies = {50.0f, 100.0f, 200.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f, 8000.0f};
    
    for (size_t i = 0; i < std::min(centerFrequencies.size(), static_cast<size_t>(8)); ++i) {
        NeuralOscillator& osc = pImpl->oscillators.emplace_back();
        osc.id = NeuronId(30000 + i);
        osc.centerFrequency = centerFrequencies[i];
        osc.bandwidth = osc.centerFrequency * 0.1f;  // 10% bandwidth
        osc.phase = pImpl->uniformDist(pImpl->rng) * 2.0f * M_PI;
        osc.amplitude = pImpl->uniformDist(pImpl->rng) * 0.5f + 0.5f;
        osc.preferredModulation = pImpl->uniformDist(pImpl->rng);
        osc.coherence = 0.0f;
        osc.activation = 0.0f;
    }
}

void AudioProcessor::processThroughOscillators(const std::vector<float>& data, 
                                              size_t numSamples, float sampleRate) {
    // Process audio through neural oscillators
    // Simulates frequency-tuned neural population responses
    
    if (pImpl->oscillators.empty()) return;
    
    float dt = 1.0f / sampleRate;
    
    for (auto& osc : pImpl->oscillators) {
        // Simulate neural oscillator dynamics
        // Based on phase oscillator model with noise
        
        // Phase update
        float naturalFrequency = osc.centerFrequency * 2.0f * M_PI;
        osc.phase += naturalFrequency * dt + pImpl->normalDist(pImpl->rng) * 0.1f;
        
        // Amplitude modulation based on input
        // Simplified: average amplitude of input near oscillator frequency
        osc.activation = osc.amplitude * std::sin(osc.phase);
        
        // Add some noise and biological variability
        osc.activation += pImpl->normalDist(pImpl->rng) * 0.1f;
        osc.activation = std::tanh(osc.activation);
        
        // Update coherence with input
        osc.coherence = std::abs(osc.activation);
        
        // Store features for output
        pImpl->features.push_back(osc.centerFrequency);
        pImpl->features.push_back(osc.activation);
        pImpl->features.push_back(osc.coherence);
    }
}

void AudioProcessor::extractSpectralFeatures(const std::vector<float>& data, 
                                            size_t numSamples, float sampleRate) {
    // Extract spectral features using neural computation
    // Implemented as neural population activity patterns
    
    pImpl->spectralFeatures.clear();
    
    if (numSamples < 2) return;
    
    // Compute power spectral density using FFT (neural approximation)
    size_t fftSize = 1024;
    if (numSamples < fftSize) fftSize = numSamples;
    
    std::vector<float> spectrum(fftSize, 0.0f);
    
    // Simple neural spectral estimation
    // Use neural population tuning for frequency bands
    std::vector<float> frequencyBands = {100.0f, 200.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f, 8000.0f, 16000.0f};
    
    for (size_t band = 0; band < frequencyBands.size() && band < pImpl->oscillators.size(); ++band) {
        float freq = frequencyBands[band];
        float binWidth = sampleRate / static_cast<float>(fftSize);
        size_t centerBin = static_cast<size_t>(freq / binWidth);
        
        if (centerBin < fftSize / 2) {
            // Neural computation of spectral power
            float bandPower = 0.0f;
            size_t halfWidth = static_cast<size_t>(pImpl->oscillators[band].bandwidth / binWidth);
            
            for (size_t i = std::max(0, static_cast<int>(centerBin) - halfWidth); 
                 i < std::min(fftSize, centerBin + halfWidth + 1); ++i) {
                bandPower += data[i] * data[i];
            }
            
            // Normalize by number of samples in band
            bandPower /= (2 * halfWidth + 1);
            
            // Apply neural gain control
            float neuralGain = pImpl->oscillators[band].activation * 2.0f + 0.1f;
            float feature = std::log10(bandPower * neuralGain + 0.0001f) * 10.0f;  // Convert to dB
            
            pImpl->spectralFeatures.push_back(feature);
        }
    }
}

void AudioProcessor::extractTemporalFeatures(const std::vector<float>& data, 
                                            size_t numSamples, float sampleRate) {
    // Extract temporal features using neural dynamics
    // Implemented using echoic memory traces
    
    pImpl->temporalFeatures.clear();
    pImpl->auditoryTemporalFeatures.clear();
    
    if (numSamples < 10) return;
    
    // Compute temporal envelopes and dynamics
    std::vector<float> envelope(numSamples, 0.0f);
    std::vector<float> derivative(numSamples, 0.0f);
    
    // Simple neural-based envelope detection
    for (size_t i = 1; i < numSamples - 1; ++i) {
        // Compute local amplitude using neural-like smoothing
        float windowSize = 5.0f;
        float sum = 0.0f;
        float count = 0.0f;
        
        for (int j = -static_cast<int>(windowSize); j <= static_cast<int>(windowSize); ++j) {
            int idx = i + j;
            if (idx >= 0 && idx < static_cast<int>(numSamples)) {
                float mag = std::abs(data[idx]);
                sum += mag;
                count += 1.0f;
            }
        }
        
        envelope[i] = sum / count;
        
        // Compute derivative using neural-like differentiation
        derivative[i] = envelope[i] - envelope[std::max(0, static_cast<int>(i-1))];
    }
    
    // Extract temporal features
    float meanEnvelope = std::accumulate(envelope.begin(), envelope.end(), 0.0f) / envelope.size();
    float envelopeVariance = 0.0f;
    for (float val : envelope) {
        envelopeVariance += (val - meanEnvelope) * (val - meanEnvelope);
    }
    envelopeVariance /= envelope.size();
    
    // Temporal modulation rate
    float modulationRate = 0.0f;
    float maxDerivative = 0.0f;
    for (float val : derivative) {
        modulationRate += std::abs(val);
        maxDerivative = std::max(maxDerivative, std::abs(val));
    }
    modulationRate /= derivative.size();
    
    pImpl->temporalFeatures.push_back(meanEnvelope);
    pImpl->temporalFeatures.push_back(std::sqrt(envelopeVariance));  // Envelope stability
    pImpl->temporalFeatures.push_back(modulationRate);
    pImpl->temporalFeatures.push_back(maxDerivative);
    
    // Store for auditory working memory
    pImpl->auditoryTemporalFeatures = envelope;
}

void AudioProcessor::performBinauralProcessing(const std::vector<float>& data, 
                                              size_t numSamples, float sampleRate) {
    // Perform binaural processing and sound localization
    
    pImpl->binauralFeatures.clear();
    
    if (pImpl->binauralProcessor.leftId.value == 0) return;
    
    // Update binaural estimates based on current input
    float currentILD = 0.0f;
    float currentITD = 0.0f;
    
    size_t numChannels = 2;
    if (data.size() >= numChannels) {
        size_t samplesPerChannel = numSamples / numChannels;
        
        // Compute running ILD and ITD
        float leftSum = 0.0f, rightSum = 0.0f;
        for (size_t i = 0; i < samplesPerChannel; ++i) {
            leftSum += data[i * numChannels];
            rightSum += data[i * numChannels + 1];
        }
        
        float leftAvg = leftSum / static_cast<float>(samplesPerChannel);
        float rightAvg = rightSum / static_cast<float>(samplesPerChannel);
        
        currentILD = 20.0f * std::log10(std::abs(rightAvg) / (std::abs(leftAvg) + 0.0001f));
        currentITD = (rightAvg - leftAvg) * 1000.0f;  // Approximate ITD in ms
    }
    
    // Update binaural processor state
    pImpl->binauralProcessor.interauralLevelDifference = currentILD;
    pImpl->binauralProcessor.interauralTimeDifference = currentITD;
    
    // Compute sound source location using empirical binaural cues
    // This is a simplified model - real implementation would use more sophisticated algorithms
    float azimuth = 0.0f, elevation = 0.0f;
    
    // Estimate azimuth from ILD (interaural level difference)
    // This is a simplified lookup table approximation
    if (std::abs(currentILD) < 15.0f) {
        azimuth = currentILD * 15.0f;  // Linear mapping for small ILD
    } else {
        azimuth = std::copysign(45.0f, currentILD);  // Capped at +/- 45 degrees
    }
    
    // Estimate elevation from spectral cues (placeholder)
    elevation = 0.0f;  // Would use spectral shape analysis
    
    // Estimate distance from overall level
    float overallLevel = 0.0f;
    for (float val : data) {
        overallLevel += std::abs(val);
    }
    overallLevel /= data.size();
    float distance = std::max(0.5f, 10.0f - overallLevel * 10.0f);  // Inverse relationship
    
    pImpl->binauralProcessor.soundSourceAzimuth = azimuth;
    pImpl->binauralProcessor.soundSourceElevation = elevation;
    pImpl->binauralProcessor.confidence = std::min(1.0f, std::abs(currentILD) / 20.0f);
    
    // Store binaural features
    pImpl->binauralFeatures.push_back(azimuth);
    pImpl->binauralFeatures.push_back(elevation);
    pImpl->binauralFeatures.push_back(distance);
    pImpl->binauralFeatures.push_back(pImpl->binauralProcessor.confidence);
}

void AudioProcessor::recognizeAuditoryPatterns(const std::vector<float>& data, 
                                              size_t numSamples, float sampleRate) {
    // Pattern recognition for auditory events
    // Based on neural population responses
    
    pImpl->patternPopulations.clear();
    
    if (numSamples < 100) return;
    
    // Create pattern populations for different auditory patterns
    std::vector<AuditoryPattern> patternTypes = {AuditoryPattern::Tone, AuditoryPattern::Noise, 
                                                  AuditoryPattern::ComplexTone, AuditoryPattern::Music};
    
    for (size_t i = 0; i < patternTypes.size() && i < 4; ++i) {
        PatternPopulation pop;
        pop.patternType = patternTypes[i];
        pop.activationThreshold = 0.5f;
        pop.tuningWidth = 0.2f;
        
        // Compute pattern features
        std::vector<float> features(4, 0.0f);
        
        // Feature 1: Tonality (spectral peaks)
        float tonality = 0.0f;
        size_t peakCount = 0;
        for (size_t j = 0; j < numSamples - 10; ++j) {
            bool isPeak = data[j] > data[j-1] && data[j] > data[j+1];
            if (isPeak) peakCount++;
        }
        tonality = static_cast<float>(peakCount) / static_cast<float>(numSamples - 10);
        features[0] = tonality;
        
        // Feature 2: Entropy (complexity)
        std::vector<float> histogram(10, 0.0f);
        for (size_t j = 0; j < numSamples; ++j) {
            int bin = static_cast<int>(std::abs(data[j]) * 10.0f) % 10;
            histogram[bin] += 1.0f;
        }
        
        float entropy = 0.0f;
        for (float count : histogram) {
            if (count > 0) {
                float p = count / numSamples;
                entropy -= p * std::log10(p + 0.0001f);
            }
        }
        features[1] = entropy / 4.0f;  // Normalize
        
        // Feature 3: Amplitude envelope shape
        float meanAmplitude = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
        float amplitudeVariation = 0.0f;
        for (float val : data) {
            amplitudeVariation += (val - meanAmplitude) * (val - meanAmplitude);
        }
        amplitudeVariation /= data.size();
        features[2] = std::sqrt(amplitudeVariation);
        
        // Feature 4: Temporal regularity
        float temporalCoherence = 0.0f;
        size_t zeroCrossings = 0;
        for (size_t j = 1; j < numSamples; ++j) {
            if ((data[j-1] > 0 && data[j] < 0) || (data[j-1] < 0 && data[j] > 0)) {
                zeroCrossings++;
            }
        }
        temporalCoherence = static_cast<float>(zeroCrossings) / static_cast<float>(numSamples - 1);
        features[3] = temporalCoherence;
        
        pop.patternFeatures = features;
        
        pImpl->patternPopulations.push_back(pop);
    }
}

void AudioProcessor::updateEchoicMemory(const std::vector<float>& data, 
                                       size_t numSamples, float sampleRate) {
    // Update echoic memory with new auditory traces
    
    if (echoicMemory.size() >= 10) {
        // Remove oldest trace
        echoicMemory.erase(echoicMemory.begin());
    }
    
    EchoicMemoryTrace& trace = echoicMemory.emplace_back();
    trace.waveform = std::vector<float>(data.begin(), data.end());
    trace.startTime = 0.0f;  // In real implementation, track actual time
    trace.duration = numSamples / sampleRate;
    
    // Compute salience based on temporal envelope and spectral content
    trace.salience = 0.0f;
    
    // Simple salience computation
    float maxAmplitude = 0.0f;
    for (float val : data) {
        maxAmplitude = std::max(maxAmplitude, std::abs(val));
    }
    
    trace.salience = maxAmplitude * 2.0f;
    
    // Encoding strength based on novelty
    trace.encodingStrength = 1.0f - computeAuditoryNovelty(data);
    
    // Decay and consolidation flags
    trace.consolidated = false;
}

void AudioProcessor::integrateWithWorkingMemory() {
    // Integrate auditory features with working memory
    
    if (!pImpl->brain || !pImpl->brain->getAdvancedWorkingMemory()) {
        return;
    }
    
    // Store auditory features in working memory
    for (size_t i = 0; i < std::min(pImpl->spectralFeatures.size(), static_cast<size_t>(50)); ++i) {
        float featureValue = pImpl->spectralFeatures[i];
        
        // Store with decay mechanism
        pImpl->brain->getAdvancedWorkingMemory()->store(
            NeuronId(40000 + i),
            featureValue,
            pImpl->brain->getTotalNeuronCount()
        );
        
        // Also update auditory working memory vector
        if (i < pImpl->auditoryWorkingMemory.size()) {
            pImpl->auditoryWorkingMemory[i] = featureValue * (1.0f - pImpl->brain->getDecayRate());
        } else {
            pImpl->auditoryWorkingMemory.push_back(featureValue);
        }
    }
}

void AudioProcessor::applyAttentionAndGating() {
    // Apply attention and auditory gating
    
    pImpl->auditoryGatingSignals.clear();
    pImpl->auditoryAttentionSignals.clear();
    
    // Compute attention based on spectral and temporal features
    for (size_t i = 0; i < pImpl->spectralFeatures.size(); ++i) {
        float spectralFeature = pImpl->spectralFeatures[i];
        
        // Saliency computation
        float saliency = std::abs(spectralFeature);
        
        // Gating based on binaural confidence
        float binauralConfidence = (i < pImpl->binauralFeatures.size()) ? 
            pImpl->binauralFeatures[3] : 0.5f;  // Confidence is last binaural feature
        
        float gating = binauralConfidence * (1.0f - saliency * 0.5f);
        
        pImpl->auditoryGatingSignals.push_back(gating);
        pImpl->auditoryAttentionSignals.push_back(saliency);
    }
}

void AudioProcessor::crossModalIntegration() {
    // Cross-modal integration with visual modality
    
    pImpl->crossModalIntegrationWeights.clear();
    
    if (!pImpl->brain) return;
    
    // Check for visual features in working memory
    if (pImpl->brain->getAdvancedWorkingMemory()) {
        // Get some visual features (simplified)
        float visualFeature = 0.0f;
        if (pImpl->brain->getAdvancedWorkingMemory()->contains(NeuronId(30000))) {
            visualFeature = pImpl->brain->getAdvancedWorkingMemory()->retrieve(NeuronId(30000));
        }
        
        // Compute cross-modal weights
        pImpl->crossModalIntegrationWeights.resize(4, 0.0f);
        pImpl->crossModalIntegrationWeights[0] = 0.6f;  // Visual weight
        pImpl->crossModalIntegrationWeights[1] = 0.4f;  // Auditory weight
        
        // Apply cross-modal enhancement if both modalities are active
        if (visualFeature > 0.5f && !pImpl->spectralFeatures.empty()) {
            // Enhance auditory processing based on visual input
            for (size_t i = 0; i < pImpl->spectralFeatures.size(); ++i) {
                pImpl->spectralFeatures[i] *= 1.2f;
            }
        }
    }
}

void AudioProcessor::integrateWithMotorPlanning() {
    // Integrate with motor planning
    
    pImpl->motorPlanningSignals.clear();
    
    if (!pImpl->brain) return;
    
    // Compute audio-motor integration signals
    float motorRelevance = 0.0f;
    
    // Based on temporal modulation and binaural processing
    if (!pImpl->temporalFeatures.empty()) {
        motorRelevance += pImpl->temporalFeatures[2];  // Modulation rate
    }
    
    if (!pImpl->binauralFeatures.empty()) {
        motorRelevance += pImpl->binauralFeatures[0];  // Azimuth
    }
    
    pImpl->motorPlanningSignals.push_back(motorRelevance);
    
    // Update concept formation
    if (pImpl->brain->getConceptFormation()) {
        pImpl->brain->getConceptFormation()->updateConcept(
            "audio_input",
            std::vector<float>(pImpl->spectralFeatures.begin(), pImpl->spectralFeatures.end())
        );
    }
}

void AudioProcessor::updateEpisodicMemory(const Audio& input) {
    // Update episodic memory with auditory experience
    
    if (!pImpl->brain || !pImpl->brain->getAdvancedEpisodicMemory()) return;
    
    EpisodicMemoryItem episode;
    episode.timestamp = pImpl->brain->getTotalNeuronCount();
    episode.sensoryState = pImpl->features;
    
    // Store active neurons
    for (size_t i = 0; i < pImpl->oscillators.size(); ++i) {
        if (pImpl->oscillators[i].id.value > 0) {
            episode.activeNeurons.push_back(pImpl->oscillators[i].id);
            episode.neuronActivations.push_back(pImpl->spectralFeatures[i % pImpl->spectralFeatures.size()]);
        }
    }
    
    // Store in episodic memory
    pImpl->brain->getAdvancedEpisodicMemory()->storeExperience(
        episode.sensoryState,
        episode.activeNeurons,
        episode.neuronActivations,
        ActionType::Wait,
        0.5f,
        0.1f
    );
}

const std::vector<float>& AudioProcessor::extractSummaryFeatures() {
    // Extract summary features for output
    
    pImpl->features.clear();
    
    // Combine all feature types
    for (float feature : pImpl->spectralFeatures) {
        pImpl->features.push_back(feature);
    }
    
    for (float feature : pImpl->temporalFeatures) {
        pImpl->features.push_back(feature);
    }
    
    for (float feature : pImpl->binauralFeatures) {
        pImpl->features.push_back(feature);
    }
    
    for (size_t i = 0; i < pImpl->patternPopulations.size(); ++i) {
        for (float feature : pImpl->patternPopulations[i].patternFeatures) {
            pImpl->features.push_back(feature);
        }
    }
    
    return pImpl->features;
}

float AudioProcessor::computeAuditoryNovelty(const std::vector<float>& data) {
    // Compute novelty of auditory input
    if (data.empty()) return 0.0f;
    
    // Simple novelty estimation based on deviation from mean
    float mean = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
    float variance = 0.0f;
    for (float val : data) {
        variance += (val - mean) * (val - mean);
    }
    variance /= data.size();
    
    return std::sqrt(variance);
}

} // namespace nlm
