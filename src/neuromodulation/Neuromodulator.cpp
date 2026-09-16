#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>
#include <stdexcept>
#include <limits>
#include <cstring>

namespace nlm {

// Helper function for generating random numbers
class RandomGenerator {
public:
    RandomGenerator() : seed(std::chrono::system_clock::now().time_since_epoch().count()) {
        rng.seed(seed);
    }
    
    float randFloat() {
        return std::uniform_real_distribution<float>(0.0f, 1.0f)(rng);
    }
    
    float randFloat(float min, float max) {
        return std::uniform_real_distribution<float>(min, max)(rng);
    }
    
    int randInt(int min, int max) {
        return std::uniform_int_distribution<int>(min, max)(rng);
    }
    
    float getGaussian(float mean, float stddev) {
        return std::normal_distribution<float>(mean, stddev)(rng);
    }
    
    void setSeed(unsigned int s) {
        seed = s;
        rng.seed(seed);
    }
    
private:
    unsigned int seed;
    std::mt19937 rng;
};

// Ionic current parameters for dopamine neuron
struct IonicCurrents {
    float Na;      // Sodium current (mS/cm²)
    float K;       // Potassium current (mS/cm²)
    float Ca;      // Calcium current (mS/cm²)
    float leak;    // Leak current (mS/cm²)
    
    IonicCurrents() : Na(0.0f), K(0.0f), Ca(0.0f), leak(0.0f) {}
};

// Synaptic parameters for dopamine neuron
struct SynapticParameters {
    float releaseProbability;      // Probability of vesicle release (0-1)
    float vesicleSize;             // Number of dopamine molecules per vesicle
    float poolSize;                // Size of vesicle pool
    float resynthesisRate;         // Rate of vesicle resynthesis
    float depletionRate;           // Rate of vesicle depletion
    
    SynapticParameters()
        : releaseProbability(0.3f)
        , vesicleSize(1000.0f)
        , poolSize(10000.0f)
        , resynthesisRate(0.05f)
        , depletionRate(0.1f) {}
};

// Dopamine neuron state
struct DopamineNeuronState {
    float membranePotential;        // mV
    float restingPotential;        // mV
    float threshold;               // mV
    float resetPotential;          // mV
    float timeConstant;            // ms
    float adaptationVariable;      // for spike frequency adaptation
    float CaCurrent;              // intracellular Ca2+ concentration
    uint32_t refractoryRemaining;  // remaining refractory time (ms)
    Timestamp lastSpikeTime;      // last spike time (-1 if none)
    bool isBursting;              // burst firing mode
    float burstTime;              // remaining burst time (ms)
    float firingRate;             // firing rate (Hz)
    
    DopamineNeuronState()
        : membranePotential(-70.0f)
        , restingPotential(-70.0f)
        , threshold(-55.0f)
        , resetPotential(-70.0f)
        , timeConstant(10.0f)
        , adaptationVariable(0.0f)
        , CaCurrent(0.0f)
        , refractoryRemaining(0)
        , lastSpikeTime(-1.0f)
        , isBursting(false)
        , burstTime(0.0f)
        , firingRate(0.0f) {}
};

// TD learning parameters
struct TDLearningParams {
    float alpha;           // learning rate
    float gamma;           // discount factor
    float lambda;          // eligibility trace decay
    float traceSize;       // eligibility trace memory
    
    TDLearningParams()
        : alpha(0.01f)
        , gamma(0.9f)
        , lambda(0.9f)
        , traceSize(1000) {}
};

// Dopamine signal components
struct DopamineSignal {
    float tonic;      // tonic (baseline) signal level
    float phasic;     // phasic (burst) signal level
    float predictionError; // reward prediction error component
    
    DopamineSignal() : tonic(0.0f), phasic(0.0f), predictionError(0.0f) {}
};

// Forward declarations
class Config;
class Brain;
class PredictionError;
class STDP;
class Hebbian;

// Dopamine neuron's LIF implementation
class DopamineNeuron {
private:
    Neuron neuron;
    
public:
    explicit DopamineNeuron(NeuronId id) : neuron(id) {}
    
    ~DopamineNeuron() = default;
    
    // Disable copying, enable moving
    DopamineNeuron(const DopamineNeuron&) = delete;
    DopamineNeuron& operator=(const DopamineNeuron&) = delete;
    DopamineNeuron(DopamineNeuron&&) noexcept;
    DopamineNeuron& operator=(DopamineNeuron&&) noexcept;
    
