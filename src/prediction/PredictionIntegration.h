// Prediction System Integration for NLM Brain Loop
// This file implements the integration of prediction systems into the main brain loop
// to enable forward prediction and temporal sequence learning

#pragma once

#include "Brain.hpp"
#include "prediction/PredictionSystem.hpp"
#include "prediction/NeuralPrediction.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "memory/WorkingMemory.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include <memory>
#include <vector>
#include <chrono>
#include <cmath>

namespace nlm {

// Prediction Integration Configuration
struct PredictionIntegrationConfig {
    // Prediction system parameters
    bool enable_prediction = true;
    float prediction_learning_rate = 0.001f;
    float prediction_error_threshold = 0.1f;
    float prediction_confidence_threshold = 0.7f;
    size_t prediction_history_size = 100;
    float prediction_decay_rate = 0.95f;
    
    // Temporal processing
    bool enable_temporal_processing = true;
    size_t prediction_horizon = 10;  // Number of steps to predict ahead
    float temporal_window_size = 50.0f;  // Time window for temporal processing
    bool enable_sequence_learning = true;
    
    // Integration with other systems
    bool enable_prediction_to_working_memory = true;
    bool enable_prediction_to_planning = true;
    bool enable_prediction_error_to_plasticity = true;
    bool enable_prediction_confidence_to_attention = true;
    
    // Prediction error processing
    bool enable_prediction_error_processing = true;
    float prediction_error_learning_rate = 0.01f;
    float prediction_error_decay_rate = 0.9f;
    bool enable_sustained_prediction_error = false;  // Long-term prediction errors
    
    // Forward models
    bool enable_forward_models = true;
    float forward_model_prediction_weight = 0.6f;
    float inverse_model_weight = 0.4f;
    
    // Active inference
    bool enable_active_inference = true;
    float precision_weighting = 1.0f;
    float action_selection_precision = 1.0f;
    float belief_update_rate = 0.1f;
    
    // Prediction monitoring
    bool enable_prediction_monitoring = true;
    float prediction_accuracy_threshold = 0.8f;
    bool enable_prediction_failure_recovery = true;
    float recovery_learning_rate = 0.05f;
    
    // Prediction validation
    bool enable_prediction_validation = true;
    float validation_horizon = 5.0f;  // How far ahead to validate predictions
    float validation_tolerance = 0.2f;
};

// Prediction Integration Brain
class PredictionIntegratedBrain {
public:
    PredictionIntegratedBrain(std::shared_ptr<Brain> brain,
                             std::shared_ptr<PredictionIntegrationConfig> config = nullptr);
    ~PredictionIntegratedBrain();
    
    // Disable copying, enable moving
    PredictionIntegratedBrain(const PredictionIntegratedBrain&) = delete;
    PredictionIntegratedBrain& operator=(const PredictionIntegratedBrain&) = delete;
    PredictionIntegratedBrain(PredictionIntegratedBrain&&) noexcept;
    PredictionIntegratedBrain& operator=(PredictionIntegratedBrain&&) noexcept;
    
    // Initialize with prediction system
    bool initialize();
    
    // Enhanced simulation step with prediction
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Prediction system operations
    void updatePredictionSystem();
    void processPredictionErrors();
    void generatePredictions();
    void evaluatePredictionAccuracy();
    
    // Integration with memory systems
    void integratePredictionWithWorkingMemory();
    void storePredictionInMemory();
    void retrievePredictionsFromMemory();
    
    // Integration with planning systems
    void integratePredictionWithPlanning();
    void updatePlanningBasedOnPredictions();
    void selectActionsBasedOnPredictions();
    
    // Prediction error processing
    void computePredictionErrors();
    void applyPredictionErrorLearning();
    void modulatePlasticityBasedOnPredictionError();
    
    // Neuromodulation integration
    void integratePredictionWithNeuromodulation();
    void applyPredictionConfidenceToAttention();
    void applyPredictionErrorToMotivation();
    
