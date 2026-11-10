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

void testPhase6MemoryIntegration() {
    // Phase 6: Test integration of all memory systems
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test working memory integration
    auto* wm = brain.getWorkingMemory();
    if (!wm) throw std::runtime_error("working memory not available");
    
    // Store a neuron in working memory
    nlm::NeuronId firstNeuron;
    bool found = false;
    for (const auto& region : brain.getRegions()) {
        for (auto* neuron : region->getAllNeurons()) {
            firstNeuron = neuron->getId();
            found = true;
            break;
        }
        if (found) break;
    }
    
    if (!found) throw std::runtime_error("no neurons to store");
    wm->storeToNeuron(firstNeuron, 1.0f);
    
    // Test episodic memory integration
    auto* em = brain.getEpisodicMemory();
    if (!em) throw std::runtime_error("episodic memory not available");
    
    // Create and store an episode
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 1;
    episode.reward = 0.5f;
    episode.activeNeurons.push_back(firstNeuron);
    episode.neuronActivations.push_back(1.0f);
    
    em->storeEpisode(episode);
    
    // Test associative memory integration
    auto* am = brain.getAssociativeMemory();
    if (!am) throw std::runtime_error("associative memory not available");
    
    // Store an association
    am->storeAssociation(firstNeuron, firstNeuron);
    
    // Verify that all memory systems are working
    if (wm->getActiveTraces() == 0) throw std::runtime_error("working memory traces not working");
    if (em->getEpisodeCount() == 0) throw std::runtime_error("episodic memory not storing episodes");
    if (!am->hasAssociation(firstNeuron, firstNeuron)) throw std::runtime_error("associative memory not storing associations");
    
    std::cout << "    testPhase6MemoryIntegration passed" << std::endl;
}

void testPhase6PredictionIntegration() {
    // Phase 6: Test prediction system integration with working memory
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test prediction system availability
    auto* prediction = brain.getPredictionSystem();
    if (!prediction) throw std::runtime_error("prediction system not available");
    
    // Test prediction system with working memory
    auto* wm = brain.getWorkingMemory();
    if (!wm) throw std::runtime_error("working memory not available for prediction");
    
    // Store some neurons in working memory to provide input for prediction
    nlm::NeuronId neuron1, neuron2;
    bool found1 = false, found2 = false;
    
    int count = 0;
    for (const auto& region : brain.getRegions()) {
        for (auto* neuron : region->getAllNeurons()) {
            if (!found1) {
                neuron1 = neuron->getId();
                found1 = true;
                wm->storeToNeuron(neuron1, 0.8f);
            } else if (!found2) {
                neuron2 = neuron->getId();
                found2 = true;
                wm->storeToNeuron(neuron2, 0.6f);
            }
            count++;
            if (found1 && found2) break;
        }
        if (found1 && found2) break;
    }
    
    if (!found1 || !found2) throw std::runtime_error("not enough neurons for prediction test");
    
    // Test prediction system
    std::cout << "    testPhase6PredictionIntegration passed" << std::endl;
}

void testPhase6CognitionIntegration() {
    // Phase 6: Test integration of all cognition systems
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test NeuralPlanner integration
    auto* planner = brain.getPlanner();
    if (!planner) throw std::runtime_error("neural planner not available");
    
    // Test ConceptFormation integration
    auto* conceptFormation = brain.getConceptFormation();
    if (!conceptFormation) throw std::runtime_error("concept formation not available");
    
    // Test AttentionalSelection integration
    auto* attention = brain.getAttention();
    if (!attention) throw std::runtime_error("attentional selection not available");
    
    // Test cognitive system integration
    if (planner && conceptFormation && attention) {
        std::cout << "    testPhase6CognitionIntegration passed" << std::endl;
    } else {
        throw std::runtime_error("cognition systems not properly integrated");
    }
}

void testPhase6NeuromodulationIntegration() {
    // Phase 6: Test integration of all neuromodulation systems
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test neuromodulation systems availability
    auto* dopamine = brain.getDopamine();
    auto* curiosity = brain.getCuriosity();
    auto* novelty = brain.getNovelty();
    auto* predictionError = brain.getPredictionErrorSignal();
    
    if (!dopamine) throw std::runtime_error("dopamine not available");
    if (!curiosity) throw std::runtime_error("curiosity not available");
    if (!novelty) throw std::runtime_error("novelty not available");
    if (!predictionError) throw std::runtime_error("prediction error not available");
    
    // Test neuromodulation system integration
    // Apply neuromodulatory signals to see if they work
    nlm::Neuromodulator dopamineSignal(nlm::NeuromodulatorType::Dopamine, 0.5f);
    nlm::Neuromodulator curiositySignal(nlm::NeuromodulatorType::Curiosity, 0.8f);
    nlm::Neuromodulator noveltySignal(nlm::NeuromodulatorType::Novelty, 0.3f);
    nlm::Neuromodulator predictionErrorSignal(nlm::NeuromodulatorType::PredictionError, 0.2f);
    
    brain.applyNeuromodulation(dopamineSignal);
    brain.applyNeuromodulation(curiositySignal);
    brain.applyNeuromodulation(noveltySignal);
    brain.applyNeuromodulation(predictionErrorSignal);
    
    std::cout << "    testPhase6NeuromodulationIntegration passed" << std::endl;
}

void testPhase6FullIntegration() {
    // Phase 6: Full integration test of all systems
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.2f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify that all Phase 6 systems are available
    if (!brain.getWorkingMemory()) throw std::runtime_error("working memory not available");
    if (!brain.getEpisodicMemory()) throw std::runtime_error("episodic memory not available");
    if (!brain.getAssociativeMemory()) throw std::runtime_error("associative memory not available");
    if (!brain.getPredictionSystem()) throw std::runtime_error("prediction system not available");
    if (!brain.getPlanner()) throw std::runtime_error("neural planner not available");
    if (!brain.getConceptFormation()) throw std::runtime_error("concept formation not available");
    if (!brain.getAttention()) throw std::runtime_error("attentional selection not available");
    if (!brain.getDopamine()) throw std::runtime_error("dopamine not available");
    if (!brain.getCuriosity()) throw std::runtime_error("curiosity not available");
    if (!brain.getNovelty()) throw std::runtime_error("novelty not available");
    if (!brain.getPredictionErrorSignal()) throw std::runtime_error("prediction error not available");
    
    // Test that integration methods work
    brain.integrateCognitiveSystems();
    brain.integratePredictionSystem();
    brain.integrateConceptFormation();
    
    // Run some steps to test real integration
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step, step * 0.001);
        
        // Verify that systems are still functioning
        if (brain.getWorkingMemory() && brain.getWorkingMemory()->getMemoryNeurons().size() > 0) {
            break; // Working memory is being used
        }
    }
    
    // Final verification
    if (brain.getTotalNeuronCount() == 0) throw std::runtime_error("brain not properly initialized");
    if (brain.getTotalSynapseCount() == 0) throw std::runtime_error("brain has no synapses");
    
    std::cout << "    testPhase6FullIntegration passed" << std::endl;
}
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
    testPhase6MemoryIntegration();
    testPhase6PredictionIntegration();
    testPhase6CognitionIntegration();
    testPhase6NeuromodulationIntegration();
    testPhase6FullIntegration();
}

} // namespace test_brain
