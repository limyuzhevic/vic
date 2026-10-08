#include "ExperimentRunner.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <thread>
#include <mutex>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

struct ExperimentRunner::Impl {
    std::vector<std::shared_ptr<Experiment>> experiments;
    uint64_t globalSeed;
    
    // Advanced features
    ExecutionMode executionMode;
    std::unordered_map<std::string, bool> enabledFeatures;
    bool performanceMonitoringEnabled;
    
    // Performance tracking
    std::vector<AdvancedExperimentResult> recentResults;
    std::atomic<double> totalPerformanceScore{0.0};
    
    Impl() : globalSeed(42), 
             executionMode(ExecutionMode::SEQUENTIAL),
             performanceMonitoringEnabled(false) {
        // Enable default features
        enabledFeatures["replay"] = false;
        enabledFeatures["checkpointing"] = false;
        enabledFeatures["development"] = false;
        enabledFeatures["curiosity"] = false;
        enabledFeatures["prediction"] = false;
        enabledFeatures["parallel"] = false;
        enabledFeatures["profiling"] = false;
    }
};

ExperimentRunner::ExperimentRunner() : pImpl(new Impl) {}

ExperimentRunner::~ExperimentRunner() = default;

std::shared_ptr<Experiment> ExperimentRunner::createExperiment(const std::string& name,
                                                                const AdvancedExperimentConfig& config) {
    auto exp = std::make_shared<Experiment>(name);
    exp->setSeed(pImpl->globalSeed++);
    
    // Apply advanced configuration
    if (config.enableReplay) {
        exp->enableReplay(true);
    }
    if (config.enableCheckpointing) {
        exp->enableCheckpointing(true);
    }
    if (config.enableDevelopment) {
        exp->enableDevelopment(true);
    }
    
    pImpl->experiments.push_back(exp);
    return exp;
}

std::shared_ptr<Experiment> ExperimentRunner::createPlasticityExperiment(const std::string& name) {
    AdvancedExperimentConfig config;
    config.experimentVariant = AdvancedExperimentConfig::Variant::PLASTICITY;
    config.enableDevelopment = true;
    config.maxSteps = 5000;
    return createExperiment(name, config);
}

std::shared_ptr<Experiment> ExperimentRunner::createDevelopmentalExperiment(const std::string& name) {
    AdvancedExperimentConfig config;
    config.experimentVariant = AdvancedExperimentConfig::Variant::DEVELOPMENTAL;
    config.enableDevelopment = true;
    config.enableCheckpointing = true;
    config.maxSteps = 10000;
    return createExperiment(name, config);
}

std::shared_ptr<Experiment> ExperimentRunner::createSocialLearningExperiment(const std::string& name) {
    AdvancedExperimentConfig config;
    config.experimentVariant = AdvancedExperimentConfig::Variant::SOCIAL_LEARNING;
    config.enableDevelopment = true;
    config.enableCuriosity = true;
    config.maxSteps = 8000;
    return createExperiment(name, config);
}

std::shared_ptr<Experiment> ExperimentRunner::createContinualLearningExperiment(const std::string& name) {
    AdvancedExperimentConfig config;
    config.experimentVariant = AdvancedExperimentConfig::Variant::CONTINUAL_LEARNING;
    config.enableDevelopment = true;
    config.enableCheckpointing = true;
    config.maxSteps = 15000;
    return createExperiment(name, config);
}

std::shared_ptr<Experiment> ExperimentRunner::createNoveltyDrivenExperiment(const std::string& name) {
    AdvancedExperimentConfig config;
    config.experimentVariant = AdvancedExperimentConfig::Variant::NOVELTY_DRIVEN;
    config.enableCuriosity = true;
    config.enableNovelty = true;
    config.maxSteps = 12000;
    return createExperiment(name, config);
}

