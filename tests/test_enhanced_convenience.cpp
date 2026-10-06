// Enhanced Convenience Functions Tests
// Tests for the new convenience functions in Python bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_enhanced_convenience {

void testRunDemoSimulation() {
    auto config = nlm::createDefaultConfig();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Runtime);
    
    auto brain = nlm::run_demo_simulation(config, 50);
    assert(brain != nullptr);
    assert(brain->getTotalNeuronCount() == 50);
    
    std::cout << "    testRunDemoSimulation passed" << std::endl;
}

void testRunAgentEpisode() {
    auto config = nlm::createDefaultConfig();
    auto brain = nlm::createBrain(config);
    brain->initialize();
    
    auto world = nlm::createSimpleWorld();
    world->configure(15, 15, 8, 8);
    world->reset();
    
    auto agent = nlm::createAgentBrain(brain);
    agent->initialize(*world);
    
    // Enable learning subsystems
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    auto results = nlm::run_agent_episode(brain, world, agent, 25);
    
    // Verify results
    auto& [returned_brain, returned_world, returned_agent] = results;
    assert(returned_brain != nullptr);
    assert(returned_world != nullptr);
    assert(returned_agent != nullptr);
    
    assert(returned_brain->getTotalNeuronCount() == 50);
    assert(returned_world->getWidth() == 15);
    
    std::cout << "    testRunAgentEpisode passed" << std::endl;
}

void testCreateDevelopmentConfig() {
    auto config = nlm::create_development_config();
    assert(config != nullptr);
    
    // Verify development-stage specific settings
    assert(config->has("brain.neuron_count"));
    assert(config->has("plasticity.stdp.enable"));
    assert(config->has("plasticity.hebbian.enable"));
    assert(config->has("plasticity.structural.enable"));
    assert(config->has("neuromod.curiosity.enable"));
    assert(config->has("neuromod.novelty.enable"));
    
    // Check specific values for developmental config
    auto neuron_count = config->getOr<int64_t>("brain.neuron_count", 0);
    assert(neuron_count == 500);  // Expected value for development config
    
    auto stdp_enabled = config->getOr<bool>("plasticity.stdp.enable", false);
    assert(stdp_enabled == true);
    
    auto curiosity_enabled = config->getOr<bool>("neuromod.curiosity.enable", false);
    assert(curiosity_enabled == true);
    
    std::cout << "    testCreateDevelopmentConfig passed" << std::endl;
}

void testCreateSteadyStateConfig() {
    auto config = nlm::create_steady_state_config();
    assert(config != nullptr);
    
    // Verify steady-state settings
    assert(config->has("plasticity.structural.enable"));
    assert(config->has("neuromod.curiosity.enable"));
    
    // Check that steady-state has different values
    auto neuron_count = config->getOr<int64_t>("brain.neuron_count", 0);
    assert(neuron_count == 1000);  // Expected value for steady-state config
    
    auto structural_enabled = config->getOr<bool>("plasticity.structural.enable", false);
    assert(structural_enabled == false);  // Disabled in steady-state
    
    auto curiosity_enabled = config->getOr<bool>("neuromod.curiosity.enable", false);
    assert(curiosity_enabled == false);  // Disabled in steady-state
    
    std::cout << "    testCreateSteadyStateConfig passed" << std::endl;
}

void testCreateExploratoryConfig() {
    auto config = nlm::create_exploratory_config();
    assert(config != nullptr);
    
    // Verify exploratory settings
    assert(config->has("plasticity.structural.enable"));
    assert(config->has("neuromod.curiosity.enable"));
    
    // Check that exploratory has different values
    auto neuron_count = config->getOr<int64_t>("brain.neuron_count", 0);
    assert(neuron_count == 300);  // Expected value for exploratory config
    
    auto structural_enabled = config->getOr<bool>("plasticity.structural.enable", false);
    assert(structural_enabled == true);  // Enabled in exploratory
    
    auto curiosity_enabled = config->getOr<bool>("neuromod.curiosity.enable", false);
    assert(curiosity_enabled == true);  // Enabled in exploratory
    
    std::cout << "    testCreateExploratoryConfig passed" << std::endl;
}

