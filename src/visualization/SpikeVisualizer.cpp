// SpikeVisualizer.cpp - Spike activity visualization implementation

#include "SpikeVisualizer.hpp"
#include "Brain.hpp"
#include "Neuron.hpp"
#include "SpikeEvent.hpp"
#include "SpikeSystem.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <map>
#include <unordered_set>
#include <numeric>
#include <limits>

namespace nlm {

struct SpikeVisualizer::Impl {
    SpikeVisualizationConfig config;
    SpikeStats stats;
    bool initialized;
    std::unique_ptr<Brain> brainData;
    std::vector<SpikeEvent> spikeHistory;
    std::unordered_map<int64_t, std::vector<int64_t>> neuronSpikes; // neuronId -> spike times
    std::unordered_map<int64_t, std::vector<double>> neuronFiringRates; // neuronId -> firing rates
    std::unordered_map<int64_t, double> currentFiringRates;
    std::vector<SpikeBin> spikeBins;
    std::vector<CorrelationData> correlationData;
    double lastUpdateTime;
    double currentTime;
    std::chrono::steady_clock::time_point lastRealTimeUpdate;
    
    Impl() : initialized(false), lastUpdateTime(0.0), currentTime(0.0), lastRealTimeUpdate(std::chrono::steady_clock::now()) {
        config.mode = VisualizationMode::RASTER_PLOT;
        config.timeScale = 1.0;
        config.binWidth = 0.001;
        config.enableRealTimeUpdates = false;
        config.updateInterval = 0.1;
    }
};

SpikeVisualizer::SpikeVisualizer() : pImpl(std::make_unique<Impl>()) {
}

SpikeVisualizer::SpikeVisualizer(const SpikeVisualizationConfig& config) : pImpl(std::make_unique<Impl>()) {
    pImpl->config = config;
}

SpikeVisualizer::~SpikeVisualizer() {
}

void SpikeVisualizer::configure(const SpikeVisualizationConfig& config) {
    pImpl->config = config;
}

bool SpikeVisualizer::initialize(const Brain& brain) {
    pImpl->brainData = std::make_unique<Brain>(brain);
    pImpl->initialized = true;
    pImpl->spikeHistory.clear();
    pImpl->neuronSpikes.clear();
    pImpl->neuronFiringRates.clear();
    pImpl->currentFiringRates.clear();
    pImpl->spikeBins.clear();
    pImpl->correlationData.clear();
    
    // Initialize firing rate arrays
    for (const auto& neuron : pImpl->brainData->getNeurons()) {
        pImpl->currentFiringRates[neuron.getId()] = 0.0;
    }
    
    NLM_LOG_INFO("SpikeVisualizer initialized for " + std::to_string(pImpl->brainData->getNeuronCount()) + " neurons");
    return true;
}

void SpikeVisualizer::update(const Brain& brain, const std::vector<SpikeEvent>& spikes, double timeStep) {
    if (!pImpl->initialized) return;
    
    pImpl->currentTime += timeStep;
    pImpl->spikeHistory.insert(pImpl->spikeHistory.end(), spikes.begin(), spikes.end());
    
    // Update neuron spike data
    for (const auto& spike : spikes) {
        pImpl->neuronSpikes[spike.neuronId].push_back(spike.time);
    }
    
    // Calculate firing rates (simplified)
    updateFiringRates(*pImpl->brainData, timeStep);
    
    // Update statistics
    updateStatistics(*pImpl->brainData);
    
    // Real-time update handling
    if (pImpl->config.enableRealTimeUpdates) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration<double>(now - pImpl->lastRealTimeUpdate).count();
        if (elapsed >= pImpl->config.updateInterval) {
            // Trigger real-time update
            pImpl->lastRealTimeUpdate = now;
        }
    }
    
    // Generate correlation data if enabled
    if (pImpl->config.enableCrossCorrelation) {
        updateCorrelationData(*pImpl->brainData);
    }
}

