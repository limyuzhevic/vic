#include "SimulationVisualizer.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>

namespace nlm {

struct SimulationVisualizer::Impl {
    std::shared_ptr<SimulationClock> simulationClock;
    
    // Time series data
    std::unordered_map<std::string, std::deque<TimeSeriesPoint>> timeSeriesData;
    size_t maxTimeSeriesPoints = 1000;
    
    // Spatial visualization
    std::unordered_map<RegionId, VisualizationPosition> spatialPositions;
    
    // Simulation state tracking
    std::deque<TimeSeriesPoint> stepHistory;
    std::deque<TimeSeriesPoint> spikeHistory;
    std::deque<TimeSeriesPoint> firingRateHistory;
    
    // Visualization parameters
    bool show3D = true;
    bool showConnections = true;
    float pointSize = 1.0f;
    float connectionStrength = 0.5f;
    
    Impl() {}
};

SimulationVisualizer::SimulationVisualizer() : pImpl(new Impl) {}

SimulationVisualizer::~SimulationVisualizer() = default;

bool SimulationVisualizer::initialize() {
    return true;
}

void SimulationVisualizer::update(double deltaTime) {
    if (!pImpl->simulationClock) return;
    
    SimulationStep currentStep = pImpl->simulationClock->getCurrentStep();
    Timestamp currentTime = pImpl->simulationClock->getCurrentTime();
    
    // Record simulation state
    TimeSeriesPoint stepPoint(currentStep, currentTime, static_cast<float>(currentStep));
    pImpl->stepHistory.push_back(stepPoint);
    if (pImpl->stepHistory.size() > pImpl->maxTimeSeriesPoints) {
        pImpl->stepHistory.pop_front();
    }
    
    // Update time series data
    for (auto& pair : pImpl->timeSeriesData) {
        TimeSeriesPoint point(currentStep, currentTime, pair.second.back().value);
        pair.second.push_back(point);
        if (pair.second.size() > pImpl->maxTimeSeriesPoints) {
            pair.second.pop_front();
        }
    }
}

void SimulationVisualizer::render() {
    std::cout << "=== Simulation Visualization ===" << std::endl;
    
    if (!pImpl->simulationClock) {
        std::cout << "No simulation clock set!" << std::endl;
        return;
    }
    
    auto clock = pImpl->simulationClock;
    
    std::cout << "Simulation State:" << std::endl;
    std::cout << "  Step: " << clock->getCurrentStep() << std::endl;
    std::cout << "  Time: " << clock->getCurrentTime() << "s" << std::endl;
    std::cout << "  Simulation Time: " << clock->getSimulationTime() << "s" << std::endl;
    std::cout << "  Time Step: " << clock->getTimeStep() << "s" << std::endl;
    std::cout << "  Step History Points: " << pImpl->stepHistory.size() << std::endl;
    
    // Generate spatial layout visualization
    std::string spatialLayout = generateSpatialLayout();
    std::cout << std::endl << "Spatial Layout:" << std::endl;
    std::cout << spatialLayout << std::endl;
    
    // Generate time series visualization
    std::cout << std::endl << "Time Series Data:" << std::endl;
    for (const auto& pair : pImpl->timeSeriesData) {
        std::string seriesViz = generateTimeSeriesVisualization(pair.first);
        std::cout << "  " << pair.first << ":" << std::endl;
        std::cout << seriesViz << std::endl;
    }
}

void SimulationVisualizer::close() {
    pImpl->timeSeriesData.clear();
    pImpl->stepHistory.clear();
    pImpl->spikeHistory.clear();
    pImpl->firingRateHistory.clear();
    pImpl->spatialPositions.clear();
}

void SimulationVisualizer::setSimulationClock(std::shared_ptr<SimulationClock> clock) {
    pImpl->simulationClock = clock;
}

void SimulationVisualizer::setSpatialGrid(const std::unordered_map<RegionId, VisualizationPosition>& positions) {
    pImpl->spatialPositions = positions;
}

void SimulationVisualizer::addTimeSeriesData(const std::string& name, const TimeSeriesPoint& point) {
    pImpl->timeSeriesData[name].push_back(point);
    if (pImpl->timeSeriesData[name].size() > pImpl->maxTimeSeriesPoints) {
        pImpl->timeSeriesData[name].pop_front();
    }
}

void SimulationVisualizer::setTimeSeriesLimit(size_t limit) {
    pImpl->maxTimeSeriesPoints = limit;
    
    // Trim existing data
    for (auto& pair : pImpl->timeSeriesData) {
        while (pair.second.size() > limit) {
            pair.second.pop_front();
        }
    }
    while (pImpl->stepHistory.size() > limit) {
        pImpl->stepHistory.pop_front();
    }
}

const std::unordered_map<std::string, std::deque<TimeSeriesPoint>>& SimulationVisualizer::getTimeSeriesData() const {
    return pImpl->timeSeriesData;
}

const std::unordered_map<RegionId, VisualizationPosition>& SimulationVisualizer::getSpatialPositions() const {
    return pImpl->spatialPositions;
}

std::string SimulationVisualizer::generateSpatialLayout() const {
    std::stringstream ss;
    
    if (pImpl->spatialPositions.empty()) {
        return "  No spatial positions defined";
    }
    
    // Simple 2D projection for display
    ss << "  Regions:" << std::endl;
    for (const auto& pair : pImpl->spatialPositions) {
        const auto& pos = pair.second;
        ss << "    Region " << pair.first << ": (" 
           << std::fixed << std::setprecision(2) << pos.x << ", " 
           << pos.y << ", " << pos.z << ")" << std::endl;
    }
    
    if (pImpl->showConnections && pImpl->spatialPositions.size() > 1) {
        ss << "  Connections (nearest neighbors):" << std::endl;
        std::vector<RegionId> regionIds;
        for (const auto& pair : pImpl->spatialPositions) {
            regionIds.push_back(pair.first);
        }
        
        for (size_t i = 0; i < regionIds.size(); ++i) {
            for (size_t j = i + 1; j < regionIds.size(); ++j) {
                const auto& pos1 = pImpl->spatialPositions.at(regionIds[i]);
                const auto& pos2 = pImpl->spatialPositions.at(regionIds[j]);
                float distance = pos1.getDistance(pos2);
                
                if (distance < 10.0f) { // Arbitrary connection threshold
                    ss << "    " << regionIds[i] << " -> " << regionIds[j] << 
                       ": distance=" << std::fixed << std::setprecision(2) << distance << 
                       ", strength=" << (1.0f - distance / 10.0f) * pImpl->connectionStrength << std::endl;
                }
            }
        }
    }
    
    return ss.str();
}

std::string SimulationVisualizer::generateTimeSeriesVisualization(const std::string& seriesName) const {
    std::stringstream ss;
    
    auto it = pImpl->timeSeriesData.find(seriesName);
    if (it == pImpl->timeSeriesData.end() || it->second.empty()) {
        return "    No data available";
    }
    
    const auto& series = it->second;
    
    // Generate simple ASCII line chart
    ss << "    Chart (last " << std::min(size_t(20), series.size()) << " points):" << std::endl;
    
    std::vector<float> values;
    for (const auto& point : series) {
        values.push_back(point.value);
    }
    
    if (values.empty()) {
        return "    No values in series";
    }
    
    float minVal = *std::min_element(values.begin(), values.end());
    float maxVal = *std::max_element(values.begin(), values.end());
    float range = maxVal - minVal;
    if (range == 0) range = 1.0f;
    
    size_t displayPoints = std::min(size_t(20), series.size());
    size_t startIdx = series.size() - displayPoints;
    
    for (size_t i = 0; i < displayPoints; ++i) {
        size_t idx = startIdx + i;
        float value = series[idx].value;
        int barHeight = static_cast<int>(((value - minVal) / range) * 20.0f);
        
        ss << "      Step " << series[idx].step << ": [";
        for (int j = 0; j < barHeight; ++j) ss << "*";
        ss << " " << std::fixed << std::setprecision(2) << value << "]" << std::endl;
    }
    
    return ss.str();
}

} // namespace nlm
