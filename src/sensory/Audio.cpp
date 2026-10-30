#include "Audio.hpp"
#include "core/Config/Config.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <complex>

namespace nlm {

struct AudioProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    bool isInitialized;
    std::shared_ptr<Config> config;
    
    // Neural components for audio processing
    std::vector<float> spectralRepresentations;
    std::vector<float> neuralResponses;
    std::vector<float> synapticWeights;
    std::vector<float> temporalFilters;
    
    Impl() : featureDimensions(0), isInitialized(false) {
        spectralRepresentations.reserve(1024); // Typical STFT size
        neuralResponses.reserve(1024);
        synapticWeights.reserve(1024);
        temporalFilters.reserve(256);
    }
};

AudioProcessor::AudioProcessor() : pImpl(new Impl) {
    // Initialize with default configuration
    pImpl->config = std::make_shared<Config>();
    pImpl->config->addConfig("audio_feature_dimensions", 512, "Audio feature extraction dimensions", "int", true);
    pImpl->config->addConfig("audio_sample_rate", 44100, "Audio sample rate for processing", "int", true);
    pImpl->config->addConfig("audio_filter_length", 1024, "Length of neural temporal filters", "int", true);
    pImpl->config->addConfig("audio_connection_strength", 0.6, "Synaptic connection strength for audio", "double", true);
    pImpl->config->addConfig("audio_frequency_bands", 32, "Number of frequency bands for analysis", "int", true);
    pImpl->config->addConfig("audio_processing_enabled", true, "Enable advanced audio processing", "bool", true);
}

AudioProcessor::~AudioProcessor() = default;

void AudioProcessor::initialize(const Brain& brain) {
    if (!pImpl->config->getOr<bool>("audio_processing_enabled", true)) {
        return;
    }
    
    pImpl->featureDimensions = pImpl->config->getOr<size_t>("audio_feature_dimensions", 512);
    size_t filterLength = pImpl->config->getOr<size_t>("audio_filter_length", 1024);
    size_t numFrequencyBands = pImpl->config->getOr<size_t>("audio_frequency_bands", 32);
    
    // Initialize neural representations
    pImpl->spectralRepresentations.resize(pImpl->featureDimensions);
    pImpl->neuralResponses.resize(pImpl->featureDimensions, 0.0f);
    pImpl->temporalFilters.resize(filterLength, 0.0f);
    
    // Initialize synaptic weights for audio processing
    pImpl->synapticWeights.resize(pImpl->featureDimensions);
    float connectionStrength = pImpl->config->getOr<double>("audio_connection_strength", 0.6);
    
    // Create realistic neural connectivity for audio
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> weightDist(-connectionStrength, connectionStrength);
    
    for (size_t i = 0; i < pImpl->featureDimensions; ++i) {
        pImpl->synapticWeights[i] = weightDist(gen);
    }
    
    // Initialize temporal filters with realistic patterns
    float frequency = static_cast<float>(pImpl->config->getOr<size_t>("audio_sample_rate", 44100)) / static_cast<float>(numFrequencyBands);
    for (size_t i = 0; i < filterLength; ++i) {
        // Create oscillatory patterns for temporal processing
        float phase = 2.0f * static_cast<float>(M_PI) * frequency * static_cast<float>(i) / 
                     static_cast<float>(pImpl->config->getOr<size_t>("audio_sample_rate", 44100));
        pImpl->temporalFilters[i] = std::sin(phase) * std::exp(-static_cast<float>(i) / (filterLength / 10.0f));
    }
    
    pImpl->isInitialized = true;
}

void AudioProcessor::process(const Audio& input) {
    if (!pImpl->isInitialized || !pImpl->config->getOr<bool>("audio_processing_enabled", true)) {
        // Fallback to simple pass-through
        pImpl->features = input.getData();
        pImpl->featureDimensions = input.getDimensions();
        return;
    }
    
    // Real audio processing using NLM neural machinery
    processWithNeuralMachinery(input);
}