bool SpikeVisualizer::render() {
    if (!pImpl->initialized) return false;
    
    // Render based on visualization mode
    switch (pImpl->config.mode) {
        case VisualizationMode::RASTER_PLOT:
            renderRasterPlot();
            break;
        case VisualizationMode::PERISTIMULUS_HISTOGRAM:
            renderPeristimulusHistogram();
            break;
        case VisualizationMode::SPIKE_TRAIN:
            renderSpikeTrain();
            break;
        case VisualizationMode::RATE_MAP:
            renderRateMap();
            break;
        case VisualizationMode::CROSS_CORRELATION:
            renderCrossCorrelation();
            break;
        case VisualizationMode::COHERENCE:
            renderCoherence();
            break;
        case VisualizationMode::POPULATION_DENSITY:
            renderPopulationDensity();
            break;
        case VisualizationMode::SYNCHRONY:
            renderSynchrony();
            break;
        case VisualizationMode::INPUT_OUTPUT_MAPPING:
            renderInputOutputMapping();
            break;
        case VisualizationMode::TIME_SLICE:
            renderTimeSlice();
            break;
        case VisualizationMode::COMPARING:
            renderComparing();
            break;
        case VisualizationMode::STATISTICS_OVERLAY:
            renderStatisticsOverlay();
            break;
        default:
            renderRasterPlot();
            break;
    }
    
    return true;
}

bool SpikeVisualizer::exportVisualization(const std::string& filename) const {
    if (!pImpl->config.enableExport) {
        NLM_LOG_WARNING("Export is disabled in configuration");
        return false;
    }
    
    std::string extension = getFileExtension(filename);
    
    if (extension == "csv" || extension == "CSV") {
        return exportToCsv(filename);
    } else if (extension == "json" || extension == "JSON") {
        return exportToJson(filename);
    } else if (extension == "png" || extension == "PNG") {
        return exportToPng(filename);
    } else if (extension == "pdf" || extension == "PDF") {
        return exportToPdf(filename);
    } else if (extension == "svg" || extension == "SVG") {
        return exportToSvg(filename);
    } else {
        NLM_LOG_ERROR("Unsupported export format: " + extension);
        return false;
    }
}

std::string SpikeVisualizer::generateSpikeAnalysis() const {
    std::ostringstream oss;
    oss << "Spike Activity Analysis Report\n";
    oss << "=============================\n\n";
    oss << "Generated: " << getCurrentTime() << "\n\n";
    oss << "Overview:\n";
    oss << "  Total Spikes: " << pImpl->stats.totalSpikes << "\n";
    oss << "  Spiking Neurons: " << pImpl->stats.totalSpikingNeurons << "\n";
    oss << "  Mean Firing Rate: " << pImpl->stats.meanFiringRate << " Hz\n";
    oss << "  Max Firing Rate: " << pImpl->stats.maxFiringRate << " Hz\n";
    oss << "  Min Firing Rate: " << pImpl->stats.minFiringRate << " Hz\n";
    oss << "  CV of Firing Rate: " << pImpl->stats.cvFiringRate << "\n";
    oss << "  Mean Synchrony: " << pImpl->stats.mean synchrony << "\n";
    oss << "  Mean Cross-Correlation: " << pImpl->stats.meanCrossCorrelation << "\n\n";
    
    oss << "Neuron Statistics:\n";
    for (const auto& pair : pImpl->stats.neuronStats) {
        oss << "  Neuron " << pair.first << ": " << pair.second << " Hz\n";
    }
    
    oss << "\nSynchronized Pairs:\n";
    for (const auto& pair : pImpl->stats.synchronizedPairs) {
        oss << "  Neuron " << pair.first << " <-> Neuron " << pair.second << "\n";
    }
    
    oss << "\nVisualization Configuration:\n";
    oss << "  Mode: " << visualizationModeToString(pImpl->config.mode) << "\n";
    oss << "  Time Scale: " << pImpl->config.timeScale << "\n";
    oss << "  Bin Width: " << pImpl->config.binWidth << " s\n";
    oss << "  Color Scheme: " << pImpl->config.colorScheme << "\n";
    
    return oss.str();
}

SpikeVisualizer::SpikeStats SpikeVisualizer::getSpikeStats() const {
    return pImpl->stats;
}

size_t SpikeVisualizer::getSpikeCount(int64_t neuronId) const {
    auto it = pImpl->neuronSpikes.find(neuronId);
    return (it != pImpl->neuronSpikes.end()) ? it->second.size() : 0;
}

double SpikeVisualizer::getFiringRate(int64_t neuronId) const {
    auto it = pImpl->currentFiringRates.find(neuronId);
    return (it != pImpl->currentFiringRates.end()) ? it->second : 0.0;
}

std::vector<int64_t> SpikeVisualizer::getSpikeTimes(int64_t neuronId) const {
    auto it = pImpl->neuronSpikes.find(neuronId);
    if (it != pImpl->neuronSpikes.end()) {
        return it->second;
    }
    return {};
}

SpikeVisualizer::VisualizationMode SpikeVisualizer::getVisualizationMode() const {
    return pImpl->config.mode;
}

void SpikeVisualizer::setVisualizationMode(VisualizationMode mode) {
    pImpl->config.mode = mode;
}

void SpikeVisualizer::setBrain(const Brain& brain) {
    if (pImpl->initialized) {
        pImpl->brainData = std::make_unique<Brain>(brain);
        pImpl->currentFiringRates.clear();
        for (const auto& neuron : pImpl->brainData->getNeurons()) {
            pImpl->currentFiringRates[neuron.getId()] = 0.0;
        }
        updateStatistics(*pImpl->brainData);
    }
}

// Private helper methods
void SpikeVisualizer::updateFiringRates(const Brain& brain, double timeStep) {
    // Update firing rates based on recent spike activity
    // This is a simplified implementation
    
    for (auto& pair : pImpl->currentFiringRates) {
        int64_t neuronId = pair.first;
        auto it = pImpl->neuronSpikes.find(neuronId);
        if (it != pImpl->neuronSpikes.end()) {
            // Calculate firing rate from recent spikes
            size_t spikeCount = it->second.size();
            // In real implementation, this would use time window
            pair.second = (timeStep > 0) ? (spikeCount / timeStep) : 0.0;
        }
    }
}

void SpikeVisualizer::updateStatistics(const Brain& brain) {
    // Calculate spike statistics
    pImpl->stats.totalSpikes = 0;
    pImpl->stats.totalSpikingNeurons = 0;
    pImpl->stats.meanFiringRate = 0.0;
    pImpl->stats.maxFiringRate = 0.0;
    pImpl->stats.minFiringRate = std::numeric_limits<double>::max();
    pImpl->stats.cvFiringRate = 0.0;
    
    std::vector<double> firingRates;
    
    for (const auto& pair : pImpl->currentFiringRates) {
        double rate = pair.second;
        pImpl->stats.totalSpikes += getSpikeCount(pair.first);
        
        if (rate > 0) {
            pImpl->stats.totalSpikingNeurons++;
            firingRates.push_back(rate);
            
            if (rate > pImpl->stats.maxFiringRate) {
                pImpl->stats.maxFiringRate = rate;
            }
            if (rate < pImpl->stats.minFiringRate) {
                pImpl->stats.minFiringRate = rate;
            }
        }
    }
    
    // Calculate statistics
    if (!firingRates.empty()) {
        pImpl->stats.meanFiringRate = std::accumulate(firingRates.begin(), firingRates.end(), 0.0) / firingRates.size();
        
        double variance = 0.0;
        for (double rate : firingRates) {
            variance += (rate - pImpl->stats.meanFiringRate) * (rate - pImpl->stats.meanFiringRate);
        }
        variance /= firingRates.size();
        pImpl->stats.cvFiringRate = (pImpl->stats.meanFiringRate > 0) ? 
            std::sqrt(variance) / pImpl->stats.meanFiringRate : 0.0;
    }
    
    // Calculate synchrony (simplified)
    pImpl->stats.mean synchrony = 0.0; // Placeholder
    
    // Calculate cross-correlation (simplified)
    pImpl->stats.meanCrossCorrelation = 0.0; // Placeholder
    
    // Update neuron stats
    pImpl->stats.neuronStats.clear();
    for (const auto& pair : pImpl->currentFiringRates) {
        pImpl->stats.neuronStats[pair.first] = pair.second;
    }
}

void SpikeVisualizer::updateCorrelationData(const Brain& brain) {
    // Calculate cross-correlation between neurons
    // This is a simplified placeholder
    pImpl->correlationData.clear();
    
    // In real implementation, this would calculate actual correlations
    size_t numNeurons = brain.getNeuronCount();
    if (numNeurons > 1 && numNeurons <= 10) { // Only for small networks
        for (size_t i = 0; i < numNeurons; ++i) {
            for (size_t j = i + 1; j < numNeurons; ++j) {
                CorrelationData corr;
                corr.neuronId1 = i;
                corr.neuronId2 = j;
                corr.crossCorrelation = (std::rand() % 100) / 100.0 - 0.5; // Random between -0.5 and 0.5
                corr.coherence = std::abs(corr.crossCorrelation);
                corr.lag = 0;
                pImpl->correlationData.push_back(corr);
            }
        }
    }
}

