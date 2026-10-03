#include "VisualizationUtils.hpp"
#include "BrainVisualizer.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>

namespace nlm {

std::string formatNeuralNetworkASCII(const std::vector<NeuronVisualizationData>& neurons,
                                    const std::vector<SynapseVisualizationData>& synapses,
                                    const std::string& colorScheme,
                                    int detailLevel) {
    std::stringstream ss;
    
    ss << "Neural Network Visualization" << std::endl;
    ss << "===========================" << std::endl;
    ss << "Total Neurons: " << neurons.size() << std::endl;
    ss << "Total Synapses: " << synapses.size() << std::endl;
    ss << std::endl;
    
    // Group neurons by type and region
    std::map<NeuronType, std::vector<const NeuronVisualizationData*>> neuronsByType;
    for (const auto& neuron : neurons) {
        neuronsByType[neuron.type].push_back(&neuron);
    }
    
    // Display neurons by type
    ss << "Neurons by Type:" << std::endl;
    for (const auto& [type, neuronList] : neuronsByType) {
        ss << "  " << neuronTypeToString(type) << ": " << neuronList.size() << std::endl;
        
        if (detailLevel >= 2) {
            ss << "    Active: " << countActiveNeurons(neuronList) << std::endl;
            ss << "    Firing Rate (avg): " << calculateAverageFiringRate(neuronList) << " Hz" << std::endl;
        }
    }
    
    ss << std::endl;
    
    // Display synapse statistics
    ss << "Synapse Statistics:" << std::endl;
    ss << "  Total: " << synapses.size() << std::endl;
    
    if (!synapses.empty()) {
        float totalWeight = 0.0f;
        int positiveSynapses = 0, negativeSynapses = 0;
        
        for (const auto& synapse : synapses) {
            totalWeight += synapse.weight;
            if (synapse.weight > 0.0f) positiveSynapses++;
            else if (synapse.weight < 0.0f) negativeSynapses++;
        }
        
        ss << "  Average Weight: " << (totalWeight / synapses.size()) << std::endl;
        ss << "  Positive Synapses: " << positiveSynapses << " (" 
           << (positiveSynapses * 100.0f / synapses.size()) << "%)" << std::endl;
        ss << "  Negative Synapses: " << negativeSynapses << " (" 
           << (negativeSynapses * 100.0f / synapses.size()) << "%)" << std::endl;
    }
    
    return ss.str();
}

std::string formatMemoryASCII(const MemoryVisualizationData& memory,
                             int detailLevel) {
    std::stringstream ss;
    
    ss << "Memory System: " << memory.type << std::endl;
    ss << "=================" << std::endl;
    ss << "Active Traces: " << memory.traces.size() << std::endl;
    ss << "Patterns: " << memory.patterns.size() << std::endl;
    ss << "Activation Level: " << memory.activationLevel << std::endl;
    ss << "System Strength: " << memory.strength << std::endl;
    ss << std::endl;
    
    if (detailLevel >= 2 && !memory.traces.empty()) {
        ss << "Recent Traces:" << std::endl;
        for (size_t i = 0; i < std::min(size_t(5), memory.traces.size()); ++i) {
            ss << "  " << i << ". ID: " << memory.traces[i].id 
               << " Priority: " << memory.traces[i].priority 
               << " Strength: " << memory.traces[i].strength << std::endl;
        }
    }
    
    return ss.str();
}

std::string formatNeuromodulationASCII(const NeuromodulationVisualizationData& neuromodulator,
                                       int detailLevel) {
    std::stringstream ss;
    
    ss << "Neuromodulator: " << neuromodulator.type << std::endl;
    ss << "=======================" << std::endl;
    ss << "Concentration: " << neuromodulator.concentration << std::endl;
    ss << "Effect: " << neuromodulator.effect << std::endl;
    ss << "Target Level: " << neuromodulator.targetLevel << std::endl;
    ss << "Modulating: " << (neuromodulator.isModulating ? "Yes" : "No") << std::endl;
    ss << std::endl;
    
    if (detailLevel >= 2) {
        ss << "Recent History:" << std::endl;
        for (int i = 0; i < 10; ++i) {
            ss << "  " << i << ". " << neuromodulator.history[i] << std::endl;
        }
    }
    
    return ss.str();
}

std::string formatCognitiveASCII(const CognitiveVisualizationData& cognitive,
                                 int detailLevel) {
    std::stringstream ss;
    
    ss << "Cognitive System: " << cognitive.type << std::endl;
    ss << "====================" << std::endl;
    ss << "Activation: " << cognitive.activation << std::endl;
    ss << "Confidence: " << cognitive.confidence << std::endl;
    ss << "Focus Level: " << cognitive.focusLevel << std::endl;
    ss << "Active Concepts: " << cognitive.activeConcepts.size() << std::endl;
    ss << "Planned Actions: " << cognitive.plannedActions.size() << std::endl;
    ss << std::endl;
    
    if (detailLevel >= 2) {
        ss << "Active Concepts:" << std::endl;
        for (size_t i = 0; i < std::min(size_t(3), cognitive.activeConcepts.size()); ++i) {
            ss << "  " << i << ". " << cognitive.activeConcepts[i] << std::endl;
        }
        
        ss << "Planned Actions:" << std::endl;
        for (size_t i = 0; i < std::min(size_t(3), cognitive.plannedActions.size()); ++i) {
            ss << "  " << i << ". " << cognitive.plannedActions[i] << std::endl;
        }
    }
    
    return ss.str();
}

std::string formatDevelopmentASCII(const DevelopmentVisualizationData& development,
                                   int detailLevel) {
    std::stringstream ss;
    
    ss << "Development System" << std::endl;
    ss << "===================" << std::endl;
    ss << "Stage: " << developmentalStageToString(development.stage) << std::endl;
    ss << "Plasticity Rate: " << development.plasticityRate << std::endl;
    ss << "Maturation Level: " << development.maturationLevel << std::endl;
    ss << "Capability Level: " << development.capabilityLevel << std::endl;
    ss << "Age: " << development.age << std::endl;
    ss << std::endl;
    
    if (detailLevel >= 2 && !development.capabilityHistory.empty()) {
        ss << "Capability Progress:" << std::endl;
        for (size_t i = 0; i < std::min(size_t(10), development.capabilityHistory.size()); ++i) {
            ss << "  " << i << ". " << development.capabilityHistory[i] << std::endl;
        }
    }
    
    return ss.str();
}

std::string formatPerformanceASCII(const PerformanceMetricsVisualizationData& metrics,
                                   int detailLevel) {
    std::stringstream ss;
    
    ss << "Performance Metrics" << std::endl;
    ss << "====================" << std::endl;
    ss << "Average Firing Rate: " << metrics.averageFiringRate << " Hz" << std::endl;
    ss << "Spike Count: " << metrics.spikeCount << std::endl;
    ss << "Learning Progress: " << metrics.learningProgress << std::endl;
    ss << "Memory Retention: " << metrics.memoryRetention << std::endl;
    ss << "Prediction Accuracy: " << metrics.predictionAccuracy << std::endl;
    ss << "Exploration Rate: " << metrics.explorationRate << std::endl;
    ss << std::endl;
    
    if (detailLevel >= 2 && !metrics.timeSeriesData.empty()) {
        ss << "Recent Time Series:" << std::endl;
        size_t historySize = std::min(size_t(10), metrics.timeSeriesData.size());
        for (size_t i = metrics.timeSeriesData.size() - historySize; i < metrics.timeSeriesData.size(); ++i) {
            ss << "  " << i << ". " << metrics.timeSeriesData[i] << std::endl;
        }
    }
    
    return ss.str();
}

bool exportToCSV(const std::string& filename, const std::vector<NeuronVisualizationData>& neurons,
                const std::vector<SynapseVisualizationData>& synapses) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for CSV export: " + filename);
        return false;
    }
    
    // Write header
    file << "NeuronID,X,Y,Z,Type,RegionID,MembranePotential,Threshold,IsActive,IsSynapsing,FiringRate,LastSpikeTime" << std::endl;
    
    // Write neuron data
    for (const auto& neuron : neurons) {
        file << neuron.id << ","
             << neuron.x << ","
             << neuron.y << ","
             << neuron.z << ","
             << neuronTypeToString(neuron.type) << ","
             << neuron.regionId << ","
             << neuron.membranePotential << ","
             << neuron.threshold << ","
             << (neuron.isActive ? "true" : "false") << ","
             << (neuron.isSynapsing ? "true" : "false") << ","
             << neuron.firingRate << ","
             << neuron.lastSpikeTime << std::endl;
    }
    
    // Write synapse header and data
    file << std::endl << "SynapseID,SourceNeuron,TargetNeuron,Weight,Delay,IsActive,LastTransmission,Toxicity,HEbbianActive,STDPActive" << std::endl;
    
    for (const auto& synapse : synapses) {
        file << synapse.id << ","
             << synapse.sourceNeuron << ","
             << synapse.targetNeuron << ","
             << synapse.weight << ","
             << synapse.delay << ","
             << (synapse.isActive ? "true" : "false") << ","
             << synapse.lastTransmission << ","
             << synapse.toxicity << ","
             << (synapse.hebbianActive ? "true" : "false") << ","
             << (synapse.stdpActive ? "true" : "false") << std::endl;
    }
    
    file.close();
    NLM_LOG_INFO("Successfully exported data to CSV: " + filename);
    return true;
}

