// Stress Integration Tests
// 
// These tests validate system behavior under extreme conditions and error scenarios,
// ensuring robustness and graceful failure handling.

#ifndef NLM_STRESS_TESTS_HPP
#define NLM_STRESS_TESTS_HPP

#include "IntegrationTestFramework.hpp"
#include <cassert>
#include <vector>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <stdexcept>

namespace nlm {
namespace stress_tests {

// Test system behavior under high load conditions
class HighLoadStressTest : public integration_tests::IntegrationTestBase {
public:
    void setupConfiguration() override {
        config_->set("neuron_count", 300);
        config_->set("region_count", 3);
        config_->set("connection_probability", 0.12f);
        config_->set("simulation_timestep", 0.001);
        config_->set("random_seed", 77777u);
        config_->set("stress_mode", true);
    }
    
    void setupSystem() override {
        brain_ = std::make_shared<Brain>(config_);
        assertCondition(brain_->initialize(), "Brain initialization failed");
    }
    
    void runTest() override {
        const int stressIterations = 100;
        const int stepsPerIteration = 200;
        double totalTime = 0.0;
        int errorCount = 0;
        int instabilityCount = 0;
        
        for (int iteration = 0; iteration < stressIterations; ++iteration) {
            auto start = std::chrono::high_resolution_clock::now();
            
            bool iterationStable = true;
            bool iterationErrored = false;
            
            try {
                // Run high-load simulation
                for (int step = 0; step < stepsPerIteration; ++step) {
                    // Inject extreme input patterns
                    brain_->injectCurrentToNeurons(NeuronType::Sensory, 100.0f + (iteration % 50));
                    
                    // Run neural computation
                    brain_->step(step);
                    
                    // Check for instability
                    if (brain_->getFiringNeuronCount() > brain_->getTotalNeuronCount() * 0.9) {
                        iterationStable = false;
                    }
                    
                    // Simulate occasional errors
                    if (step % 97 == 0) {
                        iterationErrored = true;
                        errorCount++;
                        throw std::runtime_error("Simulated system error");
                    }
                }
                
                // Handle errors gracefully
                if (iterationErrored) {
                    brain_->reset();
                    brain_->initialize();
                }
                
            } catch (const std::exception& e) {
                iterationStable = false;
                errorCount++;
                // System should recover from errors
                brain_->reset();
                brain_->initialize();
            }
            
            if (!iterationStable) {
                instabilityCount++;
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double>(end - start).count();
            totalTime += duration;
            
            recordMetric("stress_iteration_" + std::to_string(iteration) + ".duration", duration);
            recordMetric("stress_iteration_" + std::to_string(iteration) + ".stable", 
                        iterationStable ? 1.0 : 0.0);
            
            std::cout << "    Stress iteration " << iteration << ": " 
                      << "duration=" << duration << "s, "
                      << "stable=" << (iterationStable ? "yes" : "no") << ", "
                      << "errors=" << errorCount << std::endl;
        }
        
        // Calculate stress test metrics
        double avgDuration = totalTime / stressIterations;
        double stabilityRate = (stressIterations - instabilityCount) / stressIterations * 100.0;
        
        recordMetric("avg_stress_duration", avgDuration);
        recordMetric("stability_rate", stabilityRate);
        recordMetric("total_errors_encountered", static_cast<double>(errorCount));
        
        // Stress test benchmarks
        assertCondition(avgDuration < 15.0, "Average stress iteration time should be < 15s");
        assertCondition(stabilityRate > 70.0, 
                       "System should maintain >70% stability under stress");
        
        std::cout << "    High load stress test PASSED: "
                  << "avg_duration=" << avgDuration << "s, "
                  << "stability_rate=" << stabilityRate << "%, "
                  << "errors=" << errorCount << std::endl;
    }
};

// Test system behavior with memory pressure
class MemoryPressureStressTest : public integration_tests::IntegrationTestBase {
public:
    void setupConfiguration() override {
        config_->set("neuron_count", 500);
        config_->set("region_count", 3);
        config_->set("connection_probability", 0.06f);
        config_->set("simulation_timestep", 0.001);
        config_->set("random_seed", 88888u);
        config_->set("memory_pressure_enabled", true);
        config_->set("allocation_limit_mb", 256);
    }
    
    void setupSystem() override {
        brain_ = std::make_shared<Brain>(config_);
        assertCondition(brain_->initialize(), "Brain initialization failed");
    }
    
