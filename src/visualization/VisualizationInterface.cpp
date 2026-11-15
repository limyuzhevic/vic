#include "VisualizationInterface.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    std::chrono::high_resolution_clock::time_point lastRender;
    
    Impl() : active(false), updateRate(30.0) {
        lastRender = std::chrono::high_resolution_clock::now();
    }
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // Initialize basic visualization system
    // For now, we'll implement console-based visualization
    pImpl->active = true;
    return true;
}

void VisualizationInterface::update() {
    if (!pImpl->active) return;
    
    // Update visualization state
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration<double>(now - pImpl->lastRender).count();
    
    if (elapsed >= 1.0 / pImpl->updateRate) {
        render();
        pImpl->lastRender = now;
    }
}

void VisualizationInterface::render() {
    if (!pImpl->active) return;
    
    // Clear screen (simple approach)
    std::cout << "\033[2J\033[H" << std::endl;  // Clear screen and move cursor to top-left
    
    // Basic console-based brain visualization
    renderBrainStatus();
    renderActivityOverview();
    renderNetworkGraph();
    renderSpikeActivity();
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

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    if (!pImpl->active) return;
    
    std::cout << "=== NLM Network Visualization ===" << std::endl;
    std::cout << "Total Neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "Total Synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "Active Neurons: " << brain.getActiveNeuronCount() << std::endl;
    std::cout << "Firing Neurons: " << brain.getFiringNeuronCount() << std::endl;
    std::cout << "Total Spikes: " << brain.getTotalSpikeCount() << std::endl;
    
    std::cout << "\nNeuron Distribution:" << std::endl;
    std::cout << "  Excitatory: " << calculateNeuronCountByType(brain, NeuronType::Excitatory) << std::endl;
    std::cout << "  Inhibitory:  " << calculateNeuronCountByType(brain, NeuronType::Inhibitory) << std::endl;
    std::cout << "  Sensory:     " << calculateNeuronCountByType(brain, NeuronType::Sensory) << std::endl;
    std::cout << "  Motor:       " << calculateNeuronCountByType(brain, NeuronType::Motor) << std::endl;
    std::cout << "  Internal:    " << calculateNeuronCountByType(brain, NeuronType::Internal) << std::endl;
    
    std::cout << "\nConnectivity Statistics:" << std::endl;
    std::cout << "  Connection density: " << calculateConnectionDensity(brain) << std::endl;
    std::cout << "  Avg. weight: " << calculateAvgWeight(brain) << std::endl;
    std::cout << "  E/I ratio: " << calculateEIRatio(brain) << std::endl;
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    if (!pImpl->active) return;
    
    std::cout << "\n=== Activity Visualization ===" << std::endl;
    
    // Simple ASCII progress bars for key metrics
    renderProgressBar("Firing Rate", getFiringRatePercentage(brain), 50);
    renderProgressBar("Activity Level", getActivityLevel(brain), 50);
    renderProgressBar("Learning Progress", getLearningProgress(brain), 50);
    
    // Display recent spike information
    displayRecentSpikes(brain);
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    if (!pImpl->active) return;
    
    std::cout << "\n=== Spike Activity ===" << std::endl;
    std::cout << "Total Spikes Recorded: " << spikes.size() << std::endl;
    
    if (!spikes.empty()) {
        // Analyze spike patterns
        auto recentSpikes = getRecentSpikes(spikes);
        auto spikeTimes = extractSpikeTimes(recentSpikes);
        
        if (!spikeTimes.empty()) {
            double maxTime = *std::max_element(spikeTimes.begin(), spikeTimes.end());
            renderSpikeRaster(recentSpikes, maxTime);
            displaySpikeStatistics(recentSpikes);
        }
    }
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    if (!pImpl->active) return;
    
    std::cout << "\n=== Synaptic Weight Visualization ===" << std::endl;
    
    // Collect weight statistics
    auto weightStats = analyzeWeightDistribution(brain);
    
    std::cout << "Weight Distribution:" << std::endl;
    std::cout << "  Min: " << weightStats.minWeight << std::endl;
    std::cout << "  Max: " << weightStats.maxWeight << std::endl;
    std::cout << "  Mean: " << weightStats.meanWeight << std::endl;
    std::cout << "  Median: " << weightStats.medianWeight << std::endl;
    std::cout << "  Std Dev: " << weightStats.stdDevWeight << std::endl;
    
    std::cout << "\nWeight Histogram:" << std::endl;
    renderWeightHistogram(weightStats);
}