bool exportToJSON(const std::string& filename, const std::vector<NeuronVisualizationData>& neurons,
                 const std::vector<SynapseVisualizationData>& synapses) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for JSON export: " + filename);
        return false;
    }
    
    file << "{" << std::endl;
    file << "  \"neurons\": [" << std::endl;
    
    for (size_t i = 0; i < neurons.size(); ++i) {
        const auto& neuron = neurons[i];
        file << "    {" << std::endl;
        file << "      \"id\": " << neuron.id << "," << std::endl;
        file << "      \"x\": " << neuron.x << "," << std::endl;
        file << "      \"y\": " << neuron.y << ",\" " << std::endl;
        file << "      \"z\": " << neuron.z << "," << std::endl;
        file << "      \"type\": \"" << neuronTypeToString(neuron.type) << "\"," << std::endl;
        file << "      \"regionId\": " << neuron.regionId << "," << std::endl;
        file << "      \"membranePotential\": " << neuron.membranePotential << "," << std::endl;
        file << "      \"threshold\": " << neuron.threshold << "," << std::endl;
        file << "      \"isActive\": " << (neuron.isActive ? "true" : "false") << "," << std::endl;
        file << "      \"isSynapsing\": " << (neuron.isSynapsing ? "true" : "false") << "," << std::endl;
        file << "      \"firingRate\": " << neuron.firingRate << "," << std::endl;
        file << "      \"lastSpikeTime\": " << neuron.lastSpikeTime" << std::endl;
        if (i == neurons.size() - 1) {
            file << "    }" << std::endl;
        } else {
            file << "    },"; << std::endl;
        }
    }
    
    file << "  ]," << std::endl;
    file << "  \"synapses\": [" << std::endl;
    
    for (size_t i = 0; i < synapses.size(); ++i) {
        const auto& synapse = synapses[i];
        file << "    {" << std::endl;
        file << "      \"id\": " << synapse.id << "," << std::endl;
        file << "      \"sourceNeuron\": " << synapse.sourceNeuron << "," << std::endl;
        file << "      \"targetNeuron\": " << synapse.targetNeuron << "," << std::endl;
        file << "      \"weight\": " << synapse.weight << "," << std::endl;
        file << "      \"delay\": " << synapse.delay << "," << std::endl;
        file << "      \"isActive\": " << (synapse.isActive ? "true" : "false") << "," << std::endl;
        file << "      \"lastTransmission\": " << synapse.lastTransmission << "," << std::endl;
        file << "      \"toxicity\": " << synapse.toxicity << "," << std::endl;
        file << "      \"hebbianActive\": " << (synapse.hebbianActive ? "true" : "false") << "," << std::endl;
        file << "      \"stdpActive\": " << (synapse.stdpActive ? "true" : "false") << std::endl;
        if (i == synapses.size() - 1) {
            file << "    }" << std::endl;
        } else {
            file << "    },"; << std::endl;
        }
    }
    
    file << "  ]" << std::endl;
    file << "}" << std::endl;
    
    file.close();
    NLM_LOG_INFO("Successfully exported data to JSON: " + filename);
    return true;
}

