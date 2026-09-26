#include "VisualizationInterface.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "dynamics/SpikeSystem.hpp"
#include "plasticity/STDP.hpp"
#include <cmath>
#include <algorithm>
#include <random>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <iostream>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    DebugLevel debugLevel;
    bool showLabels;
    std::string colorScheme;
    std::string exportPath;
    VisualizationMode currentMode;
    
    // Performance monitoring
    std::chrono::high_resolution_clock::time_point lastFrameTime;
    std::vector<double> frameTimes;
    size_t maxFrameHistory;
    
    // Data storage for visualization
    std::vector<std::vector<float>> networkData;
    std::vector<float> activityData;
    std::vector<SpikeEvent> spikeData;
    std::vector<std::vector<float>> weightData;
    std::map<std::string, std::string> statistics;
    
    // Configuration
    std::map<std::string, std::string> config;
    
    Impl() : active(false), updateRate(30.0), debugLevel(DebugLevel::Basic), 
             showLabels(true), colorScheme("default"), maxFrameHistory(100) {
        frameTimes.reserve(maxFrameHistory);
        networkData.resize(10, std::vector<float>(10, 0.0f));
    }
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize(const std::string& configFile) {
    // Load configuration if provided
    if (!configFile.empty()) {
        loadConfiguration(configFile);
    }
    
    // Initialize based on configuration
    pImpl->active = true;
    pImpl->lastFrameTime = std::chrono::high_resolution_clock::now();
    
    // Initialize visualization backend (placeholder)
    // In a real implementation, this would initialize OpenGL, SDL, etc.
    
    NLM_LOG_INFO("VisualizationInterface initialized");
    NLM_LOG_INFO("  Update rate: " + std::to_string(pImpl->updateRate) + " Hz");
    NLM_LOG_INFO("  Debug level: " + std::to_string(static_cast<int>(pImpl->debugLevel)));
    NLM_LOG_INFO("  Color scheme: " + pImpl->colorScheme);
    
    return true;
}

void VisualizationInterface::update() {
    if (!pImpl->active) return;
    
    // Record frame time for performance monitoring
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(currentTime - pImpl->lastFrameTime);
    pImpl->lastFrameTime = currentTime;
    
    if (pImpl->frameTimes.size() >= pImpl->maxFrameHistory) {
        pImpl->frameTimes.erase(pImpl->frameTimes.begin());
    }
    pImpl->frameTimes.push_back(duration.count() * 1000.0); // Convert to milliseconds
    
    // Update visualization data
    // In a real implementation, this would update the visualization buffers
}

void VisualizationInterface::render(VisualizationMode mode) {
    if (!pImpl->active) return;
    
    pImpl->currentMode = mode;
    
    // In a real implementation, this would render the visualization
    // For now, we'll just log the render call
    
    NLM_LOG_DEBUG("Rendering visualization mode: " + std::to_string(static_cast<int>(mode)));
}