AdvancedExperimentResult ExperimentRunner::runExperiment(std::shared_ptr<Experiment> experiment,
                                                         std::shared_ptr<Brain> brain,
                                                         std::shared_ptr<Environment> environment,
                                                         const AdvancedExperimentConfig& config) {
    AdvancedExperimentResult result;
    result.experimentName = experiment->getName();
    result.seed = experiment->getSeed();
    result.startTime = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO("Starting experiment: " + result.experimentName);
    
    if (!experiment || !brain || !environment) {
        NLM_LOG_ERROR("Invalid experiment, brain, or environment");
        result.success = false;
        return result;
    }
    
    try {
        experiment->setStartStep(0);
        
        // Reset environment and brain
        auto observation = environment->reset();
        brain->receiveSensoryInput(*observation);
        
        // Performance monitoring setup
        auto perfStart = std::chrono::high_resolution_clock::now();
        
        // Run simulation loop with advanced features
        for (SimulationStep step = 0; step < config.maxSteps && !environment->isDone(); ++step) {
            // Brain step
            brain->step(step, step * config.timestep);
            
            // Record advanced metrics
            experiment->recordStep(
                step,
                brain->getTotalNeuronCount(),
                brain->getTotalSynapseCount(),
                brain->getFiringNeuronCount(),
                brain->getAverageFiringRate()
            );
            
            // Get action from brain
            auto action = brain->produceAction();
            
            // Environment step
            observation = environment->step(*action);
            
            // Send observation to brain
            brain->receiveSensoryInput(*observation);
            
            // Record reward and apply neuromodulation
            float reward = environment->getLastReward();
            experiment->recordMetric("total_reward", 
                experiment->getMetric("total_reward") + reward);
            
            // Apply advanced neuromodulation if enabled
            if (config.enableCuriosity) {
                // Curiosity-driven exploration
                float curiosity = brain->getCuriosityLevel();
                experiment->recordMetric("curiosity", curiosity);
            }
            
            if (config.enablePrediction) {
                // Prediction error tracking
                float predictionError = brain->getPredictionError();
                experiment->recordMetric("prediction_error", predictionError);
            }
        }
        
        experiment->setEndStep(config.maxSteps);
        result.success = true;
        
        // Performance calculation
        auto perfEnd = std::chrono::high_resolution_clock::now();
        auto wallTime = std::chrono::duration<double>(perfEnd - perfStart).count();
        result.wallClockTime = wallTime;
        result.cpuTime = wallTime; // Simplified for now
        result.stepsPerSecond = config.maxSteps / wallTime;
        
        // Collect comprehensive metrics
        result.totalNeurons = brain->getTotalNeuronCount();
        result.totalSynapses = brain->getTotalSynapseCount();
        result.avgFiringRate = brain->getAverageFiringRate();
        result.maxActiveTraces = (brain->getWorkingMemory() ? 
            brain->getWorkingMemory()->getMaxActiveTraces() : 0);
        result.totalEpisodesStored = (brain->getEpisodicMemory() ? 
            brain->getEpisodicMemory()->getEpisodeCount() : 0);
        
        // Integration verification
        result.memoryIntegrated = (brain->getWorkingMemory() != nullptr);
        result.neuromodulationIntegrated = (brain->getDopamine() != nullptr);
        result.predictionIntegrated = (brain->getPredictionSystem() != nullptr);
        result.developmentIntegrated = (brain->getDevelopmentSystem() != nullptr);
        
        // Advanced learning metrics
        result.finalLearningProgress = experiment->getMetric("learning_progress");
        result.predictionAccuracy = experiment->getMetric("prediction_accuracy");
        result.rewardEfficiency = experiment->getMetric("reward_efficiency");
        result.explorationRate = experiment->getMetric("exploration_rate");
        
        // Neuromodulation levels
        result.dopamineLevel = (brain->getDopamine() ? 
            brain->getDopamine()->getLevel() : 0.0f);
        result.curiosityLevel = (brain->getCuriosity() ? 
            brain->getCuriosity()->getLevel() : 0.0f);
        result.noveltyLevel = (brain->getNovelty() ? 
            brain->getNovelty()->getLevel() : 0.0f);
        result.predictionError = (brain->getPredictionError() ? 
            brain->getPredictionError()->getError() : 0.0f);
        
        // Development stage
        result.developmentalStage = (brain->getDevelopmentSystem() ? 
            brain->getDevelopmentSystem()->getCurrentStage() : 0.0f);
        
        // Store result for performance tracking
        pImpl->recentResults.push_back(result);
        
        // Update performance score
        double performanceScore = calculatePerformanceScore(result);
        pImpl->totalPerformanceScore += performanceScore;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Experiment failed with exception: " + std::string(e.what()));
        result.success = false;
    }
    
    result.endTime = std::chrono::high_resolution_clock::now();
    return result;
}

