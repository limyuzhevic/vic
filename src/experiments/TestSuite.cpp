// NLM (熙然) - Neural Learning Machine
// Test Suite Implementation
// Implements all test functions from main.cpp with proper error handling and logging

#include "TestSuite.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>

namespace nlm {

// Constructor
TestSuite::TestSuite(const TestConfig& config) : config_(config) {
    if (config_.enableLogging) {
        NLM_LOG_INFO("TestSuite initialized with configuration");
    }
}

// Run all tests in sequence
TestSuite::SuiteStats TestSuite::runAllTests(std::shared_ptr<Brain> brain) {
    SuiteStats stats;
    stats.totalTests = 3;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (config_.enableLogging) {
        NLM_LOG_INFO("Starting Test Suite with " + std::to_string(stats.totalTests) + " tests");
    }
    
    // Run Test 1: Basic Connectivity
    logTestStart("Basic Connectivity Test");
    TestResult result1 = runBasicConnectivityTestImpl(brain);
    stats.results["basic_connectivity"] = result1;
    if (result1.passed) stats.passedTests++;
    else stats.failedTests++;
    
    if (config_.resetBetweenTests) {
        resetBrain(brain);
    }
    
    // Run Test 2: Plasticity Experiment
    logTestStart("Plasticity Experiment");
    TestResult result2 = runPlasticityExperimentImpl(brain);
    stats.results["plasticity_experiment"] = result2;
    if (result2.passed) stats.passedTests++;
    else stats.failedTests++;
    
    if (config_.resetBetweenTests) {
        resetBrain(brain);
    }
    
    // Run Test 3: STDP Verification
    logTestStart("STDP Verification");
    TestResult result3 = runStdpVerificationImpl(brain);
    stats.results["stdp_verification"] = result3;
    if (result3.passed) stats.passedTests++;
    else stats.failedTests++;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    stats.totalDurationSeconds = getElapsedSeconds(startTime);
    
    if (config_.enableLogging) {
        NLM_LOG_INFO("Test Suite completed: " + std::to_string(stats.passedTests) + "/" + 
                    std::to_string(stats.totalTests) + " tests passed");
    }
    
    return stats;
}

// Basic connectivity test implementation
TestSuite::TestResult TestSuite::runBasicConnectivityTestImpl(std::shared_ptr<Brain> brain) {
    TestResult result;
    result.testName = "Basic Connectivity Test";
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    logTestProgress("Basic Connectivity Test", "Running basic neural connectivity test...");
    
    try {
        // Initialize experiment
        LearningExperiment experiment(brain, config_.seed);
        experiment.recordInitialState();
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into neurons
        logTestProgress("Basic Connectivity Test", 
                       "Injecting current into " + std::to_string(config_.connectivityTestNeurons) + " neurons...");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            result.passed = false;
            result.message = "No brain region found";
            return result;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            result.passed = false;
            result.message = "No neurons found";
            return result;
        }
        
        // Inject current into specified number of neurons
        for (size_t i = 0; i < std::min(config_.connectivityTestNeurons, neurons.size()); ++i) {
            neurons[i]->injectCurrent(config_.connectivityTestCurrent);
        }
        
        // Run simulation steps
        logTestProgress("Basic Connectivity Test", 
                       "Running " + std::to_string(config_.connectivityTestSteps) + " simulation steps...");
        
        for (SimulationStep step = 0; step < config_.connectivityTestSteps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        // Calculate results
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        result.metrics["initial_spikes"] = static_cast<double>(initialSpikes);
        result.metrics["spikes_generated"] = static_cast<double>(spikes);
        result.metrics["firing_rate"] = (spikes > 0) ? 
            (static_cast<double>(spikes) / config_.connectivityTestSteps) : 0.0;
        
        // Determine if test passes
        result.passed = (spikes > 0);
        
        if (result.passed) {
            result.message = "Spikes propagate through network successfully";
            result.details = "Generated " + std::to_string(spikes) + " spikes from " + 
                            std::to_string(config_.connectivityTestNeurons) + " injected neurons";
        } else {
            result.message = "No spikes generated - checking neuron parameters";
            // Check neuron parameters for diagnosis
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                std::string neuronInfo = "Neuron " + std::to_string(i) + 
                                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                                        " thresh=" + std::to_string(neurons[i]->getThreshold());
                result.details += "\n  " + neuronInfo;
            }
        }
        
        experiment.recordFinalState();
        experiment.computeStatistics();
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.message = "Exception occurred: " + std::string(e.what());
    }
    
    result.durationSeconds = getElapsedSeconds(startTime);
    logTestEnd(result);
    
    return result;
}

