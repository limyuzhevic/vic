// Development System Integration for NLM Brain Loop
// This file implements enhanced integration of development systems into the main brain loop
// to enable comprehensive developmental processes that affect all brain functions

#pragma once

#include "Brain.hpp"
#include "development/DevelopmentSystem.hpp"
#include "development/Synaptogenesis.hpp"
#include "development/Pruning.hpp"
#include "development/Maturation.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include <memory>
#include <vector>
#include <chrono>
#include <cmath>
#include <algorithm>

namespace nlm {

// Enhanced Development Integration Configuration
struct DevelopmentIntegrationConfig {
    // Core development parameters
    bool enable_development = true;
    float developmental_time_scale = 1.0f;
    float critical_period_duration = 60.0f;
    float maturation_age = 300.0f;
    float aging_threshold = 900.0f;
    
    // Developmental stage effects
    bool enable_stage_specific_effects = true;
    float initial_plasticity_modifier = 1.0f;
    float critical_period_plasticity_modifier = 0.8f;
    float maturation_plasticity_modifier = 0.5f;
    float adult_plasticity_modifier = 0.2f;
    
    // Structural development
    bool enable_structural_development = true;
    float synaptogenesis_rate = 0.0001f;
    float pruning_rate = 0.00001f;
    float structural_plasticity_decay = 0.9f;
    bool enable_connectivity_optimization = true;
    
    // Functional development
    bool enable_functional_maturation = true;
    float prediction_system_maturation_rate = 0.8f;
    float memory_system_maturation_rate = 0.7f;
    float cognitive_system_maturation_rate = 0.6f;
    float attention_system_maturation_rate = 0.5f;
    
    // Neuromodulation development
    bool enable_neuromodulation_maturation = true;
    float dopamine_system_maturation = 0.9f;
    float curiosity_system_maturation = 0.8f;
    float novelty_system_maturation = 0.7f;
    float prediction_error_maturation = 0.6f;
    
    // Learning and adaptation development
    bool enable_learning_capacity_development = true;
    float working_memory_capacity_growth = 0.3f;
    float episodic_memory_capacity_growth = 0.2f;
    float semantic_memory_capacity_growth = 0.4f;
    float prediction_accuracy_improvement = 0.5f;
    float concept_complexity_growth = 0.3f;
    
    // Development monitoring
    bool enable_development_monitoring = true;
    float development_progress_threshold = 0.7f;
    bool enable_development_checkpointing = true;
    float developmental_replay_rate = 0.8f;
    
    // Environment interaction development
    bool enable_environment_interaction_development = true;
    float exploration_capacity_growth = 0.4f;
    float exploitation_efficiency_growth = 0.3f;
    float curiosity_growth_rate = 0.5f;
    float novelty_response_growth = 0.4f;
    
    // Development cross-system integration
    bool enable_cross_system_development = true;
    float memory_prediction_interaction = 0.6f;
    float cognition_memory_interaction = 0.5f;
    float neuromodulation_cognition_interaction = 0.4f;
    float development_behavior_interaction = 0.7f;
    
    // Developmental plasticity
    bool enable_developmental_plasticity = true;
    float developmental_learning_rate = 0.02f;
    float critical_period_sensitivity = 2.0f;
    float maturity_plasticity_sensitivity = 0.5f;
    float adult_stability_factor = 0.3f;
    
    // Developmental specialization
    bool enable_specialization = true;
    float sensory_specialization_rate = 0.3f;
    float motor_specialization_rate = 0.4f;
    float cognitive_specialization_rate = 0.5f;
    float social_specialization_rate = 0.2f;
};

// Enhanced Development-Integrated Brain
class DevelopmentIntegratedBrain {
public:
    DevelopmentIntegratedBrain(std::shared_ptr<Brain> brain,
                               std::shared_ptr<DevelopmentIntegrationConfig> config = nullptr);
    ~DevelopmentIntegratedBrain();
    
