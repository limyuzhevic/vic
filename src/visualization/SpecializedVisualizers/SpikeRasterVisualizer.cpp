#include "SpikeRasterVisualizer.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

namespace nlm {

struct SpikeRasterVisualizer::Impl {
    // Spike data storage
    std::vector<SpikeData> allSpikes;
    std::vector<SpikeTrain> spikeTrains;  // Converted from spikes
    std::map<NeuronId, std::vector<SpikeData>> spikesByNeuron;
    std::map<NeuronType, PopulationActivity> populationActivity;
    
    // Configuration
    RasterPlotConfig config;
    Timestamp currentTime;       // Current simulation time
    Timestamp timeWindow;        // Time window for display
    bool advancedAnalysisEnabled; // Whether advanced analysis is enabled
    
    // Interaction state
    bool mousePressed;           // Whether mouse button is pressed
    float lastMouseX;            // Last mouse X position
    float lastMouseY;            // Last mouse Y position
    std::vector<NeuronId> selectedNeurons; // Selected neurons
    NeuronId hoveredNeuron;       // Currently hovered neuron
    
    // Rendering state
    float zoomLevel;             // Current zoom level
    float panX;                  // X pan offset
    float panY;                  // Y pan offset
    bool showDebugInfo;          // Whether to show debug info
    bool showGrid;              // Whether to show grid lines
    
    // Performance optimization
    bool lodEnabled;             // Whether level of detail is enabled
    size_t maxSpikeCount;        // Maximum number of spikes to store
    bool compressionEnabled;    // Whether spike compression is enabled
    std::vector<bool> neuronVisible; // Visibility of neurons
    
    // Statistics
    uint64_t totalSpikes;        // Total number of spikes recorded
    float averageFiringRate;     // Average firing rate across all neurons
    float maxFiringRate;         // Maximum firing rate
    float minFiringRate;         // Minimum firing rate
    float cvFiringRate;          // Coefficient of variation of firing rates
    std::map<uint32_t, uint32_t> burstStats; // Statistics about bursts
    
    // Colors and styling
    std::map<std::string, SDL_Color> colorMap; // Color map for visualization
    
    // Font rendering
    TTF_Font* font;              // Font for text rendering
    TTF_Font* fontSmall;         // Small font
    
    // Interaction state
    std::map<NeuronType, std::vector<float>> neuronPositions; // Neuron positions for grouping
    std::map<RegionId, std::vector<float>> regionPositions;   // Region positions
    
    // Analysis results
    std::vector<std::string> spikePatterns;   // Detected spike patterns
    std::vector<std::string> populationDynamics; // Population dynamics analysis
    
    Impl() : currentTime(0.0), timeWindow(10.0), advancedAnalysisEnabled(false),
             mousePressed(false), lastMouseX(0.0f), lastMouseY(0.0f),
             hoveredNeuron(INVALID_NEURON_ID), zoomLevel(1.0f), panX(0.0f), panY(0.0f),
             showDebugInfo(false), showGrid(true), lodEnabled(true), maxSpikeCount(10000),
             compressionEnabled(true), totalSpikes(0), averageFiringRate(0.0f),
             maxFiringRate(0.0f), minFiringRate(0.0f), cvFiringRate(0.0f), font(nullptr),
             fontSmall(nullptr) {
        
        // Initialize color map
        colorMap["spike"] = {255, 100, 100, 255};              // Red for spikes
        colorMap["selected_spike"] = {255, 255, 100, 255};   // Yellow for selected
        colorMap["burst_spike"] = {255, 200, 0, 255};        // Orange for bursts
        colorMap["neuron_background"] = {50, 50, 50, 255};   // Dark gray
        colorMap["neuron_border"] = {200, 200, 200, 255};   // Light gray
        colorMap["sensory"] = {100, 255, 100, 255};           // Green
        colorMap["motor"] = {255, 100, 255, 255};            // Magenta
        colorMap["internal"] = {100, 200, 255, 255};         // Blue
        colorMap["inhibitory"] = {255, 150, 150, 255};      // Light red
        colorMap["time_axis"] = {200, 200, 200, 255};        // Light gray
        colorMap["population"] = {150, 150, 150, 255};      // Gray for populations
        colorMap["grid"] = {40, 40, 40, 255};                // Grid lines
        
        // Initialize burst statistics
        burstStats[0] = 0;  // No burst
        burstStats[1] = 0;  // Single spike
        burstStats[2] = 0;  // Double spike
        burstStats[3] = 0;  // Triple spike
        burstStats[4] = 0;  // Quad spike or more
    }
};

SpikeRasterVisualizer::SpikeRasterVisualizer() : pImpl(std::make_unique<Impl>()) {
}

SpikeRasterVisualizer::~SpikeRasterVisualizer() {
    close();
}

bool SpikeRasterVisualizer::initialize() {
    if (pImpl->font) {
        TTF_CloseFont(pImpl->font);
        pImpl->font = nullptr;
    }
    
    if (pImpl->fontSmall) {
        TTF_CloseFont(pImpl->fontSmall);
        pImpl->fontSmall = nullptr;
    }
    
    // Load fonts
    pImpl->font = TTF_OpenFont("assets/fonts/Roboto-Regular.ttf", 10);
    pImpl->fontSmall = TTF_OpenFont("assets/fonts/Roboto-Regular.ttf", 8);
    
    if (!pImpl->font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
    }
    
    pImpl->selectedNeurons.clear();
    pImpl->hoveredNeuron = INVALID_NEURON_ID;
    
    return true;
}

void SpikeRasterVisualizer::update() {
    // Update spike trains from brain data
    // Note: This would require access to brain instance
    // For now, placeholder implementation
    
    // Update population statistics
    computePopulationActivity();
    
    // Update spike statistics
    computeSpikeStatistics();
    
    // Analyze burst patterns if enabled
    if (pImpl->advancedAnalysisEnabled) {
        analyzeBurstPatterns();
        computeCrossCorrelation();
    }
}

void SpikeRasterVisualizer::render() {
    if (pImpl->allSpikes.empty()) {
        return;
    }
    
    // Clear background
    SDL_SetRenderDrawColor(pImpl->renderer, pImpl->colorMap["neuron_background"].r,
                          pImpl->colorMap["neuron_background"].g, pImpl->colorMap["neuron_background"].b,
                          pImpl->colorMap["neuron_background"].a);
    SDL_RenderClear(pImpl->renderer);
    
    // Apply transformation
    SDL_RenderSetScale(pImpl->renderer, pImpl->zoomLevel, pImpl->zoomLevel);
    SDL_RenderSetLogicalTranslation(pImpl->renderer, -pImpl->panX, -pImpl->panY);
    
    // Render grid
    if (pImpl->showGrid) {
        renderGrid();
    }
    
    // Render spike trains
    if (pImpl->config.showSpikeTrains) {
        renderSpikeTrains();
    }
    
    // Render population groups
    if (pImpl->config.groupByType || pImpl->config.groupByRegion) {
        renderPopulationGroups();
    }
    
    // Render neuron labels
    if (pImpl->config.showNeuronLabels) {
        renderNeuronLabels();
    }
    
    // Render statistics
    if (pImpl->config.showStatistics) {
        renderStatistics();
    }
    
    // Render selection
    if (pImpl->config.enableSelection && !pImpl->selectedNeurons.empty()) {
        renderSelection();
    }
    
    // Render time axis
    if (pImpl->config.showTimeAxis) {
        renderTimeAxis();
    }
    
    // Render legend
    renderLegend();
    
    // Render debug info
    if (pImpl->showDebugInfo) {
        renderDebugInfo();
    }
    
    // Reset transformation
    SDL_RenderSetScale(pImpl->renderer, 1.0f, 1.0f);
    SDL_RenderSetLogicalTranslation(pImpl->renderer, 0, 0);
}

void SpikeRasterVisualizer::close() {
    if (pImpl->font) {
        TTF_CloseFont(pImpl->font);
        pImpl->font = nullptr;
    }
    
    if (pImpl->fontSmall) {
        TTF_CloseFont(pImpl->fontSmall);
        pImpl->fontSmall = nullptr;
    }
}

bool SpikeRasterVisualizer::isActive() const {
    return pImpl->initialized;
}

void SpikeRasterVisualizer::setUpdateRate(double hz) {
    // Note: Controlled by visualization engine
}

double SpikeRasterVisualizer::getUpdateRate() const {
    return 60.0;
}

void SpikeRasterVisualizer::visualizeSpikes(const Brain& brain) {
    // Convert brain spikes to SpikeData format
    pImpl->allSpikes.clear();
    pImpl->spikesByNeuron.clear();
    
    // Get spike events from brain
    auto spikeEvents = brain.getSpikeEvents();  // Placeholder - need actual API
    
    for (const auto& event : spikeEvents) {
        SpikeData spike;
        spike.neuronId = event.source_neuron;
        spike.time = event.timestamp;
        spike.amplitude = 1.0f;  // Placeholder
        spike.width = 0.5f;      // Placeholder
        spike.baseline = -70.0f; // Placeholder
        spike.threshold = -55.0f; // Placeholder
        
        // Find neuron type
        auto neurons = brain.getAllNeurons();
        for (const auto& neuron : neurons) {
            if (neuron->getId() == spike.neuronId) {
                spike.neuronType = neuron->getType();
                break;
            }
        }
        
        pImpl->allSpikes.push_back(spike);
        pImpl->spikesByNeuron[spike.neuronId].push_back(spike);
    }
    
    // Convert to spike trains for easier processing
    pImpl->spikeTrains.clear();
    for (const auto& pair : pImpl->spikesByNeuron) {
        SpikeTrain train;
        train.neuronId = pair.first;
        train.neuronType = pair.second.empty() ? NeuronType::Internal : pair.second[0].neuronType;
        train.x = 0.0f;  // Will be updated based on time
        
        for (const auto& spike : pair.second) {
            train.spikeTimes.push_back(spike.time);
            train.spikeAmplitudes.push_back(spike.amplitude);
        }
        
        // Compute firing rate
        if (!train.spikeTimes.empty() && pImpl->timeWindow > 0.0) {
            train.firingRate = train.spikeTimes.size() / pImpl->timeWindow * 1000.0f;  // Convert to Hz
        }
        
        pImpl->spikeTrains.push_back(train);
    }
    
    // Compute population activity
    computePopulationActivity();
    
    // Update statistics
    computeSpikeStatistics();
}

void SpikeRasterVisualizer::computePopulationActivity() {
    pImpl->populationActivity.clear();
    
    // Group spikes by neuron type
    std::map<NeuronType, std::vector<Timestamp>> spikesByType;
    for (const auto& spike : pImpl->allSpikes) {
        spikesByType[spike.neuronType].push_back(spike.time);
    }
    
    // Compute statistics for each type
    for (const auto& pair : spikesByType) {
        PopulationActivity activity;
        activity.neuronType = pair.first;
        
        const auto& times = pair.second;
        if (!times.empty()) {
            // Sort times
            std::vector<Timestamp> sortedTimes = times;
            std::sort(sortedTimes.begin(), sortedTimes.end());
            
            // Compute firing rate at different time points
            float binWidth = pImpl->timeWindow / 100.0f;  // 100 bins
            activity.spikeCounts.resize(100, 0);
            
            for (Timestamp t : sortedTimes) {
                size_t bin = static_cast<size_t>((t / pImpl->timeWindow) * 100.0f);
                if (bin < 100) {
                    activity.spikeCounts[bin]++;
                }
            }
            
            // Compute firing rate in each bin (spikes per second)
            float timeStep = pImpl->timeWindow / 100.0f;
            activity.activities.resize(100);
            activity.firingRates.resize(100);
            activity.membranePotentials.resize(100);
            
            for (size_t i = 0; i < 100; ++i) {
                activity.activities[i] = static_cast<float>(activity.spikeCounts[i]) / timeStep;  // Hz
                activity.firingRates[i] = activity.activities[i];  // Simplified
                activity.membranePotentials[i] = -70.0f + activity.firingRates[i] * 10.0f;  // Simplified
            }
            
            // Compute statistics
            float totalFiringRate = std::accumulate(activity.firingRates.begin(), activity.firingRates.end(), 0.0f);
            activity.meanFiringRate = totalFiringRate / activity.firingRates.size();
            
            float variance = 0.0f;
            for (float rate : activity.firingRates) {
                variance += (rate - activity.meanFiringRate) * (rate - activity.meanFiringRate);
            }
            activity.stdFiringRate = std::sqrt(variance / activity.firingRates.size());
            
            activity.minFiringRate = *std::min_element(activity.firingRates.begin(), activity.firingRates.end());
            activity.maxFiringRate = *std::max_element(activity.firingRates.begin(), activity.firingRates.end());
            
            if (activity.meanFiringRate > 0.0f) {
                activity.cvFiringRate = activity.stdFiringRate / activity.meanFiringRate;
            }
        }
        
        pImpl->populationActivity[pair.first] = activity;
    }
}

void SpikeRasterVisualizer::computeSpikeStatistics() {
    pImpl->totalSpikes = pImpl->allSpikes.size();
    
    if (pImpl->spikeTrains.empty()) {
        return;
    }
    
    // Compute overall statistics
    std::vector<float> allFiringRates;
    float totalActivity = 0.0f;
    
    for (const auto& train : pImpl->spikeTrains) {
        allFiringRates.push_back(train.firingRate);
        totalActivity += train.firingRate;
    }
    
    pImpl->averageFiringRate = totalActivity / pImpl->spikeTrains.size();
    
    if (!allFiringRates.empty()) {
        pImpl->minFiringRate = *std::min_element(allFiringRates.begin(), allFiringRates.end());
        pImpl->maxFiringRate = *std::max_element(allFiringRates.begin(), allFiringRates.end());
        
        float variance = 0.0f;
        for (float rate : allFiringRates) {
            variance += (rate - pImpl->averageFiringRate) * (rate - pImpl->averageFiringRate);
        }
        pImpl->cvFiringRate = std::sqrt(variance / allFiringRates.size());
    }
}

void SpikeRasterVisualizer::analyzeBurstPatterns() {
    // Analyze burst patterns in spike trains
    pImpl->burstStats.clear();
    pImpl->spikePatterns.clear();
    
    for (const auto& train : pImpl->spikeTrains) {
        if (train.spikeTimes.empty()) {
            pImpl->burstStats[0]++;
            continue;
        }
        
        if (train.spikeTimes.size() == 1) {
            pImpl->burstStats[1]++;
            continue;
        }
        
        // Detect bursts based on inter-spike intervals
        std::vector<float> isi;
        for (size_t i = 1; i < train.spikeTimes.size(); ++i) {
            isi.push_back(train.spikeTimes[i] - train.spikeTimes[i-1]);
        }
        
        if (isi.empty()) {
            pImpl->burstStats[1]++;
            continue;
        }
        
        // Threshold for burst detection (low ISI)
        float burstThreshold = 0.02f;  // 20ms threshold
        size_t burstLength = 0;
        
        for (float interval : isi) {
            if (interval < burstThreshold) {
                burstLength++;
            } else {
                break;
            }
        }
        
        if (burstLength >= 3) {  // At least 3 spikes in burst
            pImpl->burstStats[4]++;  // Quad or more spike burst
            pImpl->spikePatterns.push_back("Quad or more spike burst at neuron " + std::to_string(train.neuronId.index()));
        } else if (burstLength == 2) {
            pImpl->burstStats[3]++;  // Triple spike burst
            pImpl->spikePatterns.push_back("Triple spike burst at neuron " + std::to_string(train.neuronId.index()));
        } else if (burstLength == 1) {
            pImpl->burstStats[2]++;  // Double spike burst
            pImpl->spikePatterns.push_back("Double spike burst at neuron " + std::to_string(train.neuronId.index()));
        } else {
            pImpl->burstStats[1]++;  // Single spike
            pImpl->spikePatterns.push_back("Single spike at neuron " + std::to_string(train.neuronId.index()));
        }
    }
}

void SpikeRasterVisualizer::computeCrossCorrelation() {
    // Compute cross-correlation between different populations
    // Placeholder implementation
}

void SpikeRasterVisualizer::zoom(float factor) {
    pImpl->zoomLevel *= factor;
    pImpl->zoomLevel = std::max(0.1f, std::min(10.0f, pImpl->zoomLevel));
}

void SpikeRasterVisualizer::pan(float dx, float dy) {
    pImpl->panX += dx;
    pImpl->panY += dy;
}

void SpikeRasterVisualizer::selectNeuron(NeuronId neuronId) {
    if (pImpl->config.enableSelection) {
        pImpl->selectedNeurons.push_back(neuronId);
    }
}

void SpikeRasterVisualizer::clearSelection() {
    pImpl->selectedNeurons.clear();
}

std::vector<NeuronId> SpikeRasterVisualizer::getSelectedNeurons() const {
    return pImpl->selectedNeurons;
}

void SpikeRasterVisualizer::exportSpikeData(const std::string& filename, const std::string& format) {
    std::ofstream file(filename);
    if (file.is_open()) {
        if (format == "csv") {
            file << "NeuronId,NeuronType,SpikeTime,Amplitude,Width" << std::endl;
            for (const auto& spike : pImpl->allSpikes) {
                file << spike.neuronId.index() << ","
                     << static_cast<int>(spike.neuronType) << ","
                     << spike.time << ","
                     << spike.amplitude << ","
                     << spike.width << std::endl;
            }
        } else {
            file << "Spike data exported to " << filename << std::endl;
        }
        file.close();
    }
}

void SpikeRasterVisualizer::exportAnalysisResults(const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "=== Spike Train Analysis ===" << std::endl;
        file << "Total spikes: " << pImpl->totalSpikes << std::endl;
        file << "Average firing rate: " << pImpl->averageFiringRate << " Hz" << std::endl;
        file << "Burst statistics:" << std::endl;
        for (const auto& pair : pImpl->burstStats) {
            file << "  " << pair.first << " spikes: " << pair.second << std::endl;
        }
        file.close();
    }
}