// Helper functions for visualization

int VisualizationInterface::calculateNeuronCountByType(const Brain& brain, NeuronType type) {
    int count = 0;
    for (auto region : brain.getRegions()) {
        for (auto neuron : region->getAllNeurons()) {
            if (neuron->getType() == type) {
                count++;
            }
        }
    }
    return count;
}

double VisualizationInterface::calculateConnectionDensity(const Brain& brain) {
    size_t totalSynapses = brain.getTotalSynapseCount();
    size_t neurons = brain.getTotalNeuronCount();
    
    if (neurons == 0) return 0.0;
    
    return (double)totalSynapses / (neurons * neurons);
}

double VisualizationInterface::calculateAvgWeight(const Brain& brain) {
    double totalWeight = 0.0;
    size_t synapseCount = 0;
    
    for (auto region : brain.getRegions()) {
        for (auto synapse : region->getSynapses()) {
            totalWeight += synapse->getWeight();
            synapseCount++;
        }
    }
    
    if (synapseCount == 0) return 0.0;
    return totalWeight / synapseCount;
}

double VisualizationInterface::calculateEIRatio(const Brain& brain) {
    size_t excitatory = calculateNeuronCountByType(brain, NeuronType::Excitatory);
    size_t inhibitory = calculateNeuronCountByType(brain, NeuronType::Inhibitory);
    
    if (inhibitory == 0) return (double)excitatory;
    return (double)excitatory / inhibitory;
}

double VisualizationInterface::getFiringRatePercentage(const Brain& brain) {
    size_t neurons = brain.getTotalNeuronCount();
    if (neurons == 0) return 0.0;
    
    return (double)brain.getFiringNeuronCount() / neurons * 100.0;
}

double VisualizationInterface::getActivityLevel(const Brain& brain) {
    size_t spikes = brain.getTotalSpikeCount();
    if (spikes == 0) return 0.0;
    
    return std::min(1.0, (double)spikes / 10000.0);  // Cap at 10k spikes
}

double VisualizationInterface::getLearningProgress(const Brain& brain) {
    // This is a simple proxy - in real implementation would be more sophisticated
    auto weights = extractAllWeights(brain);
    if (weights.empty()) return 0.0;
    
    double avgWeight = 0.0;
    for (float w : weights) avgWeight += w;
    avgWeight /= weights.size();
    
    return avgWeight / 2.0;  // Normalize to [0, 1] assuming weights range [0, 2]
}

void VisualizationInterface::renderProgressBar(const std::string& label, double value, int width) {
    std::cout << "  " << std::left << std::setw(15) << label << " [";
    
    int filled = (int)(value * width / 100.的上);
    for (int i = 0; i < width; ++i) {
        if (i < filled) {
            std::cout << "=";
        } else if (i == filled) {
            std::cout << ">";
        } else {
            std::cout << " ";
        }
    }
    
    std::cout << "] " << std::fixed << std::setprecision(1) << (value * 100.0) << "%" << std::endl;
}

void VisualizationInterface::displayRecentSpikes(const Brain& brain) {
    std::cout << "\nRecent Spike Activity:" << std::endl;
    
    // This would normally access spike history - for now just show current status
    size_t totalSpikes = brain.getTotalSpikeCount();
    size_t recentSpikes = totalSpikes > 100 ? totalSpikes - 100 : 0;
    
    std::cout << "  Total spikes: " << totalSpikes << std::endl;
    std::cout << "  Recent activity: " << (totalSpikes > 100 ? "High" : "Low") << std::endl;
}

std::vector<SpikeEvent> VisualizationInterface::getRecentSpikes(const std::vector<SpikeEvent>& spikes) {
    if (spikes.size() <= 100) {
        return spikes;
    }
    
    size_t startIndex = spikes.size() - 100;
    return std::vector<SpikeEvent>(spikes.begin() + startIndex, spikes.end());
}

