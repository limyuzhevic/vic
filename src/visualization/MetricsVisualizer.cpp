#include "SimulationResultsVisualizers.hpp"

namespace nlm {

struct MetricsVisualizer::Impl {
    std::unordered_map<std::string, std::deque<std::pair<double, double>>> metricHistory;
    std::unordered_map<std::string, std::string> metricUnits;
    std::unordered_map<std::string, double> currentMetrics;
    std::unordered_map<std::string, double> alertThresholds;
    std::unordered_map<std::string, int> alertLevels;
    double currentTime;
    double timeWindow;
    bool showAlerts;
    bool showHistory;
    
    Impl() : currentTime(0.0), timeWindow(60.0), showAlerts(true), showHistory(true) {}
};

MetricsVisualizer::MetricsVisualizer() : pImpl(std::make_unique<Impl>()) {}

MetricsVisualizer::~MetricsVisualizer() = default;

MetricsVisualizer::MetricsVisualizer(MetricsVisualizer&&) noexcept = default;
MetricsVisualizer& MetricsVisualizer::operator=(MetricsVisualizer&&) noexcept = default;

bool MetricsVisualizer::initialize(const VisualizationConfig& config) {
    return Visualizer::initialize(config);
}

void MetricsVisualizer::update(const VisualizationData& data) {
    auto timeSeriesData = data.getTimeSeriesData();
    
    for (const auto& [timestamp, value] : timeSeriesData) {
        // Extract metric name from value? This needs to be handled differently
        // For now, we'll just track basic metrics
        pImpl->currentMetrics["metric_" + std::to_string(timestamp)] = value;
    }
    
    pImpl->currentTime = timeSeriesData.empty() ? 0.0 : timeSeriesData.back().first;
    
    Visualizer::update(data);
}

void MetricsVisualizer::render(bool output) {
    Visualizer::render(output);
}

void MetricsVisualizer::configure(const VisualizationConfig& config) {
    Visualizer::configure(config);
}

VisualizationType MetricsVisualizer::getType() const {
    return VisualizationType::SimulationResults;
}

std::string MetricsVisualizer::getName() const {
    return "MetricsVisualizer";
}

void MetricsVisualizer::addMetric(const std::string& name, const std::string& unit) {
    pImpl->metricUnits[name] = unit;
}

void MetricsVisualizer::setMetricAlert(const std::string& metricName, double threshold, int level) {
    pImpl->alertThresholds[metricName] = threshold;
    pImpl->alertLevels[metricName] = level;
}

std::unordered_map<std::string, double> MetricsVisualizer::getCurrentMetrics() const {
    return pImpl->currentMetrics;
}

std::vector<std::pair<double, double>> MetricsVisualizer::getMetricHistory(const std::string& metricName) const {
    auto it = pImpl->metricHistory.find(metricName);
    if (it != pImpl->metricHistory.end()) {
        return std::vector<std::pair<double, double>>(it->second.begin(), it->second.end());
    }
    return {};
}

bool MetricsVisualizer::exportData(const std::string& format, const std::string& filepath) {
    // Implementation would export metrics data in specified format
    return false;
}

} // namespace nlm