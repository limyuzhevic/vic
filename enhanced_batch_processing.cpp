// Enhanced batch processing and high-level convenience functions

// This file provides batch processing capabilities and high-level convenience functions
// including:
// 1. Batch simulation execution for multiple scenarios
// 2. Parallel processing of independent simulations
// 3. Experiment management and result aggregation
// 4. High-level training and learning workflows
// 5. Performance benchmarking and comparison
// 6. Automated testing and validation
// 7. Data export and analysis

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <chrono>
#include <cmath>
#include <random>

namespace py = pybind11;
namespace nlm {

// Batch processing result aggregation
struct BatchResult {
    std::string batchId;
    std::string description;
    size_t totalIterations;
    size_t successfulIterations;
    size_t failedIterations;
    double executionTime;
    std::map<std::string, double> metrics;
    std::vector<std::map<std::string, double>> iterationResults;
    
    std::string getSummary() const {
        std::ostringstream oss;
        oss << "Batch Result: " << batchId << "\n";
        oss << "Description: " << description << "\n";
        oss << "Iterations: " << successfulIterations << "/" << totalIterations << " (" 
            << (totalIterations > 0 ? (successfulIterations * 100.0 / totalIterations) : 0.0) << "%)" << "\n";
        oss << "Execution Time: " << executionTime << " seconds\n";
        oss << "Metrics:" << std::endl;
        
        for (const auto& [name, value] : metrics) {
            oss << "  " << name << ": " << value << "\n";
        }
        
        return oss.str();
    }
    
    double getSuccessRate() const {
        return totalIterations > 0 ? (successfulIterations * 100.0 / totalIterations) : 0.0;
    }
    
    double getAverageMetric(const std::string& metricName) const {
        double sum = 0.0;
        size_t count = 0;
        
        for (const auto& result : iterationResults) {
            auto it = result.find(metricName);
            if (it != result.end()) {
                sum += it->second;
                count++;
            }
        }
        
        return count > 0 ? sum / count : 0.0;
    }
};

// Batch simulation manager
class BatchSimulationManager {
public:
    struct SimulationTask {
        std::string taskId;
        std::shared_ptr<Config> config;
        std::shared_ptr<SimpleWorld> world;
        std::shared_ptr<AgentBrain> agent;
        size_t iterations;
        std::function<void(size_t, double)> progressCallback;
        std::map<std::string, std::variant<int, double, std::string, bool>> parameters;
        
        SimulationTask() : iterations(100), progressCallback(nullptr) {}
        SimulationTask(const std::string& id, std::shared_ptr<Config> cfg, 
                     size_t iters = 100, std::function<void(size_t, double)> callback = nullptr)
            : taskId(id), config(cfg), iterations(iters), progressCallback(callback) {}
    };
    
    BatchSimulationManager() : maxThreads(std::thread::hardware_concurrency()), 
                              progressReportingInterval(10), autoSaveEnabled(true) {}
    
