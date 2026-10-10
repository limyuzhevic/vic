#include "SimulationResultsVisualizers.hpp"

namespace nlm {

struct ResultsVisualizer::Impl {
    std::vector<std::tuple<std::string, std::string, double, double>> results;
    std::string resultGroup;
    bool showStatisticalAnalysis;
    bool showErrorBars;
    bool showEffectSizes;
    bool showConfidenceIntervals;
    
    Impl() : showStatisticalAnalysis(true), showErrorBars(true), 
             showEffectSizes(true), showConfidenceIntervals(true) {}
};

ResultsVisualizer::ResultsVisualizer() : pImpl(std::make_unique<Impl>()) {}

ResultsVisualizer::~ResultsVisualizer() = default;

ResultsVisualizer::ResultsVisualizer(ResultsVisualizer&&) noexcept = default;
ResultsVisualizer& ResultsVisualizer::operator=(ResultsVisualizer&&) noexcept = default;

bool ResultsVisualizer::initialize(const VisualizationConfig& config) {
    return Visualizer::initialize(config);
}

void ResultsVisualizer::update(const VisualizationData& data) {
    Visualizer::update(data);
}

void ResultsVisualizer::render(bool output) {
    Visualizer::render(output);
}

void ResultsVisualizer::configure(const VisualizationConfig& config) {
    Visualizer::configure(config);
}

VisualizationType ResultsVisualizer::getType() const {
    return VisualizationType::SimulationResults;
}

std::string ResultsVisualizer::getName() const {
    return "ResultsVisualizer";
}

void ResultsVisualizer::addResult(const std::string& resultName, const std::string& metricName,
                                 double value, double error) {
    pImpl->results.emplace_back(resultName, metricName, value, error);
}

void ResultsVisualizer::setResultGroup(const std::string& groupName) {
    pImpl->resultGroup = groupName;
}

std::unordered_map<std::string, double> ResultsVisualizer::getResultsStatistics() const {
    std::unordered_map<std::string, double> stats;
    
    if (pImpl->results.empty()) {
        return stats;
    }
    
    // Calculate basic statistics
    std::vector<double> values;
    for (const auto& result : pImpl->results) {
        values.push_back(std::get<2>(result));
    }
    
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    stats["mean"] = sum / values.size();
    
    double variance = 0.0;
    for (double value : values) {
        double diff = value - stats["mean"];
        variance += diff * diff;
    }
    stats["std_dev"] = std::sqrt(variance / values.size());
    
    stats["min"] = *std::min_element(values.begin(), values.end());
    stats["max"] = *std::max_element(values.begin(), values.end());
    stats["count"] = static_cast<double>(values.size());
    
    return stats;
}

bool ResultsVisualizer::exportData(const std::string& format, const std::string& filepath) {
    // Implementation would export results data in specified format
    return false;
}

} // namespace nlm