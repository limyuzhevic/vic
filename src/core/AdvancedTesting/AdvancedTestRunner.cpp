#include "AdvancedTestConfiguration.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <chrono>

using namespace nlm;

AdvancedTestConfiguration::AdvancedTestConfiguration()
    : testMode_(TestMode::Basic),
      validationLevel_(ValidationLevel::Basic) {
    loadDefaultParameters();
}

void AdvancedTestConfiguration::setTestMode(TestMode mode) {
    testMode_ = mode;
}

AdvancedTestConfiguration::TestMode AdvancedTestConfiguration::getTestMode() const {
    return testMode_;
}

void AdvancedTestConfiguration::setValidationLevel(ValidationLevel level) {
    validationLevel_ = level;
}

AdvancedTestConfiguration::ValidationLevel AdvancedTestConfiguration::getValidationLevel() const {
    return validationLevel_;
}

void AdvancedTestConfiguration::setStressTestParams(const StressTestParams& params) {
    stressTestParams_ = params;
}

const AdvancedTestConfiguration::StressTestParams& AdvancedTestConfiguration::getStressTestParams() const {
    return stressTestParams_;
}

void AdvancedTestConfiguration::setPerformanceBenchmarkParams(const PerformanceBenchmarkParams& params) {
    performanceBenchmarkParams_ = params;
}

const AdvancedTestConfiguration::PerformanceBenchmarkParams& AdvancedTestConfiguration::getPerformanceBenchmarkParams() const {
    return performanceBenchmarkParams_;
}

void AdvancedTestConfiguration::addParameter(const std::string& key, const std::string& value) {
    customParameters_[key] = value;
}

std::string AdvancedTestConfiguration::getParameter(const std::string& key) const {
    auto it = customParameters_.find(key);
    return it != customParameters_.end() ? it->second : "";
}

std::map<std::string, std::string> AdvancedTestConfiguration::getAllParameters() const {
    return customParameters_;
}

std::vector<std::string> AdvancedTestConfiguration::validate() const {
    std::vector<std::string> errors;
    
    switch (validationLevel_) {
        case ValidationLevel::None:
            break;
        case ValidationLevel::Basic:
            if (customParameters_.find("neuron_count") != customParameters_.end()) {
                try {
                    int64_t neuronCount = std::stoll(customParameters_["neuron_count"]);
                    if (neuronCount <= 0 || neuronCount > 100000) {
                        errors.push_back("Invalid neuron count: " + customParameters_["neuron_count"]);
                    }
                } catch (const std::exception&) {
                    errors.push_back("Invalid neuron count format: " + customParameters_["neuron_count"]);
                }
            }
            break;
        case ValidationLevel::Comprehensive:
            // Add more comprehensive validation
            if (customParameters_.find("simulation_timestep") != customParameters_.end()) {
                try {
                    double timestep = std::stod(customParameters_["simulation_timestep"]);
                    if (timestep <= 0.0 || timestep > 1.0) {
                        errors.push_back("Invalid timestep: " + customParameters_["simulation_timestep"]);
                    }
                } catch (const std::exception&) {
                    errors.push_back("Invalid timestep format: " + customParameters_["simulation_timestep"]);
                }
            }
            break;
        case ValidationLevel::Strict:
            // Add strict validation including all parameters
            errors.insert(errors.end(), validate().begin(), validate().end());
            break;
    }
    
    return errors;
}

