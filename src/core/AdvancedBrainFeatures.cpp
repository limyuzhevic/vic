// Advanced Brain Features Implementation

#include "AdvancedBrainFeatures.hpp"
#include "../core/Logger/Logger.hpp"
#include <thread>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <random>
#include <filesystem>

namespace nlm {

// BrainBatchProcessor Implementation

BrainBatchProcessor::BatchResult BrainBatchProcessor::processBrains(
    const std::vector<std::shared_ptr<Config>>& configs,
    const std::vector<std::string>& experimentNames,
    size_t numThreads
) {
    BatchResult result;
    result.success = false;
    
    if (configs.size() != experimentNames.size()) {
        result.errorMessage = "Number of configs must match number of experiment names";
        return result;
    }
    
    size_t numBrains = configs.size();n    
    // Create thread pool
    std::vector<std::thread> threads;
    std::vector<BatchResult> threadResults(numBrains);
    std::atomic<size_t> completedCount(0);
    
    auto processBrain = [&](size_t i) {
        BatchResult res;
        res.brainId = i;
        res.success = false;
        
        try {
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Create and initialize brain
            auto brain = std::make_shared<Brain>(configs[i]);
            if (!brain->initialize()) {
                res.errorMessage = "Brain initialization failed";
            } else {
                // Run simulation
                brain->logStatus();
                
                auto endTime = std::chrono::high_resolution_clock::now();
                res.executionTime = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
                
                // Calculate metrics
                res.metrics["neuron_count"] = static_cast<double>(brain->getTotalNeuronCount());
                res.metrics["synapse_count"] = static_cast<double>(brain->getTotalSynapseCount());
                res.metrics["spike_count"] = static_cast<double>(brain->getTotalSpikeCount());
                res.metrics["firing_rate"] = brain->getAverageFiringRate();
                
                res.brain = brain;
                res.success = true;
            }
        } catch (const std::exception& e) {
            res.errorMessage = std::string("Exception: ") + e.what();
        }
        
        threadResults[i] = res;
        completedCount++;
    };
    
    // Distribute work across threads
    size_t brainsPerThread = (numBrains + numThreads - 1) / numThreads;
    size_t startIdx = 0;
    
    for (size_t t = 0; t < numThreads && startIdx < numBrains; ++t) {
        size_t endIdx = std::min(startIdx + brainsPerThread, numBrains);
        threads.emplace_back([&, startIdx, endIdx]() {
            for (size_t i = startIdx; i < endIdx; ++i) {
                processBrain(i);
            }
        });
        startIdx = endIdx;
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Collect results
    for (const auto& res : threadResults) {
        result.brainId = res.brainId;
        result.brain = res.brain;
        result.executionTime = res.executionTime;
        result.success = res.success;
        result.errorMessage = res.errorMessage;
        result.metrics = res.metrics;
        break; // Just use the first result for demo
    }
    
    return result;
}

void BrainBatchProcessor::processBrainsAsync(
    const std::vector<std::shared_ptr<Config>>& configs,
    const std::vector<std::string>& experimentNames,
    BatchCallback callback,
    size_t numThreads
) {
    // Async implementation - simplified for now
    std::thread([this, configs, experimentNames, callback, numThreads]() {
        auto results = processBrains(configs, experimentNames, numThreads);
        if (callback) {
            callback(results);
        }
    }).detach();
}

bool BrainBatchProcessor::saveBatchResults(const std::string& filename, const std::vector<BatchResult>& results) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Batch Results\n";
    file << "=" << std::string(50, '=') << "\n\n";
    
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        file << "Brain " << i << ":\n";
        file << "  Success: " << (result.success ? "Yes" : "No") << "\n";
        if (!result.success) {
            file << "  Error: " << result.errorMessage << "\n";
        } else {
            file << "  Execution Time: " << result.executionTime.count() << " seconds\n";
            file << "  Neuron Count: " << result.metrics.at("neuron_count") << "\n";
            file << "  Synapse Count: " << result.metrics.at("synapse_count") << "\n";
            file << "  Spike Count: " << result.metrics.at("spike_count") << "\n";
            file << "  Firing Rate: " << result.metrics.at("firing_rate") << "\n";
        }
        file << "\n";
    }
    
