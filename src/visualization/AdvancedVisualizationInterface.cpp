// AdvancedVisualizationInterface.cpp - Implementation
g
#include "AdvancedVisualizationInterface.hpp"
#include "../src/core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <filesystem>
#include <thread>
#include <atomic>
#include <random>

namespace nlm {

// Forward declaration for LOG_ERROR
void LOG_ERROR(const std::string& message);

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    std::string backend;
    json visualizationOptions;
    int frameCount;
    std::string outputDirectory;
    std::atomic<bool> running;
    std::thread renderThread;
    std::mutex renderMutex;
    
    // Performance monitoring
    std::chrono::high_resolution_clock::time_point lastFrameTime;
    double lastFrameDuration;
    double fps;
    
    // Export state
    std::string lastExportFile;
    std::map<std::string, std::string> developmentStages;
    
    Impl() : active(false), updateRate(30.0), backend("console"), frameCount(0), 
             running(false), lastFrameDuration(0.0), fps(0.0), lastExportFile("") {
        lastFrameTime = std::chrono::high_resolution_clock::now();
        setDefaultOptions();
        initializeDevelopmentStages();
    }
    
    void setDefaultOptions() {
        visualizationOptions = {
            {"show_network", true},
            {"show_activity", true},
            {"show_spikes", true},
            {"show_weights", true},
            {"show_development", true},
            {"show_memory", true},
            {"show_neuromodulation", true},
            {"show_learning", true},
            {"show_performance", true},
            {"network_layout", "spring"},
            {"activity_mode", "raster"},
            {"color_scheme", "default"},
            {"render_3d", false},
            {"animation_enabled", false},
            {"frame_rate", 30.0},
            {"export_format", "json"},
            {"export_path", "./exports"},
            {"export_quality", "medium"},
            {"max_export_frames", 1000}
        };
    }
    
    void initializeDevelopmentStages() {
        developmentStages = {
            {"Initial", "Initial (plastic, high learning capacity)"},
            {"CriticalPeriod", "Critical Period (enhanced plasticity)"},
            {"Maturation", "Maturation (stabilizing connections)"},
            {"Adult", "Adult (mature, baseline plasticity)"},
            {"Aging", "Aging (declining plasticity)"}
        };
    }
};

VisualizationInterface::VisualizationInterface() : pImpl(std::make_unique<Impl>()) {}

VisualizationInterface::~VisualizationInterface() {
    close();
}

bool VisualizationInterface::initialize(const std::string& backend) {
    std::lock_guard<std::mutex> lock(renderMutex);
    
    pImpl->backend = backend;
    pImpl->active = true;
    pImpl->running = true;
    
    // Create output directory if needed
    if (!backend.empty() && backend != "console" && backend != "none") {
        std::filesystem::create_directories(pImpl->outputDirectory);
    }
    
    return true;
}

void VisualizationInterface::update() {
    std::lock_guard<std::mutex> lock(renderMutex);
    
    if (!pImpl->active) return;
    
    // Update performance metrics
    auto currentTime = std::chrono::high_resolution_clock::now();
    pImpl->lastFrameDuration = std::chrono::duration<double>(
        currentTime - pImpl->lastFrameTime).count();
    pImpl->fps = 1.0 / std::max(pImpl->lastFrameDuration, 1e-6);
    pImpl->lastFrameTime = currentTime;
    
    pImpl->frameCount++;
}

void VisualizationInterface::render(const Brain& brain, const std::string& mode) {
    std::lock_guard<std::mutex> lock(renderMutex);
    
    if (!pImpl->active) return;
    
    try {
        if (pImpl->backend == "console") {
            renderToConsole(brain, mode);
        } else if (pImpl->backend == "file") {
            renderToFile(brain, mode);
        } else {
            renderToConsole(brain, mode);
        }
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("Visualization render error: ") + e.what());
    }
}

void VisualizationInterface::close() {
    std::lock_guard<std::mutex> lock(renderMutex);
    
    pImpl->active = false;
    pImpl->running = false;
}

bool VisualizationInterface::isActive() const {
    std::lock_guard<std::mutex> lock(renderMutex);
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    std::lock_guard<std::mutex> lock(renderMutex);
    pImpl->updateRate = hz;
}

double VisualizationInterface::getUpdateRate() const {
    std::lock_guard<std::mutex> lock(renderMutex);
    return pImpl->updateRate;
}

void VisualizationInterface::visualizeNetwork(const Brain& brain, bool showWeights) {
    std::cout << "\n=== Network Visualization ===" << std::endl;
    
    for (const auto& regionId : brain.getRegionIds()) {
        const auto* region = brain.getRegion(regionId);
        if (region) {
            std::cout << "Region " << regionId.index() << ": " 
                     << region->getAllNeurons().size() << " neurons";
            
            if (showWeights) {
                float eiRatio = region->getExcitationInhibitionRatio();
                std::cout << " (E/I: " << eiRatio << ")";
            }
            
            std::cout << std::endl;
        }
    }
}

