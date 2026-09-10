#include "VisualizationInterface.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../core/Config/Config.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <sstream>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    
    // Visualization data storage
    std::vector<NetworkNode> network_nodes;
    std::vector<NetworkEdge> network_edges;
    ActivityData activity_data;
    SpikeData spike_data;
    WeightMatrix weight_matrix;
    
    Impl() : active(false), updateRate(30.0) {}
    
    // Helper methods for visualization
    void clearNetworkData() {
        network_nodes.clear();
        network_edges.clear();
    }
    
    void clearActivityData() {
        activity_data.membrane_potentials.clear();
        activity_data.firing_rates.clear();
        activity_data.spike_times.clear();
    }
    
    void clearSpikeData() {
        spike_data.spike_times.clear();
        spike_data.neuron_ids.clear();
        spike_data.regions.clear();
    }
    
    void clearWeightMatrix() {
        weight_matrix.matrix.clear();
        weight_matrix.row_labels.clear();
        weight_matrix.col_labels.clear();
    }
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize() {
    // TODO PHASE 2: Initialize real visualization (e.g., with GLFW, SDL, or custom)
    // For now, enable basic visualization data collection
    pImpl->active = true;
    return true;
}

void VisualizationInterface::update() {
    // Update visualization data from brain state
    if (pImpl->active) {
        // Clear old data
        pImpl->clearNetworkData();
        pImpl->clearActivityData();
        pImpl->clearSpikeData();
        pImpl->clearWeightMatrix();
        
        // Process brain data for visualization
        // This will be implemented in generateVisualization methods
    }
}