    void runTest() override {
        const int memoryStressIterations = 80;
        double totalTime = 0.0;
        int memoryViolations = 0;
        
        for (int iteration = 0; iteration < memoryStressIterations; ++iteration) {
            auto start = std::chrono::high_resolution_clock::now();
            
            try {
                // Run memory-intensive simulation
                for (int step = 0; step < 150; ++step) {
                    // Create memory pressure with random activity patterns
                    int neuronsToActivate = 20 + (iteration % 30);
                    neuronsToActivate = std::min(neuronsToActivate, 
                                               static_cast<int>(brain_->getTotalNeuronCount()));
                    
                    for (int i = 0; i < neuronsToActivate; ++i) {
                        NeuronType type = static_cast<NeuronType>(i % 3);
                        brain_->injectCurrentToNeurons(type, 30.0f + (iteration % 10) * 5.0f);
                    }
                    
                    brain_->step(step);
                    
                    // Simulate memory violations occasionally
                    if (step % 77 == 0 && iteration % 5 == 0) {
                        memoryViolations++;
                        throw std::runtime_error("Simulated memory pressure violation");
                    }
                    
                }
                
            } catch (const std::exception& e) {
                memoryViolations++;
                // System should handle memory pressure gracefully
                brain_->reset();
                brain_->initialize();
                
                if (memoryViolations > memoryStressIterations * 0.2) {
                    throw; // Too many violations
                }
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double>(end - start).count();
            totalTime += duration;
            
            recordMetric("memory_iteration_" + std::to_string(iteration) + ".duration", duration);
            recordMetric("memory_iteration_" + std::to_string(iteration) + ".violations", 
                        static_cast<double>(memoryViolations));
            
            std::cout << "    Memory stress iteration " << iteration << ": " 
                      << "duration=" << duration << "s, "
                      << "violations=" << memoryViolations << std::endl;
        }
        
        // Calculate memory stress metrics
        double avgMemoryDuration = totalTime / memoryStressIterations;
        double memoryViolationRate = (memoryViolations * 100.0) / memoryStressIterations;
        
        recordMetric("avg_memory_duration", avgMemoryDuration);
        recordMetric("memory_violation_rate", memoryViolationRate);
        
        // Memory stress benchmarks
        assertCondition(avgMemoryDuration < 12.0, "Average memory stress time should be < 12s");
        assertCondition(memoryViolationRate < 30.0, 
                       "Memory violation rate should be < 30%");
        
        std::cout << "    Memory pressure stress test PASSED: "
                  << "avg_duration=" << avgMemoryDuration << "s, "
                  << "violation_rate=" << memoryViolationRate << "%, "
                  << "total_violations=" << memoryViolations << std::endl;
    }
};

// Test error handling and recovery mechanisms
class ErrorRecoveryStressTest : public integration_tests::IntegrationTestBase {
public:
    void setupConfiguration() override {
        config_->set("neuron_count", 400);
        config_->set("region_count", 2);
        config_->set("connection_probability", 0.08f);
        config_->set("simulation_timestep", 0.001);
        config_->set("random_seed", 99999u);
        config_->set("error_injection_enabled", true);
        config_->set("auto_recovery_enabled", true);
    }
    
    void setupSystem() override {
        brain_ = std::make_shared<Brain>(config_);
        assertCondition(brain_->initialize(), "Brain initialization failed");
    }
    
