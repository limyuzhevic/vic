// Phase 6 Development System Enhancement

/**
 * Enhanced development system for Phase 6 integration
 * 
 * This enhancement provides comprehensive development integration
 * across multiple brain systems: neural excitability, attention,
 * memory consolidation, neuromodulation, and learning.
 */

#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <functional>
#include <vector>

namespace nlm {

// Enhanced development stages with additional effects
enum class EnhancedDevelopmentalStage {
    Initial,           // High plasticity, rapid learning
    CriticalPeriod,   // Sensitive period for sensory-motor integration
    Maturation,       // Fine-tuning and optimization
    Adult,            // Stable, optimized performance
    Aging            // Gradual decline, maintenance
};

/**
 * Enhanced development system for Phase 6
 * 
 * Provides comprehensive development integration across:
 * - Neural excitability modulation
 * - Attention system development
 * - Memory consolidation effects
 * - Neuromodulation level development
 * - Learning rate modulation
 * - Behavioral pattern stabilization
 */
class DevelopmentSystem {
public:
    DevelopmentSystem();
    ~DevelopmentSystem();
    
    // === Stage Management ===
    
    // Get current developmental stage
    EnhancedDevelopmentalStage getEnhancedStage() const;
    void setEnhancedStage(EnhancedDevelopmentalStage stage);
    
    // Advance developmental stage
    void advanceEnhancedStage();
    
    // Get stage name
    const char* getEnhancedStageName() const;
    
    // === Development Effects ===
    
    // Update development with comprehensive effects
    void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Apply development effects to all brain systems
    void applyDevelopmentEffects(Brain* brain);
    
    // === Neural Excitability Modulation ===
    
    // Get neural excitability modifier for current stage
    float getNeuralExcitabilityModifier() const;
    
    // Apply neural excitability effects
    void updateNeuralExcitability(Brain* brain);
    
    // === Attention System Integration ===
    
    // Get attention modifier for current stage
    float getAttentionModifier() const;
    
    // Update attention system based on developmental stage
    void updateAttentionSystem(Brain* brain);
    
    // === Memory Consolidation Effects ===
    
    // Get memory consolidation modifier for current stage
    float getMemoryConsolidationModifier() const;
    
    // Update memory consolidation based on developmental stage
    void updateMemoryConsolidation(Brain* brain);
    
    // === Neuromodulation Effects ===
    
    // Get neuromodulation level modifier for current stage
    float getNeuromodulationModifier() const;
    
    // Apply neuromodulation effects
    void updateNeuromodulationSystem(Brain* brain);
    
    // === Learning Rate Modulation ===
    
    // Get learning rate modifier for current stage
    float getLearningRateModifier() const;
    
    // Apply learning rate effects
    void updateLearningRates(Brain* brain);
    
    // === Behavioral Pattern Stabilization ===
    
    // Get behavioral stability level for current stage
    float getBehavioralStability() const;
    
    // Apply behavioral stabilization effects
    void stabilizeBehaviorPatterns(Brain* brain);
    
    // === Integration with Attention System ===
    
    // Update development based on attentional focus
    void updateBasedOnAttention(const std::vector<NeuronId>& attendedNeurons);
    
    // === Integration with Memory System ===
    
    // Update development based on memory consolidation
    void updateBasedOnMemory(const std::vector<const EpisodicMemoryItem*>& episodes);
    
    // === Integration with Neuromodulation ===
    
    // Update development based on neuromodulation levels
    void updateBasedOnNeuromodulation(float dopamineLevel, float curiosityLevel, float noveltyLevel);
    
    // === Development Stage Information ===
    
    // Get developmental age (in simulation time)
    double getDevelopmentalAge() const;
    
    // Set developmental age
    void setDevelopmentalAge(double age);
    
    // Get stage age (time in current stage)
    double getStageAge() const;
    
    // Get time in current stage (steps)
    SimulationStep getStepsInCurrentStage() const;
    
    // Get stage progression percentage
    float getStageProgression() const;
    
    // === Development Planning ===
    
    // Check if development should advance to next stage
    bool shouldAdvanceStage() const;
    
    // Get stage advancement threshold
    double getStageAdvancementThreshold() const;
    
    // Set stage advancement threshold
    void setStageAdvancementThreshold(double threshold);
    
    // === Development Parameters ===
    
    // Get development configuration
    const DevelopmentConfig& getConfig() const;
    
