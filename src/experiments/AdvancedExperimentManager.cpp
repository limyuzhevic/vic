#include "AdvancedExperimentManager.hpp"
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <random>
#include <cmath>

namespace nlm {

struct AdvancedExperimentManager::Impl {
    std::vector<Phase6IntegrationResult> experimentResults;
    std::map<std::string, json> experimentData;
    json optimizationHistory;
    bool initialized;
};

AdvancedExperimentManager::AdvancedExperimentManager() : pImpl(std::make_unique<Impl>()) {
    pImpl->initialized = false;
}

AdvancedExperimentManager::~AdvancedExperimentManager() = default;

std::vector<Phase6IntegrationResult> AdvancedExperimentManager::runExperimentSet(
    const std::vector<Phase6Config>& configs,
    const std::string& experimentPrefix) {
    
    std::cout << "Running experiment set with " << configs.size() << " configurations..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    std::vector<Phase6IntegrationResult> results;
    
    for (size_t i = 0; i < configs.size(); ++i) {
        std::string currentPrefix = experimentPrefix + "_" + std::to_string(i);
        std::cout << "  Running experiment " << i + 1 << "/" << configs.size() << ": " << currentPrefix << std::endl;
        
        Phase6Config config = configs[i];
        config.checkpointPath = currentPrefix + "_checkpoint.bin";
        
        Phase6IntegrationResult result = experiment.run(config);
        results.push_back(result);
        
        // Store experiment data
        json data;
        data["config"] = {
            {"neuronCount", config.neuronCount},
            {"maxSteps", config.maxSteps},
            {"regionCount", config.regionCount},
            {"connectionProbability", config.connectionProbability},
            {"enableCheckpointing", config.enableCheckpointing},
            {"enableReplay", config.enableReplay},
            {"enableDevelopment", config.enableDevelopment}
        };
        data["result"] = {
            {"totalReward", result.totalReward},
            {"avgFiringRate", result.avgFiringRate},
            {"memoryEpisodesStored", result.memoryEpisodesStored},
            {"noveltyLevel", result.noveltyLevel},
            {"curiosityLevel", result.curiosityLevel},
            {"dopamineLevel", result.dopamineLevel},
            {"memoryWorkingMemoryIntegrated", result.memoryWorkingMemoryIntegrated},
            {"memoryEpisodicMemoryIntegrated", result.memoryEpisodicMemoryIntegrated},
            {"neuromodulationIntegrated", result.neuromodulationIntegrated},
            {"predictionIntegrated", result.predictionIntegrated},
            {"developmentIntegrated", result.developmentIntegrated},
            {"checkpointingWorks", result.checkpointingWorks},
            {"replayWorks", result.replayWorks},
            {"totalWallClockTime", result.totalWallClockTime}
        };
        
        pImpl->experimentData[currentPrefix] = data;
    }
    
    pImpl->experimentResults = results;
    pImpl->initialized = true;
    
    std::cout << "Experiment set completed. Results: " << results.size() << " experiments." << std::endl;
    return results;
}

json AdvancedExperimentManager::compareExperiments(const std::vector<Phase6IntegrationResult>& results) {
    json comparison;
    
    if (results.empty()) {
        return comparison;
    }
    
    comparison["experiments"] = results.size();
    
    // Calculate statistics for each metric
    std::vector<double> totalRewards, firingRates, wallClockTimes;
    std::vector<bool> learningSuccess;
    
    for (const auto& result : results) {
        totalRewards.push_back(result.totalReward);
        firingRates.push_back(result.avgFiringRate);
        wallClockTimes.push_back(result.totalWallClockTime);
        learningSuccess.push_back(result.totalReward > 0);
    }
    
    // Mean values
    double meanReward = std::accumulate(totalRewards.begin(), totalRewards.end(), 0.0) / totalRewards.size();
    double meanFiring = std::accumulate(firingRates.begin(), firingRates.end(), 0.0) / firingRates.size();
    double meanTime = std::accumulate(wallClockTimes.begin(), wallClockTimes.end(), 0.0) / wallClockTimes.size();
    
    double successRate = std::accumulate(learningSuccess.begin(), learningSuccess.end(), 0.0) / learningSuccess.size() * 100.0;
    
    comparison["statistics"]["totalReward"] = {
        {"mean", meanReward},
        {"min", *std::min_element(totalRewards.begin(), totalRewards.end())},
        {"max", *std::max_element(totalRewards.begin(), totalRewards.end())},
        {"stddev", calculateStdDev(totalRewards, meanReward)},
        {"median", calculateMedian(totalRewards)}
    };
    
    comparison["statistics"]["firingRate"] = {
        {"mean", meanFiring},
        {"min", *std::min_element(firingRates.begin(), firingRates.end())},
        {"max", *std::max_element(firingRates.begin(), firingRates.end())},
        {"stddev", calculateStdDev(firingRates, meanFiring)},
        {"median", calculateMedian(firingRates)}
    };
    
    comparison["statistics"]["wallClockTime"] = {
        {"mean", meanTime},
        {"min", *std::min_element(wallClockTimes.begin(), wallClockTimes.end())},
        {"max", *std::max_element(wallClockTimes.begin(), wallClockTimes.end())},
        {"stddev", calculateStdDev(wallClockTimes, meanTime)},
        {"median", calculateMedian(wallClockTimes)}
    };
    
    comparison["statistics"]["learningSuccessRate"] = successRate;
    
    // Find best and worst experiments
    auto bestByReward = std::max_element(totalRewards.begin(), totalRewards.end());
    auto worstByReward = std::min_element(totalRewards.begin(), totalRewards.end());
    
    comparison["bestExperiment"] = "byReward:" + std::to_string(std::distance(totalRewards.begin(), bestByReward));
    comparison["worstExperiment"] = "byReward:" + std::to_string(std::distance(totalRewards.begin(), worstByReward));
    
    // Correlation between metrics
    json correlations = calculateCorrelations(totalRewards, firingRates, wallClockTimes);
    comparison["correlations"] = correlations;
    
    return comparison;
}

Phase6Config AdvancedExperimentManager::findOptimalParameters(
    const std::vector<Phase6Config>& configs,
    const std::vector<Phase6IntegrationResult>& results) {
    
    if (configs.size() != results.size() || configs.empty()) {
        return configs.empty() ? Phase6Config() : configs[0];
    }
    
    // Simple optimization based on total reward (higher is better)
    double bestReward = -1.0;
    Phase6Config optimalConfig;
    
    for (size_t i = 0; i < configs.size(); ++i) {
        double reward = results[i].totalReward;
        if (reward > bestReward) {
            bestReward = reward;
            optimalConfig = configs[i];
        }
    }
    
    std::cout << "Optimal configuration found with total reward: " << bestReward << std::endl;
    std::cout << "  Neurons: " << optimalConfig.neuronCount << ", Steps: " << optimalConfig.maxSteps << std::endl;
    std::cout << "  Regions: " << optimalConfig.regionCount << ", Connection probability: " << optimalConfig.connectionProbability << std::endl;
    
    return optimalConfig;
}

bool AdvancedExperimentManager::exportResults(
    const std::vector<Phase6IntegrationResult>& results,
    const std::string& directory,
    const std::vector<std::string>& formats) {
    
    // Create directory if it doesn't exist
    if (!std::filesystem::exists(directory)) {
        std::filesystem::create_directories(directory);
    }
    
    std::cout << "Exporting results to: " << directory << std::endl;
    
    // Export as JSON
    if (std::find(formats.begin(), formats.end(), "json") != formats.end()) {
        std::string jsonFile = directory + "/results.json";
        std::ofstream file(jsonFile);
        if (file.is_open()) {
            json jsonData;
            jsonData["experiments"] = results.size();
            jsonData["timestamp"] = getCurrentTimestamp();
            
            json experimentsArray = json::array();
            for (size_t i = 0; i < results.size(); ++i) {
                json expData;
                expData["experiment_id"] = i;
                expData["total_reward"] = results[i].totalReward;
                expData["avg_firing_rate"] = results[i].avgFiringRate;
                expData["memory_episodes_stored"] = results[i].memoryEpisodesStored;
                expData["novelty_level"] = results[i].noveltyLevel;
                expData["curiosity_level"] = results[i].curiosityLevel;
                expData["dopamine_level"] = results[i].dopamineLevel;
                expData["learning_success"] = results[i].totalReward > 0;
                
                experimentsArray.push_back(expData);
            }
            
            jsonData["experiments"] = experimentsArray;
            file << jsonData.dump(2);
            file.close();
            std::cout << "  JSON export: " << jsonFile << std::endl;
        }
    }
    
    // Export as CSV
    if (std::find(formats.begin(), formats.end(), "csv") != formats.end()) {
        std::string csvFile = directory + "/results.csv";
        std::ofstream file(csvFile);
        if (file.is_open()) {
            file << "experiment_id,total_reward,avg_firing_rate,memory_episodes_stored,novelty_level,curiosity_level,dopamine_level,learning_success,wall_clock_time" << std::endl;
            
            for (size_t i = 0; i < results.size(); ++i) {
                file << i << ","
                     << results[i].totalReward << ","
                     << results[i].avgFiringRate << ","
                     << results[i].memoryEpisodesStored << ","
                     << results[i].noveltyLevel << ","
                     << results[i].curiosityLevel << ","
                     << results[i].dopamineLevel << ","
                     << (results[i].totalReward > 0 ? "true" : "false") << ","
                     << results[i].totalWallClockTime << std::endl;
            }
            
            file.close();
            std::cout << "  CSV export: " << csvFile << std::endl;
        }
    }
    
    // Export as Markdown
    if (std::find(formats.begin(), formats.end(), "markdown") != formats.end()) {
        std::string mdFile = directory + "/results.md";
        std::ofstream file(mdFile);
        if (file.is_open()) {
            file << "# NLM Experiment Results" << std::endl << std::endl;
            file << "| Experiment ID | Total Reward | Avg Firing Rate | Memory Episodes | Novelty Level | Curiosity Level | Dopamine Level | Learning Success | Wall Clock Time |" << std::endl;
            file << "|--------------|--------------|-----------------|-----------------|---------------|-----------------|----------------|------------------|-----------------|" << std::endl;
            
            for (size_t i = 0; i < results.size(); ++i) {
                file << "| " << i << " | "
                     << results[i].totalReward << " | "
                     << results[i].avgFiringRate << " | "
                     << results[i].memoryEpisodesStored << " | "
                     << results[i].noveltyLevel << " | "
                     << results[i].curiosityLevel << " | "
                     << results[i].dopamineLevel << " | "
                     << (results[i].totalReward > 0 ? "✓" : "✗") << " | "
                     << results[i].totalWallClockTime << " |" << std::endl;
            }
            
            file.close();
            std::cout << "  Markdown export: " << mdFile << std::endl;
        }
    }
    
    return true;
}

std::string AdvancedExperimentManager::generateReport(
    const std::vector<Phase6IntegrationResult>& results,
    const std::string& title) {
    
    std::stringstream report;
    
    report << "# " << title << "\n\n";
    report << "Generated on: " << getCurrentTimestamp() << "\n\n";
    report << "## Summary" << std::endl;
    report << "- Total experiments: " << results.size() << "\n";
    report << "- Successful experiments: " << std::count_if(results.begin(), results.end(),
        [](const Phase6IntegrationResult& r) { return r.totalReward > 0; }) << "\n";
    report << "- Overall success rate: " << calculateSuccessRate(results) * 100 << "%\n\n";
    
    if (!results.empty()) {
        double avgReward = calculateMean(results, [](const Phase6IntegrationResult& r) { return r.totalReward; });
        double avgFiring = calculateMean(results, [](const Phase6IntegrationResult& r) { return r.avgFiringRate; });
        
        report << "## Statistics" << std::endl;
        report << "- Average total reward: " << avgReward << "\n";
        report << "- Average firing rate: " << avgFiring << " Hz\n";
        report << "- Average wall clock time: " << calculateMean(results, [](const Phase6IntegrationResult& r) { return r.totalWallClockTime; }) << " s\n\n";
        
        report << "## Individual Experiments" << std::endl;
        report << "| ID | Total Reward | Firing Rate | Novelty | Curiosity | Dopamine | Success | Time (s) |" << std::endl;
        report << "|----|--------------|-------------|---------|-----------|----------|---------|----------|" << std::endl;
        
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& r = results[i];
            report << "| " << i << " | " << r.totalReward << " | " << r.avgFiringRate << " | "
                   << r.noveltyLevel << " | " << r.curiosityLevel << " | " << r.dopamineLevel << " | "
                   << (r.totalReward > 0 ? "✓" : "✗") << " | " << r.totalWallClockTime << " |" << std::endl;
        }
        
        report << "\n## Integration Status" << std::endl;
        report << "- Working Memory Integrated: " << (std::all_of(results.begin(), results.end(),
                     [](const Phase6IntegrationResult& r) { return r.memoryWorkingMemoryIntegrated; }) ? "✓ All" : "✗ Some") << "\n";
        report << "- Episodic Memory Integrated: " << (std::all_of(results.begin(), results.end(),
                     [](const Phase6IntegrationResult& r) { return r.memoryEpisodicMemoryIntegrated; }) ? "✓ All" : "✗ Some") << "\n";
        report << "- Neuromodulation Integrated: " << (std::all_of(results.begin(), results.end(),
                     [](const Phase6IntegrationResult& r) { return r.neuromodulationIntegrated; }) ? "✓ All" : "✗ Some") << "\n";
        report << "- Prediction Integrated: " << (std::all_of(results.begin(), results.end(),
                     [](const Phase6IntegrationResult& r) { return r.predictionIntegrated; }) ? "✓ All" : "✗ Some") << "\n";
        report << "- Development Integrated: " << (std::all_of(results.begin(), results.end(),
                     [](const Phase6IntegrationResult& r) { return r.developmentIntegrated; }) ? "✓ All" : "✗ Some") << "\n";
    }
    
