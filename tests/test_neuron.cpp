// Neuron Tests
#include "src/brain/Neuron.hpp"
#include "src/core/Types/Types.hpp"
#include "src/core/Random/Random.hpp"
#include <cassert>
#include <iostream>

namespace test_neuron {

void test_neuron_creation() {
    std::cout << "Testing neuron creation..." << std::endl;
    nlm::NeuronId id(42);
    nlm::Neuron neuron(id);
    
    assert(neuron.getId() == id);
    assert(neuron.getType() == nlm::NeuronType::Internal);
    assert(neuron.getState().membranePotential == -70.0f);
    std::cout << "✓ Neuron creation test passed" << std::endl;
}

void test_neuron_state_access() {
    std::cout << "Testing neuron state access..." << std::endl;
    nlm::NeuronId id(43);
    nlm::Neuron neuron(id);
    
    nlm::NeuronState& state = neuron.getState();
    state.membranePotential = -50.0f;
    assert(neuron.getMembranePotential() == -50.0f);
    
    const nlm::NeuronState& const_state = neuron.getState();
    assert(const_state.membranePotential == -50.0f);
    std::cout << "✓ Neuron state access test passed" << std::endl;
}

void test_neuron_lif_step() {
    std::cout << "Testing neuron LIF step with standard timestep..." << std::endl;
    nlm::NeuronId id(44);
    nlm::Neuron neuron(id);
    
    // Set up neuron to be close to threshold
    nlm::NeuronState& state = neuron.getState();
    state.restingPotential = -70.0f;
    state.threshold = -55.0f;
    state.membranePotential = -56.0f;  // Just below threshold
    state.refractoryRemaining = 0;
    state.adaptationVariable = 0.0f;
    
    // Take a step
    neuron.step(0.0f);
    
    // Neuron should not fire (still below threshold after dynamics)
    // but state should be updated
    assert(state.firingState == nlm::FiringState::Active);
    std::cout << "✓ Neuron LIF step test passed" << std::endl;
}

void test_neuron_random_initialization() {
    std::cout << "Testing neuron random initialization..." << std::endl;
    nlm::NeuronId id(45);
    nlm::Neuron neuron(id);
    
    nlm::RandomGenerator rng(42);
    neuron.initializeRandom(rng);
    
    nlm::NeuronState& state = neuron.getState();
    assert(state.membranePotential >= -75.0f && state.membranePotential <= -65.0f);
    assert(state.threshold >= -65.0f && state.threshold <= -45.0f);
    assert(state.restingPotential >= -75.0f && state.restingPotential <= -65.0f);
    assert(state.refractoryPeriod >= 2 && state.refractoryPeriod <= 10);
    std::cout << "✓ Neuron random initialization test passed" << std::endl;
}

void test_neuron_step_with_explicit_timestep() {
    std::cout << "Testing neuron step with explicit timestep..." << std::endl;
    nlm::NeuronId id(46);
    nlm::Neuron neuron(id);
    
    // Set up neuron
    nlm::NeuronState& state = neuron.getState();
    state.restingPotential = -70.0f;
    state.threshold = -55.0f;
    state.membranePotential = -60.0f;
    state.refractoryRemaining = 0;
    state.adaptationVariable = 0.0f;
    
    // Take a step with explicit timestep
    neuron.step(0.0f, 0.001f);  // 1ms timestep
    
    // Neuron should not fire but state should be updated
    assert(state.firingState == nlm::FiringState::Active);
    std::cout << "✓ Neuron step with explicit timestep test passed" << std::endl;
}

void test_neuron_refractory_behavior() {
    std::cout << "Testing neuron refractory behavior..." << std::endl;
    nlm::NeuronId id(47);
    nlm::Neuron neuron(id);
    
    nlm::NeuronState& state = neuron.getState();
    state.refractoryPeriod = 5;
    state.refractoryRemaining = 5;
    
    assert(neuron.isRefractory() == true);
    
    // Decrement refractory
    neuron.decrementRefractory();
    assert(state.refractoryRemaining == 4);
    assert(neuron.isRefractory() == true);
    
    // Clear refractory
    for (int i = 0; i < 4; ++i) {
        neuron.decrementRefractory();
    }
    assert(state.refractoryRemaining == 0);
    assert(neuron.isRefractory() == false);
    assert(state.firingState == nlm::FiringState::Resting);
    std::cout << "✓ Neuron refractory behavior test passed" << std::endl;
}

void test_neuron_plasticity_flags() {
    std::cout << "Testing neuron plasticity flags..." << std::endl;
    nlm::NeuronId id(48);
    nlm::Neuron neuron(id);
    
    neuron.enablePlasticity(true, false, true);
    
    const nlm::PlasticityFlags& flags = neuron.getPlasticityFlags();
    assert(flags.hebbian == true);
    assert(flags.stdp == false);
    assert(flags.reward_modulated == true);
    std::cout << "✓ Neuron plasticity flags test passed" << std::endl;
}

void test_neuron_reset() {
    std::cout << "Testing neuron reset..." << std::endl;
    nlm::NeuronId id(49);
    nlm::Neuron neuron(id);
    
    nlm::NeuronState& state = neuron.getState();
    state.membranePotential = -50.0f;
    state.refractoryRemaining = 3;
    
    neuron.reset();
    
    assert(state.membranePotential == -70.0f);  // Default value
    assert(state.refractoryRemaining == 0);
    assert(neuron.isRefractory() == false);
    std::cout << "✓ Neuron reset test passed" << std::endl;
}

void run_all_tests() {
    std::cout << "=== NLM Neuron Tests ===" << std::endl;
    
    test_neuron_creation();
    test_neuron_state_access();
    test_neuron_lif_step();
    test_neuron_random_initialization();
    test_neuron_step_with_explicit_timestep();
    test_neuron_refractory_behavior();
    test_neuron_plasticity_flags();
    test_neuron_reset();
    
    std::cout << "=== All Neuron Tests Passed ===" << std::endl;
}

} // namespace test_neuron

int main() {
    test_neuron::run_all_tests();
    return 0;
}
