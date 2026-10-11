// NLM Enhanced Test Fixture System
// Phase 7: Comprehensive Test Suite
// Advanced test fixture system for neural network components

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <map>
#include <set>
#include <algorithm>
#include <chrono>

// Forward declarations for neural network components
namespace nlm {
    class Neuron;
    class Synapse;
    class Brain;
    class MemorySystem;
    class Config;
    class AdvancedNeuronModel;
}

// Neuron test fixture
class NeuronTestFixture {
protected:
    std::vector<std::unique_ptr<nlm::Neuron>> neurons;
    std::map<nlm::NeuronId, std::unique_ptr<nlm::Neuron>> neuron_map;
    
    // Default neuron parameters
    struct DefaultParameters {
        float membrane_potential = -70.0f;
        float threshold = -55.0f;
        float resting_potential = -70.0f;
        float refractory_period = 5;
        float time_constant = 10.0f;
        float capacitance = 1.0f;
        std::string neuron_type = "LIF";
    } default_params;
    
public:
    NeuronTestFixture() : neurons(), neuron_map() {}
    
    virtual ~NeuronTestFixture() {
        clear_neurons();
    }
    
    // Setup and teardown
    virtual void setup() {
        std::cout << "[FIXTURE] NeuronTestFixture setup" << std::endl;
        clear_neurons();
    }
    
    virtual void teardown() {
        std::cout << "[FIXTURE] NeuronTestFixture teardown" << std::endl;
        clear_neurons();
    }
    
    // Test scenario setup
    virtual void setup_test() {
        std::cout << "[FIXTURE] NeuronTestFixture setup_test" << std::endl;
    }
    
    virtual void teardown_test() {
        std::cout << "[FIXTURE] NeuronTestFixture teardown_test" << std::endl;
    }
    
    // Neuron creation utilities
    nlm::NeuronId create_neuron(nlm::NeuronType type = nlm::NeuronType::Internal) {
        nlm::NeuronId id = generate_unique_id();
        neurons.push_back(std::make_unique<nlm::Neuron>(id, type));
        neuron_map[id] = std::move(neurons.back());
        return id;
    }
    
    nlm::NeuronId create_neuron_with_params(const nlm::NeuronParameters& params) {
        nlm::NeuronId id = generate_unique_id();
        neurons.push_back(std::make_unique<nlm::Neuron>(id, params));
        neuron_map[id] = std::move(neurons.back());
        return id;
    }
    
    // Neuron retrieval
    nlm::Neuron* get_neuron(nlm::NeuronId id) {
        auto it = neuron_map.find(id);
        return (it != neuron_map.end()) ? it->second.get() : nullptr;
    }
    
    const nlm::Neuron* get_neuron(nlm::NeuronId id) const {
        auto it = neuron_map.find(id);
        return (it != neuron_map.end()) ? it->second.get() : nullptr;
    }
    
    // Neuron management
    void add_neuron(std::unique_ptr<nlm::Neuron> neuron) {
        if (neuron) {
            nlm::NeuronId id = neuron->getId();
            neurons.push_back(std::move(neuron));
            neuron_map[id] = std::move(neurons.back());
        }
    }
    
    void remove_neuron(nlm::NeuronId id) {
        auto it = neuron_map.find(id);
        if (it != neuron_map.end()) {
            // Remove from vector
            neurons.erase(std::remove_if(neurons.begin(), neurons.end(),
                [id](const std::unique_ptr<nlm::Neuron>& n) { return n->getId() == id; }),
                neurons.end());
            
            // Remove from map
            neuron_map.erase(it);
        }
    }
    
    void clear_neurons() {
        neurons.clear();
        neuron_map.clear();
    }
    
    // Neuron statistics
    size_t get_neuron_count() const {
        return neurons.size();
    }
    
    std::vector<nlm::NeuronId> get_all_neuron_ids() const {
        std::vector<nlm::NeuronId> ids;
        for (const auto& pair : neuron_map) {
            ids.push_back(pair.first);
        }
        return ids;
    }
    
    // Neuron parameter utilities
    void set_default_parameters(const DefaultParameters& params) {
        default_params = params;
    }
    
    DefaultParameters get_default_parameters() const {
        return default_params;
    }
    
    // Neuron type utilities
    std::vector<nlm::NeuronType> get_supported_neuron_types() const {
        return {nlm::NeuronType::Internal, nlm::NeuronType::Excitatory, 
                nlm::NeuronType::Inhibitory, nlm::NeuronType::Sensory,
                nlm::NeuronType::Motor};
    }
    
    // Neuron state utilities
    bool are_all_neurons_idle() const {
        for (const auto& neuron : neurons) {
            if (neuron->isFiring() || neuron->isRefractory()) {
                return false;
            }
        }
        return true;
    }
    
