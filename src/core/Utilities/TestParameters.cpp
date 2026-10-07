// TestParameters.cpp
// Implementation of centralized test configuration for NLM

#include "core/Utilities/TestParameters.hpp"
#include "core/Utilities/CommandLineOptions.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <iomanip>

namespace nlm {

bool TestParameters::validate() const {
    return validateRanges() && validatePlasticityParams() && 
           validateDevelopmentParams() && validateStdpParams() && 
           validateConnectivityParams();
}

std::string TestParameters::getConfigSummary() const {
    std::stringstream ss;
    ss << "Test Configuration: " << name << "\n";
    ss << "  Neurons: " << neuronCount << "\n";
    ss << "  Steps: " << maxSteps << "\n";
    ss << "  Timestep: " << timestep << "s\n";
    ss << "  Seed: " << randomSeed << "\n";
    ss << "  Hebbian: " << (enableHebbian ? "YES" : "NO") << "\n";
    ss << "  STDP: " << (enableStdp ? "YES" : "NO") << "\n";
    ss << "  Structural: " << (enableStructuralPlasticity ? "YES" : "NO") << "\n";
    ss << "  Development: " << (enableDevelopment ? "YES" : "NO") << "\n";
    ss << "  Synaptogenesis rate: " << synaptogenesisRate << "\n";
    ss << "  Pruning rate: " << pruningRate << "\n";
    ss << "  STDP LTP weight: " << stdpLtpWeight << "\n";
    ss << "  STDP LTD weight: " << stdpLtdWeight << "\n";
    ss << "  STDP tau: " << stdpTau << "\n";
    ss << "  Connection probability: " << connectionProbability << "\n";
    ss << "  Initial weight mean: " << initialWeightMean << "\n";
    ss << "  Initial weight std: " << initialWeightStd << "\n";
    ss << "  Weight change threshold: " << weightChangeThreshold << "\n";
    ss << "  Firing rate threshold: " << firingRateThreshold << "\n";
    ss << "  Convergence threshold: " << convergenceThreshold << "\n";
    ss << "  Test neuron count: " << testNeuronCount << "\n";
    ss << "  Test injection current: " << testInjectionCurrent << "\n";
    ss << "  Test steps: " << testSteps << "\n";
    ss << "  Plasticity test steps: " << plasticityTestSteps << "\n";
    ss << "  STDP test steps: " << stdpTestSteps << "\n";
    ss << "  Learning test steps: " << learningTestSteps << "\n";
    ss << "  Verbose logging: " << (verboseLogging ? "YES" : "NO") << "\n";
    ss << "  Progress logging: " << (progressLogging ? "YES" : "NO") << "\n";
    ss << "  Benchmark mode: " << (benchmarkMode ? "YES" : "NO") << "\n";
    ss << "  Measure wall time: " << (measureWallTime ? "YES" : "NO") << "\n";
    ss << "  Measure CPU time: " << (measureCpuTime ? "YES" : "NO") << "\n";
    return ss.str();
}

bool TestParameters::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Could not open configuration file: " + filename);
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    bool success = true;
    
    while (std::getline(file, line)) {
        lineNum++;
        
        // Skip empty lines and comments
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty()) continue;
        
