#pragma once

#include <string>
#include <vector>
#include <chrono>

namespace nlm {

class AdvancedAnalysisTools {
public:
    AdvancedAnalysisTools();
    ~AdvancedAnalysisTools() = default;
    
    // Run specific analysis mode
    void runAnalysis(std::string mode, std::vector<std::string>& args);
    
    // Show all available analysis modes
    void showAnalysisModes();
    
private:
    // Analysis data structures
    struct LearningMetrics {
        float initialReward;
        float peakReward;
        float finalReward;
        float rewardTrend;
        float learningRate;
        std::vector<float> episodeRewards;
        std::vector<float> actionRatios;
    };
    
    struct SynapticMetrics {
        float meanWeight;
        float weightVariance;
        float maxWeight;
        float minWeight;
        float weightStabilization;
        std::vector<float> weightHistory;
        std::vector<float> weightChanges;
    };
    
    struct FiringMetrics {
        float meanFiringRate;
        float firingRateVariance;
        float maxFiringRate;
        float minFiringRate;
        float firingRateStability;
        std::vector<float> firingRateHistory;
        std::map<NeuronType, float> typeSpecificRates;
    };
    
    struct NetworkMetrics {
        float connectivity;
        float clusteringCoefficient;
        float pathLength;
        float smallWorldness;
        std::vector<int> degreeDistribution;
        std::map<RegionId, float> regionConnectivity;
    };
    
    // Analysis data storage
    LearningMetrics learningData;
    SynapticMetrics synapticData;
    FiringMetrics firingData;
    NetworkMetrics networkData;
    std::chrono::high_resolution_clock::time_point simulationStart;
    
    void initializeAnalysisData();
    void updateMetricsFromBrain(const class Brain& brain);
    
    // Analysis methods
    void analyzeLearningCurve(std::vector<std::string>& args);
    void analyzeSynapticDynamics(std::vector<std::string>& args);
    void analyzeFiringRates(std::vector<std::string>& args);
    void analyzeNetworkConnectivity(std::vector<std::string>& args);
    void analyzeMemorySystems(std::vector<std::string>& args);
    void analyzePlasticityChanges(std::vector<std::string>& args);
    void analyzeBehaviorPatterns(std::vector<std::string>& args);
    void exportAnalysisData(std::vector<std::string>& args);
    void runVisualization(std::vector<std::string>& args);
};

} // namespace nlm