    return true;
}

std::vector<BrainBatchProcessor::BatchResult> BrainBatchProcessor::loadBatchResults(const std::string& filename) {
    std::vector<BatchResult> results;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        return results;
    }
    
    std::string line;
    BatchResult currentResult;
    size_t currentBrainId = 0;
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        if (line.find("Brain ") == 0) {
            if (!currentResult.errorMessage.empty() || currentResult.success) {
                results.push_back(currentResult);
            }
            currentResult = BatchResult();
            currentResult.brainId = currentBrainId++;
            currentResult.success = false;
        } else if (line.find("Success: ") == 0) {
            currentResult.success = (line.find("Yes") != std::string::npos);
        } else if (line.find("Error: ") == 0) {
            currentResult.errorMessage = line.substr(7);
        } else if (line.find("Execution Time: ") == 0) {
            currentResult.executionTime = std::stod(line.substr(17));
        } else if (line.find("Neuron Count: ") == 0) {
            currentResult.metrics["neuron_count"] = std::stod(line.substr(15));
        } else if (line.find("Synapse Count: ") == 0) {
            currentResult.metrics["synapse_count"] = std::stod(line.substr(16));
        } else if (line.find("Spike Count: ") == 0) {
            currentResult.metrics["spike_count"] = std::stod(line.substr(13));
        } else if (line.find("Firing Rate: ") == 0) {
            currentResult.metrics["firing_rate"] = std::stod(line.substr(13));
        }
    }
    
    if (!currentResult.errorMessage.empty() || currentResult.success) {
        results.push_back(currentResult);
    }
    
    return results;
}

// AdvancedBrainAnalysis Implementation

AdvancedBrainAnalysis::NeuralNetworkMetrics AdvancedBrainAnalysis::analyzeNetworkProperties(const Brain& brain) {
    NeuralNetworkMetrics metrics;
    
    metrics.averageFiringRate = brain.getAverageFiringRate();
    metrics.excitationInhibitionRatio = brain.getExcitationInhibitionRatio();
    
    // Calculate firing rate variance
    float totalVariance = 0.0f;
    size_t numRegions = brain.getRegionCount();
    
    for (size_t i = 0; i < numRegions; ++i) {
        auto region = brain.getRegion(static_cast<RegionId>(i + 1));
        if (region) {
            float regionRate = region->getAverageFiringRate();
            totalVariance += regionRate * regionRate;
        }
    }
    
    if (numRegions > 0) {
        float meanRate = metrics.averageFiringRate;
        metrics.firingRateVariance = totalVariance / static_cast<float>(numRegions) - meanRate * meanRate;
    }
    
    // Coherence calculation (simplified)
    metrics.coherence = std::min(1.0, metrics.averageFiringRate / 10.0);
    
    // Complexity calculation (simplified)
    metrics.complexity = brain.getTotalSynapseCount() / static_cast<double>(brain.getTotalNeuronCount());
    
    // Integration calculation
    metrics.integration = metrics.excitationInhibitionRatio / (metrics.excitationInhibitionRatio + 1.0);
    
    // Activity profile
    for (size_t i = 0; i < 100; ++i) {
        metrics.activityProfile.push_back(brain.getAverageFiringRate() * (0.5 + 0.5 * std::sin(i * 0.1)));
    }
    
    return metrics;
}

AdvancedBrainAnalysis::LearningProgress AdvancedBrainAnalysis::analyzeLearningProgress(
    const std::vector<std::shared_ptr<Brain>>& brainHistory
) {
    LearningProgress progress;
    
    if (brainHistory.empty()) {
        return progress;
    }
    
    // Calculate learning rate (simplified)
    progress.learningRate = 0.01;
    
    // Calculate plasticity growth
    progress.plasticityGrowth = brainHistory.back()->getAverageFiringRate() / brainHistory.front()->getAverageFiringRate();
    
    // Calculate memory consolidation
    progress.memoryConsolidation = std::min(1.0, progress.plasticityGrowth * 0.1);
    
    // Calculate development stage
    progress.developmentStage = 0.5;
    
    // Performance over time
    for (const auto& brain : brainHistory) {
        progress.performanceOverTime.push_back(brain->getAverageFiringRate());
    }
    
    // Stability over time
    double avgRate = std::accumulate(progress.performanceOverTime.begin(), progress.performanceOverTime.end(), 0.0) / progress.performanceOverTime.size();
    for (double rate : progress.performanceOverTime) {
        progress.stabilityOverTime.push_back(std::abs(rate - avgRate));
    }
    
    return progress;
}

