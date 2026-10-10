#include "NeuralActivityVisualizers.hpp"

namespace nlm {

struct SynapticActivityVisualizer::Impl {
    float minWeight;
    float maxWeight;
    std::deque<std::tuple<int64_t, int64_t, float, double>> synapticData;
    double currentTime;
    size_t maxSynapticHistory;
    bool showStatistics;
    bool showDelays;
    bool showPlasticity;
    
    Impl() : minWeight(0.0f), maxWeight(0.0f), currentTime(0.0), maxSynapticHistory(1000),
             showStatistics(true), showDelays(true), showPlasticity(true) {}
};

SynapticActivityVisualizer::SynapticActivityVisualizer() : pImpl(std::make_unique<Impl>()) {}

SynapticActivityVisualizer::~SynapticActivityVisualizer() = default;

SynapticActivityVisualizer::SynapticActivityVisualizer(SynapticActivityVisualizer&&) noexcept = default;
SynapticActivityVisualizer& SynapticActivityVisualizer::operator=(SynapticActivityVisualizer&&) noexcept = default;

bool SynapticActivityVisualizer::initialize(const VisualizationConfig& config) {
    return Visualizer::initialize(config);
}

void SynapticActivityVisualizer::update(const VisualizationData& data) {
    auto synapticDataVec = data.getSynapticWeightData();
    
    for (const auto& synapse : synapticDataVec) {
        pImpl->synapticData.emplace_back(
            std::get<0>(synapse),
            std::get<1>(synapse),
            std::get<2>(synapse),
            std::get<3>(synapse)
        );
    }
    
    // Trim history
    while (pImpl->synapticData.size() > pImpl->maxSynapticHistory) {
        pImpl->synapticData.pop_front();
    }
    
    pImpl->currentTime = pImpl->synapticData.empty() ? 0.0 : std::get<3>(pImpl->synapticData.back());
    
    Visualizer::update(data);
}

void SynapticActivityVisualizer::render(bool output) {
    Visualizer::render(output);
}

void SynapticActivityVisualizer::configure(const VisualizationConfig& config) {
    Visualizer::configure(config);
}

VisualizationType SynapticActivityVisualizer::getType() const {
    return VisualizationType::NeuralActivity;
}

std::string SynapticActivityVisualizer::getName() const {
    return "SynapticActivityVisualizer";
}

void SynapticActivityVisualizer::setSynapticFilter(float minWeight, float maxWeight) {
    pImpl->minWeight = minWeight;
    pImpl->maxWeight = maxWeight;
}

std::unordered_map<std::string, double> SynapticActivityVisualizer::getSynapticStatistics() const {
    std::unordered_map<std::string, double> stats;
    
    if (pImpl->synapticData.empty()) {
        return stats;
    }
    
    // Calculate basic statistics
    std::vector<float> weights;
    for (const auto& synapse : pImpl->synapticData) {
        weights.push_back(std::get<2>(synapse));
    }
    
    double sum = std::accumulate(weights.begin(), weights.end(), 0.0);
    stats["mean_weight"] = sum / weights.size();
    
    double variance = 0.0;
    for (float weight : weights) {
        double diff = weight - stats["mean_weight"];
        variance += diff * diff;
    }
    stats["std_dev_weight"] = std::sqrt(variance / weights.size());
    
    stats["max_weight"] = *std::max_element(weights.begin(), weights.end());
    stats["min_weight"] = *std::min_element(weights.begin(), weights.end());
    stats["total_connections"] = static_cast<double>(pImpl->synapticData.size());
    
    return stats;
}

bool SynapticActivityVisualizer::exportData(const std::string& format, const std::string& filepath) {
    // Implementation would export synaptic data in specified format
    return false;
}

} // namespace nlm