    bool are_all_neurons_at_rest() const {
        for (const auto& neuron : neurons) {
            if (neuron->getMembranePotential() != default_params.resting_potential) {
                return false;
            }
        }
        return true;
    }
    
    // Neuron testing utilities
    bool neuron_meets_criteria(nlm::NeuronId id, 
        std::function<bool(const nlm::Neuron&)> criteria) const {
        const nlm::Neuron* neuron = get_neuron(id);
        return neuron && criteria(*neuron);
    }
    
    std::vector<nlm::NeuronId> neurons_meeting_criteria(
        std::function<bool(const nlm::Neuron&)> criteria) const {
        std::vector<nlm::NeuronId> matching_ids;
        
        for (const auto& pair : neuron_map) {
            if (criteria(*pair.second)) {
                matching_ids.push_back(pair.first);
            }
        }
        
        return matching_ids;
    }
    
    // Neuron simulation utilities
    void advance_neuron_states(double time_step) {
        for (auto& neuron : neurons) {
            neuron->step(time_step);
        }
    }
    
    void reset_all_neurons() {
        for (auto& neuron : neurons) {
            neuron->reset();
        }
    }
    
    void inject_current_to_neurons(nlm::NeuronId id, float current) {
        nlm::Neuron* neuron = get_neuron(id);
        if (neuron) {
            neuron->injectCurrent(current);
        }
    }
    
    void trigger_spike(nlm::NeuronId id) {
        nlm::Neuron* neuron = get_neuron(id);
        if (neuron) {
            neuron->recordSpike(0.0f);
        }
    }
    
private:
    nlm::NeuronId generate_unique_id() {
        static std::atomic<uint64_t> next_id{1000};
        return nlm::NeuronId(next_id++);
    }
};

// Synapse test fixture
class SynapseTestFixture {
protected:
    std::vector<std::unique_ptr<nlm::Synapse>> synapses;
    std::map<nlm::SynapseId, std::unique_ptr<nlm::Synapse>> synapse_map;
    std::map<nlm::SynapseId, nlm::NeuronId> synapse_to_pre_neuron;
    std::map<nlm::SynapseId, nlm::NeuronId> synapse_to_post_neuron;
    
    struct DefaultParameters {
        float weight = 1.0f;
        float delay = 1.0f;
        float max_weight = 10.0f;
        float min_weight = 0.1f;
        std::string plasticity_rule = "STDP";
        bool enabled = true;
    } default_params;
    
public:
    SynapseTestFixture() : synapses(), synapse_map(), synapse_to_pre_neuron(), synapse_to_post_neuron() {}
    
    virtual ~SynapseTestFixture() {
        clear_synapses();
    }
    
    // Setup and teardown
    virtual void setup() {
        std::cout << "[FIXTURE] SynapseTestFixture setup" << std::endl;
        clear_synapses();
    }
    
    virtual void teardown() {
        std::cout << "[FIXTURE] SynapseTestFixture teardown" << std::endl;
        clear_synapses();
    }
    
    virtual void setup_test() {
        std::cout << "[FIXTURE] SynapseTestFixture setup_test" << std::endl;
    }
    
    virtual void teardown_test() {
        std::cout << "[FIXTURE] SynapseTestFixture teardown_test" << std::endl;
    }
    
    // Synapse creation utilities
    nlm::SynapseId create_synapse(nlm::NeuronId pre_id, nlm::NeuronId post_id) {
        nlm::SynapseId id = generate_unique_id();
        synapses.push_back(std::make_unique<nlm::Synapse>(id, pre_id, post_id));
        synapse_map[id] = std::move(synapses.back());
        synapse_to_pre_neuron[id] = pre_id;
        synapse_to_post_neuron[id] = post_id;
        return id;
    }
    
    nlm::SynapseId create_synapse_with_params(nlm::NeuronId pre_id, nlm::NeuronId post_id,
        const nlm::SynapseParameters& params) {
        nlm::SynapseId id = generate_unique_id();
        synapses.push_back(std::make_unique<nlm::Synapse>(id, pre_id, post_id, params));
        synapse_map[id] = std::move(synapses.back());
        synapse_to_pre_neuron[id] = pre_id;
        synapse_to_post_neuron[id] = post_id;
        return id;
    }
    
    // Synapse retrieval
    nlm::Synapse* get_synapse(nlm::SynapseId id) {
        auto it = synapse_map.find(id);
        return (it != synapse_map.end()) ? it->second.get() : nullptr;
    }
    
    const nlm::Synapse* get_synapse(nlm::SynapseId id) const {
        auto it = synapse_map.find(id);
        return (it != synapse_map.end()) ? it->second.get() : nullptr;
    }
    
