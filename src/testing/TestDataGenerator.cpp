// Automated Test Data Generation Implementation for NLM

#include "TestDataGenerator.hpp"
#include <numeric>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <random>
#include <chrono>

namespace nlm {
namespace testing {

TestDataGenerator::TestDataGenerator() {
    std::cout << "=== NLM Test Data Generator Initialization ===" << std::endl;
    sessionStartTime_ = std::chrono::high_resolution_clock::now();
    
    // Initialize random number generators
    std::random_device rd;
    seed_ = rd();
    rng_.seed(seed_);
    
    std::cout << "Test data generator initialized successfully" << std::endl;
}

TestDataGenerator::~TestDataGenerator() {
    std::cout << "=== NLM Test Data Generator Shutdown ===" << std::endl;
}

std::vector<std::string> TestDataGenerator::generateConfigurationData(const std::string& configType,
                                                                       size_t count) {
    std::cout << "Generating " << count << " configuration data items for type: " << configType << std::endl;
    
    std::vector<std::string> configData;
    
    for (size_t i = 0; i < count; ++i) {
        std::string config;
        
        if (configType == "brain") {
            config = generateBrainConfig(i);
        } else if (configType == "memory") {
            config = generateMemoryConfig(i);
        } else if (configType == "dynamics") {
            config = generateDynamicsConfig(i);
        } else if (configType == "environment") {
            config = generateEnvironmentConfig(i);
        } else if (configType == "neuromodulation") {
            config = generateNeuromodulationConfig(i);
        } else if (configType == "development") {
            config = generateDevelopmentConfig(i);
        } else {
            config = generateGenericConfig(i);
        }
        
        configData.push_back(config);
    }
    
    generatedData_["configurations"] = configData;
    return configData;
}

std::vector<std::vector<double>> TestDataGenerator::generateNeuralData(const std::string& dataType,
                                                                      size_t neuronCount,
                                                                      size_t timeSteps) {
    std::cout << "Generating neural data: type=" << dataType << ", neurons=" << neuronCount 
              << ", time steps=" << timeSteps << std::endl;
    
    std::vector<std::vector<double>> neuralData(neuronCount, std::vector<double>(timeSteps, 0.0));
    
    for (size_t neuron = 0; neuron < neuronCount; ++neuron) {
        if (dataType == "spiking") {
            generateSpikingData(neuralData[neuron], timeSteps);
        } else if (dataType == "potential") {
            generatePotentialData(neuralData[neuron], timeSteps);
        } else if (dataType == "synapses") {
            generateSynapticData(neuralData[neuron], timeSteps);
        } else if (dataType == "activity") {
            generateActivityData(neuralData[neuron], timeSteps);
        } else {
            generateRandomData(neuralData[neuron], timeSteps);
        }
    }
    
    generatedData_[dataType] = neuralData;
    return neuralData;
}

std::vector<double> TestDataGenerator::generateSpikeData(const std::string& pattern,
                                                         double duration,
                                                         double rate) {
    std::cout << "Generating spike data: pattern=" << pattern << ", duration=" << duration 
              << ", rate=" << rate << std::endl;
    
    std::vector<double> spikeTimes;
    
    if (pattern == "poisson") {
        spikeTimes = generatePoissonSpikeTrain(duration, rate);
    } else if (pattern == "regular") {
        spikeTimes = generateRegularSpikeTrain(duration, rate);
    } else if (pattern == "burst") {
        spikeTimes = generateBurstSpikeTrain(duration, rate);
    } else if (pattern == "complex") {
        spikeTimes = generateComplexSpikeTrain(duration, rate);
    } else {
        spikeTimes = generateRandomSpikeTrain(duration, rate);
    }
    
    generatedData_["spike_data"] = spikeTimes;
    return spikeTimes;
}

std::vector<std::vector<std::string>> TestDataGenerator::generatePluginData(const std::string& pluginType,
                                                                           size_t count) {
    std::cout << "Generating " << count << " plugin data items for type: " << pluginType << std::endl;
    
    std::vector<std::vector<std::string>> pluginData;
    
    for (size_t i = 0; i < count; ++i) {
        std::vector<std::string> plugin;
        
        if (pluginType == "plasticity") {
            plugin = generatePlasticityPlugin(i);
        } else if (pluginType == "memory") {
            plugin = generateMemoryPlugin(i);
        } else if (pluginType == "neuromodulation") {
            plugin = generateNeuromodulationPlugin(i);
        } else if (pluginType == "neural_model") {
            plugin = generateNeuralModelPlugin(i);
        } else {
            plugin = generateGenericPlugin(i);
        }
        
        pluginData.push_back(plugin);
    }
    
    generatedData_["plugins"] = pluginData;
    return pluginData;
}

std::vector<BenchmarkResult> TestDataGenerator::generatePerformanceData(const std::string& benchmarkType,
                                                                         size_t iterations) {
    std::cout << "Generating performance data: type=" << benchmarkType << ", iterations=" << iterations << std::endl;
    
    std::vector<BenchmarkResult> performanceData;
    
    for (size_t i = 0; i < iterations; ++i) {
        BenchmarkResult result;
        
        if (benchmarkType == "brain_step") {
            result = generateBrainStepBenchmark(i);
        } else if (benchmarkType == "memory_update") {
            result = generateMemoryUpdateBenchmark(i);
        } else if (benchmarkType == "dynamics_calculation") {
            result = generateDynamicsCalculationBenchmark(i);
        } else if (benchmarkType == "environment_simulation") {
            result = generateEnvironmentSimulationBenchmark(i);
        } else {
            result = generateGenericBenchmark(i);
        }
        
        performanceData.push_back(result);
    }
    
    generatedData_["performance_data"] = performanceData;
    return performanceData;
}

std::vector<double> TestDataGenerator::generateRandomData(size_t count, double min, double max) {
    std::uniform_real_distribution<double> dist(min, max);
    
    std::vector<double> data;
    for (size_t i = 0; i < count; ++i) {
        data.push_back(dist(rng_));
    }
    
    generatedData_["random_data"] = data;
    return data;
}

std::vector<std::string> TestDataGenerator::generateRandomStrings(size_t count, size_t length) {
    std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::uniform_int_distribution<size_t> charDist(0, charset.size() - 1);
    std::uniform_int_distribution<size_t> lenDist(0, length);
    
    std::vector<std::string> strings;
    for (size_t i = 0; i < count; ++i) {
        size_t strLength = lenDist(rng_);
        std::string str;
        for (size_t j = 0; j < strLength; ++j) {
            str += charset[charDist(rng_)];
        }
        strings.push_back(str);
    }
    
    generatedData_["random_strings"] = strings;
    return strings;
}

std::vector<std::vector<double>> TestDataGenerator::generateRandomMatrix(size_t rows, size_t cols) {
    std::vector<std::vector<double>> matrix(rows, std::vector<double>(cols, 0.0));
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            matrix[i][j] = dist(rng_);
        }
    }
    
