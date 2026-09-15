#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Maintenance connections (recurrent)
    std::vector<std::pair<NeuronId, NeuronId>> maintenance_synapses;
    
    // Memory trace ages
    std::vector<SimulationStep> trace_ages;
    
    Impl() : brain(nullptr) {}
};

NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(new Impl)
    , brain(nullptr)
    , capacity(100)
    , decay_rate(0.01f)
{
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    this->brain = brain;
    NLM_LOG_INFO("NeuralWorkingMemory initialized");
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain) return;
    
    // Find neurons to encode this pattern
    size_t neurons_needed = std::min(pattern.size(), memory_neurons.size());
    
    for (size_t i = 0; i < neurons_needed; ++i) {
        NeuronId neuron = memory_neurons[i % memory_neurons.size()];
        float activation = pattern[i] * strength;
        
        // Set neuron activation
        if (auto* n = brain->get_region(neuron.get_id() / 1000)->get_all_neurons()) {
            for (auto* nn : *n) {
                if (nn->get_id() == neuron) {
                    nn->inject_current(activation * 5.0f);
                    break;
                }
            }
        }
        
        // Update stored activation
        if (i < memory_activations.size()) {
            memory_activations[i] = activation;
        } else {
            memory_activations.push_back(activation);
            memory_timestamps.push_back(0);
            memory_neurons.push_back(neuron);
        }
    }
    
    // Create maintenance connections if needed
    for (size_t i = 1; i < memory_neurons.size(); ++i) {
        create_recurrent_connection(memory_neurons[i-1], memory_neurons[i], strength * 0.5f);
    }
}

void NeuralWorkingMemory::store_to_neuron(NeuronId neuron, float activation) {
    // Find or add this neuron to memory
    auto it = std::find(memory_neurons.begin(), memory_neurons.end(), neuron);
    
    if (it != memory_neurons.end()) {
        size_t idx = std::distance(memory_neurons.begin(), it);
        memory_activations[idx] = activation;
        memory_timestamps[idx] = 0;
    } else if (memory_neurons.size() < capacity) {
        memory_neurons.push_back(neuron);
        memory_activations.push_back(activation);
        memory_timestamps.push_back(0);
    }
    
    // Inject current to maintain activation
    if (brain) {
        brain->inject_current(neuron, activation * 5.0f);
    }
}

std::vector<float> NeuralWorkingMemory::retrieve() const {
    std::vector<float> result;
    result.reserve(memory_activations.size());
    
    for (float activation : memory_activations) {
        result.push_back(activation);
    }
    
    return result;
}

bool NeuralWorkingMemory::contains(NeuronId neuron) const {
    return std::find(memory_neurons.begin(), memory_neurons.end(), neuron) != memory_neurons.end();
}

float NeuralWorkingMemory::get_neuron_activation(NeuronId neuron) const {
    auto it = std::find(memory_neurons.begin(), memory_neurons.end(), neuron);
    if (it != memory_neurons.end()) {
        size_t idx = std::distance(memory_neurons.begin(), it);
        return memory_activations[idx];
    }
    return 0.0f;
}

void NeuralWorkingMemory::update(TimestepDuration dt) {
    if (!brain) return;
    
    // Update maintenance - reinforce active memory neurons
    for (size_t i = 0; i < memory_neurons.size(); ++i) {
        NeuronId neuron = memory_neurons[i];
        float activation = memory_activations[i];
        
        if (activation > 0.1f) {
            // Inject maintenance current
            brain->inject_current(neuron, activation * 2.0f);
            
            // Age the trace
            memory_timestamps[i]++;
            
            // Check if trace is too old
            if (memory_timestamps[i] > 1000) {
                activation *= (1.0f - decay_rate);
            }
            
            memory_activations[i] = activation;
        }
    }
    
    // Decay weak traces
    decay_weak_traces();
    
    // Run competition to select winners
    run_competition();
}

void NeuralWorkingMemory::clear() {
    memory_neurons.clear();
    memory_activations.clear();
    memory_timestamps.clear();
    active_traces.clear();
    pImpl->maintenance_synapses.clear();
}

void NeuralWorkingMemory::strengthen_memory(float factor) {
    for (auto& activation : memory_activations) {
        activation = std::min(1.0f, activation * factor);
    }
}

