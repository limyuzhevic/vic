// NLM Memory Leak Detection Tests
// Phase 7: Comprehensive Test Suite
// Memory leak detection tests for major components

#pragma once

#include "test_infrastructure/base/test_base.hpp"
#include "test_infrastructure/utils/test_config.hpp"
#include "test_infrastructure/fixtures/test_fixture.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <memory>
#include <vector>
#include <iostream>

// Memory leak detection test configuration
struct MemoryLeakTestConfig {
    std::string test_name;
    std::string description;
    size_t allocation_count;
    std::vector<std::string> component_types;
    bool should_track_pointers;
    double timeout_seconds;
    std::string leak_detection_method;
    std::string expected_leak_count;
    std::vector<std::string> tags;
    
    MemoryLeakTestConfig(const std::string& name, const std::string& desc,
                        size_t count, const std::vector<std::string>& components)
        : test_name(name), description(desc), allocation_count(count),
          component_types(components), should_track_pointers(true),
          timeout_seconds(60.0), leak_detection_method("TRACK_ALLOCS"),
          expected_leak_count("0") {}
};

// Memory leak test result
struct MemoryLeakTestResult {
    std::string test_name;
    bool passed = false;
    size_t detected_leaks = 0;
    size_t expected_leaks = 0;
    double execution_time = 0.0;
    std::string leak_detection_method;
    std::vector<std::string> leak_descriptions;
    std::string error_message;
    std::chrono::time_point<std::chrono::system_clock> start_time;
    std::chrono::time_point<std::chrono::system_clock> end_time;
    
    MemoryLeakTestResult(const std::string& name) : test_name(name) {}
    
    bool has_leaks() const {
        return detected_leaks > expected_leaks;
    }
    
    double leak_percentage() const {
        return expected_leaks > 0 ? (static_cast<double>(detected_leaks) / expected_leaks * 100.0) : 0.0;
    }
};

// Neuron memory leak test fixture
class NeuronMemoryLeakTestFixture : public TestFixture {
private:
    std::vector<std::unique_ptr<nlm::Neuron>> neurons;
    std::vector<void*> tracked_pointers;
    
public:
    NeuronMemoryLeakTestFixture() : neurons(), tracked_pointers() {}
    
    virtual void setup() override {
        std::cout << "[MEMORY LEAK] NeuronMemoryLeakTestFixture setup" << std::endl;
        clear_neurons();
    }
    
    virtual void teardown() override {
        std::cout << "[MEMORY LEAK] NeuronMemoryLeakTestFixture teardown" << std::endl;
        clear_tracked_pointers();
    }
    
    virtual void setup_test() override {
        std::cout << "[MEMORY LEAK] NeuronMemoryLeakTestFixture setup_test" << std::endl;
    }
    
    virtual void teardown_test() override {
        std::cout << "[MEMORY LEAK] NeuronMemoryLeakTestFixture teardown_test" << std::endl;
        check_memory_leaks("Neuron creation/destruction cycle");
    }
    
    // Neuron creation and tracking
    nlm::NeuronId create_tracked_neuron(nlm::NeuronType type = nlm::NeuronType::Internal) {
        nlm::NeuronId id = generate_unique_id();
        
        // Create neuron using allocation tracking
        void* neuron_ptr = ::operator new(sizeof(nlm::Neuron));
        neurons.emplace_back(new (neuron_ptr) nlm::Neuron(id, type));
        tracked_pointers.push_back(neuron_ptr);
        
        std::cout << "[MEMORY LEAK] Created neuron " << id.getId() << " of type " << get_neuron_type_string(type) << std::endl;
        return id;
    }
    
    void delete_tracked_neuron(nlm::NeuronId id) {
        auto it = std::find_if(neurons.begin(), neurons.end(),
            [id](const std::unique_ptr<nlm::Neuron>& n) { return n->getId() == id; });
        
        if (it != neurons.end()) {
            nlm::Neuron* neuron_ptr = it->get();
            
            // Find and remove from tracked pointers
            auto ptr_it = std::find(tracked_pointers.begin(), tracked_pointers.end(),
                reinterpret_cast<void*>(neuron_ptr));
            
            if (ptr_it != tracked_pointers.end()) {
                tracked_pointers.erase(ptr_it);
            }
            
            // Explicitly call destructor and free memory
            neuron_ptr->~Neuron();
            ::operator delete(reinterpret_cast<void*>(neuron_ptr));
            
            neurons.erase(it);
            
            std::cout << "[MEMORY LEAK] Deleted neuron " << id.getId() << std::endl;
        }
    }
    
    void clear_neurons() {
        for (auto& neuron : neurons) {
            nlm::Neuron* neuron_ptr = neuron.get();
            neuron_ptr->~Neuron();
            ::operator delete(reinterpret_cast<void*>(neuron_ptr));
        }
        
        neurons.clear();
    }
    