std::vector<AdvancedExperimentResult> ExperimentRunner::runBatch(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    const AdvancedExperimentConfig& config) {
    
    std::vector<AdvancedExperimentResult> results;
    results.reserve(experiments.size());
    
    if (pImpl->executionMode == ExecutionMode::PARALLEL && config.enableParallel && experiments.size() > 1) {
        // Parallel execution
        std::vector<std::thread> threads;
        std::mutex resultsMutex;
        
        for (auto& exp : experiments) {
            threads.emplace_back([&, exp]() {
                auto brain = std::make_shared<Brain>(brainTemplate->getConfig());
                brain->initialize();
                
                AdvancedExperimentResult result = runExperiment(exp, brain, environmentTemplate, config);
                
                std::lock_guard<std::mutex> lock(resultsMutex);
                results.push_back(result);
            });
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
    } else {
        // Sequential execution
        for (auto& exp : experiments) {
            auto brain = std::make_shared<Brain>(brainTemplate->getConfig());
            brain->initialize();
            
            AdvancedExperimentResult result = runExperiment(exp, brain, environmentTemplate, config);
            results.push_back(result);
        }
    }
    
    return results;
}

const std::vector<std::shared_ptr<Experiment>>& ExperimentRunner::getExperiments() const {
    return pImpl->experiments;
}

std::shared_ptr<Experiment> ExperimentRunner::getExperiment(const std::string& name) const {
    for (auto& exp : pImpl->experiments) {
        if (exp->getName() == name) {
            return exp;
        }
    }
    return nullptr;
}

void ExperimentRunner::clearExperiments() {
    pImpl->experiments.clear();
}

void ExperimentRunner::setGlobalSeed(uint64_t seed) {
    pImpl->globalSeed = seed;
}

bool ExperimentRunner::saveResults(const std::string& filepath, 
                                   const std::vector<AdvancedExperimentResult>& results) const {
    if (results.empty()) {
        NLM_LOG_ERROR("No results to save");
        return false;
    }
    
    try {
        // Try JSON format first
        if (filepath.substr(filepath.find_last_of(".") + 1) == "json" || 
            pImpl->enabledFeatures["enableJSONOutput"]) {
            return exportResultsToJSON(results, filepath);
        } else if (filepath.substr(filepath.find_last_of(".") + 1) == "csv" ||
                  pImpl->enabledFeatures["enableCSVOutput"]) {
            return exportResultsToCSV(results, filepath);
        } else if (filepath.substr(filepath.find_last_of(".") + 1) == "bin" ||
                  pImpl->enabledFeatures["enableBinaryOutput"]) {
            return exportResultsToBinary(results, filepath);
        } else {
            // Default to JSON
            std::string jsonPath = filepath;
            if (jsonPath.find(".") == std::string::npos) {
                jsonPath += ".json";
            }
            return exportResultsToJSON(results, jsonPath);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save results: " + std::string(e.what()));
        return false;
    }
}

bool ExperimentRunner::loadConfig(const std::string& filepath, AdvancedExperimentConfig& config) const {
    // TODO: Implement config loading from file
    NLM_LOG_INFO("Config loading not yet implemented: " + filepath);
    return false;
}

bool ExperimentRunner::saveConfig(const std::string& filepath, const AdvancedExperimentConfig& config) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open file for writing: " + filepath);
            return false;
        }
        
        // Simple key=value format
        file << "# NLM Advanced Experiment Configuration" << std::endl;
        file << "# Generated on " << __DATE__ << " at " << __TIME__ << std::endl;
        file << std::endl;
        
        file << "max_steps = " << config.maxSteps << std::endl;
        file << "timestep = " << config.timestep << std::endl;
        file << "random_seed = " << config.randomSeed << std::endl;
        file << "enable_replay = " << (config.enableReplay ? "true" : "false") << std::endl;
        file << "enable_checkpointing = " << (config.enableCheckpointing ? "true" : "false") << std::endl;
        file << "enable_development = " << (config.enableDevelopment ? "true" : "false") << std::endl;
        file << "enable_curiosity = " << (config.enableCuriosity ? "true" : "false") << std::endl;
        file << "enable_prediction = " << (config.enablePrediction ? "true" : "false") << std::endl;
        
        file.close();
        NLM_LOG_INFO("Configuration saved to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save config: " + std::string(e.what()));
        return false;
    }
}

