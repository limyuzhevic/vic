#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "core/Types/Types.hpp"

namespace nlm {
    class Brain;
    class VisualizationBackend;
    class VisualizationRenderer;
    
    // Network visualization data structures
    struct NetworkNode {
        NeuronId id;
        float x, y, z;           // Position in 3D space
        float potential;          // Membrane potential (-70 to 50 mV)
        float radius;             // Node size based on neuron importance
        std::string type;         // "sensory", "motor", "internal", "excitatory", "inhibitory"
        bool active;              // Currently firing or active
        bool selected;            // Currently selected/highlighted
        uint64_t region_id;        // Which region this neuron belongs to
        
        NetworkNode() : id(), x(0), y(0), z(0), potential(-70), radius(5.0f), 
                       active(false), selected(false), region_id(0) {}
    };
    
    struct NetworkEdge {
        SynapseId id;
        NeuronId source_id;
        NeuronId target_id;
        float weight;             // Synaptic strength (-1.0 to 1.0)
        float alpha;              // Transparency (0.0 to 1.0)
        bool active;              // Connection is active
        SynapseType type;         // Excitatory, inhibitory, modulatory, etc.
        
        NetworkEdge() : id(), source_id(), target_id(), weight(0), alpha(0.5f), 
                       active(false), type(SynapseType::Excitatory) {}
    };
    
    // Activity visualization data structures
    struct ActivityData {
        std::vector<float> membrane_potentials;  // Per-neuron potentials (mV)
        std::vector<float> firing_rates;         // Per-neuron firing rates (Hz)
        std::vector<double> spike_times;         // Spike timestamps (s)
        std::vector<float> synaptic_weights;     // Recent synaptic weight changes
        float max_potential;                      // Maximum potential in current frame
        float min_potential;                      // Minimum potential in current frame
        float max_firing_rate;                   // Maximum firing rate (Hz)
        float min_firing_rate;                   // Minimum firing rate (Hz)
        
        ActivityData() : max_potential(-70), min_potential(50), max_firing_rate(0), min_firing_rate(0) {}
    };
    
    // Spike visualization data structures
    struct SpikeData {
        std::vector<double> spike_times;         // List of spike timestamps (s)
        std::vector<NeuronId> neuron_ids;        // Which neurons spiked
        std::vector<uint64_t> regions;           // Which regions neurons belong to
        std::vector<SynapseId> synapse_ids;       // Which synapses were involved
        float max_rate;                          // Maximum firing rate (Hz)
        float min_rate;                          // Minimum firing rate (Hz)
        float total_spikes;                      // Total number of spikes
        
        SpikeData() : max_rate(0), min_rate(0), total_spikes(0) {}
    };
    
    // Weight matrix visualization data structures
    struct WeightMatrix {
        std::vector<std::vector<float>> matrix;  // 2D weight matrix
        std::vector<std::string> row_labels;     // Row labels (neuron/region names)
        std::vector<std::string> col_labels;     // Column labels
        float min_weight;                        // Minimum weight value in matrix
        float max_weight;                        // Maximum weight value in matrix
        std::vector<std::vector<float>> changes; // Weight changes over time
        
        WeightMatrix() : min_weight(0), max_weight(0) {}
    };
    
    // Visualization configuration
    struct VisualizationConfig {
        bool enable_network_viz = true;
        bool enable_activity_viz = true;
        bool enable_spike_viz = true;
        bool enable_weights_viz = true;
        std::string backend_type;                // "opengl", "sfml", "custom"
        std::string output_format;               // "png", "video", "none"
        std::string output_path;                 // Where to save visualizations
        double update_rate = 30.0;               // Hz
        bool interactive = true;                 // Enable mouse/keyboard interaction
        
        VisualizationConfig() {}
    };

    // Forward declaration for implementation
    class VisualizationInterface::Impl;

} // namespace nlm