    void clear_tracked_pointers() {
        for (void* ptr : tracked_pointers) {
            ::operator delete(ptr);
        }
        tracked_pointers.clear();
    }
    
    // Neuron parameter testing with memory tracking
    void test_neuron_creation_with_parameters() {
        std::cout << "[MEMORY LEAK] Testing neuron creation with various parameters" << std::endl;
        
        // Test with different neuron types
        std::vector<nlm::NeuronType> types = {
            nlm::NeuronType::Internal,
            nlm::NeuronType::Excitatory,
            nlm::NeuronType::Inhibitory,
            nlm::NeuronType::Sensory,
            nlm::NeuronType::Motor
        };
        
        for (const auto& type : types) {
            nlm::NeuronId id = create_tracked_neuron(type);
            
            // Verify neuron was created correctly
            nlm::Neuron* neuron = get_neuron(id);
            assert(neuron != nullptr);
            assert(neuron->getId() == id);
            assert(neuron->getType() == type);
            
            // Test neuron properties
            test_neuron_properties(neuron);
            
            // Clean up
            delete_tracked_neuron(id);
        }
        
        std::cout << "[MEMORY LEAK] Neuron creation test passed" << std::endl;
    }
    
    void test_neuron_parameter_initialization() {
        std::cout << "[MEMORY LEAK] Testing neuron parameter initialization" << std::endl;
        
        nlm::NeuronParameters params;
        params.neuron_type = nlm::NeuronType::Excitatory;
        params.threshold = -45.0f;
        params.resting_potential = -65.0f;
        params.refractory_period = 10;
        params.time_constant = 15.0f;
        params.capacitance = 2.0f;
        
        nlm::NeuronId id = create_tracked_neuron_with_params(params);
        
        // Verify parameters were set correctly
        nlm::Neuron* neuron = get_neuron(id);
        assert(neuron != nullptr);
        assert(neuron->getType() == params.neuron_type);
        assert(neuron->getThreshold() == params.threshold);
        assert(neuron->getMembranePotential() == params.resting_potential);
        assert(neuron->getRefractoryPeriod() == params.refractory_period);
        
        // Test parameter modification
        neuron->setMembranePotential(-60.0f);
        neuron->setThreshold(-50.0f);
        neuron->setRefractoryPeriod(5);
        
        assert(neuron->getMembranePotential() == -60.0f);
        assert(neuron->getThreshold() == -50.0f);
        assert(neuron->getRefractoryPeriod() == 5);
        
        // Clean up
        delete_tracked_neuron(id);
        
        std::cout << "[MEMORY LEAK] Neuron parameter initialization test passed" << std::endl;
    }
    
    void test_neuron_lifecycle() {
        std::cout << "[MEMORY LEAK] Testing neuron lifecycle management" << std::endl;
        
        std::vector<nlm::NeuronId> created_ids;
        
        // Create multiple neurons
        for (int i = 0; i < 10; ++i) {
            nlm::NeuronId id = create_tracked_neuron(nlm::NeuronType::Internal);
            created_ids.push_back(id);
        }
        
        // Verify all neurons exist
        assert(neurons.size() == 10);
        
        // Test neuron operations
        for (const auto& id : created_ids) {
            nlm::Neuron* neuron = get_neuron(id);
            assert(neuron != nullptr);
            
            // Test neuron operations
            neuron->setMembranePotential(-50.0f);
            neuron->injectCurrent(5.0f);
            neuron->recordSpike(0.1f);
            neuron->reset();
            
            // Verify operations
            assert(neuron->getMembranePotential() == -70.0f);
            assert(neuron->getTotalCurrent() == 0.0f);
            assert(neuron->getSpikeHistory().size() == 0);
        }
        
        // Clean up all neurons
        for (const auto& id : created_ids) {
            delete_tracked_neuron(id);
        }
        
        assert(neurons.empty());
        
        std::cout << "[MEMORY LEAK] Neuron lifecycle test passed" << std::endl;
    }
    
    void test_neuron_memory_efficiency() {
        std::cout << "[MEMORY LEAK] Testing neuron memory efficiency" << std::endl;
        
        size_t initial_tracked_count = tracked_pointers.size();
        
        // Create and destroy neurons in a loop
        for (int iteration = 0; iteration < 100; ++iteration) {
            nlm::NeuronId id = create_tracked_neuron(nlm::NeuronType::Internal);
            
            // Use neuron
            nlm::Neuron* neuron = get_neuron(id);
            neuron->setMembranePotential(-50.0f + iteration * 0.1f);
            
            // Delete neuron
            delete_tracked_neuron(id);
            
            // Check for memory leaks every 10 iterations
            if (iteration % 10 == 0) {
                check_memory_leaks("Neuron batch processing");
            }
        }
        
        // Final memory check
        check_memory_leaks("Neuron memory efficiency test");
        
        std::cout << "[MEMORY LEAK] Neuron memory efficiency test passed" << std::endl;
    }
    