bool ExperimentRunner::validateConfig(const AdvancedExperimentConfig& config) const {
    std::vector<std::string> errors;
    
    if (config.maxSteps == 0) {
        errors.push_back("maxSteps must be greater than 0");
    }
    
    if (config.timestep <= 0.0) {
        errors.push_back("timestep must be positive");
    }
    
    if (config.numThreads < 1) {
        errors.push_back("numThreads must be at least 1");
    }
    
    if (config.batchSize < 1) {
        errors.push_back("batchSize must be at least 1");
    }
    
    if (!errors.empty()) {
        for (const auto& error : errors) {
            NLM_LOG_ERROR("Config validation error: " + error);
        }
        return false;
    }
    
    return true;
}

size_t ExperimentRunner::getExperimentCount() const {
    return pImpl->experiments.size();
}

float ExperimentRunner::getAverageSuccessRate() const {
    if (pImpl->recentResults.empty()) return 0.0f;
    
    size_t successCount = 0;
    for (const auto& result : pImpl->recentResults) {
        if (result.success) successCount++;
    }
    
    return static_cast<float>(successCount) / pImpl->recentResults.size();
}

double ExperimentRunner::getAverageWallClockTime() const {
    if (pImpl->recentResults.empty()) return 0.0;
    
    double totalTime = 0.0;
    for (const auto& result : pImpl->recentResults) {
        totalTime += result.wallClockTime;
    }
    
    return totalTime / pImpl->recentResults.size();
}

bool ExperimentRunner::exportResultsToJSON(const std::vector<AdvancedExperimentResult>& results, 
                                          const std::string& filepath) const {
    try {
        json j = results;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << j.dump(2);
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to export JSON: " + std::string(e.what()));
        return false;
    }
}

bool ExperimentRunner::exportResultsToCSV(const std::vector<AdvancedExperimentResult>& results,
                                         const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Write header
        file << "ExperimentName,Seed,Success,WallClockTime,CPUTime,StepsPerSecond,";
        file << "TotalNeurons,TotalSynapses,AvgFiringRate,MaxActiveTraces,TotalEpisodesStored";
        file << ",FinalLearningProgress,PredictionAccuracy,RewardEfficiency,ExplorationRate";
        file << ",DopamineLevel,CuriosityLevel,NoveltyLevel,PredictionError";
        file << ",DevelopmentalStage,MemoryIntegrated,NeuromodulationIntegrated";
        file << ",PredictionIntegrated,DevelopmentIntegrated,ReplayWorks,CheckpointingWorks";
        file << std::endl;
        
        // Write data
        for (const auto& result : results) {
            file << result.experimentName << ",";
            file << result.seed << ",";
            file << (result.success ? "true" : "false") << ",";
            file << result.wallClockTime << ",";
            file << result.cpuTime << ",";
            file << result.stepsPerSecond << ",";
            file << result.totalNeurons << ",";
            file << result.totalSynapses << ",";
            file << result.avgFiringRate << ",";
            file << result.maxActiveTraces << ",";
            file << result.totalEpisodesStored << ",";
            file << result.finalLearningProgress << ",";
            file << result.predictionAccuracy << ",";
            file << result.rewardEfficiency << ",";
            file << result.explorationRate << ",";
            file << result.dopamineLevel << ",";
            file << result.curiosityLevel << ",";
            file << result.noveltyLevel << ",";
            file << result.predictionError << ",";
            file << result.developmentalStage << ",";
            file << (result.memoryIntegrated ? "true" : "false") << ",";
            file << (result.neuromodulationIntegrated ? "true" : "false") << ",";
            file << (result.predictionIntegrated ? "true" : "false") << ",";
            file << (result.developmentIntegrated ? "true" : "false") << ",";
            file << (result.replayWorks ? "true" : "false") << ",";
            file << (result.checkpointingWorks ? "true" : "false") << ";";
            file << std::endl;
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to export CSV: " + std::string(e.what()));
        return false;
    }
}