    // Disable copying, enable moving
    DevelopmentIntegratedBrain(const DevelopmentIntegratedBrain&) = delete;
    DevelopmentIntegratedBrain& operator=(const DevelopmentIntegratedBrain&) = delete;
    DevelopmentIntegratedBrain(DevelopmentIntegratedBrain&&) noexcept;
    DevelopmentIntegratedBrain& operator=(DevelopmentIntegratedBrain&&) noexcept;
    
    // Initialize with development system
    bool initialize();
    
    // Enhanced simulation step with development
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Development system operations
    void updateDevelopmentSystem();
    void updateDevelopmentalStages();
    void applyStructuralDevelopment();
    void applyFunctionalDevelopment();
    void applyNeuromodulationDevelopment();
    void applyLearningCapacityDevelopment();
    void applyEnvironmentInteractionDevelopment();
    
    // Integration with memory systems
    void integrateDevelopmentWithWorkingMemory();
    void integrateDevelopmentWithEpisodicMemory();
    void integrateDevelopmentWithSemanticMemory();
    
    // Integration with prediction systems
    void integrateDevelopmentWithPrediction();
    void applyDevelopmentToPredictionAccuracy();
    void applyDevelopmentToPredictionLearning();
    
    // Integration with cognition systems
    void integrateDevelopmentWithCognition();
    void applyDevelopmentToAttention();
    void applyDevelopmentToPlanning();
    void applyDevelopmentToConceptFormation();
    
    // Integration with neuromodulation
    void integrateDevelopmentWithNeuromodulation();
    void applyDevelopmentToDopamineSystem();
    void applyDevelopmentToCuriositySystem();
    void applyDevelopmentToNoveltySystem();
    
    // Development cross-system integration
    void integrateDevelopmentCrossSystems();
    void optimizeCrossSystemDevelopment();
    void coordinateDevelopmentEffects();
    
    // Developmental plasticity
    void updateDevelopmentalPlasticity();
    void adaptDevelopmentalLearningRates();
    void maintainDevelopmentalBalance();
    
    // Developmental specialization
    void applyDevelopmentalSpecialization();
    void developSensorySpecialization();
    void developMotorSpecialization();
    void developCognitiveSpecialization();
    void developSocialSpecialization();
    
    // Development monitoring and evaluation
    void monitorDevelopmentProgress();
    void evaluateDevelopmentalOutcomes();
    void trackDevelopmentalMilestones();
    
    // Getters
    class DevelopmentSystem* getDevelopmentSystem();
    class Synaptogenesis* getSynaptogenesis();
    class Pruning* getPruning();
    class Maturation* getMaturation();
    
    // Development statistics
    float getDevelopmentalProgress() const;
    DevelopmentalStage getCurrentDevelopmentalStage() const;
    float getCurrentPlasticityModifier() const;
    float getDevelopmentalAge() const;
    size_t getStructuralDevelopmentCount() const;
    size_t getFunctionalDevelopmentCount() const;
    
    // Development metrics
    struct DevelopmentMetrics {
        double developmental_age = 0.0;
        double developmental_progress = 0.0;
        double plasticity_modifier = 1.0;
        double critical_period_progress = 0.0;
        double maturation_progress = 0.0;
        double aging_progress = 0.0;
        
        double working_memory_capacity = 0.0;
        double episodic_memory_capacity = 0.0;
        double semantic_memory_capacity = 0.0;
        double prediction_accuracy = 0.0;
        double concept_complexity = 0.0;
        double attention_span = 0.0;
        
        double dopamine_function = 0.0;
        double curiosity_function = 0.0;
        double novelty_function = 0.0;
        double prediction_error_function = 0.0;
        
        double exploration_capacity = 0.0;
        double exploitation_efficiency = 0.0;
        double specialization_level = 0.0;
        double cross_system_integration = 0.0;
        
        double developmental_stability = 0.0;
        double developmental_resilience = 0.0;
        double developmental_flexibility = 0.0;
        double developmental_adaptability = 0.0;
    };
    