std::vector<double> VisualizationInterface::extractSpikeTimes(const std::vector<SpikeEvent>& spikes) {
    std::vector<double> times;
    for (const auto& spike : spikes) {
        times.push_back(static_cast<double>(spike.timestamp));
    }
    return times;
}

void VisualizationInterface::renderSpikeRaster(const std::vector<SpikeEvent>& spikes, double maxTime) {
    std::cout << "Spike Raster (time axis horizontally):" << std::endl;
    
    if (spikes.size() > 40) {
        std::cout << "  (Too many spikes to display - showing summary)" << std::endl;
        return;
    }
    
    // Create a simple ASCII representation
    std::unordered_map<size_t, std::vector<double>> neuronSpikes;
    for (const auto& spike : spikes) {
        neuronSpikes[static_cast<size_t>(spike.source_neuron.index())].push_back(spike.timestamp);
    }
    
    for (const auto& [neuronId, timestamps] : neuronSpikes) {
        std::cout << "  Neuron " << neuronId << ": ";
        for (double time : timestamps) {
            int pos = static_cast<int>(time * 10) % 40;  // Scale to reasonable width
            std::string line(40, ' ');
            line[pos] = '*';
            std::cout << "\n    " << line;
        }
        std::cout << std::endl;
    }
}

void VisualizationInterface::displaySpikeStatistics(const std::vector<SpikeEvent>& spikes) {
    if (spikes.empty()) return;
    
    std::cout << "\nSpike Statistics:" << std::endl;
    
    std::unordered_map<size_t, size_t> neuronSpikeCounts;
    for (const auto& spike : spikes) {
        neuronSpikeCounts[static_cast<size_t>(spike.source_neuron.index())]++;
    }
    
    size_t maxSpikes = 0;
    for (const auto& count : neuronSpikeCounts) {
        if (count.second > maxSpikes) maxSpikes = count.second;
    }
    
    std::cout << "  Max spikes by single neuron: " << maxSpikes << std::endl;
    
    double minTime = std::numeric_limits<double>::max();
    double maxTime = std::numeric_limits<double>::min();
    for (const auto& spike : spikes) {
        if (spike.timestamp < minTime) minTime = spike.timestamp;
        if (spike.timestamp > maxTime) maxTime = spike.timestamp;
    }
    
    std::cout << "  Time range: [" << minTime << ", " << maxTime << "]" << std::endl;
    std::cout << "  Average rate: " << (spikes.size() / (maxTime - minTime + 0.001)) << " spikes/unit time" << std::endl;
}

struct WeightStats {
    double minWeight;
    double maxWeight;
    double meanWeight;
    double medianWeight;
    double stdDevWeight;
};

WeightStats VisualizationInterface::analyzeWeightDistribution(const Brain& brain) {
    WeightStats stats;
    stats.minWeight = std::numeric_limits<double>::max();
    stats.maxWeight = std::numeric_limits<double>::min();
    std::vector<double> weights;
    
    for (auto region : brain.getRegions()) {
        for (auto synapse : region->getSynapses()) {
            double w = synapse->getWeight();
            weights.push_back(w);
            if (w < stats.minWeight) stats.minWeight = w;
            if (w > stats.maxWeight) stats.maxWeight = w;
        }
    }
    
    if (weights.empty()) {
        stats.meanWeight = stats.medianWeight = stats.stdDevWeight = 0.0;
        return stats;
    }
    
    // Calculate mean
    double sum = 0.0;
    for (double w : weights) sum += w;
    stats.meanWeight = sum / weights.size();
    
    // Calculate standard deviation
    double sqSum = 0.0;
    for (double w : weights) {
        double diff = w - stats.meanWeight;
        sqSum += diff * diff;
    }
    stats.stdDevWeight = std::sqrt(sqSum / weights.size());
    
    // Calculate median
    std::sort(weights.begin(), weights.end());
    size_t n = weights.size();
    if (n % 2 == 0) {
        stats.medianWeight = (weights[n/2 - 1] + weights[n/2]) / 2.0;
    } else {
        stats.medianWeight = weights[n/2];
    }
    
    return stats;
}