// Plasticity experiment implementation
TestSuite::TestResult TestSuite::runPlasticityExperimentImpl(std::shared_ptr<Brain> brain) {
    TestResult result;
    result.testName = "Plasticity Experiment";
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    logTestProgress("Plasticity Experiment", "Starting plasticity learning experiment...");
    
    try {
        // Initialize experiment
        LearningExperiment experiment(brain, config_.seed);
        experiment.recordInitialState();
        
        // Enable plasticity on synapses
        logTestProgress("Plasticity Experiment", "Enabling plasticity mechanisms...");
        
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(config_.enableHebbianPlasticity, 
                                    config_.enableStdpPlasticity, 
                                    config_.enableStructuralPlasticity);
            }
        }
        
        // Record initial statistics
        size_t initialSpikes = brain->getTotalSpikeCount();
        size_t initialFiring = brain->getFiringNeuronCount();
        
        // Apply repeated input pattern
        logTestProgress("Plasticity Experiment", 
                       "Applying input patterns for " + std::to_string(config_.plasticityTestSteps) + " steps...");
        
        for (SimulationStep step = 0; step < config_.plasticityTestSteps; ++step) {
            // Create input pattern
            for (size_t i = 0; i < config_.plasticityInputNeurons && 
                              i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, config_.plasticityInputCurrent);
            }
            
            brain->step(step, step * 0.001);
            
            // Log progress every 100 steps
            if (step % 100 == 0) {
                size_t spikes = brain->getTotalSpikeCount();
                size_t firing = brain->getFiringNeuronCount();
                logTestProgress("Plasticity Experiment", 
                               "Step " + std::to_string(step) + 
                               " | Spikes: " + std::to_string(spikes) + 
                               " | Firing: " + std::to_string(firing));
            }
        }
        
        // Record final statistics
        experiment.recordFinalState();
        experiment.computeStatistics();
        
        // Calculate results
        size_t finalSpikes = brain->getTotalSpikeCount();
        size_t finalFiring = brain->getFiringNeuronCount();
        
        result.metrics["initial_spikes"] = static_cast<double>(initialSpikes);
        result.metrics["final_spikes"] = static_cast<double>(finalSpikes);
        result.metrics["spike_increase"] = static_cast<double>(finalSpikes - initialSpikes);
        result.metrics["initial_firing"] = static_cast<double>(initialFiring);
        result.metrics["final_firing"] = static_cast<double>(finalFiring);
        result.metrics["firing_increase"] = static_cast<double>(finalFiring - initialFiring);
        
        // Check for learning (simplified check)
        bool hasLearningOccurred = (finalSpikes > initialSpikes * 1.1) || 
                                  (finalFiring > initialFiring * 1.1);
        
        result.passed = hasLearningOccurred;
        
        if (result.passed) {
            result.message = "Plasticity learning experiment successful";
            result.details = "Spike count increased from " + std::to_string(initialSpikes) + 
                            " to " + std::to_string(finalSpikes) +
                            ", firing neurons from " + std::to_string(initialFiring) + 
                            " to " + std::to_string(finalFiring);
        } else {
            result.message = "Limited learning observed in plasticity experiment";
            result.details = "Spike activity change may be below threshold for significant learning";
        }
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.message = "Exception occurred: " + std::string(e.what());
    }
    
    result.durationSeconds = getElapsedSeconds(startTime);
    logTestEnd(result);
    
    return result;
}

// STDP verification implementation
TestSuite::TestResult TestSuite::runStdpVerificationImpl(std::shared_ptr<Brain> brain) {
    TestResult result;
    result.testName = "STDP Verification";
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    logTestProgress("STDP Verification", "Running STDP verification test...");
    
    try {
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            result.passed = false;
            result.message = "No brain region found";
            return result;
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            result.passed = false;
            result.message = "Not enough synapses for STDP test (need 5, have " + 
                            std::to_string(synapses.size()) + ")";
            return result;
        }
        
        logTestProgress("STDP Verification", "Testing STDP on " + std::to_string(std::min(size_t(5), synapses.size())) + 
                       " synapses...");
        
        // Enable STDP only on first 5 synapses
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        }
        
        // Create correlated activity (pre then post)
        logTestProgress("STDP Verification", "Creating correlated pre->post activity (50 trials)...");
        
        for (int trial = 0; trial < config_.stdpTestTrials; ++trial) {
            // Fire pre-synaptic neuron
            Neuron* preNeuron = nullptr;
            Neuron* postNeuron = nullptr;
            
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                preNeuron = neurons[0];
                postNeuron = neurons[1];
            }
            
            if (preNeuron && postNeuron) {
                // Pre fires first
                preNeuron->injectCurrent(config_.stdpCorrelationCurrent);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                // Then post fires
                postNeuron->injectCurrent(config_.stdpCorrelationCurrent);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        // Check weight changes
        logTestProgress("STDP Verification", "Analyzing synaptic weight changes...");
        
        std::vector<float> afterWeights;
        float totalDelta = 0.0f;
        
        for (size_t i = 0; i < 5; ++i) {
            afterWeights.push_back(synapses[i]->getWeight());
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            totalDelta += delta;
            
            result.details += "Synapse " + std::to_string(i) + 
                            " before: " + std::to_string(beforeWeights[i]) + 
                            " after: " + std::to_string(afterWeights[i]) + 
                            " (Δ=" + std::to_string(delta) + ")\n";
        }
        
        result.metrics["total_delta"] = static_cast<double>(totalDelta);
        result.metrics["avg_delta"] = (totalDelta / 5.0);
        
        // Determine if test passes based on STDP behavior
        result.passed = (totalDelta > config_.stdpWeightThreshold);
        
        if (result.passed) {
            result.message = "STDP working: Pre-before-post produced potentiation";
            result.details += "\n✓ STDP potentiation detected (total Δ = " + std::to_string(totalDelta) + ")";
        } else if (totalDelta < -config_.stdpWeightThreshold) {
            result.message = "STDP reversed: Check parameters";
            result.details += "\n! STDP reversal detected (total Δ = " + std::to_string(totalDelta) + ")";
        } else {
            result.message = "No significant change: STDP may not be triggering";
            result.details += "\n! No significant change detected (total Δ = " + std::to_string(totalDelta) + ")";
        }
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.message = "Exception occurred: " + std::string(e.what());
    }
    
    result.durationSeconds = getElapsedSeconds(startTime);
    logTestEnd(result);
    
    return result;
}