void VisualizationInterface::visualizeActivity(const Brain& brain, bool showRaster) {
    std::cout << "\n=== Neural Activity Visualization ===" << std::endl;
    
    if (showRaster) {
        std::cout << "Spike Raster Plot:" << std::endl;
        size_t totalSpikes = brain.getTotalSpikeCount();
        std::cout << "Total spikes recorded: " << totalSpikes << std::endl;
        
        if (totalSpikes > 0) {
            std::cout << "Average firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
            std::cout << "Active neuron percentage: " << 
                    100.0 * brain.getActiveNeuronCount() / std::max(1, brain.getTotalNeuronCount()) << "%" << std::endl;
        }
    }
    
    // Show neuron states
    std::cout << "\nNeuron States:" << std::endl;
    std::cout << "  Resting: " << (brain.getTotalNeuronCount() - brain.getActiveNeuronCount()) << std::endl;
    std::cout << "  Firing: " << brain.getFiringNeuronCount() << std::endl;
    std::cout << "  Active: " << brain.getActiveNeuronCount() << std::endl;
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    std::cout << "\n=== Spike Visualization ===" << std::endl;
    std::cout << "Spike Events: " << spikes.size() << std::endl;
    
    if (!spikes.empty()) {
        std::cout << "Time range: " << spikes.front().timestamp << " - " 
                 << spikes.back().timestamp << " ms" << std::endl;
        
        // Group spikes by neuron
        std::map<uint64_t, size_t> neuronSpikeCounts;
        for (const auto& spike : spikes) {
            neuronSpikeCounts[spike.neuronId.value]++;
        }
        
        std::cout << "\nSpikes per neuron:" << std::endl;
        for (const auto& pair : neuronSpikeCounts) {
            std::cout << "  Neuron " << pair.first << ": " << pair.second << " spikes" << std::endl;
        }
    }
}

void VisualizationInterface::visualizeWeights(const Brain& brain, bool showSynapticStrength) {
    std::cout << "\n=== Synaptic Weight Visualization ===" << std::endl;
    
    // Calculate global statistics
    float eiRatio = brain.getExcitationInhibitionRatio();
    std::cout << "Global E/I ratio: " << eiRatio << std::endl;
    
    // Show region-specific statistics
    for (const auto& regionId : brain.getRegionIds()) {
        const auto* region = brain.getRegion(regionId);
        if (region) {
            std::cout << "\nRegion " << regionId.index() << ":" << std::endl;
            
            float regionEIRatio = region->getExcitationInhibitionRatio();
            std::cout << "  E/I ratio: " << regionEIRatio << std::endl;
        }
    }
}

void VisualizationInterface::visualizeDevelopment(const Brain& brain) {
    std::cout << "\n=== Development Visualization ===" << std::endl;
    std::cout << "Developmental Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << std::endl;
    
    auto it = pImpl->developmentStages.find(developmentStageToString(brain.getDevelopmentalStage()));
    if (it != pImpl->developmentStages.end()) {
        std::cout << "Stage Description: " << it->second << std::endl;
    }
}

void VisualizationInterface::visualizeMemory(const Brain& brain) {
    std::cout << "\n=== Memory Visualization ===" << std::endl;
    
    if (auto* workingMem = brain.getWorkingMemory()) {
        std::cout << "Working Memory: " << workingMem->getActiveEntries() << " entries" << std::endl;
    }
    
    if (auto* episodicMem = brain.getEpisodicMemory()) {
        std::cout << "Episodic Memory: " << episodicMem->getEpisodeCount() << " episodes" << std::endl;
    }
    
    if (auto* assocMem = brain.getAssociativeMemory()) {
        std::cout << "Associative Memory: " << assocMem->getAssociationCount() << " associations" << std::endl;
    }
}

void VisualizationInterface::visualizeNeuromodulation(const Brain& brain) {
    std::cout << "\n=== Neuromodulation Visualization ===" << std::endl;
    
    if (auto* dopamine = brain.getDopamine()) {
        std::cout << "Dopamine: " << dopamine->getCurrentLevel() << " (range: 0.0-1.0)" << std::endl;
    }
    
    if (auto* curiosity = brain.getCuriosity()) {
        std::cout << "Curiosity: " << curiosity->getCurrentLevel() << " (range: 0.0-1.0)" << std::endl;
    }
    
    if (auto* novelty = brain.getNovelty()) {
        std::cout << "Novelty: " << novelty->getCurrentLevel() << " (range: 0.0-1.0)" << std::endl;
    }
    
    if (auto* predictionError = brain.getPredictionErrorSignal()) {
        std::cout << "Prediction Error: " << predictionError->getCurrentError() << std::endl;
    }
}

