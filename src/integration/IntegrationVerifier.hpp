#pragma once

#include "../performance/PerformanceMonitor.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <mutex>

namespace nlm {

/**
 * IntegrationVerification Framework for NLM brain integration
 * 
 * Comprehensive validation of all NLM integration points:
 * - Brain-Memory integration (NeuralWorkingMemory, NeuralEpisodicMemory)
 * - Neuromodulation-Plasticity integration (all neuromodulators affecting plasticity)
 * - Development-Plasticity integration (developmental stage effects on plasticity)
 * - Cross-system validation and health checks
 * 
 * Provides detailed logging, validation error reporting, and integration statistics.
 */

class IntegrationVerifier {
public:
    struct IntegrationResult {
        bool success;
        std::string message;
        std::unordered_map<std::string, std::string> metadata;
        std::chrono::milliseconds validationTime;
    };

    struct IntegrationReport {
        std::vector<IntegrationResult> validations;
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
        std::chrono::milliseconds totalValidationTime;
        size_t totalValidations;
        size_t successfulValidations;
        
        std::string toString() const;
        bool hasErrors() const { return !errors.empty(); }
        bool isValid() const { return !hasErrors(); }
    };

    IntegrationVerifier();
    ~IntegrationVerifier();
    
    // Disable copying
    IntegrationVerifier(const IntegrationVerifier&) = delete;
    IntegrationVerifier& operator=(const IntegrationVerifier&) = delete;
    
    // Initialize verifier with brain reference
    void initialize(Brain* brain);
    
    // High-priority integration verifications
    IntegrationResult verifyBrainMemoryIntegration();
    IntegrationResult verifyNeuromodulationPlasticityIntegration();
    IntegrationResult verifyDevelopmentPlasticityIntegration();
    
    // Performance integration verifications
    IntegrationResult verifyPerformanceIntegration();
    IntegrationResult verifyResourceUsageIntegration();
    IntegrationResult verifyOptimizationIntegration();
    IntegrationResult verifyMonitoringIntegration();
    
    // Additional integration verifications
    IntegrationResult verifyPredictionMemoryIntegration();
    IntegrationResult verifyCognitionMemoryIntegration();
    IntegrationResult verifySensoryMemoryIntegration();
    IntegrationResult verifyMotorCognitionIntegration();
    IntegrationResult verifyWorldEnvironmentIntegration();
    IntegrationResult verifyDevelopmentBrainIntegration();
    IntegrationResult verifyNeuromodulationBrainIntegration();
    
    // Cross-system validations
    IntegrationResult verifySystemConnectivity();
    IntegrationResult verifyPlasticityConsistency();
    IntegrationResult verifyNeuromodulationBalance();
    IntegrationResult verifyMemoryCapacity();
    
    // Global integration verification
    IntegrationReport verifyAllIntegrations();
    
    // Get current integration status
    const IntegrationReport& getCurrentReport() const { return currentReport_; }
    
    // Reset and reinitialize
    void reset();
    
    // Enable/disable specific validations
    void enableValidation(const std::string& validationName, bool enabled = true);
    bool isValidationEnabled(const std::string& validationName) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Current verification report
    IntegrationReport currentReport_;
    
    // Configuration
    std::unordered_map<std::string, bool> validationEnabled_;
    bool loggingEnabled_;
    bool detailedReportingEnabled_;
    
    // Validation helper methods
    IntegrationResult validateBrainMemorySystem() const;
    IntegrationResult validateNeuromodulationEffects() const;
    IntegrationResult validateDevelopmentEffects() const;
    IntegrationResult validateCrossSystemConnections() const;
    IntegrationResult validateResourceAllocation() const;
    
    // Statistics and logging
    void logValidationResult(const std::string& validationName, const IntegrationResult& result);
    void generateIntegrationStatistics(IntegrationReport& report) const;
    
    // Helper classes and utilities
    class ValidationTimer {
    public:
        ValidationTimer() : start_(std::chrono::high_resolution_clock::now()) {}
        std::chrono::milliseconds elapsed() const {
            return std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - start_);
        }
    private:
        std::chrono::high_resolution_clock::time_point start_;
    };
};

} // namespace nlm