    BatchResult runBatch(const std::vector<SimulationTask>& tasks,
                        const std::string& batchId = "batch",
                        const std::string& description = "",
                        bool parallel = true) {
        BatchResult result;
        result.batchId = batchId;
        result.description = description;
        result.totalIterations = 0;
        result.successfulIterations = 0;
        result.failedIterations = 0;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        if (parallel && tasks.size() > 1) {
            result = runParallel(tasks, result);
        } else {
            result = runSequential(tasks, result);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Auto-save if enabled
        if (autoSaveEnabled) {
            saveBatchResult(result, "batch_results_" + batchId + "_" + 
                          getCurrentTimestamp() + ".json");
        }
        
        return result;
    }
    
    BatchResult runSimulationTask(const SimulationTask& task) {
        BatchResult result;
        result.batchId = task.taskId;
        result.description = "Simulation task: " + task.taskId;
        result.totalIterations = task.iterations;
        
        try {
            // Create brain
            auto brain = std::make_shared<Brain>(task.config);
            brain->initialize();
            
            // Initialize world if provided
            std::shared_ptr<SimpleWorld> world = task.world;
            if (!world) {
                world = std::make_shared<SimpleWorld>();
                world->configure(20, 20, 8, 8);
                world->reset();
            }
            
            // Initialize agent
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(*world);
            
            // Enable features based on configuration
            if (task.parameters.count("reward_modulation") && 
                std::get<bool>(task.parameters.at("reward_modulation"))) {
                agent->enableRewardModulation(true);
            }
            if (task.parameters.count("curiosity") && 
                std::get<bool>(task.parameters.at("curiosity"))) {
                agent->enableCuriosity(true);
            }
            if (task.parameters.count("development") && 
                std::get<bool>(task.parameters.at("development"))) {
                agent->enableDevelopment(true);
            }
            if (task.parameters.count("structural_plasticity") && 
                std::get<bool>(task.parameters.at("structural_plasticity"))) {
                agent->enableStructuralPlasticity(true);
            }
            
            std::vector<std::map<std::string, double>> iterationResults;
            size_t successfulIterations = 0;
            
            for (size_t iteration = 0; iteration < task.iterations; ++iteration) {
                try {
                    // Run simulation step
                    world->update(0.1);
                    
                    auto percept = world->getSensoryPercept();
                    agent->processSensoryInput(percept);
                    
                    brain->step(iteration);
                    
                    auto action = agent->decodeMotorCommand();
                    world->applyMotorCommand(action, world->getSimulationTime());
                    
                    // Collect metrics
                    std::map<std::string, double> iterationMetrics;
                    iterationMetrics["iteration"] = static_cast<double>(iteration);
                    iterationMetrics["firing_neurons"] = brain->getFiringNeuronCount();
                    iterationMetrics["total_spikes"] = brain->getTotalSpikeCount();
                    iterationMetrics["average_firing_rate"] = brain->getAverageFiringRate();
                    iterationMetrics["curiosity"] = agent->getCuriosityLevel();
                    iterationMetrics["novelty"] = agent->getNoveltyLevel();
                    iterationMetrics["reward"] = agent->getNeuromodulationLevel();
                    
                    iterationResults.push_back(iterationMetrics);
                    successfulIterations++;
                    
                    // Call progress callback
                    if (task.progressCallback) {
                        double progress = static_cast<double>(iteration) / task.iterations;
                        task.progressCallback(iteration, progress);
                    }
                    
                    // Report progress periodically
                    if (progressReportingInterval > 0 && iteration % progressReportingInterval == 0) {
                        std::cout << "Progress: " << task.taskId << ": " 
                                 << iteration << "/" << task.iterations << " (" 
                                 << (iteration * 100.0 / task.iterations) << "%)" << std::endl;
                    }
                    
                } catch (const std::exception& e) {
                    std::cerr << "Error in iteration " << iteration << " for task " 
                             << task.taskId << ": " << e.what() << std::endl;
                    result.failedIterations++;
                }
            }
            
            // Calculate summary metrics
            result.successfulIterations = successfulIterations;
            result.iterationResults = iterationResults;
            
            // Aggregate metrics
            std::map<std::string, double> aggregatedMetrics;
            for (const auto& metricName : {"firing_neurons", "total_spikes", "average_firing_rate",
                                          "curiosity", "novelty", "reward"}) {
                double avg = result.getAverageMetric(metricName);
                aggregatedMetrics[metricName] = avg;
            }
            
            result.metrics = aggregatedMetrics;
            
        } catch (const std::exception& e) {
            std::cerr << "Error running simulation task " << task.taskId << ": " << e.what() << std::endl;
            result.failedIterations = task.iterations;
        }
        
        return result;
    }
    
    void setMaxThreads(size_t threads) {
        maxThreads = threads;
    }
    
    void setProgressReportingInterval(size_t interval) {
        progressReportingInterval = interval;
    }
    
    void enableAutoSave(bool enable) {
        autoSaveEnabled = enable;
    }
    
    void saveBatchResult(const BatchResult& result, const std::string& filepath) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Cannot open file for saving batch result: " << filepath << std::endl;
            return;
        }
        
        file << "{\n";
        file << "  \"batchId\": \"" << result.batchId << "\",\n";
        file << "  \"description\": \"" << result.description << "\",\n";
        file << "  \"totalIterations\": " << result.totalIterations << ",\n";
        file << "  \"successfulIterations\": " << result.successfulIterations << ",\n";
        file << "  \"failedIterations\": " << result.failedIterations << ",\n";
        file << "  \"executionTime\": " << result.executionTime << ",\n";
        file << "  \"metrics\": {\n";
        
