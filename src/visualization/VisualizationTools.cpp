#include "visualization/VisualizationTools.hpp"
#include <iomanip>
#include <limits>
#include <sstream>

namespace nlm {

// VisualizationPanel implementation
void VisualizationPanel::updateFrequencyChanged() {
    // Default implementation - do nothing
}

// SpikeRasterPanel implementation
void SpikeRasterPanel::render() {
    // Call the base class render method to print summary
    VisualizationPanel::render();
}

// TimeSeriesPanel implementation
void TimeSeriesPanel::render() {
    // Call the base class render method to print summary
    VisualizationPanel::render();
}

// HeatmapPanel implementation
void HeatmapPanel::render() {
    // Call the base class render method to print summary
    VisualizationPanel::render();
}

// RealTimeVisualizationManager implementation
void RealTimeVisualizationManager::updateLoop() {
    while (running_) {
        updatePanels();
        std::this_thread::sleep_for(std::chrono::milliseconds(update_interval_ms_));
    }
}

} // namespace nlm