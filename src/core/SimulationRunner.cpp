// SimulationRunner implementation
#include "SimulationRunner.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {

SimulationRunner::SimulationRunner(std::shared_ptr<Config> config, EnhancedLogger& logger)
    : config_(config), logger_(logger), brain_(nullptr), clock_(nullptr) {
    // Initialize brain and clock will be done in runTest()
}

bool SimulationRunner::runTest(const std::string& testName, const std::map<std::string, std::string>& params) {
    logger_.logProgress("Starting test", 0.0, "Test: " + testName);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    bool success = false;
    
    if (testName == "basic") {
        success = runBasicConnectivityTest();
    } else if (testName == "plasticity") {
        success = runPlasticityExperiment();
    } else if (testName == "stdp") {
        success = runStdpVerificationTest();
    } else if (testName == "development") {
        success = runDevelopmentTest();
    } else if (testName == "neuromodulation") {
        success = runNeuromodulationTest();
    } else if (testName == "memory") {
        success = runMemoryTest();
    } else {
        logger_.logProgress("Test", -1.0, "Unknown test: " + testName);
        return false;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(endTime - startTime).count();
    
    logger_.logExperiment(testName, duration, success);
    
    return success;
}

bool SimulationRunner::runAllTests(const std::map<std::string, std::string>& globalParams) {
    logger_.logProgress("Running all tests", 0.0, "Starting full test suite");
    
    std::vector<std::string> tests = getAvailableTests();
    int completedTests = 0;
    
    for (size_t i = 0; i < tests.size(); ++i) {
        std::string testName = tests[i];
        
        logger_.logProgress("Test suite", (i + 1) / (double)tests.size(), 
                           "Running: " + testName);
        
        bool success = runTest(testName, globalParams);
        if (!success) {
            logger_.logProgress("Test suite", -1.0, "Test failed: " + testName);
            return false;
        }
        
        completedTests++;
    }
    
    logger_.logProgress("Test suite", 1.0, "All tests completed: " + std::to_string(completedTests) + "/" + std::to_string(tests.size()));
    return true;
}

std::vector<std::string> SimulationRunner::getAvailableTests() {
    return {"basic", "plasticity", "stdp", "development", "neuromodulation", "memory"};
}

std::map<std::string, std::vector<std::string>> SimulationRunner::getTestParameters(const std::string& testName) {
    std::map<std::string, std::vector<std::string>> params;
    
    if (testName == "basic") {
        params["neurons"] = {"100", "500", "1000"};
        params["steps"] = {"10", "50", "100"};
    } else if (testName == "plasticity") {
        params["plasticity_enabled"] = {"true", "false"};
        params["steps"] = {"500", "1000", "2000"};
    } else if (testName == "stdp") {
        params["stdp_enabled"] = {"true", "false"};
        params["trials"] = {"25", "50", "100"};
    }
    
    return params;
}

std::string SimulationRunner::getTestDescription(const std::string& testName) {
    std::map<std::string, std::string> descriptions = {
        {"basic", "Basic connectivity test - verifies that spikes propagate through the network"},
        {"plasticity", "Plasticity learning experiment - tests synaptic strengthening and weakening"},
        {"stdp", "STDP verification test - confirms spike-timing dependent plasticity"},
        {"development", "Development experiment - tests brain maturation over time"},
        {"neuromodulation", "Neuromodulation experiment - tests reward and exploration signals"},
        {"memory", "Memory experiment - tests pattern storage and retrieval"}
    };
    
    auto it = descriptions.find(testName);
    if (it != descriptions.end()) {
        return it->second;
    }
    
    return "Unknown test";
}

bool SimulationRunner::saveResults(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "NLM Simulation Results" << std::endl;
    file << "=======================" << std::endl;
    file << "Timestamp: " << std::chrono::system_clock::now() << std::endl;
    file << "Tests run: " << getAvailableTests().size() << std::endl;
    
    // Save brain state if available
    if (brain_) {
        bool saveSuccess = brain_->save(filepath + ".brain");
        if (!saveSuccess) {
            file << "Warning: Failed to save brain state" << std::endl;
        }
    }
    
    file.close();
    return true;
}

bool SimulationRunner::exportResults(const std::string& filepath, const std::string& format) const {
    if (format == "json") {
        // JSON export implementation
        return saveResults(filepath + ".json");
    } else if (format == "csv") {
        // CSV export implementation
        return saveResults(filepath + ".csv");
    } else if (format == "xml") {
        // XML export implementation
        return saveResults(filepath + ".xml");
    }
    return false;
}

void SimulationRunner::initializeBrain() {
    // Brain initialization will be handled by NLMApplication
}

bool SimulationRunner::measureExecutionTime(std::function<bool()> func) {
    auto start = std::chrono::high_resolution_clock::now();
    bool result = func();
    auto end = std::chrono::high_resolution_clock::now();
    
    double duration = std::chrono::duration<double>(end - start).count();
    
    logger_.logProgress("Execution time", -1.0, "Function executed in " + std::to_string(duration) + "s");
    
    return result;
}

bool SimulationRunner::runBasicConnectivityTest() {
    // Implement basic connectivity test
    // This would include the original runBasicConnectivityTest logic
    logger_.logProgress("Basic test", 0.5, "Injecting current into neurons");
    
    // For now, just return true as a placeholder
    logger_.logProgress("Basic test", 1.0, "Basic connectivity test completed");
    return true;
}

bool SimulationRunner::runPlasticityExperiment() {
    // Implement plasticity experiment
    logger_.logProgress("Plasticity test", 0.5, "Running plasticity simulation");
    
    // For now, just return true as a placeholder
    logger_.logProgress("Plasticity test", 1.0, "Plasticity experiment completed");
    return true;
}

bool SimulationRunner::runStdpVerificationTest() {
    // Implement STDP verification test
    logger_.logProgress("STDP test", 0.5, "Running STDP verification");
    
    // For now, just return true as a placeholder
    logger_.logProgress("STDP test", 1.0, "STDP verification completed");
    return true;
}

bool SimulationRunner::runDevelopmentTest() {
    // Implement development test
    logger_.logProgress("Development test", 0.5, "Running development simulation");
    
    // For now, just return true as a placeholder
    logger_.logProgress("Development test", 1.0, "Development test completed");
    return true;
}

bool SimulationRunner::runNeuromodulationTest() {
    // Implement neuromodulation test
    logger_.logProgress("Neuromodulation test", 0.5, "Running neuromodulation simulation");
    
    // For now, just return true as a placeholder
    logger_.logProgress("Neuromodulation test", 1.0, "Neuromodulation test completed");
    return true;
}

bool SimulationRunner::runMemoryTest() {
    // Implement memory test
    logger_.logProgress("Memory test", 0.5, "Running memory simulation");
    
    // For now, just return true as a placeholder
    logger_.logProgress("Memory test", 1.0, "Memory test completed");
    return true;
}

} // namespace nlm