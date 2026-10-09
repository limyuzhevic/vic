#pragma once

#include <cstdint>
#include <string>
#include "core/Types/Types.hpp"

namespace nlm {

// Phase 6 integration constants
namespace Phase6Constants {
    // Default experiment configuration values
    constexpr uint64_t DEFAULT_MAX_STEPS = 10000;
    constexpr size_t DEFAULT_NEURON_COUNT = 1000;
    constexpr size_t DEFAULT_REGION_COUNT = 1;
    constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;
    constexpr bool DEFAULT_ENABLE_DEVELOPMENT = true;
    constexpr bool DEFAULT_ENABLE_CHECKPOINTING = true;
    constexpr bool DEFAULT_ENABLE_REPLAY = true;
    inline const std::string DEFAULT_CHECKPOINT_PATH = "./checkpoint_test.bin";
    
    // Plasticity parameters
    constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;
    constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;
    constexpr float DEFAULT_STDP_TIME_CONSTANT = 20.0f;
    constexpr float DEFAULT_SYNAPSIOGENESIS_RATE = 0.0001f;
    constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
    
    // Learning experiment constants
    constexpr float LEARNING_WEIGHT_CHANGE_THRESHOLD = 0.01f;
    constexpr float LEARNING_WEIGHT_CHANGE_SENSITIVITY = 0.001f;
    
    // Test configuration constants
    constexpr uint64_t BASIC_CONNECTIVITY_NEURONS = 50;
    constexpr uint64_t PLASTICITY_EXPERIMENT_STEPS = 1000;
    constexpr float SIMULATION_TIMESTEP = 0.001f;
    constexpr float INJECTED_CURRENT_STRONG = 60.0f;
    constexpr float INJECTED_CURRENT_WEAK = 30.0f;
    constexpr size_t LOG_PROGRESS_INTERVAL = 100;
    constexpr size_t STD_PROCESSING_BATCH_SIZE = 5;
    constexpr uint32_t REGION_ID_FIRST = 1;
    constexpr NeuronType DEFAULT_NEURON_TYPE = NeuronType::Sensory;
}

// Test configuration structure for parameterization
struct TestConfig {
    std::string name;
    uint64_t maxSteps;
    size_t neuronCount;
    float injectedCurrent;
    float timestep;
    bool enablePlasticity;
    bool enableStdp;
    bool enableStructural;
    
    TestConfig(const std::string& n, uint64_t steps, size_t neurons, 
               float current, float dt, bool plasticity = false,
               bool stdp = false, bool structural = false)
        : name(n), maxSteps(steps), neuronCount(neurons), 
          injectedCurrent(current), timestep(dt),
          enablePlasticity(plasticity), enableStdp(stdp),
          enableStructural(structural) {}
    
    // Convenience methods for common test configurations
    static TestConfig basicConnectivity() {
        return TestConfig("basic_connectivity", 
                          Phase6Constants::BASIC_CONNECTIVITY_NEURONS,
                          Phase6Constants::BASIC_CONNECTIVITY_NEURONS,
                          Phase6Constants::INJECTED_CURRENT_STRONG,
                          Phase6Constants::SIMULATION_TIMESTEP);
    }
    
    static TestConfig plasticityExperiment() {
        return TestConfig("plasticity_experiment",
                          Phase6Constants::PLASTICITY_EXPERIMENT_STEPS,
                          Phase6Constants::DEFAULT_NEURON_COUNT,
                          Phase6Constants::INJECTED_CURRENT_WEAK,
                          Phase6Constants::SIMULATION_TIMESTEP,
                          true, true, true);
    }
    
    static TestConfig stdpVerification() {
        return TestConfig("stdp_verification",
                          100,
                          Phase6Constants::STD_PROCESSING_BATCH_SIZE,
                          Phase6Constants::INJECTED_CURRENT_STRONG,
                          Phase6Constants::SIMULATION_TIMESTEP,
                          false, true, false);
    }
};

} // namespace nlm