    DevelopmentMetrics getDevelopmentMetrics() const;
    
    // Configuration management
    void updateConfiguration(const DevelopmentIntegrationConfig& config);
    DevelopmentIntegrationConfig getConfiguration() const;
    
    // Checkpoint and persistence
    bool saveDevelopmentState(const std::string& filepath);
    bool loadDevelopmentState(const std::string& filepath);
    
    // Monitoring and debugging
    void logDevelopmentStatus() const;
    void printDevelopmentMetrics() const;
    void visualizeDevelopmentProgress() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Development Integration Manager
class DevelopmentIntegrationManager {
public:
    DevelopmentIntegrationManager(std::shared_ptr<DevelopmentIntegratedBrain> development_brain);
    ~DevelopmentIntegrationManager();
    
    // Update all development integration components
    void update(double timestep);
    
    // Coordinate development with other systems
    void coordinateDevelopmentWithMemory();
    void coordinateDevelopmentWithPrediction();
    void coordinateDevelopmentWithCognition();
    void coordinateDevelopmentWithNeuromodulation();
    
    // Development learning and adaptation
    void adaptDevelopmentalParameters();
    void learnFromDevelopmentalProgress();
    void optimizeDevelopmentalStrategies();
    
    // Developmental specialization and optimization
    void optimizeDevelopmentalSpecialization();
    void enhanceDevelopmentalCrossSystems();
    void maintainDevelopmentalBalance();
    
    // Development monitoring and evaluation
    void evaluateDevelopmentalOutcomes();
    void trackDevelopmentalMilestones();
    void assessDevelopmentalQuality();
    
    // Performance optimization
    void optimizeDevelopmentalPerformance();
    void manageDevelopmentalResources();
    void scaleDevelopmentalOperations();
    
    // Error handling and recovery
    void handleDevelopmentErrors();
    void recoverFromDevelopmentFailures();
    void validateDevelopmentIntegrity();
    
    // Configuration management
    void updateConfiguration(const DevelopmentIntegrationConfig& config);
    DevelopmentIntegrationConfig getConfiguration() const;
    
    // Monitoring and statistics
    double getDevelopmentalProgress() const;
    double getSpecializationLevel() const;
    double getCrossSystemIntegration() const;
    double getDevelopmentalEfficiency() const;
    double getIntegrationQuality() const;
    
    // Event handling
    void handleDevelopmentEvent(const std::string& event_type, const std::string& data);
    void registerDevelopmentEventCallback(const std::string& event_type,
                                         std::function<void(const std::string&)> callback);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Development Integration Utilities
namespace DevelopmentIntegrationUtils {
    // Developmental stage assessment
    float assessDevelopmentalStage(float age, float critical_period_end, float maturation_end);
    float calculatePlasticityModifier(float age, float critical_period_end, float maturation_end);
    float calculateStageProgress(float age, float stage_duration);
    
    // Structural development algorithms
    void applySynaptogenesis(class Synaptogenesis& synaptogenesis,
                             float age, float plasticity_modifier);
    void applyPruning(class Pruning& pruning,
                      float age, float plasticity_modifier);
    void applyMaturation(class Maturation& maturation,
                        float age, float plasticity_modifier);
    
    // Functional development algorithms
    void developWorkingMemory(class WorkingMemory& working_memory,
                             float age, float developmental_progress);
    void developEpisodicMemory(class EpisodicMemory& episodic_memory,
                              float age, float developmental_progress);
    void developSemanticMemory(class SemanticMemory& semantic_memory,
                              float age, float developmental_progress);
    
    // Neuromodulation development algorithms
    void developDopamineSystem(class Dopamine& dopamine,
                              float age, float developmental_progress);
    void developCuriositySystem(class Curiosity& curiosity,
                               float age, float developmental_progress);
    void developNoveltySystem(class Novelty& novelty,
                             float age, float developmental_progress);
    void developPredictionErrorSystem(class PredictionError& prediction_error,
                                     float age, float developmental_progress);
    
