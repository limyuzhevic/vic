#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace nlm {

// NeuromodulatorManager: Manages all neuromodulators and their interactions
// Provides integration with Brain for neuromodulation effects
class NeuromodulatorManager {
public:
    NeuromodulatorManager();
    ~NeuromodulatorManager();
    
    // Initialize all neuromodulators
    void initialize(class Brain* brain);
    
    // Update all neuromodulators
    void updateAll(TimestepDuration dt);
    
    // Add neuromodulator to the system
    void addModulator(Neuromodulator* modulator);
    
    // Get specific neuromodulator by name
    Neuromodulator* getModulator(const std::string& name);
    const Neuromodulator* getModulator(const std::string& name) const;
    
    // Get composite neuromodulation level
    float getCompositeNeuromodulationLevel() const;
    
    // Get neuromodulator levels for decision making
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    float getNeuromodulationLevel() const;
    
    // Get composite neuromodulation for brain
    float getTotalNeuromodulation() const;
    
    // Apply neuromodulators to brain
    void applyToBrain(class Brain* brain);
    
    // Enable/disable specific neuromodulators
    void enableModulator(const std::string& name, bool enable);
    bool isModulatorEnabled(const std::string& name) const;
    
    // Reset all neuromodulators
    void resetAll();
    
    // Get all active neuromodulator names
    std::vector<std::string> getActiveModulatorNames() const;
    
    // Get neuromodulator stats
    struct NeuromodulatorStats {
        std::string name;
        float level;
        float plasticityFactor;
        bool enabled;
    };
    
    std::vector<NeuromodulatorStats> getAllStats() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm