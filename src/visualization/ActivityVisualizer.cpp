#include "ActivityVisualizer.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>
#include <unordered_map>
#include <mutex>
#include <SDL.h>
#include <SDL_image.h>

namespace nlm {

struct ActivityVisualizer::Impl {
    // Activity data storage
    std::vector<ActivitySample> activitySamples;
    std::vector<SpikeTrain> spikeTrains;
    std::vector<PopulationStats> populationStats;
    HeatMapData heatmapData;
    
    // Time-series data
    std::map<std::string, TimeSeriesData> timeSeries;
    std::mutex timeSeriesMutex;
    
    // Configuration
    ActivityVisualizationConfig config;
    Timestamp currentTime;
    Timestamp timeWindow;
    bool advancedFeaturesEnabled;
    
    // Interaction state
    bool mousePressed;
    float lastMouseX, lastMouseY;
    std::vector<NeuronId> selectedNeurons;
    NeuronId hoveredNeuron;
    
    // Rendering state
    float zoomLevel;
    float panX, panY;
    bool showDebugInfo;
    bool showGrid;
    
    // Performance optimization
    bool lodEnabled;
    size_t maxSpikeCount;
    bool timeSeriesCompression;
    std::vector<bool> spikeVisible;
    
    // Statistics
    float totalActivity;
    uint32_t totalSpikes;
    float averageFiringRate;
    float averageMembranePotential;
    float maxActivityLevel;
    
    // Colors and styling
    std::map<std::string, SDL_Color> colorMap;
    std::map<std::string, std::vector<float>> gradientStops;
    
    // Font rendering
    TTF_Font* font;
    SDL_Texture* textureCache;
    
    // Heatmap rendering
    SDL_Texture* heatmapTexture;
    std::vector<float> heatmapBuffer;
    
