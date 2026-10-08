#include "Audio.hpp"
#include "brain/Brain.hpp"
#include "brain/NeuralPopulation.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Curiosity.hpp"
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <fftw3.h>

namespace nlm {

#include "Audio.hpp"
#include "brain/Brain.hpp"
#include "brain/NeuralPopulation.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Curiosity.hpp"
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <fftw3.h>

namespace nlm {

struct AudioProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    std::shared_ptr<Brain> brain;
    PopulationId spectralAnalysisPopulation;
    PopulationId featureExtractionPopulation;
    PopulationId modulationPopulation;
    float currentVolume;
    float spectralSpread;
    float spectralCentroid;
    std::vector<float> harmonicFeatures;
    std::vector<float> temporalFeatures;
    
    Impl() : featureDimensions(0), currentVolume(0.0f), spectralSpread(0.0f), 
             spectralCentroid(0.0f), harmonicFeatures(20, 0.0f), 
             temporalFeatures(10, 0.0f) {
        initializeNeuralInfrastructure();
    }
    
    void initializeNeuralInfrastructure() {
        // Create specialized neural populations for audio processing
        spectralAnalysisPopulation = 0;
        featureExtractionPopulation = 1;
        modulationPopulation = 2;
        
        // Initialize brain with appropriate configuration for audio processing
        Config config;
        config.set("brain", {
            {"neuronCount", 200},
            {"connectionProbability", 0.1},
            {"v_thresh", -55.0},
            {"v_reset", -65.0},
            {"tau_mem", 15.0},
            {"tau_ref", 2.0},
            {"i_thresh", 8.0},
            {"c_m", 1.0}
        });
        
        brain = std::make_shared<Brain>(config);
        brain->initialize();
    }
    
