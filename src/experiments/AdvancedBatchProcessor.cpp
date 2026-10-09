#include "AdvancedBatchProcessor.hpp"
#include <thread>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <iomanip>

namespace nlm {

AdvancedBatchProcessor::AdvancedBatchProcessor() : isInitialized_(false) {
    std::memset(&stats_, 0, sizeof(stats_));
}

AdvancedBatchProcessor::~AdvancedBatchProcessor() {
    clear();
}

void AdvancedBatchProcessor::configure(const BatchConfig& config) {
    std::lock_guard<std::mutex> lock(statsMutex_);
    config_ = config;
    isInitialized_ = true;
}

std::vector<BatchExperimentResult> AdvancedBatchProcessor::executeBatch(
    std::function<std::unique_ptr<Brain>(size_t, std::shared_ptr<Config>)> experimentFactory,
    const std::vector<size_t>& experimentIds) {
    
    if (!isInitialized_) {
        throw std::runtime_error("Batch processor not initialized");
    }
    
    std::lock_guard<std::mutex> lock(statsMutex_);
    
    // Create experiment queue
    std::vector<std::pair<size_t, std::shared_ptr<Config>>> experimentQueue;
    
    if (experimentIds.empty()) {
        // Create all experiments
        for (size_t i = 0; i < config_.numExperiments; ++i) {
            auto expConfig = std::make_shared<Config>(ConfigBuilder::createDefaultConfig());
            // Modify config for each experiment if needed
            experimentQueue.emplace_back(i, expConfig);
        }
    } else {
        // Use specified experiment IDs
        for (size_t id : experimentIds) {
            auto expConfig = std::make_shared<Config>(ConfigBuilder::createDefaultConfig());
            experimentQueue.emplace_back(id, expConfig);
        }
    }
    
    // Results vector
    std::vector<BatchExperimentResult> results;
    results.reserve(experimentQueue.size());
    
    // Create worker threads
    std::vector<std::thread> workers;
    std::mutex resultsMutex;
    std::condition_variable resultsAvailable;
    std::atomic<bool> stopFlag{false};
    
    for (size_t i = 0; i < config_.parallelThreads; ++i) {
        workers.emplace_back(&AdvancedBatchProcessor::workerThread, this, i,
                           experimentFactory, std::ref(resultsMutex), std::ref(experimentQueue),
                           std::ref(results), std::ref(resultsAvailable), std::ref(stopFlag));
    }
    
    // Wait for all workers to complete
    for (auto& worker : workers) {
        worker.join();
    }
    
    // Aggregate results if enabled
    if (config_.enableResultAggregation && results.size() > 1) {
        BatchExperimentResult aggregated(0);
        for (const auto& result : results) {
            aggregated.experimentId = 0; // Aggregated
            aggregated.finalReward += result.finalReward;
            aggregated.averageFiringRate += result.averageFiringRate;
            aggregated.totalSpikes += result.totalSpikes;
            aggregated.memoryUsage += result.memoryUsage;
            aggregated.success = aggregated.success || result.success;
            aggregated.rewardHistory.insert(aggregated.rewardHistory.end(), 
                                           result.rewardHistory.begin(), 
                                           result.rewardHistory.end());
            aggregated.performanceMetrics.insert(aggregated.performanceMetrics.end(),
                                                result.performanceMetrics.begin(),
                                                result.performanceMetrics.end());
        }
        
        if (results.size() > 1) {
            aggregated.finalReward /= results.size();
            aggregated.averageFiringRate /= results.size();
            aggregated.memoryUsage /= results.size();
        }
        
        aggregated.success = std::all_of(results.begin(), results.end(), 
                                        [](const BatchExperimentResult& r) { return r.success; });
        
        results.clear();
        results.push_back(aggregated);
    }
    
    // Update statistics
    stats_.totalExperiments = results.size();
    stats_.successfulExperiments = std::count_if(results.begin(), results.end(),
                                                [](const BatchExperimentResult& r) { return r.success; });
    
    return results;
}

BatchExperimentResult AdvancedBatchProcessor::executeExperiment(
    size_t experimentId,
    std::function<std::unique_ptr<Brain>(size_t, std::shared_ptr<Config>)> experimentFactory,
    std::shared_ptr<Config> config) {
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    BatchExperimentResult result(experimentId);
    
    try {
        // Create brain with configuration
        auto brain = experimentFactory(experimentId, config);
        
        if (!brain) {
            result.errorMessage = "Failed to create brain instance";
            return result;
        }
        
        // Initialize brain
        if (!brain->initialize()) {
            result.errorMessage = "Brain initialization failed";
            return result;
        }
        
        // Run experiment
        for (size_t step = 0; step < config_.duration; ++step) {
            brain->step(step);
            
            // Collect metrics
            result.totalSpikes += brain->getTotalSpikeCount();
            
            // Store reward if available
            if (brain->getDopamine()) {
                float reward = brain->getDopamine()->getLevel() * config_.baseReward;
                result.rewardHistory.push_back(reward);
                result.finalReward += reward;
            }
            
            // Store performance metrics
            result.performanceMetrics.push_back(brain->getAverageFiringRate());
        }
        
        // Calculate averages
        if (!result.rewardHistory.empty()) {
            result.finalReward /= result.rewardHistory.size();
        }
        
        if (!result.performanceMetrics.empty()) {
            float sum = std::accumulate(result.performanceMetrics.begin(), 
                                       result.performanceMetrics.end(), 0.0f);
            result.averageFiringRate = sum / result.performanceMetrics.size();
        }
        
        result.success = true;
        
        // Calculate execution time
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
    } catch (const std::exception& e) {
        result.errorMessage = std::string("Exception: ") + e.what();
    } catch (...) {
        result.errorMessage = "Unknown exception occurred";
    }
    
    return result;
}

AdvancedBatchProcessor::PerformanceStats AdvancedBatchProcessor::getPerformanceStats() const {
    std::lock_guard<std::mutex> lock(statsMutex_);
    
    PerformanceStats stats;
    stats.totalExperiments = stats_.totalExperiments;
    
    if (stats_.totalExperiments > 0) {
        stats.averageExecutionTime = (stats_.totalExecutionTime / stats_.totalExperiments) / 1000.0f; // Convert to seconds
        stats.successRate = static_cast<float>(stats_.successfulExperiments) / stats_.totalExperiments;
        stats.totalMemoryUsage = stats_.totalMemoryUsage;
        stats.peakMemoryUsage = stats_.peakMemoryUsage;
    }
    
    return stats;
}

bool AdvancedBatchProcessor::saveResults(const std::vector<BatchExperimentResult>& results, const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "Batch Experiment Results" << std::endl;
        file << "=======================" << std::endl;
        file << "Number of experiments: " << results.size() << std::endl;
        file << std::endl;
        
        for (const auto& result : results) {
            file << "Experiment ID: " << result.experimentId << std::endl;
            file << "Success: " << (result.success ? "Yes" : "No") << std::endl;
            file << "Final Reward: " << result.finalReward << std::endl;
            file << "Average Firing Rate: " << result.averageFiringRate << std::endl;
            file << "Total Spikes: " << result.totalSpikes << std::endl;
            file << "Memory Usage: " << result.memoryUsage << std::endl;
            
            if (!result.errorMessage.empty()) {
                file << "Error: " << result.errorMessage << std::endl;
            }
            
            file << std::endl;
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<BatchExperimentResult> AdvancedBatchProcessor::loadResults(const std::string& filepath) const {
    std::vector<BatchExperimentResult> results;
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return results;
        }
        
        std::string line;
        BatchExperimentResult* currentResult = nullptr;
        size_t currentId = 0;
        
        while (std::getline(file, line)) {
            if (line.empty()) {
                currentResult = nullptr;
                continue;
            }
            
            if (line.find("Experiment ID:") != std::string::npos) {
                currentId = std::stoull(line.substr(15));
                currentResult = &results.emplace_back(currentId);
            } else if (line.find("Success: ") != std::string::npos) {
                if (currentResult) {
                    currentResult->success = (line.substr(9) == "Yes");
                }
            } else if (line.find("Final Reward: ") != std::string::npos) {
                if (currentResult) {
                    currentResult->finalReward = std::stof(line.substr(14));
                }
            } else if (line.find("Average Firing Rate: ") != std::string::npos) {
                if (currentResult) {
                    currentResult->averageFiringRate = std::stof(line.substr(23));
                }
            } else if (line.find("Total Spikes: ") != std::string::npos) {
                if (currentResult) {
                    currentResult->totalSpikes = std::stoull(line.substr(14));
                }
            } else if (line.find("Memory Usage: ") != std::string::npos) {
                if (currentResult) {
                    currentResult->memoryUsage = std::stoull(line.substr(15));
                }
            } else if (line.find("Error: ") != std::string::npos) {
                if (currentResult) {
                    currentResult->errorMessage = line.substr(7);
                }
            }
        }
        
        file.close();
        
    } catch (const std::exception& e) {
        // Return empty results on error
    }
    
    return results;
}

void AdvancedBatchProcessor::clear() {
    std::lock_guard<std::mutex> lock(statsMutex_);
    
    stats_.totalExperiments = 0;
    stats_.totalExecutionTime = 0;
    stats_.successfulExperiments = 0;
    stats_.totalMemoryUsage = 0;
    stats_.peakMemoryUsage = 0;
    
    isInitialized_ = false;
}

void AdvancedBatchProcessor::workerThread(size_t threadId,
                                        std::function<std::unique_ptr<Brain>(size_t, std::shared_ptr<Config>)> experimentFactory,
                                        std::mutex& queueMutex,
                                        std::vector<std::pair<size_t, std::shared_ptr<Config>>>& experimentQueue,
                                        std::vector<BatchExperimentResult>& results,
                                        std::condition_variable& resultsAvailable,
                                        std::atomic<bool>& stopFlag) {
    
    while (!stopFlag) {
        std::unique_lock<std::mutex> lock(queueMutex);
        
        // Wait for experiment to process
        if (experimentQueue.empty()) {
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        
        // Get next experiment
        auto experiment = experimentQueue.back();
        experimentQueue.pop_back();
        lock.unlock();
        
        // Execute experiment
        BatchExperimentResult result = std::invoke(std::bind(&AdvancedBatchProcessor::executeExperimentInWorker,
                                                          std::placeholders::_1, std::placeholders::_2),
                                                 this, experiment.first, experiment.second);
        
        // Add result
        lock.lock();
        results.push_back(std::move(result));
        lock.unlock();
        
        resultsAvailable.notify_one();
    }
}

BatchExperimentResult AdvancedBatchProcessor::executeExperimentInWorker(
    size_t experimentId,
    std::shared_ptr<Config> config) {
    return executeExperiment(experimentId, nullptr, config);
}

BatchExperimentResult AdvancedBatchProcessor::aggregateResults(const std::vector<BatchExperimentResult>& results) const {
    BatchExperimentResult aggregated(0);
    
    for (const auto& result : results) {
        aggregated.experimentId = 0; // Aggregated
        aggregated.finalReward += result.finalReward;
        aggregated.averageFiringRate += result.averageFiringRate;
        aggregated.totalSpikes += result.totalSpikes;
        aggregated.memoryUsage += result.memoryUsage;
        aggregated.success = aggregated.success || result.success;
        aggregated.rewardHistory.insert(aggregated.rewardHistory.end(),
                                       result.rewardHistory.begin(),
                                       result.rewardHistory.end());
        aggregated.performanceMetrics.insert(aggregated.performanceMetrics.end(),
                                            result.performanceMetrics.begin(),
                                            result.performanceMetrics.end());
    }
    
    if (results.size() > 1) {
        aggregated.finalReward /= results.size();
        aggregated.averageFiringRate /= results.size();
        aggregated.memoryUsage /= results.size();
    }
    
    aggregated.success = std::all_of(results.begin(), results.end(),
                                    [](const BatchExperimentResult& r) { return r.success; });
    
    return aggregated;
}

bool AdvancedBatchProcessor::saveConfig(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Convert config to string
    std::stringstream configStream;
    
    configStream << "numExperiments = " << config_.numExperiments << std::endl;
    configStream << "parallelThreads = " << config_.parallelThreads << std::endl;
    configStream << "duration = " << config_.duration << std::endl;
    configStream << "baseReward = " << config_.baseReward << std::endl;
    configStream << "enableMemorySharing = " << (config_.enableMemorySharing ? "true" : "false") << std::endl;
    configStream << "enableResultAggregation = " << (config_.enableResultAggregation ? "true" : "false") << std::endl;
    configStream << "maxMemoryPerExperiment = " << config_.maxMemoryPerExperiment << std::endl;
    
    file << configStream.str();
    file.close();
    
    return true;
}

bool AdvancedBatchProcessor::loadConfig(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    BatchConfig tempConfig;
    
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t delimiter = line.find('=');
        if (delimiter == std::string::npos) {
            continue;
        }
        
        std::string key = line.substr(0, delimiter);
        std::string value = line.substr(delimiter + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t\r\n"));
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        
        value.erase(0, value.find_first_not_of(" \t\r\n"));
        value.erase(value.find_last_not_of(" \t\r\n") + 1);
        
        // Parse values
        if (key == "numExperiments") {
            tempConfig.numExperiments = std::stoull(value);
        } else if (key == "parallelThreads") {
            tempConfig.parallelThreads = std::stoull(value);
        } else if (key == "duration") {
            tempConfig.duration = std::stoull(value);
        } else if (key == "baseReward") {
            tempConfig.baseReward = std::stof(value);
        } else if (key == "enableMemorySharing") {
            tempConfig.enableMemorySharing = (value == "true" || value == "1");
        } else if (key == "enableResultAggregation") {
            tempConfig.enableResultAggregation = (value == "true" || value == "1");
        } else if (key == "maxMemoryPerExperiment") {
            tempConfig.maxMemoryPerExperiment = std::stoull(value);
        }
    }
    
    file.close();
    
    configure(tempConfig);
    return true;
}

} // namespace nlm