void testBenchmarkBrain() {
    auto config = nlm::createDefaultConfig();
    config->set("neuron_count", static_cast<int64_t>(30), nlm::ConfigSource::Runtime);
    
    auto brain = nlm::createBrain(config);
    brain->initialize();
    
    auto result = nlm::benchmark_brain(brain, 20);
    
    // Unpack benchmark result tuple
    auto& [spikes, synapses, firing, duration_ms, spikes_per_ms, neurons_per_ms] = result;
    
    assert(spikes >= 0);
    assert(synapses >= 0);
    assert(firing >= 0);
    assert(duration_ms > 0);
    assert(spikes_per_ms >= 0.0f);
    assert(neurons_per_ms >= 0.0f);
    
    // Verify reasonable expectations
    assert(spikes <= 5000);  // Should not be extremely high for 20 steps
    assert(spikes_per_ms < 100.0f);  // Reasonable spikes per ms
    
    std::cout << "    testBenchmarkBrain passed (spikes: " << spikes << ", "
              << "duration: " << duration_ms << "ms)" << std::endl;
}

void testCompareDevelopmentalStages() {
    auto base_config = nlm::createDefaultConfig();
    
    std::vector<std::string> stage_names = {"Stage1", "Stage2", "Stage3"};
    
    auto results = nlm::compare_developmental_stages(base_config, stage_names, 15);
    
    assert(results.size() == 3);
    
    for (size_t i = 0; i < results.size(); ++i) {
        auto& [stage_name, total_spikes, total_time, spikes_per_ms, firing_rate] = results[i];
        
        assert(stage_name == stage_names[i]);
        assert(total_spikes >= 0);
        assert(total_time > 0);
        assert(spikes_per_ms >= 0.0f);
        assert(firing_rate >= 0.0f);
        
        // Reasonable constraints
        assert(total_spikes <= 10000);
        assert(spikes_per_ms < 50.0f);
    }
    
    std::cout << "    testCompareDevelopmentalStages passed" << std::endl;
}

void testExportImportBrainState() {
    auto config = nlm::createDefaultConfig();
    config->set("neuron_count", static_cast<int64_t>(25), nlm::ConfigSource::Runtime);
    
    auto brain = nlm::createBrain(config);
    brain->initialize();
    
    // Run a few steps to have some state
    for (int i = 0; i < 10; ++i) {
        brain->step(i);
    }
    
    // Export
    bool export_result = nlm::export_brain_state(brain, "test_export.bin");
    assert(export_result);
    
    // Create new brain and import
    auto imported_brain = nlm::createBrain(config);
    imported_brain->initialize();
    
    bool import_result = nlm::import_brain_state(imported_brain, "test_export.bin");
    assert(import_result);
    
    std::cout << "    testExportImportBrainState passed" << std::endl;
}

void testConstantsAndTypeAliases() {
    // Test that type aliases are accessible
    // Note: In pybind11, type aliases are typically accessed differently
    // but we can test the constants
    
    assert(nlm::MAX_NEURONS > 0);
    assert(nlm::MAX_SYNAPSES > 0);
    assert(nlm::DEFAULT_SIMULATION_DT > 0.0f);
    assert(nlm::NEURON_SCALE_FACTOR > 0.0f);
    assert(nlm::DEFAULT_EXPLORATION_RATE >= 0.0f && nlm::DEFAULT_EXPLORATION_RATE <= 1.0f);
    assert(nlm::MAX_PLACIDITY_MODIFIER > nlm::MIN_PLACIDITY_MODIFIER);
    
    std::cout << "    testConstantsAndTypeAliases passed" << std::endl;
}

void runAll() {
    std::cout << "Running enhanced convenience function tests..." << std::endl;
    
    testRunDemoSimulation();
    testRunAgentEpisode();
    testCreateDevelopmentConfig();
    testCreateSteadyStateConfig();
    testCreateExploratoryConfig();
    testBenchmarkBrain();
    testCompareDevelopmentalStages();
    testExportImportBrainState();
    testConstantsAndTypeAliases();
    
    std::cout << std::endl << "All enhanced convenience function tests passed!" << std::endl;
}

} // namespace test_enhanced_convenience

// Main function for enhanced convenience tests
int main() {
    test_enhanced_convenience::runAll();
    return 0;
}
