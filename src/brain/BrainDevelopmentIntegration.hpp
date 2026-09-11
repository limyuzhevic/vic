#pragma once

// BrainDevelopmentIntegration.hpp - Developmental system integration
// Contains developmental system integration functionality

#include "BrainCore.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <memory>

namespace nlm {

// Forward declarations
class BrainCore;

// Brain Development Integration Implementation
class BrainDevelopmentIntegration {
public:
    struct Impl;
    Impl* pImpl;
    
    // Create brain development integration with reference to core brain
    explicit BrainDevelopmentIntegration(BrainCore* core);
    
    ~BrainDevelopmentIntegration();
    
    // Disable copying, enable moving
    BrainDevelopmentIntegration(const BrainDevelopmentIntegration&) = delete;
    BrainDevelopmentIntegration& operator=(const BrainDevelopmentIntegration&) = delete;
    BrainDevelopmentIntegration(BrainDevelopmentIntegration&&) noexcept;
    BrainDevelopmentIntegration& operator=(BrainDevelopmentIntegration&&) noexcept;
    
    // Initialize development system
    bool initialize();
    
    // Update development system
    void update(BrainCore* brain, RandomGenerator& rng, float timeDelta);
    
    // Get development system
    DevelopmentSystem* getDevelopmentSystem();
    
    // Get developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Set developmental stage
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Calculate plasticity modulation based on developmental stage
    float calculatePlasticityModulation(DevelopmentalStage stage) const;
    
    // Clear development state
    void clear();
    
    // Get statistics
    size_t getDevelopmentCycles() const;
    float getDevelopmentalProgress() const;
    
private:
    // Internal helper methods
    void updateDevelopmentStage();
    void updateSynaptogenesis();
    void updatePruning();
    void updateMaturation();
    void applyDevelopmentEffects();
    
    // Utility methods
    void calculateDevelopmentStats() const;
    
    struct Impl {
        BrainCore* brainCore;
        
        // Development system reference
        DevelopmentSystem* developmentSystem;
        
        // Development state
        DevelopmentalStage currentStage;
        size_t developmentCycles;
        float developmentalProgress;
        
        Impl(BrainCore* core);
    };
};

} // namespace nlm

