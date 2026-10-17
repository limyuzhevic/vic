#include "visualization/VisualizationManager.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

void VisualizationManager::enableFileLogging(const std::string& directory) {
    // Create output files
    std::string activityFile = directory + "/nlm_activity.csv";
    std::string performanceFile = directory + "/nlm_performance.csv";
    std::string eventFile = directory + "/nlm_events.csv";
    
    activityLogFile.open(activityFile, std::ios::out | std::ios::trunc);
    performanceLogFile.open(performanceFile, std::ios::out | std::ios::trunc);
    eventLogFile.open(eventFile, std::ios::out | std::ios::trunc);
    
    if (activityLogFile.is_open()) {
        activityLogFile << "timestamp,neuron_activity,total_neurons,total_synapses,firing_neurons,total_spikes,avg_firing_rate,e_i_ratio,active_memory,episodic_memory,development_stage\n";
    }
    if (performanceLogFile.is_open()) {
        performanceLogFile << "timestamp,total_steps,total_neurons,total_synapses,firing_neurons,total_spikes,avg_firing_rate,e_i_ratio,memory_traces,episodic_episodes,connections,plasticity_rate\n";
    }
    if (eventLogFile.is_open()) {
        eventLogFile << "timestamp,event_type,description,metadata\n";
    }
    
    logger->info("File logging enabled: " + directory);
}

void VisualizationManager::collectPerformanceData(const Brain& brain) {
    PerformanceMetrics metrics;
    metrics.timestamp = std::chrono::system_clock::now();
    metrics.totalSteps = totalStepsProcessed;
    metrics.totalNeurons = brain.getTotalNeuronCount();
    metrics.totalSynapses = brain.getTotalSynapseCount();
    metrics.firingNeurons = brain.getFiringNeuronCount();
    metrics.totalSpikeCount = brain.getTotalSpikeCount();
    metrics.averageFiringRate = brain.getAverageFiringRate();
    metrics.excitationInhibitionRatio = brain.getExcitationInhibitionRatio();
    
    if (brain.getWorkingMemory()) {
        metrics.activeMemoryTraces = brain.getWorkingMemory()->getActiveTraces();
    } else {
        metrics.activeMemoryTraces = 0;
    }
    
    if (brain.getEpisodicMemory()) {
        metrics.episodicMemoryEpisodes = brain.getEpisodicMemory()->getEpisodeCount();
    } else {
        metrics.episodicMemoryEpisodes = 0;
    }
    
    metrics.interRegionConnections = 0;  // TODO: Implement if available
    metrics.developmentalStage = static_cast<size_t>(brain.getDevelopmentalStage());
    
    metrics.plasticityRate = 1.0f;  // TODO: Get from development system
    
    performanceHistory.push_back(metrics);
    
    // Write to file if logging is enabled
    if (performanceLogFile.is_open()) {
        performanceLogFile << formatTimestamp(metrics.timestamp) << ","
                         << metrics.totalSteps << ","
                         << metrics.totalNeurons << ","
                         << metrics.totalSynapses << ","
                         << metrics.firingNeurons << ","
                         << metrics.totalSpikeCount << ","
                         << metrics.averageFiringRate << ","
                         << metrics.excitationInhibitionRatio << ","
                         << metrics.activeMemoryTraces << ","
                         << metrics.episodicMemoryEpisodes << ","
                         << metrics.interRegionConnections << ","
                         << metrics.plasticityRate << "\n";
    }
    
    totalStepsProcessed++;
}