void VisualizationInterface::visualizeLearning(const Brain& brain) {
    std::cout << "\n=== Learning Visualization ===" << std::endl;
    
    // Get learning statistics
    size_t totalSpikes = brain.getTotalSpikeCount();
    size_t firingNeurons = brain.getFiringNeuronCount();
    
    if (totalSpikes > 0) {
        double learningIndex = static_cast<double>(firingNeurons) / totalSpikes;
        std::cout << "Learning Index: " << learningIndex << std::endl;
        
        if (learningIndex > 0.1) {
            std::cout << "Status: Active learning" << std::endl;
        } else {
            std::cout << "Status: Low activity" << std::endl;
        }
    }
    
    // Show plasticity statistics
    std::cout << "Plasticity: Enabled (STDP, Hebbian, Structural)" << std::endl;
}

void VisualizationInterface::visualizePerformance(const Brain& brain) {
    std::cout << "\n=== Performance Visualization ===" << std::endl;
    std::cout << "FPS: " << pImpl->fps << std::endl;
    std::cout << "Last frame time: " << pImpl->lastFrameDuration * 1000 << "ms" << std::endl;
    
    // Estimate brain performance
    double efficiency = brain.getAverageFiringRate() / std::max(brain.getTotalNeuronCount(), 1ul);
    std::cout << "Efficiency: " << efficiency << " spikes per neuron" << std::endl;
    
    if (pImpl->fps > 50) {
        std::cout << "Performance: Excellent" << std::endl;
    } else if (pImpl->fps > 30) {
        std::cout << "Performance: Good" << std::endl;
    } else if (pImpl->fps > 15) {
        std::cout << "Performance: Fair" << std::endl;
    } else {
        std::cout << "Performance: Poor" << std::endl;
    }
}

void VisualizationInterface::renderToConsole(const Brain& brain, const std::string& mode) {
    std::cout << "=== NLM Advanced Visualization (Frame " << pImpl->frameCount << ") ===" << std::endl;
    std::cout << "FPS: " << pImpl->fps << " | Frame time: " << pImpl->lastFrameDuration * 1000 << "ms" << std::endl;
    std::cout << "Backend: " << pImpl->backend << " | Mode: " << mode << std::endl;
    
    // Brain statistics
    std::cout << "\n--- Brain Statistics ---" << std::endl;
    std::cout << "Regions: " << brain.getRegionCount() << std::endl;
    std::cout << "Total Neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "Total Synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "Active Neurons: " << brain.getActiveNeuronCount() << std::endl;
    std::cout << "Firing Neurons: " << brain.getFiringNeuronCount() << std::endl;
    std::cout << "Average Firing Rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "Total Spikes: " << brain.getTotalSpikeCount() << std::endl;
    
    // Visualization components
    if (pImpl->visualizationOptions.value("show_network", true)) {
        renderNetworkConsole(brain);
    }
    
    if (pImpl->visualizationOptions.value("show_activity", true)) {
        renderActivityConsole(brain);
    }
    
    if (pImpl->visualizationOptions.value("show_spikes", true)) {
        renderSpikesConsole(brain);
    }
    
    if (pImpl->visualizationOptions.value("show_weights", true)) {
        renderWeightsConsole(brain);
    }
    
    if (pImpl->visualizationOptions.value("show_development", true)) {
        renderDevelopmentConsole(brain);
    }
    
    if (pImpl->visualizationOptions.value("show_memory", true)) {
        renderMemoryConsole(brain);
    }
    
    if (pImpl->visualizationOptions.value("show_neuromodulation", true)) {
        renderNeuromodulationConsole(brain);
    }
    
    if (pImpl->visualizationOptions.value("show_learning", true)) {
        renderLearningConsole(brain);
    }
    
    if (pImpl->visualizationOptions.value("show_performance", true)) {
        renderPerformanceConsole(brain);
    }
    
    std::cout << "\n=== End Frame " << pImpl->frameCount << " ===" << std::endl;
}

void VisualizationInterface::renderToFile(const Brain& brain, const std::string& mode) {
    std::string filename = pImpl->outputDirectory + "/nlm_visualization_" + 
                          std::to_string(pImpl->frameCount) + "." + 
                          pImpl->visualizationOptions.value("export_format", "json");
    
    json frameData;
    frameData["frame"] = pImpl->frameCount;
    frameData["timestamp"] = std::chrono::system_clock::now();
    frameData["backend"] = pImpl->backend;
    frameData["mode"] = mode;
    frameData["fps"] = pImpl->fps;
    frameData["frame_time_ms"] = pImpl->lastFrameDuration * 1000;
    
    // Add brain statistics
    frameData["brain_stats"] = {
        {"regions", brain.getRegionCount()},
        {"neurons", brain.getTotalNeuronCount()},
        {"synapses", brain.getTotalSynapseCount()},
        {"active_neurons", brain.getActiveNeuronCount()},
        {"firing_neurons", brain.getFiringNeuronCount()},
        {"average_firing_rate", brain.getAverageFiringRate()},
        {"total_spikes", brain.getTotalSpikeCount()}
    };
    
    // Write to file
    std::ofstream file(filename);
    if (file.is_open()) {
        file << frameData.dump(2);
        pImpl->lastExportFile = filename;
    }
}

void VisualizationInterface::renderNetworkConsole(const Brain& brain) {
    std::cout << "\n--- Network Structure ---" << std::endl;
    for (const auto& regionId : brain.getRegionIds()) {
        const auto* region = brain.getRegion(regionId);
        if (region) {
            std::cout << "  Region " << regionId.index() << ": " 
                     << region->getAllNeurons().size() << " neurons";
            
            float eiRatio = region->getExcitationInhibitionRatio();
            std::cout << " (E/I: " << eiRatio << ")" << std::endl;
        }
    }
}

void VisualizationInterface::renderActivityConsole(const Brain& brain) {
    std::cout << "\n--- Neural Activity ---" << std::endl;
    std::cout << "  Active Neurons: " << brain.getActiveNeuronCount() << std::endl;
    std::cout << "  Firing Neurons: " << brain.getFiringNeuronCount() << std::endl;
    std::cout << "  Average Firing Rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "  Total Spikes: " << brain.getTotalSpikeCount() << std::endl;
}

void VisualizationInterface::renderSpikesConsole(const Brain& brain) {
    std::cout << "\n--- Spike Activity ---" << std::endl;
    std::cout << "  Firing Neuron Ratio: " << 
              static_cast<double>(brain.getFiringNeuronCount()) / std::max(1, brain.getTotalNeuronCount()) << std::endl;
}

void VisualizationInterface::renderWeightsConsole(const Brain& brain) {
    std::cout << "\n--- Synaptic Properties ---" << std::endl;
    std::cout << "  Global E/I Ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
}

void VisualizationInterface::renderDevelopmentConsole(const Brain& brain) {
    std::cout << "\n--- Development Status ---" << std::endl;
    std::cout << "  Current Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << std::endl;
}

void VisualizationInterface::renderMemoryConsole(const Brain& brain) {
    std::cout << "\n--- Memory Systems ---" << std::endl;
    
    if (auto* workingMem = brain.getWorkingMemory()) {
        std::cout << "  Working Memory: " << workingMem->getActiveEntries() << " entries" << std::endl;
    }
    
    if (auto* episodicMem = brain.getEpisodicMemory()) {
        std::cout << "  Episodic Memory: " << episodicMem->getEpisodeCount() << " episodes" << std::endl;
    }
}

void VisualizationInterface::renderNeuromodulationConsole(const Brain& brain) {
    std::cout << "\n--- Neuromodulation ---" << std::endl;
    
    if (auto* dopamine = brain.getDopamine()) {
        std::cout << "  Dopamine: " << dopamine->getCurrentLevel() << std::endl;
    }
    
    if (auto* curiosity = brain.getCuriosity()) {
        std::cout << "  Curiosity: " << curiosity->getCurrentLevel() << std::endl;
    }
    
    if (auto* novelty = brain.getNovelty()) {
        std::cout << "  Novelty: " << novelty->getCurrentLevel() << std::endl;
    }
}

void VisualizationInterface::renderLearningConsole(const Brain& brain) {
    std::cout << "\n--- Learning Status ---" << std::endl;
    
    double learningIndex = brain.getAverageFiringRate() / 100.0; // Normalized
    std::cout << "  Learning Index: " << learningIndex << std::endl;
    std::cout << "  Plasticity: Enabled" << std::endl;
}

void VisualizationInterface::renderPerformanceConsole(const Brain& brain) {
    std::cout << "\n--- Performance ---" << std::endl;
    std::cout << "  FPS: " << pImpl->fps << std::endl;
    std::cout << "  Frame Time: " << pImpl->lastFrameDuration * 1000 << "ms" << std::endl;
}

std::string VisualizationInterface::developmentStageToString(DevelopmentalStage stage) {
    switch (stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: return "Unknown";
    }
}

// ModelExporter implementation
struct ModelExporter::Impl {
    std::string backend;
    json exportOptions;
    size_t exportCount;
    double totalExportSize;
    std::string lastExportFile;
    
    Impl() : exportCount(0), totalExportSize(0.0) {
        setDefaultOptions();
    }
    
    void setDefaultOptions() {
        exportOptions = {
            {"include_metadata", true},
            {"include_plasticity", true},
            {"include_memory", true},
            {"include_development", true},
            {"include_neuromodulation", true},
            {"scale_factor", 1.0},
            {"version", "2.0.0"},
            {"compression", "none"},
            {"quality", "medium"}
        };
    }
};

ModelExporter::ModelExporter() : pImpl(std::make_unique<Impl>()) {}

ModelExporter::~ModelExporter() = default;

bool ModelExporter::exportModel(const Brain& brain, const std::string& filepath,
                               const ExportOptions& options) {
    try {
        std::string extension = getFileExtension(filepath);
        bool success = false;
        
        if (extension == "json" || extension.empty()) {
            success = exportToJson(brain, options);
        } else if (extension == "csv") {
            success = exportToCsv(brain, filepath, options);
        } else {
            // Default to JSON
            success = exportToJson(brain, options);
        }
        
        if (success) {
            pImpl->exportCount++;
            pImpl->lastExportFile = filepath;
        }
        
        return success;
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("Model export error: ") + e.what());
        return false;
    }
}

bool ModelExporter::exportToJson(const Brain& brain, const ExportOptions& options) {
    json model;
    
    // Basic brain information
    model["brain_type"] = "NLM";
    model["version"] = "2.0.0";
    model["timestamp"] = std::chrono::system_clock::now();
    model["model_id"] = generateModelId();
    
    // Brain statistics
    model["statistics"] = {
        {"neuron_count", brain.getTotalNeuronCount()},
        {"synapse_count", brain.getTotalSynapseCount()},
        {"region_count", brain.getRegionCount()},
        {"active_neuron_count", brain.getActiveNeuronCount()},
        {"firing_neuron_count", brain.getFiringNeuronCount()},
        {"average_firing_rate", brain.getAverageFiringRate()},
        {"total_spike_count", brain.getTotalSpikeCount()},
        {"excitation_inhibition_ratio", brain.getExcitationInhibitionRatio()}
    };
    
    // Regions information
    model["regions"] = json::array();
    for (const auto& regionId : brain.getRegionIds()) {
        const auto* region = brain.getRegion(regionId);
        if (region) {
            json regionInfo;
            regionInfo["id"] = regionId.index();
            regionInfo["neuron_count"] = region->getAllNeurons().size();
            regionInfo["type"] = "unknown"; // Simplified
            
            // Neurons information
            regionInfo["neurons"] = json::array();
            for (const auto& neuron : region->getAllNeurons()) {
                json neuronInfo;
                neuronInfo["id"] = neuron->getId();
                neuronInfo["type"] = static_cast<int>(neuron->getType());
                neuronInfo["membrane_potential"] = neuron->getMembranePotential();
                neuronInfo["threshold"] = neuron->getThreshold();
                neuronInfo["refractory_period"] = neuron->getRefractoryPeriod();
                neuronInfo["active"] = neuron->isActive();
                
                regionInfo["neurons"].push_back(neuronInfo);
            }
            
            // Synapses information
            regionInfo["synapses"] = json::array();
            for (const auto& synapse : region->getSynapses()) {
                json synapseInfo;
                synapseInfo["id"] = synapse->getId();
                synapseInfo["source"] = synapse->getSourceNeuron().index();
                synapseInfo["target"] = synapse->getDestinationNeuron().index();
                synapseInfo["weight"] = synapse->getWeight();
                synapseInfo["type"] = static_cast<int>(synapse->getType());
                synapseInfo["delay"] = synapse->getDelay();
                synapseInfo["active"] = synapse->isActive();
                
                regionInfo["synapses"].push_back(synapseInfo);
            }
            
            model["regions"].push_back(regionInfo);
        }
    }
    
    // Memory information
    if (options.includeMemory) {
        model["memory"] = json::object();
        
        if (auto* workingMem = brain.getWorkingMemory()) {
            model["memory"]["working_memory"] = {
                {"active_entries", workingMem->getActiveEntries()},
                {"capacity", workingMem->getCapacity()}
            };
        }
        
        if (auto* episodicMem = brain.getEpisodicMemory()) {
            model["memory"]["episodic_memory"] = {
                {"episode_count", episodicMem->getEpisodeCount()},
                {"max_episodes", episodicMem->getMaxEpisodes()}
            };
        }
        
        if (auto* assocMem = brain.getAssociativeMemory()) {
            model["memory"]["associative_memory"] = {
                {"association_count", assocMem->getAssociationCount()},
                {"max_associations", assocMem->getMaxAssociations()}
            };
        }
    }
    
    // Development information
    if (options.includeDevelopment) {
        model["development"] = {
            {"stage", static_cast<int>(brain.getDevelopmentalStage())},
            {"stage_name", developmentStageToString(brain.getDevelopmentalStage())}
        };
    }
    
    // Neuromodulation information
    if (options.includeNeuromodulation) {
        model["neuromodulation"] = json::object();
        
        if (auto* dopamine = brain.getDopamine()) {
            model["neuromodulation"]["dopamine"] = dopamine->getCurrentLevel();
        }
        
        if (auto* curiosity = brain.getCuriosity()) {
            model["neuromodulation"]["curiosity"] = curiosity->getCurrentLevel();
        }
        
        if (auto* novelty = brain.getNovelty()) {
            model["neuromodulation"]["novelty"] = novelty->getCurrentLevel();
        }
        
        if (auto* predictionError = brain.getPredictionErrorSignal()) {
            model["neuromodulation"]["prediction_error"] = predictionError->getCurrentError();
        }
    }
    
    // Plasticity information
    if (options.includePlasticity) {
        model["plasticity"] = json::object();
        
        // Add plasticity rule information
        if (auto* stdp = brain.getSTDP()) {
            json stdpInfo;
            stdpInfo["learning_rate"] = stdp->getLearningRate();
            stdpInfo["max_weight"] = stdp->getMaxWeight();
            stdpInfo["min_weight"] = stdp->getMinWeight();
            model["plasticity"]["stdp"] = stdpInfo;
        }
        
        if (auto* hebbian = brain.getHebbian()) {
            json hebbianInfo;
            hebbianInfo["learning_rate"] = hebbian->getLearningRate();
            model["plasticity"]["hebbian"] = hebbianInfo;
        }
    }
    
    // Write to file
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << model.dump(2);
        return true;
    }
    
    return false;
}

