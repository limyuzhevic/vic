#ifndef NLM_STATISTICS_HPP
#define NLM_STATISTICS_HPP

#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include "brain/Brain.hpp"

namespace nlm {

class SimulationStatistics {
public:
    struct PerformanceMetrics {
        double totalSimulationTime;  // seconds
        size_t totalStepsExecuted;
        size_t totalSpikeCount;
        size_t activeNeuronCount;
        double averageFiringRate;
        double stepsPerSecond;
        double spikesPerSecond;
        double efficiencyScore;  // normalized activity measure
    };

    struct LearningMetrics {
        bool learningDetected;
        float initialSynapseWeightMean;
        float finalSynapseWeightMean;
        float weightChangeMagnitude;
        size_t strengthenedConnections;
        size_t weakenedConnections;
        size_t stableConnections;
        float synapticPlasticityLevel;
    };

    struct SystemMetrics {
        size_t neuronCount;
        size_t synapseCount;
        size_t regionCount;
        double memoryUsage;  // MB (estimated)
        std::chrono::system_clock::time_point simulationStart;
        std::chrono::system_clock::time_point simulationEnd;
    };

    SimulationStatistics() = default;
    explicit SimulationStatistics(std::shared_ptr<Brain> brain);

    PerformanceMetrics calculatePerformanceMetrics() const;
    LearningMetrics calculateLearningMetrics() const;
    SystemMetrics calculateSystemMetrics() const;

    std::string generateReport(const std::string& reportType = "full") const;
    bool saveReport(const std::string& filepath, const std::string& reportType = "full") const;

    static std::string getVersionInfo();
    static std::string getSystemInfo();

private:
    std::shared_ptr<Brain> brain;
    mutable std::vector<float> initialWeights;
    mutable std::vector<float> finalWeights;

    float calculateStandardDeviation(const std::vector<float>& values) const;
    float calculateMean(const std::vector<float>& values) const;
    double calculateEfficiencyScore(size_t spikeCount, size_t neuronCount, double duration) const;
    std::string formatTime(double seconds) const;
    std::string formatNumber(size_t number) const;
};

} // namespace nlm

#endif // NLM_STATISTICS_HPP