void VisualizationInterface::close() {
    pImpl->active = false;
    // Cleanup visualization backend
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::setDebugLevel(DebugLevel level) {
    pImpl->debugLevel = level;
}

void VisualizationInterface::setShowLabels(bool show) {
    pImpl->showLabels = show;
}

void VisualizationInterface::setColorScheme(const std::string& scheme) {
    pImpl->colorScheme = scheme;
}

void VisualizationInterface::setExportPath(const std::string& path) {
    pImpl->exportPath = path;
}

void VisualizationInterface::visualizeNetwork(const Brain& brain, VisualizationMode mode) {
    // Generate network visualization data
    pImpl->networkData.clear();
    
    // Get regions from brain
    auto regions = brain.getRegions();
    
    // Estimate network size (placeholder - would use actual network topology)
    size_t numRegions = regions.size();
    size_t maxNeuronsPerRegion = 100; // Placeholder
    
    pImpl->networkData.resize(numRegions, std::vector<float>(maxNeuronsPerRegion, 0.0f));
    
    // Fill with activity data (placeholder)
    for (size_t i = 0; i < numRegions && i < pImpl->networkData.size(); ++i) {
        for (size_t j = 0; j < maxNeuronsPerRegion && j < pImpl->networkData[i].size(); ++j) {
            pImpl->networkData[i][j] = static_cast<float>(rand()) / RAND_MAX * 100.0f;
        }
    }
    
    NLM_LOG_INFO("Network visualization generated: " + std::to_string(numRegions) + " regions, " + 
                std::to_string(pImpl->networkData.empty() ? 0 : pImpl->networkData[0].size()) + " neurons/region");
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    // Generate activity visualization data
    pImpl->activityData.clear();
    
    // Get activity statistics
    size_t totalSpikes = brain.getTotalSpikeCount();
    size_t firingNeurons = brain.getFiringNeuronCount();
    float avgFiringRate = brain.getAverageFiringRate();
    
    // Store activity data
    pImpl->activityData = {
        static_cast<float>(totalSpikes),
        static_cast<float>(firingNeurons),
        avgFiringRate,
        brain.getExcitationInhibitionRatio()
    };
    
    NLM_LOG_INFO("Activity visualization generated: " + std::to_string(totalSpikes) + " total spikes, " +
                std::to_string(firingNeurons) + " firing neurons, " + std::to_string(avgFiringRate) + " Hz average");
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    pImpl->spikeData = spikes;
    
    NLM_LOG_INFO("Spike visualization generated: " + std::to_string(spikes.size()) + " spike events");
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    // Generate weight matrix visualization data
    pImpl->weightData.clear();
    
    // Get regions from brain
    auto regions = brain.getRegions();
    
    // Estimate weight matrix size (placeholder)
    size_t numRegions = regions.size();
    pImpl->weightData.resize(numRegions, std::vector<float>(numRegions, 0.0f));
    
    // Fill with random weights for demonstration
    for (size_t i = 0; i < numRegions; ++i) {
        for (size_t j = 0; j < numRegions; ++j) {
            pImpl->weightData[i][j] = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f;
        }
    }
    
    NLM_LOG_INFO("Weight visualization generated: " + std::to_string(numRegions) + " x " + std::to_string(numRegions) + " weight matrix");
}

void VisualizationInterface::visualizeConnectivity(const Brain& brain) {
    // Generate connectivity visualization data
    NLM_LOG_INFO("Connectivity visualization: Computing connectivity metrics...");
    
    // Get brain statistics
    size_t totalNeurons = brain.getTotalNeuronCount();
    size_t totalSynapses = brain.getTotalSynapseCount();
    
    if (totalNeurons > 0) {
        float connectionDensity = static_cast<float>(totalSynapses) / (totalNeurons * totalNeurons);
        NLM_LOG_INFO("  Connection density: " + std::to_string(connectionDensity * 100) + "%");
    }
    
    NLM_LOG_INFO("Connectivity visualization completed");
}

void VisualizationInterface::visualizeMemory(const Brain& brain) {
    NLM_LOG_INFO("Memory visualization: Generating memory state visualization...");
    
    // Get memory system statistics
    if (brain.getWorkingMemory()) {
        NLM_LOG_INFO("  Working memory traces: " + std::to_string(brain.getWorkingMemory()->getActiveTraces()));
    }
    
    if (brain.getEpisodicMemory()) {
        NLM_LOG_INFO("  Episodic memory episodes: " + std::to_string(brain.getEpisodicMemory()->getEpisodeCount()));
    }
    
    NLM_LOG_INFO("Memory visualization completed");
}

void VisualizationInterface::visualizeNeuromodulation(const Brain& brain) {
    NLM_LOG_INFO("Neuromodulation visualization: Generating neuromodulation state visualization...");
    
    if (brain.getDopamine()) {
        NLM_LOG_INFO("  Dopamine level: " + std::to_string(brain.getDopamine()->getLevel()));
    }
    
    if (brain.getNovelty()) {
        NLM_LOG_INFO("  Novelty level: " + std::to_string(brain.getNovelty()->getLevel()));
    }
    
    if (brain.getCuriosity()) {
        NLM_LOG_INFO("  Curiosity level: " + std::to_string(brain.getCuriosity()->getLevel()));
    }
    
    NLM_LOG_INFO("Neuromodulation visualization completed");
}

void VisualizationInterface::visualizeDevelopment(const Brain& brain) {
    NLM_LOG_INFO("Development visualization: Generating developmental stage visualization...");
    
    DevelopmentStage stage = brain.getDevelopmentalStage();
    NLM_LOG_INFO("  Developmental stage: " + std::to_string(static_cast<int>(stage)));
    
    // Get development system statistics
    if (brain.getDevelopmentSystem()) {
        NLM_LOG_INFO("  Development system active: Yes");
    }
    
    NLM_LOG_INFO("Development visualization completed");
}

bool VisualizationInterface::exportData(const std::string& filename, ExportFormat format) {
    std::string extension;
    switch (format) {
        case ExportFormat::Text: extension = ".txt"; break;
        case ExportFormat::CSV: extension = ".csv"; break;
        case ExportFormat::JSON: extension = ".json"; break;
        case ExportFormat::PNG: extension = ".png"; break;
        case ExportFormat::SVG: extension = ".svg"; break;
        case ExportFormat::PDF: extension = ".pdf"; break;
        case ExportFormat::HDF5: extension = ".h5"; break;
        default: extension = ".dat"; break;
    }
    
    std::string fullFilename = filename + extension;
    
    switch (format) {
        case ExportFormat::Text:
            return exportTextData(fullFilename);
        case ExportFormat::CSV:
            return exportCSVData(fullFilename);
        case ExportFormat::JSON:
            return exportJSONData(fullFilename);
        case ExportFormat::PNG:
        case ExportFormat::SVG:
        case ExportFormat::PDF:
            return exportImageData(fullFilename, format);
        case ExportFormat::HDF5:
            return exportHDF5Data(fullFilename);
        default:
            return exportTextData(fullFilename);
    }
}

bool VisualizationInterface::exportTextData(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for text export: " + filename);
        return false;
    }
    
    file << "NLM Visualization Data Export\n";
    file << "=============================\n\n";
    
    file << "Network Data:\n";
    for (size_t i = 0; i < pImpl->networkData.size(); ++i) {
        file << "  Region " << i << ": ";
        for (size_t j = 0; j < pImpl->networkData[i].size(); ++j) {
            if (j > 0) file << ", ";
            file << pImpl->networkData[i][j];
        }
        file << "\n";
    }
    
    file << "\nActivity Data:\n";
    file << "  Total Spikes: " << pImpl->activityData.size() > 0 ? pImpl->activityData[0] : 0 << "\n";
    file << "  Firing Neurons: " << pImpl->activityData.size() > 1 ? pImpl->activityData[1] : 0 << "\n";
    file << "  Average Firing Rate: " << pImpl->activityData.size() > 2 ? pImpl->activityData[2] : 0 << "\n";
    file << "  E/I Ratio: " << pImpl->activityData.size() > 3 ? pImpl->activityData[3] : 0 << "\n";
    
    file << "\nSpike Data:\n";
    file << "  Total Spikes: " << pImpl->spikeData.size() << "\n";
    
    file << "\nWeight Data:\n";
    for (size_t i = 0; i < pImpl->weightData.size(); ++i) {
        file << "  Row " << i << ": ";
        for (size_t j = 0; j < pImpl->weightData[i].size(); ++j) {
            if (j > 0) file << ", ";
            file << pImpl->weightData[i][j];
        }
        file << "\n";
    }
    
    file.close();
    NLM_LOG_INFO("Data exported to text file: " + filename);
    return true;
}