bool ModelExporter::exportToCsv(const Brain& brain, const std::string& directory,
                               const ExportOptions& options) {
    std::filesystem::create_directories(directory);
    
    bool success = true;
    
    // Neurons CSV
    {
        std::string neuronsFile = directory + "/neurons.csv";
        std::ofstream file(neuronsFile);
        if (!file.is_open()) return false;
        
        file << "neuron_id,region_id,type,membrane_potential,threshold,refractory,active" << std::endl;
        
        for (const auto& regionId : brain.getRegionIds()) {
            const auto* region = brain.getRegion(regionId);
            if (region) {
                for (const auto& neuron : region->getAllNeurons()) {
                    file << neuron->getId().value << "," 
                         << regionId.index() << "," 
                         << static_cast<int>(neuron->getType()) << "," 
                         << neuron->getMembranePotential() << "," 
                         << neuron->getThreshold() << "," 
                         << neuron->getRefractoryPeriod() << "," 
                         << (neuron->isActive() ? "true" : "false") << std::endl;
                }
            }
        }
    }
    
    // Synapses CSV
    {
        std::string synapsesFile = directory + "/synapses.csv";
        std::ofstream file(synapsesFile);
        if (!file.is_open()) return false;
        
        file << "synapse_id,source_neuron,target_neuron,weight,type,delay,active" << std::endl;
        
        for (const auto& regionId : brain.getRegionIds()) {
            const auto* region = brain.getRegion(regionId);
            if (region) {
                for (const auto& synapse : region->getSynapses()) {
                    file << synapse->getId().value << "," 
                         << synapse->getSourceNeuron().index() << "," 
                         << synapse->getDestinationNeuron().index() << "," 
                         << synapse->getWeight() << "," 
                         << static_cast<int>(synapse->getType()) << "," 
                         << synapse->getDelay() << "," 
                         << (synapse->isActive() ? "true" : "false") << std::endl;
                }
            }
        }
    }
    
    return success;
}

bool ModelExporter::exportTopology(const Brain& brain, const std::string& filepath) {
    json topology;
    
    // Network topology
    topology["type"] = "graph";
    topology["nodes"] = brain.getTotalNeuronCount();
    topology["edges"] = brain.getTotalSynapseCount();
    
    // Region information
    topology["regions"] = json::array();
    for (const auto& regionId : brain.getRegionIds()) {
        const auto* region = brain.getRegion(regionId);
        if (region) {
            json regionInfo;
            regionInfo["id"] = regionId.index();
            regionInfo["neuron_count"] = region->getAllNeurons().size();
            regionInfo["connections"] = region->getSynapses().size();
            topology["regions"].push_back(regionInfo);
        }
    }
    
    // Write to file
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << topology.dump(2);
        return true;
    }
    
    return false;
}

bool ModelExporter::exportWeights(const Brain& brain, const std::string& filepath) {
    json weights;
    
    weights["type"] = "synaptic_weights";
    weights["timestamp"] = std::chrono::system_clock::now();
    
    // Collect all synaptic weights
    weights["synapses"] = json::array();
    for (const auto& regionId : brain.getRegionIds()) {
        const auto* region = brain.getRegion(regionId);
        if (region) {
            for (const auto& synapse : region->getSynapses()) {
                json synapseInfo;
                synapseInfo["source"] = synapse->getSourceNeuron().index();
                synapseInfo["target"] = synapse->getDestinationNeuron().index();
                synapseInfo["weight"] = synapse->getWeight();
                synapseInfo["type"] = static_cast<int>(synapse->getType());
                weights["synapses"].push_back(synapseInfo);
            }
        }
    }
    
    // Write to file
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << weights.dump(2);
        return true;
    }
    
    return false;
}

bool ModelExporter::exportNeurons(const Brain& brain, const std::string& filepath) {
    json neurons;
    
    neurons["type"] = "neural_population";
    neurons["timestamp"] = std::chrono::system_clock::now();
    
    // Collect neuron information
    neurons["neurons"] = json::array();
    for (const auto& regionId : brain.getRegionIds()) {
        const auto* region = brain.getRegion(regionId);
        if (region) {
            for (const auto& neuron : region->getAllNeurons()) {
                json neuronInfo;
                neuronInfo["id"] = neuron->getId().value;
                neuronInfo["region"] = regionId.index();
                neuronInfo["type"] = static_cast<int>(neuron->getType());
                neuronInfo["membrane_potential"] = neuron->getMembranePotential();
                neuronInfo["threshold"] = neuron->getThreshold();
                neuronInfo["refractory_period"] = neuron->getRefractoryPeriod();
                neuronInfo["active"] = neuron->isActive();
                neuronInfo["spike_count"] = 0; // Would need to track this
                
                neurons["neurons"].push_back(neuronInfo);
            }
        }
    }
    
    // Write to file
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << neurons.dump(2);
        return true;
    }
    
    return false;
}