void VisualizationInterface::render() {
    // TODO PHASE 2: Render visualization to screen/window
    // For now, just output summary information
    if (pImpl->active) {
        std::stringstream ss;
        ss << "Visualization Status:" << std::endl;
        ss << "  Nodes: " << pImpl->network_nodes.size() << std::endl;
        ss << "  Edges: " << pImpl->network_edges.size() << std::endl;
        ss << "  Spikes recorded: " << pImpl->spike_data.spike_times.size() << std::endl;
        ss << "  Weight matrix: " << pImpl->weight_matrix.matrix.size() << "x" 
           << (pImpl->weight_matrix.matrix.empty() ? 0 : pImpl->weight_matrix.matrix[0].size()) << std::endl;
        std::string output = ss.str();
        // In real implementation, this would be rendered to a window
    }
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
    
    // Generate network visualization data
    pImpl->clearNetworkData();
    
    // Create nodes for each neuron
    size_t neuron_idx = 0;
    for (const auto& region : brain.getRegions()) {
        for (const auto& population : region->getPopulations()) {
            for (const auto& neuron : population->getNeurons()) {
                NetworkNode node;
                node.id = neuron_idx;
                
                // Position: place neurons in 2D grid for visualization
                static float nodeSpacing = 20.0f;
                static float regionWidth = 200.0f;
                static float regionHeight = 150.0f;
                
                // Simple 2D positioning based on population index
                int region_id = static_cast<int>(region->getId().value % 3);  // 3 regions in grid
                int pop_id = static_cast<int>(population->getId().value % 3);
                
                node.x = regionWidth * region_id + nodeSpacing * pop_id;
                node.y = regionHeight * (neuron_idx % 3);
                node.z = 0.0f;
                
                // Set neuron properties based on type
                node.type = "internal";
                switch (population->getNeuronType()) {
                    case NeuronType::Sensory:   node.type = "sensory"; break;
                    case NeuronType::Motor:    node.type = "motor"; break;
                    case NeuronType::Excitatory: node.type = "excitatory"; break;
                    case NeuronType::Inhibitory: node.type = "inhibitory"; break;
                    case NeuronType::Modulatory: node.type = "modulatory"; break;
                    default: break;
                }
                
                // Get membrane potential
                const auto& state = neuron->getState();
                node.potential = state.membranePotential;
                
                // Determine if neuron is active
                node.active = (state.firingState == FiringState::Active || 
                              state.firingState == FiringState::Refractory);
                
                pImpl->network_nodes.push_back(node);
                neuron_idx++;
            }
        }
    }
    
    // Create edges for synapses
    for (const auto& region : brain.getRegions()) {
        for (const auto& synapse : region->getSynapses()) {
            NetworkEdge edge;
            edge.source_id = synapse->getSourceNeuron().index();
            edge.target_id = synapse->getDestinationNeuron().index();
            edge.weight = synapse->getWeight();
            edge.active = std::abs(edge.weight) > 0.01f; // Consider significant synapses active
            pImpl->network_edges.push_back(edge);
        }
    }
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    if (!pImpl->active) return;
    
    pImpl->clearActivityData();
    
    // Collect activity data from all neurons
    size_t neuron_count = 0;
    float max_potential = -1000.0f;
    float min_potential = 1000.0f;
    
    for (const auto& region : brain.getRegions()) {
        for (const auto& population : region->getPopulations()) {
            for (const auto& neuron : population->getNeurons()) {
                const auto& state = neuron->getState();
                
                // Add membrane potential
                pImpl->activity_data.membrane_potentials.push_back(state.membranePotential);
                
                // Calculate firing rate approximation
                float firing_rate = 0.0f;
                if (state.lastSpikeTime >= 0.0f) {
                    // Simple firing rate estimate (spikes per second)
                    firing_rate = 1000.0f / (brain.getConfig()->getOr<double>("simulation_timestep", 0.001));
                }
                pImpl->activity_data.firing_rates.push_back(firing_rate);
                
                // Update min/max potentials
                if (state.membranePotential > max_potential) max_potential = state.membranePotential;
                if (state.membranePotential < min_potential) min_potential = state.membranePotential;
                
                neuron_count++;
            }
        }
    }
    
    // Set activity data statistics
    pImpl->activity_data.max_potential = max_potential;
    pImpl->activity_data.min_potential = min_potential;
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    if (!pImpl->active) return;
    
    pImpl->clearSpikeData();
    
    // Convert SpikeEvent to visualization data
    for (const auto& spike : spikes) {
        pImpl->spike_data.spike_times.push_back(spike.timestamp);
        pImpl->spike_data.neuron_ids.push_back(spike.source_neuron.value);
        
        // Find region for spike visualization
        uint64_t region_id = 0;
        // In a real implementation, we'd map neuron to its region
        pImpl->spike_data.regions.push_back(region_id);
    }
    
    // Calculate spike rate statistics
    if (!spikes.empty()) {
        pImpl->spike_data.max_rate = static_cast<float>(spikes.size()) / 1000.0f; // spikes per ms
        pImpl->spike_data.min_rate = 0.0f;
    }
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    if (!pImpl->active) return;
    
    pImpl->clearWeightMatrix();
    
    // Build a simplified weight matrix visualization
    // For a full implementation, this would create a matrix showing connection strengths
    
    // Create a simple 3x3 matrix for demonstration
    std::vector<std::vector<float>> matrix(3, std::vector<float>(3, 0.0f));
    
    // Fill with sample data based on brain regions
    float total_weight = 0.0f;
    size_t synapse_count = 0;
    
    for (const auto& region : brain.getRegions()) {
        // Get region average weight
        float region_weight = region->getAverageSynapticWeight();
        if (synapse_count < 9) {
            matrix[synapse_count % 3][synapse_count / 3] = region_weight;
            total_weight += region_weight;
            synapse_count++;
        }
    }
    
    // Find min/max for normalization
    float min_weight = 1000.0f;
    float max_weight = -1000.0f;
    for (const auto& row : matrix) {
        for (float weight : row) {
            if (weight > max_weight) max_weight = weight;
            if (weight < min_weight) min_weight = weight;
        }
    }
    
    pImpl->weight_matrix.matrix = matrix;
    pImpl->weight_matrix.min_weight = min_weight;
    pImpl->weight_matrix.max_weight = max_weight;
    
    // Add labels
    pImpl->weight_matrix.row_labels = {"Region 1", "Region 2", "Region 3"};
    pImpl->weight_matrix.col_labels = {"Region 1", "Region 2", "Region 3"};
}

} // namespace nlm