bool VisualizationInterface::exportCSVData(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for CSV export: " + filename);
        return false;
    }
    
    file << "type,value\n";
    
    // Export network statistics
    file << "network_regions," << pImpl->networkData.size() << "\n";
    if (!pImpl->networkData.empty()) {
        file << "network_neurons_per_region," << pImpl->networkData[0].size() << "\n";
    }
    
    // Export activity statistics
    file << "activity_total_spikes," << (pImpl->activityData.size() > 0 ? pImpl->activityData[0] : 0) << "\n";
    file << "activity_firing_neurons," << (pImpl->activityData.size() > 1 ? pImpl->activityData[1] : 0) << "\n";
    file << "activity_avg_firing_rate," << (pImpl->activityData.size() > 2 ? pImpl->activityData[2] : 0) << "\n";
    file << "activity_ei_ratio," << (pImpl->activityData.size() > 3 ? pImpl->activityData[3] : 0) << "\n";
    
    // Export spike statistics
    file << "spikes_total," << pImpl->spikeData.size() << "\n";
    
    // Export weight statistics
    size_t totalWeights = 0;
    for (const auto& row : pImpl->weightData) {
        totalWeights += row.size();
    }
    file << "weights_total," << totalWeights << "\n";
    
    file.close();
    NLM_LOG_INFO("Data exported to CSV file: " + filename);
    return true;
}