void VisualizationManager::collectNeuralActivity(const Brain& brain) {
    NeuralActivitySnapshot snapshot(brain.getTotalNeuronCount());
    snapshot.timestamp = std::chrono::system_clock::now();
    
    // Collect neural activity data
    for (size_t i = 0; i < brain.getTotalNeuronCount(); ++i) {
        // TODO: Get actual neuron state from brain
        // This would require accessing neurons, which may need API improvements
        snapshot.neuronActivity[i] = 0.0f;  // Placeholder
        snapshot.neuronTypes[i] = "Internal";  // Placeholder
        snapshot.regionIds[i] = i % 3 + 1;  // Placeholder
        
        if (i % 7 == 0) {  // Simulate some active neurons
            snapshot.neuronActivity[i] = (i % 5 + 1) * 0.2f;
            snapshot.firingCounts[i] = (i % 3) + 1;
        }
    }
    
    activityHistory.push_back(snapshot);
    
    // Write to file if logging is enabled
    if (activityLogFile.is_open()) {
        activityLogFile << formatTimestamp(snapshot.timestamp) << ",";
        for (size_t i = 0; i < snapshot.neuronActivity.size(); ++i) {
            if (i > 0) activityLogFile << ",";
            activityLogFile << snapshot.neuronActivity[i];
        }
        activityLogFile << "," << snapshot.totalNeurons() << ","
                       << "PLACEHOLDER" << ","
                       << "PLACEHOLDER" << ","
                       << "PLACEHOLDER" << ","
                       << "PLACEHOLDER" << ","
                       << static_cast<int>(snapshot.stage) << "\n";
    }
}

void VisualizationManager::collectMemoryActivity(const Brain& brain) {
    MemoryActivitySnapshot memory;
    memory.timestamp = std::chrono::system_clock::now();
    
    if (brain.getWorkingMemory()) {
        memory.workingMemoryNeurons = brain.getWorkingMemory()->getMemoryNeurons();
        memory.workingMemoryActivations = brain.getWorkingMemory()->getActivationLevels();
    }
    
    if (brain.getEpisodicMemory()) {
        memory.episodicMemoryCapacity = brain.getEpisodicMemory()->getMaxEpisodes();
        memory.episodicMemoryCurrentSize = brain.getEpisodicMemory()->getEpisodeCount();
        memory.episodicMemoryImportance = brain.getEpisodicMemory()->getImportanceScores();
    }
    
    if (brain.getAssociativeMemory()) {
        memory.associativeMemoryCapacity = brain.getAssociativeMemory()->getMaxCapacity();
        memory.associativeMemoryCurrentSize = brain.getAssociativeMemory()->getCurrentSize();
    }
    
    if (brain.getDopamine()) {
        memory.dopamineLevel = brain.getDopamine()->getLevel();
    }
    if (brain.getCuriosity()) {
        memory.curiosityLevel = brain.getCuriosity()->getLevel();
    }
    if (brain.getNovelty()) {
        memory.noveltyLevel = brain.getNovelty()->getLevel();
    }
    if (brain.getPredictionErrorSignal()) {
        memory.predictionError = brain.getPredictionErrorSignal()->getLevel();
    }
    
    memoryHistory.push_back(memory);
}

void VisualizationManager::collectDevelopmentProgress(const Brain& brain) {
    DevelopmentProgress dev;
    dev.timestamp = std::chrono::system_clock::now();
    dev.stage = brain.getDevelopmentalStage();
    dev.plasticityRate = 1.0f;  // TODO: Get from development system
    dev.synaptogenesisProgress = 0.5f;  // TODO: Get actual progress
    dev.pruningProgress = 0.3f;  // TODO: Get actual progress
    dev.maturationProgress = 0.7f;  // TODO: Get actual progress
    dev.stabilityLevel = 0.8f;  // TODO: Get actual stability
    
    developmentHistory.push_back(dev);
}

void VisualizationManager::logNeuralEvent(NeuralEvent::EventType type, const std::string& description) {
    logNeuralEvent(type, description, std::unordered_map<std::string, std::string>());
}

void VisualizationManager::logNeuralEvent(NeuralEvent::EventType type, const std::string& description,
                                         const std::unordered_map<std::string, std::string>& metadata) {
    NeuralEvent event(type, description);
    event.metadata = metadata;
    eventLog.push_back(event);
    
    // Write to file if logging is enabled
    if (eventLogFile.is_open()) {
        eventLogFile << formatTimestamp(event.timestamp) << ","
                    << static_cast<int>(event.type) << ","
                    << event.description << ",";
        
        bool first = true;
        for (const auto& pair : event.metadata) {
            if (!first) eventLogFile << ";";
            eventLogFile << pair.first << "=" << pair.second;
            first = false;
        }
        eventLogFile << "\n";
    }
}