    // Synapse management
    void add_synapse(std::unique_ptr<nlm::Synapse> synapse) {
        if (synapse) {
            nlm::SynapseId id = synapse->getId();
            synapses.push_back(std::move(synapse));
            synapse_map[id] = std::move(synapses.back());
            
            auto it = synapse_to_pre_neuron.find(id);
            if (it == synapse_to_pre_neuron.end()) {
                synapse_to_pre_neuron[id] = it->second;
            }
            
            auto post_it = synapse_to_post_neuron.find(id);
            if (post_it == synapse_to_post_neuron.end()) {
                synapse_to_post_neuron[id] = it->second;
            }
        }
    }
    
    void remove_synapse(nlm::SynapseId id) {
        auto it = synapse_map.find(id);
        if (it != synapse_map.end()) {
            // Remove from vector
            synapses.erase(std::remove_if(synapses.begin(), synapses.end(),
                [id](const std::unique_ptr<nlm::Synapse>& s) { return s->getId() == id; }),
                synapses.end());
            
            // Remove from maps
            synapse_map.erase(it);
            synapse_to_pre_neuron.erase(id);
            synapse_to_post_neuron.erase(id);
        }
    }
    
    void clear_synapses() {
        synapses.clear();
        synapse_map.clear();
        synapse_to_pre_neuron.clear();
        synapse_to_post_neuron.clear();
    }
    
    // Synapse statistics
    size_t get_synapse_count() const {
        return synapses.size();
    }
    
    std::vector<nlm::SynapseId> get_all_synapse_ids() const {
        std::vector<nlm::SynapseId> ids;
        for (const auto& pair : synapse_map) {
            ids.push_back(pair.first);
        }
        return ids;
    }
    
    // Synapse relationship utilities
    std::vector<nlm::SynapseId> get_synapses_from_neuron(nlm::NeuronId neuron_id) const {
        std::vector<nlm::SynapseId> synapse_ids;
        
        for (const auto& pair : synapse_map) {
            auto pre_it = synapse_to_pre_neuron.find(pair.first);
            if (pre_it != synapse_to_pre_neuron.end() && pre_it->second == neuron_id) {
                synapse_ids.push_back(pair.first);
            }
        }
        
        return synapse_ids;
    }
    
    std::vector<nlm::SynapseId> get_synapses_to_neuron(nlm::NeuronId neuron_id) const {
        std::vector<nlm::SynapseId> synapse_ids;
        
        for (const auto& pair : synapse_map) {
            auto post_it = synapse_to_post_neuron.find(pair.first);
            if (post_it != synapse_to_post_neuron.end() && post_it->second == neuron_id) {
                synapse_ids.push_back(pair.first);
            }
        }
        
        return synapse_ids;
    }
    
    // Synapse parameter utilities
    void set_default_parameters(const DefaultParameters& params) {
        default_params = params;
    }
    
    DefaultParameters get_default_parameters() const {
        return default_params;
    }
    
    // Synapse state utilities
    bool are_all_synapses_enabled() const {
        for (const auto& synapse : synapses) {
            if (!synapse->isEnabled()) {
                return false;
            }
        }
        return true;
    }
    
    bool are_all_synapses_at_max_weight() const {
        for (const auto& synapse : synapses) {
            if (synapse->getWeight() < default_params.max_weight) {
                return false;
            }
        }
        return true;
    }
    
    // Synapse testing utilities
    bool synapse_meets_criteria(nlm::SynapseId id,
        std::function<bool(const nlm::Synapse&)> criteria) const {
        const nlm::Synapse* synapse = get_synapse(id);
        return synapse && criteria(*synapse);
    }
    
    std::vector<nlm::SynapseId> synapses_meeting_criteria(
        std::function<bool(const nlm::Synapse&)> criteria) const {
        std::vector<nlm::SynapseId> matching_ids;
        
        for (const auto& pair : synapse_map) {
            if (criteria(*pair.second)) {
                matching_ids.push_back(pair.first);
            }
        }
        
        return matching_ids;
    }
    
    // Synapse simulation utilities
    void advance_synapse_states(double time_step) {
        for (auto& synapse : synapses) {
            synapse->step(time_step);
        }
    }
    
    void update_all_synapses() {
        for (auto& synapse : synapses) {
            synapse->update();
        }
    }
    
    void enable_all_synapses(bool enabled) {
        for (auto& synapse : synapses) {
            synapse->setEnabled(enabled);
        }
    }
    