        // Parse key = value
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNum) + ": " + line);
            continue;
        }
        
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        
        key.erase(0, key.find_first_not_of(" \t\r\n"));
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        value.erase(0, value.find_first_not_of(" \t\r\n"));
        value.erase(value.find_last_not_of(" \t\r\n") + 1);
        
        // Parse based on key
        try {
            if (key == "neuron_count" || key == "neurons") {
                neuronCount = std::stoul(value);
            } else if (key == "max_steps" || key == "steps") {
                maxSteps = std::stoul(value);
            } else if (key == "timestep" || key == "dt") {
                timestep = std::stod(value);
            } else if (key == "random_seed" || key == "seed") {
                randomSeed = std::stoull(value);
            } else if (key == "connection_probability" || key == "connectivity") {
                connectionProbability = std::stof(value);
            } else if (key == "stdp_ltp_weight") {
                stdpLtpWeight = std::stof(value);
            } else if (key == "stdp_ltd_weight") {
                stdpLtdWeight = std::stof(value);
            } else if (key == "stdp_tau") {
                stdpTau = std::stof(value);
            } else if (key == "synaptogenesis_rate") {
                synaptogenesisRate = std::stof(value);
            } else if (key == "pruning_rate") {
                pruningRate = std::stof(value);
            } else if (key == "maturation_rate") {
                maturationRate = std::stof(value);
            } else if (key == "enable_hebbian") {
                enableHebbian = (value == "true" || value == "1" || value == "yes");
            } else if (key == "enable_stdp") {
                enableStdp = (value == "true" || value == "1" || value == "yes");
            } else if (key == "enable_structural") {
                enableStructuralPlasticity = (value == "true" || value == "1" || value == "yes");
            } else if (key == "enable_development") {
                enableDevelopment = (value == "true" || value == "1" || value == "yes");
            } else if (key == "initial_weight_mean") {
                initialWeightMean = std::stof(value);
            } else if (key == "initial_weight_std") {
                initialWeightStd = std::stof(value);
            } else if (key == "weight_change_threshold") {
                weightChangeThreshold = std::stof(value);
            } else if (key == "firing_rate_threshold") {
                firingRateThreshold = std::stof(value);
            } else if (key == "convergence_threshold") {
                convergenceThreshold = std::stof(value);
            } else if (key == "verbose_logging") {
                verboseLogging = (value == "true" || value == "1" || value == "yes");
            } else if (key == "progress_logging") {
                progressLogging = (value == "true" || value == "1" || value == "yes");
            } else if (key == "benchmark_mode") {
                benchmarkMode = (value == "true" || value == "1" || value == "yes");
            } else if (key == "test_neuron_count") {
                testNeuronCount = std::stoul(value);
            } else if (key == "test_injection_current") {
                testInjectionCurrent = std::stof(value);
            } else if (key == "test_steps") {
                testSteps = std::stoul(value);
            } else if (key == "plasticity_test_steps") {
                plasticityTestSteps = std::stoul(value);
            } else if (key == "stdp_test_steps") {
                stdpTestSteps = std::stoul(value);
            } else if (key == "learning_test_steps") {
                learningTestSteps = std::stoul(value);
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Error parsing config line ") + std::to_string(lineNum) + ": " + std::string(e.what()));
            success = false;
        }
    }
    
    file.close();
    NLM_LOG_INFO("Configuration loaded from file: " + filename);
    return success;
}

void TestParameters::loadFromCommandLine(const CommandLineOptions& cliOptions) {
    // Override with command line options
    if (cliOptions.neuronCount != 500) {
        neuronCount = cliOptions.neuronCount;
    }
    
    if (cliOptions.maxSteps != 1000) {
        maxSteps = cliOptions.maxSteps;
    }
    
    if (cliOptions.simulationTimestep != 0.001) {
        timestep = cliOptions.simulationTimestep;
    }
    
    if (cliOptions.randomSeed != 42) {
        randomSeed = cliOptions.randomSeed;
    }
    
    if (cliOptions.runConnectivityTest) {
        testMode = "connectivity";
    } else if (cliOptions.runPlasticityTest) {
        testMode = "plasticity";
    } else if (cliOptions.runStdpTest) {
        testMode = "stdp";
    } else if (cliOptions.runLearningTest) {
        testMode = "learning";
    }
    
    enableHebbian = cliOptions.enableHebbian;
    enableStdp = cliOptions.enableStdp;
    enableStructuralPlasticity = cliOptions.enableStructuralPlasticity;
    enableDevelopment = cliOptions.enableDevelopment;
    
    if (cliOptions.benchmark) {
        benchmarkMode = true;
    }
    
    if (cliOptions.verbose) {
        verboseLogging = true;
        progressLogging = true;
    }
}

TestParameters TestParameters::forTest(const std::string& testName) const {
    TestParameters testParams = *this;
    
    if (testName == "connectivity") {
        testParams.name = "Connectivity Test";
        testParams.testSteps = 50;
        testParams.testNeuronCount = 10;
        testParams.testInjectionCurrent = 50.0f;
    } else if (testName == "plasticity") {
        testParams.name = "Plasticity Test";
        testParams.testSteps = 0;
        testParams.plasticityTestSteps = 1000;
        testParams.testInjectionCurrent = 30.0f;
    } else if (testName == "stdp") {
        testParams.name = "STDP Test";
        testParams.testSteps = 0;
        testParams.stdpTestSteps = 200;
        testParams.testInjectionCurrent = 60.0f;
    } else if (testName == "learning") {
        testParams.name = "Learning Test";
        testParams.testSteps = 0;
        testParams.learningTestSteps = 500;
        testParams.testInjectionCurrent = 40.0f;
    } else {
        testParams.name = testName + " Test";
    }
    
    return testParams;
}

TestParameters TestParameters::defaultConfig() {
    TestParameters params;
    params.name = "Default Test Configuration";
    params.neuronCount = 500;
    params.maxSteps = 1000;
    params.timestep = 0.001;
    params.randomSeed = 42;
    params.enableHebbian = true;
    params.enableStdp = true;
    params.enableStructuralPlasticity = true;
    params.enableDevelopment = false;
    params.synaptogenesisRate = 0.0001f;
    params.pruningRate = 0.00001f;
    params.maturationRate = 0.00005f;
    params.stdpLtpWeight = 0.02f;
    params.stdpLtdWeight = 0.015f;
    params.stdpTau = 20.0f;
    params.connectionProbability = 0.15f;
    params.initialWeightMean = 0.5f;
    params.initialWeightStd = 0.1f;
    params.weightChangeThreshold = 0.01f;
    params.firingRateThreshold = 0.1f;
    params.convergenceThreshold = 0.001f;
    params.testNeuronCount = 20;
    params.testInjectionCurrent = 50.0f;
    params.testSteps = 100;
    params.plasticityTestSteps = 1000;
    params.stdpTestSteps = 200;
    params.learningTestSteps = 500;
    params.verboseLogging = true;
    params.progressLogging = true;
    params.benchmarkMode = false;
    params.measureWallTime = true;
    params.measureCpuTime = false;
    return params;
}