bool ExperimentRunner::exportResultsToBinary(const std::vector<AdvancedExperimentResult>& results,
                                            const std::string& filepath) const {
    try {
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Write number of results
        size_t resultCount = results.size();
        file.write(reinterpret_cast<const char*>(&resultCount), sizeof(resultCount));
        
        // Write each result (simplified binary format)
        for (const auto& result : results) {
            // Write key fields (simplified - in real implementation would be more comprehensive)
            file.write(reinterpret_cast<const char*>(&result.success), sizeof(result.success));
            file.write(reinterpret_cast<const char*>(&result.wallClockTime), sizeof(result.wallClockTime));
            file.write(reinterpret_cast<const char*>(&result.totalNeurons), sizeof(result.totalNeurons));
            file.write(reinterpret_cast<const char*>(&result.avgFiringRate), sizeof(result.avgFiringRate));
            
            // Write strings with length prefix
            auto writeString = [&file](const std::string& str) {
                uint32_t len = static_cast<uint32_t>(str.size());
                file.write(reinterpret_cast<const char*>(&len), sizeof(len));
                file.write(str.c_str(), len);
            };
            
            writeString(result.experimentName);
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to export binary: " + std::string(e.what()));
        return false;
    }
}

double ExperimentRunner::calculatePerformanceScore(const AdvancedExperimentResult& result) const {
    double score = 0.0;
    
    // Success bonus
    if (result.success) score += 50.0;
    
    // Performance metrics
    if (result.wallClockTime > 0) {
        score += std::min(25.0, 1000.0 / result.wallClockTime); // Faster is better
    }
    
    // Learning progress
    score += std::min(15.0, result.finalLearningProgress * 10.0);
    
    // Integration quality
    if (result.memoryIntegrated) score += 5.0;
    if (result.neuromodulationIntegrated) score += 5.0;
    if (result.predictionIntegrated) score += 5.0;
    if (result.developmentIntegrated) score += 5.0;
    
    return score;
}

void ExperimentRunner::setExecutionMode(ExecutionMode mode) {
    pImpl->executionMode = mode;
}

void ExperimentRunner::enableFeature(const std::string& featureName, bool enable) {
    pImpl->enabledFeatures[featureName] = enable;
}

bool ExperimentRunner::isFeatureEnabled(const std::string& featureName) const {
    auto it = pImpl->enabledFeatures.find(featureName);
    return (it != pImpl->enabledFeatures.end()) ? it->second : false;
}

std::vector<std::string> ExperimentRunner::getRecommendedExperiments(const AdvancedExperimentConfig& config) const {
    std::vector<std::string> recommendations;
    
    if (config.enableDevelopment && config.enableCuriosity) {
        recommendations.push_back("NoveltyDrivenExperiment");
    }
    
    if (config.enableCheckpointing && config.maxSteps > 10000) {
        recommendations.push_back("ContinualLearningExperiment");
    }
    
    if (config.enablePrediction && config.enableDevelopment) {
        recommendations.push_back("DevelopmentalExperiment");
    }
    
    if (config.enableCuriosity && config.enablePrediction) {
        recommendations.push_back("SocialLearningExperiment");
    }
    
    return recommendations;
}

std::string ExperimentRunner::generateAnalysisReport(const std::vector<AdvancedExperimentResult>& results) const {
    std::stringstream report;
    
    report << "=== NLM Advanced Experiment Analysis Report ===" << std::endl;
    report << "Generated on: " << __DATE__ << " at " << __TIME__ << std::endl;
    report << "Total Experiments: " << results.size() << std::endl;
    report << std::endl;
    
    if (results.empty()) {
        report << "No experiment results to analyze." << std::endl;
        return report.str();
    }
    
    // Summary statistics
    size_t successfulExperiments = 0;
    double totalWallTime = 0.0;
    double totalPerformanceScore = 0.0;
    
    for (const auto& result : results) {
        if (result.success) successfulExperiments++;
        totalWallTime += result.wallClockTime;
        totalPerformanceScore += calculatePerformanceScore(result);
    }
    
    report << "=== Summary Statistics ===" << std::endl;
    report << "Successful Experiments: " << successfulExperiments << " / " << results.size() << std::endl;
    report << "Success Rate: " << (static_cast<double>(successfulExperiments) / results.size() * 100) << "%" << std::endl;
    report << "Average Wall Clock Time: " << (totalWallTime / results.size()) << "s" << std::endl;
    report << "Average Performance Score: " << (totalPerformanceScore / results.size()) << std::endl;
    report << std::endl;
    
    // Integration analysis
    report << "=== Integration Analysis ===" << std::endl;
    size_t memoryIntegrated = 0, neuromodulationIntegrated = 0, predictionIntegrated = 0, developmentIntegrated = 0;
    
    for (const auto& result : results) {
        if (result.memoryIntegrated) memoryIntegrated++;
        if (result.neuromodulationIntegrated) neuromodulationIntegrated++;
        if (result.predictionIntegrated) predictionIntegrated++;
        if (result.developmentIntegrated) developmentIntegrated++;
    }
    
    report << "Memory Integration: " << memoryIntegrated << "/" << results.size() << " (" 
           << (static_cast<double>(memoryIntegrated) / results.size() * 100) << "%)" << std::endl;
    report << "Neuromodulation Integration: " << neuromodulationIntegrated << "/" << results.size() << " ("
           << (static_cast<double>(neuromodulationIntegrated) / results.size() * 100) << "%)" << std::endl;
    report << "Prediction Integration: " << predictionIntegrated << "/" << results.size() << " ("
           << (static_cast<double>(predictionIntegrated) / results.size() * 100) << "%)" << std::endl;
    report << "Development Integration: " << developmentIntegrated << "/" << results.size() << " ("
           << (static_cast<double>(developmentIntegrated) / results.size() * 100) << "%)" << std::endl;
    report << std::endl;
    
    // Performance ranking
    report << "=== Performance Ranking ===" << std::endl;
    std::vector<std::pair<double, const AdvancedExperimentResult*>> ranked;
    for (const auto& result : results) {
        ranked.emplace_back(calculatePerformanceScore(result), &result);
    }
    std::sort(ranked.begin(), ranked.end(), std::greater<>());
    
    for (size_t i = 0; i < std::min(size_t(5), ranked.size()); ++i) {
        report << (i + 1) << ". " << ranked[i].second->experimentName 
               << " (Score: " << ranked[i].first << ", Time: " << ranked[i].second->wallClockTime << "s)" << std::endl;
    }
    
    return report.str();
}

void ExperimentRunner::enablePerformanceMonitoring(bool enable) {
    pImpl->performanceMonitoringEnabled = enable;
}

double ExperimentRunner::getAveragePerformanceScore() const {
    if (pImpl->recentResults.empty()) return 0.0;
    return pImpl->totalPerformanceScore / pImpl->recentResults.size();
}

std::vector<std::string> ExperimentRunner::getValidConfigKeys() {
    return {
        "max_steps", "timestep", "random_seed", "enable_replay", 
        "enable_checkpointing", "enable_development", "enable_curiosity",
        "enable_prediction", "enable_parallel", "enable_profiling",
        "num_threads", "batch_size", "experiment_variant"
    };
}

std::unordered_map<std::string, std::string> ExperimentRunner::getConfigDescription(const std::string& key) {
    static const std::unordered_map<std::string, std::string> descriptions = {
        {"max_steps", "Maximum number of simulation steps"},
        {"timestep", "Simulation timestep in seconds"},
        {"random_seed", "Random seed for reproducible experiments"},
        {"enable_replay", "Enable episodic memory replay"},
        {"enable_checkpointing", "Enable checkpoint save/load"},
        {"enable_development", "Enable developmental processes"},
        {"enable_curiosity", "Enable curiosity-driven exploration"},
        {"enable_prediction", "Enable predictive processing"},
        {"enable_parallel", "Enable parallel experiment execution"},
        {"enable_profiling", "Enable performance profiling"},
        {"num_threads", "Number of threads for parallel processing"},
        {"batch_size", "Batch size for large simulations"},
        {"experiment_variant", "Type of experiment to run"}
    };
    
    auto it = descriptions.find(key);
    return (it != descriptions.end()) ? std::unordered_map<std::string, std::string>{ {key, it->second} } : std::unordered_map<std::string, std::string>{};
}

} // namespace nlm

