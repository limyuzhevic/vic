#include "AudioProcessor.hpp"

namespace nlm {

struct AudioProcessor::Impl {
    // AudioFeatures storage
    AudioFeatures features;
    std::vector<float> processedFeatures;
    
    // Neural components
    std::unique_ptr<NeuralRegion> cochlearRegion;
    std::unique_ptr<SpikeSystem> spikeSystem;
    
    // Biological parameters
    float frequencyResolution;
    float temporalWindow;
    float maxSpikeRate;
    float cochlearSensitivity;
    
    // Pre-computed filter banks for frequency analysis
    std::vector<std::vector<float>> filterBanks;
    
    Impl(size_t numSamples) 
        : features(numSamples)
        , frequencyResolution(0.5f)
        , temporalWindow(0.1f)
        , maxSpikeRate(100.0f)
        , cochlearSensitivity(1.0f) {
        
        // Initialize gammatone filter banks for frequency analysis
        int numFilters = 20;
        filterBanks.resize(numFilters, std::vector<float>(256, 0.0f));
        for (int i = 0; i < numFilters; ++i) {
            float centerFreq = 100.0f + i * 500.0f;  // 100Hz to 10.5kHz
            for (int j = 0; j < 256; ++j) {
                float freq = (float)j * 2000.0f / 256.0f;  // 0 to 2000Hz
                // Simple gammatone-like filter response
                float response = std::exp(-std::abs(freq - centerFreq) / centerFreq);
                filterBanks[i][j] = response;
            }
        }
    }
};

AudioProcessor::AudioProcessor() : pImpl(new Impl(1024)) {
    initializeNeuralComponents(1024);
}

AudioProcessor::~AudioProcessor() = default;

void AudioProcessor::initializeNeuralComponents(size_t numSamples) {
    // Create cochlear region with auditory nerve fibers
    pImpl->cochlearRegion = std::make_unique<NeuralRegion>(RegionId(2));
    
    // Add auditory nerve neuron populations
    // Low frequency range (0-2kHz)
    PopulationId lowFreqPop = pImpl->cochlearRegion->addPopulation(numSamples / 4, NeuronType::Sensory);
    // Mid frequency range (2-8kHz)
    pImpl->cochlearRegion->addPopulation(numSamples / 4, NeuronType::Sensory);
    // High frequency range (8-20kHz)
    pImpl->cochlearRegion->addPopulation(numSamples / 4, NeuronType::Sensory);
    // Temporal processing neurons
    pImpl->cochlearRegion->addPopulation(numSamples / 4, NeuronType::Internal);
    
    // Initialize spike system for auditory event processing
    pImpl->spikeSystem = std::make_unique<SpikeSystem>();
    
    // Register spike handler for auditory activity recording
    pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
        if (event.populationId >= PopulationId(3) && event.populationId < PopulationId(7)) {
            // Auditory nerve - record spike timing and rate
            size_t idx = event.populationId.value - 3;
            if (idx < pImpl->features.spikeRates.size()) {
                pImpl->features.spikeRates[idx] += 1.0f;
            }
        }
    });
}

void AudioProcessor::process(const Audio& input) {
    reset();
    
    const auto& inputData = input.getData();
    size_t numSamples = input.getNumSamples();
    
    // Process through cochlear neural circuit
    processThroughCochlea(inputData);
    
    // Extract frequency features using filter bank analysis
    extractFrequencyFeatures(inputData);
    
    // Compute temporal features for rhythm and onset detection
    computeTemporalFeatures(inputData);
    
    // Convert features to spike patterns
    convertToSpikes(inputData);
    
    // Apply lateral inhibition for contrast enhancement
    applyInhibition(pImpl->features.spectralPeaks, 0.3f);
    applyInhibition(pImpl->features.harmonicFeatures, 0.3f);
    
    // Combine all features into final representation
    combineFeatures(pImpl->features);
    
    // Generate output feature vector
    generateOutputFeatures();
}