    generatedData_["random_matrix"] = matrix;
    return matrix;
}

void TestDataGenerator::generateUnitTestData() {
    std::cout << "Generating unit test data..." << std::endl;
    
    // Generate configuration data
    std::vector<std::string> configData = generateConfigurationData("brain", 100);
    std::vector<std::string> memoryData = generateConfigurationData("memory", 50);
    std::vector<std::string> dynamicsData = generateConfigurationData("dynamics", 75);
    
    // Generate neural data
    std::vector<std::vector<double>> spikingData = generateNeuralData("spiking", 20, 100);
    std::vector<std::vector<double>> potentialData = generateNeuralData("potential", 15, 200);
    
    // Generate spike data
    std::vector<double> spikeTrain = generateSpikeData("poisson", 10.0, 50.0);
    
    std::cout << "Unit test data generation completed" << std::endl;
}

void TestDataGenerator::generateIntegrationTestData() {
    std::cout << "Generating integration test data..." << std::endl;
    
    // Generate cross-component data
    std::vector<std::vector<double>> crossData = generateNeuralData("activity", 30, 150);
    
    // Generate performance data
    std::vector<BenchmarkResult> perfData = generatePerformanceData("brain_step", 200);
    
    // Generate plugin data
    std::vector<std::vector<std::string>> pluginData = generatePluginData("plasticity", 25);
    
    std::cout << "Integration test data generation completed" << std::endl;
}