    return report.str();
}

json AdvancedExperimentManager::performStatisticalAnalysis(const std::vector<Phase6IntegrationResult>& results) {
    json analysis;
    
    if (results.empty()) {
        return analysis;
    }
    
    // Extract metrics
    std::vector<double> rewards, firingRates, times, novelties, curiosities, dopamines;
    std::vector<bool> successes;
    
    for (const auto& result : results) {
        rewards.push_back(result.totalReward);
        firingRates.push_back(result.avgFiringRate);
        times.push_back(result.totalWallClockTime);
        novelties.push_back(result.noveltyLevel);
        curiosities.push_back(result.curiosityLevel);
        dopamines.push_back(result.dopamineLevel);
        successes.push_back(result.totalReward > 0);
    }
    
    // Calculate descriptive statistics
    json descriptiveStats;
    descriptiveStats["totalReward"] = calculateDescriptiveStats(rewards);
    descriptiveStats["firingRate"] = calculateDescriptiveStats(firingRates);
    descriptiveStats["wallClockTime"] = calculateDescriptiveStats(times);
    descriptiveStats["noveltyLevel"] = calculateDescriptiveStats(novelties);
    descriptiveStats["curiosityLevel"] = calculateDescriptiveStats(curiosities);
    descriptiveStats["dopamineLevel"] = calculateDescriptiveStats(dopamines);
    descriptiveStats["successRate"] = calculateSuccessRate(successes) * 100.0;
    
    analysis["descriptiveStatistics"] = descriptiveStats;
    
    // Calculate correlations
    json correlations = calculateCorrelations(rewards, firingRates, times);
    analysis["correlations"] = correlations;
    
    // Normality tests (simplified)
    json normalityTests;
    for (const auto& name : {"totalReward", "firingRate", "wallClockTime", "noveltyLevel", "curiosityLevel", "dopamineLevel"}) {
        json stats;
        if (name == "totalReward") stats = descriptiveStats["totalReward"];
        else if (name == "firingRate") stats = descriptiveStats["firingRate"];
        else if (name == "wallClockTime") stats = descriptiveStats["wallClockTime"];
        else if (name == "noveltyLevel") stats = descriptiveStats["noveltyLevel"];
        else if (name == "curiosityLevel") stats = descriptiveStats["curiosityLevel"];
        else if (name == "dopamineLevel") stats = descriptiveStats["dopamineLevel"];
        
        double skewness = calculateSkewness(stats["values"]);
        double kurtosis = calculateKurtosis(stats["values"]);
        
        stats["skewness"] = skewness;
        stats["kurtosis"] = kurtosis;
        stats["isNormal"] = std::abs(skewness) < 1.0 && std::abs(kurtosis - 3.0) < 1.0;
        
        normalityTests[name] = stats;
    }
    
    analysis["normalityTests"] = normalityTests;
    
    // Outlier detection (simplified)
    json outlierDetection;
    for (const auto& name : {"totalReward", "firingRate"}) {
        double mean = descriptiveStats[name]["mean"].get<double>();
        double stddev = descriptiveStats[name]["stddev"].get<double>();
        
        json outliers;
        for (double value : (name == "totalReward" ? rewards : firingRates)) {
            if (std::abs(value - mean) > 2.0 * stddev) {
                outliers.push_back(value);
            }
        }
        
        outlierDetection[name] = outliers;
    }
    
    analysis["outlierDetection"] = outlierDetection;
    
    return analysis;
}

