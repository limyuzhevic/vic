#pragma once

#include "Development.hpp"
#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>

namespace nlm {

class DevelopmentalStageManager {
public:
    DevelopmentalStageManager();
    ~DevelopmentalStageManager();
    
    // Update stage progression based on age
    void update(Brain* brain, double age, SimulationStep currentStep);
    
    // Get current stage
    DevelopmentalStage getCurrentStage() const { return currentStage; }
    
    // Get stage-specific characteristics
    float getStagePlasticity() const;
    float getStageSynapticDensity() const;
    float getStageLearningRate() const;
    float getStageCriticalPeriod() const;
    
    // Get stage duration thresholds
    static double getStageDuration(DevelopmentalStage stage);
    
    // Get stage name with full description
    const char* getStageDescription(DevelopmentalStage stage) const;
    
private:
    struct Impl;
    Impl* pImpl;
    DevelopmentalStage currentStage;
};

} // namespace nlm