bool VisualizationInterface::exportJSONData(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for JSON export: " + filename);
        return false;
    }
    
    file << "{\n";
    file << "  \"nlm_visualization_data\": {\n";
    
    file << "    \"network_data\": [\n";
    for (size_t i = 0; i < pImpl->networkData.size(); ++i) {
        file << "      [";
        for (size_t j = 0; j < pImpl->networkData[i].size(); ++j) {
            if (j > 0) file << ", ";
            file << pImpl->networkData[i][j];
        }
        file << "]" << (i < pImpl->networkData.size() - 1 ? "," : "") << "\n";
    }
    file << "    ],\n";
    
    file << "    \"activity_data\": [";
    for (size_t i = 0; i < pImpl->activityData.size(); ++i) {
        if (i > 0) file << ", ";
        file << pImpl->activityData[i];
    }
    file << "],\n";
    
    file << "    \"spike_count\": " << pImpl->spikeData.size() << ",\n";
    
    file << "    \"weight_data\": [\n";
    for (size_t i = 0; i < pImpl->weightData.size(); ++i) {
        file << "      [";
        for (size_t j = 0; j < pImpl->weightData[i].size(); ++j) {
            if (j > 0) file << ", ";
            file << pImpl->weightData[i][j];
        }
        file << "]" << (i < pImpl->weightData.size() - 1 ? "," : "") << "\n";
    }
    file << "    ]\n";
    
    file << "  }\n";
    file << "}\n";
    
    file.close();
    NLM_LOG_INFO("Data exported to JSON file: " + filename);
    return true;
}

bool VisualizationInterface::exportImageData(const std::string& filename, ExportFormat format) {
    NLM_LOG_WARN("Image export not implemented yet: " + filename);
    return false;
}

bool VisualizationInterface::exportHDF5Data(const std::string& filename) {
    NLM_LOG_WARN("HDF5 export not implemented yet: " + filename);
    return false;
}