double VisualizationManager::getAverageFiringRate() const {
    if (performanceHistory.empty()) return 0.0;
    
    double total = 0.0;
    for (const auto& metrics : performanceHistory) {
        total += metrics.averageFiringRate;
    }
    return total / performanceHistory.size();
}

size_t VisualizationManager::getMaxFiringNeurons() const {
    if (performanceHistory.empty()) return 0;
    
    size_t max = 0;
    for (const auto& metrics : performanceHistory) {
        if (metrics.firingNeurons > max) {
            max = metrics.firingNeurons;
        }
    }
    return max;
}

float VisualizationManager::getAveragePlasticity() const {
    if (developmentHistory.empty()) return 0.0f;
    
    float total = 0.0f;
    for (const auto& dev : developmentHistory) {
        total += dev.plasticityRate;
    }
    return total / developmentHistory.size();
}

size_t VisualizationManager::getTotalEventsLogged() const {
    return eventLog.size();
}

std::chrono::duration<double> VisualizationManager::getUptime() const {
    return std::chrono::system_clock::now() - startTime;
}

void VisualizationManager::printActivitySummary() const {
    std::cout << "=== Neural Activity Summary ===" << std::endl;
    std::cout << "Total snapshots collected: " << activityHistory.size() << std::endl;
    if (!activityHistory.empty()) {
        std::cout << "Latest activity timestamp: " << formatTimestamp(activityHistory.back().timestamp) << std::endl;
        std::cout << "Average neuron activity: " << getAverageActivityLevel() << std::endl;
    }
    std::cout << std::endl;
}

void VisualizationManager::printPerformanceSummary() const {
    std::cout << "=== Performance Summary ===" << std::endl;
    std::cout << "Total performance samples: " << performanceHistory.size() << std::endl;
    std::cout << "Average firing rate: " << getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "Max firing neurons: " << getMaxFiringNeurons() << std::endl;
    std::cout << "Average plasticity rate: " << getAveragePlasticity() << std::endl;
    std::cout << std::endl;
}

void VisualizationManager::printMemorySummary() const {
    std::cout << "=== Memory Summary ===" << std::endl;
    std::cout << "Memory activity samples: " << memoryHistory.size() << std::endl;
    if (!memoryHistory.empty()) {
        const auto& latest = memoryHistory.back();
        std::cout << "Current working memory traces: " << latest.workingMemoryNeurons.size() << std::endl;
        std::cout << "Episodic memory usage: " << latest.episodicMemoryCurrentSize << "/" << latest.episodicMemoryCapacity << std::endl;
        std::cout << "Dopamine level: " << latest.dopamineLevel << std::endl;
        std::cout << "Curiosity level: " << latest.curiosityLevel << std::endl;
    }
    std::cout << std::endl;
}

void VisualizationManager::printDevelopmentSummary() const {
    std::cout << "=== Development Summary ===" << std::endl;
    std::cout << "Development progress samples: " << developmentHistory.size() << std::endl;
    if (!developmentHistory.empty()) {
        const auto& latest = developmentHistory.back();
        std::cout << "Current developmental stage: " << static_cast<int>(latest.stage) << std::endl;
        std::cout << "Plasticity rate: " << latest.plasticityRate << std::endl;
        std::cout << "Synaptogenesis progress: " << latest.synaptogenesisProgress << std::endl;
        std::cout << "Pruning progress: " << latest.pruningProgress << std::endl;
        std::cout << "Maturation progress: " << latest.maturationProgress << std::endl;
        std::cout << "Stability level: " << latest.stabilityLevel << std::endl;
    }
    std::cout << std::endl;
}

double VisualizationManager::getAverageActivityLevel() const {
    if (activityHistory.empty()) return 0.0;
    
    double total = 0.0;
    size_t count = 0;
    for (const auto& snapshot : activityHistory) {
        for (float activity : snapshot.neuronActivity) {
            total += activity;
            count++;
        }
    }
    return count > 0 ? total / count : 0.0;
}

void VisualizationManager::integrateWithBrain(Brain& brain) {
    // Set up event handlers for brain activities
    activeConnections++;  // This would connect to actual brain events in a real implementation
    
    logger->info("Visualization manager integrated with brain.");
}

