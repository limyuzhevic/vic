// Configuration Comparison Tool
// Allows running multiple experimental configurations and comparing results

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>

namespace nlm {

class ConfigComparator {
public:
    struct ComparisonResult {
        std::string configName;
        Phase6IntegrationResult experimentResult;
        float performanceScore;
        float stabilityScore;
        std::vector<float> metricHistory;
    };
    
    ConfigComparator() = default;
    ~ConfigComparator() = default;
    
    void addConfig(const std::string& name, const Phase6Config& config) {
        configs_.push_back({name, config});
        std::cout << "Added config: " << name << std::endl;
    }
    
    std::vector<ComparisonResult> runComparison() {
        std::cout << "Running configuration comparison...\n";
        std::cout << "Comparing " << configs_.size() << " configurations...\n";
        
        std::vector<ComparisonResult> results;
        Phase6IntegratedExperiment experiment;
        
        for (const auto& configPair : configs_) {
            std::cout << "\n--- Testing config: " << configPair.name << " ---\n";
            
            ComparisonResult result;
            result.configName = configPair.name;
            
            // Run the experiment
            auto experimentResult = experiment.run(configPair.config);
            result.experimentResult = experimentResult;
            
            // Calculate performance score
            result.performanceScore = calculatePerformanceScore(experimentResult);
            
            // Calculate stability score
            result.stabilityScore = calculateStabilityScore(experimentResult);
            
            // Collect metric history (simplified)
            result.metricHistory = collectMetricHistory(experimentResult);
            
            results.push_back(result);
            
            // Print results
            printResultSummary(result);
        }
        
        // Generate comparison report
        printComparisonReport(results);
        
        return results;
    }
    
    void printComparisonReport(const std::vector<ComparisonResult>& results) {
        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << "CONFIGURATION COMPARISON REPORT" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        // Header
        std::cout << std::left << std::setw(20) << "Config Name";
        std::cout << std::left << std::setw(12) << "Performance";
        std::cout << std::left << std::setw(12) << "Stability";
        std::cout << std::left << std::setw(15) << "Total Reward";
        std::cout << std::left << std::setw(15) << "Firing Rate";
        std::cout << std::left << std::setw(15) << "Episodes";
        std::cout << std::left << std::setw(15) << "Wall Time";
        std::cout << std::endl;
        
        std::cout << std::string(80, '-') << std::endl;
        
        // Data rows
        for (const auto& result : results) {
            std::cout << std::left << std::setw(20) << result.configName;
            std::cout << std::left << std::setw(12) << std::fixed << std::setprecision(2) << result.performanceScore;
            std::cout << std::left << std::setw(12) << std::fixed << std::setprecision(2) << result.stabilityScore;
            std::cout << std::left << std::setw(15) << std::fixed << std::setprecision(3) << result.experimentResult.totalReward;
            std::cout << std::left << std::setw(15) << std::fixed << std::setprecision(2) << result.experimentResult.avgFiringRate;
            std::cout << std::left << std::setw(15) << std::fixed << std::setprecision(0) << result.experimentResult.memoryEpisodesStored;
            std::cout << std::left << std::setw(15) << std::fixed << std::setprecision(2) << result.experimentResult.totalWallClockTime;
            std::cout << std::endl;
        }
        
        // Find best configuration
        if (!results.empty()) {
            auto best = std::max_element(results.begin(), results.end(),
                [](const ComparisonResult& a, const ComparisonResult& b) {
                    return a.performanceScore < b.performanceScore;
                });
            
            std::cout << "\nBest performing configuration: " << best->configName << std::endl;
            std::cout << "Performance score: " << best->performanceScore << std::endl;
        }
        
        // Statistical analysis
        printStatisticalAnalysis(results);
    }
    
    void printStatisticalAnalysis(const std::vector<ComparisonResult>& results) {
        if (results.size() < 2) return;
        
        std::cout << std::string(80, '=') << std::endl;
        std::cout << "STATISTICAL ANALYSIS" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        std::vector<float> performanceScores;
        std::vector<float> stabilityScores;
        std::vector<float> rewards;
        
        for (const auto& result : results) {
            performanceScores.push_back(result.performanceScore);
            stabilityScores.push_back(result.stabilityScore);
            rewards.push_back(result.experimentResult.totalReward);
        }
        
        // Calculate statistics
        auto calcStats = [](const std::vector<float>& values) {
            float sum = std::accumulate(values.begin(), values.end(), 0.0f);
            float mean = sum / values.size();
            float variance = 0.0f;
            for (float val : values) {
                variance += (val - mean) * (val - mean);
            }
            variance /= values.size();
            float stddev = std::sqrt(variance);
            
            return std::make_tuple(mean, stddev);
        };
        
        auto [perfMean, perfStddev] = calcStats(performanceScores);
        auto [stabMean, stabStddev] = calcStats(stabilityScores);
        auto [rewMean, rewStddev] = calcStats(rewards);
        
        std::cout << "Performance Score: Mean = " << perfMean << ", Std Dev = " << perfStddev << std::endl;
        std::cout << "Stability Score: Mean = " << stabMean << ", Std Dev = " << stabStddev << std::endl;
        std::cout << "Total Reward: Mean = " << rewMean << ", Std Dev = " << rewStddev << std::endl;
        
        // Coefficient of variation
        float perfCV = (perfMean > 0) ? (perfStddev / perfMean) * 100 : 0;
        float stabCV = (stabMean > 0) ? (stabStddev / stabMean) * 100 : 0;
        
        std::cout << "\nCoefficient of Variation:";
        std::cout << "\n  Performance: " << perfCV << "% (lower is better)";
        std::cout << "\n  Stability: " << stabCV << "% (lower is better)";
    }
    
private:
    struct NamedConfig {
        std::string name;
        Phase6Config config;
    };
    
