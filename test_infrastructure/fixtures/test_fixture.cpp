// NLM Enhanced Test Fixture Implementation
// Phase 7: Comprehensive Test Suite

#include "test_infrastructure/fixtures/test_fixture.hpp"

// Global fixture manager
TestFixtureManager global_fixture_manager;

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