void VisualizationInterface::renderWeightHistogram(const WeightStats& stats) {
    std::cout << "  Weight range: [" << stats.minWeight << ", " << stats.maxWeight << "]" << std::endl;
    
    // Create a simple histogram
    size_t numBins = 10;
    double binWidth = (stats.maxWeight - stats.minWeight) / numBins;
    
    std::vector<size_t> binCounts(numBins, 0);
    
    // Estimate bin counts (simplified - would need actual weight data)
    for (size_t i = 0; i < numBins; ++i) {
        size_t count = static_cast<size_t>((stats.stdDevWeight / binWidth) * 10);
        binCounts[i] = count;
    }
    
    std::cout << "  Histogram:" << std::endl;
    for (size_t i = 0; i < numBins; ++i) {
        std::cout << "    " << std::setw(3) << i << ": ";
        size_t barLength = (binCounts[i] * 30) / (std::max<size_t>(1, binCounts[0]));
        for (size_t j = 0; j < barLength; ++j) {
            std::cout << "*";
        }
        std::cout << " (" << binCounts[i] << ")" << std::endl;
    }
}

std::vector<float> VisualizationInterface::extractAllWeights(const Brain& brain) {
    std::vector<float> weights;
    
    for (auto region : brain.getRegions()) {
        for (auto synapse : region->getSynapses()) {
            weights.push_back(synapse->getWeight());
        }
    }
    
    return weights;
}

} // namespace nlm

// Add include for numeric_limits
#include <limits>
#include <algorithm>
#include <numeric>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    
    Impl() : active(false), updateRate(30.0) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

#include "VisualizationInterface.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <limits>
#include <numeric>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    std::chrono::high_resolution_clock::time_point lastRender;
    
    Impl() : active(false), updateRate(30.0) {
        lastRender = std::chrono::high_resolution_clock::now();
    }
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // Initialize basic visualization system
    // For now, we'll implement console-based visualization
    pImpl->active = true;
    return true;
}

void VisualizationInterface::update() {
    if (!pImpl->active) return;
    
    // Update visualization state
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration<double>(now - pImpl->lastRender).count();igg

    if (elapsed >= 1.0 / pImpl->updateRate) {
        render();
        pImpl->lastRender = now;
    }
}

