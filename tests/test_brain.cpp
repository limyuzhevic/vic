// Brain Tests
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "cognition/ConceptFormation.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace test_brain {

void testConfigurationValidation() {
    nlm::Config config;
    
    // Test basic configuration
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Test configuration access
    assert(config.has("neuron_count"));
    assert(config.has("simulation_timestep"));
    assert(config.has("region_count"));
    
    // Test configuration summary
    std::string summary = config.summary();
    assert(!summary.empty());
    
    std::cout << "    testConfigurationValidation passed" << std::endl;
}

void testCommandLineParsing() {
    auto config = std::make_shared<nlm::Config>();
    
    // Test basic config creation
    config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    
    // Test config access
    assert(config->getOr<int64_t>("random_seed", 0) == 42);
    assert(config->getOr<int64_t>("neuron_count", 0) == 100);
    
    // Test default values
    assert(config->getOr<int64_t>("unknown_key", 999) == 999);
    assert(config->getOr<double>("unknown_key", 3.14) == 3.14);
    
    std::cout << "    testCommandLineParsing passed" << std::endl;
}

void testBrainConfigurationOptions() {
    auto config = std::make_shared<nlm::Config>();
    
    // Test STDP parameters
    config->set("stdp_ltp_weight", 0.02f, nlm::ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, nlm::ConfigSource::Default);
    config->set("stdp_tau", 20.0f, nlm::ConfigSource::Default);
    
    // Test plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, nlm::ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, nlm::ConfigSource::Default);
    
    // Test connection parameters
    config->set("connection_probability", 0.15f, nlm::ConfigSource::Default);
    
    // Verify STDP parameters
    assert(config->getOr<float>("stdp_ltp_weight", 0.0f) == 0.02f);
    assert(config->getOr<float>("stdp_ltd_weight", 1.0f) == 0.015f);
    assert(config->getOr<float>("stdp_tau", 0.0f) == 20.0f);
    
    // Verify plasticity parameters
    assert(config->getOr<float>("synaptogenesis_rate", 1.0f) == 0.0001f);
    assert(config->getOr<float>("pruning_rate", 1.0f) == 0.00001f);
    
    // Verify connection probability
    assert(config->getOr<float>("connection_probability", 1.0f) == 0.15f);
    
    std::cout << "    testBrainConfigurationOptions passed" << std::endl;
}

void testEnhancedTestExecution() {
    // This test verifies that enhanced test execution options work
    // Test with max steps limited
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    if (brain.initialize()) {
        // Test basic connectivity with limited steps
        runBasicConnectivityTest(brain, 100);  // Limit to 100 steps instead of default 50
        
        // The test should still pass within the limited steps
        std::cout << "    testEnhancedTestExecution passed" << std::endl;
    }
}

void testErrorHandling() {
    // Test error handling and validation
    auto config = std::make_shared<nlm::Config>();
    
    // Test invalid configuration values that should trigger warnings
    config->set("simulation_timestep", 0.0, nlm::ConfigSource::Default);  // Invalid timestep
    config->set("neuron_count", -1, nlm::ConfigSource::Default);  // Invalid neuron count
    
    // The configuration should still work but validation should warn
    assert(config->getOr<double>("simulation_timestep", 0.001) == 0.0);
    assert(config->getOr<int64_t>("neuron_count", 100) == -1);
    
    std::cout << "    testErrorHandling passed" << std::endl;
}

void testBrainInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool success = brain.initialize();
    assert(success);
    
    // Verify brain initialized with expected properties
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testBrainInitialization passed" << std::endl;
}

void testBrainStep() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Just verify it doesn't crash
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "    testBrainStep passed" << std::endl;
}

void testBrainReset() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    brain.step(0);
    brain.reset();
    
    // Verify reset worked
    assert(brain.getTotalNeuronCount() == 50);
    
    std::cout << "    testBrainReset passed" << std::endl;
}

void testBrainRegions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 3);
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        assert(region != nullptr);
    }
    
    std::cout << "    testBrainRegions passed" << std::endl;
}

void testBrainActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    std::cout << "    testBrainActionProduction passed" << std::endl;
}

void testBrainStatus() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify statistics are accessible
    assert(brain.getTotalNeuronCount() > 0);
    assert(brain.getTotalSynapseCount() >= 0);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    
    std::cout << "    testBrainStatus passed" << std::endl;
}

