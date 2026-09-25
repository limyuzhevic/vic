// Test settings implementation
// Implementation of NLM test settings
#pragma once

#include "test_parameters.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

namespace nlm {
namespace test {

// Global test settings instance
TestSettings globalTestSettings;

// Load test settings from file
bool loadTestSettings(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Warning: Could not load test settings from " << filename << std::endl;
        return false;
    }
    
    std::string line;
    std::string currentSection;
    
    while (std::getline(file, line)) {
        // Remove whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        if (line[0] == '[' && line.back() == ']') {
            // Section header
            currentSection = line.substr(1, line.size() - 2);
        } else if (line.find('=') != std::string::npos) {
            // Key-value pair
            size_t pos = line.find('=');
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);
            
            setSettingFromString(currentSection, key, value);
        }
    }
    
    return true;
}

// Save test settings to file
bool saveTestSettings(const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Error: Could not save test settings to " << filename << std::endl;
        return false;
    }
    
    file << "[performance]" << std::endl;
    file << "minNeuronsPerSecond = " << globalTestSettings.performance.minNeuronsPerSecond << std::endl;
    file << "maxMemoryUsageMB = " << globalTestSettings.performance.maxMemoryUsageMB << std::endl;
    file << "maxStepTimeMs = " << std::chrono::duration_cast<std::chrono::milliseconds>(globalTestSettings.performance.maxStepTime).count() << std::endl;
    file << "enableMonitoring = " << (globalTestSettings.performance.enableMonitoring ? "true" : "false") << std::endl;
    file << "enableBenchmarking = " << (globalTestSettings.performance.enableBenchmarking ? "true" : "false") << std::endl;
    file << "benchmarkIterations = " << globalTestSettings.performance.benchmarkIterations << std::endl;
    file << std::endl;
    
    file << "[stress]" << std::endl;
    file << "maxNetworkSize = " << globalTestSettings.stress.maxNetworkSize << std::endl;
    file << "maxPlasticityRate = " << globalTestSettings.stress.maxPlasticityRate << std::endl;
    file << "maxSimulationTimeSeconds = " << std::chrono::duration_cast<std::chrono::seconds>(globalTestSettings.stress.maxSimulationTime).count() << std::endl;
    file << "enableConcurrentStress = " << (globalTestSettings.stress.enableConcurrentStress ? "true" : "false") << std::endl;
    file << std::endl;
    
    file << "[memory]" << std::endl;
    file << "maxMemoryDeltaKB = " << globalTestSettings.memory.maxMemoryDeltaKB << std::endl;
    file << "detectLeaks = " << (globalTestSettings.memory.detectLeaks ? "true" : "false") << std::endl;
    file << "allocationRounds = " << globalTestSettings.memory.allocationRounds << std::endl;
    file << "sampleIntervalMs = " << globalTestSettings.memory.sampleInterval << std::endl;
    file << std::endl;
    
    file << "[concurrency]" << std::endl;
    file << "maxThreads = " << globalTestSettings.concurrency.maxThreads << std::endl;
    file << "threadContention = " << globalTestSettings.concurrency.threadContention << std::endl;
    file << "detectDeadlocks = " << (globalTestSettings.concurrency.detectDeadlocks ? "true" : "false") << std::endl;
    file << "testRaceConditions = " << (globalTestSettings.concurrency.testRaceConditions ? "true" : "false") << std::endl;
    file << std::endl;
    
    file << "[regression]" << std::endl;
    file << "regressionThreshold = " << globalTestSettings.regression.regressionThreshold << std::endl;
    file << "compareWithBaseline = " << (globalTestSettings.regression.compareWithBaseline ? "true" : "false") << std::endl;
    file << "baselineFile = " << globalTestSettings.regression.baselineFile << std::endl;
    file << "generateRegressionReport = " << (globalTestSettings.regression.generateRegressionReport ? "true" : "false") << std::endl;
    file << std::endl;
    
    file << "[output]" << std::endl;
    file << "reportFormat = " << globalTestSettings.output.reportFormat << std::endl;
    file << "reportFile = " << globalTestSettings.output.reportFile << std::endl;
    file << "verboseOutput = " << (globalTestSettings.output.verboseOutput ? "true" : "false") << std::endl;
    file << "generateStatistics = " << (globalTestSettings.output.generateStatistics ? "true" : "false") << std::endl;
    file << "saveIntermediateResults = " << (globalTestSettings.output.saveIntermediateResults ? "true" : "false") << std::endl;
    
    return true;
}