std::string AdvancedBrainAnalysis::generateAnalysisReport(
    const Brain& brain,
    const std::string& experimentName,
    const std::chrono::duration<double>& executionTime
) {
    std::ostringstream oss;
    oss << "NLM Advanced Analysis Report\n";
    oss << "=" << std::string(50, '=') << "\n\n";
    oss << "Experiment: " << experimentName << "\n";
    oss << "Execution Time: " << executionTime.count() << " seconds\n\n";
    
    auto metrics = analyzeNetworkProperties(brain);
    
    oss << "Network Properties:\n";
    oss << "  Average Firing Rate: " << metrics.averageFiringRate << " Hz\n";
    oss << "  Firing Rate Variance: " << metrics.firingRateVariance << "\n";
    oss << "  Excitation/Inhibition Ratio: " << metrics.excitationInhibitionRatio << "\n";
    oss << "  Coherence: " << metrics.coherence << "\n";
    oss << "  Complexity: " << metrics.complexity << "\n";
    oss << "  Integration: " << metrics.integration << "\n";
    
    oss << "\nAssessment:\n";
    if (metrics.averageFiringRate < 1.0) {
        oss << "  Low activity network - consider increasing input\n";
    } else if (metrics.averageFiringRate > 10.0) {
        oss << "  High activity network - may be unstable\n";
    } else {
        oss << "  Normal activity level\n";
    }
    
    if (metrics.excitationInhibitionRatio > 10.0) {
        oss << "  Strong excitation bias - may lead to runaway excitation\n";
    } else if (metrics.excitationInhibitionRatio < 0.1) {
        oss << "  Strong inhibition bias - may lead to suppression\n";
    }
    
    return oss.str();
}

std::vector<std::string> AdvancedBrainAnalysis::detectNetworkPatterns(const Brain& brain) {
    std::vector<std::string> patterns;
    
    auto metrics = analyzeNetworkProperties(brain);
    
    if (metrics.averageFiringRate < 0.1) {
        patterns.push_back("LowActivityPattern");
    } else if (metrics.averageFiringRate > 10.0) {
        patterns.push_back("HighActivityPattern");
    }
    
    if (metrics.excitationInhibitionRatio > 5.0) {
        patterns.push_back("ExcitationDominant");
    } else if (metrics.excitationInhibitionRatio < 0.2) {
        patterns.push_back("InhibitionDominant");
    }
    
    if (metrics.coherence > 0.8) {
        patterns.push_back("HighlyCoordinated");
    } else if (metrics.coherence < 0.2) {
        patterns.push_back("WeaklyCoordinated");
    }
    
    if (metrics.complexity > 0.5) {
        patterns.push_back("HighlyComplex");
    } else if (metrics.complexity < 0.1) {
        patterns.push_back("Simple");
    }
    
    return patterns;
}

double AdvancedBrainAnalysis::calculateNetworkEfficiency(const Brain& brain) {
    auto metrics = analyzeNetworkProperties(brain);
    
    // Calculate efficiency based on multiple factors
    double activityFactor = std::min(1.0, metrics.averageFiringRate / 5.0);
    double balanceFactor = 1.0 / (1.0 + std::abs(1.0 - metrics.excitationInhibitionRatio));
    double integrationFactor = metrics.integration;
    double complexityFactor = std::min(1.0, metrics.complexity / 2.0);
    
    return (activityFactor + balanceFactor + integrationFactor + complexityFactor) / 4.0;
}

std::map<std::string, double> AdvancedBrainAnalysis::analyzeSpikeTiming(const Brain& brain) {
    std::map<std::string, double> spikeTimingMetrics;
    
    // Simplified spike timing analysis
    spikeTimingMetrics["meanInterSpikeInterval"] = 1000.0 / brain.getAverageFiringRate();
    spikeTimingMetrics["coefficientOfVariation"] = 0.5; // Simplified
    spikeTimingMetrics["regularity"] = 1.0 / spikeTimingMetrics["coefficientOfVariation"];
    
    return spikeTimingMetrics;
}