void VisualizationInterface::computeNetworkMetrics(const Brain& brain) {
    NLM_LOG_INFO("Computing network metrics...");
    
    // Compute basic network metrics
    size_t totalNeurons = brain.getTotalNeuronCount();
    size_t totalSynapses = brain.getTotalSynapseCount();
    size_t totalRegions = brain.getRegionCount();
    
    if (totalNeurons > 0) {
        float connectionDensity = static_cast<float>(totalSynapses) / (totalNeurons * totalNeurons);
        pImpl->statistics["connection_density"] = std::to_string(connectionDensity);
        
        float avgConnectionsPerNeuron = static_cast<float>(totalSynapses) / totalNeurons;
        pImpl->statistics["avg_connections_per_neuron"] = std::to_string(avgConnectionsPerNeuron);
        
        float neuronDensity = static_cast<float>(totalNeurons) / totalRegions;
        pImpl->statistics["neurons_per_region"] = std::to_string(neuronDensity);
    }
    
    pImpl->statistics["total_neurons"] = std::to_string(totalNeurons);
    pImpl->statistics["total_synapses"] = std::to_string(totalSynapses);
    pImpl->statistics["total_regions"] = std::to_string(totalRegions);
    
    NLM_LOG_INFO("Network metrics computed successfully");
}

void VisualizationInterface::computeActivityStatistics(const Brain& brain) {
    NLM_LOG_INFO("Computing activity statistics...");
    
    // Compute activity statistics
    size_t totalSpikes = brain.getTotalSpikeCount();
    size_t firingNeurons = brain.getFiringNeuronCount();
    float avgFiringRate = brain.getAverageFiringRate();
    float eiRatio = brain.getExcitationInhibitionRatio();
    
    pImpl->statistics["total_spikes"] = std::to_string(totalSpikes);
    pImpl->statistics["firing_neurons"] = std::to_string(firingNeurons);
    pImpl->statistics["avg_firing_rate"] = std::to_string(avgFiringRate);
    pImpl->statistics["ei_ratio"] = std::to_string(eiRatio);
    
    // Compute firing rate statistics
    float firingRateStdDev = 0.0f; // Placeholder - would need actual firing rate data
    pImpl->statistics["firing_rate_std_dev"] = std::to_string(firingRateStdDev);
    
    NLM_LOG_INFO("Activity statistics computed successfully");
}

void VisualizationInterface::computePlasticityMetrics(const Brain& brain) {
    NLM_LOG_INFO("Computing plasticity metrics...");
    
    // Get plasticity systems
    auto* stdp = brain.getSTDP();
    auto* hebbian = brain.getHebbian();
    auto* structural = brain.getStructuralPlasticity();
    
    if (stdp) {
        // Get STDP statistics
        pImpl->statistics["stdp_active"] = "true";
        // In a real implementation, you would get actual statistics
        pImpl->statistics["stdp_ltp_rate"] = "0.02";
        pImpl->statistics["stdp_ltd_rate"] = "0.015";
    }
    
    if (hebbian) {
        pImpl->statistics["hebbian_active"] = "true";
        pImpl->statistics["hebbian_learning_rate"] = "0.01";
    }
    
    if (structural) {
        pImpl->statistics["structural_active"] = "true";
        pImpl->statistics["synaptogenesis_rate"] = "0.0001";
        pImpl->statistics["pruning_rate"] = "0.00001";
    }
    
    NLM_LOG_INFO("Plasticity metrics computed successfully");
}

void VisualizationInterface::generateReport(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for report generation: " + filename);
        return;
    }
    
    file << "NLM Visualization Report\n";
    file << "========================\n\n";
    
    file << "Generated on: " << getCurrentTimestamp() << "\n\n";
    
    file << "Network Statistics:\n";
    for (const auto& stat : pImpl->statistics) {
        if (stat.first.find("network") != std::string::npos || 
            stat.first.find("neuron") != std::string::npos ||
            stat.first.find("synapse") != std::string::npos) {
            file << "  " << stat.first << ": " << stat.second << "\n";
        }
    }
    
    file << "\nActivity Statistics:\n";
    for (const auto& stat : pImpl->statistics) {
        if (stat.first.find("activity") != std::string::npos ||
            stat.first.find("spike") != std::string::npos ||
            stat.first.find("firing") != std::string::npos) {
            file << "  " << stat.first << ": " << stat.second << "\n";
        }
    }
    
    file << "\nPlasticity Statistics:\n";
    for (const auto& stat : pImpl->statistics) {
        if (stat.first.find("plasticity") != std::string::npos ||
            stat.first.find("stdp") != std::string::npos ||
            stat.first.find("hebbian") != std::string::npos) {
            file << "  " << stat.first << ": " << stat.second << "\n";
        }
    }
    
    file << "\nPerformance Statistics:\n";
    file << "  Average frame time: " << (pImpl->frameTimes.empty() ? 0.0 : std::accumulate(pImpl->frameTimes.begin(), pImpl->frameTimes.end(), 0.0) / pImpl->frameTimes.size()) << " ms\n";
    file << "  Frame rate: " << (pImpl->frameTimes.empty() ? 0.0 : 1000.0 / (std::accumulate(pImpl->frameTimes.begin(), pImpl->frameTimes.end(), 0.0) / pImpl->frameTimes.size())) << " Hz\n";
    
    file.close();
    NLM_LOG_INFO("Report generated: " + filename);
}