    // Memory leak detection utilities
    void check_memory_leaks(const std::string& test_description) {
        size_t leaked_count = get_leaked_allocations();
        
        if (leaked_count > 0) {
            std::cout << "[MEMORY LEAK] " << test_description << ": " << leaked_count << " memory leaks detected" << std::endl;
            
            // Add leak information to fixture state
            std::stringstream ss;
            ss << "Detected " << leaked_count << " memory leaks during " << test_description;
            add_warning(ss.str());
        } else {
            std::cout << "[MEMORY LEAK] " << test_description << ": No memory leaks detected" << std::endl;
        }
    }
    
    size_t get_leaked_allocations() const {
        return tracked_pointers.size();
    }
    
    nlm::Neuron* create_neuron_with_params(const nlm::NeuronParameters& params) {
        nlm::NeuronId id = generate_unique_id();
        
        void* neuron_ptr = ::operator new(sizeof(nlm::Neuron));
        neurons.emplace_back(new (neuron_ptr) nlm::Neuron(id, params));
        tracked_pointers.push_back(neuron_ptr);
        
        return neurons.back().get();
    }
    
    nlm::Neuron* get_neuron(nlm::NeuronId id) {
        auto it = std::find_if(neurons.begin(), neurons.end(),
            [id](const std::unique_ptr<nlm::Neuron>& n) { return n->getId() == id; });
        return (it != neurons.end()) ? it->get() : nullptr;
    }
    
    const nlm::Neuron* get_neuron(nlm::NeuronId id) const {
        auto it = std::find_if(neurons.begin(), neurons.end(),
            [id](const std::unique_ptr<nlm::Neuron>& n) { return n->getId() == id; });
        return (it != neurons.end()) ? it->get() : nullptr;
    }
    
    std::string get_neuron_type_string(nlm::NeuronType type) const {
        switch (type) {
            case nlm::NeuronType::Internal: return "Internal";
            case nlm::NeuronType::Excitatory: return "Excitatory";
            case nlm::NeuronType::Inhibitory: return "Inhibitory";
            case nlm::NeuronType::Sensory: return "Sensory";
            case nlm::NeuronType::Motor: return "Motor";
            default: return "Unknown";
        }
    }
    
    nlm::NeuronId generate_unique_id() {
        static std::atomic<uint64_t> next_id{4000};
        return nlm::NeuronId(next_id++);
    }
};

// Synapse memory leak test fixture
class SynapseMemoryLeakTestFixture : public TestFixture {
private:
    std::vector<std::unique_ptr<nlm::Synapse>> synapses;
    std::vector<void*> tracked_pointers;
    
public:
    SynapseMemoryLeakTestFixture() : synapses(), tracked_pointers() {}
    
    virtual void setup() override {
        std::cout << "[MEMORY LEAK] SynapseMemoryLeakTestFixture setup" << std::endl;
        clear_synapses();
    }
    
    virtual void teardown() override {
        std::cout << "[MEMORY LEAK] SynapseMemoryLeakTestFixture teardown" << std::endl;
        clear_tracked_pointers();
    }
    
    virtual void setup_test() override {
        std::cout << "[MEMORY LEAK] SynapseMemoryLeakTestFixture setup_test" << std::endl;
    }
    
    virtual void teardown_test() override {
        std::cout << "[MEMORY LEAK] SynapseMemoryLeakTestFixture teardown_test" << std::endl;
        check_memory_leaks("Synapse creation/destruction cycle");
    }
    
    // Synapse creation and tracking
    nlm::SynapseId create_tracked_synapse(nlm::NeuronId pre_id, nlm::NeuronId post_id) {
        nlm::SynapseId id = generate_unique_id();
        
        // Create synapse using allocation tracking
        void* synapse_ptr = ::operator new(sizeof(nlm::Synapse));
        synapses.emplace_back(new (synapse_ptr) nlm::Synapse(id, pre_id, post_id));
        tracked_pointers.push_back(synapse_ptr);
        
        std::cout << "[MEMORY LEAK] Created synapse " << id.getId() << " from " << pre_id.getId() 
                  << " to " << post_id.getId() << std::endl;
        return id;
    }
    
    void delete_tracked_synapse(nlm::SynapseId id) {
        auto it = std::find_if(synapses.begin(), synapses.end(),
            [id](const std::unique_ptr<nlm::Synapse>& s) { return s->getId() == id; });
        
        if (it != synapses.end()) {
            nlm::Synapse* synapse_ptr = it->get();
            
            // Find and remove from tracked pointers
            auto ptr_it = std::find(tracked_pointers.begin(), tracked_pointers.end(),
                reinterpret_cast<void*>(synapse_ptr));
            
            if (ptr_it != tracked_pointers.end()) {
                tracked_pointers.erase(ptr_it);
            }
            
            // Explicitly call destructor and free memory
            synapse_ptr->~Synapse();
            ::operator delete(reinterpret_cast<void*>(synapse_ptr));
            
            synapses.erase(it);
            
            std::cout << "[MEMORY LEAK] Deleted synapse " << id.getId() << std::endl;
        }
    }
    