void VisualizationInterface::render() {
    if (!pImpl->active) return;
    
    // Clear screen (simple approach)
    std::cout << "\033[2J\033[H" << std::endl;  // Clear screen and move cursor to top-left
    
    // Basic console-based brain visualization
    renderBrainStatus();
    renderActivityOverview();
    renderNetworkGraph();
    renderSpikeActivity();
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

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    if (!pImpl->active) return;
    
    std::cout << "=== NLM Network Visualization ===" << std::endl;
    std::cout << "Total Neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "Total Synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "Active Neurons: " << brain.getActiveNeuronCount() << std::endl;
    std::cout << "Firing Neurons: " << brain.getFiringNeuronCount() << std::endl;
    std::cout << "Total Spikes: " << brain.getTotalSpikeCount() << std::endl;
    
    std::cout << "\nNeuron Distribution:" << std::endl;
    std::cout << "  Excitatory: " << calculateNeuronCountByType(brain, NeuronType::Excitatory) << std::endl;
    std::cout << "  Inhibitory:  " << calculateNeuronCountByType(brain, NeuronType::Inhibitory) << std::endl;
    std::cout << "  Sensory:     " << calculateNeuronCountByType(brain, NeuronType::Sensory) << std::endl;
    std::cout << "  Motor:       " << calculateNeuronCountByType(brain, NeuronType::Motor) << std::endl;
    std::cout << "  Internal:    " << calculateNeuronCountByType(brain, NeuronType::Internal) << std::endl;
    
    std::cout << "\nConnectivity Statistics:" << std::endl;
    std::cout << "  Connection density: " << calculateConnectionDensity(brain) << std::endl;
    std::cout << "  Avg. weight: " << calculateAvgWeight(brain) << std::endl;
    std::cout << "  E/I ratio: " << calculateEIRatio(brain) << std::endl;
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    if (!pImpl->active) return;
    
    std::cout << "\n=== Activity Visualization ===" << std::endl;
    
    // Simple ASCII progress bars for key metrics
    renderProgressBar("Firing Rate", getFiringRatePercentage(brain), 50);
    renderProgressBar("Activity Level", getActivityLevel(brain), 50);
    renderProgressBar("Learning Progress", getLearningProgress(brain), 50);
    
    // Display recent spike information
    displayRecentSpikes(brain);
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    if (!pImpl->active) return;
    
    std::cout << "\n=== Spike Activity ===" << std::endl;
    std::cout << "Total Spikes Recorded: " << spikes.size() << std::endl;
    
    if (!spikes.empty()) {
        // Analyze spike patterns
        auto recentSpikes = getRecentSpikes(spikes);
        auto spikeTimes = extractSpikeTimes(recentSpikes);
        
        if (!spikeTimes.empty()) {
            double maxTime = *std::max_element(spikeTimes.begin(), spikeTimes.end());
            renderSpikeRaster(recentSpikes, maxTime);
            displaySpikeStatistics(recentSpikes);
        }
    }
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    if (!pImpl->active) return;
    
    std::cout << "\n=== Synaptic Weight Visualization ===" << std::endl;
    
    // Collect weight statistics
    auto weightStats = analyzeWeightDistribution(brain);
    
    std::cout << "Weight Distribution:" << std::endl;
    std::cout << "  Min: " << weightStats.minWeight << std::endl;
    std::cout << "  Max: " << weightStats.maxWeight << std::endl;
    std::cout << "  Mean: " << weightStats.meanWeight << std::endl;
    std::cout << "  Median: " << weightStats.medianWeight << std::endl;
    std::cout << "  Std Dev: " << weightStats.stdDevWeight << std::endl;
    
    std::cout << "\nWeight Histogram:" << std::endl;
    renderWeightHistogram(weightStats);
}

// Helper functions for visualization

int VisualizationInterface::calculateNeuronCountByType(const Brain& brain, NeuronType type) {
    int count = 0;
    for (auto region : brain.getRegions()) {
        for (auto neuron : region->getAllNeurons()) {
            if (neuron->getType() == type) {
                count++;
            }
        }
    }
    return count;
}

double VisualizationInterface::calculateConnectionDensity(const Brain& brain) {
    size_t totalSynapses = brain.getTotalSynapseCount();
    size_t neurons = brain.getTotalNeuronCount();
    
    if (neurons == 0) return 0.0;
    
    return (double)totalSynapses / (neurons * neurons);
}

double VisualizationInterface::calculateAvgWeight(const Brain& brain) {
    double totalWeight = 0.0;
    size_t synapseCount = 0;
    
    for (auto region : brain.getRegions()) {
        for (auto synapse : region->getSynapses()) {
            totalWeight += synapse->getWeight();
            synapseCount++;
        }
    }
    
    if (synapseCount == 0) return 0.0;
    return totalWeight / synapseCount;
}

double VisualizationInterface::calculateEIRatio(const Brain& brain) {
    size_t excitatory = calculateNeuronCountByType(brain, NeuronType::Excitatory);
    size_t inhibitory = calculateNeuronCountByType(brain, NeuronType::Inhibitory);
    
    if (inhibitory == 0) return (double)excitatory;
    return (double)excitatory / inhibitory;
}

double VisualizationInterface::getFiringRatePercentage(const Brain& brain) {
    size_t neurons = brain.getTotalNeuronCount();
    if (neurons == 0) return 0.0;
    
    return (double)brain.getFiringNeuronCount() / neurons * 100.0;
}

double VisualizationInterface::getActivityLevel(const Brain& brain) {
    size_t spikes = brain.getTotalSpikeCount();
    if (spikes == 0) return 0.0;
    
    return std::min(1.0, (double)spikes / 10000.0);  // Cap at 10k spikes
}