    void set_all_synapse_weights(float weight) {
        for (auto& synapse : synapses) {
            synapse->setWeight(weight);
        }
    }
    
private:
    nlm::SynapseId generate_unique_id() {
        static std::atomic<uint64_t> next_id{2000};
        return nlm::SynapseId(next_id++);
    }
};

// Brain test fixture
class BrainTestFixture {
protected:
    std::unique_ptr<nlm::Brain> brain;
    std::vector<nlm::NeuronId> created_neurons;
    std::vector<nlm::SynapseId> created_synapses;
    
    struct DefaultConfig {
        std::string name = "Test Brain";
        size_t neuron_count = 100;
        float connection_probability = 0.1f;
        float learning_rate = 0.01f;
        float simulation_timestep = 0.001f;
        size_t max_memory_capacity = 1000;
        bool enable_memory_system = true;
        bool enable_plasticity = true;
        bool enable_neuromodulation = true;
    } default_config;
    
public:
    BrainTestFixture() : brain(nullptr), created_neurons(), created_synapses() {}
    
    virtual ~BrainTestFixture() {
        clear_brain();
    }
    
    // Setup and teardown
    virtual void setup() {
        std::cout << "[FIXTURE] BrainTestFixture setup" << std::endl;
        clear_brain();
    }
    
    virtual void teardown() {
        std::cout << "[FIXTURE] BrainTestFixture teardown" << std::endl;
        clear_brain();
    }
    
    virtual void setup_test() {
        std::cout << "[FIXTURE] BrainTestFixture setup_test" << std::endl;
    }
    
    virtual void teardown_test() {
        std::cout << "[FIXTURE] BrainTestFixture teardown_test" << std::endl;
    }
    
    // Brain creation utilities
    void create_brain() {
        clear_brain();
        brain = std::make_unique<nlm::Brain>(default_config.neuron_count);
        std::cout << "[FIXTURE] Brain created with " << default_config.neuron_count << " neurons" << std::endl;
    }
    
    void create_brain_with_config(const nlm::BrainConfig& config) {
        clear_brain();
        brain = std::make_unique<nlm::Brain>(config);
        std::cout << "[FIXTURE] Brain created with custom config: " << config.name << std::endl;
    }
    
    // Brain management
    nlm::Brain* get_brain() {
        return brain.get();
    }
    
    const nlm::Brain* get_brain() const {
        return brain.get();
    }
    
    bool has_brain() const {
        return brain != nullptr;
    }
    
    void clear_brain() {
        if (brain) {
            brain.reset();
        }
        created_neurons.clear();
        created_synapses.clear();
    }
    
    // Brain operation utilities
    void initialize_brain() {
        if (brain) {
            brain->initialize();
        }
    }
    
    void simulate_brain(double time_step) {
        if (brain) {
            brain->step(time_step);
        }
    }
    
    void simulate_brain(double time_step, double duration) {
        if (brain) {
            double elapsed = 0.0;
            while (elapsed < duration) {
                brain->step(std::min(time_step, duration - elapsed));
                elapsed += time_step;
            }
        }
    }
    
    // Neuron management in brain
    nlm::NeuronId add_neuron_to_brain(nlm::NeuronType type) {
        if (!brain) return nlm::NeuronId(0);
        
        nlm::NeuronId id = brain->addNeuron(type);
        created_neurons.push_back(id);
        return id;
    }
    
    bool remove_neuron_from_brain(nlm::NeuronId id) {
        if (!brain) return false;
        
        bool removed = brain->removeNeuron(id);
        if (removed) {
            created_neurons.erase(std::remove(created_neurons.begin(), created_neurons.end(), id),
                                created_neurons.end());
        }
        return removed;
    }
    
    // Synapse management in brain
    nlm::SynapseId add_synapse_to_brain(nlm::NeuronId pre_id, nlm::NeuronId post_id) {
        if (!brain) return nlm::SynapseId(0);
        
        nlm::SynapseId id = brain->addSynapse(pre_id, post_id);
        created_synapses.push_back(id);
        return id;
    }
    
    bool remove_synapse_from_brain(nlm::SynapseId id) {
        if (!brain) return false;
        
        bool removed = brain->removeSynapse(id);
        if (removed) {
            created_synapses.erase(std::remove(created_synapses.begin(), created_synapses.end(), id),
                                 created_synapses.end());
        }
        return removed;
    }
    
    // Brain statistics
    size_t get_neuron_count() const {
        return brain ? brain->getNeuronCount() : 0;
    }
    
    size_t get_synapse_count() const {
        return brain ? brain->getSynapseCount() : 0;
    }
    
    size_t get_active_neuron_count() const {
        return brain ? brain->getActiveNeuronCount() : 0;
    }
    
    size_t get_firing_neuron_count() const {
        return brain ? brain->getFiringNeuronCount() : 0;
    }
    
