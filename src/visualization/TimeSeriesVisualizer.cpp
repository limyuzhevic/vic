#include "SimulationResultsVisualizers.hpp"

namespace nlm {

struct TimeSeriesVisualizer::Impl {
    std::unordered_map<std::string, std::deque<std::pair<double, double>>> seriesData;
    std::unordered_map<std::string, std::string> seriesUnits;
    double startTime;
    double endTime;
    double timeWindow;
    bool showGrid;
    bool showAnnotations;
    bool showStatistics;
    
    Impl() : startTime(0.0), endTime(100.0), timeWindow(60.0), showGrid(true), 
             showAnnotations(true), showStatistics(true) {}
};

TimeSeriesVisualizer::TimeSeriesVisualizer() : pImpl(std::make_unique<Impl>()) {}

TimeSeriesVisualizer::~TimeSeriesVisualizer() = default;

TimeSeriesVisualizer::TimeSeriesVisualizer(TimeSeriesVisualizer&&) noexcept = default;
TimeSeriesVisualizer& TimeSeriesVisualizer::operator=(TimeSeriesVisualizer&&) noexcept = default;

bool TimeSeriesVisualizer::initialize(const VisualizationConfig& config) {
    return Visualizer::initialize(config);
}

void TimeSeriesVisualizer::update(const VisualizationData& data) {
    auto timeSeriesData = data.getTimeSeriesData();
    
    // For now, we'll store all time series data
    // In a real implementation, we'd need to associate series with specific metrics
    for (const auto& [timestamp, value] : timeSeriesData) {
        pImpl->seriesData["default_series"].emplace_back(timestamp, value);
    }
    
    pImpl->startTime = timeSeriesData.empty() ? 0.0 : timeSeriesData.front().first;
    pImpl->endTime = timeSeriesData.empty() ? 0.0 : timeSeriesData.back().first;
    
    Visualizer::update(data);
}

void TimeSeriesVisualizer::render(bool output) {
    Visualizer::render(output);
}

void TimeSeriesVisualizer::configure(const VisualizationConfig& config) {
    Visualizer::configure(config);
}

VisualizationType TimeSeriesVisualizer::getType() const {
    return VisualizationType::SimulationResults;
}

std::string TimeSeriesVisualizer::getName() const {
    return "TimeSeriesVisualizer";
}

void TimeSeriesVisualizer::addTimeSeries(const std::string& name, const std::string& unit) {
    pImpl->seriesUnits[name] = unit;
}

void TimeSeriesVisualizer::setTimeWindow(double startTime, double endTime) {
    pImpl->startTime = startTime;
    pImpl->endTime = endTime;
    pImpl->timeWindow = endTime - startTime;
}

std::pair<double, double> TimeSeriesVisualizer::getTimeWindow() const {
    return {pImpl->startTime, pImpl->endTime};
}

bool TimeSeriesVisualizer::exportData(const std::string& format, const std::string& filepath) {
    // Implementation would export time series data in specified format
    return false;
}

} // namespace nlm