json AdvancedExperimentManager::analyzeParameterSensitivity(
    const std::vector<Phase6Config>& configs,
    const std::vector<Phase6IntegrationResult>& results,
    const std::string& parameter) {
    
    json sensitivity;
    
    if (configs.size() != results.size() || configs.empty()) {
        return sensitivity;
    }
    
    sensitivity["parameter"] = parameter;
    sensitivity["experiments"] = configs.size();
    
    // Extract parameter values and corresponding rewards
    std::vector<double> paramValues, rewards;
    
    for (size_t i = 0; i < configs.size(); ++i) {
        double paramValue = extractParameterValue(configs[i], parameter);
        double reward = results[i].totalReward;
        
        paramValues.push_back(paramValue);
        rewards.push_back(reward);
    }
    
    // Calculate sensitivity metrics
    double paramRange = *std::max_element(paramValues.begin(), paramValues.end()) -
                       *std::min_element(paramValues.begin(), paramValues.end());
    
    if (paramRange > 0) {
        // Calculate sensitivity as the range of rewards for given parameter range
        double rewardRange = *std::max_element(rewards.begin(), rewards.end()) -
                            *std::min_element(rewards.begin(), rewards.end());
        
        double sensitivityScore = rewardRange / paramRange;
        
        sensitivity["range"] = paramRange;
        sensitivity["rewardRange"] = rewardRange;
        sensitivity["sensitivityScore"] = sensitivityScore;
        
        // Calculate correlation
        double correlation = calculateCorrelation(paramValues, rewards);
        sensitivity["correlation"] = correlation;
        
        // Determine sensitivity level
        std::string sensitivityLevel;
        if (std::abs(correlation) > 0.7) sensitivityLevel = "high";
        else if (std::abs(correlation) > 0.3) sensitivityLevel = "medium";
        else sensitivityLevel = "low";
        
        sensitivity["sensitivityLevel"] = sensitivityLevel;
        
        // Visualize data points
        json dataPoints = json::array();
        for (size_t i = 0; i < paramValues.size(); ++i) {
            json point;
            point["parameter"] = paramValues[i];
            point["reward"] = rewards[i];
            dataPoints.push_back(point);
        }
        
        sensitivity["dataPoints"] = dataPoints;
    }
    
    return sensitivity;
}