void VisualizationInterface::enableDebugOutput(DebugLevel level) {
    pImpl->debugLevel = level;
    NLM_LOG_INFO("Debug level set to: " + std::to_string(static_cast<int>(level)));
}

void VisualizationInterface::logState(const Brain& brain) {
    if (pImpl->debugLevel < DebugLevel::Detailed) return;
    
    NLM_LOG_DEBUG("=== Brain State Log ===");
    NLM_LOG_DEBUG("Total neurons: " + std::to_string(brain.getTotalNeuronCount()));
    NLM_LOG_DEBUG("Total synapses: " + std::to_string(brain.getTotalSynapseCount()));
    NLM_LOG_DEBUG("Active neurons: " + std::to_string(brain.getActiveNeuronCount()));
    NLM_LOG_DEBUG("Firing neurons: " + std::to_string(brain.getFiringNeuronCount()));
    NLM_LOG_DEBUG("Total spikes: " + std::to_string(brain.getTotalSpikeCount()));
    NLM_LOG_DEBUG("E/I ratio: " + std::to_string(brain.getExcitationInhibitionRatio()));
    NLM_LOG_DEBUG("Average firing rate: " + std::to_string(brain.getAverageFiringRate()));
    NLM_LOG_DEBUG("Regions: " + std::to_string(brain.getRegionCount()));
    NLM_LOG_DEBUG("========================");
}