bool ModelExporter::exportSpikeStats(const Brain& brain, const std::string& filepath) {
    json spikeStats;
    
    spikeStats["type"] = "spike_statistics";
    spikeStats["timestamp"] = std::chrono::system_clock::now();
    
    spikeStats["total_spikes"] = brain.getTotalSpikeCount();
    spikeStats["firing_neuron_count"] = brain.getFiringNeuronCount();
    spikeStats["average_firing_rate"] = brain.getAverageFiringRate();
    
    // Calculate firing statistics
    size_t totalNeurons = brain.getTotalNeuronCount();
    if (totalNeurons > 0) {
        spikeStats["active_neuron_percentage"] = 
            100.0 * brain.getActiveNeuronCount() / totalNeurons;
        spikeStats["firing_neuron_percentage"] = 
            100.0 * brain.getFiringNeuronCount() / totalNeurons;
    }
    
    // Write to file
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << spikeStats.dump(2);
        return true;
    }
    
    return false;
}

bool ModelExporter::exportLearningStats(const Brain& brain, const std::string& filepath) {
    json learningStats;
    
    learningStats["type"] = "learning_statistics";
    learningStats["timestamp"] = std::chrono::system_clock::now();
    
    // Learning indicators
    learningStats["learning_indicators"] = {
        {"excitatory_inhibition_ratio", brain.getExcitationInhibitionRatio()},
        {"plasticity_enabled", true},
        {"development_stage", static_cast<int>(brain.getDevelopmentalStage())}
    };
    
    // Performance metrics
    learningStats["performance_metrics"] = {
        {"fps", pImpl->fps},
        {"frame_time_ms", pImpl->lastFrameDuration * 1000},
        {"efficiency_score", calculateEfficiencyScore(brain)}
    };
    
    // Write to file
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << learningStats.dump(2);
        return true;
    }
    
    return false;
}

ModelStats ModelExporter::calculateStats(const Brain& brain) const {
    ModelStats stats;
    
    stats.neuronCount = brain.getTotalNeuronCount();
    stats.synapseCount = brain.getTotalSynapseCount();
    stats.regionCount = brain.getRegionCount();
    stats.firingRate = brain.getAverageFiringRate();
    
    // Estimate memory usage
    stats.memoryUsage = estimateMemoryUsage(brain);
    
    // Calculate efficiency
    stats.efficiency = calculateEfficiencyScore(brain);
    
    // Calculate plasticity activity (simplified)
    stats.plasticityCount = static_cast<double>(brain.getTotalSynapseCount()) * 0.5; // Placeholder
    
    return stats;
}

bool ModelExporter::validateModel(const Brain& brain, std::vector<std::string>& errors) const {
    errors.clear();
    
    if (brain.getTotalNeuronCount() == 0) {
        errors.push_back("Brain has no neurons");
    }
    
    if (brain.getTotalSynapseCount() == 0) {
        errors.push_back("Brain has no synapses");
    }
    
    // Check for consistency
    size_t totalNeurons = brain.getTotalNeuronCount();
    size_t activeNeurons = brain.getActiveNeuronCount();
    
    if (activeNeurons > totalNeurons) {
        errors.push_back("Active neurons count exceeds total neurons");
    }
    
    if (brain.getExcitationInhibitionRatio() < 0.0 || brain.getExcitationInhibitionRatio() > 10.0) {
        errors.push_back("Excitation/Inhibition ratio out of valid range");
    }
    
    return errors.empty();
}

void ModelExporter::setExportBackend(const std::string& backend) {
    pImpl->backend = backend;
}

std::string ModelExporter::getExportBackend() const {
    return pImpl->backend;
}

// Helper functions
std::string ModelExporter::getFileExtension(const std::string& filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos != std::string::npos && dotPos < filename.length() - 1) {
        return filename.substr(dotPos + 1);
    }
    return "";
}

std::string ModelExporter::generateModelId() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 999999);
    
    return std::to_string(dis(gen));
}

double ModelExporter::calculateEfficiencyScore(const Brain& brain) const {
    double firingRate = brain.getAverageFiringRate();
    double neuronCount = brain.getTotalNeuronCount();
    double eiRatio = brain.getExcitationInhibitionRatio();
    
    double efficiency = firingRate / neuronCount / eiRatio;
    return efficiency;
}

double ModelExporter::estimateMemoryUsage(const Brain& brain) const {
    // Estimate memory usage based on components
    double baseSize = 1024.0; // 1KB base
    
    // Neurons
    double neuronMemory = brain.getTotalNeuronCount() * 256.0; // ~256 bytes per neuron
    
    // Synapses
    double synapseMemory = brain.getTotalSynapseCount() * 128.0; // ~128 bytes per synapse
    
    // Memory systems
    double memorySystemMemory = 1024.0; // 1KB for memory systems
    
    double totalMemory = baseSize + neuronMemory + synapseMemory + memorySystemMemory;
    
    return totalMemory;
}