// JSON serialization for AdvancedExperimentResult (needed for exportResultsToJSON)
void to_json(json& j, const AdvancedExperimentResult& result) {
    j = json{
        {"experimentName", result.experimentName},
        {"seed", result.seed},
        {"success", result.success},
        {"wallClockTime", result.wallClockTime},
        {"cpuTime", result.cpuTime},
        {"stepsPerSecond", result.stepsPerSecond},
        {"totalNeurons", result.totalNeurons},
        {"totalSynapses", result.totalSynapses},
        {"avgFiringRate", result.avgFiringRate},
        {"avgSynapticWeight", result.avgSynapticWeight},
        {"maxActiveTraces", result.maxActiveTraces},
        {"totalEpisodesStored", result.totalEpisodesStored},
        {"finalLearningProgress", result.finalLearningProgress},
        {"predictionAccuracy", result.predictionAccuracy},
        {"rewardEfficiency", result.rewardEfficiency},
        {"explorationRate", result.explorationRate},
        {"dopamineLevel", result.dopamineLevel},
        {"curiosityLevel", result.curiosityLevel},
        {"noveltyLevel", result.noveltyLevel},
        {"predictionError", result.predictionError},
        {"developmentalStage", result.developmentalStage},
        {"memoryIntegrated", result.memoryIntegrated},
        {"neuromodulationIntegrated", result.neuromodulationIntegrated},
        {"predictionIntegrated", result.predictionIntegrated},
        {"developmentIntegrated", result.developmentIntegrated},
        {"replayWorks", result.replayWorks},
        {"checkpointingWorks", result.checkpointingWorks},
        {"customMetrics", result.customMetrics}
    };
}