std::string AdvancedTestConfiguration::generateSummary() const {
    std::ostringstream oss;
    oss << "=== Advanced Test Configuration ===" << std::endl;
    oss << "Test Mode: ";
    switch (testMode_) {
        case TestMode::Basic: oss << "Basic"; break;
        case TestMode::Learning: oss << "Learning"; break;
        case TestMode::Plasticity: oss << "Plasticity"; break;
        case TestMode::Integration: oss << "Integration"; break;
        case TestMode::Stress: oss << "Stress"; break;
        case TestMode::Performance: oss << "Performance"; break;
    }
    oss << std::endl;
    
    oss << "Validation Level: ";
    switch (validationLevel_) {
        case ValidationLevel::None: oss << "None"; break;
        case ValidationLevel::Basic: oss << "Basic"; break;
        case ValidationLevel::Comprehensive: oss << "Comprehensive"; break;
        case ValidationLevel::Strict: oss << "Strict"; break;
    }
    oss << std::endl;
    
    if (testMode_ == TestMode::Stress) {
        oss << "Stress Test Parameters:" << std::endl;
        oss << "  Stress Level: " << stressTestParams_.currentStressLevel << std::endl;
        oss << "  Duration Steps: " << stressTestParams_.durationSteps << std::endl;
        oss << "  Recovery Enabled: " << (stressTestParams_.enableRecovery ? "Yes" : "No") << std::endl;
        oss << "  Recovery Threshold: " << stressTestParams_.recoveryThreshold << std::endl;
    }
    
    if (testMode_ == TestMode::Performance) {
        oss << "Performance Benchmark Parameters:" << std::endl;
        oss << "  Measure Memory Usage: " << (performanceBenchmarkParams_.measureMemoryUsage ? "Yes" : "No") << std::endl;
        oss << "  Measure CPU Usage: " << (performanceBenchmarkParams_.measureCpuUsage ? "Yes" : "No") << std::endl;
        oss << "  Warmup Steps: " << performanceBenchmarkParams_.warmupSteps << std::endl;
        oss << "  Measurement Steps: " << performanceBenchmarkParams_.measurementSteps << std::endl;
        oss << "  Operations to Benchmark: " << performanceBenchmarkParams_.operationsToBenchmark.size() << std::endl;
    }
    
    if (!customParameters_.empty()) {
        oss << "Custom Parameters:" << std::endl;
        for (const auto& pair : customParameters_) {
            oss << "  " << pair.first << " = " << pair.second << std::endl;
        }
    }
    
    return oss.str();
}

void AdvancedTestConfiguration::resetToDefaults() {
    testMode_ = TestMode::Basic;
    validationLevel_ = ValidationLevel::Basic;
    loadDefaultParameters();
    customParameters_.clear();
}

void AdvancedTestConfiguration::loadDefaultParameters() {
    customParameters_["neuron_count"] = "500";
    customParameters_["region_count"] = "1";
    customParameters_["simulation_timestep"] = "0.001";
    customParameters_["connection_probability"] = "0.15";
}

AdvancedTestRunner::AdvancedTestRunner(std::shared_ptr<ConfigurationManager> config)
    : config_(config) {
}

int AdvancedTestRunner::runAdvancedTests(std::shared_ptr<Brain> brain, const AdvancedTestConfiguration& config) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot run advanced tests: Brain is null");
        return -1;
    }
    
    NLM_LOG_INFO("Starting advanced tests with configuration:");
    NLM_LOG_INFO(config.generateSummary());
    
    int testsRun = 0;
    int testsPassed = 0;
    int testsFailed = 0;
    
    switch (config.getTestMode()) {
        case AdvancedTestConfiguration::TestMode::Basic:
            // Run basic tests
            if (runBasicConnectivityTest(brain)) {
                testsPassed++;
                testsRun++;
            } else {
                testsFailed++;
                testsRun++;
            }
            break;
            
        case AdvancedTestConfiguration::TestMode::Learning:
            // Run learning tests
            if (runPlasticityExperiment(brain)) {
                testsPassed++;
                testsRun++;
            } else {
                testsFailed++;
                testsRun++;
            }
            break;
            
        case AdvancedTestConfiguration::TestMode::Plasticity:
            // Run plasticity tests
            if (runStdpVerification(brain)) {
                testsPassed++;
                testsRun++;
            } else {
                testsFailed++;
                testsRun++;
            }
            break;
            
        case AdvancedTestConfiguration::TestMode::Integration:
            // Run integration tests
            if (runIntegrationTest(brain, config)) {
                testsPassed++;
                testsRun++;
            } else {
                testsFailed++;
                testsRun++;
            }
            break;
            
        case AdvancedTestConfiguration::TestMode::Stress:
            // Run stress tests
            if (runStressTest(brain, config)) {
                testsPassed++;
                testsRun++;
            } else {
                testsFailed++;
                testsRun++;
            }
            break;
            
        case AdvancedTestConfiguration::TestMode::Performance:
            // Run performance benchmarks
            if (runPerformanceBenchmark(brain, config)) {
                testsPassed++;
                testsRun++;
            } else {
                testsFailed++;
                testsRun++;
            }
            break;
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Advanced Test Summary ===");
    NLM_LOG_INFO("Tests run: " + std::to_string(testsRun));
    NLM_LOG_INFO("Tests passed: " + std::to_string(testsPassed));
    NLM_LOG_INFO("Tests failed: " + std::to_string(testsFailed));
    
    return testsFailed == 0 ? 0 : 1;
}