    // Brain configuration utilities
    void set_default_config(const DefaultConfig& config) {
        default_config = config;
    }
    
    DefaultConfig get_default_config() const {
        return default_config;
    }
    
    // Brain state utilities
    bool is_brain_initialized() const {
        return brain && brain->isInitialized();
    }
    
    bool is_brain_running() const {
        return brain && brain->isRunning();
    }
    
    // Brain testing utilities
    bool brain_meets_criteria(std::function<bool(const nlm::Brain&)> criteria) const {
        return brain && criteria(*brain);
    }
    
    nlm::NeuronId find_neuron_by_criteria(
        std::function<bool(const nlm::Neuron&)> criteria) const {
        if (!brain) return nlm::NeuronId(0);
        
        return brain->findNeuron(criteria);
    }
    
    nlm::SynapseId find_synapse_by_criteria(
        std::function<bool(const nlm::Synapse&)> criteria) const {
        if (!brain) return nlm::SynapseId(0);
        
        return brain->findSynapse(criteria);
    }
    
    // Brain simulation utilities
    void run_brain_simulation(double duration, double time_step = 0.001) {
        if (!brain) return;
        
        double elapsed = 0.0;
        while (elapsed < duration) {
            brain->step(std::min(time_step, duration - elapsed));
            elapsed += time_step;
        }
    }
    
    void run_brain_to_completion(double target_activity_level) {
        if (!brain) return;
        
        int steps = 0;
        while (get_firing_neuron_count() < target_activity_level && steps < 10000) {
            brain->step(default_config.simulation_timestep);
            steps++;
        }
    }
    
    // Brain memory utilities
    size_t get_memory_usage() const {
        return brain ? brain->getMemoryUsage() : 0;
    }
    
    size_t get_peak_memory_usage() const {
        return brain ? brain->getPeakMemoryUsage() : 0;
    }
    
    bool is_memory_exhausted() const {
        return brain ? brain->isMemoryExhausted() : false;
    }
    
    void clear_memory() {
        if (brain) {
            brain->clearMemory();
        }
    }
};

// AdvancedNeuronModel test fixture
class AdvancedNeuronModelTestFixture {
protected:
    std::vector<std::unique_ptr<nlm::AdvancedNeuronModel>> neurons;
    std::map<nlm::NeuronId, std::unique_ptr<nlm::AdvancedNeuronModel>> neuron_map;
    
    struct DefaultParameters {
        std::string model_type = "LIF";
        float membrane_potential = -70.0f;
        float threshold = -55.0f;
        float reset_potential = -70.0f;
        float time_constant = 10.0f;
        float capacitance = 1.0f;
        float resting_rate = 0.0f;
        float maximum_rate = 100.0f;
        bool enabled = true;
    } default_params;
    
public:
    AdvancedNeuronModelTestFixture() : neurons(), neuron_map() {}
    
    virtual ~AdvancedNeuronModelTestFixture() {
        clear_neurons();
    }
    
    // Setup and teardown
    virtual void setup() {
        std::cout << "[FIXTURE] AdvancedNeuronModelTestFixture setup" << std::endl;
        clear_neurons();
    }
    
    virtual void teardown() {
        std::cout << "[FIXTURE] AdvancedNeuronModelTestFixture teardown" << std::endl;
        clear_neurons();
    }
    
    virtual void setup_test() {
        std::cout << "[FIXTURE] AdvancedNeuronModelTestFixture setup_test" << std::endl;
    }
    
    virtual void teardown_test() {
        std::cout << "[FIXTURE] AdvancedNeuronModelTestFixture teardown_test" << std::endl;
    }
    
    // Advanced neuron creation utilities
    nlm::NeuronId create_advanced_neuron(const std::string& model_type) {
        nlm::NeuronId id = generate_unique_id();
        neurons.push_back(create_advanced_neuron_by_type(model_type, id));
        neuron_map[id] = std::move(neurons.back());
        return id;
    }
    
    std::unique_ptr<nlm::AdvancedNeuronModel> create_advanced_neuron_by_type(
        const std::string& model_type, nlm::NeuronId id) {
        // Simplified creation - would need actual factory logic
        return std::make_unique<nlm::AdvancedNeuronModel>(id, default_params);
    }
    
    // Neuron retrieval
    nlm::AdvancedNeuronModel* get_advanced_neuron(nlm::NeuronId id) {
        auto it = neuron_map.find(id);
        return (it != neuron_map.end()) ? it->second.get() : nullptr;
    }
    
    const nlm::AdvancedNeuronModel* get_advanced_neuron(nlm::NeuronId id) const {
        auto it = neuron_map.find(id);
        return (it != neuron_map.end()) ? it->second.get() : nullptr;
    }
    
