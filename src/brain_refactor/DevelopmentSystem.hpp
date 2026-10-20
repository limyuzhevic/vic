#pragma once

#include "../development/DevelopmentSystem.hpp"
#include "ISubsystem.hpp"
#include <memory>

namespace nlm {

class DevelopmentSystem : public ISubsystem {
public:
    DevelopmentSystem();
    ~DevelopmentSystem() override;
    
    // ISubsystem interface
    bool initialize(Brain* brain) override;
    void update(float dt) override;
    void reset() override;
    void logStatus() const override;
    
    // Developmental stage management
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    bool isInCriticalPeriod() const;
    bool isInMaturationPhase() const;
    
    // Development timeline
    void advanceDevelopment();
    void triggerDevelopmentalEvent(DevelopmentalStage stage);
    size_t getCurrentAgeInSteps() const;
    float getMaturationProgress() const;
    
    // Plasticity regulation
    float getPlasticityModulation() const;
    void updatePlasticityRegulation();
    void applyDevelopmentEffectsToPlasticity();
    
    // Synaptogenesis and pruning
    void triggerSynaptogenesis();
    void triggerPruning();
    void updateSynaptogenesisRate(float rate);
    void updatePruningRate(float rate);
    float getSynaptogenesisRate() const;
    float getPruningRate() const;
    
    // Neural refinement
    void refineConnections();
    void optimizeConnectivity();
    void establishLongRangeConnections();
    
    // Development statistics
    size_t getSynaptogenesisCount() const;
    size_t getPruningCount() const;
    size_t getConnectionOptimizationCount() const;
    float getDevelopmentalStability() const;
    
    // Configuration
    void setDevelopmentSpeed(float speed);
    void setCriticalPeriodDuration(size_t steps);
    void setMaturationDuration(size_t steps);
    
    // Development milestones
    bool hasReachedMaturity() const;
    bool isDevelopmentComplete() const;
    void resetDevelopmentProgress();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