void AudioProcessor::processThroughCochlea(const std::vector<float>& inputData) {
    size_t numSamples = inputData.size();
    
    // Simulate cochlear processing through spiking auditory nerve fibers
    
    // Create auditory nerve population
    auto neurons = pImpl->cochlearRegion->getAllNeurons();
    
    // Apply cochlear filtering (frequency decomposition)
    std::vector<float> filteredSignal(numSamples, 0.0f);
    for (size_t i = 0; i < numSamples; ++i) {
        // Apply different filter responses based on neuron type
        for (size_t filterIdx = 0; filterIdx < pImpl->filterBanks.size(); ++filterIdx) {
            if (filterIdx < pImpl->filterBanks.size()) {
                float freqBin = i % 256;  // Frequency bin (0-255)
                filteredSignal[i] += inputData[i] * pImpl->filterBanks[filterIdx][freqBin];
            }
        }
    }
    
    // Inject filtered signal as current into auditory neurons
    float maxInput = *std::max_element(filteredSignal.begin(), filteredSignal.end());
    float minInput = *std::min_element(filteredSignal.begin(), filteredSignal.end());
    float inputRange = maxInput - minInput;
    
    // Distribute input to auditory neuron populations
    size_t neuronCount = neurons.size();
    if (neuronCount > 0) {
        for (size_t i = 0; i < filteredSignal.size() && i < neuronCount; ++i) {
            float normalizedInput = (filteredSignal[i] - minInput) / (inputRange + 0.001f);
            float current = normalizedInput * pImpl->cochlearSensitivity * 10.0f;
            neurons[i]->injectCurrent(current);
        }
    }
}

void AudioProcessor::extractFrequencyFeatures(const std::vector<float>& inputData) {
    size_t numSamples = inputData.size();
    
    // Compute frequency domain features using filter bank analysis
    for (size_t sampleIdx = 0; sampleIdx < numSamples; ++sampleIdx) {
        float sample = inputData[sampleIdx];
        
        // Apply each filter bank to extract frequency components
        for (size_t filterIdx = 0; filterIdx < pImpl->filterBanks.size(); ++filterIdx) {
            if (filterIdx < pImpl->filterBanks.size()) {
                size_t freqBin = sampleIdx % 256;
                float filterResponse = pImpl->filterBanks[filterIdx][freqBin];
                float contribution = sample * filterResponse;
                
                size_t featureIdx = sampleIdx * pImpl->filterBanks.size() + filterIdx;
                if (featureIdx < pImpl->features.spectralPeaks.size()) {
                    pImpl->features.spectralPeaks[featureIdx] += contribution;
                }
                
                // Update spectral centroid
                if (featureIdx < pImpl->features.spectralCentroid.size()) {
                    pImpl->features.spectralCentroid[featureIdx] += freqBin * contribution;
                }
            }
        }
        
        // Compute spectral spread
        if (sampleIdx < pImpl->features.spectralSpread.size()) {
            float meanFreq = 0.0f, sumWeights = 0.0f;
            for (size_t filterIdx = 0; filterIdx < pImpl->filterBanks.size() && filterIdx < 20; ++filterIdx) {
                size_t featIdx = sampleIdx * pImpl->filterBanks.size() + filterIdx;
                if (featIdx < pImpl->features.spectralPeaks.size() && 
                    pImpl->features.spectralPeaks[featIdx] > 0.0f) {
                    meanFreq += filterIdx * pImpl->features.spectralPeaks[featIdx];
                    sumWeights += pImpl->features.spectralPeaks[featIdx];
                }
            }
            
            if (sumWeights > 0.0f) {
                meanFreq /= sumWeights;
                float spread = 0.0f;
                for (size_t filterIdx = 0; filterIdx < pImpl->filterBanks.size() && filterIdx < 20; ++filterIdx) {
                    size_t featIdx = sampleIdx * pImpl->filterBanks.size() + filterIdx;
                    if (featIdx < pImpl->features.spectralPeaks.size()) {
                        spread += pImpl->features.spectralPeaks[featIdx] * (filterIdx - meanFreq) * (filterIdx - meanFreq);
                    }
                }
                pImpl->features.spectralSpread[sampleIdx] = std::sqrt(spread);
            }
        }
    }
}

