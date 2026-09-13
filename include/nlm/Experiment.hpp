#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <map>
#include <set>
#include <cmath>
#include <random>

#include "nlm/Constants.hpp"
#include "nlm/CLI.hpp"
#include "nlm/Exception.hpp"
#include "nlm/Config.hpp"

namespace nlm {

/**
 * Base class for all neural computation experiments
 * Provides common functionality and abstract interface
 */
class ExperimentBase {
public:
    struct ExperimentResult {
        std::string experimentName;
        std::string timestamp;
        bool success;
        std::string errorMessage;
        
        // Metrics
        float executionTime;
        size_t totalSteps;
        float averageFiringRate;
        size_t totalSpikes;
        float weightChangeTotal;
        
        // Detailed metrics
        std::map<std::string, float> detailedMetrics;
        
        // Learning detection
        bool learningDetected;
        float learningRate;
        
        // Validation
        bool validated;
        std::vector<std::string> validationErrors;
        
        ExperimentResult() : success(false), executionTime(0.0f), totalSteps(0),
                            averageFiringRate(0.0f), totalSpikes(0), weightChangeTotal(0.0f),
                            learningDetected(false), learningRate(0.0f), validated(false) {}
    };

    ExperimentBase(const std::string& name, const std::string& description);
    virtual ~ExperimentBase() = default;

    // Get experiment metadata
    const std::string& getName() const { return name; }
    const std::string& getDescription() const { return description; }
    virtual std::string getVersion() const { return "1.0.0"; }

    // Experiment lifecycle
    virtual bool initialize(const Config& config) = 0;
    virtual bool run() = 0;
    virtual void cleanup() = 0;
    
    // Get experiment results
    const ExperimentResult& getResult() const { return result; }
    
    // Check if experiment completed successfully
    bool isCompleted() const { return completed; }
    
    // Get experiment status
    virtual std::string getStatus() const { return completed ? "Completed" : "Running"; }

    // Run experiment with error handling
    bool execute(const Config& config);

protected:
    std::string name;
    std::string description;
    ExperimentResult result;
    bool completed;
    std::chrono::steady_clock::time_point startTime;
    
    // Helper methods
    void setSuccess();
    void setError(const std::string& error);
    void recordMetric(const std::string& name, float value);
    std::string getTimestamp() const;
    float getExecutionTime() const;
};

/**
 * Connectivity experiment - tests basic neural network properties
 */
class ConnectivityExperiment : public ExperimentBase {
public:
    ConnectivityExperiment();
    
    bool initialize(const Config& config) override;
    bool run() override;
    void cleanup() override;
    
    std::string getStatus() const override { return "Testing basic neural connectivity and spike propagation"; }
    
    // Specific connectivity metrics
    struct ConnectivityMetrics {
        bool spikesPropagate;
        size_t totalSpikesGenerated;
        float averageFiringRate;
        std::vector<float> neuronPotentials;
        std::vector<size_t> activeNeuronCount;
    };
    
    const ConnectivityMetrics& getConnectivityMetrics() const { return metrics; }

private:
    ConnectivityMetrics metrics;
    std::vector<float> initialPotentials;
};

/**
 * Plasticity experiment - tests synaptic learning mechanisms
 */
class PlasticityExperiment : public ExperimentBase {
public:
    PlasticityExperiment();
    
    bool initialize(const Config& config) override;
    bool run() override;
    void cleanup() override;
    
    std::string getStatus() const override { return "Testing synaptic plasticity (STDP, Hebbian, reward-modulated)"; }
    
    // Plasticity metrics
    struct PlasticityMetrics {
        bool hebbianLearningActive;
        bool stdpLearningActive;
        bool rewardModulatedLearningActive;
        float totalWeightChange;
        float weightChangeRate;
        std::map<std::string, float> plasticityRuleMetrics;
    };
    
    const PlasticityMetrics& getPlasticityMetrics() const { return metrics; }

private:
    PlasticityMetrics metrics;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
};

/**
 * STDP (Spike-Timing-Dependent Plasticity) experiment
 */
class StdpExperiment : public ExperimentBase {
public:
    StdpExperiment();
    
    bool initialize(const Config& config) override;
    bool run() override;
    void cleanup() override;
    
    std::string getStatus() const override { return "Testing STDP rules with pre-before-post spike timing"; }
    
    // STDP metrics
    struct StdpMetrics {
        bool stdpMechanismWorking;
        float potentiationObserved;
        float depressionObserved;
        float timingDependentEffect;
        std::vector<float> weightChanges;
    };
    
