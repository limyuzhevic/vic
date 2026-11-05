#pragma once

#include "../development/DevelopmentSystem.hpp"
#include "../development/Synaptogenesis.hpp"
#include "../development/Pruning.hpp"
#include "../development/Maturation.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <memory>
#include <vector>

namespace nlm {

class Brain;

class BrainDevelopment {
public:
    BrainDevelopment() = default;
    ~BrainDevelopment() = default;
    
    // Configuration and setup
    void configureFromConfig(const Config* config);
    void setDevelopmentParameters(float developmentRate, float criticalPeriodThreshold, float maturationSpeed);
    
    // Development updates (called during step)
    void updateAllDevelopment(Brain* brain, TimestepDuration timestep, Timestamp currentTime);
    void updateDevelopmentalStage(Brain* brain, Timestamp currentTime);
    void updateSynaptogenesis(Brain* brain, RandomGenerator& rng);
    void updatePruning(Brain* brain, RandomGenerator& rng);
    void updateMaturation(Brain* brain, Timestamp currentTime);
    
    // Development access and control
    DevelopmentSystem* getDevelopmentSystem() { return developmentSystem_.get(); }
    Synaptogenesis* getSynaptogenesis() { return synaptogenesis_.get(); }
    Pruning* getPruning() { return pruning_.get(); }
    Maturation* getMaturation() { return maturation_.get(); }
    
    // Development statistics and diagnostics
    void collectDevelopmentStats(Brain* brain);
    float getDevelopmentRate() const { return developmentRate_; }
    float getCriticalPeriodThreshold() const { return criticalPeriodThreshold_; }
    float getMaturationSpeed() const { return maturationSpeed_; }
    DevelopmentalStage getCurrentStage() const { return currentStage_; }
    
    // Development system management
    void resetDevelopment(Brain* brain);
    void enableDevelopment(Brain* brain, bool enable);
    void setDevelopmentalStage(Brain* brain, DevelopmentalStage stage);
    void updateDevelopmentParameters(Brain* brain, float plasticityModulation);
    
    // Development-specific processing
    void processDevelopmentalMaturation(Brain* brain, Timestamp currentTime);
    void processCriticalPeriod(Brain* brain);
    void processSynapticCompetition(Brain* brain, RandomGenerator& rng);
    void processExperienceIndependentDevelopment(Brain* brain, Timestamp currentTime);
    
private:
    std::unique_ptr<DevelopmentSystem> developmentSystem_;
    std::unique_ptr<Synaptogenesis> synaptogenesis_;
    std::unique_ptr<Pruning> pruning_;
    std::unique_ptr<Maturation> maturation_;
    
    // Development statistics
    float developmentRate_ = 0.01f;
    float criticalPeriodThreshold_ = 0.5f;
    float maturationSpeed_ = 0.1f;
    DevelopmentalStage currentStage_ = DevelopmentalStage::Initial;
    
    // Configuration parameters
    float synaptogenesisRate_ = 0.0001f;
    float pruningRate_ = 0.00001f;
    float maturationRate_ = 0.001f;
    size_t criticalPeriodStart_ = 1000;
    size_t criticalPeriodEnd_ = 5000;
    float developmentalPlasticityMod_ = 1.0f;
    
    // Helper methods
    void initializeDevelopmentSystems(Brain* brain);
    void validateDevelopmentState(Brain* brain) const;
    void updateDevelopmentConnections(Brain* brain);
    void processDevelopmentLearning(Brain* brain, TimestepDuration timestep);
    DevelopmentalStage calculateDevelopmentalStage(Timestamp currentTime) const;
    
    friend class BrainCore; // For internal access
};

} // namespace nlm