std::vector<std::string> SpikeRasterVisualizer::analyzeSpikePatterns() const {
    return pImpl->spikePatterns;
}

std::vector<std::string> SpikeRasterVisualizer::analyzePopulationDynamics() const {
    std::vector<std::string> dynamics;
    
    for (const auto& pair : pImpl->populationActivity) {
        std::stringstream ss;
        ss << "Population " << static_cast<int>(pair.first) << ": "
           << "Mean firing rate: " << pair.second.meanFiringRate << " Hz, "
           << "CV: " << pair.second.cvFiringRate << ", "
           << "Firing rate range: [" << pair.second.minFiringRate << ", " << pair.second.maxFiringRate << "] Hz";
        dynamics.push_back(ss.str());
    }
    
    return dynamics;
}

void SpikeRasterVisualizer::enableLOD(bool enable) {
    pImpl->lodEnabled = enable;
}

void SpikeRasterVisualizer::setMaxSpikeCount(size_t maxSpikes) {
    pImpl->maxSpikeCount = maxSpikes;
}

void SpikeRasterVisualizer::enableCompression(bool enable) {
    pImpl->compressionEnabled = enable;
}

void SpikeRasterVisualizer::renderTimeAxis() {
    // Render time axis
    // Placeholder implementation
}

void SpikeRasterVisualizer::renderSpikeTrains() {
    // Render spike trains
    // Placeholder implementation
}

void SpikeRasterVisualizer::renderPopulationGroups() {
    // Render population groups
    // Placeholder implementation
}

void SpikeRasterVisualizer::renderNeuronLabels() {
    // Render neuron labels
    // Placeholder implementation
}

void SpikeRasterVisualizer::renderStatistics() {
    // Render statistics
    // Placeholder implementation
}

void SpikeRasterVisualizer::renderSelection() {
    // Render selection
    // Placeholder implementation
}

void SpikeRasterVisualizer::renderLegend() {
    // Render legend
    // Placeholder implementation
}

void SpikeRasterVisualizer::renderGrid() {
    // Render grid lines
    // Placeholder implementation
}

void SpikeRasterVisualizer::renderDebugInfo() {
    // Render debug information
    std::string debugText = 
        "Total Spikes: " + std::to_string(pImpl->totalSpikes) + "\n" +
        "Average Firing Rate: " + std::to_string(pImpl->averageFiringRate) + " Hz\n" +
        "Zoom: " + std::to_string(pImpl->zoomLevel);
    
    // Note: Would need proper text rendering implementation
}

} // namespace nlm
