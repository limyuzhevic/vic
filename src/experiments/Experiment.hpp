#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <memory>
#include <vector>

namespace nlm {

// Experiment: Records data from a single experimental run
class Experiment {
public:
    Experiment();
    Experiment(const std::string& name);
    ~Experiment();
    
    // Experiment metadata
    const std::string& getName() const;
    void setName(const std::string& name);
    
    // Random seed
    uint64_t getSeed() const;
    void setSeed(uint64_t seed);
    
    // Configuration
    void setConfigValue(const std::string& key, const std::string& value);
    std::string getConfigValue(const std::string& key) const;
    
    // Metrics
    void recordMetric(const std::string& name, double value);
    void recordMetric(const std::string& name, float value);
    void recordMetric(const std::string& name, int value);
    
    // Get all metrics
    const std::vector<std::pair<std::string, double>>& getMetrics() const;
    
    // Get metric value
    double getMetric(const std::string& name) const;
    
    // Check if metric exists
    bool hasMetric(const std::string& name) const;
    
    // Recording step data
    void recordStep(SimulationStep step, size_t neuronCount, size_t synapseCount,
                   size_t firingCount, float avgFiringRate);
    
    // Get step data
    struct StepData {
        SimulationStep step;
        size_t neuronCount;
        size_t synapseCount;
        size_t firingCount;
        float avgFiringRate;
    };
    const std::vector<StepData>& getStepData() const;
    
    // Duration
    SimulationStep getStartStep() const;
    void setStartStep(SimulationStep step);
    SimulationStep getEndStep() const;
    void setEndStep(SimulationStep step);
    
    // Notes
    void addNote(const std::string& note);
    const std::vector<std::string>& getNotes() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