        bool firstMetric = true;
        for (const auto& [name, value] : result.metrics) {
            if (!firstMetric) file << ",\n";
            file << "    \"" << name << "\": " << value;
            firstMetric = false;
        }
        file << "\n  },\n";
        
        file << "  \"iterationResults\": [\n";
        
        for (size_t i = 0; i < result.iterationResults.size(); ++i) {
            file << "    {\n";
            const auto& iterResult = result.iterationResults[i];
            bool firstKey = true;
            for (const auto& [key, value] : iterResult) {
                if (!firstKey) file << ",\n";
                file << "      \"" << key << "\": " << value;
                firstKey = false;
            }
            file << "\n";
            if (i < result.iterationResults.size() - 1) {
                file << "    },\n";
            } else {
                file << "    }\n";
            }
        }
        
        file << "  ]\n";
        file << "}\n";
        
        file.close();
    }
    
private:
    BatchResult runParallel(const std::vector<SimulationTask>& tasks, BatchResult& result) {
        result.totalIterations = 0;
        
        std::vector<std::thread> threads;
        std::vector<BatchResult> threadResults;
        std::mutex resultsMutex;
        
        // Limit concurrent threads
        size_t numThreads = std::min(maxThreads, tasks.size());
        
        for (size_t i = 0; i < numThreads; ++i) {
            threads.emplace_back([&, i]() {
                BatchResult localResult;
                size_t taskIndex = i;
                
                while (taskIndex < tasks.size()) {
                    BatchResult taskResult = runSimulationTask(tasks[taskIndex]);
                    
                    {
                        std::lock_guard<std::mutex> lock(resultsMutex);
                        threadResults.push_back(taskResult);
                        
                        result.totalIterations += taskResult.totalIterations;
                        result.successfulIterations += taskResult.successfulIterations;
                        result.failedIterations += taskResult.failedIterations;
                        
                        // Merge metrics
                        for (const auto& [name, value] : taskResult.metrics) {
                            if (result.metrics.count(name)) {
                                result.metrics[name] += value;
                            } else {
                                result.metrics[name] = value;
                            }
                        }
                        
                        // Merge iteration results
                        result.iterationResults.insert(result.iterationResults.end(),
                                                      taskResult.iterationResults.begin(),
                                                      taskResult.iterationResults.end());
                    }
                    
                    taskIndex += numThreads;
                }
            });
        }
        
        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }
        
        return result;
    }
    
    BatchResult runSequential(const std::vector<SimulationTask>& tasks, BatchResult& result) {
        result.totalIterations = 0;
        
        for (const auto& task : tasks) {
            BatchResult taskResult = runSimulationTask(task);
            
            result.totalIterations += taskResult.totalIterations;
            result.successfulIterations += taskResult.successfulIterations;
            result.failedIterations += taskResult.failedIterations;
            
            // Merge metrics
            for (const auto& [name, value] : taskResult.metrics) {
                if (result.metrics.count(name)) {
                    result.metrics[name] += value;
                } else {
                    result.metrics[name] = value;
                }
            }
            
            // Merge iteration results
            result.iterationResults.insert(result.iterationResults.end(),
                                          taskResult.iterationResults.begin(),
                                          taskResult.iterationResults.end());
            
            std::cout << "Completed task: " << task.taskId << std::endl;
        }
        
        return result;
    }
    
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
        
        return ss.str();
    }
    
    // Data members
    size_t maxThreads;
    size_t progressReportingInterval;
    bool autoSaveEnabled;
};

// High-level learning workflow manager
class LearningWorkflowManager {
public:
    struct LearningScenario {
        std::string name;
        std::shared_ptr<ConfigurationTemplate> configTemplate;
        std::vector<std::pair<std::string, std::variant<int, double, std::string, bool>>> hyperparameters;
        size_t episodes;
        size_t stepsPerEpisode;
        std::function<void(double)> progressCallback;
        std::function<bool(const std::map<std::string, double>&)> stoppingCondition;
    };
    
    LearningWorkflowManager() = default;
    