    void computeSpectralFeatures(const std::vector<float>& audioData) {
        if (audioData.empty()) return;
        
        size_t n = audioData.size();
        if (n < 256) {
            features = audioData;
            featureDimensions = audioData.size();
            return;
        }
        
        // Convert to spectrogram using FFT for spectral decomposition
        size_t fftSize = 1024;
        std::vector<float> spectrum(fftSize / 2 + 1, 0.0f);
        
        // Real FFT using NLM's neural computation framework
        fftwf_plan plan = fftwf_plan_r2c_1d(n, 
            const_cast<float*>(audioData.data()), 
            reinterpret_cast<fftwf_complex*>(spectrum.data()), 
            FFTW_ESTIMATE);
        
        fftwf_execute(plan);
        fftwf_destroy_plan(plan);
        
        // Extract spectral features using neural mechanisms
        std::vector<float> magnitudeSpectrum(fftSize / 2 + 1, 0.0f);
        spectralSpread = 0.0f;
        spectralCentroid = 0.0f;
        float totalMagnitude = 0.0f;
        
        for (size_t i = 0; i < fftSize / 2 + 1; ++i) {
            float freq = static_cast<float>(i) * 440.0f / (fftSize / 2); // Normalized frequency
            float magnitude = std::sqrt(spectrum[i][0] * spectrum[i][0] + spectrum[i][1] * spectrum[i][1]);
            magnitudeSpectrum[i] = magnitude;
            totalMagnitude += magnitude;
            
            // Neural-based spectral decomposition with lateral inhibition
            float inhibitionStrength = 0.05f * magnitude;
            
            // Inject current into spectral analysis neurons
            if (brain) {
                brain->injectCurrentToNeurons(NeuronType::SENSORY, inhibitionStrength * 10.0f);
            }
        }
        
        if (totalMagnitude > 0.0f) {
            // Compute spectral centroid (weighted average of frequency)
            float weightedSum = 0.0f;
            for (size_t i = 0; i < fftSize / 2 + 1; ++i) {
                float freq = static_cast<float>(i) * 440.0f / (fftSize / 2);
                weightedSum += freq * magnitudeSpectrum[i];
            }
            spectralCentroid = weightedSum / totalMagnitude;
            
            // Compute spectral spread (variance of frequency distribution)
            float meanFreq = spectralCentroid;
            for (size_t i = 0; i < fftSize / 2 + 1; ++i) {
                float freq = static_cast<float>(i) * 440.0f / (fftSize / 2);
                spectralSpread += magnitudeSpectrum[i] * (freq - meanFreq) * (freq - meanFreq);
            }
            spectralSpread = std::sqrt(spectralSpread / totalMagnitude);
        }
        
        // Create feature vector using biologically plausible sound representation
        features.clear();
        featureDimensions = 64; // Compressed feature space
        
        // Fundamental spectral features (log compressed)
        for (size_t i = 0; i < std::min<size_t>(17, magnitudeSpectrum.size()); ++i) {
            features.push_back(std::log(1.0f + magnitudeSpectrum[i * 4])); // Log scale for dynamic range
        }
        
        // Temporal modulation features
        std::vector<float> temporalModulation(10, 0.0f);
        for (size_t i = 0; i + 10 < audioData.size(); i += 10) {
            float windowEnergy = 0.0f;
            for (size_t j = 0; j < 10 && i + j < audioData.size(); ++j) {
                windowEnergy += audioData[i + j] * audioData[i + j];
            }
            temporalModulation[i % 10] += std::sqrt(windowEnergy);
        }
        
        // Add temporal features to representation
        for (float tm : temporalModulation) {
            features.push_back(std::min(1.0f, tm / 10.0f));
        }
        
        // Add harmonic and modulation features
        for (size_t i = 0; i < harmonicFeatures.size(); ++i) {
            features.push_back(harmonicFeatures[i]);
        }
        for (size_t i = 0; i < temporalFeatures.size(); ++i) {
            features.push_back(temporalFeatures[i]);
        }
        
        // Add envelope and timbre features
        float minVal = *std::min_element(audioData.begin(), audioData.end());
        float maxVal = *std::max_element(audioData.begin(), audioData.end());
        float envelope = (maxVal - minVal) / (maxVal + 1e-6f);
        features.push_back(envelope);
        features.push_back(spectralSpread);
        features.push_back(spectralCentroid);
        
        // Neuromodulatory influence on feature extraction
        if (brain) {
            auto curiosity = brain->getCuriosity();
            if (curiosity) {
                float curiosityLevel = curiosity->getLevel();
                float modulation = std::min(1.0f, curiosityLevel * 0.5f);
                
                // Apply curiosity-driven exploration to features
                for (size_t i = 0; i < features.size() && i < 20; ++i) {
                    features[i] = features[i] * (1.0f - modulation) + 
                                 features[i] * 0.5f * modulation * (i % 2 == 0 ? 1.0f : 0.5f);
                }
            }
        }
    }
    
    void updateNeuralDynamics(const std::vector<float>& audioData) {
        if (!brain) return;
        
        // Update neural populations with audio input
        brain->receiveSensoryInput(audioData);
        
        // Step neural dynamics for spectral processing
        for (int step = 0; step < 10; ++step) {
            brain->step(step);
        }
        
        // Extract features from neural activity patterns
        std::vector<float> neuralActivityPattern;
        auto regions = brain->getRegions();
        
        for (const auto& region : regions) {
            auto neurons = region->getAllNeurons();
            for (Neuron* neuron : neurons) {
                float membranePotential = neuron->getMembranePotential();
                neuralActivityPattern.push_back(std::min(1.0f, std::max(-1.0f, (membranePotential + 65.0f) / 30.0f)));
            }
        }
        
        // Apply competitive network dynamics for feature selection
        if (neuralActivityPattern.size() > 10) {
            // Lateral inhibition simulation
            std::vector<float> inhibitedPattern = lateralInhibition(neuralActivityPattern, 0.3f);
            
            // Competitive selection based on prediction error
            std::vector<float> selectedFeatures = competitiveSelection(inhibitedPattern, 5);
            
            // Blend original and selected features
            for (size_t i = 0; i < std::min(features.size(), selectedFeatures.size()); ++i) {
                features[i] = 0.7f * features[i] + 0.3f * selectedFeatures[i];
            }
        }
    }
    
