#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

#include "NeuromodulationConfig.hpp"
#include "Neuromodulator.hpp"

namespace nlm {

class NeuromodulationController {
public:
    NeuromodulationController(const NeuromodulationConfig& config);
    ~NeuromodulationController();
    
    // Register a neuromodulator factory
    void registerNeuromodulator(const std::string& name,
        std::function<std::unique_ptr<Neuromodulator>(const NeuromodulationConfig&)> factory);
    
    // Initialize controller with brain reference
    void initialize(class Brain* brain);
    
    // Update all neuromodulators
    void update(double timestep);
    
    // Signal reward to neuromodulators
    void signalReward(float reward);
    
    // Signal prediction error
    void signalPredictionError(float error);
    
    // Get neuromodulator level by name
    float getNeuromodulatorLevel(const std::string& name) const;
    
    // Get dopamine level (convenience method)
    float getDopamineLevel() const;
    
    // Get novelty level (convenience method)
    float getNoveltyLevel() const;
    
    // Get curiosity level (convenience method)
    float getCuriosityLevel() const;
    
    // Get prediction error (convenience method)
    float getPredictionError() const;
    
    // Get plasticity factor (weighted average of neuromodulators)
    float getPlasticityFactor() const;
    
    // Reset all neuromodulators
    void reset();
    
    // Enable/disable neuromodulation
    void enableNeuromodulation(bool enabled);
    bool isNeuromodulationEnabled() const;
    
    // Get current configuration
    const NeuromodulationConfig& getConfig() const;
    
    // Update configuration
    void updateConfig(const NeuromodulationConfig& config);
    
    // Check if a neuromodulator exists
    bool hasNeuromodulator(const std::string& name) const;
    
    // Get list of registered neuromodulator names
    std::vector<std::string> getNeuromodulatorNames() const;
    
    // Add specific neuromodulator instances
    void addNeuromodulator(std::unique_ptr<Neuromodulator> neuromodulator);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm