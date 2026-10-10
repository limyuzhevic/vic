#include "NeuralActivityVisualizers.hpp"

namespace nlm {

struct SpikeTrainsVisualizer::Impl {
    std::vector<int64_t> neuronsToDisplay;
    std::unordered_map<int64_t, std::vector<int64_t>> neuronGroups;
    std::unordered_map<int64_t, std::string> groupColors;
    std::deque<std::tuple<int64_t, double, float>> spikeData;
    double currentTime;
    size_t maxSpikeHistory;
    bool showStatistics;
    bool showNeuronsByRegion;
    
    Impl() : currentTime(0.0), maxSpikeHistory(1000), showStatistics(true), showNeuronsByRegion(true) {}
};

SpikeTrainsVisualizer::SpikeTrainsVisualizer() : pImpl(std::make_unique<Impl>()) {}

SpikeTrainsVisualizer::~SpikeTrainsVisualizer() = default;

SpikeTrainsVisualizer::SpikeTrainsVisualizer(SpikeTrainsVisualizer&&) noexcept = default;
SpikeTrainsVisualizer& SpikeTrainsVisualizer::operator=(SpikeTrainsVisualizer&&) noexcept = default;

bool SpikeTrainsVisualizer::initialize(const VisualizationConfig& config) {
    return Visualizer::initialize(config);
}

void SpikeTrainsVisualizer::update(const VisualizationData& data) {
    auto spikeDataVec = data.getSpikeData();
    
    for (const auto& spike : spikeDataVec) {
        pImpl->spikeData.emplace_back(std::get<0>(spike), std::get<1>(spike), std::get<2>(spike));
    }
    
    // Trim history
    while (pImpl->spikeData.size() > pImpl->maxSpikeHistory) {
        pImpl->spikeData.pop_front();
    }
    
    pImpl->currentTime = pImpl->spikeData.empty() ? 0.0 : std::get<1>(pImpl->spikeData.back());
    
    Visualizer::update(data);
}

void SpikeTrainsVisualizer::render(bool output) {
    Visualizer::render(output);
}

void SpikeTrainsVisualizer::configure(const VisualizationConfig& config) {
    Visualizer::configure(config);
}

VisualizationType SpikeTrainsVisualizer::getType() const {
    return VisualizationType::NeuralActivity;
}

std::string SpikeTrainsVisualizer::getName() const {
    return "SpikeTrainsVisualizer";
}

void SpikeTrainsVisualizer::setNeuronsToDisplay(const std::vector<int64_t>& neuronIds) {
    pImpl->neuronsToDisplay = neuronIds;
}

void SpikeTrainsVisualizer::addNeuronGroup(int64_t regionId, const std::vector<int64_t>& neuronIds,
                                         const std::string& color) {
    pImpl->neuronGroups[regionId] = neuronIds;
    pImpl->groupColors[regionId] = color;
}

std::unordered_map<std::string, double> SpikeTrainsVisualizer::getSpikeStatistics() const {
    std::unordered_map<std::string, double> stats;
    
    if (pImpl->spikeData.empty()) {
        return stats;
    }
    
    // Calculate basic statistics
    stats["total_spikes"] = static_cast<double>(pImpl->spikeData.size());
    
    std::unordered_map<int64_t, size_t> spikesPerNeuron;
    for (const auto& spike : pImpl->spikeData) {
        spikesPerNeuron[std::get<0>(spike)]++;
    }
    
    stats["unique_neurons"] = static_cast<double>(spikesPerNeuron.size());
    
    if (!spikesPerNeuron.empty()) {
        double sum = 0.0;
        for (const auto& [neuronId, count] : spikesPerNeuron) {
            sum += static_cast<double>(count);
        }
        double mean = sum / spikesPerNeuron.size();
        stats["mean_spikes_per_neuron"] = mean;
        
        double variance = 0.0;
        for (const auto& [neuronId, count] : spikesPerNeuron) {
            variance += (static_cast<double>(count) - mean) * (static_cast<double>(count) - mean);
        }
        stats["std_dev_spikes_per_neuron"] = std::sqrt(variance / spikesPerNeuron.size());
    }
    
    return stats;
}

bool SpikeTrainsVisualizer::exportData(const std::string& format, const std::string& filepath) {
    // Implementation would export spike data in specified format
    return false;
}

} // namespace nlm