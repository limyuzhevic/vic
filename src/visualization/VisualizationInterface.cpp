#include "VisualizationInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <cmath>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    Mode currentMode;
    Analysis currentAnalysis;
    bool debugEnabled;
    
    // Performance monitoring
    std::chrono::steady_clock::time_point lastUpdateTime;
    double frameTime;
    int frameCount;
    
    // Data for visualization
    std::vector<double> activityHistory;
    std::vector<float> weightHistory;
    std::vector<SpikeEvent> spikeHistory;
    
    // Callback
    VisualizationCallback callback;
    
    Impl() : active(false), updateRate(30.0), currentMode(Mode::Network), 
             currentAnalysis(Analysis::Statistics), debugEnabled(false),
             frameTime(0.0), frameCount(0) {
        lastUpdateTime = std::chrono::steady_clock::now();
    }
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize(Mode mode, Analysis analysis,
                                       double updateRate, bool enableDebug) {
    try {
        pImpl->currentMode = mode;
        pImpl->currentAnalysis = analysis;
        pImpl->updateRate = updateRate;
        pImpl->debugEnabled = enableDebug;
        
        // Initialize based on mode
        switch (mode) {
            case Mode::Network:
                // Initialize network visualization components
                break;
            case Mode::Activity:
                // Initialize activity visualization components
                break;
            case Mode::Spikes:
                // Initialize spike visualization components
                break;
            case Mode::Weights:
                // Initialize weight visualization components
                break;
            case Mode::Memory:
                // Initialize memory visualization components
                break;
            case Mode::Development:
                // Initialize development visualization components
                break;
            case Mode::Neuromodulation:
                // Initialize neuromodulation visualization components
                break;
            case Mode::Performance:
                // Initialize performance visualization components
                break;
            case Mode::Custom:
                // Initialize custom visualization components
                break;
            case Mode::None:
                // No initialization needed
                break;
        }
        
        pImpl->active = true;
        
        if (pImpl->debugEnabled) {
            NLM_LOG_INFO("VisualizationInterface initialized successfully");
            NLM_LOG_INFO("  Mode: " + std::to_string(static_cast<int>(mode)));
            NLM_LOG_INFO("  Analysis: " + std::to_string(static_cast<int>(analysis)));
            NLM_LOG_INFO("  Update Rate: " + std::to_string(updateRate) + " Hz");
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize visualization interface: " + std::string(e.what()));
        pImpl->active = false;
        return false;
    }
}

void VisualizationInterface::update() {
    if (!pImpl->active) {
        return;
    }
    
    try {
        auto currentTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - pImpl->lastUpdateTime);
        pImpl->frameTime = duration.count() / 1000.0;
        pImpl->lastUpdateTime = currentTime;
        pImpl->frameCount++;
        
        // Update visualization data based on mode
        switch (pImpl->currentMode) {
            case Mode::Network:
                // Update network visualization data
                break;
            case Mode::Activity:
                // Update activity visualization data
                break;
            case Mode::Spikes:
                // Update spike visualization data
                break;
            case Mode::Weights:
                // Update weight visualization data
                break;
            case Mode::Memory:
                // Update memory visualization data
                break;
            case Mode::Development:
                // Update development visualization data
                break;
            case Mode::Neuromodulation:
                // Update neuromodulation visualization data
                break;
            case Mode::Performance:
                // Update performance visualization data
                break;
            case Mode::Custom:
                // Update custom visualization data
                break;
            case Mode::None:
                // No update needed
                break;
        }
        
        if (pImpl->debugEnabled && pImpl->frameCount % 30 == 0) {
            double fps = pImpl->frameCount / (pImpl->frameTime + 0.001);
            NLM_LOG_INFO("VisualizationInterface: " + std::to_string(fps) + " FPS");
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error updating visualization interface: " + std::string(e.what()));
    }
}

void VisualizationInterface::render(bool showLabels, bool showValues,
                                    bool animated, double timeScale) {
    if (!pImpl->active) {
        return;
    }
    
    try {
        switch (pImpl->currentMode) {
            case Mode::Network:
                // Render network visualization
                if (pImpl->debugEnabled) {
                    NLM_LOG_INFO("Rendering network visualization");
                }
                break;
            case Mode::Activity:
                // Render activity visualization
                if (pImpl->debugEnabled) {
                    NLM_LOG_INFO("Rendering activity visualization");
                }
                break;
            case Mode::Spikes:
                // Render spike visualization
                if (pImpl->debugEnabled) {
                    NLM_LOG_INFO("Rendering spike visualization");
                }
                break;
            case Mode::Weights:
                // Render weight visualization
                if (pImpl->debugEnabled) {
                    NLM_LOG_INFO("Rendering weight visualization");
                }
                break;
            case Mode::Memory:
                // Render memory visualization
                if (pImpl->debugEnabled) {
                    NLM_LOG_INFO("Rendering memory visualization");
                }
                break;
            case Mode::Development:
                // Render development visualization
                if (pImpl->debugEnabled) {
                    NLM_LOG_INFO("Rendering development visualization");
                }
                break;
            case Mode::Neuromodulation:
                // Render neuromodulation visualization
                if (pImpl->debugEnabled) {
                    NLM_LOG_INFO("Rendering neuromodulation visualization");
                }
                break;
            case Mode::Performance:
                // Render performance visualization
                if (pImpl->debugEnabled) {
                    NLM_LOG_INFO("Rendering performance visualization");
                }
                break;
            case Mode::Custom:
                // Render custom visualization
                if (pImpl->debugEnabled) {
                    NLM_LOG_INFO("Rendering custom visualization");
                }
                break;
            case Mode::None:
                // No rendering needed
                break;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error rendering visualization interface: " + std::string(e.what()));
    }
}

void VisualizationInterface::close() {
    if (pImpl->active) {
        // Cleanup visualization resources
        pImpl->active = false;
        pImpl->callback = nullptr;
        
        if (pImpl->debugEnabled) {
            NLM_LOG_INFO("VisualizationInterface closed");
        }
    }
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setMode(Mode mode) {
    pImpl->currentMode = mode;
    
    if (pImpl->debugEnabled) {
        NLM_LOG_INFO("VisualizationInterface mode changed to: " + std::to_string(static_cast<int>(mode)));
    }
}

VisualizationInterface::Mode VisualizationInterface::getMode() const {
    return pImpl->currentMode;
}

void VisualizationInterface::setAnalysis(Analysis analysis) {
    pImpl->currentAnalysis = analysis;
    
    if (pImpl->debugEnabled) {
        NLM_LOG_INFO("VisualizationInterface analysis changed to: " + std::to_string(static_cast<int>(analysis)));
    }
}

VisualizationInterface::Analysis VisualizationInterface::getAnalysis() const {
    return pImpl->currentAnalysis;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
    
    if (pImpl->debugEnabled) {
        NLM_LOG_INFO("VisualizationInterface update rate changed to: " + std::to_string(hz) + " Hz");
    }
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::setDebugEnabled(bool enabled) {
    pImpl->debugEnabled = enabled;
    
    if (enabled) {
        NLM_LOG_INFO("Debug mode enabled for VisualizationInterface");
    } else {
        NLM_LOG_INFO("Debug mode disabled for VisualizationInterface");
    }
}

bool VisualizationInterface::isDebugEnabled() const {
    return pImpl->debugEnabled;
}

void VisualizationInterface::visualizeNetwork(const Brain& brain,
                                             const std::string& title,
                                             bool showRegions,
                                             bool showPopulations,
                                             bool showConnections) {
    if (!pImpl->active) {
        return;
    }
    
    // Collect network data
    std::vector<std::string> regions;
    std::vector<int> neuronCounts;
    std::vector<int> synapseCounts;
    
    for (const auto& region : brain.getRegions()) {
        regions.push_back(region->getName());
        neuronCounts.push_back(region->getTotalNeuronCount());
        synapseCounts.push_back(region->getSynapseCount());
    }
    
    if (pImpl->debugEnabled) {
        NLM_LOG_INFO("Network visualization: " + title);
        NLM_LOG_INFO("  Regions: " + std::to_string(regions.size()));
        NLM_LOG_INFO("  Total Neurons: " + std::to_string(brain.getTotalNeuronCount()));
        NLM_LOG_INFO("  Total Synapses: " + std::to_string(brain.getTotalSynapseCount()));
    }
    
    // Trigger visualization callback
    if (pImpl->callback) {
        py::dict data;
        data["type"] = "network";
        data["title"] = title;
        data["regions"] = regions;
        data["neuron_counts"] = neuronCounts;
        data["synapse_counts"] = synapseCounts;
        data["total_neurons"] = brain.getTotalNeuronCount();
        data["total_synapses"] = brain.getTotalSynapseCount();
        
        pImpl->callback("data", data);
    }
}

void VisualizationInterface::visualizeActivity(const Brain& brain,
                                               const std::string& title,
                                               const std::vector<std::string>& metrics) {
    if (!pImpl->active) {
        return;
    }
    
    // Collect activity data
    double firingRate = brain.getAverageFiringRate();
    size_t activeNeurons = brain.getActiveNeuronCount();
    size_t firingNeurons = brain.getFiringNeuronCount();
    double e_iRatio = brain.getExcitationInhibitionRatio();
    
    if (pImpl->debugEnabled) {
        NLM_LOG_INFO("Activity visualization: " + title);
        NLM_LOG_INFO("  Firing Rate: " + std::to_string(firingRate));
        NLM_LOG_INFO("  Active Neurons: " + std::to_string(activeNeurons));
        NLM_LOG_INFO("  Firing Neurons: " + std::to_string(firingNeurons));
    }
    
    // Trigger visualization callback
    if (pImpl->callback) {
        py::dict data;
        data["type"] = "activity";
        data["title"] = title;
        data["metrics"] = metrics;
        data["firing_rate"] = firingRate;
        data["active_neurons"] = activeNeurons;
        data["firing_neurons"] = firingNeurons;
        data["e_i_ratio"] = e_iRatio;
        
        pImpl->callback("data", data);
    }
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes,
                                            const std::string& title,
                                            double timeWindow,
                                            int maxNeurons,
                                            bool showInterSpikeIntervals) {
    if (!pImpl->active) {
        return;
    }
    
    // Collect spike data
    std::vector<double> spikeTimes;
    std::vector<int> neuronIds;
    double minTime = std::numeric_limits<double>::max();
    double maxTime = std::numeric_limits<double>::min();
    
    for (const auto& spike : spikes) {
        if (spike.time < minTime) minTime = spike.time;
        if (spike.time > maxTime) maxTime = spike.time;
        spikeTimes.push_back(spike.time);
        neuronIds.push_back(spike.neuronId.value);
    }
    
    if (pImpl->debugEnabled) {
        NLM_LOG_INFO("Spike visualization: " + title);
        NLM_LOG_INFO("  Total Spikes: " + std::to_string(spikes.size()));
        NLM_LOG_INFO("  Time Window: " + std::to_string(maxTime - minTime) + "s");
    }
    
    // Trigger visualization callback
    if (pImpl->callback) {
        py::dict data;
        data["type"] = "spikes";
        data["title"] = title;
        data["spike_times"] = spikeTimes;
        data["neuron_ids"] = neuronIds;
        data["min_time"] = minTime;
        data["max_time"] = maxTime;
        data["inter_spike_intervals"] = showInterSpikeIntervals;
        
        pImpl->callback("data", data);
    }
}

void VisualizationInterface::visualizeWeights(const Brain& brain,
                                              const std::string& title,
                                              bool showWeightDistribution,
                                              bool showExcitatoryInhibitory,
                                              double minWeight,
                                              double maxWeight) {
    if (!pImpl->active) {
        return;
    }
    
    // Collect weight data
    std::vector<float> weights;
    std::vector<float> excitatoryWeights;
    std::vector<float> inhibitoryWeights;
    
    for (const auto& region : brain.getRegions()) {
        for (const auto& synapse : region->getSynapses()) {
            float weight = synapse->getWeight();
            weights.push_back(weight);
            
            if (weight > 0) {
                excitatoryWeights.push_back(weight);
            } else {
                inhibitoryWeights.push_back(weight);
            }
        }
    }
    
    // Calculate statistics
    double avgWeight = 0.0, maxWeightVal = 0.0, minWeightVal = 0.0;
    if (!weights.empty()) {
        double sum = std::accumulate(weights.begin(), weights.end(), 0.0);
        avgWeight = sum / weights.size();
        maxWeightVal = *std::max_element(weights.begin(), weights.end());
        minWeightVal = *std::min_element(weights.begin(), weights.end());
    }
    
    if (pImpl->debugEnabled) {
        NLM_LOG_INFO("Weight visualization: " + title);
        NLM_LOG_INFO("  Total Synapses: " + std::to_string(weights.size()));
        NLM_LOG_INFO("  Average Weight: " + std::to_string(avgWeight));
        NLM_LOG_INFO("  Max Weight: " + std::to_string(maxWeightVal));
        NLM_LOG_INFO("  Min Weight: " + std::to_string(minWeightVal));
    }
    
    // Trigger visualization callback
    if (pImpl->callback) {
        py::dict data;
        data["type"] = "weights";
        data["title"] = title;
        data["weights"] = weights;
        data["excitatory_weights"] = excitatoryWeights;
        data["inhibitory_weights"] = inhibitoryWeights;
        data["show_weight_distribution"] = showWeightDistribution;
        data["show_excitatory_inhibitory"] = showExcitatoryInhibitory;
        data["min_weight"] = minWeight;
        data["max_weight"] = maxWeight;
        data["average_weight"] = avgWeight;
        data["max_weight_value"] = maxWeightVal;
        data["min_weight_value"] = minWeightVal;
        
        pImpl->callback("data", data);
    }
}

void VisualizationInterface::visualizeMemory(const Brain& brain,
                                             const std::string& title,
                                             bool showWorkingMemory,
                                             bool showEpisodicMemory,
                                             bool showAssociativeMemory) {
    if (!pImpl->active) {
        return;
    }
    
    if (pImpl->debugEnabled) {
        NLM_LOG_INFO("Memory visualization: " + title);
    }
    
    // Trigger visualization callback
    if (pImpl->callback) {
        py::dict data;
        data["type"] = "memory";
        data["title"] = title;
        data["show_working_memory"] = showWorkingMemory;
        data["show_episodic_memory"] = showEpisodicMemory;
        data["show_associative_memory"] = showAssociativeMemory;
        
        // Add memory system data if available
        if (brain.getWorkingMemory() && showWorkingMemory) {
            py::dict workingMemoryData;
            workingMemoryData["active_traces"] = brain.getWorkingMemory()->getActiveTraces();
            workingMemoryData["capacity"] = brain.getWorkingMemory()->getCapacity();
            data["working_memory"] = workingMemoryData;
        }
        
        if (brain.getEpisodicMemory() && showEpisodicMemory) {
            py::dict episodicMemoryData;
            episodicMemoryData["episode_count"] = brain.getEpisodicMemory()->getEpisodeCount();
            episodicMemoryData["max_episodes"] = brain.getEpisodicMemory()->getMaxEpisodes();
            data["episodic_memory"] = episodicMemoryData;
        }
        
        if (brain.getAssociativeMemory() && showAssociativeMemory) {
            // Add associative memory data if available
            py::dict associativeMemoryData;
            // TODO: Add actual associative memory data
            data["associative_memory"] = associativeMemoryData;
        }
        
        pImpl->callback("data", data);
    }
}

void VisualizationInterface::visualizeDevelopment(const Brain& brain,
                                                 const std::string& title) {
    if (!pImpl->active) {
        return;
    }
    
    if (pImpl->debugEnabled) {
        NLM_LOG_INFO("Development visualization: " + title);
    }
    
    // Trigger visualization callback
    if (pImpl->callback) {
        py::dict data;
        data["type"] = "development";
        data["title"] = title;
        data["developmental_stage"] = static_cast<int>(brain.getDevelopmentalStage());
        
        pImpl->callback("data", data);
    }
}

void VisualizationInterface::visualizeNeuromodulation(const Brain& brain,
                                                     const std::string& title,
                                                     bool showDopamine,
                                                     bool showCuriosity,
                                                     bool showNovelty,
                                                     bool showPredictionError) {
    if (!pImpl->active) {
        return;
    }
    
    if (pImpl->debugEnabled) {
        NLM_LOG_INFO("Neuromodulation visualization: " + title);
    }
    
    // Trigger visualization callback
    if (pImpl->callback) {
        py::dict data;
        data["type"] = "neuromodulation";
        data["title"] = title;
        data["show_dopamine"] = showDopamine;
        data["show_curiosity"] = showCuriosity;
        data["show_novelty"] = showNovelty;
        data["show_prediction_error"] = showPredictionError;
        
        pImpl->callback("data", data);
    }
}

void VisualizationInterface::visualizePerformance(const Brain& brain,
                                                  const std::string& title,
                                                  const std::vector<std::string>& metrics) {
    if (!pImpl->active) {
        return;
    }
    
    if (pImpl->debugEnabled) {
        NLM_LOG_INFO("Performance visualization: " + title);
    }
    
    // Collect performance data
    std::map<std::string, double> metricValues;
    
    if (pImpl->currentAnalysis == Analysis::Statistics || pImpl->currentAnalysis == Analysis::All) {
        metricValues["firing_rate"] = brain.getAverageFiringRate();
        metricValues["spike_count"] = brain.getTotalSpikeCount();
        metricValues["e_i_ratio"] = brain.getExcitationInhibitionRatio();
    }
    
    if (pImpl->currentAnalysis == Analysis::Dynamics || pImpl->currentAnalysis == Analysis::All) {
        metricValues["active_neurons"] = brain.getActiveNeuronCount();
        metricValues["firing_neurons"] = brain.getFiringNeuronCount();
    }
    
    if (pImpl->currentAnalysis == Analysis::Connectivity || pImpl->currentAnalysis == Analysis::All) {
        metricValues["region_count"] = brain.getRegionCount();
        metricValues["synapse_count"] = brain.getTotalSynapseCount();
    }
    
    // Trigger visualization callback
    if (pImpl->callback) {
        py::dict data;
        data["type"] = "performance";
        data["title"] = title;
        data["metrics"] = metrics;
        data["metric_values"] = metricValues;
        
        pImpl->callback("data", data);
    }
}

bool VisualizationInterface::exportData(const std::string& filepath, const std::string& format) {
    if (!pImpl->active) {
        return false;
    }
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
            return false;
        }
        
        if (format == "csv") {
            // Export as CSV
            file << "metric,value\n";
            file << "firing_rate," << brain.getAverageFiringRate() << "\n";
            file << "spike_count," << brain.getTotalSpikeCount() << "\n";
            file << "active_neurons," << brain.getActiveNeuronCount() << "\n";
            file << "firing_neurons," << brain.getFiringNeuronCount() << "\n";
        } else if (format == "json") {
            // Export as JSON (simplified)
            file << "{\n";
            file << "  \"firing_rate\": " << brain.getAverageFiringRate() << ",\n";
            file << "  \"spike_count\": " << brain.getTotalSpikeCount() << ",\n";
            file << "  \"active_neurons\": " << brain.getActiveNeuronCount() << ",\n";
            file << "  \"firing_neurons\": " << brain.getFiringNeuronCount() << "\n";
            file << "}\n";
        } else {
            NLM_LOG_ERROR("Unsupported export format: " + format);
            return false;
        }
        
        file.close();
        
        if (pImpl->debugEnabled) {
            NLM_LOG_INFO("Data exported to: " + filepath + " (format: " + format + ")");
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error exporting data to " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

std::string VisualizationInterface::generateReport(const Brain& brain) const {
    std::ostringstream report;
    
    report << "NLM Visualization Report" << std::endl;
    report << "=" << std::string(50, '=') << std::endl;
    report << "Generated: " << std::chrono::system_clock::now() << std::endl;
    report << "Brain Statistics:" << std::endl;
    report << "  Total Neurons: " << brain.getTotalNeuronCount() << std::endl;
    report << "  Total Synapses: " << brain.getTotalSynapseCount() << std::endl;
    report << "  Active Neurons: " << brain.getActiveNeuronCount() << std::endl;
    report << "  Firing Neurons: " << brain.getFiringNeuronCount() << std::endl;
    report << "  Average Firing Rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    report << "  Excitation/Inhibition Ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
    report << "  Developmental Stage: " << static_cast<int>(brain.getDevelopmentalStage()) << std::endl;
    
    report << std::endl;
    report << "Memory Systems:" << std::endl;
    if (brain.getWorkingMemory()) {
        report << "  Working Memory: " << brain.getWorkingMemory()->getActiveTraces() << 
               " active traces (capacity: " << brain.getWorkingMemory()->getCapacity() << ")" << std::endl;
    }
    if (brain.getEpisodicMemory()) {
        report << "  Episodic Memory: " << brain.getEpisodicMemory()->getEpisodeCount() << 
               " episodes (max: " << brain.getEpisodicMemory()->getMaxEpisodes() << ")" << std::endl;
    }
    
    report << std::endl;
    report << "Visualization Settings:" << std::endl;
    report << "  Mode: " << static_cast<int>(pImpl->currentMode) << std::endl;
    report << "  Analysis: " << static_cast<int>(pImpl->currentAnalysis) << std::endl;
    report << "  Update Rate: " << pImpl->updateRate << " Hz" << std::endl;
    report << "  Debug Enabled: " << (pImpl->debugEnabled ? "Yes" : "No") << std::endl;
    
    return report.str();
}

void VisualizationInterface::setCallback(VisualizationCallback callback) {
    pImpl->callback = callback;
}

bool VisualizationInterface::saveConfig(const std::string& filepath) {
    if (!pImpl->active) {
        return false;
    }
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for writing config: " + filepath);
            return false;
        }
        
        file << "mode=" << static_cast<int>(pImpl->currentMode) << std::endl;
        file << "analysis=" << static_cast<int>(pImpl->currentAnalysis) << std::endl;
        file << "update_rate=" << pImpl->updateRate << std::endl;
        file << "debug_enabled=" << (pImpl->debugEnabled ? "1" : "0") << std::endl;
        
        file.close();
        
        if (pImpl->debugEnabled) {
            NLM_LOG_INFO("Configuration saved to: " + filepath);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving configuration to " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool VisualizationInterface::loadConfig(const std::string& filepath) {
    if (!pImpl->active) {
        return false;
    }
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for reading config: " + filepath);
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                
                if (key == "mode") {
                    pImpl->currentMode = static_cast<Mode>(std::stoi(value));
                } else if (key == "analysis") {
                    pImpl->currentAnalysis = static_cast<Analysis>(std::stoi(value));
                } else if (key == "update_rate") {
                    pImpl->updateRate = std::stod(value);
                } else if (key == "debug_enabled") {
                    pImpl->debugEnabled = (value == "1");
                }
            }
        }
        
        file.close();
        
        if (pImpl->debugEnabled) {
            NLM_LOG_INFO("Configuration loaded from: " + filepath);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading configuration from " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

} // namespace nlm