    // Forward and inverse models
    void updateForwardModels();
    void updateInverseModels();
    void maintainInternalModels();
    
    // Active inference
    void performActiveInference();
    void selectExpected actions();
    void updateBeliefsBasedOnActions();
    
    // Prediction validation and recovery
    void validatePredictions();
    void recoverFromPredictionFailures();
    void improvePredictionAccuracy();
    
    // Prediction monitoring
    void monitorPredictionPerformance();
    void logPredictionMetrics();
    void analyzePredictionQuality();
    
    // Getters
    class PredictionSystem* getPredictionSystem();
    class NeuralPrediction* getNeuralPrediction();
    class PredictionError* getPredictionError();
    
    // Prediction statistics
    size_t getPredictionHistorySize() const;
    float getPredictionAccuracy() const;
    float getAveragePredictionConfidence() const;
    size_t getPredictionErrorCount() const;
    float getPredictionLearningProgress() const;
    
    // Prediction metrics
    struct PredictionMetrics {
        double total_predictions = 0;
        double successful_predictions = 0;
        double prediction_accuracy = 0.0;
        double average_confidence = 0.0;
        double average_prediction_error = 0.0;
        double prediction_error_variance = 0.0;
        double learning_rate_achieved = 0.0;
        double prediction_stability = 0.0;
        double temporal_prediction_accuracy = 0.0;
        double forward_model_accuracy = 0.0;
        double inverse_model_accuracy = 0.0;
        double active_inference_accuracy = 0.0;
    };
    
    PredictionMetrics getPredictionMetrics() const;
    
    // Configuration management
    void updateConfiguration(const PredictionIntegrationConfig& config);
    PredictionIntegrationConfig getConfiguration() const;
    
    // Checkpoint and persistence
    bool savePredictionState(const std::string& filepath);
    bool loadPredictionState(const std::string& filepath);
    
    // Monitoring and debugging
    void logPredictionStatus() const;
    void printPredictionMetrics() const;
    void visualizePredictions() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Prediction Integration Manager
class PredictionIntegrationManager {
public:
    PredictionIntegrationManager(std::shared_ptr<PredictionIntegratedBrain> prediction_brain);
    ~PredictionIntegrationManager();
    
    // Update all prediction integration components
    void update(double timestep);
    
    // Coordinate prediction with other systems
    void coordinatePredictionWithMemory();
    void coordinatePredictionWithPlanning();
    void coordinatePredictionWithNeuromodulation();
    
    // Prediction learning and adaptation
    void adaptPredictionModels();
    void learnFromPredictionErrors();
    void optimizePredictionParameters();
    
    // Prediction validation and recovery
    void validatePredictionsContinuously();
    void recoverFromPredictionFailures();
    void maintainPredictionRobustness();
    
    // Performance optimization
    void optimizePredictionPerformance();
    void managePredictionResources();
    void scalePredictionOperations();
    
    // Error handling and recovery
    void handlePredictionErrors();
    void recoverFromPredictionErrors();
    void validatePredictionIntegrity();
    
    // Configuration management
    void updateConfiguration(const PredictionIntegrationConfig& config);
    PredictionIntegrationConfig getConfiguration() const;
    
    // Monitoring and statistics
    size_t getTotalPredictionsMade() const;
    float getOverallPredictionAccuracy() const;
    size_t getActivePredictionModels() const;
    float getPredictionSystemEfficiency() const;
    float getIntegrationQuality() const;
    
    // Event handling
    void handlePredictionEvent(const std::string& event_type, const std::string& data);
    void registerPredictionEventCallback(const std::string& event_type,
                                        std::function<void(const std::string&)> callback);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Prediction Integration Utilities
namespace PredictionIntegrationUtils {
    // Prediction error computation
    float computePredictionError(const class NeuralPrediction& prediction,
                                const class SensoryInput& actual_input,
                                const class SensoryInput& predicted_input);
    float computeTemporalPredictionError(const class NeuralPrediction& prediction,
                                        const class NeuralWorkingMemory& memory,
                                        size_t horizon);
    float computeForwardModelError(const class NeuralPrediction& prediction,
                                  const class SensoryInput& current_input);
    float computeInverseModelError(const class NeuralPrediction& prediction,
                                  const class Action& predicted_action,
                                  const class Action& actual_action);
    
