#ifndef CUSTOM_PLASTICITY_HPP
#define CUSTOM_PLASTICITY_HPP

#include <vector>
#include <string>
#include <map>
#include <functional>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <chrono>

namespace nlm {
namespace advanced {

// Plasticity rule type enumeration
enum class PlasticityRuleType {
    STDP,
    HEBBIAN,
    ANTI_HEBBIAN,
    OJA,
    BCM,
    HOMOCLASTIC,
    MODULATORY,
    CUSTOM
};

// Spike timing difference structure
struct SpikeTiming {
    double pre_spike_time;      // Time of presynaptic spike
    double post_spike_time;     // Time of postsynaptic spike
    double time_difference;     // Pre - Post time difference (positive if pre before post)
    double weight_change;       // Predicted weight change
};

// Synaptic state for plasticity
struct SynapticState {
    double weight;                    // Current synaptic weight
    double history_weight;             // Historical weight for homeostasis
    double last_pre_spike_time;       // Time of last presynaptic spike
    double last_post_spike_time;      // Time of last postsynaptic spike
    double eligibility_trace;          // STDP eligibility trace
    double modulatory_signal;          // Modulatory signal (e.g., dopamine)
    double homeostasis_factor;         // Homeostatic scaling factor
    std::map<std::string, double> additional_state;
};

// Plasticity rule interface
class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    virtual std::string getName() const = 0;
    virtual PlasticityRuleType getType() const = 0;
    virtual double computeWeightChange(const SpikeTiming& spike_timing,
                                       const SynapticState& state,
                                       const std::map<std::string, double>& context) = 0;
    virtual void updateState(SynapticState& state, double dt,
                           const std::map<std::string, double>& context) = 0;
    virtual void reset() = 0;
    virtual PlasticityRule* clone() const = 0;
};

// STDP plasticity rule implementation
class STDPPlasticityRule : public PlasticityRule {
private:
    double learning_rate;              // Overall learning rate
    double tau_pre;                    // Presynaptic trace time constant
    double tau_post;                   // Postsynaptic trace time constant
    double A_pre;                      // Presynaptic learning rate
    double A_post;                     // Postsynaptic learning rate
    double w_max;                      // Maximum weight
    double w_min;                      // Minimum weight

public:
    STDPPlasticityRule(double learning_rate = 0.01, double tau_pre = 20.0,
                     double tau_post = 20.0, double A_pre = 0.01,
                     double A_post = -0.012, double w_max = 1.0, double w_min = 0.0)
        : learning_rate(learning_rate), tau_pre(tau_pre), tau_post(tau_post),
          A_pre(A_pre), A_post(A_post), w_max(w_max), w_min(w_min) {}

    std::string getName() const override { return "STDP"; }
    PlasticityRuleType getType() const override { return PlasticityRuleType::STDP; }

    double computeWeightChange(const SpikeTiming& spike_timing,
                               const SynapticState& state,
                               const std::map<std::string, double>& context) override {
        double dt_pre = std::chrono::duration<double>::zero().count();
        double dt_post = std::chrono::duration<double>::zero().count();

        // Calculate weight change based on spike timing
        double weight_change = 0.0;

        if (spike_timing.pre_spike_time > 0) {
            double delta_t = spike_timing.time_difference;
            if (delta_t > 0) { // Pre before post (LTP)
                weight_change += A_pre * std::exp(-delta_t / tau_pre);
            } else { // Post before pre (LTD)
                weight_change += A_post * std::exp(delta_t / tau_post);
            }
        }

        // Apply learning rate
        weight_change *= learning_rate;

        // Apply bounds
        weight_change = std::max(-state.weight, std::min(w_max - state.weight, weight_change));

        return weight_change;
    }

    void updateState(SynapticState& state, double dt,
                    const std::map<std::string, double>& context) override {
        // Update STDP traces
        if (state.last_pre_spike_time > 0) {
            double delta_t = dt - (state.last_pre_spike_time > 0 ? state.last_pre_spike_time : 0);
            if (state.eligibility_trace > 0) {
                state.eligibility_trace *= std::exp(-delta_t / tau_pre);
            }
        }

        if (state.last_post_spike_time > 0) {
            double delta_t = dt - (state.last_post_spike_time > 0 ? state.last_post_spike_time : 0);
            if (state.eligibility_trace < 0) {
                state.eligibility_trace *= std::exp(-delta_t / tau_post);
            }
        }

        // Update homeostasis
        double target_weight = context.count("target_weight") ? context.at("target_weight") : 0.5;
        double homeostatic_strength = context.count("homeostatic_strength") ? 
            context.at("homeostatic_strength") : 0.1;

        double weight_diff = target_weight - state.weight;
        state.homeostasis_factor = homeostatic_strength * weight_diff;
    }