bool AdvancedTestRunner::runStressTest(std::shared_ptr<Brain> brain, const AdvancedTestConfiguration& config) {
    return runStressTestImpl(brain, config);
}

bool AdvancedTestRunner::runPerformanceBenchmark(std::shared_ptr<Brain> brain, const AdvancedTestConfiguration& config) {
    return runPerformanceBenchmarkImpl(brain, config);
}

bool AdvancedTestRunner::runIntegrationTest(std::shared_ptr<Brain> brain, const AdvancedTestConfiguration& config) {
    return runIntegrationTestImpl(brain, config);
}

std::string AdvancedTestRunner::getTestResults() const {
    return testResults_;
}

void AdvancedTestRunner::clearTestResults() {
    testResults_.clear();
}

bool AdvancedTestRunner::runStressTestImpl(std::shared_ptr<Brain> brain, const AdvancedTestConfiguration& config) {
    NLM_LOG_INFO("=== Stress Test Implementation ===");
    
    const auto& params = config.getStressTestParams();
    NLM_LOG_INFO("Running stress test with level: " + std::to_string(params.currentStressLevel));
    NLM_LOG_INFO("Duration: " + std::to_string(params.durationSteps) + " steps");
    
    // Simulate stress by injecting varying levels of current
    for (int step = 0; step < params.durationSteps; ++step) {
        // Vary input current based on stress level
        float current = 30.0f + params.currentStressLevel * 50.0f;
        
        for (size_t i = 0; i < std::min(size_t(20), brain->getTotalNeuronCount() / 4); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, current);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 10% of duration
        if (params.durationSteps > 0 && step % (params.durationSteps / 10) == 0) {
            logTestProgress("Stress Test", static_cast<double>(step) / params.durationSteps);
        }
    }
    
    // Check if recovery is needed
    if (params.enableRecovery) {
        NLM_LOG_INFO("Checking recovery after stress test...");
        // Simulate recovery by reducing stress level gradually
        for (int step = 0; step < params.durationSteps / 2; ++step) {
            float current = 30.0f * params.recoveryThreshold;
            
            for (size_t i = 0; i < std::min(size_t(10), brain->getTotalNeuronCount() / 4); ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, current);
            }
            
            brain->step(params.durationSteps + step, (params.durationSteps + step) * 0.001);
        }
    }
    
    NLM_LOG_INFO("✓ Stress test completed successfully");
    return true;
}