    BatchResult runLearningWorkflow(const LearningScenario& scenario,
                                   const std::string& workflowId = "learning_workflow",
                                   bool useBatchProcessing = true) {
        BatchResult result;
        result.batchId = workflowId;
        result.description = "Learning workflow: " + scenario.name;
        result.totalIterations = scenario.episodes * scenario.stepsPerEpisode;
        
        std::cout << "Starting learning workflow: " << scenario.name << std::endl;
        std::cout << "Episodes: " << scenario.episodes << ", Steps per episode: " << scenario.stepsPerEpisode << std::endl;
        
        if (useBatchProcessing) {
            // Create batch of simulation tasks
            std::vector<BatchSimulationManager::SimulationTask> tasks;
            
            for (size_t episode = 0; episode < scenario.episodes; ++episode) {
                // Create config based on template
                auto config = std::make_shared<Config>();
                if (scenario.configTemplate) {
                    config = scenario.configTemplate->applyTemplate(config);
                }
                
                // Apply hyperparameters
                for (const auto& [key, value] : scenario.hyperparameters) {
                    setConfigValue(config, key, value);
                }
                
                // Create task
                BatchSimulationManager::SimulationTask task;
                task.taskId = scenario.name + "_episode_" + std::to_string(episode);
                task.config = config;
                task.iterations = scenario.stepsPerEpisode;
                task.progressCallback = scenario.progressCallback;
                
                tasks.push_back(task);
            }
            
            // Run batch
            BatchSimulationManager batchManager;
            batchManager.setProgressReportingInterval(50);
            result = batchManager.runBatch(tasks, workflowId, scenario.name, useBatchProcessing);
            
        } else {
            // Run sequentially
            size_t totalSuccessful = 0;
            size_t totalFailed = 0;
            std::vector<std::map<std::string, double>> allIterationResults;
            
            for (size_t episode = 0; episode < scenario.episodes; ++episode++) {
                // Create config based on template
                auto config = std::make_shared<Config>();
                if (scenario.configTemplate) {
                    config = scenario.configTemplate->applyTemplate(config);
                }
                
                // Apply hyperparameters
                for (const auto& [key, value] : scenario.hyperparameters) {
                    setConfigValue(config, key, value);
                }
                
                // Run episode
                BatchSimulationManager::SimulationTask task;
                task.taskId = scenario.name + "_episode_" + std::to_string(episode);
                task.config = config;
                task.iterations = scenario.stepsPerEpisode;
                task.progressCallback = scenario.progressCallback;
                
                BatchSimulationManager batchManager;
                BatchResult episodeResult = batchManager.runSimulationTask(task);
                
                totalSuccessful += episodeResult.successfulIterations;
                totalFailed += episodeResult.failedIterations;
                allIterationResults.insert(allIterationResults.end(),
                                         episodeResult.iterationResults.begin(),
                                         episodeResult.iterationResults.end());
                
                // Check stopping condition
                if (scenario.stoppingCondition) {
                    std::map<std::string, double> currentMetrics;
                    for (const auto& metricName : {"curiosity", "novelty", "reward"}) {
                        currentMetrics[metricName] = episodeResult.getAverageMetric(metricName);
                    }
                    
                    if (scenario.stoppingCondition(currentMetrics)) {
                        std::cout << "Stopping condition met. Ending workflow early." << std::endl;
                        break;
                    }
                }
                
                double progress = static_cast<double>(episode) / scenario.episodes;
                if (scenario.progressCallback) {
                    scenario.progressCallback(progress);
                }
            }
            
            result.successfulIterations = totalSuccessful;
            result.failedIterations = totalFailed;
            result.iterationResults = allIterationResults;
            
            // Calculate aggregated metrics
            std::map<std::string, double> aggregatedMetrics;
            size_t totalIterationsProcessed = 0;
            
            for (const auto& metricName : {"firing_neurons", "total_spikes", "average_firing_rate",
                                          "curiosity", "novelty", "reward"}) {
                double total = 0.0;
                size_t count = 0;
                
                for (const auto& result : allIterationResults) {
                    auto it = result.find(metricName);
                    if (it != result.end()) {
                        total += it->second;
                        count++;
                    }
                }
                
                if (count > 0) {
                    aggregatedMetrics[metricName] = total / count;
                    totalIterationsProcessed += count;
                }
            }
            
            result.metrics = aggregatedMetrics;
            result.totalIterations = totalIterationsProcessed;
        }
        
        // Calculate execution time
        auto endTime = std::chrono::high_resolution_clock::now();
        result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        std::cout << "Learning workflow completed: " << scenario.name << std::endl;
        std::cout << "Success rate: " << result.getSuccessRate() << "%" << std::endl;
        std::cout << "Execution time: " << result.executionTime << " seconds" << std::endl;
        
        return result;
    }
    