AdvancedExperimentManager::ExperimentStats AdvancedExperimentManager::calculateStats(
    const std::vector<Phase6IntegrationResult>& results) {
    
    ExperimentStats stats;
    
    if (results.empty()) {
        return stats;
    }
    
    // Calculate means
    stats.meanTotalReward = calculateMean(results, [](const Phase6IntegrationResult& r) { return r.totalReward; });
    stats.meanFiringRate = calculateMean(results, [](const Phase6IntegrationResult& r) { return r.avgFiringRate; });
    stats.meanWallClockTime = calculateMean(results, [](const Phase6IntegrationResult& r) { return r.totalWallClockTime; });
    
    // Calculate standard deviations
    stats.meanTotalReward = calculateMean(results, [](const Phase6IntegrationResult& r) { return r.totalReward; });
    stats.meanFiringRate = calculateMean(results, [](const Phase6IntegrationResult& r) { return r.avgFiringRate; });
    stats.meanWallClockTime = calculateMean(results, [](const Phase6IntegrationResult& r) { return r.totalWallClockTime; });
    
    // Collect individual values
    for (const auto& result : results) {
        stats.firingRates.push_back(result.avgFiringRate);
        stats.totalRewards.push_back(result.totalReward);
        stats.wallClockTimes.push_back(result.totalWallClockTime);
    }
    
    return stats;
}

