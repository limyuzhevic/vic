#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <memory>
#include <vector>

namespace nlm {

// Experiment class represents a single scientific experimental run
// Records neural network behavior and metrics for analysis
// Used for systematic investigation of brain dynamics, learning, and cognition
class Experiment {
public:
    // Default constructor
    Experiment();
    
    // Constructor with experiment name
    explicit Experiment(const std::string& name);
    
    ~Experiment();
    
    // Disable copying for memory safety, enable moving semantics
    Experiment(const Experiment&) = delete;
    Experiment& operator=(const Experiment&) = delete;
    Experiment(Experiment&&) noexcept;
    Experiment& operator=(Experiment&&) noexcept;
    
    // Experiment metadata
    // Gets the unique name/identifier for this experiment
    const std::string& getName() const;
    // Sets the experiment name/identifier
    void setName(const std::string& name);
    
    // Random seed for reproducible experiments
    // Gets the random seed used for this experiment
    uint64_t getSeed() const;
    // Sets the random seed (affects neural network initialization)
    void setSeed(uint64_t seed);
    
    // Configuration management
    // Sets a configuration parameter for this experiment
    void setConfigValue(const std::string& key, const std::string& value);
    // Retrieves a configuration parameter value
    std::string getConfigValue(const std::string& key) const;
    
    // Metrics collection and analysis
    // Records a numerical metric with given name and value
    // Supports int, float, and double types for different precision needs
    void recordMetric(const std::string& name, double value);
    void recordMetric(const std::string& name, float value);
    void recordMetric(const std::string& name, int value);
    
    // Gets all recorded metrics as name-value pairs
    const std::vector<std::pair<std::string, double>>& getMetrics() const;
    
    // Retrieves specific metric value by name
    double getMetric(const std::string& name) const;
    
    // Checks if a specific metric has been recorded
    bool hasMetric(const std::string& name) const;
    
    // Time-series data collection
    // Records neural network state at a specific simulation step
    // Collects counts and rates for analyzing dynamic behavior
    void recordStep(SimulationStep step, size_t neuronCount, size_t synapseCount,
                   size_t firingCount, float avgFiringRate);
    
    // Step data structure for neural network state snapshots
    struct StepData {
        SimulationStep step;        // Simulation step number
        size_t neuronCount;          // Total neurons in network
        size_t synapseCount;         // Total synapses in network
        size_t firingCount;          // Number of firing neurons
        float avgFiringRate;         // Average firing rate across all neurons (Hz)
    };
    
    // Gets complete time-series step data
    const std::vector<StepData>& getStepData() const;
    
    // Experiment time range
    // Gets the starting simulation step
    SimulationStep getStartStep() const;
    // Sets the starting simulation step
    void setStartStep(SimulationStep step);
    // Gets the ending simulation step
    SimulationStep getEndStep() const;
    // Sets the ending simulation step
    void setEndStep(SimulationStep step);
    
    // Research notes and observations
    // Adds a qualitative note or observation from the experiment
    void addNote(const std::string& note);
    // Gets all research notes
    const std::vector<std::string>& getNotes() const;
    
private:
    // Pimpl idiom for implementation hiding
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