    std::vector<LearningScenario> getDefaultLearningScenarios() {
        std::vector<LearningScenario> scenarios;
        
        // Random exploration scenario
        LearningScenario randomExploration;
        randomExploration.name = "Random Exploration";
        randomExploration.configTemplate = ConfigurationTemplate::createDefaultTemplate();
        randomExploration.episodes = 10;
        randomExploration.stepsPerEpisode = 100;
        randomExploration.hyperparameters = {
            {"neuromod.curiosity.enable", true},
            {"neuromod.novelty.enable", true},
            {"development.enable", true}
        };
        scenarios.push_back(randomExploration);
        
        // Structured learning scenario
        LearningScenario structuredLearning;
        structuredLearning.name = "Structured Learning";
        structuredLearning.configTemplate = ConfigurationTemplate::createLearningTemplate();
        structuredLearning.episodes = 20;
        structuredLearning.stepsPerEpisode = 150;
        structuredLearning.hyperparameters = {
            {"plasticity.stdp.enable", true},
            {"plasticity.hebbian.enable", true},
            {"plasticity.structural.enable", true},
            {"neuromod.curiosity.enable", true},
            {"development.enable", true},
            {"development.stage_count", 3}
        };
        scenarios.push_back(structuredLearning);
        
        // Performance-optimized scenario
        LearningScenario performanceOptimized;
        performanceOptimized.name = "Performance Optimized";
        performanceOptimized.configTemplate = ConfigurationTemplate::createPerformanceTemplate();
        performanceOptimized.episodes = 15;
        performanceOptimized.stepsPerEpisode = 200;
        performanceOptimized.hyperparameters = {
            {"plasticity.stdp.enable", true},
            {"plasticity.hebbian.enable", false},
            {"neuromod.curiosity.enable", false},
            {"development.enable", false}
        };
        scenarios.push_back(performanceOptimized);
        
        return scenarios;
    }
    
private:
    void setConfigValue(std::shared_ptr<Config> config, const std::string& key,
                       const std::variant<int, double, std::string, bool>& value) {
        std::visit([&config, &key](auto&& val) {
            config->set(key, val, ConfigSource::Runtime);
        }, value);
    }
    
    // Data members
    // (no persistent state needed)
};

// Experiment management system
class ExperimentManager {
public:
    struct Experiment {
        std::string name;
        std::string description;
        std::shared_ptr<ConfigurationTemplate> configTemplate;
        std::vector<LearningScenario> learningScenarios;
        std::vector<BatchSimulationManager::SimulationTask> controlTasks;
        std::map<std::string, std::string> metadata;
        std::chrono::system_clock::time_point createdTime;
        std::chrono::system_clock::time_point lastRunTime;
        
        Experiment() : createdTime(std::chrono::system_clock::now()) {}
        Experiment(const std::string& n, const std::string& desc) 
            : name(n), description(desc), createdTime(std::chrono::system_clock::now()) {}
    };
    
    ExperimentManager() = default;
    
    std::string createExperiment(const std::string& name, const std::string& description = "") {
        Experiment exp(name, description);
        experiments[name] = exp;
        return name;
    }
    
    bool addLearningScenario(const std::string& experimentName, 
                            const LearningScenario& scenario) {
        auto it = experiments.find(experimentName);
        if (it == experiments.end()) {
            return false;
        }
        
        it->second.learningScenarios.push_back(scenario);
        return true;
    }
    
    bool addControlTask(const std::string& experimentName,
                       const BatchSimulationManager::SimulationTask& task) {
        auto it = experiments.find(experimentName);
        if (it == experiments.end()) {
            return false;
        }
        
        it->second.controlTasks.push_back(task);
        return true;
    }
    
    std::vector<std::string> getExperimentNames() const {
        std::vector<std::string> names;
        for (const auto& [name, _] : experiments) {
            names.push_back(name);
        }
        return names;
    }
    
    Experiment getExperiment(const std::string& name) const {
        auto it = experiments.find(name);
        if (it != experiments.end()) {
            return it->second;
        }
        return Experiment(); // Return empty experiment
    }
    