double VisualizationInterface::getLearningProgress(const Brain& brain) {
    // This is a simple proxy - in real implementation would be more sophisticated
    auto weights = extractAllWeights(brain);
    if (weights.empty()) return 0.0;
    
    double avgWeight = 0.0;
    for (float w : weights) avgWeight += w;
    avgWeight /= weights.size();
    
    return avgWeight / 2.0;  // Normalize to [0, 1] assuming weights range [0, 2]
}

void VisualizationInterface::renderProgressBar(const std::string& label, double value, int width) {
    std::cout << "  " << std::left << std::setw(15) << label << " [";
    
    int filled = static_cast<int>(value * width / 100.0);
    for (int i = 0; i < width; ++i) {
        if (i < filled) {
            std::cout << "=";
        } else if (i == filled) {
            std::cout << ">";
        } else {
            std::cout << " ";
        }
    }
    
    std::cout << "] " << std::fixed << std::setprecision(1) << (value * 100.0) << "%" << std::endl;
}

void VisualizationInterface::displayRecentSpikes(const Brain& brain) {
    std::cout << "\nRecent Spike Activity:" << std::endl;
    
    // This would normally access spike history - for now just show current status
    size_t totalSpikes = brain.getTotalSpikeCount();
    size_t recentSpikes = totalSpikes > 100 ? totalSpikes - 100 : 0;
    
    std::cout << "  Total spikes: " << totalSpikes << std::endl;
    std::cout << "  Recent activity: " << (totalSpikes > 100 ? "High" : "Low") << std::endl;
}

std::vector<SpikeEvent> VisualizationInterface::getRecentSpikes(const std::vector<SpikeEvent>& spikes) {
    if (spikes.size() <= 100) {
        return spikes;
    }
    
    size_t startIndex = spikes.size() - 100;
    return std::vector<SpikeEvent>(spikes.begin() + startIndex, spikes.end());
}

std::vector<double> VisualizationInterface::extractSpikeTimes(const std::vector<SpikeEvent>& spikes) {
    std::vector<double> times;
    for (const auto& spike : spikes) {
        times.push_back(static_cast<double>(spike.timestamp));
    }
    return times;
}

void VisualizationInterface::renderSpikeRaster(const std::vector<SpikeEvent>& spikes, double maxTime) {
    std::cout << "Spike Raster (time axis horizontally):" << std::endl;
    
    if (spikes.size() > 40) {
        std::cout << "  (Too many spikes to display - showing summary)" << std::endl;
        return;
    }
    
    // Create a simple ASCII representation
    std::unordered_map<size_t, std::vector<double>> neuronSpikes;
    for (const auto& spike : spikes) {
        neuronSpikes[static_cast<size_t>(spike.source_neuron.index())].push_back(spike.timestamp);
    }
    
    for (const auto& [neuronId, timestamps] : neuronSpikes) {
        std::cout << "  Neuron " << neuronId << ": ";
        for (double time : timestamps) {
            int pos = static_cast<int>(time * 10) % 40;  // Scale to reasonable width
            std::string line(40, ' ');
            line[pos] = '*';
            std::cout << "\n    " << line;
        }
        std::cout << std::endl;
    }
}

void VisualizationInterface::displaySpikeStatistics(const std::vector<SpikeEvent>& spikes) {
    if (spikes.empty()) return;
    
    std::cout << "\nSpike Statistics:" << std::endl;
    
    std::unordered_map<size_t, size_t> neuronSpikeCounts;
    for (const auto& spike : spikes) {
        neuronSpikeCounts[static_cast<size_t>(spike.source_neuron.index())]++;
    }
    
    size_t maxSpikes = 0;
    for (const auto& count : neuronSpikeCounts) {
        if (count.second > maxSpikes) maxSpikes = count.second;
    }
    
    std::cout << "  Max spikes by single neuron: " << maxSpikes << std::endl;
    
    double minTime = std::numeric_limits<double>::max();
    double maxTime = std::numeric_limits<double>::min();
    for (const auto& spike : spikes) {
        if (spike.timestamp < minTime) minTime = spike.timestamp;
        if (spike.timestamp > maxTime) maxTime = spike.timestamp;
    }
    
    std::cout << "  Time range: [" << minTime << ", " << maxTime << "]" << std::endl;
    std::cout << "  Average rate: " << (spikes.size() / (maxTime - minTime + 0.001)) << " spikes/unit time" << std::endl;
}

