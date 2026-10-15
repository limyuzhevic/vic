#include "NeuralActivityVisualizer.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <queue>

namespace nlm {

struct NeuralActivityVisualizer::Impl {
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SpikeSystem> spikeSystem;
    
    // Activity data storage
    std::deque<std::pair<SimulationStep, NeuronId>> recentSpikes;
    std::unordered_map<NeuronId, float> neuronActivity;
    std::deque<std::vector<float>> spikeRasterBuffer;
    std::unordered_map<RegionId, float> activityHeatmap;
    
    // Sampling and thresholds
    double samplingInterval = 0.1; // seconds
    float activityThreshold = 0.5f;
    SimulationStep spikeBufferSize = 1000;
    
    // Performance tracking
    double lastSampleTime = 0.0;
    size_t totalSpikesProcessed = 0;
    
    // Helper structures
    struct NeuronStats {
        float totalActivity = 0.0f;
        float maxActivity = 0.0f;
        size_t spikeCount = 0;
    };
    
    Impl() {
        recentSpikes.reserve(spikeBufferSize);
    }
};

NeuralActivityVisualizer::NeuralActivityVisualizer() : pImpl(new Impl) {}

NeuralActivityVisualizer::~NeuralActivityVisualizer() = default;

bool NeuralActivityVisualizer::initialize() {
    return true;
}

void NeuralActivityVisualizer::update(double deltaTime) {
    pImpl->lastSampleTime += deltaTime;
    
    if (pImpl->lastSampleTime >= pImpl->samplingInterval && pImpl->brain) {
        // Sample current brain state
        pImpl->neuronActivity.clear();
        pImpl->activityHeatmap.clear();
        
        // Get spike system statistics
        if (pImpl->spikeSystem) {
            auto spikes = pImpl->spikeSystem->getRecentSpikes(pImpl->spikeBufferSize);
            pImpl->recentSpikes = std::move(spikes);
            pImpl->totalSpikesProcessed += pImpl->recentSpikes.size();
        }
        
        // Sample neuron activity levels
        if (pImpl->brain) {
            for (const auto& regionId : pImpl->brain->getRegionIds()) {
                if (auto* region = pImpl->brain->getRegion(regionId)) {
                    float regionActivity = 0.0f;
                    size_t neuronCount = 0;
                    
                    for (const auto& neuron : region->getAllNeurons()) {
                        float activity = neuron->getMembranePotential() / neuron->getThreshold();
                        pImpl->neuronActivity[neuron->getId()] = activity;
                        regionActivity += activity;
                        neuronCount++;
                    }
                    
                    if (neuronCount > 0) {
                        pImpl->activityHeatmap[regionId] = regionActivity / neuronCount;
                    }
                }
            }
        }
        
        pImpl->lastSampleTime = 0.0;
    }
}

void NeuralActivityVisualizer::render() {
    // Text-based visualization of neural activity
    std::cout << "=== Neural Activity Visualization ===" << std::endl;
    
    // Spike raster plot
    std::cout << "Spike Raster (last " << pImpl->recentSpikes.size() << " spikes):" << std::endl;
    for (const auto& spike : pImpl->recentSpikes) {
        std::cout << "  Time " << spike.first << ": Neuron " << spike.second << std::endl;
    }
    
    // Neuron activity summary
    std::cout << std::endl << "Neuron Activity Levels:" << std::endl;
    int highActivityCount = 0;
    int mediumActivityCount = 0;
    int lowActivityCount = 0;
    
    for (const auto& pair : pImpl->neuronActivity) {
        float activity = pair.second;
        if (activity > pImpl->activityThreshold * 2) {
            highActivityCount++;
            std::cout << "  Neuron " << pair.first << ": HIGH (" << std::fixed << std::setprecision(2) << activity << ")" << std::endl;
        } else if (activity > pImpl->activityThreshold) {
            mediumActivityCount++;
            std::cout << "  Neuron " << pair.first << ": MEDIUM (" << std::fixed << std::setprecision(2) << activity << ")" << std::endl;
        } else {
            lowActivityCount++;
        }
    }
    
    std::cout << std::endl << "Activity Distribution:" << std::endl;
    std::cout << "  High activity (>2x threshold): " << highActivityCount << std::endl;
    std::cout << "  Medium activity (1-2x threshold): " << mediumActivityCount << std::endl;
    std::cout << "  Low activity (<1x threshold): " << lowActivityCount << std::endl;
    
    // Activity heatmap summary
    std::cout << std::endl << "Region Activity Heatmap:" << std::endl;
    for (const auto& pair : pImpl->activityHeatmap) {
        std::cout << "  Region " << pair.first << ": " << std::fixed << std::setprecision(3) << pair.second << std::endl;
    }
}

void NeuralActivityVisualizer::close() {
    pImpl->recentSpikes.clear();
    pImpl->neuronActivity.clear();
    pImpl->spikeRasterBuffer.clear();
    pImpl->activityHeatmap.clear();
}

void NeuralActivityVisualizer::setBrain(std::shared_ptr<Brain> brain) {
    pImpl->brain = brain;
}

void NeuralActivityVisualizer::setSpikeSystem(std::shared_ptr<SpikeSystem> spikeSystem) {
    pImpl->spikeSystem = spikeSystem;
}

void NeuralActivityVisualizer::setSamplingInterval(double seconds) {
    pImpl->samplingInterval = seconds;
}

void NeuralActivityVisualizer::setActivityThreshold(float threshold) {
    pImpl->activityThreshold = threshold;
}

const std::deque<std::pair<SimulationStep, NeuronId>>& NeuralActivityVisualizer::getRecentSpikes() const {
    return pImpl->recentSpikes;
}

const std::unordered_map<NeuronId, float>& NeuralActivityVisualizer::getNeuronActivity() const {
    return pImpl->neuronActivity;
}

NeuralActivityVisualizer::FiringRateStats NeuralActivityVisualizer::getFiringRateStats() const {
    FiringRateStats stats;
    if (pImpl->recentSpikes.empty()) return stats;
    
    // Calculate firing rates from spike data (simplified)
    std::vector<float> rates;
    for (const auto& spike : pImpl->recentSpikes) {
        rates.push_back(1.0f); // Each spike is one event
    }
    
    if (rates.empty()) return stats;
    
    stats.average = std::accumulate(rates.begin(), rates.end(), 0.0f) / rates.size();
    stats.maximum = *std::max_element(rates.begin(), rates.end());
    stats.minimum = *std::min_element(rates.begin(), rates.end());
    
    // Calculate standard deviation
    float sum = std::accumulate(rates.begin(), rates.end(), 0.0f);
    float mean = sum / rates.size();
    float sqSum = 0.0f;
    for (float rate : rates) {
        sqSum += (rate - mean) * (rate - mean);
    }
    stats.standardDeviation = std::sqrt(sqSum / rates.size());
    
    return stats;
}

const std::unordered_map<RegionId, float>& NeuralActivityVisualizer::getActivityHeatmap() const {
    return pImpl->activityHeatmap;
}

} // namespace nlm