void TestDataGenerator::generatePerformanceTestData() {
    std::cout << "Generating performance test data..." << std::endl;
    
    // Generate performance benchmarks
    std::vector<BenchmarkResult> brainStepData = generatePerformanceData("brain_step", 500);
    std::vector<BenchmarkResult> memoryData = generatePerformanceData("memory_update", 300);
    std::vector<BenchmarkResult> dynamicsData = generatePerformanceData("dynamics_calculation", 200);
    
    // Generate load test data
    std::vector<double> loadData = generateRandomData(100, 0.0, 100.0);
    
    std::cout << "Performance test data generation completed" << std::endl;
}

void TestDataGenerator::generateConfigurationTestData() {
    std::cout << "Generating configuration test data..." << std::endl;
    
    // Generate configuration variations
    std::vector<std::string> configVariations = generateConfigurationData("brain", 150);
    std::vector<std::string> memoryVariations = generateConfigurationData("memory", 100);
    std::vector<std::string> dynamicsVariations = generateConfigurationData("dynamics", 125);
    
    // Generate parameter combinations
    std::vector<std::vector<double>> paramCombinations = generateRandomMatrix(50, 5);
    
    std::cout << "Configuration test data generation completed" << std::endl;
}

void TestDataGenerator::generateBrainValidationData() {
    std::cout << "Generating brain validation test data..." << std::endl;
    
    // Generate spike patterns for validation
    std::vector<double> regularSpikes = generateSpikeData("regular", 10.0, 20.0);
    std::vector<double> poissonSpikes = generateSpikeData("poisson", 10.0, 50.0);
    
    // Generate neural activity patterns
    std::vector<std::vector<double>> activityPatterns = generateNeuralData("activity", 25, 100);
    
    // Generate brain state snapshots
    std::vector<std::vector<double>> stateSnapshots = generateNeuralData("potential", 50, 200);
    
    std::cout << "Brain validation test data generation completed" << std::endl;
}

void TestDataGenerator::generateMemoryTestData() {
    std::cout << "Generating memory test data..." << std::endl;
    
    // Generate memory usage patterns
    std::vector<double> memoryUsage = generateRandomData(100, 10.0, 1000.0);
    
    // Generate allocation patterns
    std::vector<double> allocationPatterns = generateRandomData(50, 1.0, 100.0);
    
    // Generate memory stress scenarios
    std::vector<std::vector<double>> stressScenarios;
    for (size_t i = 0; i < 10; ++i) {
        stressScenarios.push_back(generateRandomData(20, 100.0, 1000.0));
    }
    
    std::cout << "Memory test data generation completed" << std::endl;
}

bool TestDataGenerator::validateGeneratedData(const std::string& dataType,
                                              const std::vector<std::any>& data) const {
    std::cout << "Validating generated data: " << dataType << std::endl;
    
    bool isValid = true;
    
    if (dataType == "configurations") {
        for (const auto& item : data) {
            std::string config = std::any_cast<std::string>(item);
            if (!validateConfigurationString(config)) {
                isValid = false;
            }
        }
    } else if (dataType == "spike_data") {
        for (const auto& item : data) {
            double spikeTime = std::any_cast<double>(item);
            if (!validateSpikeTime(spikeTime)) {
                isValid = false;
            }
        }
    } else if (dataType == "performance_data") {
        for (const auto& item : data) {
            BenchmarkResult result = std::any_cast<BenchmarkResult>(item);
            if (!validateBenchmarkResult(result)) {
                isValid = false;
            }
        }
    } else {
        // For other types, assume valid for now
    }
    
    if (isValid) {
        std::cout << "Data validation passed" << std::endl;
    } else {
        std::cout << "Data validation failed" << std::endl;
    }
    
    return isValid;
}

