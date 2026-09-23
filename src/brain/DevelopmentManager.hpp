#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../core/Random/Random.hpp"
#include <memory>
#include <string>

namespace nlm {

// Development system status
struct DevelopmentStatus {
    DevelopmentalStage currentStage{DevelopmentalStage::Initial};
    double developmentalAge{0.0};
    bool criticalPeriodActive{false};
    float criticalPeriodProgress{0.0f};
    float plasticityModifier{1.0f};
    
    // Check for consistency
    bool isConsistent() const {
        return developmentalAge >= 0.0 &&
               criticalPeriodProgress >= 0.0f && criticalPeriodProgress <= 1.0f &&
               plasticityModifier >= 0.0f && plasticityModifier <= 2.0f;
    }
    
    std::string toString() const {
        std::string stageStr;
        switch (currentStage) {
            case DevelopmentalStage::Initial: stageStr = "Initial"; break;
            case DevelopmentalStage::CriticalPeriod: stageStr = "CriticalPeriod"; break;
            case DevelopmentalStage::Maturation: stageStr = "Maturation"; break;
            case DevelopmentalStage::Adult: stageStr = "Adult"; break;
        }
        return "Stage: " + stageStr +
               ", Age: " + std::to_string(developmentalAge) +
               ", CriticalPeriod: " + std::to_string(criticalPeriodActive) +
               ", Progress: " + std::to_string(criticalPeriodProgress) +
               ", Plasticity: " + std::to_string(plasticityModifier);
    }
};

class DevelopmentManager {
public:
    DevelopmentManager();
    ~DevelopmentManager();
    
    // Initialize development system
    bool initialize(Brain* brain);
    
    // Update development system
    void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Get development status
    DevelopmentStatus getStatus() const { return status_; }
    
    // Stage management
    DevelopmentalStage getStage() const { return status_.currentStage; }
    void setStage(DevelopmentalStage stage);
    
    // Development effects
    void applyDevelopmentEffects(Brain* brain, float plasticityModifier);
    
    // Configuration validation
    bool validateConfiguration(std::shared_ptr<Config> config) const;
    
    // Development system access
    DevelopmentSystem* getDevelopmentSystem() { return developmentSystem.get(); }
    
    // State management
    void advanceStage();
    void setDevelopmentalAge(double age) { status_.developmentalAge = age; }
    
private:
    struct Impl;
    Impl* pImpl;
    DevelopmentStatus status_;
};
