#include "VisualizationInterface.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <chrono>

namespace nlm {

struct VisualizationInterface::Impl {
    VisualizationBackend backend;
    VisualizationFormat format;
    bool active;
    double updateRate;
    
    // Visualization data
    std::vector<std::string> networkData;
    std::vector<std::string> activityData;
    std::vector<SpikeEvent> spikeData;
    std::vector<std::vector<double>> weightData;
    
    // Options
    NetworkVisualizationOptions networkOptions;
    ActivityVisualizationOptions activityOptions;
    SpikeRasterOptions spikeOptions;
    WeightMatrixOptions weightOptions;
    
    // Flags for enabling/disabling visualization components
    bool enableNetworkVisualization;
    bool enableActivityVisualization;
    bool enableSpikeRasterVisualization;
    bool enableWeightMatrixVisualization;
    
    Impl() : backend(VisualizationBackend::Console), format(VisualizationFormat::Text),
             active(false), updateRate(30.0), enableNetworkVisualization(true),
             enableActivityVisualization(true), enableSpikeRasterVisualization(true),
             enableWeightMatrixVisualization(true) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

VisualizationInterface::VisualizationInterface(VisualizationInterface&& other) noexcept
    : pImpl(std::move(other.pImpl)) {}

VisualizationInterface& VisualizationInterface::operator=(VisualizationInterface&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool VisualizationInterface::initialize(VisualizationBackend backend, VisualizationFormat format) {
    pImpl->backend = backend;
    pImpl->format = format;
    pImpl->active = true;
    
    switch (pImpl->backend) {
        case VisualizationBackend::Console:
            std::cout << "Console visualization initialized." << std::endl;
            break;
        case VisualizationBackend::File:
            std::cout << "File visualization initialized." << std::endl;
            break;
        case VisualizationBackend::None:
            std::cout << "Visualization disabled." << std::endl;
            break;
    }
    
    return true;
}

void VisualizationInterface::update() {
    // Update visualization data
    // This would be called from the main simulation loop
    if (pImpl->enableNetworkVisualization) {
        // Network data would be updated here
    }
    if (pImpl->enableActivityVisualization) {
        // Activity data would be updated here
    }
    if (pImpl->enableSpikeRasterVisualization) {
        // Spike data would be updated here
    }
    if (pImpl->enableWeightMatrixVisualization) {
        // Weight data would be updated here
    }
}

void VisualizationInterface::render() {
    if (!pImpl->active) return;
    
    switch (pImpl->backend) {
        case VisualizationBackend::Console:
            renderToConsole();
            break;
        case VisualizationBackend::File:
            renderToFile();
            break;
        case VisualizationBackend::None:
            // No rendering needed
            break;
    }
}

void VisualizationInterface::close() {
    pImpl->active = false;
    pImpl->networkData.clear();
    pImpl->activityData.clear();
    pImpl->spikeData.clear();
    pImpl->weightData.clear();
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

void VisualizationInterface::setNetworkOptions(const NetworkVisualizationOptions& options) {
    pImpl->networkOptions = options;
}

void VisualizationInterface::setActivityOptions(const ActivityVisualizationOptions& options) {
    pImpl->activityOptions = options;
}

void VisualizationInterface::setSpikeRasterOptions(const SpikeRasterOptions& options) {
    pImpl->spikeOptions = options;
}

void VisualizationInterface::setWeightMatrixOptions(const WeightMatrixOptions& options) {
    pImpl->weightOptions = options;
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    if (!pImpl->enableNetworkVisualization) return;
    
    pImpl->networkData.clear();
    
    // Generate network visualization data
    std::stringstream ss;
    
    // Count neurons and synapses
    size_t neuronCount = brain.getTotalNeuronCount();
    size_t synapseCount = brain.getTotalSynapseCount();
    
    ss << "=== Network Visualization ===" << std::endl;
    ss << "Neurons: " << neuronCount << std::endl;
    ss << "Synapses: " << synapseCount << std::endl;
    ss << "Regions: " << brain.getRegionCount() << std::endl;
    
    // Add region information
    for (size_t regionId = 1; regionId <= brain.getRegionCount(); ++regionId) {
        if (auto* region = brain.getRegion(RegionId(regionId))) {
            ss << "Region " << regionId << ": " << region->getNeuronCount() << " neurons, "
               << region->getSynapseCount() << " synapses" << std::endl;
        }
    }
    
    // Add neuron type distribution
    if (auto* region = brain.getRegion(RegionId(1))) {
        auto neurons = region->getAllNeurons();
        size_t sensoryCount = 0, motorCount = 0, internalCount = 0;
        
        for (auto* neuron : neurons) {
            switch (neuron->getType()) {
                case NeuronType::Sensory: ++sensoryCount; break;
                case NeuronType::Motor: ++motorCount; break;
                case NeuronType::Internal: ++internalCount; break;
            }
        }
        
        ss << "Neuron types (Region 1):" << std::endl;
        ss << "  Sensory: " << sensoryCount << std::endl;
        ss << "  Motor: " << motorCount << std::endl;
        ss << "  Internal: " << internalCount << std::endl;
    }
    
    pImpl->networkData.push_back(ss.str());
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    if (!pImpl->enableActivityVisualization) return;
    
    pImpl->activityData.clear();
    
    // Generate activity visualization data
    std::stringstream ss;
    
    ss << "=== Activity Visualization ===" << std::endl;
    ss << "Total spikes: " << brain.getTotalSpikeCount() << std::endl;
    ss << "Firing neurons: " << brain.getFiringNeuronCount() << std::endl;
    ss << "Time: " << brain.getCurrentTime() << std::endl;
    
    // Add firing rate information for first region
    if (auto* region = brain.getRegion(RegionId(1))) {
        auto neurons = region->getAllNeurons();
        ss << "\nNeuron Activity (Region 1):" << std::endl;
        
        // Calculate average firing rate
        double totalFiringRate = 0.0;
        size_t activeNeurons = 0;
        
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            if (neurons[i]->isFiring()) {
                totalFiringRate += neurons[i]->getFiringRate();
                ++activeNeurons;
                ss << "  Neuron " << i << ": " << std::fixed << std::setprecision(2)
                   << neurons[i]->getFiringRate() << " Hz";
                
                if (pImpl->activityOptions.showMembranePotential) {
                    ss << ", V=" << neurons[i]->getMembranePotential() << "mV";
                }
                if (pImpl->activityOptions.showRefractoryState) {
                    ss << ", refractory=" << neurons[i]->isRefractory();
                }
                
                ss << std::endl;
            }
        }
        
        if (activeNeurons > 0) {
            double avgFiringRate = totalFiringRate / activeNeurons;
            ss << "\nAverage firing rate: " << std::fixed << std::setprecision(2) << avgFiringRate << " Hz";
        }
    }
    
    pImpl->activityData.push_back(ss.str());
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    if (!pImpl->enableSpikeRasterVisualization) return;
    
    pImpl->spikeData = spikes;
    
    // Generate spike raster visualization
    std::stringstream ss;
    
    ss << "=== Spike Raster Visualization ===" << std::endl;
    ss << "Total spikes: " << spikes.size() << std::endl;
    ss << "Time window: " << pImpl->spikeOptions.timeWindow << " seconds" << std::endl;
    
    // Simple spike count summary
    if (!spikes.empty()) {
        auto minTime = spikes.front().timestamp;
        auto maxTime = spikes.back().timestamp;
        double duration = maxTime - minTime;
        
        ss << "Time range: " << std::fixed << std::setprecision(3) << minTime << " - " 
           << maxTime << " seconds" << std::endl;
        ss << "Average firing rate: " << (spikes.size() / duration) << " Hz" << std::endl;
    }
    
    // In a real implementation, this would generate actual raster plots
    ss << "\nSpike events (showing first 20):" << std::endl;
    for (size_t i = 0; i < std::min(size_t(20), spikes.size()); ++i) {
        ss << "  Time: " << std::fixed << std::setprecision(3) << spikes[i].timestamp
           << "s, Neuron: " << spikes[i].neuronId << std::endl;
    }
    
    if (spikes.size() > 20) {
        ss << "  ... and " << (spikes.size() - 20) << " more spikes" << std::endl;
    }
    
    pImpl->networkData.push_back(ss.str());
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    if (!pImpl->enableWeightMatrixVisualization) return;
    
    // Generate weight matrix visualization data
    pImpl->weightData.clear();
    
    std::stringstream ss;
    
    ss << "=== Weight Matrix Visualization ===" << std::endl;
    ss << "Total synapses: " << brain.getTotalSynapseCount() << std::endl;
    
    // Add synapse weight statistics
    double totalWeight = 0.0;
    double maxWeight = 0.0;
    double minWeight = 1e9;
    size_t synapseCount = 0;
    
    // This would iterate through all synapses in the brain
    // For demonstration, we'll just provide a placeholder
    ss << "Note: Weight matrix visualization requires synapse access." << std::endl;
    ss << "Weight statistics would be calculated by iterating through all connections." << std::endl;
    
    pImpl->networkData.push_back(ss.str());
}

void VisualizationInterface::renderToConsole() {
    std::cout << "=== NLM Visualization Output ===" << std::endl;
    
    if (pImpl->enableNetworkVisualization && !pImpl->networkData.empty()) {
        std::cout << pImpl->networkData.back() << std::endl;
    }
    
    if (pImpl->enableActivityVisualization && !pImpl->activityData.empty()) {
        std::cout << pImpl->activityData.back() << std::endl;
    }
    
    if (pImpl->enableSpikeRasterVisualization && !pImpl->spikeData.empty()) {
        std::cout << "Spike data available: " << pImpl->spikeData.size() << " events" << std::endl;
    }
    
    if (pImpl->enableWeightMatrixVisualization && !pImpl->weightData.empty()) {
        std::cout << "Weight matrix data available" << std::endl;
    }
    
    std::cout << "==============================" << std::endl;
}

void VisualizationInterface::renderToFile() {
    std::cout << "Rendering visualization to file..." << std::endl;
    
    // In a real implementation, this would write to a file
    // For demonstration, we'll just log the action
    std::cout << "Note: File rendering would write to visualization output files." << std::endl;
}

bool VisualizationInterface::exportToFile(const std::string& filepath, VisualizationFormat format) {
    if (pImpl->backend != VisualizationBackend::File) {
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Export based on format
    switch (format) {
        case VisualizationFormat::Text:
            exportToText(file);
            break;
        case VisualizationFormat::JSON:
            exportToJSON(file);
            break;
        case VisualizationFormat::CSV:
            exportToCSV(file);
            break;
        case VisualizationFormat::HTML:
            exportToHTML(file);
            break;
        case VisualizationFormat::ASCII_ART:
            exportToASCIIArt(file);
            break;
    }
    
    file.close();
    return true;
}

std::string VisualizationInterface::getVisualizationData() const {
    std::stringstream ss;
    
    ss << "Visualization Data (" << pImpl->networkData.size() << " entries)" << std::endl;
    ss << "Network entries: " << pImpl->networkData.size() << std::endl;
    ss << "Activity entries: " << pImpl->activityData.size() << std::endl;
    ss << "Spike events: " << pImpl->spikeData.size() << std::endl;
    ss << "Weight matrices: " << pImpl->weightData.size() << std::endl;
    
    return ss.str();
}

void VisualizationInterface::clearData() {
    pImpl->networkData.clear();
    pImpl->activityData.clear();
    pImpl->spikeData.clear();
    pImpl->weightData.clear();
}

void VisualizationInterface::enableNetworkVisualization(bool enable) {
    pImpl->enableNetworkVisualization = enable;
}

void VisualizationInterface::enableActivityVisualization(bool enable) {
    pImpl->enableActivityVisualization = enable;
}

void VisualizationInterface::enableSpikeRasterVisualization(bool enable) {
    pImpl->enableSpikeRasterVisualization = enable;
}

void VisualizationInterface::enableWeightMatrixVisualization(bool enable) {
    pImpl->enableWeightMatrixVisualization = enable;
}

void VisualizationInterface::exportToText(std::ofstream& file) {
    file << "=== NLM Visualization Output ===" << std::endl;
    
    if (pImpl->enableNetworkVisualization && !pImpl->networkData.empty()) {
        file << pImpl->networkData.back() << std::endl;
    }
    
    if (pImpl->enableActivityVisualization && !pImpl->activityData.empty()) {
        file << pImpl->activityData.back() << std::endl;
    }
}

void VisualizationInterface::exportToJSON(std::ofstream& file) {
    file << "{\n";
    file << "  \"visualization_type\": \"nlm_visualization\",\n";
    file << "  \"timestamp\": \"" << getCurrentTimestamp() << "\",\n";
    file << "  \"data_points\": {\n";
    
    if (pImpl->enableNetworkVisualization && !pImpl->networkData.empty()) {
        file << "    \"network_data\": [\"" << escapeJSON(pImpl->networkData.back()) << "\"]\n";
    }
    
    file << "  }\n";
    file << "}\n";
}

void VisualizationInterface::exportToCSV(std::ofstream& file) {
    file << "visualization_type,timestamp,network_data,activity_data\n";
    file << "nlm_visualization," << getCurrentTimestamp() << ",";
    
    if (!pImpl->networkData.empty()) {
        file << "\"" << escapeCSV(pImpl->networkData.back()) << "\"";
    }
    file << ",";
    
    if (!pImpl->activityData.empty()) {
        file << "\"" << escapeCSV(pImpl->activityData.back()) << "\"";
    }
    file << "\n";
}

void VisualizationInterface::exportToHTML(std::ofstream& file) {
    file << "<!DOCTYPE html>\n";
    file << "<html><head><title>NLM Visualization</title></head><body>" << std::endl;
    file << "<h1>NLM Neural Visualization</h1>\n";
    file << "<pre>" << std::endl;
    
    if (pImpl->enableNetworkVisualization && !pImpl->networkData.empty()) {
        file << pImpl->networkData.back() << std::endl;
    }
    
    file << "</pre></body></html>\n";
}

void VisualizationInterface::exportToASCIIArt(std::ofstream& file) {
    file << "=== ASCII ART NEURAL NETWORK ===" << std::endl;
    file << "   .-'" << std::endl;
    file << "  / o \  NLM" << std::endl;
    file << " | | |" << std::endl;
    file << "  '-'" << std::endl;
    
    if (pImpl->enableNetworkVisualization && !pImpl->networkData.empty()) {
        // Simple representation of network data
        file << "Network stats: " << countLines(pImpl->networkData.back()) << " lines" << std::endl;
    }
}

std::string VisualizationInterface::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string VisualizationInterface::escapeJSON(const std::string& input) {
    std::string output;
    for (char c : input) {
        switch (c) {
            case '\"': output += "\\\""; break;
            case '\\': output += "\\\\"; break;
            case '\b': output += "\\b"; break;
            case '\f': output += "\\f"; break;
            case '\n': output += "\\n"; break;
            case '\r': output += "\\r"; break;
            case '\t': output += "\\t"; break;
            default: output += c; break;
        }
    }
    return output;
}

std::string VisualizationInterface::escapeCSV(const std::string& input) {
    std::string output;
    if (input.find(',') != std::string::npos || input.find('"') != std::string::npos) {
        output += "\"";
        for (char c : input) {
            if (c == '"') output += "\"\"";
            else output += c;
        }
        output += "\"";
    } else {
        output = input;
    }
    return output;
}

size_t VisualizationInterface::countLines(const std::string& input) {
    size_t count = 0;
    for (char c : input) {
        if (c == '\n') ++count;
    }
    return count;
}

} // namespace nlm