// Python interface implementations
namespace python {

std::shared_ptr<VisualizationInterface> createVisualization(const std::string& backend) {
    auto viz = std::make_shared<VisualizationInterface>();
    viz->initialize(backend);
    return viz;
}

std::shared_ptr<ModelExporter> createModelExporter(const std::string& backend) {
    auto exporter = std::make_shared<ModelExporter>();
    exporter->setExportBackend(backend);
    return exporter;
}

void visualizeBrain(const Brain& brain, const std::string& outputFile) {
    auto viz = createVisualization("console");
    viz->render(brain, "default");
    
    if (!outputFile.empty()) {
        auto exporter = createModelExporter("json");
        exporter->exportModel(brain, outputFile);
    }
}

void exportBrainModel(const Brain& brain, const std::string& outputFile, 
                     const std::string& format) {
    auto exporter = createModelExporter(format);
    exporter->exportModel(brain, outputFile);
}

void visualizeLearningProgress(const Brain& brain, int maxSteps) {
    std::cout << "=== Learning Progress Visualization ===" << std::endl;
    std::cout << "Brain learning over " << maxSteps << " steps" << std::endl;
    
    // Simulate learning progress
    for (int step = 0; step < maxSteps; step += maxSteps / 10) {
        std::cout << "Step " << step << ": " 
                 << brain.getAverageFiringRate() << " Hz, " 
                 << brain.getExcitationInhibitionRatio() << " E/I ratio" << std::endl;
    }
}

void visualizePerformanceMetrics(const Brain& brain, const std::vector<double>& metrics) {
    std::cout << "=== Performance Metrics Visualization ===" << std::endl;
    
    for (size_t i = 0; i < metrics.size(); ++i) {
        std::cout << "Metric " << i << ": " << metrics[i] << std::endl;
    }
}

void compareModels(const std::vector<std::shared_ptr<Brain>>& brains,
                  const std::vector<std::string>& labels) {
    std::cout << "=== Model Comparison ===" << std::endl;
    
    if (brains.size() != labels.size()) {
        std::cout << "Error: Number of brains and labels must match" << std::endl;
        return;
    }
    
    for (size_t i = 0; i < brains.size(); ++i) {
        std::cout << "\nModel " << labels[i] << ":" << std::endl;
        std::cout << "  Neurons: " << brains[i]->getTotalNeuronCount() << std::endl;
        std::cout << "  Synapses: " << brains[i]->getTotalSynapseCount() << std::endl;
        std::cout << "  Firing Rate: " << brains[i]->getAverageFiringRate() << " Hz" << std::endl;
        std::cout << "  E/I Ratio: " << brains[i]->getExcitationInhibitionRatio() << std::endl;
    }
}

json exportBrainForPython(const Brain& brain, const std::map<std::string, bool>& includeComponents) {
    json result;
    
    result["brain_type"] = "NLM";
    result["version"] = "2.0.0";
    result["statistics"] = {
        {"neuron_count", brain.getTotalNeuronCount()},
        {"synapse_count", brain.getTotalSynapseCount()},
        {"region_count", brain.getRegionCount()}
    };
    
    // Include optional components
    if (includeComponents.at("network")) {
        result["network"] = json::array();
        for (const auto& regionId : brain.getRegionIds()) {
            const auto* region = brain.getRegion(regionId);
            if (region) {
                json regionInfo;
                regionInfo["id"] = regionId.index();
                regionInfo["neuron_count"] = region->getAllNeurons().size();
                result["network"].push_back(regionInfo);
            }
        }
    }
    
    if (includeComponents.at("spikes")) {
        result["spikes"] = {
            {"total_spikes", brain.getTotalSpikeCount()},
            {"firing_neuron_count", brain.getFiringNeuronCount()}
        };
    }
    
    if (includeComponents.at("memory")) {
        result["memory"] = json::object();
        
        if (auto* workingMem = brain.getWorkingMemory()) {
            result["memory"]["working_memory"] = workingMem->getActiveEntries();
        }
        
        if (auto* episodicMem = brain.getEpisodicMemory()) {
            result["memory"]["episodic_memory"] = episodicMem->getEpisodeCount();
        }
    }
    
    if (includeComponents.at("neuromodulation")) {
        result["neuromodulation"] = json::object();
        
        if (auto* dopamine = brain.getDopamine()) {
            result["neuromodulation"]["dopamine"] = dopamine->getCurrentLevel();
        }
    }
    
    return result;
}

} // namespace python

} // namespace nlm

// Global functions for backward compatibility
void initializeNLMLogger() {
    nlm::initializeGlobalLogger();
}

void shutdownNLMLogger() {
    // Cleanup
}

void LOG_NLM(const std::string& message) {
    nlm::Logger::getGlobal().info(message);
}

void LOG_NLM_WARNING(const std::string& message) {
    nlm::Logger::getGlobal().warning(message);
}

void LOG_NLM_ERROR(const std::string& message) {
    nlm::Logger::getGlobal().error(message);
}