    // Set development configuration
    void setConfig(const DevelopmentConfig& config);
    
    // Reset development to initial state
    void resetDevelopment();
    
    // Clone development state
    std::unique_ptr<DevelopmentSystem> clone() const;
    
private:
    // Core development state
    struct Impl {
        EnhancedDevelopmentalStage stage;
        SimulationStep stageStartStep;
        SimulationStep stepsInCurrentStage;
        double stageAge;  // Age since entering current stage
        double developmentalAge_;
        double stageAdvancementThreshold_;
        DevelopmentConfig config_;
        
        Impl() : stage(EnhancedDevelopmentalStage::Initial), 
                stageStartStep(0), 
                stepsInCurrentStage(0), 
                stageAge(0.0), 
                developmentalAge_(0.0), 
                stageAdvancementThreshold_(1000.0) {}
    };
    
    std::unique_ptr<Impl> pImpl;
    DevelopmentConfig config_;
};

/**
 * Development configuration parameters
 */
struct DevelopmentConfig {
    // Stage duration thresholds (in simulation time)
    double initialStageDuration;
    double criticalPeriodDuration;
    double maturationDuration;
    
    // Plasticity modifiers for each stage
    float initialPlasticity;
    float criticalPeriodPlasticity;
    float maturationPlasticity;
    float adultPlasticity;
    float agingPlasticity;
    
    // Excitability modifiers for each stage
    float initialExcitability;
    float criticalPeriodExcitability;
    float maturationExcitability;
    float adultExtability;
    float agingExcitability;
    
    // Attention modulation for each stage
    float initialAttention;
    float criticalPeriodAttention;
    float maturationAttention;
    float adultAttention;
    float agingAttention;
    
    // Memory consolidation for each stage
    float initialMemoryConsolidation;
    float criticalPeriodMemoryConsolidation;
    float maturationMemoryConsolidation;
    float adultMemoryConsolidation;
    float agingMemoryConsolidation;
    
    // Neuromodulation for each stage
    float initialNeuromodulation;
    float criticalPeriodNeuromodulation;
    float maturationNeuromodulation;
    float adultNeuromodulation;
    float agingNeuromodulation;
    
    // Learning rate for each stage
    float initialLearningRate;
    float criticalPeriodLearningRate;
    float maturationLearningRate;
    float adultLearningRate;
    float agingLearningRate;
    
    // Behavioral stability for each stage
    float initialBehavioralStability;
    float criticalPeriodBehavioralStability;
    float maturationBehavioralStability;
    float adultBehavioralStability;
    float agingBehavioralStability;
    
    // Developmental parameters
    DevelopmentConfig()
        : initialStageDuration(60.0)
        , criticalPeriodDuration(300.0)
        , maturationDuration(600.0)
        , initialPlasticity(1.0f)
        , criticalPeriodPlasticity(0.8f)
        , maturationPlasticity(0.5f)
        , adultPlasticity(0.2f)
        , agingPlasticity(0.1f)
        , initialExcitability(1.5f)
        , criticalPeriodExcitability(1.2f)
        , maturationExcitability(1.0f)
        , adultExcitability(0.8f)
        , agingExcitability(0.6f)
        , initialAttention(0.8f)
        , criticalPeriodAttention(0.6f)
        , maturationAttention(0.4f)
        , adultAttention(0.2f)
        , agingAttention(0.1f)
        , initialMemoryConsolidation(0.5f)
        , criticalPeriodMemoryConsolidation(0.4f)
        , maturationMemoryConsolidation(0.3f)
        , adultMemoryConsolidation(0.2f)
        , agingMemoryConsolidation(0.1f)
        , initialNeuromodulation(1.0f)
        , criticalPeriodNeuromodulation(0.9f)
        , maturationNeuromodulation(0.8f)
        , adultNeuromodulation(0.7f)
        , agingNeuromodulation(0.6f)
        , initialLearningRate(1.0f)
        , criticalPeriodLearningRate(0.9f)
        , maturationLearningRate(0.8f)
        , adultLearningRate(0.7f)
        , agingLearningRate(0.6f)
        , initialBehavioralStability(0.9f)
        , criticalPeriodBehavioralStability(0.8f)
        , maturationBehavioralStability(0.7f)
        , adultBehavioralStability(0.6f)
        , agingBehavioralStability(0.5f) {}
};

} // namespace nlm

} // namespace nlm