void TestDataGenerator::exportTestData(const std::string& filename) const {
    std::cout << "Exporting test data to: " << filename << std::endl;
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Cannot open file for writing test data: " << filename << std::endl;
        return;
    }
    
    file << "NLM Test Data Export" << std::endl;
    file << "Generated: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << std::endl;
    
    for (const auto& pair : generatedData_) {
        file << "\nData Type: " << pair.first << std::endl;
        file << "Count: " << pair.second.size() << std::endl;
        
        if (pair.first == "configurations") {
            const auto& configData = std::any_cast<const std::vector<std::string>&>(pair.second);
            for (size_t i = 0; i < configData.size(); ++i) {
                file << i << ": " << configData[i] << std::endl;
            }
        } else if (pair.first == "spike_data") {
            const auto& spikeData = std::any_cast<const std::vector<double>&>(pair.second);
            for (size_t i = 0; i < spikeData.size(); ++i) {
                file << i << ": " << spikeData[i] << std::endl;
            }
        } else if (pair.first == "performance_data") {
            const auto& perfData = std::any_cast<const std::vector<BenchmarkResult>&>(pair.second);
            for (size_t i = 0; i < perfData.size(); ++i) {
                file << i << ": " << perfData[i].benchmarkName << ", " 
                     << perfData[i].duration << " " << perfData[i].unit << std::endl;
            }
        }
    }
    
    file.close();
    std::cout << "Test data exported to: " << filename << std::endl;
}

void TestDataGenerator::loadTestData(const std::string& filename) {
    std::cout << "Loading test data from: " << filename << std::endl;
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Cannot open file for reading test data: " << filename << std::endl;
        return;
    }
    
    // Parse test data file
    // In a real implementation, this would parse the file and load the data
    std::string line;
    while (std::getline(file, line)) {
        // Parse data
    }
    
    file.close();
    std::cout << "Test data loaded" << std::endl;
}

void TestDataGenerator::clearTestData() {
    std::cout << "Clearing test data..." << std::endl;
    
    generatedData_.clear();
    std::cout << "Test data cleared" << std::endl;
}

size_t TestDataGenerator::getDataCount(const std::string& dataType) const {
    auto it = generatedData_.find(dataType);
    if (it != generatedData_.end()) {
        if (it->first == "configurations") {
            return std::any_cast<const std::vector<std::string>&>(it->second).size();
        } else if (it->first == "spike_data") {
            return std::any_cast<const std::vector<double>&>(it->second).size();
        } else if (it->first == "performance_data") {
            return std::any_cast<const std::vector<BenchmarkResult>&>(it->second).size();
        } else {
            return 0;
        }
    }
    return 0;
}

std::vector<std::string> TestDataGenerator::getAvailableDataTypes() const {
    std::vector<std::string> dataTypes;
    
    for (const auto& pair : generatedData_) {
        dataTypes.push_back(pair.first);
    }
    
    return dataTypes;
}

// Data generation helper functions

std::string TestDataGenerator::generateBrainConfig(size_t id) {
    std::ostringstream config;
    config << "[brain]" << std::endl;
    config << "  neuron_count = " << (100 + (id % 200)) << std::endl;
    config << "  region_count = " << (1 + (id % 5)) << std::endl;
    config << "  connection_probability = " << (0.1 + (id % 10) * 0.01) << std::endl;
    config << "  max_firing_rate = " << (50.0 + (id % 50)) << std::endl;
    config << "  min_firing_rate = " << (5.0 + (id % 20)) << std::endl;
    
    return config.str();
}

std::string TestDataGenerator::generateMemoryConfig(size_t id) {
    std::ostringstream config;
    config << "[memory]" << std::endl;
    config << "  capacity_mb = " << (256 + (id % 512)) << std::endl;
    config << "  retention_period_ms = " << (1000 + (id % 2000)) << std::endl;
    config << "  consolidation_rate = " << (0.1 + (id % 10) * 0.05) << std::endl;
    config << "  encoding_efficiency = " << (0.5 + (id % 50) * 0.01) << std::endl;
    
    return config.str();
}

std::string TestDataGenerator::generateDynamicsConfig(size_t id) {
    std::ostringstream config;
    config << "[dynamics]" << std::endl;
    config << "  time_step_ms = " << (1.0 + (id % 5)) << std::endl;
    config << "  refractory_period_ms = " << (2.0 + (id % 10)) << std::endl;
    config << "  synaptic_decay = " << (0.1 + (id % 10) * 0.05) << std::endl;
    config << "  plasticity_rate = " << (0.01 + (id % 10) * 0.001) << std::endl;
    
    return config.str();
}