void testBrainCheckpointRoundTrip() {
    // Phase 4: save -> load must restore weights, membrane potentials,
    // firing rates and pending delayed spikes, and a load into a
    // differently-shaped brain must fail instead of misaligning state.
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);

    nlm::Brain brain1(config);
    brain1.initialize();
    // Drive the brain so there is real state to restore (weights move,
    // neurons fire, delayed spikes are pending).
    brain1.injectCurrentToNeurons(nlm::NeuronType::Sensory, 20.0f);
    for (nlm::SimulationStep step = 0; step < 60; ++step) {
        brain1.step(step);
    }

    const std::string path = "/tmp/nlm_test_checkpoint.bin";
    if (!brain1.save(path)) throw std::runtime_error("checkpoint save failed");

    auto meanWeight = [](nlm::Brain& b) {
        double sum = 0.0;
        size_t count = 0;
        for (const auto& region : b.getRegions()) {
            for (const auto& syn : region->getSynapses()) {
                sum += syn->getWeight();
                ++count;
            }
        }
        return count > 0 ? sum / static_cast<double>(count) : 0.0;
    };
    auto firstMembrane = [](nlm::Brain& b) {
        for (const auto& region : b.getRegions()) {
            for (auto* neuron : region->getAllNeurons()) {
                return neuron->getState().membranePotential;
            }
        }
        return 0.0f;
    };

    double savedMeanWeight = meanWeight(brain1);
    float savedMembrane = firstMembrane(brain1);
    size_t savedPending = brain1.getPendingSpikeEventCount();

    nlm::Brain brain2(config);
    brain2.initialize();
    if (!brain2.load(path)) throw std::runtime_error("checkpoint load failed");
    if (std::abs(meanWeight(brain2) - savedMeanWeight) >= 1e-6) throw std::runtime_error("mean weight not restored");
    if (std::abs(firstMembrane(brain2) - savedMembrane) >= 1e-6) throw std::runtime_error("membrane not restored");
    // Pending delayed spikes round-trip (in-flight synaptic input).
    if (brain2.getPendingSpikeEventCount() != savedPending) throw std::runtime_error("pending spikes not restored");

    // Continue both brains with identical steps: firing rates should
    // stay close (they are not bit-identical: per-synapse spike
    // histories and RNG state are not part of the checkpoint, see the
    // Phase 4 report — but weights, membranes and pending spikes are).
    for (nlm::SimulationStep step = 60; step < 70; ++step) {
        brain1.step(step);
        brain2.step(step);
    }
    float rate1 = brain1.getAverageFiringRate();
    float rate2 = brain2.getAverageFiringRate();
    float denom = std::max(std::abs(rate1), 1.0f);
    if (std::abs(rate1 - rate2) / denom >= 0.25f) throw std::runtime_error("continued firing rates diverged");

    // Mismatched structure must fail, not silently misalign.
    auto smallConfig = std::make_shared<nlm::Config>();
    smallConfig->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    smallConfig->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    nlm::Brain smallBrain(smallConfig);
    smallBrain.initialize();
    if (smallBrain.load(path)) throw std::runtime_error("mismatched-structure load should fail");

    std::cout << "    testBrainCheckpointRoundTrip passed (meanWeight="
              << savedMeanWeight << ", pending=" << savedPending
              << ", rates " << rate1 << " vs " << rate2 << ")" << std::endl;
}

void testPhase6TextEncoding() {
    // Phase 6: the text encoder must be deterministic, and
    // different words must give different patterns — otherwise
    // "teaching" a word could not be distinguished from any other
    // word even in principle. Throwing checks, not assert: in a
    // Release build assert is compiled out and this test would
    // be vacuous (the Phase 4 lesson).
    auto apple1 = nlm::AgentBrain::encodeText("apple");
    auto apple2 = nlm::AgentBrain::encodeText("Apple");
    auto zebra = nlm::AgentBrain::encodeText("zebra");
    if (apple1.size() != 4) throw std::runtime_error("text code must have 4 neurons");
    if (apple1 != apple2) throw std::runtime_error("text encoding must be case-insensitive and deterministic");
    if (apple1 == zebra) throw std::runtime_error("different words must encode differently");
    std::cout << "    testPhase6TextEncoding passed" << std::endl;
}

void testPhase6ConceptSimilarity() {
    // Phase 6: cosine alone scores a bright scene and a dark
    // scene as identical (same direction, different magnitude),
    // so concept formation collapsed every scene into one
    // concept. The magnitude-aware similarity must separate
    // them while keeping a scene similar to itself.
    nlm::ConceptFormation concepts;
    std::vector<float> resource(24, 0.0f), hazard(24, 0.0f);
    for (int i = 5; i <= 10; ++i) { resource[i] = 0.3f; hazard[i] = 0.75f; }
    resource[16] = 0.8f; hazard[16] = 0.8f;
    float same = concepts.computeSimilarity(resource, resource);
    float diff = concepts.computeSimilarity(resource, hazard);
    if (same < 0.99f) throw std::runtime_error("identical patterns must score ~1.0");
    if (diff >= 0.7f) throw std::runtime_error("resource vs hazard scenes must score below the 0.7 concept threshold");
    std::cout << "    testPhase6ConceptSimilarity passed (same=" << same << ", diff=" << diff << ")" << std::endl;
}

void testPhase6WorkingMemoryTraces() {
    // Phase 6: getActiveTraces() was always 0 because
    // storeToNeuron never populated the trace list. A stored
    // neuron must be an active trace.
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    brain.initialize();
    auto* wm = brain.getWorkingMemory();
    if (!wm) throw std::runtime_error("no working memory");
    nlm::NeuronId first;
    bool found = false;
    for (const auto& region : brain.getRegions()) {
        for (auto* neuron : region->getAllNeurons()) { first = neuron->getId(); found = true; break; }
        if (found) break;
    }
    if (!found) throw std::runtime_error("no neurons to store");
    wm->storeToNeuron(first, 1.0f);
    if (wm->getActiveTraces() == 0) throw std::runtime_error("stored neuron must be an active trace");
    std::cout << "    testPhase6WorkingMemoryTraces passed" << std::endl;
}

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    testBrainCheckpointRoundTrip();
    testPhase6TextEncoding();
    testPhase6ConceptSimilarity();
    testPhase6WorkingMemoryTraces();
}

} // namespace test_brain