// Reset brain state
void TestSuite::resetBrain(std::shared_ptr<Brain> brain) {
    if (config_.enableLogging) {
        NLM_LOG_INFO("Resetting brain state for next test");
    }
    brain->reset();
    brain->initialize();
}

// Generate human-readable report
std::string TestSuite::generateHumanReadableReport(const SuiteStats& stats) const {
    std::stringstream ss;
    ss << "=== NLM Test Suite Report ===" << std::endl;
    ss << std::endl;
    ss << "Summary:" << std::endl;
    ss << "  Total Tests: " << stats.totalTests << std::endl;
    ss << "  Passed: " << stats.passedTests << std::endl;
    ss << "  Failed: " << stats.failedTests << std::endl;
    ss << "  Total Duration: " << std::fixed << std::setprecision(2) 
       << stats.totalDurationSeconds << " seconds" << std::endl;
    ss << std::endl;
    
    // Individual test results
    ss << "Detailed Results:" << std::endl;
    for (const auto& pair : stats.results) {
        const auto& result = pair.second;
        ss << "  " << result.testName << ": " << (result.passed ? "PASS" : "FAIL") << std::endl;
        ss << "    Duration: " << std::fixed << std::setprecision(3) 
           << result.durationSeconds << " seconds" << std::endl;
        ss << "    Message: " << result.message << std::endl;
        if (!result.details.empty()) {
            ss << "    Details: " << result.details << std::endl;
        }
        // Display key metrics
        if (!result.metrics.empty()) {
            ss << "    Metrics:" << std::endl;
            for (const auto& metric : result.metrics) {
                ss << "      " << metric.first << ": " << metric.second << std::endl;
            }
        }
        ss << std::endl;
    }
    
    ss << "=== End Report ===" << std::endl;
    return ss.str();
}

// Save test results to file
bool TestSuite::saveResults(const SuiteStats& stats, const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
            return false;
        }
        
        // Always generate human-readable report
        file << generateHumanReadableReport(stats);
        
        file.close();
        
        if (config_.enableLogging) {
            NLM_LOG_INFO("Test results saved to: " + filepath);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save results: " + std::string(e.what()));
        return false;
    }
}

// Print test summary to console
void TestSuite::printSummary(const SuiteStats& stats) const {
    std::cout << "=== Test Suite Summary ===" << std::endl;
    std::cout << "Total Tests: " << stats.totalTests << std::endl;
    std::cout << "Passed: " << stats.passedTests << std::endl;
    std::cout << "Failed: " << stats.failedTests << std::endl;
    std::cout << "Total Duration: " << std::fixed << std::setprecision(2) 
              << stats.totalDurationSeconds << " seconds" << std::endl;
    std::cout << std::endl;
    
    for (const auto& pair : stats.results) {
        const auto& result = pair.second;
        std::cout << result.testName << ": " << (result.passed ? "PASS" : "FAIL") 
                  << " (" << std::fixed << std::setprecision(3) 
                  << result.durationSeconds << "s)" << std::endl;
    }
}

// Helper: Get current timestamp
std::string TestSuite::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
       << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

// Helper: Get elapsed seconds
float TestSuite::getElapsedSeconds(std::chrono::high_resolution_clock::time_point start) const {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<float>>(end - start);
    return duration.count();
}

// Helper: Check condition with tolerance
bool TestSuite::checkCondition(float value, float expected, float tolerance) const {
    return std::abs(value - expected) <= tolerance;
}

// Logging helpers
void TestSuite::logTestStart(const std::string& testName) const {
    if (config_.enableLogging) {
        NLM_LOG_INFO("[TEST START] " + testName + " - " + getCurrentTimestamp());
    }
}

void TestSuite::logTestEnd(const TestResult& result) const {
    if (config_.enableLogging) {
        std::string status = result.passed ? "PASSED" : "FAILED";
        NLM_LOG_INFO("[TEST END] " + result.testName + " - " + status + 
                    " (" + std::to_string(result.durationSeconds) + "s)");
    }
}

void TestSuite::logTestProgress(const std::string& testName, const std::string& message) const {
    if (config_.enableLogging) {
        NLM_LOG_INFO("[PROGRESS] " + testName + ": " + message);
    }
}

} // namespace nlm
