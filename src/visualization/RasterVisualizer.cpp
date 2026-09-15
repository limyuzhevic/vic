#include "RasterVisualizer.hpp"
#include "VisualizationEngine.hpp"
#include "../brain/Brain.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include <string>
#include <vector>
#include <map>
#include <algorithm>

namespace nlm {

struct RasterVisualizer::Impl {
    VisualizationEngine* engine;
    bool active;
    
    // Spike data
    std::vector<DetailedSpikeEvent> spikeHistory;
    std::vector<DetailedSpikeEvent> currentWindowSpikes;
    
    // Time window
    double timeWindow;           // seconds
    double xMin, xMax;           // time range (s)
    double yMin, yMax;           // neuron index range
    
    // Filtering
    std::vector<int> filteredNeuronIds;
    std::vector<int> filteredRegionIds;
    std::vector<NeuronType> filteredTypes;
    bool showAllNeurons;
    bool showByType;
    bool showByRegion;
    bool colorByType;
    bool colorByRegion;
    bool highlightBursts;
    bool showInterSpikeIntervals;
    
    // Raster parameters
    double binWidth;             // ms per bin
    double binHeight;            // pixels per bin
    
    // Performance tracking
    double lastUpdateTime;
    
    Impl()
        : active(false)
        , timeWindow(10.0)           // 10 second time window
        , xMin(0.0), xMax(10.0)      // 10 seconds range
        , yMin(0.0), yMax(100.0)     // 100 neurons range
        , showAllNeurons(true)
        , showByType(false)
        , showByRegion(false)
        , colorByType(false)
        , colorByRegion(false)
        , highlightBursts(true)
        , showInterSpikeIntervals(false)
        , binWidth(10.0)             // 10 ms bins
        , binHeight(1.0)            // 1 pixel per neuron
        , lastUpdateTime(0.0) {}
};

RasterVisualizer::RasterVisualizer() : pImpl(new Impl) {}
RasterVisualizer::~RasterVisualizer() = default;

bool RasterVisualizer::initialize(VisualizationEngine* engine) {
    pImpl->engine = engine;
    pImpl->active = true;
    
    // Initialize backend-specific resources
    // TODO: Implement based on backend
    
    return true;
}

void RasterVisualizer::update(const Brain& brain, double timestep, const SpikeSystem& spikeSystem) {
    if (!pImpl->active) return;
    
    double currentTime = 0.0;  // TODO: Get from clock
    
    // Get spike history
    const auto& history = spikeSystem.getSpikeHistory();
    
    // Update current window spikes
    pImpl->currentWindowSpikes.clear();
    pImpl->spikeHistory = history;  // Store full history
    
    // Filter spikes based on criteria
    for (const auto& spike : history) {
        bool include = true;
        
        // Check time window
        if (spike.timestamp < currentTime - pImpl->timeWindow) {
            include = false;
        }
        
        // Check neuron filter
        if (!pImpl->showAllNeurons && !pImpl->filteredNeuronIds.empty()) {
            if (std::find(pImpl->filteredNeuronIds.begin(), pImpl->filteredNeuronIds.end(), 
                         spike.source.index()) == pImpl->filteredNeuronIds.end()) {
                include = false;
            }
        }
        
        // Check region filter
        if (!pImpl->showByRegion && !pImpl->filteredRegionIds.empty()) {
            if (std::find(pImpl->filteredRegionIds.begin(), pImpl->filteredRegionIds.end(), 
                         spike.regionId.index()) == pImpl->filteredRegionIds.end()) {
                include = false;
            }
        }
        
        // Check type filter
        if (!pImpl->showByType && !pImpl->filteredTypes.empty()) {
            // Need neuron type info - TODO: Implement neuron type lookup
            include = false;
        }
        
        if (include) {
            pImpl->currentWindowSpikes.push_back(spike);
        }
    }
}

void RasterVisualizer::render() {
    if (!pImpl->active) return;
    
    // Render spike raster plot
    // TODO: Implement backend-specific rendering
    
    // Example rendering steps:
    // 1. Setup viewport and axes
    // 2. Draw time axis (x-axis)
    // 3. Draw neuron index axis (y-axis)
    // 4. For each spike in current window:
    //    - Calculate x position (time converted to pixels)
    //    - Calculate y position (neuron index to pixel)
    //    - Draw spike as a vertical line or dot
    //    - Apply color based on type/region if enabled
    // 5. Draw burst markers if enabled
    // 6. Draw inter-spike interval statistics if enabled
}

void RasterVisualizer::close() {
    pImpl->active = false;
    
    // Cleanup backend-specific resources
    // TODO: Implement based on backend
}

void RasterVisualizer::setTimeWindow(double seconds) {
    pImpl->timeWindow = seconds;
    pImpl->xMin = pImpl->lastUpdateTime - seconds;
    pImpl->xMax = pImpl->lastUpdateTime;
}

void RasterVisualizer::setShowAllNeurons(bool show) {
    pImpl->showAllNeurons = show;
}

void RasterVisualizer::setShowByType(bool show) {
    pImpl->showByType = show;
}

void RasterVisualizer::setShowByRegion(bool show) {
    pImpl->showByRegion = show;
}

void RasterVisualizer::setColorByType(bool color) {
    pImpl->colorByType = color;
}

void RasterVisualizer::setColorByRegion(bool color) {
    pImpl->colorByRegion = color;
}

void RasterVisualizer::setHighlightBursts(bool highlight) {
    pImpl->highlightBursts = highlight;
}

void RasterVisualizer::setShowInterSpikeIntervals(bool show) {
    pImpl->showInterSpikeIntervals = show;
}

void RasterVisualizer::setFilterNeurons(const std::vector<int>& neuronIds) {
    pImpl->filteredNeuronIds = neuronIds;
    pImpl->showAllNeurons = neuronIds.empty();
}

void RasterVisualizer::setFilterRegions(const std::vector<int>& regionIds) {
    pImpl->filteredRegionIds = regionIds;
    pImpl->showByRegion = !regionIds.empty();
}

void RasterVisualizer::setFilterTypes(const std::vector<NeuronType>& types) {
    pImpl->filteredTypes = types;
    pImpl->showByType = !types.empty();
}

void RasterVisualizer::setBinWidth(double ms) {
    pImpl->binWidth = ms;
}

void RasterVisualizer::setBinHeight(double pixels) {
    pImpl->binHeight = pixels;
}

void RasterVisualizer::setXRange(double min, double max) {
    pImpl->xMin = min;
    pImpl->xMax = max;
}

void RasterVisualizer::setYRange(double min, double max) {
    pImpl->yMin = min;
    pImpl->yMax = max;
}

bool RasterVisualizer::exportRasterData(const std::string& filepath, ExportFormat format) {
    // Export raster data in requested format
    // TODO: Implement format-specific export
    return false;
}

bool RasterVisualizer::isActive() const {
    return pImpl->active;
}

RasterVisualizer::RasterStats RasterVisualizer::getStats() const {
    RasterStats stats;
    
    // Calculate statistics from current window
    stats.totalSpikes = pImpl->spikeHistory.size();
    stats.spikeCount = pImpl->currentWindowSpikes.size();
    
    // Calculate per-neuron spike counts
    std::map<NeuronId, size_t> neuronSpikeCounts;
    std::vector<double> allISIs;
    
    for (const auto& spike : pImpl->currentWindowSpikes) {
        neuronSpikeCounts[spike.source]++;
        
        // Need previous spike time for this neuron to calculate ISI
        // TODO: Implement ISI calculation
    }
    
    stats.neuronSpikeCounts.clear();
    for (const auto& pair : neuronSpikeCounts) {
        stats.neuronSpikeCounts.push_back(pair.second);
    }
    
    // Calculate ISIs statistics
    if (!allISIs.empty()) {
        double sum = 0.0, sumSq = 0.0;
        for (double isi : allISIs) {
            sum += isi;
            sumSq += isi * isi;
        }
        stats.averageInterSpikeInterval = sum / allISIs.size();
        double variance = (sumSq / allISIs.size()) - (stats.averageInterSpikeInterval * stats.averageInterSpikeInterval);
        stats.coefficientOfVariation = variance > 0 ? std::sqrt(variance) / stats.averageInterSpikeInterval : 0.0;
        stats.interSpikeIntervals = allISIs;
    }
    
    // Find max/min spike counts
    if (!neuronSpikeCounts.empty()) {
        auto maxIt = std::max_element(neuronSpikeCounts.begin(), neuronSpikeCounts.end(),
                                     [](const auto& a, const auto& b) { return a.second < b.second; });
        auto minIt = std::min_element(neuronSpikeCounts.begin(), neuronSpikeCounts.end(),
                                     [](const auto& a, const auto& b) { return a.second < b.second; });
        stats.maxSpikesPerNeuron = maxIt->second;
        stats.minSpikesPerNeuron = minIt->second;
    }
    
    // Count spikes by type and region
    // TODO: Implement when neuron type/region info is available
    
    return stats;
}

} // namespace nlm