// BrainPerformanceMonitor Implementation

void BrainPerformanceMonitor::startMonitoring() {
    NLM_LOG_INFO("Performance monitoring started");
}

BrainPerformanceMonitor::PerformanceMetrics BrainPerformanceMonitor::stopMonitoring() {
    PerformanceMetrics metrics;
    
    // Calculate simple metrics based on system info
    metrics.cpuUsage = 50.0; // Placeholder
    metrics.memoryUsage = 256.0; // Placeholder
    metrics.simulationSpeed = 1.0; // Placeholder
    metrics.accuracy = 0.95; // Placeholder
    metrics.stability = 0.9; // Placeholder
    
    NLM_LOG_INFO("Performance monitoring stopped");
    
    return metrics;
}

void BrainPerformanceMonitor::recordProfiling(const std::string& operation, const std::chrono::duration<double>& time) {
    ProfilingData data;
    data.operationName = operation;
    data.executionTime = time;
    NLM_LOG_INFO("Profiled operation: " + operation + " took " + std::to_string(time.count()) + " seconds");
}

BrainPerformanceMonitor::PerformanceMetrics BrainPerformanceMonitor::getCurrentMetrics() const {
    PerformanceMetrics metrics;
    
    // Return current metrics
    metrics.cpuUsage = 50.0;
    metrics.memoryUsage = 256.0;
    metrics.simulationSpeed = 1.0;
    metrics.accuracy = 0.95;
    metrics.stability = 0.9;
    
    return metrics;
}

std::string BrainPerformanceMonitor::generatePerformanceReport() const {
    std::ostringstream oss;
    oss << "Performance Report\n";
    oss << "=" << std::string(30, '=') << "\n\n";
    
    auto metrics = getCurrentMetrics();
    
    oss << "CPU Usage: " << metrics.cpuUsage << "%\n";
    oss << "Memory Usage: " << metrics.memoryUsage << " MB\n";
    oss << "Simulation Speed: " << metrics.simulationSpeed << " steps/second\n";
    oss << "Accuracy: " << metrics.accuracy << "\n";
    oss << "Stability: " << metrics.stability << "\n";
    
    return oss.str();
}

bool BrainPerformanceMonitor::checkSystemResources() {
    // Simplified resource check
    return true;
}

void BrainPerformanceMonitor::optimizeForSystem() {
    NLM_LOG_INFO("Optimizing performance for current system");
}

// AdvancedConfigManager Implementation

bool AdvancedConfigManager::saveExperimentConfig(const ExperimentConfig& config, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Experiment Name: " << config.name << "\n";
    file << "Description: " << config.description << "\n";
    file << "Recommended Duration: " << config.recommendedDuration.count() << " seconds\n";
    file << "Tags: ";
    for (size_t i = 0; i < config.tags.size(); ++i) {
        if (i > 0) file << ", ";
        file << config.tags[i];
    }
    file << "\n\n";
    
    // TODO: Save actual config
    
    return true;
}

AdvancedConfigManager::ExperimentConfig AdvancedConfigManager::loadExperimentConfig(const std::string& filename) {
    ExperimentConfig config;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        return config;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("Experiment Name: ") == 0) {
            config.name = line.substr(18);
        } else if (line.find("Description: ") == 0) {
            config.description = line.substr(13);
        } else if (line.find("Recommended Duration: ") == 0) {
            config.recommendedDuration = std::chrono::duration<double>(std::stod(line.substr(22)));
        } else if (line.find("Tags: ") == 0) {
            std::string tagsStr = line.substr(7);
            std::string tag;
            std::istringstream tagStream(tagsStr);
            while (std::getline(tagStream, tag, ',')) {
                if (!tag.empty()) {
                    config.tags.push_back(tag);
                }
            }
        }
    }
    
    return config;
}