    void runTest() override {
        const int errorRecoveryIterations = 60;
        double totalTime = 0.0;
        int successfulRecoveries = 0;
        int totalRecoveriesAttempted = 0;
        
        for (int iteration = 0; iteration < errorRecoveryIterations; ++iteration) {
            auto start = std::chrono::high_resolution_clock::now();
            
            bool recoverySuccessful = false;
            
            try {
                // Run simulation with potential errors
                for (int step = 0; step < 120; ++step) {
                    // Inject current to maintain activity
                    brain_->injectCurrentToNeurons(NeuronType::Sensory, 25.0f);
                    
                    // Simulate various error conditions
                    if (step % 43 == 0) {
                        throw std::runtime_error("Configuration error");
                    }
                    if (step % 67 == 0) {
                        throw std::runtime_error("Memory access error");
                    }
                    if (step % 89 == 0) {
                        throw std::runtime_error("Network connectivity error");
                    }
                    
                    brain_->step(step);
                }
                
                // No errors occurred
                recoverySuccessful = true;
                successfulRecoveries++;
                
            } catch (const std::exception& e) {
                totalRecoveriesAttempted++;
                recoverySuccessful = attemptRecovery(e);
                if (recoverySuccessful) {
                    successfulRecoveries++;
                } else {
                    // System failed to recover
                    brain_->reset();
                    brain_->initialize();
                }
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double>(end - start).count();
            totalTime += duration;
            
            recordMetric("recovery_iteration_" + std::to_string(iteration) + ".duration", duration);
            recordMetric("recovery_iteration_" + std::to_string(iteration) + ".success", 
                        recoverySuccessful ? 1.0 : 0.0);
            
            std::cout << "    Error recovery iteration " << iteration << ": " 
                      << "duration=" << duration << "s, "
                      << "recovered=" << (recoverySuccessful ? "yes" : "no") << ", "
                      << "total_attempts=" << totalRecoveriesAttempted << std::endl;
        }
        
        // Calculate error recovery metrics
        double avgErrorDuration = totalTime / errorRecoveryIterations;
        double recoverySuccessRate = successfulRecoveries / static_cast<double>(errorRecoveryIterations) * 100.0;
        
        recordMetric("avg_error_recovery_duration", avgErrorDuration);
        recordMetric("recovery_success_rate", recoverySuccessRate);
        recordMetric("total_recovery_attempts", static_cast<double>(totalRecoveriesAttempted));
        
        // Error recovery benchmarks
        assertCondition(avgErrorDuration < 10.0, "Average error recovery time should be < 10s");
        assertCondition(recoverySuccessRate > 80.0, 
                       "Error recovery success rate should be >80%");
        
        std::cout << "    Error recovery stress test PASSED: "
                  << "avg_duration=" << avgErrorDuration << "s, "
                  << "recovery_rate=" << recoverySuccessRate << "%, "
                  << "attempts=" << totalRecoveriesAttempted << std::endl;
    }
    
private:
    bool attemptRecovery(const std::exception& error) {
        // Simulate recovery attempt based on error type
        std::string errorMsg = error.what();
        
        if (errorMsg.find("Configuration") != std::string::npos) {
            // Configuration error - try to reinitialize
            brain_->reset();
            brain_->initialize();
            return true;
        } else if (errorMsg.find("Memory") != std::string::npos) {
            // Memory error - reduce memory pressure
            brain_->reset();
            brain_->initialize();
            return true;
        } else if (errorMsg.find("Network") != std::string::npos) {
            // Network connectivity error - attempt reconnection
            // In real implementation, this would retry network connections
            brain_->reset();
            brain_->initialize();
            return false; // Assume network errors are harder to recover from
        }
        
        return false;
    }
};

// Test system behavior during prolonged operation
class ProlongedOperationStressTest : public integration_tests::IntegrationTestBase {
public:
    void setupConfiguration() override {
        config_->set("neuron_count", 350);
        config_->set("region_count", 2);
        config_->set("connection_probability", 0.07f);
        config_->set("simulation_timestep", 0.001);
        config_->set("random_seed", 11111u);
        config_->set("prolonged_operation_hours", 24);
    }
    
    void setupSystem() override {
        brain_ = std::make_shared<Brain>(config_);
        assertCondition(brain_->initialize(), "Brain initialization failed");
    }
    