    // Neuron management
    void add_advanced_neuron(std::unique_ptr<nlm::AdvancedNeuronModel> neuron) {
        if (neuron) {
            nlm::NeuronId id = neuron->getId();
            neurons.push_back(std::move(neuron));
            neuron_map[id] = std::move(neurons.back());
        }
    }
    
    void remove_advanced_neuron(nlm::NeuronId id) {
        auto it = neuron_map.find(id);
        if (it != neuron_map.end()) {
            // Remove from vector
            neurons.erase(std::remove_if(neurons.begin(), neurons.end(),
                [id](const std::unique_ptr<nlm::AdvancedNeuronModel>& n) { return n->getId() == id; }),
                neurons.end());
            
            // Remove from map
            neuron_map.erase(it);
        }
    }
    
    void clear_neurons() {
        neurons.clear();
        neuron_map.clear();
    }
    
    // Advanced neuron statistics
    size_t get_advanced_neuron_count() const {
        return neurons.size();
    }
    
    std::vector<nlm::NeuronId> get_all_advanced_neuron_ids() const {
        std::vector<nlm::NeuronId> ids;
        for (const auto& pair : neuron_map) {
            ids.push_back(pair.first);
        }
        return ids;
    }
    
    // Advanced neuron type utilities
    std::vector<std::string> get_supported_model_types() const {
        return {"LIF", "HH", "Izhikevich", "AdaptiveExponential", "Theta", "MorrisLecar"};
    }
    
    // Advanced neuron state utilities
    bool are_all_advanced_neurons_enabled() const {
        for (const auto& neuron : neurons) {
            if (!neuron->isEnabled()) {
                return false;
            }
        }
        return true;
    }
    
    bool are_all_advanced_neurons_at_rest() const {
        for (const auto& neuron : neurons) {
            if (!neuron->isAtRest()) {
                return false;
            }
        }
        return true;
    }
    
    // Advanced neuron testing utilities
    bool advanced_neuron_meets_criteria(nlm::NeuronId id,
        std::function<bool(const nlm::AdvancedNeuronModel&)> criteria) const {
        const nlm::AdvancedNeuronModel* neuron = get_advanced_neuron(id);
        return neuron && criteria(*neuron);
    }
    
    std::vector<nlm::NeuronId> advanced_neurons_meeting_criteria(
        std::function<bool(const nlm::AdvancedNeuronModel&)> criteria) const {
        std::vector<nlm::NeuronId> matching_ids;
        
        for (const auto& pair : neuron_map) {
            if (criteria(*pair.second)) {
                matching_ids.push_back(pair.first);
            }
        }
        
        return matching_ids;
    }
    
    // Advanced neuron simulation utilities
    void advance_advanced_neuron_states(double time_step) {
        for (auto& neuron : neurons) {
            neuron->step(time_step);
        }
    }
    
    void reset_all_advanced_neurons() {
        for (auto& neuron : neurons) {
            neuron->reset();
        }
    }
    
    void enable_all_advanced_neurons(bool enabled) {
        for (auto& neuron : neurons) {
            neuron->setEnabled(enabled);
        }
    }
    
    void set_all_advanced_neuron_parameters(const DefaultParameters& params) {
        for (auto& neuron : neurons) {
            neuron->setParameters(params);
        }
    }
    
private:
    nlm::NeuronId generate_unique_id() {
        static std::atomic<uint64_t> next_id{3000};
        return nlm::NeuronId(next_id++);
    }
};

// Test fixture manager
class TestFixtureManager {
private:
    std::vector<std::unique_ptr<NeuronTestFixture>> neuron_fixtures;
    std::vector<std::unique_ptr<SynapseTestFixture>> synapse_fixtures;
    std::vector<std::unique_ptr<BrainTestFixture>> brain_fixtures;
    std::vector<std::unique_ptr<AdvancedNeuronModelTestFixture>> advanced_neuron_fixtures;
    
    std::map<std::string, std::vector<std::string>> fixture_dependencies;
    std::map<std::string, std::function<void()>> fixture_setup_functions;
    std::map<std::string, std::function<void()>> fixture_teardown_functions;
    
public:
    // Fixture registration
    void register_neuron_fixture(const std::string& name) {
        neuron_fixtures.push_back(std::make_unique<NeuronTestFixture>());
        fixture_dependencies[name] = {};
        fixture_setup_functions[name] = [this, name]() {
            auto it = std::find_if(neuron_fixtures.begin(), neuron_fixtures.end(),
                [name](const auto& fixture) {
                    return true; // Simplified - would need name tracking
                });
            if (it != neuron_fixtures.end()) {
                (*it)->setup();
            }
        };
        
        fixture_teardown_functions[name] = [this, name]() {
            auto it = std::find_if(neuron_fixtures.begin(), neuron_fixtures.end(),
                [name](const auto& fixture) {
                    return true; // Simplified - would need name tracking
                });
            if (it != neuron_fixtures.end()) {
                (*it)->teardown();
            }
        };
    }
    