void AudioProcessor::processWithNeuralMachinery(const Audio& input) {
    const auto& inputData = input.getData();
    size_t inputSize = input.getDimensions();
    
    if (inputSize != pImpl->spectralRepresentations.size()) {
        // Resize if needed
        pImpl->spectralRepresentations.resize(inputSize);
        pImpl->neuralResponses.resize(inputSize, 0.0f);
    }
    
    // Convert audio samples to spectral representation
    convertToSpectralRepresentation(inputData, pImpl->spectralRepresentations);
    
    // Apply neural temporal and spectral filtering
    applyNeuralFilters(pImpl->spectralRepresentations, pImpl->neuralResponses);
    
    // Extract audio features from neural responses
    extractFeaturesFromNeuralResponses(pImpl->neuralResponses, pImpl->features);
    
    pImpl->featureDimensions = pImpl->features.size();
}

void AudioProcessor::convertToSpectralRepresentation(const std::vector<float>& inputData, std::vector<float>& spectralRepresentation) {
    size_t inputSize = inputData.size();
    size_t targetSize = spectralRepresentation.size();
    
    // Ensure sizes match
    if (inputSize != targetSize) {
        spectralRepresentation.resize(inputSize);
    }
    
    // Compute spectral representation using simplified FFT-like processing
    size_t windowSize = std::min<size_t>(1024, inputSize / 4);
    
    for (size_t i = 0; i < inputSize; i += windowSize) {
        size_t currentWindow = std::min(windowSize, inputSize - i);
        
        // Apply window function
        std::vector<float> windowedSignal(currentWindow);
        for (size_t j = 0; j < currentWindow; ++j) {
            float window = 0.5f * (1.0f - std::cos(2.0f * static_cast<float>(M_PI) * j / (currentWindow - 1)));
            windowedSignal[j] = inputData[i + j] * window;
        }
        
        // Compute spectral power in different frequency bands
        for (size_t band = 0; band < std::min<size_t>(32, currentWindow / 8); ++band) {
            size_t start = band * currentWindow / 32;
            size_t end = (band + 1) * currentWindow / 32;
            if (end > currentWindow) end = currentWindow;
            
            if (start < end) {
                float bandPower = 0.0f;
                for (size_t j = start; j < end; ++j) {
                    bandPower += windowedSignal[j] * windowedSignal[j];
                }
                bandPower /= (end - start);
                
                // Map to spectral representation index
                size_t spectralIndex = i + band * 4; // Downsample
                if (spectralIndex < spectralRepresentation.size()) {
                    spectralRepresentation[spectralIndex] = std::log(1.0f + bandPower) * 20.0f; // dB scale
                }
            }
        }
    }
}

void AudioProcessor::applyNeuralFilters(const std::vector<float>& input, std::vector<float>& output) {
    size_t size = input.size();
    size_t filterLength = pImpl->temporalFilters.size();
    
    if (size == 0 || filterLength >= size) {
        output = input;
        return;
    }
    
    // Apply neural temporal filtering
    output.resize(size, 0.0f);
    
    for (size_t i = 0; i < size; ++i) {
        float filteredValue = input[i];
        
        // Apply temporal filtering with neural weights
        float temporalSum = 0.0f;
        size_t filterHalf = filterLength / 2;
        
        for (size_t j = 0; j < filterLength; ++j) {
            size_t inputIndex = i + j - filterHalf;
            if (inputIndex < size) {
                temporalSum += input[inputIndex] * pImpl->temporalFilters[j];
            }
        }
        
        filteredValue += temporalSum * 0.1f; // Temporal integration
        
        // Apply spectral neural weights
        filteredValue *= pImpl->synapticWeights[i % pImpl->synapticWeights.size()];
        
        // Neural activation function
        output[i] = std::tanh(filteredValue);
        
        // Add lateral inhibition for temporal competition
        float lateralInhibition = 0.0f;
        for (size_t j = std::max<size_t>(0, i - 20); 
             j < std::min(size, i + 21); ++j) {
            if (j != i) {
                lateralInhibition += std::abs(output[j]) * 0.005f;
            }
        }
        output[i] *= (1.0f - lateralInhibition);
    }
}