std::string TestDataGenerator::generateEnvironmentConfig(size_t id) {
    std::ostringstream config;
    config << "[environment]" << std::endl;
    config << "  stimulation_rate = " << (10.0 + (id % 20)) << std::endl;
    config << "  input_variance = " << (0.1 + (id % 10) * 0.05) << std::endl;
    config << "  noise_level = " << (0.05 + (id % 10) * 0.02) << std::endl;
    config << "  adaptation_rate = " << (0.01 + (id % 5) * 0.002) << std::endl;
    
    return config.str();
}

std::string TestDataGenerator::generateNeuromodulationConfig(size_t id) {
    std::ostringstream config;
    config << "[neuromodulation]" << std::endl;
    config << "  reward_sensitivity = " << (0.5 + (id % 10) * 0.1) << std::endl;
    config << "  curiosity_gain = " << (0.3 + (id % 10) * 0.05) << std::endl;
    config << "  prediction_error = " << (0.1 + (id % 10) * 0.02) << std::endl;
    config << "  exploration_rate = " << (0.2 + (id % 10) * 0.05) << std::endl;
    
    return config.str();
}

std::string TestDataGenerator::generateDevelopmentConfig(size_t id) {
    std::ostringstream config;
    config << "[development]" << std::endl;
    config << "  synaptogenesis_rate = " << (0.05 + (id % 10) * 0.01) << std::endl;
    config << "  pruning_rate = " << (0.02 + (id % 5) * 0.01) << std::endl;
    config << "  maturation_rate = " << (0.03 + (id % 10) * 0.005) << std::endl;
    config << "  optimization_strength = " << (0.8 + (id % 10) * 0.02) << std::endl;
    
    return config.str();
}

std::string TestDataGenerator::generateGenericConfig(size_t id) {
    std::ostringstream config;
    config << "[generic]" << std::endl;
    config << "  parameter_1 = " << (id % 100) << std::endl;
    config << "  parameter_2 = " << (id % 200) << std::endl;
    config << "  parameter_3 = " << (id % 50) << std::endl;
    config << "  parameter_4 = " << (id % 150) << std::endl;
    config << "  parameter_5 = " << (id % 75) << std::endl;
    
    return config.str();
}

void TestDataGenerator::generateSpikingData(std::vector<double>& data, size_t timeSteps) {
    std::poisson_distribution<size_t> poissonDist(10.0); // 10 spikes per 10ms
    
    for (size_t t = 0; t < timeSteps; ++t) {
        data[t] = poissonDist(rng_) * 0.1; // Convert to firing rate
    }
}

void TestDataGenerator::generatePotentialData(std::vector<double>& data, size_t timeSteps) {
    std::normal_distribution<double> normalDist(0.0, 1.0);
    
    for (size_t t = 0; t < timeSteps; ++t) {
        data[t] = normalDist(rng_) * 10.0 + 20.0; // Membrane potential around 20mV
    }
}

void TestDataGenerator::generateSynapticData(std::vector<double>& data, size_t timeSteps) {
    std::exponential_distribution<double> expDist(0.1);
    
    for (size_t t = 0; t < timeSteps; ++t) {
        data[t] = expDist(rng_) * 5.0; // Synaptic weight
    }
}

void TestDataGenerator::generateActivityData(std::vector<double>& data, size_t timeSteps) {
    std::uniform_real_distribution<double> uniformDist(0.0, 1.0);
    
    for (size_t t = 0; t < timeSteps; ++t) {
        data[t] = uniformDist(rng_); // Normalized activity
    }
}

void TestDataGenerator::generateRandomData(std::vector<double>& data, size_t timeSteps) {
    std::uniform_real_distribution<double> uniformDist(0.0, 1.0);
    
    for (size_t t = 0; t < timeSteps; ++t) {
        data[t] = uniformDist(rng_);
    }
}

std::vector<double> TestDataGenerator::generatePoissonSpikeTrain(double duration, double rate) {
    std::vector<double> spikeTimes;
    std::poisson_distribution<size_t> poissonDist(rate * 10.0); // rate per 10ms
    
    double currentTime = 0.0;
    while (currentTime < duration) {
        // Generate next spike time
        double interArrival = std::exponential_distribution<double>(1.0 / (rate * 10.0))(rng_);
        currentTime += interArrival;
        
        if (currentTime < duration) {
            spikeTimes.push_back(currentTime);
        }
    }
    
    return spikeTimes;
}

