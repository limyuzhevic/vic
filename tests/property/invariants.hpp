// Invariant Testing Framework
// Property-based invariant testing for NLM neural dynamics

#include "core/Types/Types.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include "plasticity/PlasticityRule.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <limits>

namespace invariants {

    // Forward declarations
    class Invariant;
    using InvariantPtr = std::unique_ptr<Invariant>;

    // Base class for all invariants
    class Invariant {
    public:
        virtual ~Invariant() = default;
        virtual std::string name() const = 0;
        virtual bool check(const std::string& component, const void* state) const = 0;
        virtual std::string description() const = 0;
        virtual std::string get_violation_description(const std::string& component, const void* state) const {
            return "Invariant violated";
        }
    };

    // Membrane potential bounds invariant
    class MembranePotentialBounds : public Invariant {
    public:
        std::string name() const override {
            return "MembranePotentialBounds";
        }
        
        bool check(const std::string& component, const void* state) const override {
            if (component != "neuron") return true;
            
            const auto* neuron_state = static_cast<const NeuronState*>(state);
            
            // Check that membrane potential stays within biological bounds
            // Typical range: -100 mV to +50 mV, with resting around -70 mV
            return neuron_state->membrane_potential >= -100.0f &&
                   neuron_state->membrane_potential <= 50.0f;
        }
        
        std::string description() const override {
            return "Membrane potential must remain within biological bounds (-100mV to 50mV)";
        }
        
        std::string get_violation_description(const std::string& component, const void* state) const override {
            const auto* neuron_state = static_cast<const NeuronState*>(state);
            std::stringstream ss;
            ss << "Membrane potential " << neuron_state->membrane_potential << " mV "
               << "outside valid range [-100mV, 50mV]";
            return ss.str();
        }
    };

    // Charge conservation invariant
    class ChargeConservation : public Invariant {
    public:
        std::string name() const override {
            return "ChargeConservation";
        }
        
        bool check(const std::string& component, const void* state) const override {
            if (component != "brain") return true;
            
            const auto* brain_state = static_cast<const BrainState*>(state);
            
            // Approximate charge conservation: total current should not diverge to infinity
            // In a realistic neural network, total synaptic current should be bounded
            float total_current = brain_state->total_excitatory_current + 
                                 brain_state->total_inhibitory_current;
            
            // Check for extreme values that would violate conservation
            return std::abs(total_current) < 1000.0f;  // Reasonable upper bound
        }
        
        std::string description() const override {
            return "Total synaptic current must remain within reasonable bounds";
        }
    };

    // Spike timing consistency invariant
    class SpikeTimingConsistency : public Invariant {
    public:
        std::string name() const override {
            return "SpikeTimingConsistency";
        }
        
        bool check(const std::string& component, const void* state) const override {
            if (component != "neuron") return true;
            
            const auto* neuron_state = static_cast<const NeuronState*>(state);
            
            // Check that spike times are monotonic within a neuron
            if (neuron_state->spike_times.size() < 2) return true;
            
            for (size_t i = 1; i < neuron_state->spike_times.size(); ++i) {
                if (neuron_state->spike_times[i] < neuron_state->spike_times[i-1]) {
                    return false;  // Spike times must be non-decreasing
                }
            }
            
            return true;
        }
        
        std::string description() const override {
            return "Spike times must be monotonic (non-decreasing) for each neuron";
        }
    };

    // Synapse weight bounds invariant
    class SynapseWeightBounds : public Invariant {
    public:
        std::string name() const override {
            return "SynapseWeightBounds";
        }
        
        bool check(const std::string& component, const void* state) const override {
            if (component != "synapse") return true;
            
            const auto* synapse_state = static_cast<const SynapseState*>(state);
            
            // Check that synapse weights remain within reasonable bounds
            // Typical range: -1.0 to +1.0 for normalized weights
            return synapse_state->weight >= -2.0f && synapse_state->weight <= 2.0f;
        }
        
        std::string description() const override {
            return "Synapse weights must remain within reasonable bounds (-2.0 to 2.0)";
        }
    };

    // Plasticity stability invariant
    class PlasticityStability : public Invariant {
    public:
        std::string name() const override {
            return "PlasticityStability";
        }
        
        bool check(const std::string& component, const void* state) const override {
            if (component != "plasticity") return true;
            
            const auto* plasticity_state = static_cast<const PlasticityState*>(state);
            
            // Check that plasticity parameters don't cause instability
            if (plasticity_state->learning_rate > 1.0f) {
                return false;  // Learning rate too high
            }
            
            // Check for runaway plasticity
            if (plasticity_state->weight_change_accumulator > 10.0f || 
                plasticity_state->weight_change_accumulator < -10.0f) {
                return false;
            }
            
            return true;
        }
        
        std::string description() const override {
            return "Plasticity parameters must remain stable and prevent runaway changes";
        }
    };

    // Memory consistency invariant
    class MemoryConsistency : public Invariant {
    public:
        std::string name() const override {
            return "MemoryConsistency";
        }
        
        bool check(const std::string& component, const void* state) const override {
            if (component != "memory") return true;
            
            const auto* memory_state = static_cast<const MemoryState*>(state);
            
            // Check memory system consistency
            if (memory_state->working_memory_active_traces > 1000) {
                return false;  // Too many active traces
            }
            
            if (memory_state->episodic_memory_count > 10000) {
                return false;  // Too many episodic memories
            }
            
            return true;
        }
        