struct WeightStats {
    double minWeight;
    double maxWeight;
    double meanWeight;
    double medianWeight;
    double stdDevWeight;
};

WeightStats VisualizationInterface::analyzeWeightDistribution(const Brain& brain) {
    WeightStats stats;
    stats.minWeight = std::numeric_limits<double>::max();
    stats.maxWeight = std::numeric_limits<double>::min();
    std::vector<double> weights;
    
    for (auto region : brain.getRegions()) {
        for (auto synapse : region->getSynapses()) {
            double w = synapse->getWeight();
            weights.push_back(w);
            if (w < stats.minWeight) stats.minWeight = w;
            if (w > stats.maxWeight) stats.maxWeight = w;
        }
    }
    
    if (weights.empty()) {
        stats.meanWeight = stats.medianWeight = stats.stdDevWeight = 0.0;
        return stats;
    }
    
    // Calculate mean
    double sum = 0.0;
    for (double w : weights) sum += w;
    stats.meanWeight = sum / weights.size();
    
    // Calculate standard deviation
    double sqSum = 0.0;
    for (double w : weights) {
        double diff = w - stats.meanWeight;
        sqSum += diff * diff;
    }
    stats.stdDevWeight = std::sqrt(sqSum / weights.size());
    
    // Calculate median
    std::sort(weights.begin(), weights.end());
    size_t n = weights.size();
    if (n % 2 == 0) {
        stats.medianWeight = (weights[n/2 - 1] + weights[n/2]) / 2.0;
    } else {
        stats.medianWeight = weights[n/2];
    }
    
    return stats;
}

void VisualizationInterface::renderWeightHistogram(const WeightStats& stats) {
    std::cout << "  Weight range: [" << stats.minWeight << ", " << stats.maxWeight << "]" << std::endl;
    
    // Create a simple histogram
    size_t numBins = 10;
    double binWidth = (stats.maxWeight - stats.minWeight) / numBins;
    
    std::vector<size_t> binCounts(numBins, 0);
    
    // Estimate bin counts (simplified - would need actual weight data)
    for (size_t i = 0; i < numBins; ++i) {
        size_t count = static_cast<size_t>((stats.stdDevWeight / binWidth) * 10);
        binCounts[i] = count;
    }
    
    std::cout << "  Histogram:" << std::endl;
    for (size_t i = 0; i < numBins; ++i) {
        std::cout << "    " << std::setw(3) << i << ": ";
        size_t barLength = (binCounts[i] * 30) / (std::max<size_t>(1, binCounts[0));
        for (size_t j = 0; j < barLength; ++j) {
            std::cout << "*";
        }
        std::cout << " (" << binCounts[i] << ")" << std::endl;
    }
}

std::vector<float> VisualizationInterface::extractAllWeights(const Brain& brain) {
    std::vector<float> weights;
    
    for (auto region : brain.getRegions()) {
        for (auto synapse : region->getSynapses()) {
            weights.push_back(synapse->getWeight());
        }
    }
    
    return weights;
}

void VisualizationInterface::renderBrainStatus() {
    std::cout << "=== NLM Brain Status ===" << std::endl;
}

void VisualizationInterface::renderActivityOverview() {
    std::cout << "=== Activity Overview ===" << std::endl;
}

void VisualizationInterface::renderNetworkGraph() {
    std::cout << "=== Network Graph ===" << std::endl;
}

void VisualizationInterface::renderSpikeActivity() {
    std::cout << "=== Spike Activity ===" << std::endl;
}

} // namespace nlm

// Add include for numeric_limits
#include <limits>
#include <algorithm>
#include <numeric>

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

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    // TODO PHASE 2: Implement network visualization
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    // TODO PHASE 2: Implement activity visualization
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    // TODO PHASE 2: Implement spike raster plot
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    // TODO PHASE 2: Implement weight matrix visualization
}

} // namespace nlm