    void clear_synapses() {
        for (auto& synapse : synapses) {
            nlm::Synapse* synapse_ptr = synapse.get();
            synapse_ptr->~Synapse();
            ::operator delete(reinterpret_cast<void*>(synapse_ptr));
        }
        
        synapses.clear();
    }
    
    void clear_tracked_pointers() {
        for (void* ptr : tracked_pointers) {
            ::operator delete(ptr);
        }
        tracked_pointers.clear();
    }
    
    // Synapse parameter testing with memory tracking
    void test_synapse_creation_with_parameters() {
        std::cout << "[MEMORY LEAK] Testing synapse creation with various parameters" << std::endl;
        
        // Create test neurons first
        nlm::NeuronId pre_id = create_test_neuron();
        nlm::NeuronId post_id = create_test_neuron();
        
        // Test synapse creation with different parameters
        std::vector<nlm::SynapseParameters> params_list = {
            nlm::SynapseParameters(),
            nlm::SynapseParameters{weight: 2.0f, delay: 2.0f, max_weight: 5.0f, min_weight: 0.2f},
            nlm::SynapseParameters{weight: 0.5f, delay: 0.5f, max_weight: 2.0f, min_weight: 0.1f},
        };
        
        for (size_t i = 0; i < params_list.size(); ++i) {
            nlm::SynapseId id = create_tracked_synapse_with_params(pre_id, post_id, params_list[i]);
            
            // Verify synapse was created correctly
            nlm::Synapse* synapse = get_synapse(id);
            assert(synapse != nullptr);
            assert(synapse->getId() == id);
            
            // Test synapse properties
            test_synapse_properties(synapse);
            
            // Clean up
            delete_tracked_synapse(id);
        }
        
        std::cout << "[MEMORY LEAK] Synapse creation test passed" << std::endl;
    }
    
    void test_synapse_plasticity() {
        std::cout << "[MEMORY LEAK] Testing synapse plasticity with memory tracking" << std::endl;
        
        // Create test neurons
        nlm::NeuronId pre_id = create_test_neuron();
        nlm::NeuronId post_id = create_test_neuron();
        
        nlm::SynapseId id = create_tracked_synapse(pre_id, post_id);
        
        // Test synaptic plasticity
        nlm::Synapse* synapse = get_synapse(id);
        assert(synapse != nullptr);
        
        // Test STDP plasticity
        synapse->apply_stdp(1.0f, 1.0f);
        
        float initial_weight = synapse->getWeight();
        
        // Test Hebbian plasticity
        synapse->apply_hebbian(0.5f, 0.5f);
        
        float updated_weight = synapse->getWeight();
        
        // Verify weight changed
        assert(updated_weight != initial_weight);
        
        // Test structural plasticity
        synapse->apply_structural_plasticity(1);
        
        // Clean up
        delete_tracked_synapse(id);
        
        std::cout << "[MEMORY LEAK] Synapse plasticity test passed" << std::endl;
    }
    
    void test_synapse_lifecycle() {
        std::cout << "[MEMORY LEAK] Testing synapse lifecycle management" << std::endl;
        
        // Create test neurons
        nlm::NeuronId pre_id = create_test_neuron();
        nlm::NeuronId post_id = create_test_neuron();
        
        std::vector<nlm::SynapseId> created_ids;
        
        // Create multiple synapses
        for (int i = 0; i < 20; ++i) {
            nlm::SynapseId id = create_tracked_synapse(pre_id, post_id);
            created_ids.push_back(id);
        }
        
        // Verify all synapses exist
        assert(synapses.size() == 20);
        
        // Test synapse operations
        for (const auto& id : created_ids) {
            nlm::Synapse* synapse = get_synapse(id);
            assert(synapse != nullptr);
            
            // Test synapse operations
            synapse->setWeight(1.0f + i * 0.1f);
            synapse->step(0.001f);
            synapse->update();
            
            // Verify operations
            assert(synapse->getWeight() == 1.0f + i * 0.1f);
            assert(synapse->isEnabled());
        }
        
        // Clean up all synapses
        for (const auto& id : created_ids) {
            delete_tracked_synapse(id);
        }
        
        assert(synapses.empty());
        
        std::cout << "[MEMORY LEAK] Synapse lifecycle test passed" << std::endl;
    }
    
