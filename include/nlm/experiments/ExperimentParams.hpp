#pragma once

#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <iomanip>
#include <numeric>

namespace nlm {

// Forward declarations
class Brain;
class Neuron;
class Synapse;

// Experiment parameter types
struct ExperimentParams {
    // Basic connectivity experiment parameters
    size_t neurons_to_stimulate = 10;
    float current_per_neuron = 50.0f;
    size_t duration_steps = 50;
    
    // Plasticity experiment parameters
    size_t learning_duration = 1000;
    size_t sensory_neurons_to_stimulate = 20;
    float sensory_current = 30.0f;
    
    // STDP verification parameters
    size_t stdp_trials = 50;
    size_t test_synapses = 5;
    size_t test_neuron_pairs = 1;
    
    // Common parameters
    RegionId test_region_id = RegionId(1);
    SimulationStep timestep = 0.001;
};

// Experiment results structure
struct ExperimentResult {
    bool success = false;
    std::string message;
    std::chrono::milliseconds executionTime;
    std::vector<std::string> logs;
};

// Base experiment class
class BaseExperiment {
public:
    virtual ~BaseExperiment() = default;
    
    virtual ExperimentResult run(std::shared_ptr<Brain> brain) = 0;
    virtual std::string getName() const = 0;
    virtual ExperimentParams getDefaultParams() const { return ExperimentParams(); }
};

} // namespace nlm