    // LIF step function - returns true if neuron fired
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Update neuron state for one simulation step
    void updateNeuron(TimestepDuration dt);
    
    // Get reference to underlying neuron
    Neuron& getNeuron() { return neuron; }
    const Neuron& getNeuron() const { return neuron; }
};

// Neuromodulator: Abstract base for neuromodulatory signals
class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Initialize with configuration and brain reference
    virtual void initialize(std::shared_ptr<Config> config, Brain* brain) = 0;
    
    // Reset neuromodulator state
    virtual void reset() = 0;
    
    // Get prediction error value
    virtual float getPredictionError() const = 0;
    
    // Get phasic signal component
    virtual float getPhasicSignal() const = 0;
    
    // Get tonic signal component
    virtual float getTonicSignal() const = 0;
    
    // Get complete dopamine signal
    virtual DopamineSignal getSignal() const = 0;
    
    // Check if initialized
    virtual bool isInitialized() const = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Initialization
    void initialize(std::shared_ptr<Config> config, Brain* brain) override;
    
    // Reset
    void reset() override;
    
    // Get prediction error value
    float getPredictionError() const override;
    
    // Get phasic signal component
    float getPhasicSignal() const override;
    
    // Get tonic signal component
    float getTonicSignal() const override;
    
    // Get complete dopamine signal
    DopamineSignal getSignal() const override;
    
    // Check if initialized
    bool isInitialized() const override;
    
    // Get neuron state
    DopamineNeuronState getNeuronState() const;
    void setNeuronState(const DopamineNeuronState& state);
    
    // Get synaptic parameters
    SynapticParameters getSynapticParameters() const;
    void setSynapticParameters(const SynapticParameters& params);
    
    // Get TD learning parameters
    TDLearningParams getTDLearningParameters() const;
    void setTDLearningParameters(const TDLearningParams& params);
    
    // Get value function
    const std::vector<float>& getValueFunction() const;
    
    // Get eligibility trace
    EligibilityTrace getEligibilityTrace() const;
    
    // Update TD learning
    void updateTDLearning(float reward, size_t state, size_t nextState);
    
    // Get average firing rate
    float getAverageFiringRate() const;
    void setFiringRate(float rate);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    void initialize(std::shared_ptr<Config> config, Brain* brain) override {}
    void reset() override {}
    float getPredictionError() const override { return 0.0f; }
    float getPhasicSignal() const override { return 0.0f; }
    float getTonicSignal() const override { return 0.0f; }
    DopamineSignal getSignal() const override { return DopamineSignal(); }
    bool isInitialized() const override { return false; }
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    void initialize(std::shared_ptr<Config> config, Brain* brain) override {}
    void reset() override {}
    float getPredictionError() const override { return 0.0f; }
    float getPhasicSignal() const override { return 0.0f; }
    float getTonicSignal() const override { return 0.0f; }
    DopamineSignal getSignal() const override { return DopamineSignal(); }
    bool isInitialized() const override { return false; }
};

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    void initialize(std::shared_ptr<Config> config, Brain* brain) override {}
    void reset() override {}
    float getPredictionError() const override { return 0.0f; }
    float getPhasicSignal() const override { return 0.0f; }
    float getTonicSignal() const override { return 0.0f; }
    DopamineSignal getSignal() const override { return DopamineSignal(); }
    bool isInitialized() const override { return false; }
};

// Eligibility trace for synaptic learning
class EligibilityTrace {
private:
    std::vector<float> traces;
    float decay;
    float alpha;
    
public:
    EligibilityTrace(size_t size, float decay, float alpha)
        : traces(size, 0.0f), decay(decay), alpha(alpha) {}
    
    void update() {
        for (auto& trace : traces) {
            trace *= decay;
        }
    }
    
    void add(size_t index, float value) {
        if (index < traces.size()) {
            traces[index] += value * alpha;
        }
    }
    
    void multiply(float value) {
        for (auto& trace : traces) {
            trace *= value;
        }
    }
    
    float getTrace(size_t index) const {
        if (index < traces.size()) {
            return traces[index];
        }
        return 0.0f;
    }
    
    void reset() {
        std::fill(traces.begin(), traces.end(), 0.0f);
    }
    
    size_t size() const { return traces.size(); }
};

} // namespace nlm