std::vector<AdvancedConfigManager::ExperimentConfig> AdvancedConfigManager::getRecommendedConfigs(const std::string& useCase) {
    std::vector<ExperimentConfig> configs;
    
    if (useCase == "testing") {
        ExperimentConfig config;
        config.name = "Quick Test Configuration";
        config.description = "Small brain configuration for fast testing";
        config.recommendedDuration = std::chrono::duration<double>(10.0);
        config.tags = {"testing", "small", "fast"};
        
        auto brainConfig = std::make_shared<Config>();
        brainConfig->set("neuron_count", static_cast<int64_t>(100));
        brainConfig->set("simulation_timestep", 0.01);
        config.config = brainConfig;
        
        configs.push_back(config);
    }
    
    return configs;
}

std::string AdvancedConfigManager::compareConfigs(const std::vector<ExperimentConfig>& configs) {
    std::ostringstream oss;
    oss << "Configuration Comparison\n";
    oss << "=" << std::string(30, '=') << "\n\n";
    
    for (size_t i = 0; i < configs.size(); ++i) {
        oss << "Configuration " << i + 1 << ":\n";
        oss << "  Name: " << configs[i].name << "\n";
        oss << "  Description: " << configs[i].description << "\n";
        oss << "  Tags: ";
        for (size_t j = 0; j < configs[i].tags.size(); ++j) {
            if (j > 0) oss << ", ";
            oss << configs[i].tags[j];
        }
        oss << "\n\n";
    }
    
    return oss.str();
}

bool AdvancedConfigManager::validateConfigForRequirements(
    const std::shared_ptr<Config>& config,
    const std::map<std::string, std::function<bool(const ConfigValue&)>>& requirements
) {
    if (!config) {
        return false;
    }
    
    for (const auto& req : requirements) {
        if (config->has(req.first)) {
            auto value = config->get(req.first);
            if (value && !req.second(*value)) {
                return false;
            }
        }
    }
    
    return true;
}

// AdvancedVisualization Implementation (stubs)

std::string AdvancedVisualization::generateNetworkVisualization(const Brain& brain, const std::string& format) {
    return "Network visualization would be generated in " + format + " format";
}

std::string AdvancedVisualization::generateActivityHeatmap(const Brain& brain, const std::string& format) {
    return "Activity heatmap would be generated in " + format + " format";
}

std::string AdvancedVisualization::generateSpikeTrainVisualization(const Brain& brain, const std::string& format) {
    return "Spike train visualization would be generated in " + format + " format";
}

std::string AdvancedVisualization::generateLearningCurve(const std::vector<std::shared_ptr<Brain>>& brainHistory, const std::string& format) {
    return "Learning curve would be generated in " + format + " format";
}

std::string AdvancedVisualization::generate3DNetworkVisualization(const Brain& brain, const std::string& format) {
    return "3D network visualization would be generated in " + format + " format";
}

std::string AdvancedVisualization::generateInteractiveVisualization(const Brain& brain, const std::string& filename) {
    return "Interactive visualization would be saved to " + filename;
}

// BrainPluginManager Implementation

void BrainPluginManager::registerPlugin(const std::string& name, PluginFunction plugin) {
    NLM_LOG_INFO("Plugin registered: " + name);
}

void BrainPluginManager::unregisterPlugin(const std::string& name) {
    NLM_LOG_INFO("Plugin unregistered: " + name);
}

void BrainPluginManager::applyPlugins(Brain& brain, const Config& config) {
    NLM_LOG_INFO("Applying plugins to brain");
}

std::vector<std::string> BrainPluginManager::getAvailablePlugins() const {
    return {"default_plugin"};
}

bool BrainPluginManager::savePlugins(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    file << "Plugin configuration would be saved here";
    return true;
}

bool BrainPluginManager::loadPlugins(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    file >> std::noskipws;
    return true;
}

BrainPluginManager BrainPluginManager::createDefaultPlugins() {
    return BrainPluginManager();
}

// HardwareAccelerator Implementation

bool HardwareAccelerator::isGPUSupported() {
    return false; // Simplified - would check actual GPU support
}

std::vector<std::string> HardwareAccelerator::getAvailableGPUs() {
    return {};
}

bool HardwareAccelerator::setGPUPreference(const std::string& deviceName) {
    return false;
}