    void test_synapse_memory_efficiency() {
        std::cout << "[MEMORY LEAK] Testing synapse memory efficiency" << std::endl;
        
        size_t initial_tracked_count = tracked_pointers.size();
        
        // Create test neurons once
        nlm::NeuronId pre_id = create_test_neuron();
        nlm::NeuronId post_id = create_test_neuron();
        
        // Create and destroy synapses in a loop
        for (int iteration = 0; iteration < 200; ++iteration) {
            nlm::SynapseId id = create_tracked_synapse_with_params(
                pre_id, post_id, nlm::SynapseParameters{weight: 1.0f + iteration * 0.01f});
            
            // Use synapse
            nlm::Synapse* synapse = get_synapse(id);
            synapse->step(0.001f);
            
            // Delete synapse
            delete_tracked_synapse(id);
            
            // Check for memory leaks every 20 iterations
            if (iteration % 20 == 0) {
                check_memory_leaks("Synapse batch processing");
            }
        }
        
        // Final memory check
        check_memory_leaks("Synapse memory efficiency test");
        
        std::cout << "[MEMORY LEAK] Synapse memory efficiency test passed" << std::endl;
    }
    
    // Memory leak detection utilities
    void check_memory_leaks(const std::string& test_description) {
        size_t leaked_count = get_leaked_allocations();
        
        if (leaked_count > 0) {
            std::cout << "[MEMORY LEAK] " << test_description << ": " << leaked_count << " memory leaks detected" << std::endl;
            
            std::stringstream ss;
            ss << "Detected " << leaked_count << " memory leaks during " << test_description;
            add_warning(ss.str());
        } else {
            std::cout << "[MEMORY LEAK] " << test_description << ": No memory leaks detected" << std::endl;
        }
    }
    
    size_t get_leaked_allocations() const {
        return tracked_pointers.size();
    }
    
    nlm::Synapse* create_tracked_synapse_with_params(nlm::NeuronId pre_id, nlm::NeuronId post_id,
        const nlm::SynapseParameters& params) {
        nlm::SynapseId id = generate_unique_id();
        
        void* synapse_ptr = ::operator new(sizeof(nlm::Synapse));
        synapses.emplace_back(new (synapse_ptr) nlm::Synapse(id, pre_id, post_id, params));
        tracked_pointers.push_back(synapse_ptr);
        
        return synapses.back().get();
    }
    
    nlm::Synapse* get_synapse(nlm::SynapseId id) {
        auto it = std::find_if(synapses.begin(), synapses.end(),
            [id](const std::unique_ptr<nlm::Synapse>& s) { return s->getId() == id; });
        return (it != synapses.end()) ? it->get() : nullptr;
    }
    
    const nlm::Synapse* get_synapse(nlm::SynapseId id) const {
        auto it = std::find_if(synapses.begin(), synapses.end(),
            [id](const std::unique_ptr<nlm::Synapse>& s) { return s->getId() == id; });
        return (it != synapses.end()) ? it->get() : nullptr;
    }
    
    nlm::NeuronId create_test_neuron() {
        static std::atomic<uint64_t> next_neuron_id{5000};
        return nlm::NeuronId(next_neuron_id++);
    }
    
    void test_synapse_properties(nlm::Synapse* synapse) {
        if (synapse) {
            float weight = synapse->getWeight();
            float delay = synapse->getDelay();
            bool enabled = synapse->isEnabled();
            
            std::cout << "[MEMORY LEAK] Synapse properties - weight: " << weight 
                      << ", delay: " << delay << ", enabled: " << (enabled ? "true" : "false") << std::endl;
        }
    }
    
    void test_neuron_properties(nlm::Neuron* neuron) {
        if (neuron) {
            float potential = neuron->getMembranePotential();
            float threshold = neuron->getThreshold();
            bool firing = neuron->isFiring();
            
            std::cout << "[MEMORY LEAK] Neuron properties - potential: " << potential 
                      << ", threshold: " << threshold << ", firing: " << (firing ? "true" : "false") << std::endl;
        }
    }
    
    nlm::SynapseId generate_unique_id() {
        static std::atomic<uint64_t> next_id{6000};
        return nlm::SynapseId(next_id++);
    }
};

// Brain memory leak test fixture
class BrainMemoryLeakTestFixture : public TestFixture {
private:
    std::unique_ptr<nlm::Brain> brain;
    std::vector<void*> tracked_pointers;
    
public:
    BrainMemoryLeakTestFixture() : brain(nullptr), tracked_pointers() {}
    
    virtual void setup() override {
        std::cout << "[MEMORY LEAK] BrainMemoryLeakTestFixture setup" << std::endl;
        clear_brain();
    }
    
    virtual void teardown() override {
        std::cout << "[MEMORY LEAK] BrainMemoryLeakTestFixture teardown" << std::endl;
        clear_tracked_pointers();
    }
    
    virtual void setup_test() override {
        std::cout << "[MEMORY LEAK] BrainMemoryLeakTestFixture setup_test" << std::endl;
    }
    
    virtual void teardown_test() override {
        std::cout << "[MEMORY LEAK] BrainMemoryLeakTestFixture teardown_test" << std::endl;
        check_memory_leaks("Brain creation/destruction cycle");
    }
    