bool AdvancedExperimentManager::saveConfigurations(
    const std::vector<Phase6Config>& configs,
    const std::string& filename) {
    
    json configData;
    
    for (size_t i = 0; i < configs.size(); ++i) {
        json config;
        config["id"] = i;
        config["neuronCount"] = configs[i].neuronCount;
        config["maxSteps"] = configs[i].maxSteps;
        config["regionCount"] = configs[i].regionCount;
        config["connectionProbability"] = configs[i].connectionProbability;
        config["enableCheckpointing"] = configs[i].enableCheckpointing;
        config["enableReplay"] = configs[i].enableReplay;
        config["enableDevelopment"] = configs[i].enableDevelopment;
        config["checkpointPath"] = configs[i].checkpointPath;
        
        configData["configurations"].push_back(config);
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << configData.dump(2);
    file.close();
    
    std::cout << "Saved configurations to: " << filename << std::endl;
    return true;
}

bool AdvancedExperimentManager::loadConfigurations(
    const std::string& filename,
    std::vector<Phase6Config>& configs) {
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    json configData;
    file >> configData;
    file.close();
    
    configs.clear();
    
    for (const auto& configJson : configData["configurations"]) {
        Phase6Config config;
        config.neuronCount = configJson["neuronCount"].get<size_t>();
        config.maxSteps = configJson["maxSteps"].get<uint64_t>();
        config.regionCount = configJson["regionCount"].get<size_t>();
        config.connectionProbability = configJson["connectionProbability"].get<float>();
        config.enableCheckpointing = configJson["enableCheckpointing"].get<bool>();
        config.enableReplay = configJson["enableReplay"].get<bool>();
        config.enableDevelopment = configJson["enableDevelopment"].get<bool>();
        config.checkpointPath = configJson["checkpointPath"].get<std::string>();
        
        configs.push_back(config);
    }
    
    std::cout << "Loaded configurations from: " << filename << " (" << configs.size() << " configurations)" << std::endl;
    return true;
}

std::vector<Phase6Config> AdvancedExperimentManager::createParameterGrid(
    const std::map<std::string, std::vector<double>>& parameterRanges) {
    
    std::vector<Phase6Config> configs;
    
    // Simple parameter grid - expand all combinations of parameter values
    // For demonstration, we'll create a simple grid
    std::vector<double> neuronCounts = {500, 1000, 2000};
    std::vector<double> steps = {1000, 5000, 10000};
    std::vector<double> connectionProbs = {0.05, 0.1, 0.2};
    
    for (double neurons : neuronCounts) {
        for (double step : steps) {
            for (double connProb : connectionProbs) {
                Phase6Config config;
                config.neuronCount = static_cast<size_t>(neurons);
                config.maxSteps = static_cast<uint64_t>(step);
                config.regionCount = 1;
                config.connectionProbability = static_cast<float>(connProb);
                config.enableCheckpointing = true;
                config.enableReplay = true;
                config.enableDevelopment = true;
                config.checkpointPath = "grid_checkpoint.bin";
                
                configs.push_back(config);
            }
        }
    }
    
    std::cout << "Created parameter grid with " << configs.size() << " configurations" << std::endl;
    return configs;
}

void AdvancedExperimentManager::runBatchExperiments(
    const std::vector<Phase6Config>& configs,
    const std::string& outputDir,
    bool saveResults) {
    
    std::cout << "Running batch experiments: " << configs.size() << " configurations" << std::endl;
    
    Phase6IntegratedExperiment experiment;
    std::vector<Phase6IntegrationResult> results;
    
    for (size_t i = 0; i < configs.size(); ++i) {
        std::cout << "  Running batch experiment " << i + 1 << "/" << configs.size() << std::endl;
        
        Phase6Config config = configs[i];
        config.checkpointPath = outputDir + "/batch_checkpoint_" + std::to_string(i) + ".bin";
        
        Phase6IntegrationResult result = experiment.run(config);
        results.push_back(result);
        
        if (saveResults) {
            std::string resultFile = outputDir + "/batch_result_" + std::to_string(i) + ".json";
            std::ofstream file(resultFile);
            if (file.is_open()) {
                json resultJson;
                resultJson["experiment_id"] = i;
                resultJson["result"] = {
                    {"totalReward", result.totalReward},
                    {"avgFiringRate", result.avgFiringRate},
                    {"memoryEpisodesStored", result.memoryEpisodesStored},
                    {"noveltyLevel", result.noveltyLevel},
                    {"curiosityLevel", result.curiosityLevel},
                    {"dopamineLevel", result.dopamineLevel}
                };
                file << resultJson.dump(2);
                file.close();
            }
        }
    }
    
    std::cout << "Batch experiments completed. Results: " << results.size() << " experiments." << std::endl;
}

// Helper functions

std::string AdvancedExperimentManager::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

double AdvancedExperimentManager::calculateMean(const std::vector<Phase6IntegrationResult>& results,
                                               const std::function<double(const Phase6IntegrationResult&)>& extractor) {
    if (results.empty()) return 0.0;
    
    double sum = 0.0;
    for (const auto& result : results) {
        sum += extractor(result);
    }
    return sum / results.size();
}

double AdvancedExperimentManager::calculateStdDev(const std::vector<double>& values, double mean) {
    if (values.empty()) return 0.0;
    
    double sum = 0.0;
    for (double value : values) {
        sum += (value - mean) * (value - mean);
    }
    return std::sqrt(sum / values.size());
}

double AdvancedExperimentManager::calculateMedian(std::vector<double> values) {
    if (values.empty()) return 0.0;
    
    std::sort(values.begin(), values.end());
    size_t n = values.size();
    if (n % 2 == 0) {
        return (values[n/2 - 1] + values[n/2]) / 2.0;
    } else {
        return values[n/2];
    }
}
double AdvancedExperimentManager::calculateSkewness(const std::vector<double>& values) {
    if (values.size() < 3) return 0.0;
    
    double mean = calculateMean(values);
    double stddev = calculateStdDev(values, mean);
    
    if (stddev < 1e-10) return 0.0;
    
    double skewness = 0.0;
    for (double value : values) {
        skewness += std::pow((value - mean) / stddev, 3);
    }
    return skewness / values.size();
}
double AdvancedExperimentManager::calculateKurtosis(const std::vector<double>& values) {
    if (values.size() < 4) return 0.0;
    
    double mean = calculateMean(values);
    double stddev = calculateStdDev(values, mean);
    
    if (stddev < 1e-10) return 0.0;
    
    double kurtosis = 0.0;
    for (double value : values) {
        kurtosis += std::pow((value - mean) / stddev, 4);
    }
    kurtosis = kurtosis / values.size() - 3.0;
    return kurtosis;
}
double AdvancedExperimentManager::calculateSuccessRate(const std::vector<bool>& successes) {
    if (successes.empty()) return 0.0;
    
    double successCount = std::accumulate(successes.begin(), successes.end(), 0.0);
    return successCount / successes.size();
}
double AdvancedExperimentManager::calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y) {
    if (x.size() != y.size() || x.size() < 2) return 0.0;
    
    double x_mean = calculateMean(x);
    double y_mean = calculateMean(y);
    
    double numerator = 0.0, x_denom = 0.0, y_denom = 0.0;
    
    for (size_t i = 0; i < x.size(); ++i) {
        double x_diff = x[i] - x_mean;
        double y_diff = y[i] - y_mean;
        numerator += x_diff * y_diff;
        x_denom += x_diff * x_diff;
        y_denom += y_diff * y_diff;
    }
    
    if (x_denom == 0.0 || y_denom == 0.0) return 0.0;
    
    return numerator / std::sqrt(x_denom * y_denom);
}
json AdvancedExperimentManager::calculateCorrelations(
    const std::vector<double>& x,
    const std::vector<double>& y,
    const std::vector<double>& z) {
    
    json correlations;
    
    if (!x.empty() && !y.empty()) {
        double xy_corr = calculateCorrelation(x, y);
        correlations["x_y"] = xy_corr;
    }
    
    if (!x.empty() && !z.empty()) {
        double xz_corr = calculateCorrelation(x, z);
        correlations["x_z"] = xz_corr;
    }
    
    if (!y.empty() && !z.empty()) {
        double yz_corr = calculateCorrelation(y, z);
        correlations["y_z"] = yz_corr;
    }
    
    return correlations;
}
double AdvancedExperimentManager::extractParameterValue(const Phase6Config& config, const std::string& parameter) {
    if (parameter == "neuronCount") return static_cast<double>(config.neuronCount);
    if (parameter == "maxSteps") return static_cast<double>(config.maxSteps);
    if (parameter == "regionCount") return static_cast<double>(config.regionCount);
    if (parameter == "connectionProbability") return static_cast<double>(config.connectionProbability);
    if (parameter == "enableCheckpointing") return config.enableCheckpointing ? 1.0 : 0.0;
    if (parameter == "enableReplay") return config.enableReplay ? 1.0 : 0.0;
    if (parameter == "enableDevelopment") return config.enableDevelopment ? 1.0 : 0.0;
    
    return 0.0;
}
json AdvancedExperimentManager::calculateDescriptiveStats(const std::vector<double>& values) {
    json stats;
    
    if (values.empty()) {
        return stats;
    }
    
    double mean = calculateMean(values);
    double stddev = calculateStdDev(values, mean);
    double min = *std::min_element(values.begin(), values.end());
    double max = *std::max_element(values.begin(), values.end());
    double median = calculateMedian(values);
    double skewness = calculateSkewness(values);
    double kurtosis = calculateKurtosis(values);
    
    stats["mean"] = mean;
    stats["stddev"] = stddev;
    stats["min"] = min;
    stats["max"] = max;
    stats["median"] = median;
    stats["skewness"] = skewness;
    stats["kurtosis"] = kurtosis;
    stats["values"] = values;
    
    return stats;
}

} // namespace nlm
