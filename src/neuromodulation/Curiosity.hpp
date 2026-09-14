#pragma once

#include "NeuromodulatorBase.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

// Curiosity drive: exploration motivation based on novelty and prediction error
// Computed as weighted combination of novelty and prediction error

class Curiosity : public NeuromodulatorBase {
public:
    Curiosity();
    ~Curiosity() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain) override;
    
    // Get curiosity level
    float getLevel() const override;
    
    // Override setLevel from base
    void setLevel(float level) override;
    
    // Update curiosity based on novelty and prediction error
    void update(float novelty, float predictionError, TimestepDuration dt);
    
    // Get exploration drive (same as level)
    float getExplorationDrive() const;
    
    // Get neuromodulator type
    NeuromodulatorType getType() const override;
    
    // Load configuration parameters
    void loadConfig(const std::string& key, float value) override;
    
    // Set curiosity parameters
    void setNoveltyWeight(float weight);
    void setPredictionErrorWeight(float weight);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