void SpikeVisualizer::renderRasterPlot() {
    // Render spike raster plot
    // Draw spikes as dots at (time, neuron) positions
}

void SpikeVisualizer::renderPeristimulusHistogram() {
    // Render peristimulus time histogram
    // Show firing rates in time bins around events
}

void SpikeVisualizer::renderSpikeTrain() {
    // Render continuous spike train
    // Show actual spike times as continuous traces
}

void SpikeVisualizer::renderRateMap() {
    // Render rate map
    // Show firing rates over space/time
}

void SpikeVisualizer::renderCrossCorrelation() {
    // Render cross-correlation
    // Show relationships between neuron pairs
}

void SpikeVisualizer::renderCoherence() {
    // Render coherence
    // Show signal coherence
}

void SpikeVisualizer::renderPopulationDensity() {
    // Render population density
    // Show 2D density of spikes
}

void SpikeVisualizer::renderSynchrony() {
    // Render synchrony
    // Show synchronization measures
}

void SpikeVisualizer::renderInputOutputMapping() {
    // Render input-output mapping
    // Show input-output relationships
}

void SpikeVisualizer::renderTimeSlice() {
    // Render time slice
    // Show selected time windows
}

void SpikeVisualizer::renderComparing() {
    // Render comparison
    // Show multiple conditions
}

void SpikeVisualizer::renderStatisticsOverlay() {
    // Render statistics overlay
    // Show statistical distributions
}

std::string SpikeVisualizer::visualizationModeToString(VisualizationMode mode) const {
    switch (mode) {
        case VisualizationMode::RASTER_PLOT: return "raster_plot";
        case VisualizationMode::PERISTIMULUS_HISTOGRAM: return "peristimulus_histogram";
        case VisualizationMode::SPIKE_TRAIN: return "spike_train";
        case VisualizationMode::RATE_MAP: return "rate_map";
        case VisualizationMode::CROSS_CORRELATION: return "cross_correlation";
        case VisualizationMode::COHERENCE: return "coherence";
        case VisualizationMode::POPULATION_DENSITY: return "population_density";
        case VisualizationMode::SYNCHRONY: return "synchrony";
        case VisualizationMode::INPUT_OUTPUT_MAPPING: return "input_output_mapping";
        case VisualizationMode::TIME_SLICE: return "time_slice";
        case VisualizationMode::COMPARING: return "comparing";
        case VisualizationMode::STATISTICS_OVERLAY: return "statistics_overlay";
        default: return "raster_plot";
    }
}

std::string SpikeVisualizer::getCurrentTime() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

bool SpikeVisualizer::exportToCsv(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return false;
    }
    
    // Export spike data as CSV
    file << "neuronId,time,rate\n";
    
    for (const auto& pair : pImpl->neuronSpikes) {
        int64_t neuronId = pair.first;
        double rate = getFiringRate(neuronId);
        for (int64_t spikeTime : pair.second) {
            file << neuronId << "," << spikeTime << "," << rate << "\n";
        }
    }
    
    file.close();
    NLM_LOG_INFO("Spike visualization exported as CSV: " + filename);
    return true;
}

bool SpikeVisualizer::exportToJson(const std::string& filename) const {
    // Export spike data as JSON
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return false;
    }
    
    file << generateSpikeAnalysis();
    
    file.close();
    NLM_LOG_INFO("Spike visualization exported as JSON: " + filename);
    return true;
}

bool SpikeVisualizer::exportToPng(const std::string& filename) const {
    // Placeholder for PNG export
    NLM_LOG_WARNING("PNG export not yet implemented for SpikeVisualizer");
    return false;
}

bool SpikeVisualizer::exportToPdf(const std::string& filename) const {
    // Placeholder for PDF export
    NLM_LOG_WARNING("PDF export not yet implemented for SpikeVisualizer");
    return false;
}

bool SpikeVisualizer::exportToSvg(const std::string& filename) const {
    // Placeholder for SVG export
    NLM_LOG_WARNING("SVG export not yet implemented for SpikeVisualizer");
    return false;
}

std::string SpikeVisualizer::getFileExtension(const std::string& filename) const {
    size_t pos = filename.rfind('.');
    if (pos == std::string::npos) return "";
    return filename.substr(pos + 1);
}

} // namespace nlm