void AudioProcessor::extractFeaturesFromNeuralResponses(const std::vector<float>& neuralResponses, std::vector<float>& features) {
    size_t size = neuralResponses.size();
    
    // Extract comprehensive audio features using neural responses
    features.clear();
    features.reserve(20); // Extract multiple feature categories
    
    // 1. Spectral features
    float spectralCentroid = 0.0f;
    float spectralFlux = 0.0f;
    float spectralRolloff = 0.0f;
    
    float totalPower = 0.0f;
    for (size_t i = 0; i < size; ++i) {
        totalPower += neuralResponses[i] * neuralResponses[i];
    }
    
    if (totalPower > 0.0f) {
        // Spectral centroid (center of mass)
        float weightedSum = 0.0f;
        for (size_t i = 0; i < size; ++i) {
            weightedSum += static_cast<float>(i) * neuralResponses[i] * neuralResponses[i];
        }
        spectralCentroid = weightedSum / totalPower;
        
        // Spectral flux (change in spectrum over time)
        for (size_t i = 1; i < size; ++i) {
            spectralFlux += std::abs(neuralResponses[i] - neuralResponses[i-1]);
        }
        spectralFlux /= (size - 1);
        
        // Spectral rolloff (frequency where 85% of energy is contained)
        float cumulativeEnergy = 0.0f;
        for (size_t i = 0; i < size; ++i) {
            cumulativeEnergy += neuralResponses[i] * neuralResponses[i];
            if (cumulativeEnergy >= 0.85f * totalPower && spectralRolloff == 0.0f) {
                spectralRolloff = static_cast<float>(i);
            }
        }
    }
    
    features.push_back(spectralCentroid);      // Spectral centroid
    features.push_back(spectralFlux);          // Spectral flux
    features.push_back(spectralRolloff);       // Spectral rolloff
    
    // 2. Temporal features
    float zeroCrossingRate = 0.0f;
    float rhythmStrength = 0.0f;
    float temporalCoherence = 0.0f;
    
    for (size_t i = 1; i < size; ++i) {
        if ((neuralResponses[i] >= 0.0f && neuralResponses[i-1] < 0.0f) || 
            (neuralResponses[i] < 0.0f && neuralResponses[i-1] >= 0.0f)) {
            zeroCrossingRate++;
        }
    }
    zeroCrossingRate /= (size - 1);
    features.push_back(zeroCrossingRate);      // Zero crossing rate
    
    // Rhythm detection (periodic patterns)
    float rhythmPower = 0.0f;
    for (size_t period = 1; period < std::min<size_t>(100, size / 4); ++period) {
        float periodPower = 0.0f;
        for (size_t i = 0; i < size - period; i += period) {
            periodPower += std::abs(neuralResponses[i] - neuralResponses[i + period]);
        }
        rhythmPower += periodPower;
    }
    rhythmStrength = rhythmPower / (size / 4.0f);
    features.push_back(rhythmStrength);        // Rhythm strength
    
    // 3. Harmonic features
    float harmonicity = 0.0f;
    float inharmonicity = 0.0f;
    
    for (size_t i = 0; i < size; ++i) {
        // Check harmonic relationships
        for (size_t j = i + 1; j < std::min<size_t>(i + 10, size); ++j) {
            float ratio = neuralResponses[j] / (neuralResponses[i] + 1e-6f);
            if (std::abs(ratio - std::round(ratio)) < 0.1f) {
                harmonicity += std::abs(ratio - std::round(ratio));
            } else {
                inharmonicity += std::abs(ratio - std::round(ratio));
            }
        }
    }
    harmonicity /= (size * 10.0f);
    inharmonicity /= (size * 10.0f);
    features.push_back(harmonicity);          // Harmonic content
    features.push_back(inharmonicity);        // Inharmonic content
    
    // 4. Energy and dynamics features
    float dynamicRange = 0.0f;
    float crestFactor = 0.0f;
    float entropy = 0.0f;
    
    float maxValue = *std::max_element(neuralResponses.begin(), neuralResponses.end(), 
                                      [](float a, float b) { return std::abs(a) < std::abs(b); });
    float rms = std::sqrt(totalPower / size);
    
    if (rms > 0.0f) {
        dynamicRange = 20.0f * std::log10(std::max(fabs(maxValue), rms) / (rms + 1e-6f));
        crestFactor = fabs(maxValue) / (rms + 1e-6f);
        
        // Calculate spectral entropy
        std::vector<float> probs;
        for (float val : neuralResponses) {
            probs.push_back(std::abs(val) / (totalPower + 1e-6f));
        }
        
        float normSum = std::accumulate(probs.begin(), probs.end(), 0.0f);
        if (normSum > 0.0f) {
            for (float& p : probs) p /= normSum;
            
            for (float p : probs) {
                if (p > 0.0f) {
                    entropy -= p * std::log(p);
                }
            }
        }
    }
    
    features.push_back(dynamicRange);         // Dynamic range (dB)
    features.push_back(crestFactor);          // Crest factor
    features.push_back(entropy);              // Spectral entropy
    
    // 5. Mood and timbre features
    float brightness = 0.0f;
    float warmth = 0.0f;
    float roughness = 0.0f;
    
    // Simple heuristic-based feature extraction
    for (size_t i = 0; i < std::min<size_t>(50, size); ++i) {
        if (neuralResponses[i] > 0.5f) {
            brightness += 1.0f; // High values = bright sound
        } else if (neuralResponses[i] < -0.5f) {
            warmth += 1.0f;    // Low values = warm sound
        }
        roughness += std::abs(neuralResponses[i]);
    }
    
    brightness /= 50.0f;
    warmth /= 50.0f;
    roughness /= 50.0f;
    features.push_back(brightness);           // Sound brightness
    features.push_back(warmth);               // Sound warmth
    features.push_back(roughness);            // Sound roughness
    
    // 6. Temporal envelope features
    float attackTime = 0.0f;
    float releaseTime = 0.0f;
    float sustainLevel = 0.0f;
    
    // Simplified envelope detection
    float maxEnvelope = 0.0f;
    float minEnvelope = 0.0f;
    
    for (size_t i = 0; i < size; ++i) {
        maxEnvelope = std::max(maxEnvelope, std::abs(neuralResponses[i]));
        minEnvelope = std::min(minEnvelope, std::abs(neuralResponses[i]));
    }
    
    sustainLevel = (maxEnvelope + minEnvelope) / 2.0f;
    attackTime = size / (spectralFlux + 1.0f); // Heuristic
    releaseTime = size / (rhythmStrength + 1.0f); // Heuristic
    
    features.push_back(attackTime);           // Attack time
    features.push_back(releaseTime);          // Release time
    features.push_back(sustainLevel);         // Sustain level
}