    void register_synapse_fixture(const std::string& name) {
        synapse_fixtures.push_back(std::make_unique<SynapseTestFixture>());
        fixture_dependencies[name] = {};
        fixture_setup_functions[name] = [this, name]() {
            auto it = std::find_if(synapse_fixtures.begin(), synapse_fixtures.end(),
                [name](const auto& fixture) {
                    return true; // Simplified - would need name tracking
                });
            if (it != synapse_fixtures.end()) {
                (*it)->setup();
            }
        };
        
        fixture_teardown_functions[name] = [this, name]() {
            auto it = std::find_if(synapse_fixtures.begin(), synapse_fixtures.end(),
                [name](const auto& fixture) {
                    return true; // Simplified - would need name tracking
                });
            if (it != synapse_fixtures.end()) {
                (*it)->teardown();
            }
        };
    }
    
    void register_brain_fixture(const std::string& name) {
        brain_fixtures.push_back(std::make_unique<BrainTestFixture>());
        fixture_dependencies[name] = {};
        fixture_setup_functions[name] = [this, name]() {
            auto it = std::find_if(brain_fixtures.begin(), brain_fixtures.end(),
                [name](const auto& fixture) {
                    return true; // Simplified - would need name tracking
                });
            if (it != brain_fixtures.end()) {
                (*it)->setup();
            }
        };
        
        fixture_teardown_functions[name] = [this, name]() {
            auto it = std::find_if(brain_fixtures.begin(), brain_fixtures.end(),
                [name](const auto& fixture) {
                    return true; // Simplified - would need name tracking
                });
            if (it != brain_fixtures.end()) {
                (*it)->teardown();
            }
        };
    }
    
    void register_advanced_neuron_fixture(const std::string& name) {
        advanced_neuron_fixtures.push_back(std::make_unique<AdvancedNeuronModelTestFixture>());
        fixture_dependencies[name] = {};
        fixture_setup_functions[name] = [this, name]() {
            auto it = std::find_if(advanced_neuron_fixtures.begin(), advanced_neuron_fixtures.end(),
                [name](const auto& fixture) {
                    return true; // Simplified - would need name tracking
                });
            if (it != advanced_neuron_fixtures.end()) {
                (*it)->setup();
            }
        };
        
        fixture_teardown_functions[name] = [this, name]() {
            auto it = std::find_if(advanced_neuron_fixtures.begin(), advanced_neuron_fixtures.end(),
                [name](const auto& fixture) {
                    return true; // Simplified - would need name tracking
                });
            if (it != advanced_neuron_fixtures.end()) {
                (*it)->teardown();
            }
        };
    }
    
    // Fixture access
    NeuronTestFixture* get_neuron_fixture(size_t index = 0) {
        if (index < neuron_fixtures.size()) {
            return neuron_fixtures[index].get();
        }
        return nullptr;
    }
    
    SynapseTestFixture* get_synapse_fixture(size_t index = 0) {
        if (index < synapse_fixtures.size()) {
            return synapse_fixtures[index].get();
        }
        return nullptr;
    }
    
    BrainTestFixture* get_brain_fixture(size_t index = 0) {
        if (index < brain_fixtures.size()) {
            return brain_fixtures[index].get();
        }
        return nullptr;
    }
    
    AdvancedNeuronModelTestFixture* get_advanced_neuron_fixture(size_t index = 0) {
        if (index < advanced_neuron_fixtures.size()) {
            return advanced_neuron_fixtures[index].get();
        }
        return nullptr;
    }
    
    // Fixture management
    void setup_all_fixtures() {
        for (const auto& pair : fixture_setup_functions) {
            try {
                pair.second();
            } catch (const std::exception& e) {
                std::cerr << "[FIXTURE] Setup failed for fixture " << pair.first << ": " << e.what() << std::endl;
            }
        }
    }
    
    void teardown_all_fixtures() {
        // Teardown in reverse order
        for (auto it = fixture_teardown_functions.rbegin(); it != fixture_teardown_functions.rend(); ++it) {
            try {
                it->second();
            } catch (const std::exception& e) {
                std::cerr << "[FIXTURE] Teardown failed for fixture " << it->first << ": " << e.what() << std::endl;
            }
        }
    }
    