void NeuralWorkingMemory::run_competition() {
    winners.clear();
    
    if (memory_activations.empty()) return;
    
    // Find neurons with above-threshold activation
    float threshold = 0.3f;
    
    for (size_t i = 0; i < memory_neurons.size(); ++i) {
        if (memory_activations[i] >= threshold) {
            winners.push_back(memory_neurons[i]);
        }
    }
    
    // Inhibitory competition - suppress non-winners
    for (size_t i = 0; i < memory_neurons.size(); ++i) {
        bool is_winner = std::find(winners.begin(), winners.end(), memory_neurons[i]) != winners.end();
        
        if (!is_winner && brain) {
            // Apply strong inhibition
            brain->inject_current(memory_neurons[i], -memory_activations[i] * 3.0f);
        }
    }
}

bool NeuralWorkingMemory::is_winning(NeuronId neuron) const {
    return std::find(winners.begin(), winners.end(), neuron) != winners.end();
}

float NeuralWorkingMemory::get_memory_activity() const {
    if (memory_activations.empty()) return 0.0f;
    
    float total = 0.0f;
    for (float act : memory_activations) {
        total += act;
    }
    return total / memory_activations.size();
}

void NeuralWorkingMemory::create_recurrent_connection(NeuronId from, NeuronId to, float strength) {
    // Check if connection already exists
    for (const auto& conn : pImpl->maintenance_synapses) {
        if (conn.first == from && conn.second == to) return;
    }
    
    pImpl->maintenance_synapses.emplace_back(from, to);
}

void NeuralWorkingMemory::update_recurrent_connections() {
    // Apply maintenance currents through recurrent connections
    for (const auto& conn : pImpl->maintenance_synapses) {
        float from_activation = 0.0f;
        
        // Find from neuron activation
        auto it = std::find(memory_neurons.begin(), memory_neurons.end(), conn.first);
        if (it != memory_neurons.end()) {
            size_t idx = std::distance(memory_neurons.begin(), it);
            from_activation = memory_activations[idx];
        }
        
        if (from_activation > 0.1f && brain) {
            // Send maintenance signal
            brain->inject_current(conn.second, from_activation * 2.0f);
        }
    }
}

void NeuralWorkingMemory::decay_weak_traces() {
    std::vector<size_t> to_remove;
    
    for (size_t i = 0; i < memory_activations.size(); ++i) {
        memory_activations[i] *= (1.0f - decay_rate);
        
        if (memory_activations[i] < 0.01f) {
            to_remove.push_back(i);
        }
    }
    
    // Remove weak traces (in reverse order to maintain indices)
    for (auto it = to_remove.rbegin(); it != to_remove.rend(); ++it) {
        memory_neurons.erase(memory_neurons.begin() + *it);
        memory_activations.erase(memory_activations.begin() + *it);
        memory_timestamps.erase(memory_timestamps.begin() + *it);
    }
}

// AttentionalSelection Implementation
struct AttentionalSelection::Impl {
    // Neuron competition state
    std::vector<float> competition_strength;
    std::vector<float> inhibition_level;
    
    Impl() {}
};

AttentionalSelection::AttentionalSelection()
    : pImpl(new Impl)
    , brain(nullptr)
    , inhibition_strength(0.5f)
    , excitation_strength(1.5f)
    , competition_threshold(0.3f)
{
}

AttentionalSelection::~AttentionalSelection() = default;

void AttentionalSelection::initialize(Brain* brain) {
    pImpl = std::make_unique<Impl>();
    this->brain = brain;
    NLM_LOG_INFO("AttentionalSelection initialized");
}

