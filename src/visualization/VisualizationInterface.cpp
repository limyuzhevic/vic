#include "VisualizationInterface.hpp"

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    
    Impl() : active(false), updateRate(30.0) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // TODO PHASE 2: Initialize real visualization (e.g., with GLFW, SDL, or custom)
    // PLACEHOLDER: No visualization in Phase 1
    pImpl->active = false;
    return true;
}

void VisualizationInterface::update() {
    // PLACEHOLDER: No visualization in Phase 1
}

void VisualizationInterface::render() {
    // PLACEHOLDER: No visualization in Phase 1
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
    // Output neural network structure to file for visualization
    // Generate a simple text-based network topology
    
    std::ofstream file("neural_topology.txt");
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open neural_topology.txt for writing");
        return;
    }
    
    file << "=== Neural Network Topology ===\n\n";
    
    // Write region information
    file << "Regions:" << std::endl;
    for (const auto& region : brain.getRegions()) {
        file << "  Region " << region->getId().index() << ": " 
             << region->getName() << " (" 
             << region->getTotalNeuronCount() << " neurons, "
             << region->getSynapseCount() << " synapses)\n";
    }
    
    file << "\nConnectivity Matrix:\n";
    file << "(Rows: source regions, Columns: target regions)\n\n";
    
    // Write inter-region connectivity
    for (const auto& region : brain.getRegions()) {
        file << "Region " << region->getId().index() << ":\n";
        for (const auto& conn : brain.getRegions()) {
            // Simple placeholder - in a real implementation would use inter-region connections
            float weight = (region->getId().index() == conn->getId().index()) ? 1.0f : 0.0f;
            file << "  " << conn->getId().index() << ": " << weight << " ";
        }
        file << "\n\n";
    }
    
    // Write population breakdown by type
    file << "\nPopulation Breakdown by Type:\n";
    for (const auto& region : brain.getRegions()) {
        file << "  Region " << region->getId().index() << ":\n";
        for (size_t i = 0; i < region->getPopulationCount(); ++i) {
            auto* pop = region->getPopulation(PopulationId(i));
            if (pop) {
                file << "    Population " << i << ": " 
                     << pop->getNeuronCount() << " "
                     << neuronTypeToString(pop->getNeuronType()) << " neurons\n";
            }
        }
    }
    
    // Write synaptic density
    file << "\nSynaptic Density:\n";
    for (const auto& region : brain.getRegions()) {
        size_t synapses = region->getSynapseCount();
        size_t neurons = region->getTotalNeuronCount();
        float density = neurons > 0 ? (float)synapses / neurons : 0.0f;
        file << "  Region " << region->getId().index() << ": "
             << density << " synapses/neuron\n";
    }
    
    file << "\n=== End of Neural Network Topology ===\n";
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    // Output neural activity patterns to file for visualization
    
    std::ofstream file("neural_activity.txt");
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open neural_activity.txt for writing");
        return;
    }
    
    file << "=== Neural Activity Pattern ===\n\n";
    
    // Write firing statistics
    file << "Firing Statistics:\n";
    file << "  Total spikes: " << brain.getTotalSpikeCount() << "\n";
    file << "  Active neurons: " << brain.getActiveNeuronCount() << "\n";
    file << "  Firing neurons: " << brain.getFiringNeuronCount() << "\n";
    file << "  Average firing rate: " << brain.getAverageFiringRate() << " Hz\n";
    file << "  Pending spike events: " << brain.getPendingSpikeEventCount() << "\n\n";
    
    // Write activity by region
    file << "Activity by Region:\n";
    for (const auto& region : brain.getRegions()) {
        file << "  Region " << region->getId().index() << ":\n";
        file << "    Neurons: " << region->getTotalNeuronCount() << "\n";
        file << "    Active: " << region->getActiveNeuronCount() << "\n";
        file << "    Avg firing rate: " << region->getAverageFiringRate() << " Hz\n";
    }
    
    file << "\n=== End of Neural Activity Pattern ===\n";
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    // Output spike events to file for visualization
    
    std::ofstream file("spike_raster.txt");
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open spike_raster.txt for writing");
        return;
    }
    
    file << "=== Spike Raster Plot ===\n\n";
    
    if (spikes.empty()) {
        file << "No spikes recorded\n\n";
        file << "=== End of Spike Raster ===\n";
        return;
    }
    
    file << "Spike Events (NeuronID, Time, Step):\n";
    for (const auto& spike : spikes) {
        file << "  Neuron " << spike.neuron_id << " at time " 
             << spike.time << " (step " << spike.step << ")\n";
    }
    
    file << "\nSpike Statistics:\n";
    file << "  Total spikes: " << spikes.size() << "\n";
    
    if (!spikes.empty()) {
        double minTime = spikes[0].time;
        double maxTime = spikes[0].time;
        for (const auto& spike : spikes) {
            minTime = std::min(minTime, spike.time);
            maxTime = std::max(maxTime, spike.time);
        }
        file << "  Time range: " << minTime << " - " << maxTime << "\n";
        file << "  Time span: " << (maxTime - minTime) << " time units\n";
    }
    
    file << "\n=== End of Spike Raster ===\n";
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    // Output synaptic weights distribution to file for visualization
    
    std::ofstream file("synaptic_weights.txt");
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open synaptic_weights.txt for writing");
        return;
    }
    
    file << "=== Synaptic Weight Distribution ===\n\n";
    
    // Collect all synapse weights
    std::vector<float> weights;
    for (const auto& region : brain.getRegions()) {
        for (const auto* syn : region->getSynapses()) {
            weights.push_back(syn->getWeight());
        }
    }
    
    if (weights.empty()) {
        file << "No synapses found\n\n";
        file << "=== End of Synaptic Weight Distribution ===\n";
        return;
    }
    
    file << "Weight Statistics:\n";
    file << "  Total synapses: " << weights.size() << "\n";
    
    // Calculate statistics
    float sum = 0.0f;
    float min = weights[0];
    float max = weights[0];
    for (float w : weights) {
        sum += w;
        min = std::min(min, w);
        max = std::max(max, w);
    }
    
    float mean = sum / weights.size();
    
    file << "  Min weight: " << min << "\n";
    file << "  Max weight: " << max << "\n";
    file << "  Mean weight: " << mean << "\n";
    
    // Calculate distribution
    std::vector<int> bins(10, 0);  // 10 bins from -1 to 1
    for (float w : weights) {
        int binIndex = static_cast<int>((w + 1.0f) * 9.0f / 2.0f);  // Map -1..1 to 0..9
        if (binIndex < 0) binIndex = 0;
        if (binIndex >= 10) binIndex = 9;
        bins[binIndex]++;
    }
    
    file << "\nWeight Distribution (% of synapses):\n";
    for (int i = 0; i < 10; ++i) {
        float rangeStart = -1.0f + i * 2.0f / 9.0f;
        float rangeEnd = -1.0f + (i + 1) * 2.0f / 9.0f;
        float percentage = (float)bins[i] / weights.size() * 100.0f;
        file << "  [" << rangeStart << " - " << rangeEnd << "] : " 
             << percentage << "% (" << bins[i] << " synapses)\n";
    }
    
    file << "\n=== End of Synaptic Weight Distribution ===\n";
}

std::string VisualizationInterface::neuronTypeToString(NeuronType type) {
    switch (type) {
        case NeuronType::Sensory: return "Sensory";
        case NeuronType::Internal: return "Internal";
        case NeuronType::Motor: return "Motor";
        default: return "Unknown";
    }
}

} // namespace nlm
