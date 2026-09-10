// DevelopmentComponent.h - Development system for neural maturation
#pragma once

#include "BrainComponentBase.h"
#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

class DevelopmentSystem;
class Brain;

class DevelopmentComponent : public BrainComponentBase {
public:
    DevelopmentComponent();
    ~DevelopmentComponent() override;
    
    // Initialize component with brain reference
    bool initialize(Brain* brain) override;
    
    // Update component state for current timestep
    void update(const TimestepDuration& dt) override;
    
    // Reset component to initial state
    void reset() override;
    
    // Log component status
    void logStatus() const override;
    
    // Get component name
    const char* getName() const override { return "DevelopmentComponent"; }
    
    // Access to development system
    DevelopmentSystem* getDevelopmentSystem() { return developmentSystem_.get(); }
    
    // Development methods
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    float getPlasticityModifier() const;
    bool isCriticalPeriod() const;
    float getCriticalPeriodProgress() const;
    
    // Advance developmental stage
    void advanceStage();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::unique_ptr<DevelopmentSystem> developmentSystem_;
};

} // namespace nlm