const std::vector<float>& AudioProcessor::getFeatures() const {
    return pImpl->features;
}

size_t AudioProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

bool AudioProcessor::isProcessingEnabled() const {
    return pImpl->isInitialized && pImpl->config->getOr<bool>("audio_processing_enabled", true);
}

void AudioProcessor::updateWithBrain(const Brain& brain) {
    if (pImpl->isInitialized) {
        // Update audio processing based on brain state
        // This allows audio processing to adapt to current neural conditions
        updateTemporalFilters(brain);
    }
}

void AudioProcessor::updateTemporalFilters(const Brain& brain) {
    // Update temporal filters based on brain attention or learning
    float learningRate = 0.005f;
    
    // Adjust filter parameters based on brain activity
    float brainActivity = static_cast<float>(brain.getTotalSpikeCount()) / 
                        static_cast<float>(brain.getTotalNeuronCount() + 1);
    
    for (size_t i = 0; i < pImpl->temporalFilters.size(); ++i) {
        // Adaptive filter learning
        float targetResponse = std::sin(2.0f * static_cast<float>(M_PI) * 
                                        static_cast<float>(i) / static_cast<float>(pImpl->temporalFilters.size()));
        
        pImpl->temporalFilters[i] += learningRate * brainActivity * (targetResponse - pImpl->temporalFilters[i]);
    }
}

} // namespace nlm