    // Prediction learning algorithms
    void learnForwardModel(class NeuralPrediction& prediction,
                          const class SensoryInput& current_input,
                          const class SensoryInput& next_input);
    void learnInverseModel(class NeuralPrediction& prediction,
                          const class SensoryInput& current_input,
                          const class Action& action,
                          const class SensoryInput& next_input);
    void learnSequenceModel(class NeuralPrediction& prediction,
                           const class NeuralWorkingMemory& memory);
    
    // Prediction confidence and uncertainty
    float computePredictionConfidence(const class NeuralPrediction& prediction);
    float computePredictionUncertainty(const class NeuralPrediction& prediction);
    float computePredictiveEntropy(const class NeuralPrediction& prediction);
    
    // Active inference algorithms
    void performBayesianUpdating(class NeuralPrediction& prediction,
                               const class SensoryInput& sensory_input,
                               const class Action& action,
                               float precision);
    float computeActionSelectionValue(const class NeuralPrediction& prediction,
                                    const class Action& action);
    class Action selectExpectedAction(const class NeuralPrediction& prediction);
    
    // Prediction validation
    bool validatePrediction(const class NeuralPrediction& prediction,
                           const class SensoryInput& actual_input,
                           float tolerance);
    float computePredictionValidationScore(const class NeuralPrediction& prediction,
                                         const class SensoryInput& actual_input);
    
    // Prediction recovery
    void recoverFromPredictionFailure(class NeuralPrediction& prediction,
                                     const class SensoryInput& error_signal,
                                     float recovery_rate);
    void improvePredictionAccuracy(class NeuralPrediction& prediction,
                                  const class NeuralWorkingMemory& memory,
                                  float improvement_rate);
    
    // Integration with other systems
    void integratePredictionWithMemory(class NeuralPrediction& prediction,
                                      class NeuralWorkingMemory& working_memory);
    void integratePredictionWithPlanning(class NeuralPrediction& prediction,
                                        class NeuralPlanner& planner);
    void integratePredictionWithNeuromodulation(class NeuralPrediction& prediction,
                                                class Neuromodulator& neuromodulator);
}

// Prediction Integration Configuration Presets
namespace PredictionIntegrationPresets {
    PredictionIntegrationConfig createDefaultConfig();
    PredictionIntegrationConfig createHighAccuracyConfig();
    PredictionIntegrationConfig createHighSpeedConfig();
    PredictionIntegrationConfig createBalancedConfig();
    PredictionIntegrationConfig createExperimentalConfig();
    PredictionIntegrationConfig createResearchConfig();
}

// Prediction Integration Testing Framework
class PredictionIntegrationTester {
public:
    PredictionIntegrationTester(std::shared_ptr<PredictionIntegratedBrain> prediction_brain);
    ~PredictionIntegrationTester();
    
    // Test prediction integration functionality
    bool testPredictionSystemIntegration();
    bool testPredictionErrorProcessing();
    bool testTemporalPredictionLearning();
    bool testForwardModelLearning();
    bool testInverseModelLearning();
    bool testActiveInference();
    bool testPredictionValidation();
    bool testPredictionRecovery();
    
    // Test integration with other systems
    bool testIntegrationWithMemory();
    bool testIntegrationWithPlanning();
    bool testIntegrationWithNeuromodulation();
    
    // Test performance and efficiency
    bool testPredictionAccuracy();
    bool testPredictionSpeed();
    bool testMemoryUsage();
    bool testComputationalEfficiency();
    bool testRealTimeConstraints();
    
    // Generate test reports
    std::string generateTestReport() const;
    void saveTestReport(const std::string& filepath) const;
    void printTestReport() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