        std::string description() const override {
            return "Memory system state must remain within reasonable bounds";
        }
    };

    // Neuromodulation balance invariant
    class NeuromodulationBalance : public Invariant {
    public:
        std::string name() const override {
            return "NeuromodulationBalance";
        }
        
        bool check(const std::string& component, const void* state) const override {
            if (component != "neuromodulation") return true;
            
            const auto* neuromodulation_state = static_cast<const NeuromodulationState*>(state);
            
            // Check that neuromodulator levels remain balanced
            if (neuromodulation_state->dopamine_level > 10.0f ||
                neuromodulation_state->dopamine_level < 0.0f) {
                return false;
            }
            
            if (neuromodulation_state->curiosity_level > 10.0f ||
                neuromodulation_state->curiosity_level < 0.0f) {
                return false;
            }
            
            if (neuromodulation_state->novelty_level > 10.0f ||
                neuromodulation_state->novelty_level < 0.0f) {
                return false;
            }
            
            return true;
        }
        
        std::string description() const override {
            return "Neuromodulator levels must remain within reasonable bounds";
        }
    };

    // Invariant checker for comprehensive testing
    class InvariantChecker {
    public:
        struct CheckResult {
            std::string invariant_name;
            std::string component;
            bool passed;
            std::string violation_message;
            double execution_time;
        };
        
        InvariantChecker() {
            // Register all invariants
            invariants_.push_back(std::make_unique<MembranePotentialBounds>());
            invariants_.push_back(std::make_unique<ChargeConservation>());
            invariants_.push_back(std::make_unique<SpikeTimingConsistency>());
            invariants_.push_back(std::make_unique<SynapseWeightBounds>());
            invariants_.push_back(std::make_unique<PlasticityStability>());
            invariants_.push_back(std::make_unique<MemoryConsistency>());
            invariants_.push_back(std::make_unique<NeuromodulationBalance>());
        }
        
        std::vector<CheckResult> check_all_invariants(
            const std::string& component, 
            const std::vector<void*>& states) const {
            
            std::vector<CheckResult> results;
            
            for (const auto& invariant : invariants_) {
                for (const auto& state : states) {
                    CheckResult result;
                    result.invariant_name = invariant->name();
                    result.component = component;
                    result.passed = invariant->check(component, state);
                    result.violation_message = result.passed ? "" : 
                        invariant->get_violation_description(component, state);
                    result.execution_time = 0.0; // TODO: Add timing
                    
                    results.push_back(result);
                }
            }
            
            return results;
        }
        
        bool all_invariants_pass(const std::string& component, 
                               const std::vector<void*>& states) const {
            for (const auto& invariant : invariants_) {
                for (const auto& state : states) {
                    if (!invariant->check(component, state)) {
                        return false;
                    }
                }
            }
            return true;
        }
        
        std::vector<std::string> get_failed_invariant_names(
            const std::string& component,
            const std::vector<void*>& states) const {
            
            std::vector<std::string> failed;
            
            for (const auto& invariant : invariants_) {
                for (const auto& state : states) {
                    if (!invariant->check(component, state)) {
                        failed.push_back(invariant->name() + " (" + component + ")");
                    }
                }
            }
            
            return failed;
        }
        
        size_t get_invariant_count() const {
            return invariants_.size();
        }
        
        std::vector<std::string> get_all_invariant_names() const {
            std::vector<std::string> names;
            for (const auto& invariant : invariants_) {
                names.push_back(invariant->name());
            }
            return names;
        }
        
    private:
        std::vector<InvariantPtr> invariants_;
    };

    // Convenience functions for invariant testing
    namespace test_helpers {
        
        // Create neuron state for testing
        NeuronState create_neuron_state(float potential = -70.0f, 
                                         int spike_count = 0) {
            NeuronState state;
            state.neuron_id = NeuronId(1);
            state.membrane_potential = potential;
            state.spike_count = spike_count;
            
            // Add some spike times if spike_count > 0
            for (int i = 0; i < spike_count; ++i) {
                state.spike_times.push_back(static_cast<float>(i * 10.0));
            }
            
            return state;
        }
        
        // Create synapse state for testing
        SynapseState create_synapse_state(float weight = 0.5f, 
                                           float delay = 1.0f) {
            SynapseState state;
            state.synapse_id = SynapseId(100);
            state.weight = weight;
            state.delay = delay;
            return state;
        }
        
        // Create brain state for testing
        BrainState create_brain_state() {
            BrainState state;
            state.neuron_count = 100;
            state.simulation_step = 1000;
            state.total_excitatory_current = 10.0f;
            state.total_inhibitory_current = -5.0f;
            return state;
        }
        
        // Create plasticity state for testing
        PlasticityState create_plasticity_state(float learning_rate = 0.01f) {
            PlasticityState state;
            state.learning_rate = learning_rate;
            state.weight_change_accumulator = 0.0f;
            return state;
        }
        
        // Create memory state for testing
        MemoryState create_memory_state() {
            MemoryState state;
            state.working_memory_active_traces = 10;
            state.episodic_memory_count = 100;
            state.semantic_memory_size = 500;
            return state;
        }
        
        // Create neuromodulation state for testing
        NeuromodulationState create_neuromodulation_state() {
            NeuromodulationState state;
            state.dopamine_level = 1.0f;
            state.curiosity_level = 0.5f;
            state.novelty_level = 0.3f;
            return state;
        }
        
    } // namespace test_helpers

} // namespace invariants