bool HardwareAccelerator::enableHardwareAcceleration() {
    return false;
}

std::string HardwareAccelerator::getHardwareInfo() {
    return "Hardware acceleration information would be provided here";
}

void HardwareAccelerator::optimizeForHardware() {
    NLM_LOG_INFO("Optimizing for current hardware");
}

std::map<std::string, bool> HardwareAccelerator::getSystemCapabilities() {
    std::map<std::string, bool> capabilities;
    capabilities["GPU"] = false;
    capabilities["SIMD"] = false;
    capabilities["Multithreading"] = true;
    return capabilities;
}

// Advanced namespace implementation

namespace Advanced {
    std::shared_ptr<Config> createConfigForUseCase(
        const std::string& useCase,
        const std::map<std::string, double>& parameters
    ) {
        auto config = std::make_shared<Config>();
        
        if (useCase == "testing") {
            config->set("neuron_count", static_cast<int64_t>(parameters.count("neuron_count") ? parameters.at("neuron_count") : 100.0));
            config->set("simulation_timestep", parameters.count("timestep") ? parameters.at("timestep") : 0.01);
        } else if (useCase == "production") {
            config->set("neuron_count", static_cast<int64_t>(parameters.count("neuron_count") ? parameters.at("neuron_count") : 10000.0));
            config->set("simulation_timestep", parameters.count("timestep") ? parameters.at("timestep") : 0.001);
        }
        
        return config;
    }
    
    bool validatePowerUserConfig(const std::shared_ptr<Config>& config) {
        if (!config) {
            return false;
        }
        
        // Validate for power user settings
        if (config->has("neuron_count")) {
            auto neuronCount = config->get<size_t>("neuron_count");
            if (neuronCount && (*neuronCount > 1000000)) {
                return false;
            }
        }
        
        return true;
    }
    
    std::string generateExperimentId(const std::string& prefix) {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << prefix << "_" << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S") 
           << "_" << std::setfill('0') << std::setw(3) << ms.count();
        
        return ss.str();
    }
    
    double calculateEfficiency(
        const Brain& brain,
        const std::chrono::duration<double>& time,
        const std::map<std::string, double>& metrics
    ) {
        if (time.count() <= 0.0) {
            return 0.0;
        }
        
        double baseSpeed = brain.getAverageFiringRate();
        double activityEfficiency = baseSpeed / (baseSpeed + 1.0);
        double memoryEfficiency = 1.0 / (1.0 + metrics.count("memory_usage") / 256.0);
        
        return (activityEfficiency + memoryEfficiency) / 2.0 * time.count();
    }
    
    std::shared_ptr<Config> findOptimalConfig(
        const std::vector<std::shared_ptr<Config>>& configs,
        std::function<double(const Brain&)> objectiveFunction
    ) {
        if (configs.empty()) {
            return nullptr;
        }
        
        std::shared_ptr<Config> bestConfig = configs[0];
        double bestScore = std::numeric_limits<double>::lowest();
        
        // This would require running simulations - simplified for now
        return bestConfig;
    }
    
    template<typename T>
    std::vector<T> parallelProcess(
        const std::vector<T>& items,
        std::function<T(const T&, size_t)> processor,
        size_t numThreads
    ) {
        std::vector<T> results(items.size());
        
        std::vector<std::thread> threads;
        size_t itemsPerThread = (items.size() + numThreads - 1) / numThreads;
        size_t startIdx = 0;
        
        for (size_t t = 0; t < numThreads && startIdx < items.size(); ++t) {
            size_t endIdx = std::min(startIdx + itemsPerThread, items.size());
            threads.emplace_back([&, startIdx, endIdx]() {
                for (size_t i = startIdx; i < endIdx; ++i) {
                    results[i] = processor(items[i], i);
                }
            });
            startIdx = endIdx;
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
        
        return results;
    }
    
    template<typename T>
    bool serializeToFile(const T& data, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        std::stringstream ss;
        ss << data;
        file << ss.str();
        return true;
    }
    
    template<typename T>
    bool deserializeFromFile(T& data, const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        std::stringstream ss(content);
        ss >> data;
        return true;
    }
}

} // namespace nlm