TestParameters TestParameters::performanceConfig(uint64_t neurons, uint64_t steps) {
    TestParameters params = defaultConfig();
    params.name = "Performance Test Configuration";
    params.neuronCount = neurons;
    params.maxSteps = steps;
    params.benchmarkMode = true;
    params.measureCpuTime = true;
    params.verboseLogging = false;
    params.progressLogging = false;
    return params;
}

TestParameters TestParameters::quickConfig() {
    TestParameters params = defaultConfig();
    params.name = "Quick Test Configuration";
    params.neuronCount = 100;
    params.maxSteps = 100;
    params.plasticityTestSteps = 100;
    params.stdpTestSteps = 50;
    params.learningTestSteps = 50;
    params.benchmarkMode = true;
    params.verboseLogging = false;
    params.progressLogging = false;
    return params;
}

bool TestParameters::validateRanges() const {
    if (neuronCount == 0) {
        NLM_LOG_ERROR("Error: Neuron count must be greater than 0");
        return false;
    }
    
    if (maxSteps == 0) {
        NLM_LOG_ERROR("Error: Max steps must be greater than 0");
        return false;
    }
    
    if (timestep <= 0.0) {
        NLM_LOG_ERROR("Error: Timestep must be positive");
        return false;
    }
    
    if (randomSeed == 0) {
        NLM_LOG_ERROR("Error: Random seed should be non-zero");
        return false;
    }
    
    if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
        NLM_LOG_ERROR("Error: Connection probability must be between 0 and 1");
        return false;
    }
    
    if (initialWeightMean < 0.0f || initialWeightMean > 1.0f) {
        NLM_LOG_ERROR("Error: Initial weight mean must be between 0 and 1");
        return false;
    }
    
    if (initialWeightStd < 0.0f) {
        NLM_LOG_ERROR("Error: Initial weight standard deviation must be non-negative");
        return false;
    }
    
    if (synaptogenesisRate < 0.0f) {
        NLM_LOG_ERROR("Error: Synaptogenesis rate must be non-negative");
        return false;
    }
    
    if (pruningRate < 0.0f) {
        NLM_LOG_ERROR("Error: Pruning rate must be non-negative");
        return false;
    }
    
    if (synaptogenesisRate + pruningRate > 1.0f) {
        NLM_LOG_ERROR("Error: Sum of synaptogenesis and pruning rates cannot exceed 1.0");
        return false;
    }
    
    if (stdpLtpWeight < 0.0f) {
        NLM_LOG_ERROR("Error: STDP LTP weight must be non-negative");
        return false;
    }
    
    if (stdpLtdWeight < 0.0f) {
        NLM_LOG_ERROR("Error: STDP LTD weight must be non-negative");
        return false;
    }
    
    if (stdpTau <= 0.0f) {
        NLM_LOG_ERROR("Error: STDP tau must be positive");
        return false;
    }
    
    return true;
}

bool TestParameters::validatePlasticityParams() const {
    if (!enableHebbian && !enableStdp && !enableStructuralPlasticity) {
        NLM_LOG_WARNING("Warning: All plasticity types are disabled");
    }
    
    return true;
}

bool TestParameters::validateDevelopmentParams() const {
    if (enableDevelopment) {
        if (maturationRate <= 0.0f) {
            NLM_LOG_ERROR("Error: Maturation rate must be positive when development is enabled");
            return false;
        }
    }
    
    return true;
}

bool TestParameters::validateStdpParams() const {
    if (stdpLtpWeight < 0.0f) {
        NLM_LOG_ERROR("Error: STDP LTP weight cannot be negative");
        return false;
    }
    
    if (stdpLtdWeight < 0.0f) {
        NLM_LOG_ERROR("Error: STDP LTD weight cannot be negative");
        return false;
    }
    
    return true;
}

bool TestParameters::validateConnectivityParams() const {
    if (testNeuronCount == 0) {
        NLM_LOG_ERROR("Error: Test neuron count must be greater than 0");
        return false;
    }
    
    if (testInjectionCurrent < 0.0f) {
        NLM_LOG_ERROR("Error: Test injection current must be non-negative");
        return false;
    }
    
    return true;
}

} // namespace nlm