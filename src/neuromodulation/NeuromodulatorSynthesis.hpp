#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "Neuromodulator.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// Forward declarations
class NeuromodulationEngine;

// Signal synthesis parameters
struct SynthesisParams {
    float synthesisRate;                    // Rate of signal synthesis
    float synthesisNoise;                   // Noise level in synthesis
    float synthesisAmplitude;               // Amplitude of synthesized signals
    float synthesisFrequency;               // Frequency of synthesized signals
    float synthesisDuration;                // Duration of synthesis
    float synthesisModulation;              // Modulation for synthesis
    float synthesisComplexity;              // Complexity of synthesis
    float synthesisLearningRate;            // Learning rate for synthesis
    
    SynthesisParams()
        : synthesisRate(1.0f), synthesisNoise(0.1f), synthesisAmplitude(1.0f),
          synthesisFrequency(1.0f), synthesisDuration(1.0f), synthesisModulation(1.0f),
          synthesisComplexity(0.5f), synthesisLearningRate(0.01f) {}
};

// Signal waveform types
enum class WaveformType {
    SINUSOIDAL,
    EXPONENTIAL,
    GAUSSIAN,
    RECTANGULAR,
    TRIANGULAR,
    SAWTOOTH,
    PULSE,
    CHIRP,
    BIPHASIC,
    COMBINED
};

// Signal generation state
struct SignalState {
    float currentLevel;                    // Current signal level
    float targetLevel;                     // Target signal level
    float amplitude;                       // Signal amplitude
    float frequency;                       // Signal frequency
    float phase;                           // Signal phase
    float offset;                          // Signal offset
    float decay;                           // Signal decay
    float rise;                            // Signal rise time
    float waveformType;                    // Type of waveform
    float modulationIndex;                 // Modulation index
    float noiseLevel;                      // Noise level
    float temporalCorrelation;             // Temporal correlation
    float spectralDensity;                  // Spectral density
    
    SignalState()
        : currentLevel(0.0f), targetLevel(0.0f), amplitude(1.0f), frequency(1.0f),
          phase(0.0f), offset(0.0f), decay(0.1f), rise(0.1f), waveformType(0.0f),
          modulationIndex(0.0f), noiseLevel(0.0f), temporalCorrelation(0.5f),
          spectralDensity(1.0f) {}
};

// Signal characteristics
struct SignalCharacteristics {
    float duration;                        // Signal duration
    float bandwidth;                       // Signal bandwidth
    float power;                           // Signal power
    float energy;                          // Signal energy
    float entropy;                         // Signal entropy
    float complexity;                      // Signal complexity
    float coherence;                       // Signal coherence
    float stationarity;                    // Signal stationarity
    float predictability;                  // Signal predictability
    float informationContent;              // Signal information content
    
    SignalCharacteristics()
        : duration(1.0f), bandwidth(1.0f), power(1.0f), energy(1.0f),
          entropy(0.0f), complexity(0.5f), coherence(0.0f), stationarity(0.5f),
          predictability(0.5f), informationContent(0.0f) {}
};

// Adaptive synthesis parameters
struct AdaptiveSynthesis {
    float adaptationRate;                   // Rate of adaptation
    float memoryDepth;                      // Memory depth for adaptation
    float predictionHorizon;               // Prediction horizon
    float explorationRate;                  // Rate of exploration
    float exploitationRate;                 // Rate of exploitation
    float noveltyThreshold;                 // Novelty threshold
    float complexityThreshold;              // Complexity threshold
    std::vector<float> memoryTrace;         // Memory trace for adaptation
    
    AdaptiveSynthesis()
        : adaptationRate(0.01f), memoryDepth(100.0f), predictionHorizon(10.0f),
          explorationRate(0.3f), exploitationRate(0.7f), noveltyThreshold(0.5f),
          complexityThreshold(0.3f) {
        memoryTrace.resize(100, 0.0f);
    }
};

// Neuromodulator synthesis: Dynamic signal synthesis
class NeuromodulatorSynthesis {
public:
    NeuromodulatorSynthesis();
    ~NeuromodulatorSynthesis();
    
    // Disable copying, enable moving
    NeuromodulatorSynthesis(const NeuromodulatorSynthesis&) = delete;
    NeuromodulatorSynthesis& operator=(const NeuromodulatorSynthesis&) = delete;
    NeuromodulatorSynthesis(NeuromodulatorSynthesis&&) noexcept;
    NeuromodulatorSynthesis& operator=(NeuromodulatorSynthesis&&) noexcept;
    