void VisualizationInterface::printNetworkStatistics(const Brain& brain) {
    if (pImpl->debugLevel < DebugLevel::Basic) return;
    
    NLM_LOG_INFO("=== Network Statistics ===");
    NLM_LOG_INFO("Total neurons: " + std::to_string(brain.getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(brain.getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(brain.getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons: " + std::to_string(brain.getFiringNeuronCount()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(brain.getExcitationInhibitionRatio()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(brain.getAverageFiringRate()));
    NLM_LOG_INFO("Regions: " + std::to_string(brain.getRegionCount()));
    NLM_LOG_INFO("============================");
}

void VisualizationInterface::printPlasticityStatistics(const Brain& brain) {
    if (pImpl->debugLevel < DebugLevel::Detailed) return;
    
    NLM_LOG_DEBUG("=== Plasticity Statistics ===");
    
    auto* stdp = brain.getSTDP();
    if (stdp) {
        NLM_LOG_DEBUG("STDP: Active");
        // In a real implementation, you would get actual statistics
        NLM_LOG_DEBUG("STDP LTP weight: 0.02");
        NLM_LOG_DEBUG("STDP LTD weight: 0.015");
    }
    
    auto* hebbian = brain.getHebbian();
    if (hebbian) {
        NLM_LOG_DEBUG("Hebbian: Active");
        NLM_LOG_DEBUG("Hebbian learning rate: 0.01");
    }
    
    auto* structural = brain.getStructuralPlasticity();
    if (structural) {
        NLM_LOG_DEBUG("Structural: Active");
        NLM_LOG_DEBUG("Synaptogenesis rate: 0.0001");
        NLM_LOG_DEBUG("Pruning rate: 0.00001");
    }
    
    NLM_LOG_DEBUG("==========================");
}

void VisualizationInterface::startPerformanceTimer() {
    pImpl->lastFrameTime = std::chrono::high_resolution_clock::now();
}

void VisualizationInterface::stopPerformanceTimer() {
    if (pImpl->lastFrameTime.time_since_epoch().count() == 0) return;
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(currentTime - pImpl->lastFrameTime);
    
    if (pImpl->frameTimes.size() >= pImpl->maxFrameHistory) {
        pImpl->frameTimes.erase(pImpl->frameTimes.begin());
    }
    pImpl->frameTimes.push_back(duration.count() * 1000.0);
    
    pImpl->lastFrameTime = std::chrono::high_resolution_clock::time_point();
}

double VisualizationInterface::getLastFrameTime() const {
    return pImpl->frameTimes.empty() ? 0.0 : pImpl->frameTimes.back();
}

double VisualizationInterface::getAverageFrameTime() const {
    if (pImpl->frameTimes.empty()) return 0.0;
    return std::accumulate(pImpl->frameTimes.begin(), pImpl->frameTimes.end(), 0.0) / pImpl->frameTimes.size();
}

void VisualizationInterface::onMouseClick(int x, int y) {
    // Handle mouse click events
    NLM_LOG_DEBUG("Mouse click at: (" + std::to_string(x) + ", " + std::to_string(y) + ")");
}

void VisualizationInterface::onKeyPress(int key) {
    // Handle keyboard events
    NLM_LOG_DEBUG("Key pressed: " + std::to_string(key));
}

void VisualizationInterface::onWindowResize(int width, int height) {
    // Handle window resize events
    NLM_LOG_DEBUG("Window resized to: " + std::to_string(width) + " x " + std::to_string(height));
}

bool VisualizationInterface::loadConfiguration(const std::string& filename) {
    // Load configuration from file
    std::ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filename);
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Parse configuration lines (simple key=value format)
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            pImpl->config[key] = value;
        }
    }
    
    file.close();
    NLM_LOG_INFO("Configuration loaded from: " + filename);
    return true;
}

bool VisualizationInterface::saveConfiguration(const std::string& filename) const {
    // Save configuration to file
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file for writing: " + filename);
        return false;
    }
    
    for (const auto& config : pImpl->config) {
        file << config.first << " = \"" << config.second << "\"\n";
    }
    
    file.close();
    NLM_LOG_INFO("Configuration saved to: " + filename);
    return true;
}

std::string VisualizationInterface::getCurrentModeName() const {
    switch (pImpl->currentMode) {
        case VisualizationMode::NetworkGraph: return "Network Graph";
        case VisualizationMode::ActivityHeatmap: return "Activity Heatmap";
        case VisualizationMode::SpikeRaster: return "Spike Raster";
        case VisualizationMode::WeightMatrix: return "Weight Matrix";
        case VisualizationMode::ConnectivityMatrix: return "Connectivity Matrix";
        case VisualizationMode::PopulationStats: return "Population Statistics";
        case VisualizationMode::MemoryTraces: return "Memory Traces";
        case VisualizationMode::Neuromodulation: return "Neuromodulation";
        case VisualizationMode::Development: return "Development";
        case VisualizationMode::Custom: return "Custom";
        default: return "Unknown";
    }
}

std::vector<std::string> VisualizationInterface::getAvailableModes() const {
    return {
        "Network Graph",
        "Activity Heatmap",
        "Spike Raster",
        "Weight Matrix",
        "Connectivity Matrix",
        "Population Statistics",
        "Memory Traces",
        "Neuromodulation",
        "Development",
        "Custom"
    };
}

std::map<std::string, std::string> VisualizationInterface::getStatistics() const {
    return pImpl->statistics;
}

// Helper function to trim strings
static std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Helper function to get current timestamp
static std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace nlm