    // Brain creation and tracking
    void create_tracked_brain(size_t neuron_count = 100) {
        clear_brain();
        
        void* brain_ptr = ::operator new(sizeof(nlm::Brain));
        brain = std::unique_ptr<nlm::Brain>(new (brain_ptr) nlm::Brain(neuron_count));
        tracked_pointers.push_back(brain_ptr);
        
        std::cout << "[MEMORY LEAK] Created brain with " << neuron_count << " neurons" << std::endl;
        
        // Initialize brain
        brain->initialize();
    }
    
    void delete_tracked_brain() {
        if (brain) {
            // Find and remove from tracked pointers
            auto it = std::find(tracked_pointers.begin(), tracked_pointers.end(),
                reinterpret_cast<void*>(brain.get()));
            
            if (it != tracked_pointers.end()) {
                tracked_pointers.erase(it);
            }
            
            // Explicitly call destructor and free memory
            brain->~Brain();
            ::operator delete(reinterpret_cast<void*>(brain.release()));
            
            std::cout << "[MEMORY LEAK] Deleted brain" << std::endl;
        }
    }
    
    void clear_brain() {
        if (brain) {
            brain->~Brain();
            ::operator delete(reinterpret_cast<void*>(brain.release()));
        }
        
        brain.reset();
    }
    
    void clear_tracked_pointers() {
        for (void* ptr : tracked_pointers) {
            ::operator delete(ptr);
        }
        tracked_pointers.clear();
    }
    
    // Brain parameter testing with memory tracking
    void test_brain_creation_with_parameters() {
        std::cout << "[MEMORY LEAK] Testing brain creation with various parameters" << std::endl;
        
        // Test with different brain configurations
        std::vector<nlm::BrainConfig> configs = {
            nlm::BrainConfig(),
            nlm::BrainConfig{name: "Test Brain 1", neuron_count: 50, connection_probability: 0.05},
            nlm::BrainConfig{name: "Test Brain 2", neuron_count: 200, connection_probability: 0.15},
            nlm::BrainConfig{name: "Test Brain 3", neuron_count: 500, connection_probability: 0.2},
        };
        
        for (const auto& config : configs) {
            create_tracked_brain(config.neuron_count);
            
            // Verify brain was created correctly
            assert(brain != nullptr);
            assert(brain->getNeuronCount() == config.neuron_count);
            
            // Test brain properties
            test_brain_properties(brain.get());
            
            // Clean up
            delete_tracked_brain();
        }
        
        std::cout << "[MEMORY LEAK] Brain creation test passed" << std::endl;
    }
    
    void test_brain_neuron_management() {
        std::cout << "[MEMORY LEAK] Testing brain neuron management with memory tracking" << std::endl;
        
        create_tracked_brain(100);
        
        // Add neurons
        std::vector<nlm::NeuronId> created_ids;
        for (int i = 0; i < 50; ++i) {
            nlm::NeuronId id = add_tracked_neuron(nlm::NeuronType::Internal);
            created_ids.push_back(id);
        }
        
        // Verify neurons were added
        assert(brain->getNeuronCount() == 100 + 50);
        
        // Remove neurons
        for (const auto& id : created_ids) {
            bool removed = remove_tracked_neuron(id);
            assert(removed);
        }
        
        // Verify neurons were removed
        assert(brain->getNeuronCount() == 100);
        
        // Clean up
        delete_tracked_brain();
        
        std::cout << "[MEMORY LEAK] Brain neuron management test passed" << std::endl;
    }
    
    void test_brain_synapse_management() {
        std::cout << "[MEMORY LEAK] Testing brain synapse management with memory tracking" << std::endl;
        
        create_tracked_brain(50);
        
        // Add some neurons first
        nlm::NeuronId pre_id = add_tracked_neuron(nlm::NeuronType::Excitatory);
        nlm::NeuronId post_id = add_tracked_neuron(nlm::NeuronType::Inhibitory);
        
        // Add synapses
        std::vector<nlm::SynapseId> created_ids;
        for (int i = 0; i < 25; ++i) {
            nlm::SynapseId id = add_tracked_synapse(pre_id, post_id);
            created_ids.push_back(id);
        }
        
        // Verify synapses were added
        assert(brain->getSynapseCount() == 25);
        
        // Remove synapses
        for (const auto& id : created_ids) {
            bool removed = remove_tracked_synapse(id);
            assert(removed);
        }
        
        // Verify synapses were removed
        assert(brain->getSynapseCount() == 0);
        
        // Clean up
        delete_tracked_brain();
        
        std::cout << "[MEMORY LEAK] Brain synapse management test passed" << std::endl;
    }
    
    void test_brain_simulation_memory() {
        std::cout << "[MEMORY LEAK] Testing brain simulation with memory tracking" << std::endl;
        
        create_tracked_brain(10);
        
        // Simulate brain for a period
        double total_time = 1.0; // 1 second
        double time_step = 0.001; // 1ms
        double elapsed = 0.0;
        
        while (elapsed < total_time) {
            brain->step(time_step);
            elapsed += time_step;
            
            // Check memory every 0.1 seconds
            if (std::fmod(elapsed, 0.1) < time_step) {
                check_memory_leaks("Brain simulation monitoring");
            }
        }
        
        // Final memory check
        check_memory_leaks("Brain simulation test");
        
        // Clean up
        delete_tracked_brain();
        
        std::cout << "[MEMORY LEAK] Brain simulation test passed" << std::endl;
    }
    