    const StdpMetrics& getStdpMetrics() const { return metrics; }

private:
    StdpMetrics metrics;
    std::vector<float> preSpikeTimes;
    std::vector<float> postSpikeTimes;
};

/**
 * Development experiment - tests developmental plasticity
 */
class DevelopmentExperiment : public ExperimentBase {
public:
    DevelopmentExperiment();
    
    bool initialize(const Config& config) override;
    bool run() override;
    void cleanup() override;
    
    std::string getStatus() const override { return "Testing developmental stages and critical period effects"; }
    
    // Development metrics
    struct DevelopmentMetrics {
        bool synaptogenesisActive;
        bool pruningActive;
        bool maturationActive;
        float structuralPlasticityRate;
        float criticalPeriodEffect;
    };
    
    const DevelopmentMetrics& getDevelopmentMetrics() const { return metrics; }

private:
    DevelopmentMetrics metrics;
};

/**
 * Memory experiment - tests memory systems integration
 */
class MemoryExperiment : public ExperimentBase {
public:
    MemoryExperiment();
    
    bool initialize(const Config& config) override;
    bool run() override;
    void cleanup() override;
    
    std::string getStatus() const override { return "Testing memory systems (working, episodic, semantic)"; }
    
    // Memory metrics
    struct MemoryMetrics {
        bool workingMemoryActive;
        bool episodicMemoryActive;
        bool semanticMemoryActive;
        size_t workingMemoryCapacity;
        size_t episodicMemoryEpisodes;
        float patternCompletionSuccess;
    };
    
    const MemoryMetrics& getMemoryMetrics() const { return metrics; }

private:
    MemoryMetrics metrics;
};

/**
 * Neuromodulation experiment - tests neuromodulatory systems
 */
class NeuromodulationExperiment : public ExperimentBase {
public:
    NeuromodulationExperiment();
    
    bool initialize(const Config& config) override;
    bool run() override;
    void cleanup() override;
    
    std::string getStatus() const override { return "Testing neuromodulation (dopamine, curiosity, novelty)"; }
    
    // Neuromodulation metrics
    struct NeuromodulationMetrics {
        bool dopamineActive;
        bool curiosityActive;
        bool noveltyActive;
        bool predictionErrorActive;
        float neuromodulationStrength;
        float explorationExploitationBalance;
    };
    
    const NeuromodulationMetrics& getNeuromodulationMetrics() const { return metrics; }

private:
    NeuromodulationMetrics metrics;
};

/**
 * Advanced experiment - combines multiple mechanisms
 */
class AdvancedExperiment : public ExperimentBase {
public:
    AdvancedExperiment();
    
    bool initialize(const Config& config) override;
    bool run() override;
    void cleanup() override;
    
    std::string getStatus() const override { return "Advanced experiment combining all neural mechanisms"; }
    
    // Integration metrics
    struct IntegrationMetrics {
        bool allSystemsIntegrated;
        float systemCoordination;
        float emergentBehavior;
        float learningEfficiency;
    };
    
    const IntegrationMetrics& getIntegrationMetrics() const { return metrics; }

private:
    IntegrationMetrics metrics;
};

// Experiment factory function types
using ExperimentFactory = std::function<std::unique_ptr<ExperimentBase>()>;

// Experiment registry for dynamic loading
class ExperimentRegistry {
public:
    static ExperimentRegistry& getInstance() {
        static ExperimentRegistry instance;
        return instance;
    }
    
    void registerExperiment(const std::string& name, ExperimentFactory factory) {
        factories[name] = factory;
    }
    
    std::unique_ptr<ExperimentBase> createExperiment(const std::string& name) {
        auto it = factories.find(name);
        if (it != factories.end()) {
            return it->second();
        }
        return nullptr;
    }
    
    std::vector<std::string> getExperimentNames() const {
        std::vector<std::string> names;
        names.reserve(factories.size());
        for (const auto& pair : factories) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    bool hasExperiment(const std::string& name) const {
        return factories.find(name) != factories.end();
    }
    
    void clear() {
        factories.clear();
    }

private:
    ExperimentRegistry() = default;
    std::unordered_map<std::string, ExperimentFactory> factories;
};

// Macros for experiment registration
#define REGISTER_EXPERIMENT(name, experiment_class) \
    namespace { \
        struct name##_Registration { \
            name##_Registration() { \
                ExperimentRegistry::getInstance().registerExperiment(\
                    #name, []() { return std::make_unique<experiment_class>(); }); \
            } \
        }; \
        name##_Registration name##_reg; \
    }

} // namespace nlm
