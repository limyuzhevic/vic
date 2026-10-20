#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <memory>

namespace nlm {

class DevelopmentManager {
public:
    DevelopmentManager(std::shared_ptr<Config> config);
    ~DevelopmentManager();
    
    // Initialize development system
    bool initialize(Brain* brain);
    
    // Update developmental effects
    void update(Timestamp time, TimestepDuration timestep);
    
    // Reset developmental state
    void reset();
    
    // Get component access
    DevelopmentSystem* getDevelopmentSystem() { return developmentSystem.get(); }
    
    // Development statistics
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Configuration methods
    void setDevelopmentalTiming(float criticalPeriod, float maturation) {
        criticalPeriodTiming = criticalPeriod;
        maturationTiming = maturation;
    }
    
    void enablePruning(bool enable) { pruningEnabled = enable; }
    void enableSynaptogenesis(bool enable) { synaptogenesisEnabled = enable; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    float criticalPeriodTiming;
    float maturationTiming;
    bool pruningEnabled;
    bool synaptogenesisEnabled;
};

} // namespace nlm