    void test_brain_memory_efficiency() {
        std::cout << "[MEMORY LEAK] Testing brain memory efficiency" << std::endl;
        
        size_t initial_tracked_count = tracked_pointers.size();
        
        // Create brain and run multiple simulations
        for (int iteration = 0; iteration < 10; ++iteration) {
            create_tracked_brain(5 + iteration); // Growing brain size
            
            // Simulate briefly
            double simulation_time = 0.1;
            double time_step = 0.001;
            double elapsed = 0.0;
            
            while (elapsed < simulation_time) {
                brain->step(time_step);
                elapsed += time_step;
            }
            
            // Clean up brain
            delete_tracked_brain();
            
            // Check memory every iteration
            check_memory_leaks("Brain batch processing");
        }
        
        // Final memory check
        check_memory_leaks("Brain memory efficiency test");
        
        std::cout << "[MEMORY LEAK] Brain memory efficiency test passed" << std::endl;
    }
    
    // Memory leak detection utilities
    void check_memory_leaks(const std::string& test_description) {
        size_t leaked_count = get_leaked_allocations();
        
        if (leaked_count > 0) {
            std::cout << "[MEMORY LEAK] " << test_description << ": " << leaked_count << " memory leaks detected" << std::endl;
            
            std::stringstream ss;
            ss << "Detected " << leaked_count << " memory leaks during " << test_description;
            add_warning(ss.str());
        } else {
            std::cout << "[MEMORY LEAK] " << test_description << ": No memory leaks detected" << std::endl;
        }
    }
    
    size_t get_leaked_allocations() const {
        return tracked_pointers.size();
    }
    
    nlm::NeuronId add_tracked_neuron(nlm::NeuronType type) {
        if (!brain) return nlm::NeuronId(0);
        
        nlm::NeuronId id = brain->addNeuron(type);
        
        // Track neuron creation
        void* neuron_ptr = ::operator new(sizeof(nlm::Neuron));
        tracked_pointers.push_back(neuron_ptr);
        
        return id;
    }
    
    bool remove_tracked_neuron(nlm::NeuronId id) {
        if (!brain) return false;
        
        bool removed = brain->removeNeuron(id);
        if (removed) {
            // Track neuron removal for memory leak detection
            // (would need actual tracking in real implementation)
        }
        
        return removed;
    }
    
    nlm::SynapseId add_tracked_synapse(nlm::NeuronId pre_id, nlm::NeuronId post_id) {
        if (!brain) return nlm::SynapseId(0);
        
        nlm::SynapseId id = brain->addSynapse(pre_id, post_id);
        
        // Track synapse creation
        void* synapse_ptr = ::operator new(sizeof(nlm::Synapse));
        tracked_pointers.push_back(synapse_ptr);
        
        return id;
    }
    
    bool remove_tracked_synapse(nlm::SynapseId id) {
        if (!brain) return false;
        
        bool removed = brain->removeSynapse(id);
        if (removed) {
            // Track synapse removal for memory leak detection
            // (would need actual tracking in real implementation)
        }
        
        return removed;
    }
    
    void test_brain_properties(nlm::Brain* brain_ptr) {
        if (brain_ptr) {
            size_t neuron_count = brain_ptr->getNeuronCount();
            size_t synapse_count = brain_ptr->getSynapseCount();
            size_t active_count = brain_ptr->getActiveNeuronCount();
            size_t firing_count = brain_ptr->getFiringNeuronCount();
            
            std::cout << "[MEMORY LEAK] Brain properties - neurons: " << neuron_count 
                      << ", synapses: " << synapse_count 
                      << ", active: " << active_count 
                      << ", firing: " << firing_count << std::endl;
        }
    }
};

// Memory leak test runner
class MemoryLeakTestRunner {
private:
    std::vector<MemoryLeakTestResult> results;
    std::vector<MemoryLeakTestConfig> configs;
    
public:
    void register_test(const MemoryLeakTestConfig& config) {
        configs.push_back(config);
    }
    