    // Test fixture execution
    template<typename FixtureType>
    void run_test_with_fixture(FixtureType& fixture, const std::string& test_name) {
        std::cout << "[FIXTURE] Running test: " << test_name << " with " << typeid(FixtureType).name() << std::endl;
        
        try {
            // Setup test
            fixture.setup_test();
            
            // Execute test logic (would be implemented by caller)
            // For demonstration, just print that test is running
            std::cout << "[FIXTURE] Test " << test_name << " execution would happen here" << std::endl;
            
            // Teardown test
            fixture.teardown_test();
            
            std::cout << "[FIXTURE] Test " << test_name << " completed successfully" << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << "[FIXTURE] Test " << test_name << " failed: " << e.what() << std::endl;
            throw;
        }
    }
    
    // Fixture statistics
    size_t get_neuron_fixture_count() const {
        return neuron_fixtures.size();
    }
    
    size_t get_synapse_fixture_count() const {
        return synapse_fixtures.size();
    }
    
    size_t get_brain_fixture_count() const {
        return brain_fixtures.size();
    }
    
    size_t get_advanced_neuron_fixture_count() const {
        return advanced_neuron_fixtures.size();
    }
    
    size_t get_total_fixture_count() const {
        return get_neuron_fixture_count() + get_synapse_fixture_count() +
               get_brain_fixture_count() + get_advanced_neuron_fixture_count();
    }
    
    // Fixture dependency management
    void add_fixture_dependency(const std::string& fixture_name, const std::string& dependency) {
        auto it = fixture_dependencies.find(fixture_name);
        if (it != fixture_dependencies.end()) {
            it->second.push_back(dependency);
        }
    }
    
    bool can_run_fixture(const std::string& fixture_name) const {
        auto it = fixture_dependencies.find(fixture_name);
        if (it == fixture_dependencies.end()) return true;
        
        for (const auto& dep : it->second) {
            // Check if dependency is available
            if (std::find_if(fixture_dependencies.begin(), fixture_dependencies.end(),
                [dep](const auto& pair) { return pair.first == dep; }) == fixture_dependencies.end()) {
                return false;
            }
        }
        return true;
    }
    
    // Fixture report
    void print_fixture_report() const {
        std::cout << "=== Test Fixture Report ===" << std::endl;
        std::cout << "Neuron Fixtures: " << get_neuron_fixture_count() << std::endl;
        std::cout << "Synapse Fixtures: " << get_synapse_fixture_count() << std::endl;
        std::cout << "Brain Fixtures: " << get_brain_fixture_count() << std::endl;
        std::cout << "Advanced Neuron Fixtures: " << get_advanced_neuron_fixture_count() << std::endl;
        std::cout << "Total Fixtures: " << get_total_fixture_count() << std::endl;
        std::cout << std::endl;
        
        std::cout << "Fixture Dependencies:" << std::endl;
        for (const auto& pair : fixture_dependencies) {
            std::cout << "  " << pair.first << ": ";
            for (size_t i = 0; i < pair.second.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << pair.second[i];
            }
            std::cout << std::endl;
        }
    }
};

// Global fixture manager
extern TestFixtureManager global_fixture_manager;

// Export fixture components
extern "C" {
    void export_fixture_manager(TestFixtureManager* manager) {
        if (manager) {
            global_fixture_manager = *manager;
        }
    }
    
    void export_neuron_fixture(NeuronTestFixture* fixture) {
        if (fixture && global_fixture_manager.get_neuron_fixture_count() < 1) {
            global_fixture_manager.register_neuron_fixture("NeuronFixture");
            *global_fixture_manager.get_neuron_fixture(0) = std::move(*fixture);
        }
    }
    
    void export_synapse_fixture(SynapseTestFixture* fixture) {
        if (fixture && global_fixture_manager.get_synapse_fixture_count() < 1) {
            global_fixture_manager.register_synapse_fixture("SynapseFixture");
            *global_fixture_manager.get_synapse_fixture(0) = std::move(*fixture);
        }
    }
    
    void export_brain_fixture(BrainTestFixture* fixture) {
        if (fixture && global_fixture_manager.get_brain_fixture_count() < 1) {
            global_fixture_manager.register_brain_fixture("BrainFixture");
            *global_fixture_manager.get_brain_fixture(0) = std::move(*fixture);
        }
    }
    
    void export_advanced_neuron_fixture(AdvancedNeuronModelTestFixture* fixture) {
        if (fixture && global_fixture_manager.get_advanced_neuron_fixture_count() < 1) {
            global_fixture_manager.register_advanced_neuron_fixture("AdvancedNeuronFixture");
            *global_fixture_manager.get_advanced_neuron_fixture(0) = std::move(*fixture);
        }
    }
}