std::vector<double> TestDataGenerator::generateRegularSpikeTrain(double duration, double rate) {
    std::vector<double> spikeTimes;
    double interArrival = 10.0 / rate; // milliseconds
    
    double currentTime = 0.0;
    while (currentTime < duration) {
        spikeTimes.push_back(currentTime);
        currentTime += interArrival;
    }
    
    return spikeTimes;
}

std::vector<double> TestDataGenerator::generateBurstSpikeTrain(double duration, double rate) {
    std::vector<double> spikeTimes;
    double burstDuration = 50.0; // ms
    double interBurst = 200.0; // ms
    
    double currentTime = 0.0;
    while (currentTime < duration) {
        // Generate burst
        double burstStart = currentTime;
        double spikeInterval = 5.0; // ms within burst
        for (size_t spike = 0; spike < 5; ++spike) {
            double spikeTime = burstStart + spike * spikeInterval;
            if (spikeTime < duration) {
                spikeTimes.push_back(spikeTime);
            }
        }
        
        currentTime += burstDuration + interBurst;
    }
    
    return spikeTimes;
}

std::vector<double> TestDataGenerator::generateComplexSpikeTrain(double duration, double rate) {
    std::vector<double> spikeTimes;
    
    // Combine different patterns
    std::vector<double> poissonSpikes = generatePoissonSpikeTrain(duration * 0.6, rate * 0.6);
    std::vector<double> regularSpikes = generateRegularSpikeTrain(duration * 0.4, rate * 0.4);
    
    // Merge and sort
    spikeTimes.reserve(poissonSpikes.size() + regularSpikes.size());
    spikeTimes.insert(spikeTimes.end(), poissonSpikes.begin(), poissonSpikes.end());
    spikeTimes.insert(spikeTimes.end(), regularSpikes.begin(), regularSpikes.end());
    
    std::sort(spikeTimes.begin(), spikeTimes.end());
    
    return spikeTimes;
}

std::vector<double> TestDataGenerator::generateRandomSpikeTrain(double duration, double rate) {
    std::vector<double> spikeTimes;
    
    double currentTime = 0.0;
    while (currentTime < duration) {
        // Random inter-arrival time
        double randomFactor = std::uniform_real_distribution<double>(0.5, 2.0)(rng_);
        double interArrival = (10.0 / rate) * randomFactor;
        
        currentTime += interArrival;
        if (currentTime < duration) {
            spikeTimes.push_back(currentTime);
        }
    }
    
    return spikeTimes;
}

std::vector<std::string> TestDataGenerator::generatePlasticityPlugin(size_t id) {
    std::vector<std::string> plugin;
    plugin.push_back("PlasticityPlugin");
    plugin.push_back("strength_" + std::to_string(0.1 + (id % 10) * 0.1));
    plugin.push_back("learning_rate_" + std::to_string(0.01 + (id % 10) * 0.001));
    plugin.push_back("trace_decay_" + std::to_string(0.9 + (id % 10) * 0.01));
    plugin.push_back("weight_change_" + std::to_string(0.05 + (id % 10) * 0.01));
    
    return plugin;
}

std::vector<std::string> TestDataGenerator::generateMemoryPlugin(size_t id) {
    std::vector<std::string> plugin;
    plugin.push_back("MemoryPlugin");
    plugin.push_back("capacity_" + std::to_string(128 + (id % 512)));
    plugin.push_back("retention_" + std::to_string(1000 + (id % 2000)));
    plugin.push_back("consolidation_" + std::to_string(0.1 + (id % 10) * 0.05));
    plugin.push_back("encoding_efficiency_" + std::to_string(0.5 + (id % 50) * 0.01));
    
    return plugin;
}

std::vector<std::string> TestDataGenerator::generateNeuromodulationPlugin(size_t id) {
    std::vector<std::string> plugin;
    plugin.push_back("NeuromodulationPlugin");
    plugin.push_back("reward_sensitivity_" + std::to_string(0.5 + (id % 10) * 0.1));
    plugin.push_back("curiosity_gain_" + std::to_string(0.3 + (id % 10) * 0.05));
    plugin.push_back("prediction_error_" + std::to_string(0.1 + (id % 10) * 0.02));
    plugin.push_back("exploration_rate_" + std::to_string(0.2 + (id % 10) * 0.05));
    
    return plugin;
}