    Impl() : currentTime(0.0), timeWindow(10.0f), advancedFeaturesEnabled(false),
             mousePressed(false), lastMouseX(0.0f), lastMouseY(0.0f),
             hoveredNeuron(), zoomLevel(1.0f), panX(0.0f), panY(0.0f),
             showDebugInfo(false), showGrid(true), lodEnabled(true),
             maxSpikeCount(10000), timeSeriesCompression(true),
             totalActivity(0.0f), totalSpikes(0), averageFiringRate(0.0f),
             averageMembranePotential(0.0f), maxActivityLevel(0.0f),
             font(nullptr), textureCache(nullptr), heatmapTexture(nullptr) {
        
        // Initialize color map
        colorMap["spike"] = {255, 100, 100, 255};              // Red for spikes
        colorMap["neuron_background"] = {50, 50, 50, 255};    // Dark gray
        colorMap["neuron_border"] = {200, 200, 200, 255};    // Light gray
        colorMap["active_neuron"] = {255, 200, 0, 255};      // Yellow for active
        colorMap["sensory"] = {100, 255, 100, 255};           // Green
        colorMap["motor"] = {255, 100, 255, 255};            // Magenta
        colorMap["internal"] = {100, 200, 255, 255};         // Blue
        colorMap["inhibitory"] = {255, 150, 150, 255};       // Light red
        colorMap["heatmap"] = {255, 100, 100, 200};          // Semi-transparent red
        colorMap["time_axis"] = {200, 200, 200, 255};        // Light gray
        colorMap["neuromodulation"] = {150, 100, 255, 255}; // Purple
        colorMap["reward"] = {0, 255, 100, 255};             // Green
        colorMap["prediction_error"] = {255, 150, 0, 255};  // Orange
        colorMap["grid"] = {40, 40, 40, 255};                // Grid lines
        
        // Initialize heatmap buffer
        heatmapBuffer.resize(1024 * 1024, 0.0f);  // 1024x1024 heatmap
    }
};

ActivityVisualizer::ActivityVisualizer() : pImpl(std::make_unique<Impl>()) {
}

ActivityVisualizer::~ActivityVisualizer() {
    close();
}

bool ActivityVisualizer::initialize() {
    if (pImpl->font) {
        TTF_CloseFont(pImpl->font);
        pImpl->font = nullptr;
    }
    
    // Load font for labels
    pImpl->font = TTF_OpenFont("assets/fonts/Roboto-Regular.ttf", 10);
    if (!pImpl->font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
    }
    
    // Initialize heatmap texture
    if (pImpl->heatmapTexture) {
        SDL_DestroyTexture(pImpl->heatmapTexture);
        pImpl->heatmapTexture = nullptr;
    }
    
    pImpl->selectedNeurons.clear();
    pImpl->hoveredNeuron = INVALID_NEURON_ID;
    pImpl->advancedFeaturesEnabled = false;
    
    return true;
}

void ActivityVisualizer::update() {
    std::lock_guard<std::mutex> lock(pImpl->timeSeriesMutex);
    
    // Update time series
    updateTimeSeriesData();
    
    // Update population statistics
    if (!activitySamples.empty()) {
        computePopulationStatistics(); // Need to get brain reference
    }
    
    // Update spike trains
    updateSpikeTrains(); // Need to get brain reference
    
    // Update heatmaps
    if (pImpl->heatmapData.cells.size() > 0) {
        updateHeatmaps(); // Need to get brain reference
    }
}

void ActivityVisualizer::render() {
    if (pImpl->activitySamples.empty()) {
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
    
    // Render spike rasters
    if (pImpl->config.showSpikeRasters) {
        renderSpikeRasters();
    }
    
    // Render heatmaps
    if (pImpl->config.showHeatmaps) {
        renderHeatmaps();
    }
    
    // Render membrane potentials
    if (pImpl->config.showMembranePotentials) {
        renderMembranePotentials();
    }
    
    // Render synaptic currents
    if (pImpl->config.showSynapticCurrents) {
        renderSynapticCurrents();
    }
    
    // Render neuromodulation signals
    if (pImpl->config.showNeuromodulation) {
        renderNeuromodulationSignals();
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

void ActivityVisualizer::close() {
    if (pImpl->font) {
        TTF_CloseFont(pImpl->font);
        pImpl->font = nullptr;
    }
    
    if (pImpl->heatmapTexture) {
        SDL_DestroyTexture(pImpl->heatmapTexture);
        pImpl->heatmapTexture = nullptr;
    }
}

bool ActivityVisualizer::isActive() const {
    return pImpl->initialized;
}

void ActivityVisualizer::setUpdateRate(double hz) {
    // Note: Controlled by visualization engine
}

double ActivityVisualizer::getUpdateRate() const {
    return 60.0;
}

void ActivityVisualizer::visualizeActivity(const Brain& brain) {
    // Convert brain activity to visualization data
    pImpl->activitySamples.clear();
    pImpl->spikeTrains.clear();
    
    // Extract activity data from brain
    auto neurons = brain.getAllNeurons();
    pImpl->activitySamples.reserve(neurons.size());
    
    for (const auto& neuron : neurons) {
        ActivitySample sample;
        sample.neuronId = neuron->getId();
        sample.neuronType = neuron->getType();
        sample.membranePotential = neuron->getMembranePotential();
        sample.firingState = neuron->getFiringState();
        sample.isActive = (neuron->getFiringState() == FiringState::Active);
        sample.firingRate = neuron->getFiringRate();
        sample.synapticCurrent = neuron->getTotalCurrent();
        sample.inputCurrent = neuron->getTotalCurrent(); // Placeholder
        sample.adaptationVariable = 0.0f; // Placeholder
        sample.rewardSignal = 0.0f; // Placeholder
        
        // Add to samples
        pImpl->activitySamples.push_back(sample);
        
        // Create spike train if neuron is active
        if (sample.isActive) {
            SpikeTrain spikeTrain;
            spikeTrain.neuronId = sample.neuronId;
            spikeTrain.neuronType = sample.neuronType;
            spikeTrain.x = sample.timestamp;
            spikeTrain.firingRate = sample.firingRate;
            spikeTrain.isBursting = (sample.firingRate > 50.0f); // Placeholder threshold
            spikeTrain.spikeTimes.push_back(sample.timestamp);
            spikeTrain.spikeAmplitudes.push_back(1.0f);
            
            pImpl->spikeTrains.push_back(spikeTrain);
        }
    }
    
    // Update population statistics
    computePopulationStatistics(brain);
    
    // Update spike trains
    updateSpikeTrains(brain);
    
    // Update heatmaps
    updateHeatmaps(brain);
}

void ActivityVisualizer::zoom(float factor) {
    pImpl->zoomLevel *= factor;
    pImpl->zoomLevel = std::max(0.1f, std::min(10.0f, pImpl->zoomLevel));
}

void ActivityVisualizer::pan(float dx, float dy) {
    pImpl->panX += dx;
    pImpl->panY += dy;
}

void ActivityVisualizer::selectNeuron(NeuronId neuronId) {
    if (pImpl->config.enableSelection) {
        pImpl->selectedNeurons.push_back(neuronId);
    }
}

void ActivityVisualizer::clearSelection() {
    pImpl->selectedNeurons.clear();
}

std::vector<NeuronId> ActivityVisualizer::getSelectedNeurons() const {
    return pImpl->selectedNeurons;
}

void ActivityVisualizer::computePopulationStatistics(const Brain& brain) {
    pImpl->populationStats.clear();
    
    std::map<NeuronType, std::vector<const ActivitySample*>> samplesByType;
    for (const auto& sample : pImpl->activitySamples) {
        samplesByType[sample.neuronType].push_back(&sample);
    }
    
    for (const auto& pair : samplesByType) {
        PopulationStats stats;
        stats.type = pair.first;
        stats.neuronCount = pair.second.size();
        
        if (!pair.second.empty()) {
            // Calculate statistics
            float totalFiringRate = 0.0f;
            float totalMembranePotential = 0.0f;
            float totalSynapticCurrent = 0.0f;
            
            for (const auto* sample : pair.second) {
                totalFiringRate += sample->firingRate;
                totalMembranePotential += sample->membranePotential;
                totalSynapticCurrent += sample->synapticCurrent;
                
                stats.minFiringRate = std::min(stats.minFiringRate, sample->firingRate);
                stats.maxFiringRate = std::max(stats.maxFiringRate, sample->firingRate);
                stats.minMembranePotential = std::min(stats.minMembranePotential, sample->membranePotential);
                stats.maxMembranePotential = std::max(stats.maxMembranePotential, sample->membranePotential);
            }
            
            stats.meanFiringRate = totalFiringRate / stats.neuronCount;
            stats.meanMembranePotential = totalMembranePotential / stats.neuronCount;
            stats.meanSynapticCurrent = totalSynapticCurrent / stats.neuronCount;
            
            // Calculate standard deviation
            float firingRateVariance = 0.0f;
            float potentialVariance = 0.0f;
            
            for (const auto* sample : pair.second) {
                firingRateVariance += (sample->firingRate - stats.meanFiringRate) * 
                                    (sample->firingRate - stats.meanFiringRate);
                potentialVariance += (sample->membranePotential - stats.meanMembranePotential) * 
                                   (sample->membranePotential - stats.meanMembranePotential);
            }
            
            stats.stdFiringRate = std::sqrt(firingRateVariance / stats.neuronCount);
            stats.stdMembranePotential = std::sqrt(potentialVariance / stats.neuronCount);
            
            // Create time series for statistics
            TimeSeriesData firingRateSeries("Firing Rate " + std::to_string(static_cast<int>(stats.type)));
            TimeSeriesData potentialSeries("Membrane Potential " + std::to_string(static_cast<int>(stats.type)));
            
            for (const auto& sample : pImpl->activitySamples) {
                if (sample.neuronType == stats.type) {
                    firingRateSeries.addSample(sample.timestamp, sample.firingRate);
                    potentialSeries.addSample(sample.timestamp, sample.membranePotential);
                }
            }
            
            stats.firingRateHistory = firingRateSeries.values;
            stats.membranePotentialHistory = potentialSeries.values;
            stats.activityHistory = firingRateSeries.values; // Simplified
        }
        
        pImpl->populationStats.push_back(stats);
    }
}

void ActivityVisualizer::computeSpatioTemporalStatistics(const Brain& brain) {
    // Compute spatio-temporal statistics
    // Placeholder implementation
}

std::vector<PopulationStats> ActivityVisualizer::getPopulationStatistics() const {
    return pImpl->populationStats;
}

std::vector<std::string> ActivityVisualizer::analyzeActivityPatterns() const {
    std::vector<std::string> patterns;
    patterns.push_back("Activity Analysis:");
    patterns.push_back("  Total samples: " + std::to_string(pImpl->activitySamples.size()));
    patterns.push_back("  Total spikes: " + std::to_string(pImpl->totalSpikes));
    patterns.push_back("  Average firing rate: " + std::to_string(pImpl->averageFiringRate) + " Hz");
    patterns.push_back("  Average membrane potential: " + std::to_string(pImpl->averageMembranePotential) + " mV");
    return patterns;
}

void ActivityVisualizer::renderTimeAxis() {
    // Render time axis
    // Placeholder implementation
}

void ActivityVisualizer::renderSpikeRasters() {
    // Render spike raster plots
    // Placeholder implementation
}

void ActivityVisualizer::renderHeatmaps() {
    // Render heatmaps
    // Placeholder implementation
}

void ActivityVisualizer::renderMembranePotentials() {
    // Render membrane potentials
    // Placeholder implementation
}

void ActivityVisualizer::renderSynapticCurrents() {
    // Render synaptic currents
    // Placeholder implementation
}

void ActivityVisualizer::renderNeuromodulationSignals() {
    // Render neuromodulation signals
    // Placeholder implementation
}

void ActivityVisualizer::renderStatisticalOverlays() {
    // Render statistical overlays
    // Placeholder implementation
}

void ActivityVisualizer::renderSelection() {
    // Render selected neurons
    // Placeholder implementation
}

void ActivityVisualizer::renderLegend() {
    // Render legend
    // Placeholder implementation
}

void ActivityVisualizer::renderGrid() {
    // Render grid lines
    // Placeholder implementation
}

void ActivityVisualizer::renderDebugInfo() {
    // Render debug information
    std::string debugText = 
        "Activity Samples: " + std::to_string(pImpl->activitySamples.size()) + "\n" +
        "Spike Trains: " + std::to_string(pImpl->spikeTrains.size()) + "\n" +
        "Total Spikes: " + std::to_string(pImpl->totalSpikes) + "\n" +
        "Average Firing Rate: " + std::to_string(pImpl->averageFiringRate) + " Hz\n" +
        "Zoom: " + std::to_string(pImpl->zoomLevel);
    
    // Note: Would need proper text rendering implementation
}

void ActivityVisualizer::updateSpikeTrains(const Brain& brain) {
    pImpl->totalSpikes = 0;
    pImpl->totalActivity = 0.0f;
    
    for (auto& spikeTrain : pImpl->spikeTrains) {
        // Update spike train
        spikeTrain.spikeTimes.clear();
        spikeTrain.spikeAmplitudes.clear();
        
        // Find spikes for this neuron in the current timestep
        for (const auto& sample : pImpl->activitySamples) {
            if (sample.neuronId == spikeTrain.neuronId && sample.isActive) {
                spikeTrain.spikeTimes.push_back(sample.timestamp);
                spikeTrain.spikeAmplitudes.push_back(getSpikeAmplitude(sample));
                pImpl->totalSpikes++;
                pImpl->totalActivity += getNeuronActivityLevel(sample);
            }
        }
        
        // Update firing rate estimate
        if (!spikeTrain.spikeTimes.empty()) {
            float timeSpan = pImpl->timeWindow;
            spikeTrain.firingRate = spikeTrain.spikeTimes.size() / timeSpan * 1000.0f; // Convert to Hz
        }
    }
    
    // Calculate averages
    if (!pImpl->activitySamples.empty()) {
        pImpl->averageFiringRate = pImpl->totalActivity / pImpl->activitySamples.size();
        pImpl->averageMembranePotential = 0.0f;
        
        float totalPotential = 0.0f;
        for (const auto& sample : pImpl->activitySamples) {
            totalPotential += sample.membranePotential;
        }
        
        pImpl->averageMembranePotential = totalPotential / pImpl->activitySamples.size();
        pImpl->maxActivityLevel = pImpl->totalActivity / pImpl->totalSpikes;
    }
}

void ActivityVisualizer::updateHeatmaps(const Brain& brain) {
    // Update heatmap data based on activity
    pImpl->heatmapData.normalizeActivity();
    
    // Convert heatmap to texture for rendering
    // Placeholder implementation
}

void ActivityVisualizer::updateTimeSeriesData() {
    // Update time series data
    // Placeholder implementation
}

float ActivityVisualizer::getNeuronActivityLevel(const ActivitySample& sample) const {
    if (sample.firingState == FiringState::Active) {
        return sample.firingRate / 100.0f; // Normalize to 0-1 range
    }
    return 0.0f;
}

float ActivityVisualizer::getSpikeAmplitude(const ActivitySample& sample) const {
    // Calculate spike amplitude based on neuron state
    if (sample.firingState == FiringState::Active) {
        return 1.0f + (sample.membranePotential - (-55.0f)) / 50.0f;
    }
    return 0.0f;
}

void ActivityVisualizer::exportActivityData(const std::string& filename, const std::string& format) {
    // Export activity data to file
    // Placeholder implementation
    std::cout << "Exporting activity data to " << filename << " in format " << format << std::endl;
}

void ActivityVisualizer::exportSpikeTrains(const std::string& filename) {
    // Export spike trains to file
    // Placeholder implementation
    std::cout << "Exporting spike trains to " << filename << std::endl;
}

void ActivityVisualizer::enableLOD(bool enable) {
    pImpl->lodEnabled = enable;
}

void ActivityVisualizer::setMaxSpikeCount(size_t maxSpikes) {
    pImpl->maxSpikeCount = maxSpikes;
}

void ActivityVisualizer::enableTimeSeriesCompression(bool enable) {
    pImpl->timeSeriesCompression = enable;
}

} // namespace nlm