    BatchResult runExperiment(const std::string& name, bool useBatchProcessing = true) {
        auto it = experiments.find(name);
        if (it == experiments.end()) {
            throw std::runtime_error("Experiment not found: " + name);
        }
        
        Experiment& experiment = it->second;
        experiment.lastRunTime = std::chrono::system_clock::now();
        
        BatchResult result;
        result.batchId = name + "_run_" + getCurrentTimestamp();
        result.description = "Experiment: " + experiment.name + ", Description: " + experiment.description;
        
        std::cout << "Running experiment: " << experiment.name << std::endl;
        std::cout << "Description: " << experiment.description << std::endl;
        
        // Run learning scenarios
        size_t totalEpisodes = 0;
        for (const auto& scenario : experiment.learningScenarios) {
            std::cout << "Running scenario: " << scenario.name << std::endl;
            
            LearningWorkflowManager workflowManager;
            BatchResult scenarioResult = workflowManager.runLearningWorkflow(
                scenario, name + "_scenario_" + scenario.name, useBatchProcessing);
            
            result.totalIterations += scenarioResult.totalIterations;
            result.successfulIterations += scenarioResult.successfulIterations;
            result.failedIterations += scenarioResult.failedIterations;
            
            // Merge metrics
            for (const auto& [metricName, value] : scenarioResult.metrics) {
                if (result.metrics.count(metricName)) {
                    result.metrics[metricName] += value;
                } else {
                    result.metrics[metricName] = value;
                }
            }
            
            // Merge iteration results
            result.iterationResults.insert(result.iterationResults.end(),
                                          scenarioResult.iterationResults.begin(),
                                          scenarioResult.iterationResults.end());
            
            totalEpisodes += scenario.episodes;
            
            std::cout << "Completed scenario: " << scenario.name << std::endl;
        }
        
        // Run control tasks
        for (const auto& task : experiment.controlTasks) {
            std::cout << "Running control task: " << task.taskId << std::endl;
            
            BatchSimulationManager batchManager;
            BatchResult taskResult = batchManager.runSimulationTask(task);
            
            result.totalIterations += taskResult.totalIterations;
            result.successfulIterations += taskResult.successfulIterations;
            result.failedIterations += taskResult.failedIterations;
            
            // Merge metrics
            for (const auto& [metricName, value] : taskResult.metrics) {
                if (result.metrics.count(metricName)) {
                    result.metrics[metricName] += value;
                } else {
                    result.metrics[metricName] = value;
                }
            }
            
            // Merge iteration results
            result.iterationResults.insert(result.iterationResults.end(),
                                          taskResult.iterationResults.begin(),
                                          taskResult.iterationResults.end());
        }
        
        // Calculate averages
        if (result.totalIterations > 0) {
            std::map<std::string, double> averageMetrics;
            
            for (const auto& [metricName, value] : result.metrics) {
                // Average across all tasks (simplified)
                averageMetrics[metricName] = value / (totalEpisodes + experiment.controlTasks.size());
            }
            
            result.metrics = averageMetrics;
        }
        
        // Calculate execution time
        auto endTime = std::chrono::high_resolution_clock::now();
        result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        std::cout << "Experiment completed: " << experiment.name << std::endl;
        std::cout << "Total episodes: " << totalEpisodes << std::endl;
        std::cout << "Success rate: " << result.getSuccessRate() << "%" << std::endl;
        
        return result;
    }
    