void VisualizationManager::exportConfig(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        logger->error("Failed to open config file for export: " + filepath);
        return;
    }
    
    file << "NLM Visualization Configuration\n";
    file << "======================\n\n";
    file << "Performance Sampling: Every " << performanceSamplingInterval << " steps\n";
    file << "Activity Sampling: Every " << activitySamplingInterval << " steps\n";
    file << "Event Sampling: Every " << eventSamplingInterval << " steps\n\n";
    
    file << "File Logging: ";
    if (activityLogFile.is_open() || performanceLogFile.is_open() || eventLogFile.is_open()) {
        file << "Enabled\n";
        file << "Activity Log: nlm_activity.csv\n";
        file << "Performance Log: nlm_performance.csv\n";
        file << "Event Log: nlm_events.csv\n";
    } else {
        file << "Disabled\n";
    }
    
    file << "\nData Collected:\n";
    file << "  Performance samples: " << performanceHistory.size() << "\n";
    file << "  Activity snapshots: " << activityHistory.size() << "\n";
    file << "  Memory samples: " << memoryHistory.size() << "\n";
    file << "  Development samples: " << developmentHistory.size() << "\n";
    file << "  Events logged: " << eventLog.size() << "\n";
    
    file.close();
    
    logger->info("Configuration exported to: " + filepath);
}

void VisualizationManager::exportToCSV(const std::string& filepath) const {
    // Implementation for CSV export
    // This would be more sophisticated in a real implementation
    std::ofstream file(filepath);
    if (!file.is_open()) {
        logger->error("Failed to export CSV data to: " + filepath);
        return;
    }
    
    // Write header and data based on file type
    if (filepath.find("activity") != std::string::npos) {
        file << "timestamp,neuron_activity,total_neurons,total_synapses,firing_neurons,total_spikes,avg_firing_rate,e_i_ratio,active_memory,episodic_memory,development_stage\n";
        for (const auto& snapshot : activityHistory) {
            file << formatTimestamp(snapshot.timestamp) << ","
                 << "PLACEHOLDER_ACTIVITY" << ","
                 << "PLACEHOLDER_NEURONS" << ","
                 << "PLACEHOLDER_SYNAPSES" << ","
                 << "PLACEHOLDER_FIRING" << ","
                 << "PLACEHOLDER_SPIKES" << ","
                 << "PLACEHOLDER_RATE" << ","
                 << "PLACEHOLDER_EI" << ","
                 << "PLACEHOLDER_MEMORY" << ","
                 << "PLACEHOLDER_EPISODIC" << ","
                 << static_cast<int>(snapshot.stage) << "\n";
        }
    } else if (filepath.find("performance") != std::string::npos) {
        file << "timestamp,total_steps,total_neurons,total_synapses,firing_neurons,total_spikes,avg_firing_rate,e_i_ratio,memory_traces,episodic_episodes,connections,plasticity_rate\n";
        for (const auto& metrics : performanceHistory) {
            file << formatTimestamp(metrics.timestamp) << ","
                 << metrics.totalSteps << ","
                 << metrics.totalNeurons << ","
                 << metrics.totalSynapses << ","
                 << metrics.firingNeurons << ","
                 << metrics.totalSpikeCount << ","
                 << metrics.averageFiringRate << ","
                 << metrics.excitationInhibitionRatio << ","
                 << metrics.activeMemoryTraces << ","
                 << metrics.episodicMemoryEpisodes << ","
                 << metrics.interRegionConnections << ","
                 << metrics.plasticityRate << "\n";
        }
    } else if (filepath.find("events") != std::string::npos) {
        file << "timestamp,event_type,description,metadata\n";
        for (const auto& event : eventLog) {
            file << formatTimestamp(event.timestamp) << ","
                 << static_cast<int>(event.type) << ","
                 << event.description << ",";
            bool first = true;
            for (const auto& pair : event.metadata) {
                if (!first) file << ";";
                file << pair.first << "=" << pair.second;
                first = false;
            }
            file << "\n";
        }
    }
    
    file.close();
    
    logger->info("Data exported to CSV: " + filepath);
}