bool exportToHTML(const std::string& filename, const std::vector<NeuronVisualizationData>& neurons,
                 const std::vector<SynapseVisualizationData>& synapses) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for HTML export: " + filename);
        return false;
    }
    
    file << "<!DOCTYPE html>" << std::endl;
    file << "<html>" << std::endl;
    file << "<head>" << std::endl;
    file << "    <title>NLM Neural Network Visualization</title>" << std::endl;
    file << "    <style>" << std::endl;
    file << "        body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
    file << "        .neuron { fill: #4CAF50; stroke: #45a049; }" << std::endl;
    file << "        .synapse { fill: #2196F3; stroke: #1976D2; }" << std::endl;
    file << "        .active { fill: #f44336; }" << std::endl;
    file << "        text { font-size: 12px; }" << std::endl;
    file << "    </style>" << std::endl;
    file << "</head>" << std::endl;
    file << "<body>" << std::endl;
    file << "    <h1>NLM Neural Network Visualization</h1>" << std::endl;
    file << "    <p>Total Neurons: " << neurons.size() << " | Total Synapses: " << synapses.size() << "</p>" << std::endl;
    file << "    <svg width=\"800\" height=\"600\" xmlns=\"http://www.w3.org/2000/svg\">" << std::endl;
    
    // In a complete implementation, we would draw the network here
    // For now, just create a placeholder
    file << "        <!-- Network visualization would go here -->" << std::endl;
    
    file << "    </svg>" << std::endl;
    file << "</body>" << std::endl;
    file << "</html>" << std::endl;
    
    file.close();
    NLM_LOG_INFO("Successfully exported data to HTML: " + filename);
    return true;
}