    void exportExperimentResults(const std::string& experimentName, const std::string& outputDir) {
        auto it = experiments.find(experimentName);
        if (it == experiments.end()) {
            throw std::runtime_error("Experiment not found: " + experimentName);
        }
        
        std::filesystem::create_directories(outputDir);
        
        // Export experiment metadata
        std::ofstream metadataFile(outputDir + "/metadata.json");
        if (metadataFile.is_open()) {
            metadataFile << "{\n";
            metadataFile << "  \"name\": \"" << it->second.name << "\",\n";
            metadataFile << "  \"description\": \"" << it->second.description << "\",\n";
            metadataFile << "  \"createdTime\": \"" << getTimestampString(it->second.createdTime) << "\",\n";
            
            if (it->second.lastRunTime.time_since_epoch().count() > 0) {
                metadataFile << "  \"lastRunTime\": \"" << getTimestampString(it->second.lastRunTime) << "\",\n";
            }
            
            metadataFile << "  \"learningScenariosCount\": " << it->second.learningScenarios.size() << ",\n";
            metadataFile << "  \"controlTasksCount\": " << it->second.controlTasks.size() << "\n";
            metadataFile << "}\n";
            metadataFile.close();
        }
        
        // Export experiment configuration
        std::ofstream configFile(outputDir + "/configuration.json");
        if (configFile.is_open()) {
            configFile << "{\n";
            configFile << "  \"name\": \"" << it->second.name << "\",\n";
            
            // Export learning scenarios
            configFile << "  \"learningScenarios\": [\n";
            for (size_t i = 0; i < it->second.learningScenarios.size(); ++i) {
                const auto& scenario = it->second.learningScenarios[i];
                configFile << "    {\n";
                configFile << "      \"name\": \"" << scenario.name << "\",\n";
                configFile << "      \"episodes\": " << scenario.episodes << ",\n";
                configFile << "      \"stepsPerEpisode\": " << scenario.stepsPerEpisode << "\n";
                if (i < it->second.learningScenarios.size() - 1) {
                    configFile << "    },\n";
                } else {
                    configFile << "    }\n";
                }
            }
            configFile << "  ],\n";
            
            // Export control tasks
            configFile << "  \"controlTasks\": [\n";
            for (size_t i = 0; i < it->second.controlTasks.size(); ++i) {
                const auto& task = it->second.controlTasks[i];
                configFile << "    {\n";
                configFile << "      \"taskId\": \"" << task.taskId << "\",\n";
                configFile << "      \"iterations\": " << task.iterations << "\n";
                if (i < it->second.controlTasks.size() - 1) {
                    configFile << "    },\n";
                } else {
                    configFile << "    }\n";
                }
            }
            configFile << "  ]\n";
            configFile << "}\n";
            configFile.close();
        }
        
        std::cout << "Experiment results exported to: " << outputDir << std::endl;
    }
    
private:
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
        
        return ss.str();
    }
    
    std::string getTimestampString(const std::chrono::system_clock::time_point& timestamp) const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        
        return ss.str();
    }
    
    // Data members
    std::map<std::string, Experiment> experiments;
};