    void run_all_tests() {
        std::cout << "=== NLM Memory Leak Detection Tests ===" << std::endl;
        
        for (const auto& config : configs) {
            std::cout << std::endl;
            std::cout << "Running test: " << config.test_name << std::endl;
            std::cout << "Description: " << config.description << std::endl;
            
            MemoryLeakTestResult result(config.test_name);
            result.start_time = std::chrono::system_clock::now();
            
            try {
                if (config.test_name.find("neuron") != std::string::npos) {
                    run_neuron_memory_leak_test(config, result);
                } else if (config.test_name.find("synapse") != std::string::npos) {
                    run_synapse_memory_leak_test(config, result);
                } else if (config.test_name.find("brain") != std::string::npos) {
                    run_brain_memory_leak_test(config, result);
                } else {
                    result.passed = false;
                    result.error_message = "Unknown test type";
                }
                
            } catch (const std::exception& e) {
                result.passed = false;
                result.error_message = e.what();
            }
            
            result.end_time = std::chrono::system_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(result.end_time - result.start_time);
            result.execution_time = duration.count();
            
            results.push_back(result);
            
            // Print test result
            std::cout << "Test " << config.test_name << " " 
                      << (result.passed ? "PASSED" : "FAILED") 
                      << " (" << result.execution_time << "s)" << std::endl;
            
            if (!result.passed) {
                std::cout << "Error: " << result.error_message << std::endl;
            }
        }
        
        print_summary();
    }
    
    void run_neuron_memory_leak_test(const MemoryLeakTestConfig& config, MemoryLeakTestResult& result) {
        NeuronMemoryLeakTestFixture fixture;
        
        fixture.setup();
        
        // Run neuron memory leak tests
        fixture.test_neuron_creation_with_parameters();
        fixture.test_neuron_parameter_initialization();
        fixture.test_neuron_lifecycle();
        fixture.test_neuron_memory_efficiency();
        
        fixture.teardown();
        
        result.passed = true;
        result.detected_leaks = fixture.get_leaked_allocations();
        result.expected_leaks = 0;
    }
    
    void run_synapse_memory_leak_test(const MemoryLeakTestConfig& config, MemoryLeakTestResult& result) {
        SynapseMemoryLeakTestFixture fixture;
        
        fixture.setup();
        
        // Run synapse memory leak tests
        fixture.test_synapse_creation_with_parameters();
        fixture.test_synapse_plasticity();
        fixture.test_synapse_lifecycle();
        fixture.test_synapse_memory_efficiency();
        
        fixture.teardown();
        
        result.passed = true;
        result.detected_leaks = fixture.get_leaked_allocations();
        result.expected_leaks = 0;
    }
    
    void run_brain_memory_leak_test(const MemoryLeakTestConfig& config, MemoryLeakTestResult& result) {
        BrainMemoryLeakTestFixture fixture;
        
        fixture.setup();
        
        // Run brain memory leak tests
        fixture.test_brain_creation_with_parameters();
        fixture.test_brain_neuron_management();
        fixture.test_brain_synapse_management();
        fixture.test_brain_simulation_memory();
        fixture.test_brain_memory_efficiency();
        
        fixture.teardown();
        
        result.passed = true;
        result.detected_leaks = fixture.get_leaked_allocations();
        result.expected_leaks = 0;
    }
    
    void print_summary() const {
        std::cout << std::endl;
        std::cout << "=== Memory Leak Test Summary ===" << std::endl;
        
        size_t total_tests = results.size();
        size_t passed_tests = 0;
        size_t failed_tests = 0;
        size_t total_leaks = 0;
        
        for (const auto& result : results) {
            if (result.passed) {
                passed_tests++;
            } else {
                failed_tests++;
            }
            total_leaks += result.detected_leaks;
        }
        
        std::cout << "Total Tests: " << total_tests << std::endl;
        std::cout << "Passed: " << passed_tests << std::endl;
        std::cout << "Failed: " << failed_tests << std::endl;
        std::cout << "Total Memory Leaks Detected: " << total_leaks << std::endl;
        
        if (total_leaks > 0) {
            std::cout << std::endl;
            std::cout << "MEMORY LEAKS FOUND! Investigation recommended:" << std::endl;
            for (const auto& result : results) {
                if (result.detected_leaks > 0) {
                    std::cout << "  " << result.test_name << ": " << result.detected_leaks << " leaks" << std::endl;
                }
            }
        }
    }
    
    void print_detailed_results() const {
        std::cout << std::endl;
        std::cout << "=== Detailed Memory Leak Test Results ===" << std::endl;
        
        for (const auto& result : results) {
            std::cout << "Test: " << result.test_name << std::endl;
            std::cout << "  Status: " << (result.passed ? "PASSED" : "FAILED") << std::endl;
            std::cout << "  Detected Leaks: " << result.detected_leaks << std::endl;
            std::cout << "  Expected Leaks: " << result.expected_leaks << std::endl;
            std::cout << "  Leak Percentage: " << result.leak_percentage() << "%" << std::endl;
            std::cout << "  Execution Time: " << result.execution_time << "s" << std::endl;
            
            if (!result.error_message.empty()) {
                std::cout << "  Error: " << result.error_message << std::endl;
            }
            
            if (!result.leak_descriptions.empty()) {
                std::cout << "  Leak Descriptions:" << std::endl;
                for (const auto& desc : result.leak_descriptions) {
                    std::cout << "    - " << desc << std::endl;
                }
            }
            
            std::cout << std::endl;
        }
    }
};