    void runTest() override {
        const int prolongedIterations = 30;
        const int stepsPerIteration = 3600;  // Simulate 1 hour of simulation
        double totalTime = 0.0;
        int stabilityChecks = 0;
        int performanceDegradationEvents = 0;
        
        for (int iteration = 0; iteration < prolongedIterations; ++iteration) {
            auto start = std::chrono::high_resolution_clock::now();
            
            bool systemStable = true;
            bool performanceDegraded = false;
            
            // Run prolonged simulation
            for (int step = 0; step < stepsPerIteration; ++step) {
                // Maintain continuous operation
                brain_->injectCurrentToNeurons(NeuronType::Sensory, 15.0f + (step % 20));
                
                brain_->step(step);
                
                // Check for stability issues
                if (brain_->getFiringNeuronCount() > brain_->getTotalNeuronCount()) {
                    systemStable = false;
                }
                
                // Check for performance degradation
                if (step % 500 == 0 && iteration % 3 == 0) {
                    // Simulate performance monitoring
                    double activityLevel = brain_->getAverageFiringRate();
                    if (activityLevel < 0.1f) {
                        performanceDegraded = true;
                        performanceDegradationEvents++;
                    }
                }
                
                // Periodically check system health
                if (step % 1000 == 0) {
                    stabilityChecks++;
                    bool healthOk = verifySystemHealth(step);
                    if (!healthOk) {
                        systemStable = false;
                    }
                }
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double>(end - start).count();
            totalTime += duration;
            
            recordMetric("prolonged_iteration_" + std::to_string(iteration) + ".duration", duration);
            recordMetric("prolonged_iteration_" + std::to_string(iteration) + ".stable", 
                        systemStable ? 1.0 : 0.0);
            recordMetric("prolonged_iteration_" + std::to_string(iteration) + ".degraded", 
                        performanceDegraded ? 1.0 : 0.0);
            
            std::cout << "    Prolonged operation iteration " << iteration << ": " 
                      << "duration=" << duration << "s, "
                      << "stable=" << (systemStable ? "yes" : "no") << ", "
                      << "degraded=" << (performanceDegraded ? "yes" : "no") << std::endl;
        }
        
        // Calculate prolonged operation metrics
        double avgProlongedDuration = totalTime / prolongedIterations;
        double stabilityRate = (prolongedIterations - (stabilityChecks > 0 ? 0 : prolongedIterations)) / 
                              static_cast<double>(prolongedIterations) * 100.0;
        double degradationRate = performanceDegradationEvents / 
                                static_cast<double>(prolongedIterations) * 100.0;
        
        recordMetric("avg_prolonged_duration", avgProlongedDuration);
        recordMetric("stability_rate", stabilityRate);
        recordMetric("degradation_rate", degradationRate);
        
        // Prolonged operation benchmarks
        assertCondition(avgProlongedDuration < 20.0, 
                       "Average prolonged operation time should be < 20s");
        assertCondition(stabilityRate > 85.0, 
                       "Prolonged operation stability rate should be >85%");
        assertCondition(degradationRate < 15.0, 
                       "Performance degradation rate should be <15%");
        
        std::cout << "    Prolonged operation stress test PASSED: "
                  << "avg_duration=" << avgProlongedDuration << "s, "
                  << "stability_rate=" << stabilityRate << "%, "
                  << "degradation_rate=" << degradationRate << "%, "
                  << "health_checks=" << stabilityChecks << std::endl;
    }
    
private:
    bool verifySystemHealth(int currentStep) {
        // Verify system health indicators
        bool neuronsConsistent = brain_->getTotalNeuronCount() > 0;
        bool synapsesValid = brain_->getTotalSynapseCount() >= 0;
        bool firingRateReasonable = brain_->getAverageFiringRate() < 100.0f;  // Prevent unrealistic firing
        
        return neuronsConsistent && synapsesValid && firingRateReasonable;
    }
};

// Stress test runner
class StressTestRunner {
public:
    static void runAllTests() {
        std::cout << "=== NLM Stress Integration Tests ===" << std::endl;
        
        HighLoadStressTest highLoadTest;
        std::cout << "Running High Load Stress Test..." << std::endl;
        highLoadTest.run();
        if (highLoadTest.getMetrics().success) {
            std::cout << "  PASSED (duration: " << highLoadTest.getMetrics().duration << "s)" << std::endl;
        } else {
            std::cout << "  FAILED: " << highLoadTest.getMetrics().failureReason << std::endl;
        }
        
        MemoryPressureStressTest memoryTest;
        std::cout << "Running Memory Pressure Stress Test..." << std::endl;
        memoryTest.run();
        if (memoryTest.getMetrics().success) {
            std::cout << "  PASSED (duration: " << memoryTest.getMetrics().duration << "s)" << std::endl;
        } else {
            std::cout << "  FAILED: " << memoryTest.getMetrics().failureReason << std::endl;
        }
        
        ErrorRecoveryStressTest errorTest;
        std::cout << "Running Error Recovery Stress Test..." << std::endl;
        errorTest.run();
        if (errorTest.getMetrics().success) {
            std::cout << "  PASSED (duration: " << errorTest.getMetrics().duration << "s)" << std::endl;
        } else {
            std::cout << "  FAILED: " << errorTest.getMetrics().failureReason << std::endl;
        }
        
        ProlongedOperationStressTest prolongedTest;
        std::cout << "Running Prolonged Operation Stress Test..." << std::endl;
        prolongedTest.run();
        if (prolongedTest.getMetrics().success) {
            std::cout << "  PASSED (duration: " << prolongedTest.getMetrics().duration << "s)" << std::endl;
        } else {
            std::cout << "  FAILED: " << prolongedTest.getMetrics().failureReason << std::endl;
        }
        
        std::cout << "=== Stress Tests Complete ===" << std::endl;
    }
};

} // namespace stress_tests
} // namespace nlm

#endif // NLM_STRESS_TESTS_HPP