std::vector<NeuronId> AttentionalSelection::process_competition(const std::vector<NeuronId>& competitors,
                                                               float global_inhibition) {
    winners.clear();
    
    if (competitors.empty()) return winners;
    
    // Compute activity levels for competitors
    std::vector<float> activities(competitors.size(), 0.0f);
    float total_activity = 0.0f;
    
    for (size_t i = 0; i < competitors.size(); ++i) {
        NeuronId neuron = competitors[i];
        
        // Get current activation from salience and top-down bias (from maps)
        auto sal_it = bottom_up_salience.find(neuron.get_id());
        float salience = (sal_it != bottom_up_salience.end()) ? sal_it->second : 0.0f;
        auto bias_it = top_down_bias.find(neuron.get_id());
        float bias = (bias_it != top_down_bias.end()) ? bias_it->second : 0.0f;
        
        activities[i] = salience + bias;
        total_activity += activities[i];
    }
    
    if (total_activity < 0.001f) {
        // No strong competitors - all equal
        return competitors;
    }
    
    // Competition: neurons inhibit each other based on relative activity
    for (size_t i = 0; i < competitors.size(); ++i) {
        for (size_t j = 0; j < competitors.size(); ++j) {
            if (i == j) continue;
            
            float relative_activity = activities[i] / (activities[j] + 0.001f);
            
            if (relative_activity > 1.5f) {
                // i is much stronger than j - apply inhibition to j
                if (brain) {
                    brain->inject_current(competitors[j], -global_inhibition * inhibition_strength);
                }
                pImpl->inhibition_level.push_back(global_inhibition * inhibition_strength);
            }
        }
    }
    
    // Winners are neurons with above-threshold activity
    float threshold = competition_threshold * total_activity / competitors.size();
    
    for (size_t i = 0; i < competitors.size(); ++i) {
        if (activities[i] >= threshold) {
            winners.push_back(competitors[i]);
            
            // Apply excitation to winners
            if (brain) {
                brain->inject_current(competitors[i], excitation_strength * 2.0f);
            }
        }
    }
    
    return winners;
}

void AttentionalSelection::focus_on_region(RegionId region) {
    if (std::find(attended_regions.begin(), attended_regions.end(), region) == attended_regions.end()) {
        attended_regions.push_back(region);
    }
}

void AttentionalSelection::release_attention() {
    attended_regions.clear();
}

std::vector<RegionId> AttentionalSelection::get_attended_regions() const {
    return attended_regions;
}

void AttentionalSelection::set_inhibition_strength(float strength) {
    inhibition_strength = strength;
}

void AttentionalSelection::set_excitation_strength(float strength) {
    excitation_strength = strength;
}

void AttentionalSelection::set_competition_threshold(float threshold) {
    competition_threshold = threshold;
}

float AttentionalSelection::get_inhibition_for(NeuronId neuron) const {
    auto it = std::find(winners.begin(), winners.end(), neuron);
    if (it != winners.end()) {
        return 0.0f;  // Winners don't receive inhibition
    }
    return inhibition_strength;
}

float AttentionalSelection::get_excitation_for(NeuronId neuron) const {
    auto it = std::find(winners.begin(), winners.end(), neuron);
    if (it != winners.end()) {
        return excitation_strength;
    }
    return 0.0f;
}

void AttentionalSelection::update(TimestepDuration dt) {
    // Decay salience and bias over time
    for (auto& s : bottom_up_salience) {
        s *= 0.95f;
    }
    
    for (auto& b : top_down_bias) {
        b *= 0.98f;
    }
    
    // Decay inhibition
    for (auto& i : pImpl->inhibition_level) {
        i *= 0.9f;
    }
}

bool AttentionalSelection::is_attended(NeuronId neuron) const {
    return std::find(winners.begin(), winners.end(), neuron) != winners.end();
}

void AttentionalSelection::apply_top_down_bias(NeuronId neuron, float bias_strength) {
    // Store bias in map keyed by NeuronId
    auto it = top_down_bias.find(neuron.get_id());
    if (it != top_down_bias.end()) {
        it->second += bias_strength;
    } else {
        top_down_bias[neuron.get_id()] = bias_strength;
    }
}

void AttentionalSelection::apply_bottom_up_salience(NeuronId neuron, float salience_strength) {
    // Store salience in map keyed by NeuronId
    auto it = bottom_up_salience.find(neuron.get_id());
    if (it != bottom_up_salience.end()) {
        it->second += salience_strength;
    } else {
        bottom_up_salience[neuron.get_id()] = salience_strength;
    }
}

void AttentionalSelection::reset() {
    winners.clear();
    attended_regions.clear();
    neuron_salience.clear();
    top_down_bias.clear();
    bottom_up_salience.clear();
    pImpl->inhibition_level.clear();
}

} // namespace nlm