// Enhanced Python bindings for batch processing and high-level functions
void addBatchProcessingBindings(py::module_& m) {
    // Batch simulation manager
    py::class_<BatchSimulationManager>(m, "BatchSimulationManager")
        .def(py::init<>())
        .def("run_batch", &BatchSimulationManager::runBatch,
             py::arg("tasks"), py::arg("batchId") = "batch", py::arg("description") = "",
             py::arg("parallel") = true,
             "Run batch of simulation tasks")
        .def("run_simulation_task", &BatchSimulationManager::runSimulationTask,
             py::arg("task"),
             "Run single simulation task")
        .def("set_max_threads", &BatchSimulationManager::setMaxThreads,
             py::arg("threads"),
             "Set maximum number of threads for parallel execution")
        .def("set_progress_reporting_interval", &BatchSimulationManager::setProgressReportingInterval,
             py::arg("interval"),
             "Set progress reporting interval")
        .def("enable_auto_save", &BatchSimulationManager::enableAutoSave,
             py::arg("enable"),
             "Enable automatic saving of batch results");
    
    // Learning workflow manager
    py::class_<LearningWorkflowManager>(m, "LearningWorkflowManager")
        .def(py::init<>())
        .def("run_learning_workflow", &LearningWorkflowManager::runLearningWorkflow,
             py::arg("scenario"), py::arg("workflowId") = "learning_workflow", py::arg("useBatchProcessing") = true,
             "Run learning workflow with specified scenario")
        .def("get_default_learning_scenarios", &LearningWorkflowManager::getDefaultLearningScenarios,
             "Get default learning scenarios");
    
    // Experiment manager
    py::class_<ExperimentManager>(m, "ExperimentManager")
        .def(py::init<>())
        .def("create_experiment", &ExperimentManager::createExperiment,
             py::arg("name"), py::arg("description") = "",
             "Create new experiment")
        .def("add_learning_scenario", &ExperimentManager::addLearningScenario,
             py::arg("experimentName"), py::arg("scenario"),
             "Add learning scenario to experiment")
        .def("add_control_task", &ExperimentManager::addControlTask,
             py::arg("experimentName"), py::arg("task"),
             "Add control task to experiment")
        .def("get_experiment_names", &ExperimentManager::getExperimentNames,
             "Get all experiment names")
        .def("get_experiment", &ExperimentManager::getExperiment,
             py::arg("name"),
             "Get experiment by name")
        .def("run_experiment", &ExperimentManager::runExperiment,
             py::arg("name"), py::arg("useBatchProcessing") = true,
             "Run experiment")
        .def("export_experiment_results", &ExperimentManager::exportExperimentResults,
             py::arg("experimentName"), py::arg("outputDir"),
             "Export experiment results to directory");
    
    // Batch result
    py::class_<BatchResult>(m, "BatchResult")
        .def_readonly("batch_id", &BatchResult::batchId)
        .def_readonly("description", &BatchResult::description)
        .def_readonly("total_iterations", &BatchResult::totalIterations)
        .def_readonly("successful_iterations", &BatchResult::successfulIterations)
        .def_readonly("failed_iterations", &BatchResult::failedIterations)
        .def_readonly("execution_time", &BatchResult::executionTime)
        .def_readonly("metrics", &BatchResult::metrics)
        .def("get_summary", &BatchResult::getSummary,
             "Get human-readable summary of results")
        .def("get_success_rate", &BatchResult::getSuccessRate,
             "Get success rate percentage")
        .def("get_average_metric", &BatchResult::getAverageMetric,
             py::arg("metricName"),
             "Get average value of specified metric");
    
    // Convenience functions for batch processing
    m.def("create_batch_simulation_manager", []() {
        return std::make_shared<BatchSimulationManager>();
    }, "Create batch simulation manager");
    
    m.def("create_learning_workflow_manager", []() {
        return std::make_shared<LearningWorkflowManager>();
    }, "Create learning workflow manager");
    
    m.def("create_experiment_manager", []() {
        return std::make_shared<ExperimentManager>();
    }, "Create experiment manager");
    
    m.def("create_batch_result", [](const std::string& batchId,
                                   const std::string& description,
                                   size_t totalIterations,
                                   size_t successfulIterations,
                                   size_t failedIterations) {
        BatchResult result;
        result.batchId = batchId;
        result.description = description;
        result.totalIterations = totalIterations;
        result.successfulIterations = successfulIterations;
        result.failedIterations = failedIterations;
        return result;
    }, py::arg("batchId"), py::arg("description"), py::arg("totalIterations"),
       py::arg("successfulIterations"), py::arg("failedIterations"),
       "Create batch result manually");
    
    m.def("run_batch_experiment", [](const ExperimentManager& manager,
                                     const std::string& experimentName,
                                     bool useBatchProcessing) {
        return manager.runExperiment(experimentName, useBatchProcessing);
    }, py::arg("manager"), py::arg("experimentName"), py::arg("useBatchProcessing") = true,
       "Run experiment from manager");
    
    m.def("create_learning_scenario", [](const std::string& name,
                                         const std::shared_ptr<ConfigurationTemplate>& configTemplate,
                                         size_t episodes,
                                         size_t stepsPerEpisode) {
        LearningWorkflowManager::LearningScenario scenario;
        scenario.name = name;
        scenario.configTemplate = configTemplate;
        scenario.episodes = episodes;
        scenario.stepsPerEpisode = stepsPerEpisode;
        return scenario;
    }, py::arg("name"), py::arg("configTemplate"), py::arg("episodes"), py::arg("stepsPerEpisode"),
       "Create learning scenario");
}

// Integration function to add all batch processing bindings
void addAllBatchProcessing(py::module_& m) {
    addBatchProcessingBindings(m);
    
    // Add version information
    m.attr("__version__") = "2.0.0-batch-processing";
    
    // Add batch processing utilities
    m.def("batch_version_info", []() {
        std::map<std::string, std::string> info;
        info["version"] = "2.0.0-batch-processing";
        info["features"] = "Batch simulation, Learning workflows, Experiment management";
        info["status"] = "Ready for enhanced batch processing";
        return info;
    }, "Get batch processing system information");
    
    m.def("create_default_batch_config", []() {
        auto config = std::make_shared<Config>();
        
        // Set default batch processing parameters
        config->set("batch.max_threads", static_cast<int64_t>(std::thread::hardware_concurrency()), ConfigSource::Runtime);
        config->set("batch.progress_reporting_interval", 100, ConfigSource::Runtime);
        config->set("batch.auto_save", true, ConfigSource::Runtime);
        
        return config;
    }, "Create default batch processing configuration");
}