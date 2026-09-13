#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <vector>

namespace nlm {

class Brain;
class RandomGenerator;
class Config;

class BrainDevelopment {
public:
    // Forward declaration for friendship
    friend class Brain;
    
    BrainDevelopment() = default;
    ~BrainDevelopment() = default;
    
    // Development stage management
    void update(const class Brain* brain, RandomGenerator& rng, TimestepDuration timestep);
    void setStage(DevelopmentalStage stage);
    DevelopmentalStage getStage() const;
    
    // Plasticity modulation based on development
    float getPlasticityModulation() const;
    float calculatePlasticityModulation() const;
    
    // Developmental effects on neural properties
    float getExcitabilityModulation() const;
    float getMaturationModulation() const;
    
    // Critical period management
    bool isCriticalPeriod() const;
    void enterCriticalPeriod();
    void exitCriticalPeriod();
    
    // Activity-dependent development
    void processActivityDependentDevelopment(const class Brain* brain);
    void promoteMaturation();
    
private:
    // Development stage and progression
    DevelopmentalStage developmentalStage;
    size_t stepsSinceStageChange;
    size_t criticalPeriodStartStep;
    size_t criticalPeriodEndStep;
    
    // Plasticity modulation factors
    float excitabilityModulation;
    float maturationModulation;
    float learningRateModulation;
    
    // Developmental thresholds and parameters
    static constexpr size_t CRITICAL_PERIOD_DURATION = 5000;
    static constexpr size_t MATURATION_DURATION = 10000;
    static constexpr float CRITICAL_PERIOD_PLASTICITY_MULTIPLIER = 0.8f;
    static constexpr float MATURATION_PLASTICITY_MULTIPLIER = 0.5f;
    static constexpr float ADULT_PLASTICITY_MULTIPLIER = 0.2f;
};

} // namespace nlm