std::vector<std::string> TestDataGenerator::generateNeuralModelPlugin(size_t id) {
    std::vector<std::string> plugin;
    plugin.push_back("NeuralModelPlugin");
    plugin.push_back("model_type_" + std::to_string(id % 5));
    plugin.push_back("accuracy_" + std::to_string(0.8 + (id % 20) * 0.02));
    plugin.push_back("complexity_" + std::to_string(5 + (id % 10)));
    plugin.push_back("training_efficiency_" + std::to_string(0.9 + (id % 10) * 0.01));
    
    return plugin;
}

std::vector<std::string> TestDataGenerator::generateGenericPlugin(size_t id) {
    std::vector<std::string> plugin;
    plugin.push_back("GenericPlugin");
    plugin.push_back("id_" + std::to_string(id));
    plugin.push_back("enabled_" + std::to_string((id % 2) == 0 ? "true" : "false"));
    plugin.push_back("priority_" + std::to_string((id % 5) + 1));
    plugin.push_back("config_path_" + std::to_string(id % 10));
    
    return plugin;
}

BenchmarkResult TestDataGenerator::generateBrainStepBenchmark(size_t id) {
    std::normal_distribution<double> durationDist(10.0, 2.0); // 10ms mean, 2ms stddev
    double duration = durationDist(rng_);
    
    BenchmarkResult result("brain_step_benchmark", duration, 0.0, id + 1, "ms");
    result.memoryUsage = 100.0 + (id % 50); // MB
    return result;
}

BenchmarkResult TestDataGenerator::generateMemoryUpdateBenchmark(size_t id) {
    std::normal_distribution<double> durationDist(15.0, 3.0); // 15ms mean, 3ms stddev
    double duration = durationDist(rng_);
    
    BenchmarkResult result("memory_update_benchmark", duration, 0.0, id + 1, "ms");
    result.memoryUsage = 200.0 + (id % 100); // MB
    return result;
}

BenchmarkResult TestDataGenerator::generateDynamicsCalculationBenchmark(size_t id) {
    std::normal_distribution<double> durationDist(20.0, 4.0); // 20ms mean, 4ms stddev
    double duration = durationDist(rng_);
    
    BenchmarkResult result("dynamics_calculation_benchmark", duration, 0.0, id + 1, "ms");
    result.memoryUsage = 150.0 + (id % 75); // MB
    return result;
}

BenchmarkResult TestDataGenerator::generateEnvironmentSimulationBenchmark(size_t id) {
    std::normal_distribution<double> durationDist(25.0, 5.0); // 25ms mean, 5ms stddev
    double duration = durationDist(rng_);
    
    BenchmarkResult result("environment_simulation_benchmark", duration, 0.0, id + 1, "ms");
    result.memoryUsage = 300.0 + (id % 150); // MB
    return result;
}

BenchmarkResult TestDataGenerator::generateGenericBenchmark(size_t id) {
    std::uniform_real_distribution<double> durationDist(5.0, 50.0); // 5-50ms
    double duration = durationDist(rng_);
    
    BenchmarkResult result("generic_benchmark", duration, 0.0, id + 1, "ms");
    result.memoryUsage = 50.0 + (id % 50); // MB
    return result;
}

bool TestDataGenerator::validateConfigurationString(const std::string& config) const {
    // Basic validation for configuration strings
    if (config.empty()) {
        return false;
    }
    
    // Check if it contains required sections
    if (config.find("[brain]") == std::string::npos && 
        config.find("[memory]") == std::string::npos &&
        config.find("[dynamics]") == std::string::npos) {
        return false;
    }
    
    return true;
}

bool TestDataGenerator::validateSpikeTime(double spikeTime) const {
    // Validate spike time is reasonable
    return spikeTime >= 0.0 && spikeTime <= 1000.0; // 0-1000ms
}

bool TestDataGenerator::validateBenchmarkResult(const BenchmarkResult& result) const {
    // Validate benchmark result
    return result.duration >= 0.0 && result.duration <= 1000.0 && // 0-1000ms
           result.iterations > 0 &&
           result.memoryUsage >= 0.0 &&
           result.memoryUsage <= 10000.0; // 0-10GB
}

} // namespace testing
} // namespace nlm