// Set setting from string
void setSettingFromString(const std::string& section, const std::string& key, const std::string& value) {
    if (section == "performance") {
        if (key == "minNeuronsPerSecond") {
            globalTestSettings.performance.minNeuronsPerSecond = std::stod(value);
        } else if (key == "maxMemoryUsageMB") {
            globalTestSettings.performance.maxMemoryUsageMB = std::stod(value);
        } else if (key == "maxStepTimeMs") {
            globalTestSettings.performance.maxStepTime = std::chrono::milliseconds(std::stoll(value));
        } else if (key == "enableMonitoring") {
            globalTestSettings.performance.enableMonitoring = (value == "true" || value == "1" || value == "yes");
        } else if (key == "enableBenchmarking") {
            globalTestSettings.performance.enableBenchmarking = (value == "true" || value == "1" || value == "yes");
        } else if (key == "benchmarkIterations") {
            globalTestSettings.performance.benchmarkIterations = std::stoi(value);
        }
    } else if (section == "stress") {
        if (key == "maxNetworkSize") {
            globalTestSettings.stress.maxNetworkSize = std::stoi(value);
        } else if (key == "maxPlasticityRate") {
            globalTestSettings.stress.maxPlasticityRate = std::stod(value);
        } else if (key == "maxSimulationTimeSeconds") {
            globalTestSettings.stress.maxSimulationTime = std::chrono::seconds(std::stoll(value));
        } else if (key == "enableConcurrentStress") {
            globalTestSettings.stress.enableConcurrentStress = (value == "true" || value == "1" || value == "yes");
        }
    } else if (section == "memory") {
        if (key == "maxMemoryDeltaKB") {
            globalTestSettings.memory.maxMemoryDeltaKB = std::stoul(value);
        } else if (key == "detectLeaks") {
            globalTestSettings.memory.detectLeaks = (value == "true" || value == "1" || value == "yes");
        } else if (key == "allocationRounds") {
            globalTestSettings.memory.allocationRounds = std::stoi(value);
        } else if (key == "sampleIntervalMs") {
            globalTestSettings.memory.sampleInterval = std::stoi(value);
        }
    } else if (section == "concurrency") {
        if (key == "maxThreads") {
            globalTestSettings.concurrency.maxThreads = std::stoi(value);
        } else if (key == "threadContention") {
            globalTestSettings.concurrency.threadContention = std::stod(value);
        } else if (key == "detectDeadlocks") {
            globalTestSettings.concurrency.detectDeadlocks = (value == "true" || value == "1" || value == "yes");
        } else if (key == "testRaceConditions") {
            globalTestSettings.concurrency.testRaceConditions = (value == "true" || value == "1" || value == "yes");
        }
    } else if (section == "regression") {
        if (key == "regressionThreshold") {
            globalTestSettings.regression.regressionThreshold = std::stod(value);
        } else if (key == "compareWithBaseline") {
            globalTestSettings.regression.compareWithBaseline = (value == "true" || value == "1" || value == "yes");
        } else if (key == "baselineFile") {
            globalTestSettings.regression.baselineFile = value;
        } else if (key == "generateRegressionReport") {
            globalTestSettings.regression.generateRegressionReport = (value == "true" || value == "1" || value == "yes");
        }
    } else if (section == "output") {
        if (key == "reportFormat") {
            globalTestSettings.output.reportFormat = value;
        } else if (key == "reportFile") {
            globalTestSettings.output.reportFile = value;
        } else if (key == "verboseOutput") {
            globalTestSettings.output.verboseOutput = (value == "true" || value == "1" || value == "yes");
        } else if (key == "generateStatistics") {
            globalTestSettings.output.generateStatistics = (value == "true" || value == "1" || value == "yes");
        } else if (key == "saveIntermediateResults") {
            globalTestSettings.output.saveIntermediateResults = (value == "true" || value == "1" || value == "yes");
        }
    }
}

}}} // namespace nlm::test