    // Initialization
    bool initialize(std::shared_ptr<Brain> brain, std::shared_ptr<NeuromodulationEngine> engine);
    
    // Main update step
    void update(TimestepDuration dt, SimulationStep step);
    
    // Signal synthesis
    void synthesizeSignal(const std::string& name, float intensity, float duration);
    void synthesizeNeuromodulator(const std::string& name, const Neuromodulator& modulator);
    void synthesizeDynamicSignal(const std::string& name, float baseLevel, float modulation);
    void synthesizeAdaptiveSignal(const std::string& name, float prediction, float uncertainty);
    
    // Waveform generation
    float generateWaveform(WaveformType type, float time, const SignalState& state);
    void generateSinusoidal(float& signal, float time, const SignalState& state);
    void generateExponential(float& signal, float time, const SignalState& state);
    void generateGaussian(float& signal, float time, const SignalState& state);
    void generateRectangular(float& signal, float time, const SignalState& state);
    void generateTriangular(float& signal, float time, const SignalState& state);
    void generateSawtooth(float& signal, float time, const SignalState& state);
    void generatePulse(float& signal, float time, const SignalState& state);
    void generateChirp(float& signal, float time, const SignalState& state);
    void generateBiphasic(float& signal, float time, const SignalState& state);
    
    // Adaptive synthesis
    void adaptSynthesis(const std::string& name, float reward, float predictionError);
    void updateMemoryTrace(const std::string& name, float signal);
    void computeExplorationExploitation(const std::string& name, float& exploration, float& exploitation);
    
    // Signal processing
    void filterSignal(float& signal, float cutoffFrequency, float sampleRate);
    void filterSignal(float& signal, const std::vector<float>& filterCoefficients);
    void applyEnvelope(float& signal, float attackTime, float decayTime);
    void applyModulation(float& signal, float modulationIndex, float carrierFrequency);
    void applyNoise(float& signal, float noiseLevel);
    
    // Signal analysis
    void analyzeSignal(const std::string& name, SignalCharacteristics& characteristics);
    void computeSignalStatistics(const std::string& name, float& mean, float& variance, float& skewness);
    void detectSignalEvents(const std::string& name, std::vector<float>& events);
    void classifySignal(const std::string& name, std::string& classification);
    
    // State management
    void updateState(const std::string& name, const SignalState& state);
    SignalState getState(const std::string& name) const;
    
    // Configuration
    void setSynthesisParams(const SynthesisParams& params);
    void setWaveformType(WaveformType type) { pImpl->defaultWaveformType = type; }
    void setAdaptationParams(const AdaptiveSynthesis& params);
    void enableAdaptiveSynthesis(bool enable) { pImpl->enableAdaptiveSynthesis_ = enable; }
    void enableWaveformAdaptation(bool enable) { pImpl->enableWaveformAdaptation_ = enable; }
    
    // Integration with other systems
    void integrateWithNeuromodulators(const std::vector<const Neuromodulator*>& modulators);
    void integrateWithDynamics(const class NeuromodulatorDynamics& dynamics);
    void integrateWithNetwork(const class NeuromodulatorNetwork& network);
    void integrateWithPlasticity(const class PlasticityController& plasticity);
    
    // Signal generation for specific applications
    void generateLearningSignal(float& signal, float predictionError, float reward);
    void generateExplorationSignal(float& signal, float novelty, float curiosity);
    void generateMemorySignal(float& signal, float memoryStrength, float recallProbability);
    void generateDecisionSignal(float& signal, float choiceConfidence, float actionValue);
    void generateRewardSignal(float& signal, float expectedReward, float actualReward);
    
    // Export/import
    void exportSynthesis(std::ostream& stream) const;
    void importSynthesis(std::istream& stream);
    
    // Testing interface
    float getSignalLevel(const std::string& name) const;
    float getSignalAmplitude(const std::string& name) const;
    float getSignalFrequency(const std::string& name) const;
    float getSynthesisEfficiency() const { return pImpl->synthesisEfficiency; }
    float getSignalDiversity() const { return pImpl->signalDiversity; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<NeuromodulationEngine> engine_;
    
    SynthesisParams synthesisParams_;
    AdaptiveSynthesis adaptiveParams_;
    WaveformType defaultWaveformType;
    
    bool enableAdaptiveSynthesis_;
    bool enableWaveformAdaptation_;
    
    // Performance metrics
    float synthesisEfficiency;
    float signalDiversity;
    float waveformVariety;
    float adaptationSpeed;
};

} // namespace nlm