    // Cross-system development integration
    void integrateMemoryWithPrediction(class WorkingMemory& working_memory,
                                      class PredictionSystem& prediction,
                                      float age);
    void integratePredictionWithCognition(class PredictionSystem& prediction,
                                          class NeuralPlanner& planner,
                                          float age);
    void integrateCognitionWithMemory(class NeuralPlanner& planner,
                                      class ConceptFormation& concept_formation,
                                      class WorkingMemory& working_memory,
                                      float age);
    void integrateNeuromodulationWithCognition(class Dopamine& dopamine,
                                               class NeuralPlanner& planner,
                                               float age);
    
    // Developmental plasticity algorithms
    void adaptLearningRates(class DevelopmentSystem& development,
                           float age, float environment_complexity);
    void maintainDevelopmentalBalance(class DevelopmentSystem& development,
                                      float plasticity_modifier);
    void enhanceDevelopmentalResilience(class DevelopmentSystem& development,
                                        float age, float stress_level);
    
    // Developmental specialization algorithms
    void specializeSensorySystems(class DevelopmentSystem& development,
                                float age, float environment_stimulus);
    void specializeMotorSystems(class DevelopmentSystem& development,
                              float age, float activity_level);
    void specializeCognitiveSystems(class DevelopmentSystem& development,
                                   float age, float cognitive_demand);
    void specializeSocialSystems(class DevelopmentSystem& development,
                                float age, float social_interaction);
    
    // Developmental milestone assessment
    bool assessDevelopmentalMilestones(float age, const DevelopmentMetrics& metrics);
    float calculateDevelopmentalReadiness(float age, const DevelopmentMetrics& metrics);
    float assessDevelopmentalMaturity(float age, const DevelopmentMetrics& metrics);
    
    // Developmental intervention
    void interveneInDevelopment(class DevelopmentSystem& development,
                              const std::string& intervention_type,
                              float intervention_strength);
    void supportDevelopmentalChallenges(class DevelopmentSystem& development,
                                       const std::string& challenge_type,
                                       float challenge_severity);
}

// Development Integration Configuration Presets
namespace DevelopmentIntegrationPresets {
    DevelopmentIntegrationConfig createDefaultConfig();
    DevelopmentIntegrationConfig createHighPerformanceConfig();
    DevelopmentIntegrationConfig createAccuracyFocusedConfig();
    DevelopmentIntegrationConfig createBalancedConfig();
    DevelopmentIntegrationConfig createExperimentalConfig();
    DevelopmentIntegrationConfig createResearchConfig();
}

// Development Integration Testing Framework
class DevelopmentIntegrationTester {
public:
    DevelopmentIntegrationTester(std::shared_ptr<DevelopmentIntegratedBrain> development_brain);
    ~DevelopmentIntegrationTester();
    
    // Test development integration functionality
    bool testDevelopmentSystemIntegration();
    bool testStructuralDevelopment();
    bool testFunctionalDevelopment();
    bool testNeuromodulationDevelopment();
    bool testLearningCapacityDevelopment();
    bool testEnvironmentInteractionDevelopment();
    
    // Test cross-system development integration
    bool testMemoryDevelopmentIntegration();
    bool testPredictionDevelopmentIntegration();
    bool testCognitionDevelopmentIntegration();
    bool testNeuromodulationDevelopmentIntegration();
    
    // Test developmental quality and outcomes
    bool testDevelopmentalQuality();
    bool testDevelopmentalProgress();
    bool testDevelopmentalMilestones();
    bool testDevelopmentalOutcomes();
    bool testDevelopmentalStability();
    
    // Test performance and efficiency
    bool testDevelopmentalPerformance();
    bool testDevelopmentalEfficiency();
    bool testDevelopmentalResourceUsage();
    bool testDevelopmentalRealTimeConstraints();
    
    // Generate test reports
    std::string generateTestReport() const;
    void saveTestReport(const std::string& filepath) const;
    void printTestReport() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
