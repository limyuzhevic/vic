#pragma once

#include "VisualizationInterface.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <deque>
#include <mutex>
#include <cmath>

namespace nlm {

// Time series data point
struct TimeSeriesPoint {
    SimulationStep step;
    Timestamp time;
    float value;
    
    TimeSeriesPoint() : step(0), time(0.0), value(0.0f) {}
    TimeSeriesPoint(SimulationStep s, Timestamp t, float v) : step(s), time(t), value(v) {}
};

// Spatial position for visualization
struct VisualizationPosition {
    float x;
    float y;
    float z;
    
    VisualizationPosition() : x(0.0f), y(0.0f), z(0.0f) {}
    VisualizationPosition(float x, float y, float z) : x(x), y(y), z(z) {}
    
    float getDistance(const VisualizationPosition& other) const {
        float dx = x - other.x;
        float dy = y - other.y;
        float dz = z - other.z;
        return std::sqrt(dx*dx + dy*dy + dz*dz);
    }
};

// Simulation visualization
class SimulationVisualizer : public VisualizationInterface {
public:
    SimulationVisualizer();
    ~SimulationVisualizer() override;
    
    // Initialize visualization
    bool initialize() override;
    
    // Update visualization data
    void update(double deltaTime) override;
    
    // Render current state
    void render() override;
    
    // Close visualization
    void close() override;
    
    // Set simulation clock reference
    void setSimulationClock(std::shared_ptr<SimulationClock> clock);
    
    // Set spatial grid for visualization
    void setSpatialGrid(const std::unordered_map<RegionId, VisualizationPosition>& positions);
    
    // Add time series data
    void addTimeSeriesData(const std::string& name, const TimeSeriesPoint& point);
    
    // Set time series limit
    void setTimeSeriesLimit(size_t limit);
    
    // Get time series data
    const std::unordered_map<std::string, std::deque<TimeSeriesPoint>>& getTimeSeriesData() const;
    
    // Get spatial positions
    const std::unordered_map<RegionId, VisualizationPosition>& getSpatialPositions() const;
    
    // Generate spatial layout visualization
    std::string generateSpatialLayout() const;
    
    // Generate time series visualization
    std::string generateTimeSeriesVisualization(const std::string& seriesName) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