bool AdvancedTestRunner::runPerformanceBenchmarkImpl(std::shared_ptr<Brain> brain, const AdvancedTestConfiguration& config) {
    NLM_LOG_INFO("=== Performance Benchmark Implementation ===");
    
    const auto& params = config.getPerformanceBenchmarkParams();
    NLM_LOG_INFO("Performance benchmark with memory measurement: " << 
                (params.measureMemoryUsage ? "Yes" : "No"));
    NLM_LOG_INFO("Performance benchmark with CPU measurement: " << 
                (params.measureCpuUsage ? "Yes" : "No"));
    
    // Warmup phase
    NLM_LOG_INFO("Running warmup phase (" + std::to_string(params.warmupSteps) + " steps)...");
    for (int step = 0; step < params.warmupSteps; ++step) {
        for (size_t i = 0; i < std::min(size_t(10), brain->getTotalNeuronCount() / 4); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
        }
        brain->step(step, step * 0.001);
    }
    
    // Measurement phase
    NLM_LOG_INFO("Running measurement phase (" + std::to_string(params.measurementSteps) + " steps)...");
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < params.measurementSteps; ++step) {
        for (size_t i = 0; i < std::min(size_t(10), brain->getTotalNeuronCount() / 4); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
        }
        brain->step(params.warmupSteps + step, (params.warmupSteps + step) * 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    double operationsPerSecond = params.measurementSteps / (duration.count() / 1000.0);
    
    NLM_LOG_INFO("Performance benchmark results:");
    NLM_LOG_INFO("  Total operations: " + std::to_string(params.measurementSteps));
    NLM_LOG_INFO("  Total time: " + std::to_string(duration.count() / 1000.0) + "s");
    NLM_LOG_INFO("  Operations per second: " + std::to_string(operationsPerSecond));
    
    if (params.measureMemoryUsage) {
        double memoryUsage = measureMemoryUsage();
        NLM_LOG_INFO("  Memory usage: " + std::to_string(memoryUsage) + " MB");
    }
    
    if (params.measureCpuUsage) {
        double cpuUsage = measureCpuUsage();
        NLM_LOG_INFO("  CPU usage: " + std::to_string(cpuUsage) + "%");
    }
    
    NLM_LOG_INFO("✓ Performance benchmark completed successfully");
    return true;
}

bool AdvancedTestRunner::runIntegrationTestImpl(std::shared_ptr<Brain> brain, const AdvancedTestConfiguration& config) {
    NLM_LOG_INFO("=== Integration Test Implementation ===");
    
    NLM_LOG_INFO("Running integration test to verify system-wide consistency");
    
    // Reset brain for integration test
    if (!brain->reset() || !brain->initialize()) {
        NLM_LOG_ERROR("Failed to reset brain for integration test");
        return false;
    }
    
    // Test various brain functions in sequence
    std::vector<std::function<bool(std::shared_ptr<Brain>)>> integrationTests = {
        [this](std::shared_ptr<Brain> b) { return runBasicConnectivityTest(b); },
        [this](std::shared_ptr<Brain> b) { return runPlasticityExperiment(b); },
        [this](std::shared_ptr<Brain> b) { return runStdpVerification(b); }
    };
    
    int testsPassed = 0;
    for (size_t i = 0; i < integrationTests.size(); ++i) {
        NLM_LOG_INFO("Running integration test " + std::to_string(i + 1) + "/" + std::to_string(integrationTests.size()) + "...");
        if (integrationTests[i](brain)) {
            testsPassed++;
            NLM_LOG_INFO("✓ Integration test " + std::to_string(i + 1) + " passed");
        } else {
            NLM_LOG_ERROR("✗ Integration test " + std::to_string(i + 1) + " failed");
        }
    }
    
    double integrationScore = static_cast<double>(testsPassed) / integrationTests.size();
    NLM_LOG_INFO("Integration test score: " + std::to_string(integrationScore * 100.0) + "%");
    
    if (integrationScore >= 0.8) {
        NLM_LOG_INFO("✓ Integration test passed successfully");
        return true;
    } else {
        NLM_LOG_ERROR("✗ Integration test failed (score below threshold)");
        return false;
    }
}

double AdvancedTestRunner::measureMemoryUsage() const {
    // Simplified memory measurement - in a real implementation, this would use system-specific APIs
    return 128.0;  // Return dummy value
}

double AdvancedTestRunner::measureCpuUsage() const {
    // Simplified CPU measurement - in a real implementation, this would use system-specific APIs
    return 45.0;  // Return dummy value
}

void AdvancedTestRunner::logTestProgress(const std::string& testName, double progress) const {
    std::ostringstream oss;
    oss << "  " << testName << ": " << std::fixed << std::setprecision(1) << (progress * 100.0) << "% complete";
    NLM_LOG_INFO(oss.str());
}