void VisualizationManager::exportToJSON(const std::string& filepath) const {
    // Implementation for JSON export
    // This would be more sophisticated in a real implementation
    std::ofstream file(filepath);
    if (!file.is_open()) {
        logger->error("Failed to export JSON data to: " + filepath);
        return;
    }
    
    file << "{\n";
    file << "  \"nlm_data\": {\n";
    file << "    \"performance_history\": [\n";
    for (size_t i = 0; i < performanceHistory.size(); ++i) {
        const auto& metrics = performanceHistory[i];
        file << "      {\n";
        file << "        \"timestamp\": \"" << formatTimestamp(metrics.timestamp) << "\",
";
        file << "        \"total_steps\": " << metrics.totalSteps << ",\n";
        file << "        \"total_neurons\": " << metrics.totalNeurons << ",\n";
        file << "        \"total_synapses\": " << metrics.totalSynapses << ",\n";
        file << "        \"firing_neurons\": " << metrics.firingNeurons << ",\n";
        file << "        \"total_spikes\": " << metrics.totalSpikeCount << ",\n";
        file << "        \"average_firing_rate\": " << metrics.averageFiringRate << ",\n";
        file << "        \"excitation_inhibition_ratio\": " << metrics.excitationInhibitionRatio << "\n";
        if (i < performanceHistory.size() - 1) file << "      },\n";
        else file << "      }\n";
    }
    file << "    ],\n";
    file << "    \"activity_history\": [],\n";
    file << "    \"memory_history\": [],\n";
    file << "    \"development_history\": [],\n";
    file << "    \"event_log\": []\n";
    file << "  }\n";
    file << "}\n";
    
    file.close();
    
    logger->info("Data exported to JSON: " + filepath);
}

void VisualizationManager::exportToPython(const std::string& filepath) const {
    // Implementation for Python export
    // This would be more sophisticated in a real implementation
    std::ofstream file(filepath);
    if (!file.is_open()) {
        logger->error("Failed to export Python data to: " + filepath);
        return;
    }
    
    file << "# NLM Data Export for Python Processing\n";
    file << "# Generated by NLM Visualization System\n\n";
    
    file << "import numpy as np\n";
    file << "import pandas as pd\n";
    file << "import matplotlib.pyplot as plt\n";
    file << "from datetime import datetime\n\n";
    
    file << "# Performance Data\n";
    file << "performance_data = {\n";
    file << "  'timestamps': [";
    for (size_t i = 0; i < performanceHistory.size(); ++i) {
        if (i > 0) file << ", ";
        file << "'" << formatTimestamp(performanceHistory[i].timestamp) << "'";
    }
    file << "],\n";
    
    file << "  'total_steps': [";
    for (size_t i = 0; i < performanceHistory.size(); ++i) {
        if (i > 0) file << ", ";
        file << performanceHistory[i].totalSteps;
    }
    file << "],\n";
    
    file << "  'average_firing_rate': [";
    for (size_t i = 0; i < performanceHistory.size(); ++i) {
        if (i > 0) file << ", ";
        file << performanceHistory[i].averageFiringRate;
    }
    file << "],\n";
    file << "}\n\n";
    
    file.close();
    
    logger->info("Data exported to Python format: " + filepath);
}