void AudioProcessor::computeTemporalFeatures(const std::vector<float>& inputData) {
    size_t numSamples = inputData.size();
    
    // Compute temporal features for rhythm and onset detection
    for (size_t i = 1; i < numSamples; ++i) {
        // Onset detection using zero-crossing and amplitude changes
        float diff = std::abs(inputData[i] - inputData[i-1]);
        
        if (diff > 0.5f) {  // Significant change indicates onset
            float onsetTime = (float)i / (float)inputData.size();
            size_t featureIdx = i % pImpl->features.onsetTimes.size();
            if (featureIdx < pImpl->features.onsetTimes.size()) {
                pImpl->features.onsetTimes[featureIdx] = onsetTime;
            }
        }
        
        // Rhythm features based on periodicity
        if (i >= 10 && i < numSamples - 10) {
            float windowAvg = 0.0f;
            for (size_t j = i - 10; j <= i + 10; ++j) {
                windowAvg += inputData[j];
            }
            windowAvg /= 21.0f;
            
            // Attack/decay detection
            float prev = (i > 10) ? inputData[i-5] : 0.0f;
            float next = (i < numSamples - 5) ? inputData[i+5] : 0.0f;
            
            size_t featureIdx = i % pImpl->features.attackDecayFeatures.size();
            if (featureIdx < pImpl->features.attackDecayFeatures.size()) {
                float attack = std::max(0.0f, next - windowAvg);
                float decay = std::max(0.0f, prev - windowAvg);
                pImpl->features.attackDecayFeatures[featureIdx] = (attack + decay) * 0.5f;
            }
        }
        
        // Rhythm regularity
        if (i >= 20) {
            float periodSum = 0.0f;
            size_t periodCount = 0;
            
            // Simple periodicity detection
            for (size_t j = 0; j < 10 && i - j * 10 > 0; ++j) {
                size_t lag = j * 10;
                float correlation = 0.0f;
                for (size_t k = 0; k < 10 && i - k < numSamples && i - lag - k < numSamples; ++k) {
                    correlation += inputData[i - k] * inputData[i - lag - k];
                }
                if (correlation > 0.5f) {
                    periodSum += lag;
                    periodCount++;
                }
            }
            
            if (periodCount > 0) {
                float avgPeriod = periodSum / (float)periodCount;
                size_t featureIdx = i % pImpl->features.rhythmFeatures.size();
                if (featureIdx < pImpl->features.rhythmFeatures.size()) {
                    pImpl->features.rhythmFeatures[featureIdx] = avgPeriod / 100.0f;  // Normalize
                }
            }
        }
    }
}

void AudioProcessor::convertToSpikes(const std::vector<float>& features) {
    // Generate spike patterns based on audio features
    
    auto neurons = pImpl->cochlearRegion->getAllNeurons();
    
    // Generate spike rates based on feature intensities
    for (size_t i = 0; i < neurons.size() && i < pImpl->features.spikeRates.size(); ++i) {
        float potential = neurons[i]->getState().membranePotential;
        float normalized = (potential + 70.0f) / 125.0f;  // Normalize to [0,1]
        normalized = std::min(1.0f, normalized);
        
        // Generate spike rate based on membrane potential
        float spikeRate = normalized * pImpl->maxSpikeRate;
        pImpl->features.spikeRates[i] = spikeRate;
    }
}

void AudioProcessor::applyInhibition(std::vector<float>& features, float strength) {
    // Apply lateral inhibition for feature contrast enhancement
    size_t size = features.size();
    
    for (size_t i = 0; i < size; ++i) {
        // Center-surround inhibition
        float center = features[i];
        
        float surround = 0.0f;
        size_t count = 0;
        
        if (i >= 1) surround += features[i - 1]; count++;
        if (i < size - 1) surround += features[i + 1]; count++;
        
        surround /= static_cast<float>(count);
        
        features[i] = center - strength * surround;
    }
}

void AudioProcessor::combineFeatures(AudioFeatures& output) {
    // Combine all feature maps into coherent representation
    size_t totalFeatures = output.spectralPeaks.size();
    
    // Combine frequency and temporal features
    for (size_t i = 0; i < totalFeatures; ++i) {
        // Frequency component
        float freqComponent = (output.spectralPeaks[i] + output.spectralCentroid[i]) * 0.5f;
        
        // Temporal component
        float temporalComponent = (output.onsetTimes[i] + output.rhythmFeatures[i] + 
                                 output.attackDecayFeatures[i]) * 0.333f;
        
        // Harmonic/percussive separation
        float harmonicComponent = output.harmonicFeatures[i];
        float percussiveComponent = output.percussiveFeatures[i];
        
        // Combine with spike rate timing information
        float temporalInfo = output.spikeRates[i] / pImpl->maxSpikeRate;
        
        // Final weighted combination
        output.spectralPeaks[i] = 
            0.4f * freqComponent + 
            0.2f * temporalComponent + 
            0.2f * harmonicComponent + 
            0.2f * percussiveComponent +
            0.0f * temporalInfo;  // Temporal info already included in onsetTimes
    }
}

