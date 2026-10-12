#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// PLACEHOLDER - Phase 2 will implement real neuromodulation effects

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // TODO PHASE 2: Implement real modulation
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Forward declarations for concrete neuromodulators
class Dopamine;
class Acetylcholine;
class Norepinephrine;
class Serotonin;

// NeuromodulatorManager: Manages all neuromodulators
class NeuromodulatorManager {
public:
    NeuromodulatorManager();
    ~NeuromodulatorManager();
    
    // Add a neuromodulator
    void addNeuromodulator(const std::string& name, Neuromodulator* modulator);
    
    // Get neuromodulator by name
    Neuromodulator* getNeuromodulator(const std::string& name);
    const Neuromodulator* getNeuromodulator(const std::string& name) const;
    
    // Get all neuromodulator levels
    std::unordered_map<std::string, float> getAllLevels() const;
    
    // Update all neuromodulators
    void updateAll(TimestepDuration dt);
    
    // Update neuromodulators for specific functions
    void updateForAttention(float novelty, float predictionError);
    void updateForMemoryConsolidation(float reward);
    
    // Get individual neuromodulator levels
    float getAcetylcholine() const;
    float getNorepinephrine() const;
    float getSerotonin() const;
    float getDopamine() const;
    
    // Set individual neuromodulator levels
    void setAcetylcholine(float level);
    void setNorepinephrine(float level);
    void setSerotonin(float level);
    void setDopamine(float level);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
