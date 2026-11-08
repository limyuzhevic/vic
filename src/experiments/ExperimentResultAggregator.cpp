// ExperimentResultAggregator implementation - common result handling
#include "ExperimentInfrastructure.hpp"
#include <fstream>
#include <algorithm>
#include <numeric>

namespace nlm {

struct ExperimentResultAggregator::Impl {
    std::vector<std::pair<std::string, ExperimentMetrics>> results;
    std::map<std::string, std::vector<double>> metricValues;
};

ExperimentResultAggregator::ExperimentResultAggregator() : experimentCount_(0) {
    pImpl = std::make_unique<Impl>();
}

ExperimentResultAggregator::~ExperimentResultAggregator() = default;

void ExperimentResultAggregator::addResult(const std::string& experimentName, const ExperimentMetrics& metrics) {
    pImpl->results.push_back(std::make_pair(experimentName, metrics));
    experimentCount_++;
    
    // Store metric values for aggregation
    pImpl->metricValues["episode"].push_back(metrics.episode);
    pImpl->metricValues["steps"].push_back(static_cast<double>(metrics.steps));
    pImpl->metricValues["totalReward"].push_back(metrics.totalReward);
    pImpl->metricValues["finalEnergy"].push_back(metrics.finalEnergy);
    pImpl->metricValues["finalNovelty"].push_back(metrics.finalNovelty);
    pImpl->metricValues["finalCuriosity"].push_back(metrics.finalCuriosity);
    pImpl->metricValues["predictionError"].push_back(metrics.predictionError);
    pImpl->metricValues["developmentalAge"].push_back(metrics.developmentalAge);
    pImpl->metricValues["totalSynapses"].push_back(static_cast<double>(metrics.totalSynapses));
    pImpl->metricValues["averageWeight"].push_back(metrics.averageWeight);
    pImpl->metricValues["maxWeight"].push_back(metrics.maxWeight);
    pImpl->metricValues["totalSpikes"].push_back(static_cast<double>(metrics.totalSpikes));
    pImpl->metricValues["averageFiringRate"].push_back(metrics.averageFiringRate);
}

double ExperimentResultAggregator::getAverageMetric(const std::string& metricName) const {
    auto it = pImpl->metricValues.find(metricName);
    if (it == pImpl->metricValues.end() || it->second.empty()) {
        return 0.0;
    }
    
    double sum = std::accumulate(it->second.begin(), it->second.end(), 0.0);
    return sum / it->second.size();
}

bool ExperimentResultAggregator::saveResults(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write header
    file << "ExperimentName,Episode,Steps,TotalReward,FinalEnergy,FinalNovelty,FinalCuriosity,PredictionError,DevelopmentalAge,DevelopmentalStage,TotalSynapses,AverageWeight,MaxWeight,TotalSpikes,AverageFiringRate,ForwardCount,BackwardCount,TurnLeftCount,TurnRightCount,InteractCount,WaitCount,LookLeftCount,LookRightCount\n";
    
    // Write results
    for (const auto& result : pImpl->results) {
        const auto& metrics = result.second;
        file << result.first << "," 
             << metrics.episode << "," << metrics.steps << "," << metrics.totalReward << "," << metrics.finalEnergy << "," << metrics.finalNovelty << "," << metrics.finalCuriosity << "," << metrics.predictionError << "," << metrics.developmentalAge << "," << metrics.developmentalStage << "," << metrics.totalSynapses << "," << metrics.averageWeight << "," << metrics.maxWeight << "," << metrics.totalSpikes << "," << metrics.averageFiringRate << "," << metrics.forwardCount << "," << metrics.backwardCount << "," << metrics.turnLeftCount << "," << metrics.turnRightCount << "," << metrics.interactCount << "," << metrics.waitCount << "," << metrics.lookLeftCount << "," << metrics.lookRightCount << "\n";
    }
    
    file.close();
    return true;
}

std::string ExperimentResultAggregator::getResultsAsCSV() const {
    std::ostringstream oss;
    oss << "ExperimentName,Episode,Steps,TotalReward,FinalEnergy,FinalNovelty,FinalCuriosity,PredictionError,DevelopmentalAge,DevelopmentalStage,TotalSynapses,AverageWeight,MaxWeight,TotalSpikes,AverageFiringRate,ForwardCount,BackwardCount,TurnLeftCount,TurnRightCount,InteractCount,WaitCount,LookLeftCount,LookRightCount\n";
    
    for (const auto& result : pImpl->results) {
        const auto& metrics = result.second;
        oss << result.first << "," 
            << metrics.episode << "," << metrics.steps << "," << metrics.totalReward << ",\""" << metrics.finalEnergy << "\",\""" << metrics.finalNovelty << "\",\""" << metrics.finalCuriosity << "\",\""" << metrics.predictionError << "\",\""" << metrics.developmentalAge << "\",\""" << metrics.developmentalStage << "\",\""" << metrics.totalSynapses << "\",\""" << metrics.averageWeight << "\",\""" << metrics.maxWeight << "\",\""" << metrics.totalSpikes << "\",\""" << metrics.averageFiringRate << "\",\""" << metrics.forwardCount << "\",\""" << metrics.backwardCount << "\",\""" << metrics.turnLeftCount << "\",\""" << metrics.turnRightCount << "\",\""" << metrics.interactCount << "\",\""" << metrics.waitCount << "\",\""" << metrics.lookLeftCount << "\",\""" << metrics.lookRightCount << "\"" << "\n";
    }
    
    return oss.str();
}

} // namespace nlm