    void reset() override {
        // Reset STDP-specific state
    }

    PlasticityRule* clone() const override {
        return new STDPPlasticityRule(learning_rate, tau_pre, tau_post, A_pre, A_post, w_max, w_min);
    }
};

// Hebbian plasticity rule implementation
class HebbianPlasticityRule : public PlasticityRule {
private:
    double learning_rate;
    double threshold;                   // Weight threshold for potentiation
    double decay_rate;                  // Weight decay rate

public:
    HebbianPlasticityRule(double learning_rate = 0.001, double threshold = 0.5,
                         double decay_rate = 0.0001)
        : learning_rate(learning_rate), threshold(threshold), decay_rate(decay_rate) {}

    std::string getName() const override { return "Hebbian"; }
    PlasticityRuleType getType() const override { return PlasticityRuleType::HEBBIAN; }

    double computeWeightChange(const SpikeTiming& spike_timing,
                               const SynapticState& state,
                               const std::map<std::string, double>& context) override {
        double weight_change = 0.0;

        // Simple Hebbian: weight increases when both neurons fire simultaneously
        if (spike_timing.pre_spike_time > 0 && spike_timing.post_spike_time > 0) {
            double temporal_proximity = 1.0 / (1.0 + std::abs(spike_timing.time_difference));
            weight_change = learning_rate * temporal_proximity * state.weight * (1.0 - state.weight);
        }

        // Anti-Hebbian variant
        if (context.count("anti_hebbian") && context.at("anti_hebbian")) {
            weight_change *= -1.0;
        }

        return weight_change;
    }

    void updateState(SynapticState& state, double dt,
                    const std::map<std::string, double>& context) override {
        // Apply weight decay
        state.weight *= (1.0 - decay_rate * dt);

        // Update modulatory signals
        if (context.count("modulatory_signal")) {
            state.modulatory_signal = context.at("modulatory_signal");
        }
    }

    void reset() override {
        // Reset Hebbian-specific state
    }

    PlasticityRule* clone() const override {
        return new HebbianPlasticityRule(learning_rate, threshold, decay_rate);
    }
};

// Custom plasticity rule factory
class PlasticityRuleFactory {
public:
    static std::unique_ptr<PlasticityRule> createRule(PlasticityRuleType type,
                                                      const std::map<std::string, double>& params = {});
    static std::unique_ptr<PlasticityRule> createCustomRule(const std::string& rule_name,
                                                            const std::string& code);
};

// Advanced plasticity manager
class AdvancedPlasticityManager {
private:
    std::map<size_t, SynapticState> synaptic_states_;
    std::map<size_t, std::unique_ptr<PlasticityRule>> plasticity_rules_;
    std::mutex mutex_;
    double simulation_time_;
    std::map<std::string, double> global_context_;

public:
    AdvancedPlasticityManager() : simulation_time_(0.0) {}

    // Synapse management
    size_t addSynapse(const SynapticState& initial_state);
    bool removeSynapse(size_t synapse_id);
    bool updateSynapseState(size_t synapse_id, const SynapticState& state);

    // Plasticity rule management
    bool setPlasticityRule(size_t synapse_id, std::unique_ptr<PlasticityRule> rule);
    bool setPlasticityRule(size_t synapse_id, PlasticityRuleType type,
                         const std::map<std::string, double>& params = {});
    std::string getPlasticityRuleName(size_t synapse_id) const;

    // Simulation step
    void simulate(double dt);
    void recordSpikes(size_t pre_neuron, size_t post_neuron, double time);

    // State management
    void reset();
    void setGlobalContext(const std::map<std::string, double>& context);
    double getSimulationTime() const { return simulation_time_; }

    // Analysis and statistics
    std::vector<double> getWeightHistory(size_t synapse_id) const;
    std::vector<double> getPlasticityTrace(size_t synapse_id) const;
    double getAveragePlasticityRate(size_t synapse_id) const;
    double getTotalPlasticityChange(size_t synapse_id) const;

    // Serialization
    bool saveToFile(const std::string& filename) const;
    static std::unique_ptr<AdvancedPlasticityManager> loadFromFile(const std::string& filename);
};

} // namespace advanced
} // namespace nlm

#endif // CUSTOM_PLASTICITY_HPP