std::string generateProgressBar(float progress, int width) {
    int filled = static_cast<int>(progress * width);
    std::stringstream ss;
    
    ss << "[";
    for (int i = 0; i < width; ++i) {
        if (i < filled) {
            ss << "=";
        } else if (i == filled) {
            ss << ">";
        } else {
            ss << " ";
        }
    }
    ss << "] " << std::fixed << std::setprecision(1) << (progress * 100.0f) << "%";
    
    return ss.str();
}

std::string generateStatisticsSummary(const std::vector<NeuronVisualizationData>& neurons,
                                      const std::vector<SynapseVisualizationData>& synapses,
                                      const PerformanceMetricsVisualizationData& metrics) {
    std::stringstream ss;
    
    ss << "NLM Brain State Statistics" << std::endl;
    ss << "==========================" << std::endl;
    ss << "Total Neurons: " << neurons.size() << std::endl;
    ss << "Total Synapses: " << synapses.size() << std::endl;
    ss << std::endl;
    
    if (!neurons.empty()) {
        float avgFiringRate = 0.0f;
        float avgMembranePotential = 0.0f;
        
        for (const auto& neuron : neurons) {
            avgFiringRate += neuron.firingRate;
            avgMembranePotential += neuron.membranePotential;
        }
        
        avgFiringRate /= neurons.size();
        avgMembranePotential /= neurons.size();
        
        ss << "Average Firing Rate: " << avgFiringRate << " Hz" << std::endl;
        ss << "Average Membrane Potential: " << avgMembranePotential << std::endl;
        ss << "Active Neurons: " << countActiveNeurons(neurons) << " (" 
           << (countActiveNeurons(neurons) * 100.0f / neurons.size()) << "%)" << std::endl;
    }
    
    if (!synapses.empty()) {
        float totalWeight = 0.0f;
        int positive = 0, negative = 0, zero = 0;
        
        for (const auto& synapse : synapses) {
            totalWeight += synapse.weight;
            if (synapse.weight > 0.0f) positive++;
            else if (synapse.weight < 0.0f) negative++;
            else zero++;
        }
        
        ss << "Average Synapse Weight: " << (totalWeight / synapses.size()) << std::endl;
        ss << "Positive Synapses: " << positive << " (" 
           << (positive * 100.0f / synapses.size()) << "%)" << std::endl;
        ss << "Negative Synapses: " << negative << " (" 
           << (negative * 100.0f / synapses.size()) << "%)" << std::endl;
        ss << "Zero Synapses: " << zero << " (" 
           << (zero * 100.0f / synapses.size()) << "%)" << std::endl;
    }
    
    ss << std::endl;
    ss << "Performance Metrics:" << std::endl;
    ss << "  Learning Progress: " << metrics.learningProgress << std::endl;
    ss << "  Memory Retention: " << metrics.memoryRetention << std::endl;
    ss << "  Prediction Accuracy: " << metrics.predictionAccuracy << std::endl;
    ss << "  Exploration Rate: " << metrics.explorationRate << std::endl;
    
    return ss.str();
}

std::map<std::string, std::string> getColorPalette(const std::string& scheme) {
    std::map<std::string, std::string> palette;
    
    if (scheme == "scientific") {
        palette["background"] = "#ffffff";
        palette["neurons"] = "#2E7D32";
        palette["synapses"] = "#1976D2";
        palette["active_neurons"] = "#D32F2F";
        palette["text"] = "#212121";
        palette["grid"] = "#E0E0E0";
    } else if (scheme == "artistic") {
        palette["background"] = "#1A1A2E";
        palette["neurons"] = "#16DB65";
        palette["synapses"] = "#0F3460";
        palette["active_neurons"] = "#E94560";
        palette["text"] = "#F1F1F1";
        palette["grid"] = "#16213E";
    } else if (scheme == "mono") {
        palette["background"] = "#ffffff";
        palette["neurons"] = "#000000";
        palette["synapses"] = "#333333";
        palette["active_neurons"] = "#FF0000";
        palette["text"] = "#000000";
        palette["grid"] = "#CCCCCC";
    } else {  // default
        palette["background"] = "#ffffff";
        palette["neurons"] = "#4CAF50";
        palette["synapses"] = "#2196F3";
        palette["active_neurons"] = "#f44336";
        palette["text"] = "#212121";
        palette["grid"] = "#E0E0E0";
    }
    
    return palette;
}

} // namespace nlm