std::string VisualizationManager::formatTimestamp(std::chrono::system_clock::time_point tp) const {
    auto time = std::chrono::system_clock::to_time_t(tp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Neural Activity Visualization implementation
void NeuralVisualizer::printActivityHeatmap(const NeuralActivitySnapshot& snapshot) {
    std::cout << "Activity Heatmap:" << std::endl;
    for (size_t i = 0; i < snapshot.neuronActivity.size(); ++i) {
        std::cout << "Neuron " << i << ": ";
        int bars = static_cast<int>(snapshot.neuronActivity[i] * 50);
        for (int j = 0; j < bars; ++j) {
            std::cout << "█";
        }
        std::cout << " (" << snapshot.neuronActivity[i] << ")" << std::endl;
    }
}

void NeuralVisualizer::printRegionActivity(const NeuralActivitySnapshot& snapshot) {
    std::cout << "Activity by Region:" << std::endl;
    std::unordered_map<int, std::vector<float>> regionActivities;
    
    for (size_t i = 0; i < snapshot.neuronActivity.size(); ++i) {
        int regionId = snapshot.regionIds[i];
        if (regionId >= 0) {
            regionActivities[regionId].push_back(snapshot.neuronActivity[i]);
        }
    }
    
    for (const auto& pair : regionActivities) {
        float avgActivity = 0.0f;
        for (float activity : pair.second) {
            avgActivity += activity;
        }
        avgActivity /= pair.second.size();
        
        std::cout << "  Region " << pair.first << ": avg activity = " << avgActivity 
                  << " (" << pair.second.size() << " neurons)" << std::endl;
    }
}

void NeuralVisualizer::printSynapticStrengths(const NeuralActivitySnapshot& snapshot) {
    std::cout << "Synaptic Strengths:" << std::endl;
    for (size_t i = 0; i < snapshot.neuronActivity.size(); ++i) {
        std::cout << "  Neuron " << i << ": " << snapshot.neuronActivity[i] << std::endl;
    }
}

void NeuralVisualizer::printFiringPatterns(const NeuralActivitySnapshot& snapshot) {
    std::cout << "Firing Patterns:" << std::endl;
    for (size_t i = 0; i < snapshot.neuronActivity.size(); ++i) {
        if (snapshot.firingCounts[i] > 0) {
            std::cout << "  Neuron " << i << ": fired " << snapshot.firingCounts[i] 
                      << " times (activity = " << snapshot.neuronActivity[i] << ")" << std::endl;
        }
    }
}

std::string NeuralVisualizer::generateActivityPlot(const NeuralActivitySnapshot& snapshot) {
    return "Activity plot would be generated here for neurons: " + std::to_string(snapshot.neuronActivity.size());
}

std::string NeuralVisualizer::generateRegionPlot(const NeuralActivitySnapshot& snapshot) {
    return "Region plot would be generated here for regions: " + std::to_string(snapshot.regionIds.size());
}

// Performance Visualization implementation
void PerformanceVisualizer::printPerformanceChart(const std::vector<PerformanceMetrics>& history) {
    std::cout << "Performance Chart:" << std::endl;
    std::cout << "  Steps | Firing Rate | Memory Usage" << std::endl;
    std::cout << "  ------|-------------|-------------" << std::endl;
    
    for (size_t i = 0; i < history.size(); ++i) {
        if (i % 5 == 0 || i == history.size() - 1) {  // Sample every 5th point
            std::cout << std::setw(6) << history[i].totalSteps << " | "
                      << std::setw(11) << history[i].averageFiringRate << " | "
                      << std::setw(11) << history[i].activeMemoryTraces << std::endl;
        }
    }
}

void PerformanceVisualizer::printMemoryUsage(const std::vector<PerformanceMetrics>& history) {
    std::cout << "Memory Usage Over Time:" << std::endl;
    size_t maxMemory = 0;
    for (const auto& metrics : history) {
        if (metrics.activeMemoryTraces > maxMemory) {
            maxMemory = metrics.activeMemoryTraces;
        }
    }
    
    std::cout << "  Maximum memory traces: " << maxMemory << std::endl;
    std::cout << "  Average memory traces: " << getAverageMemoryUsage(history) << std::endl;
}

void PerformanceVisualizer::printSpikeRate(const std::vector<PerformanceMetrics>& history) {
    std::cout << "Spike Rate Over Time:" << std::endl;
    float maxRate = 0.0f;
    for (const auto& metrics : history) {
        if (metrics.averageFiringRate > maxRate) {
            maxRate = metrics.averageFiringRate;
        }
    }
    
    std::cout << "  Maximum spike rate: " << maxRate << " Hz" << std::endl;
    std::cout << "  Average spike rate: " << getAverageSpikeRate(history) << " Hz" << std::endl;
}

void PerformanceVisualizer::printPlasticityOverTime(const std::vector<DevelopmentProgress>& history) {
    std::cout << "Plasticity Over Time:" << std::endl;
    float maxPlasticity = 0.0f;
    for (const auto& dev : history) {
        if (dev.plasticityRate > maxPlasticity) {
            maxPlasticity = dev.plasticityRate;
        }
    }
    
    std::cout << "  Maximum plasticity rate: " << maxPlasticity << std::endl;
    std::cout << "  Average plasticity rate: " << getAveragePlasticityRate(history) << std::endl;
}

std::string PerformanceVisualizer::generatePerformanceReport(const std::vector<PerformanceMetrics>& history) {
    std::stringstream ss;
    ss << "Performance Report\n";
    ss << "==================\n\n";
    ss << "Total samples: " << history.size() << "\n";
    ss << "Time span: " << (history.empty() ? 0 : (history.back().totalSteps - history.front().totalSteps)) << " steps\n";
    ss << "Average firing rate: " << getAverageFiringRate(history) << " Hz\n";
    ss << "Max firing neurons: " << getMaxFiringNeurons(history) << "\n";
    ss << "Max spike rate: " << getMaxSpikeRate(history) << " Hz\n";
    
    return ss.str();
}

std::string PerformanceVisualizer::generateMemoryReport(const std::vector<PerformanceMetrics>& history) {
    std::stringstream ss;
    ss << "Memory Report\n";
    ss << "============\n\n";
    ss << "Average memory traces: " << getAverageMemoryUsage(history) << "\n";
    ss << "Peak memory usage: " << getMaxMemoryUsage(history) << "\n";
    ss << "Memory efficiency: " << calculateMemoryEfficiency(history) << "\n";
    
    return ss.str();
}

float PerformanceVisualizer::getAverageFiringRate(const std::vector<PerformanceMetrics>& history) {
    if (history.empty()) return 0.0f;
    float total = 0.0f;
    for (const auto& metrics : history) {
        total += metrics.averageFiringRate;
    }
    return total / history.size();
}

size_t PerformanceVisualizer::getMaxFiringNeurons(const std::vector<PerformanceMetrics>& history) {
    if (history.empty()) return 0;
    size_t max = 0;
    for (const auto& metrics : history) {
        if (metrics.firingNeurons > max) {
            max = metrics.firingNeurons;
        }
    }
    return max;
}

float PerformanceVisualizer::getAverageMemoryUsage(const std::vector<PerformanceMetrics>& history) {
    if (history.empty()) return 0.0f;
    float total = 0.0f;
    for (const auto& metrics : history) {
        total += metrics.activeMemoryTraces;
    }
    return total / history.size();
}

size_t PerformanceVisualizer::getMaxMemoryUsage(const std::vector<PerformanceMetrics>& history) {
    if (history.empty()) return 0;
    size_t max = 0;
    for (const auto& metrics : history) {
        if (metrics.activeMemoryTraces > max) {
            max = metrics.activeMemoryTraces;
        }
    }
    return max;
}

float PerformanceVisualizer::getAverageSpikeRate(const std::vector<PerformanceMetrics>& history) {
    return getAverageFiringRate(history);  // For now, use same as average firing rate
}

float PerformanceVisualizer::getMaxSpikeRate(const std::vector<PerformanceMetrics>& history) {
    float max = 0.0f;
    for (const auto& metrics : history) {
        if (metrics.averageFiringRate > max) {
            max = metrics.averageFiringRate;
        }
    }
    return max;
}

float PerformanceVisualizer::getAveragePlasticityRate(const std::vector<DevelopmentProgress>& history) {
    if (history.empty()) return 0.0f;
    float total = 0.0f;
    for (const auto& dev : history) {
        total += dev.plasticityRate;
    }
    return total / history.size();
}

float PerformanceVisualizer::calculateMemoryEfficiency(const std::vector<PerformanceMetrics>& history) {
    if (history.empty()) return 0.0f;
    
    float totalMemory = 0.0f;
    float totalActivity = 0.0f;
    for (const auto& metrics : history) {
        totalMemory += metrics.activeMemoryTraces;
        totalActivity += metrics.averageFiringRate;
    }
    
    return totalActivity / (totalMemory + 1.0f);  // Avoid division by zero
}

} // namespace nlm