void from_json(const json& j, AdvancedExperimentResult& result) {
    j.at("experimentName").get_to(result.experimentName);
    j.at("seed").get_to(result.seed);
    j.at("success").get_to(result.success);
    j.at("wallClockTime").get_to(result.wallClockTime);
    j.at("cpuTime").get_to(result.cpuTime);
    j.at("stepsPerSecond").get_to(result.stepsPerSecond);
    j.at("totalNeurons").get_to(result.totalNeurons);
    j.at("totalSynapses").get_to(result.totalSynapses);
    j.at("avgFiringRate").get_to(result.avgFiringRate);
    j.at("avgSynapticWeight").get_to(result.avgSynapticWeight);
    j.at("maxActiveTraces").get_to(result.maxActiveTraces);
    j.at("totalEpisodesStored").get_to(result.totalEpisodesStored);
    j.at("finalLearningProgress").get_to(result.finalLearningProgress);
    j.at("predictionAccuracy").get_to(result.predictionAccuracy);
    j.at("rewardEfficiency").get_to(result.rewardEfficiency);
    j.at("explorationRate").get_to(result.explorationRate);
    j.at("dopamineLevel").get_to(result.dopamineLevel);
    j.at("curiosityLevel").get_to(result.curiosityLevel);
    j.at("noveltyLevel").get_to(result.noveltyLevel);
    j.at("predictionError").get_to(result.predictionError);
    j.at("developmentalStage").get_to(result.developmentalStage);
    j.at("memoryIntegrated").get_to(result.memoryIntegrated);
    j.at("neuromodulationIntegrated").get_to(result.neuromodulationIntegrated);
    j.at("predictionIntegrated").get_to(result.predictionIntegrated);
    j.at("developmentIntegrated").get_to(result.developmentIntegrated);
    j.at("replayWorks").get_to(result.replayWorks);
    j.at("checkpointingWorks").get_to(result.checkpointingWorks);
    j.at("customMetrics").get_to(result.customMetrics);
}