void AudioProcessor::generateOutputFeatures() {
    // Generate final feature vector for agent use
    pImpl->processedFeatures.clear();
    pImpl->processedFeatures.reserve(pImpl->features.spectralPeaks.size() * 2);
    
    // Include key features for agent processing
    for (size_t i = 0; i < pImpl->features.spectralPeaks.size(); ++i) {
        pImpl->processedFeatures.push_back(pImpl->features.spectralPeaks[i]);
    }
    
    // Add temporal features (normalized)
    for (size_t i = 0; i < pImpl->features.onsetTimes.size(); ++i) {
        pImpl->processedFeatures.push_back(
            pImpl->features.onsetTimes[i] * 10.0f  // Scale for better usage
        );
    }
    
    // Add rhythm features for temporal pattern recognition
    for (size_t i = 0; i < pImpl->features.rhythmFeatures.size(); ++i) {
        pImpl->processedFeatures.push_back(
            pImpl->features.rhythmFeatures[i]
        );
    }
}

void AudioProcessor::reset() {
    // Reset all feature maps
    for (auto& feature : pImpl->features.spectralPeaks) feature = 0.0f;
    for (auto& feature : pImpl->features.spectralCentroid) feature = 0.0f;
    for (auto& feature : pImpl->features.spectralSpread) feature = 0.0f;
    for (auto& feature : pImpl->features.onsetTimes) feature = 0.0f;
    for (auto& feature : pImpl->features.rhythmFeatures) feature = 0.0f;
    for (auto& feature : pImpl->features.attackDecayFeatures) feature = 0.0f;
    for (auto& feature : pImpl->features.spectrogramFeatures) feature = 0.0f;
    for (auto& feature : pImpl->features.harmonicFeatures) feature = 0.0f;
    for (auto& feature : pImpl->features.percussiveFeatures) feature = 0.0f;
    for (auto& feature : pImpl->features.spikeRates) feature = 0.0f;
    
    pImpl->processedFeatures.clear();
    
    // Reset neural components
    if (pImpl->cochlearRegion) pImpl->cochlearRegion->reset();
    if (pImpl->spikeSystem) pImpl->spikeSystem->reset();
}

void AudioProcessor::setParameters(float frequencyResolution, float temporalWindow, 
                                   float maxSpikeRate, float cochlearSensitivity) {
    pImpl->frequencyResolution = frequencyResolution;
    pImpl->temporalWindow = temporalWindow;
    pImpl->maxSpikeRate = maxSpikeRate;
    pImpl->cochlearSensitivity = cochlearSensitivity;
}

const std::vector<float>& AudioProcessor::getFeatures() const {
    return pImpl->processedFeatures;
}

size_t AudioProcessor::getFeatureDimensions() const {
    return pImpl->processedFeatures.size();
}

const std::vector<float>& AudioProcessor::getFrequencyFeatures() const {
    // Return combined frequency features
    static std::vector<float> combinedFreq;
    combinedFreq.clear();
    combinedFreq.reserve(pImpl->features.spectralPeaks.size() + 
                        pImpl->features.spectralCentroid.size() +
                        pImpl->features.spectralSpread.size());
    
    combinedFreq.insert(combinedFreq.end(),
                       pImpl->features.spectralPeaks.begin(),
                       pImpl->features.spectralPeaks.end());
    combinedFreq.insert(combinedFreq.end(),
                       pImpl->features.spectralCentroid.begin(),
                       pImpl->features.spectralCentroid.end());
    combinedFreq.insert(combinedFreq.end(),
                       pImpl->features.spectralSpread.begin(),
                       pImpl->features.spectralSpread.end());
    
    return combinedFreq;
}

const std::vector<float>& AudioProcessor::getTemporalFeatures() const {
    // Return combined temporal features
    static std::vector<float> combinedTemp;
    combinedTemp.clear();
    combinedTemp.reserve(pImpl->features.onsetTimes.size() + 
                        pImpl->features.rhythmFeatures.size() +
                        pImpl->features.attackDecayFeatures.size());
    
    combinedTemp.insert(combinedTemp.end(),
                       pImpl->features.onsetTimes.begin(),
                       pImpl->features.onsetTimes.end());
    combinedTemp.insert(combinedTemp.end(),
                       pImpl->features.rhythmFeatures.begin(),
                       pImpl->features.rhythmFeatures.end());
    combinedTemp.insert(combinedTemp.end(),
                       pImpl->features.attackDecayFeatures.begin(),
                       pImpl->features.attackDecayFeatures.end());
    
    return combinedTemp;
}

const AudioFeatures& AudioProcessor::getInternalFeatures() const {
    return pImpl->features;
}

} // namespace nlm