#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

// Development configuration parameters
struct DevelopmentConfig {
    // Developmental stages and timing
    float initialStageDuration_ = 60.0f;     // Duration of Initial stage (seconds)
    float criticalPeriodDuration_ = 240.0f;  // Duration of CriticalPeriod stage (seconds)
    float maturationDuration_ = 600.0f;      // Duration of Maturation stage (seconds)
    
    // Plasticity parameters
    float initialPlasticity_ = 1.0f;         // Plasticity modifier in Initial stage
    float criticalPeriodPlasticity_ = 0.8f;  // Plasticity modifier in CriticalPeriod
    float maturationPlasticity_ = 0.5f;      // Plasticity modifier in Maturation
    float adultPlasticity_ = 0.2f;           // Plasticity modifier in Adult stage
    
    // Synaptogenesis rates
    float initialSynaptogenesis_ = 0.0001f; // Synaptogenesis rate in initial stage
    float criticalPeriodSynaptogenesis_ = 0.0002f;
    float maturationSynaptogenesis_ = 0.00005f;
    float adultSynaptogenesis_ = 0.00001f;
    
    // Pruning rates
    float initialPruning_ = 0.00001f;       // Pruning rate in initial stage
    float criticalPeriodPruning_ = 0.000005f;
    float maturationPruning_ = 0.00002f;
    float adultPruning_ = 0.00003f;
    
    // Structural plasticity
    float structuralPlasticityRate_ = 0.1f;   // Rate of structural changes
    bool enableStructuralPlasticity_ = true; // Enable structural plasticity
    
    // Activity-dependent plasticity
    float activityDependentPlasticity_ = 0.5f; // Weight of activity in plasticity
    float homeostaticPlasticity_ = 0.3f;      // Weight of homeostasis in plasticity
    
    // Configuration flags
    bool enableDevelopment_ = true;           // Enable developmental processes
    bool enableStructuralPlasticityChanges_ = true; // Enable structural changes with development
    bool enableActivityDependentPlasticity_ = true; // Enable Hebbian-like learning
};

// Development event structure
struct DevelopmentEvent {
    double timestamp;           // When event occurred
    DevelopmentalStage fromStage;    // Previous developmental stage
    DevelopmentalStage toStage;      // New developmental stage
    float plasticityModifier;    // New plasticity modifier
    std::string description;    // Human-readable description
    
    DevelopmentEvent() : timestamp(0.0), fromStage(DevelopmentalStage::Initial),
                         toStage(DevelopmentalStage::Initial), plasticityModifier(0.0f),
                         description("") {}
};

// DevelopmentManager: Handles developmental processes and plasticity changes
// Separates development logic from AgentBrain for better modularity
class DevelopmentManager {
public:
    DevelopmentManager(const DevelopmentConfig& config = DevelopmentConfig());
    ~DevelopmentManager();
    
    // Configure development manager
    void configure(const DevelopmentConfig& config) { config_ = config; }
    
    // Update developmental state
    void update(double timestep, bool enableDevelopment = true);
    
    // Process developmental stage transition
    void processStageTransition();
    
    // Calculate current plasticity modifier
    float calculatePlasticityModifier() const;
    
    // Get current developmental stage
    DevelopmentalStage getCurrentStage() const { return currentStage_; }
    
    // Get developmental age (time since start)
    double getDevelopmentalAge() const { return developmentalAge_; }
    
    // Get plasticity modifier
    float getPlasticityModifier() const { return plasticityModifier_; }
    
    // Get developmental events (for debugging)
    const std::vector<DevelopmentEvent>& getEvents() const { return events_; }
    
    // Apply developmental effects to neural systems
    void applyToSTDP(STDP* stdp) const;
    void applyToStructuralPlasticity(StructuralPlasticity* sp) const;
    void applyToHebbian(Hebbian* hebbian) const;
    
    // Get stage duration based on stage
    double getStageDuration(DevelopmentalStage stage) const;
    
    // Check if development is complete (adult stage)
    bool isDevelopmentComplete() const;
    
    // Reset developmental state
    void reset();
    
    // Validate state
    bool isValid() const;
    
    // Get configuration
    const DevelopmentConfig& getConfig() const { return config_; }
    
    // Enable/disable development
    void enableDevelopment(bool enable) { config_.enableDevelopment_ = enable; }
    void enableStructuralPlasticity(bool enable) { config_.enableStructuralPlasticity_ = enable; }
    
    bool isDevelopmentEnabled() const { return config_.enableDevelopment_; }
    bool isStructuralPlasticityEnabled() const { return config_.enableStructuralPlasticity_; }
    
    // Add custom developmental stage transition
    void addCustomTransition(double age, DevelopmentalStage fromStage,
                             DevelopmentalStage toStage, float plasticityModifier,
                             const std::string& description = "");
    
private:
    // Developmental stage timing
    double getTimeInCurrentStage() const;
    
    // Stage-specific calculations
    void applyStageSpecificEffects(DevelopmentalStage stage) const;
    
    // Event management
    void addEvent(DevelopmentalStage from, DevelopmentalStage to, float plasticity,
                  const std::string& desc);
    
    // State variables
    double developmentalAge_;           // Age in seconds
    DevelopmentalStage currentStage_;    // Current developmental stage
    float plasticityModifier_;           // Current plasticity modifier (0.0-2.0)
    double stageStartTime_;              // When current stage started
    
    // Developmental events
    std::vector<DevelopmentEvent> events_;
    
    // Configuration
    DevelopmentConfig config_;
    
    // Custom transitions for testing/advanced use
    std::vector<DevelopmentEvent> customTransitions_;
};

} // namespace nlm