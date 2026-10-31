// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace test_brain {

void testBrainCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    std::cout << "    testBrainCreation passed" << std::endl;
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

void runAll() {
    testBrainCreation();
    testBrainInitialization();
    testBrainStep();
    testBrainReset();
    testBrainRegions();
    testBrainActionProduction();
    testBrainStatus();
    testBrainCheckpointRoundTrip();
}

} // namespace test_brain
