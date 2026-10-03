// DevelopmentSystem.h - Brain development and maturation system

#pragma once

#include "BrainImpl.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <memory>
#include <chrono>

namespace nlm {

class DevelopmentSystem {
public:
    DevelopmentSystem();
    ~DevelopmentSystem() = default;
    
    // Initialize with brain reference
    void initialize(BrainImpl* brain);
    
    // Core development update
    void update(double timestep, double simulationTime);
    
    // Developmental stage management
    DevelopmentalStage getCurrentStage() const { return currentStage_; }
    double getDevelopmentalAge() const { return developmentalAge_; }
    bool isInCriticalPeriod() const { return currentStage_ == DevelopmentalStage::CriticalPeriod; }
    bool isInMaturationPhase() const { return currentStage_ == DevelopmentalStage::Maturation; }
    
    // Plasticity regulation
    void regulatePlasticity();
    float getPlasticityMultiplier() const { return plasticityMultiplier_; }
    float getSynaptogenesisRate() const { return synaptogenesisRate_; }
    float getPruningRate() const { return pruningRate_; }
    
    // Structural development
    void triggerDevelopmentEvent(DevelopmentEvent event);
    void updateStructuralConnectivity();
    void optimizeNeuralTopology();
    
    // Learning capacity management
    void increaseLearningCapacity();
    void decreaseLearningCapacity();
    size_t getCurrentCapacity() const { return learningCapacity_; }
    
    // Configuration
    void setCriticalPeriodDuration(double duration) { criticalPeriodDuration_ = duration; }
    void setMaturationDuration(double duration) { maturationDuration_ = duration; }
    void setPlasticityDecayRate(float rate) { plasticityDecayRate_ = rate; }
    
    // Statistics
    float getDevelopmentProgress() const { return developmentProgress_; }
    double getTimeInCurrentStage() const { return timeInCurrentStage_; }
    size_t getConnectionCount() const { return connectionCount_; }
    
    // Configuration
    void configure(double criticalPeriodDuration, double maturationDuration,
                   float plasticityDecayRate, size_t initialCapacity);
    
private:
    BrainImpl* brain_ = nullptr;
    
    // Development subsystem
    std::unique_ptr<DevelopmentSystemImpl> developmentSystem_;
    
    // Developmental state
    DevelopmentalStage currentStage_;
    double developmentalAge_;
    double timeInCurrentStage_;
    float developmentProgress_;
    
    // Developmental timing
    double criticalPeriodDuration_;
    double maturationDuration_;
    double adultStartTime_;
    
    // Plasticity regulation
    float plasticityMultiplier_;
    float synaptogenesisRate_;
    float pruningRate_;
    float plasticityDecayRate_;
    
    // Structural development
    size_t learningCapacity_;
    size_t connectionCount_;
    std::vector<std::pair<RegionId, RegionId>> structuralConnections_;
    
    // Event management
    std::queue<DevelopmentEvent> pendingEvents_;
    std::chrono::steady_clock::time_point lastEventTime_;
    
    // Helper methods
    void updateDevelopmentalStage(double simulationTime);
    void updatePlasticityRegulation();
    void manageStructuralChanges();
    void processDevelopmentEvents();
    void updateLearningCapacity(double simulationTime);
    void maintainStructuralConnections();
    void optimizeNetworkTopology();
    void resetStageProgress();
    void scheduleNextDevelopmentEvent();
    
    friend class BrainImpl;
};

} // namespace nlm