    std::vector<float> lateralInhibition(const std::vector<float>& input, float inhibitionStrength) {
        std::vector<float> output = input;
        size_t size = input.size();
        
        for (size_t i = 0; i < size; ++i) {
            float maxNeighbor = 0.0f;
            // Find maximum among neighbors
            for (int offset = -1; offset <= 1; ++offset) {
                size_t neighbor = i + offset;
                if (neighbor < size && neighbor != i) {
                    maxNeighbor = std::max(maxNeighbor, input[neighbor]);
                }
            }
            
            // Apply lateral inhibition
            float inhibition = inhibitionStrength * (maxNeighbor - input[i]);
            output[i] = std::max(0.0f, output[i] + inhibition);
        }
        
        return output;
    }
    
    std::vector<float> competitiveSelection(const std::vector<float>& input, size_t numWinners) {
        std::vector<float> output(input.size(), 0.0f);
        
        // Find top-k features (competitive selection)
        std::vector<size_t> indices(input.size());
        std::iota(indices.begin(), indices.end(), 0);
        
        std::partial_sort(indices.begin(), indices.begin() + numWinners, indices.end(),
                         [&input](size_t a, size_t b) { return input[a] > input[b]; });
        
        // Select and enhance top features
        for (size_t i = 0; i < numWinners && i < indices.size(); ++i) {
            output[indices[i]] = input[indices[i]] * (1.0f + 0.5f * (i + 1) / static_cast<float>(numWinners));
        }
        
        return output;
    }
    
    void updateHarmonicFeatures(const std::vector<float>& audioData) {
        // Estimate harmonic structure using neural mechanisms
        for (size_t i = 0; i < harmonicFeatures.size(); ++i) {
            float freq = 220.0f * (i + 1); // Harmonic frequencies
            float harmonicEnergy = estimateHarmonicEnergy(audioData, freq, 0.02f);
            harmonicFeatures[i] = std::min(1.0f, harmonicEnergy * 10.0f);
        }
    }
    
    float estimateHarmonicEnergy(const std::vector<float>& audioData, float fundamentalFreq, float bandwidth) {
        // Simplified harmonic energy estimation
        float energy = 0.0f;
        size_t sampleRate = 44100;
        size_t fundamentalSamples = static_cast<size_t>(fundamentalFreq * sampleRate / 44100.0f);
        
        if (fundamentalSamples >= audioData.size()) return 0.0f;
        
        for (size_t i = 0; i + fundamentalSamples < audioData.size(); i += fundamentalSamples) {
            float correlation = 0.0f;
            for (size_t j = 0; j < fundamentalSamples && i + j < audioData.size(); ++j) {
                correlation += audioData[i + j] * audioData[i + j];
            }
            energy += correlation / fundamentalSamples;
        }
        
        return energy;
    }
};

AudioProcessor::AudioProcessor() : pImpl(new Impl) {}

AudioProcessor::~AudioProcessor() = default;

void AudioProcessor::process(const Audio& input) {
    // Extract raw audio data
    std::vector<float> audioData = input.getData();
    
    // Compute current audio level
    pImpl->currentVolume = 0.0f;
    if (!audioData.empty()) {
        float sum = std::accumulate(audioData.begin(), audioData.end(), 0.0f);
        pImpl->currentVolume = std::abs(sum / audioData.size());
    }
    
    // Compute spectral features using neural mechanisms
    pImpl->computeSpectralFeatures(audioData);
    
    // Update harmonic features
    pImpl->updateHarmonicFeatures(audioData);
    
    // Update neural dynamics for real-time processing
    pImpl->updateNeuralDynamics(audioData);
    
    // Apply neuromodulatory effects
    if (pImpl->brain) {
        pImpl->brain->step(0);
    }
}

const std::vector<float>& AudioProcessor::getFeatures() const {
    return pImpl->features;
}

} // namespace nlm