    std::vector<NamedConfig> configs_;
    
    float calculatePerformanceScore(const Phase6IntegrationResult& result) {
        // Weighted score combining multiple metrics
        float rewardScore = result.totalReward / 10.0f;  // Normalize
        float firingScore = result.avgFiringRate / 100.0f;  // Normalize
        float memoryScore = result.memoryEpisodesStored / 1000.0f;  // Normalize
        float stabilityScore = result.checkpointingWorks ? 1.0f : 0.5f;
        
        // Weighted average (adjust weights as needed)
        return (rewardScore * 0.4f + firingScore * 0.2f + memoryScore * 0.2f + stabilityScore * 0.2f);
    }
    
    float calculateStabilityScore(const Phase6IntegrationResult& result) {
        // Simple stability metric based on checkpointing and replay
        float stability = 0.5f;  // Base stability
        
        if (result.checkpointingWorks) stability += 0.3f;
        if (result.replayWorks) stability += 0.2f;
        
        return std::min(stability, 1.0f);  // Cap at 1.0
    }
    
    std::vector<float> collectMetricHistory(const Phase6IntegrationResult& result) {
        // Simplified metric history collection
        std::vector<float> history;
        
        // Add various metrics
        history.push_back(result.totalReward);
        history.push_back(result.avgFiringRate);
        history.push_back(result.memoryEpisodesStored);
        history.push_back(result.dopamineLevel);
        history.push_back(result.curiosityLevel);
        history.push_back(result.noveltyLevel);
        
        return history;
    }
    
    void printResultSummary(const ComparisonResult& result) {
        std::cout << "Configuration: " << result.configName << std::endl;
        std::cout << "  Performance Score: " << result.performanceScore << std::endl;
        std::cout << "  Stability Score: " << result.stabilityScore << std::endl;
        std::cout << "  Total Reward: " << result.experimentResult.totalReward << std::endl;
        std::cout << "  Average Firing Rate: " << result.experimentResult.avgFiringRate << std::endl;
        std::cout << "  Episodes Stored: " << result.experimentResult.memoryEpisodesStored << std::endl;
        std::cout << "  Checkpointing Works: " << (result.experimentResult.checkpointingWorks ? "Yes" : "No") << std::endl;
        std::cout << "  Replay Works: " << (result.experimentResult.replayWorks ? "Yes" : "No") << std::endl;
        std::cout << "  Wall Clock Time: " << result.experimentResult.totalWallClockTime << "s" << std::endl;
        std::cout << std::endl;
    }
};

} // namespace nlm

// Simple command-line interface for the comparator
int main(int argc, char* argv[]) {
    nlm::ConfigComparator comparator;
    
    // Add some example configurations
    Phase6Config basicConfig;
    basicConfig.neuronCount = 500;
    basicConfig.maxSteps = 2000;
    basicConfig.enableCheckpointing = true;
    basicConfig.enableReplay = true;
    basicConfig.enableDevelopment = true;
    comparator.addConfig("Basic", basicConfig);
    
    Phase6Config optimizedConfig = basicConfig;
    optimizedConfig.neuronCount = 1000;
    optimizedConfig.connectionProbability = 0.15f;
    comparator.addConfig("Optimized", optimizedConfig);
    
    Phase6Config lightweightConfig = basicConfig;
    lightweightConfig.neuronCount = 200;
    lightweightConfig.maxSteps = 1000;
    lightweightConfig.enableCheckpointing = false;
    comparator.addConfig("Lightweight", lightweightConfig);
    
    Phase6Config researchConfig = basicConfig;
    researchConfig.neuronCount = 1500;
    researchConfig.maxSteps = 5000;
    researchConfig.enableCheckpointing = true;
    researchConfig.enableReplay = true;
    researchConfig.enableDevelopment = true;
    comparator.addConfig("Research", researchConfig);
    
    // Run comparison
    auto results = comparator.runComparison();
    
    return 0;
}