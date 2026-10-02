#include "VisualizationInterface.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    bool saveToFile;
    std::string outputDirectory;
    
    Impl() : active(false), updateRate(30.0), saveToFile(false), outputDirectory("./viz_output") {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    pImpl->active = true;
    
    // Create output directory if it doesn't exist
    std::ifstream dirCheck(pImpl->outputDirectory + "/.test");
    if (!dirCheck.good()) {
        std::string cmd = "mkdir -p " + pImpl->outputDirectory;
        system(cmd.c_str());
    }
    
    return true;
}

void VisualizationInterface::update() {
    if (!pImpl->active) return;
    
    // Placeholder for real-time visualization updates
    // In a real implementation, this would:
    // - Process new spike events
    // - Update neural activity displays
    // - Refresh visualizations
    // - Save frames if configured
}

void VisualizationInterface::render() {
    if (!pImpl->active) return;
    
    // Placeholder for rendering visualizations
    // In a real implementation, this would:
    // - Draw neural network diagram
    // - Display spike raster plots
    // - Show weight matrices
    // - Update real-time graphs
}

void VisualizationInterface::close() {
    pImpl->active = false;
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

void VisualizationInterface::setSaveToFile(bool save) {
    pImpl->saveToFile = save;
}

bool VisualizationInterface::getSaveToFile() const {
    return pImpl->saveToFile;
}

void VisualizationInterface::setOutputDirectory(const std::string& directory) {
    pImpl->outputDirectory = directory;
}

std::string VisualizationInterface::getOutputDirectory() const {
    return pImpl->outputDirectory;
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    if (!pImpl->active) return;
    
    std::ostringstream ss;
    ss << pImpl->outputDirectory << "/network_" << getTimestamp() << ".txt";
    
    std::ofstream file(ss.str());
    if (!file.is_open()) {
        return;
    }
    
    file << "NLM Network Visualization\n";
    file << "===========================\n\n";
    
    file << "Regions: " << brain.getRegionCount() << "\n";
    file << "Total Neurons: " << brain.getTotalNeuronCount() << "\n";
    file << "Total Synapses: " << brain.getTotalSynapseCount() << "\n";
    file << "Firing Neurons: " << brain.getFiringNeuronCount() << "\n";
    file << "Average Firing Rate: " << brain.getAverageFiringRate() << " Hz\n\n";
    
    file << "Region Details:\n";
    for (size_t i = 0; i < brain.getRegionCount(); ++i) {
        RegionId regionId(i + 1);
        const NeuralRegion* region = brain.getRegion(regionId);
        if (!region) continue;
        
        file << "  Region " << region->getId().index() << " (" << region->getName() << "):";
        file << " " << region->getTotalNeuronCount() << " neurons, ";
        file << region->getSynapseCount() << " synapses\n";
    }
    
    file << "\nConnectivity Matrix:\n";
    file << "(1 = connection, 0 = no connection)\n";
    
    // Simple connectivity visualization
    for (size_t i = 0; i < brain.getRegionCount(); ++i) {
        file << "Region " << (i + 1) << ": ";
        for (size_t j = 0; j < brain.getRegionCount(); ++j) {
            // Check for any connections between regions
            bool hasConnection = false;
            for (const auto& conn : brain.getRegions()[i]->getInterRegionConnections()) {
                if (conn.sourceRegion.index() == i + 1 && conn.targetRegion.index() == j + 1) {
                    hasConnection = true;
                    break;
                }
            }
            file << (hasConnection ? "1 " : "0 ");
        }
        file << "\n";
    }
    
    file.close();
    
    if (pImpl->saveToFile) {
        NLM_LOG_INFO("Network visualization saved to: " + ss.str());
    }
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    if (!pImpl->active) return;
    
    std::ostringstream ss;
    ss << pImpl->outputDirectory << "/activity_" << getTimestamp() << ".txt";
    
    std::ofstream file(ss.str());
    if (!file.is_open()) {
        return;
    }
    
    file << "NLM Activity Visualization\n";
    file << "============================\n\n";
    
    file << "Time Step: " << brain.getTotalSpikeCount() << "\n";
    file << "Active Neurons: " << brain.getActiveNeuronCount() << "\n";
    file << "Firing Neurons: " << brain.getFiringNeuronCount() << "\n";
    file << "Average Firing Rate: " << brain.getAverageFiringRate() << " Hz\n\n";
    
    // Activity statistics by region
    file << "Activity by Region:\n";
    for (size_t i = 0; i < brain.getRegionCount(); ++i) {
        RegionId regionId(i + 1);
        const NeuralRegion* region = brain.getRegion(regionId);
        if (!region) continue;
        
        file << "  Region " << region->getId().index() << ": ";
        file << region->getActiveNeuronCount() << " active neurons\n";
    }
    
    // E/I ratio
    float eiRatio = brain.getExcitationInhibitionRatio();
    file << "\nExcitatory/Inhibitory Ratio: " << eiRatio << "\n";
    
    file.close();
    
    if (pImpl->saveToFile) {
        NLM_LOG_INFO("Activity visualization saved to: " + ss.str());
    }
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    if (!pImpl->active || spikes.empty()) return;
    
    std::ostringstream ss;
    ss << pImpl->outputDirectory << "/spikes_" << getTimestamp() << ".txt";
    
    std::ofstream file(ss.str());
    if (!file.is_open()) {
        return;
    }
    
    file << "NLM Spike Visualization\n";
    file << "========================\n\n";
    
    file << "Total Spikes: " << spikes.size() << "\n";
    file << "Time Range: " << getMinSpikeTime(spikes) << " - " << getMaxSpikeTime(spikes) << " ms\n\n";
    
    // Organize spikes by neuron
    std::unordered_map<NeuronId, std::vector<SpikeEvent>> spikesByNeuron;
    for (const auto& spike : spikes) {
        spikesByNeuron[spike.neuronId].push_back(spike);
    }
    
    file << "Spikes by Neuron:\n";
    for (const auto& pair : spikesByNeuron) {
        file << "  Neuron " << pair.first.index() << ": " << pair.second.size() << " spikes\n";
    }
    
    file << "\nSpike Times (sorted):\n";
    std::vector<double> sortedTimes;
    for (const auto& spike : spikes) {
        sortedTimes.push_back(spike.time);
    }
    std::sort(sortedTimes.begin(), sortedTimes.end());
    
    for (size_t i = 0; i < std::min(sortedTimes.size(), size_t(100)); ++i) {
        file << "  " << sortedTimes[i] << " ms\n";
    }
    if (sortedTimes.size() > 100) {
        file << "  ... and " << (sortedTimes.size() - 100) << " more\n";
    }
    
    file.close();
    
    if (pImpl->saveToFile) {
        NLM_LOG_INFO("Spike visualization saved to: " + ss.str());
    }
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    if (!pImpl->active) return;
    
    std::ostringstream ss;
    ss << pImpl->outputDirectory << "/weights_" << getTimestamp() << ".txt";
    
    std::ofstream file(ss.str());
    if (!file.is_open()) {
        return;
    }
    
    file << "NLM Weight Visualization\n";
    file << "=========================\n\n";
    
    file << "Total Synapses: " << brain.getTotalSynapseCount() << "\n\n";
    
    // Weight statistics
    double minWeight = std::numeric_limits<float>::max();
    double maxWeight = std::numeric_limits<float>::lowest();
    double sumWeight = 0.0;
    size_t count = 0;
    
    for (const auto& region : brain.getRegions()) {
        for (const auto* synapse : region->getSynapses()) {
            double weight = synapse->getWeight();
            minWeight = std::min(minWeight, weight);
            maxWeight = std::max(maxWeight, weight);
            sumWeight += weight;
            count++;
        }
    }
    
    if (count > 0) {
        double avgWeight = sumWeight / count;
        double stdDev = 0.0;
        
        for (const auto& region : brain.getRegions()) {
            for (const auto* synapse : region->getSynapses()) {
                double weight = synapse->getWeight();
                stdDev += (weight - avgWeight) * (weight - avgWeight);
            }
        }
        stdDev = std::sqrt(stdDev / count);
        
        file << "Weight Statistics:\n";
        file << "  Minimum: " << minWeight << "\n";
        file << "  Maximum: " << maxWeight << "\n";
        file << "  Average: " << avgWeight << "\n";
        file << "  Standard Deviation: " << stdDev << "\n";
        file << "  Range: " << (maxWeight - minWeight) << "\n\n";
    }
    
    // Distribution histogram
    file << "Weight Distribution (histogram):\n";
    std::vector<size_t> histogram(10, 0);  // 10 bins from -1 to 1
    
    for (const auto& region : brain.getRegions()) {
        for (const auto* synapse : region->getSynapses()) {
            double weight = synapse->getWeight();
            if (weight <= -1.0) {
                histogram[0]++;
            } else if (weight >= 1.0) {
                histogram[9]++;
            } else {
                size_t bin = static_cast<size_t>((weight + 1.0) / 2.0 * 9);
                histogram[bin]++;
            }
        }
    }
    
    for (size_t i = 0; i < histogram.size(); ++i) {
        double binMin = -1.0 + i * 0.222222;
        double binMax = binMin + 0.222222;
        file << "  [" << std::fixed << std::setprecision(3) << binMin << ", " << binMax << ") : ";
        
        double proportion = histogram[i] / static_cast<double>(count);
        size_t barLength = static_cast<size_t>(proportion * 50);
        for (size_t j = 0; j < barLength; ++j) {
            file << "|";
        }
        file << " (" << histogram[i] << ")\n";
    }
    
    file.close();
    
    if (pImpl->saveToFile) {
        NLM_LOG_INFO("Weight visualization saved to: " + ss.str());
    }
}

void VisualizationInterface::visualizeDevelopment(const Brain& brain) {
    if (!pImpl->active) return;
    
    std::ostringstream ss;
    ss << pImpl->outputDirectory << "/development_" << getTimestamp() << ".txt";
    
    std::ofstream file(ss.str());
    if (!file.is_open()) {
        return;
    }
    
    file << "NLM Development Visualization\n";
    file << "==============================\n\n";
    
    DevelopmentalStage stage = brain.getDevelopmentalStage();
    file << "Current Developmental Stage: ";
    switch (stage) {
        case DevelopmentalStage::Initial: file << "Initial"; break;
        case DevelopmentalStage::CriticalPeriod: file << "Critical Period"; break;
        case DevelopmentalStage::Maturation: file << "Maturation"; break;
        case DevelopmentalStage::Adult: file << "Adult"; break;
        case DevelopmentalStage::Aging: file << "Aging"; break;
    }
    file << "\n\n";
    
    // Activity patterns by development stage
    file << "Brain Statistics by Development Stage:\n";
    file << "  Total Neurons: " << brain.getTotalNeuronCount() << "\n";
    file << "  Total Synapses: " << brain.getTotalSynapseCount() << "\n";
    file << "  Firing Rate: " << brain.getAverageFiringRate() << " Hz\n";
    file << "  E/I Ratio: " << brain.getExcitationInhibitionRatio() << "\n";
    
    file.close();
    
    if (pImpl->saveToFile) {
        NLM_LOG_INFO("Development visualization saved to: " + ss.str());
    }
}

void VisualizationInterface::saveAll(const Brain& brain, const std::vector<SpikeEvent>& spikes) {
    if (!pImpl->active) return;
    
    NLM_LOG_INFO("Saving all visualizations to: " + pImpl->outputDirectory);
    
    visualizeNetwork(brain);
    visualizeActivity(brain);
    visualizeSpikes(spikes);
    visualizeWeights(brain);
    visualizeDevelopment(brain);
    
    NLM_LOG_INFO("All visualizations saved successfully.");
}

std::string VisualizationInterface::getTimestamp() {
    std::time_t now = std::time(nullptr);
    char buf[100];
    std::strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", std::localtime(&now));
    return std::string(buf);
}

double VisualizationInterface::getMinSpikeTime(const std::vector<SpikeEvent>& spikes) {
    if (spikes.empty()) return 0.0;
    double minTime = spikes[0].time;
    for (const auto& spike : spikes) {
        if (spike.time < minTime) {
            minTime = spike.time;
        }
    }
    return minTime;
}

double VisualizationInterface::getMaxSpikeTime(const std::vector<SpikeEvent>& spikes) {
    if (spikes.empty()) return 0.0;
    double maxTime = spikes[0].time;
    for (const auto& spike : spikes) {
        if (spike.time > maxTime) {
            maxTime = spike.time;
        }
    }
    return maxTime;
}

} // namespace nlm
