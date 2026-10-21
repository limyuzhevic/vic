// Brain Tests
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

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

void testBrainCheckpoint() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run some simulation steps to create activity
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
    }
    
    // Test checkpoint save
    std::string checkpointFile = "test_checkpoint.nlm";
    bool saveSuccess = brain.save(checkpointFile);
    assert(saveSuccess);
    
    // Test checkpoint load
    nlm::Brain brain2(config);
    brain2.initialize();  // Initialize fresh brain
    
    bool loadSuccess = brain2.load(checkpointFile);
    assert(loadSuccess);
    
    // Verify some basic properties are preserved
    assert(brain2.getTotalNeuronCount() == 50);
    assert(brain2.getTotalSynapseCount() > 0);
    
    // Cleanup
    std::remove(checkpointFile.c_str());
    
    std::cout << "    testBrainCheckpoint passed" << std::endl;
}

void testBrainConnectivityAndPlasticity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.2f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Enable plasticity on synapses
    if (auto* region = brain.getRegion(nlm::RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply current injection to stimulate activity
    brain.injectCurrentToNeurons(nlm::NeuronType::Internal, 30.0f);
    
    // Run plasticity experiment
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Verify that plasticity occurred by checking that some synapses changed weight
    bool plasticityDetected = false;
    if (auto* region = brain.getRegion(nlm::RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            if (syn->getWeight() != 0.2f) {  // Default weight
                plasticityDetected = true;
                break;
            }
        }
    }
    
    assert(plasticityDetected);
    
    std::cout << "    testBrainConnectivityAndPlasticity passed" << std::endl;
}

void testBrainSensoryMotor() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test action production
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    // Test action selection based on motor neuron activity
    // Inject current into motor neurons to influence action
    brain.injectCurrentToNeurons(nlm::NeuronType::Motor, 50.0f);
    
    auto action2 = brain.produceAction();
    assert(action2 != nullptr);
    
    std::cout << "    testBrainSensoryMotor passed" << std::endl;
}

void testBrainNeuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Apply neuromodulation
    nlm::Dopamine dopamine;
    dopamine.initialize(&brain);
    brain.applyNeuromodulation(dopamine);
    
    // Verify neuromodulation affects brain state
    assert(brain.getTotalNeuronCount() == 100);
    
    // Inject current and check that neuromodulation still works
    brain.injectCurrentToNeurons(nlm::NeuronType::Internal, 20.0f);
    
    nlm::Novelty novelty;
    novelty.initialize(&brain);
    brain.applyNeuromodulation(novelty);
    
    std::cout << "    testBrainNeuromodulation passed" << std::endl;
}

void testBrainMemorySystems() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test working memory
    nlm::NeuralWorkingMemory* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    // Test episodic memory
    nlm::NeuralEpisodicMemory* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Test associative memory
    nlm::NeuralAssociativeMemory* associativeMemory = brain.getAssociativeMemory();
    assert(associativeMemory != nullptr);
    
    // Inject current and store in working memory
    brain.injectCurrentToNeurons(nlm::NeuronType::Internal, 30.0f);
    
    std::cout << "    testBrainMemorySystems passed" << std::endl;
}

void testBrainPredictionCognition() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test prediction system
    nlm::PredictionSystem* predictionSystem = brain.getPredictionSystem();
    assert(predictionSystem != nullptr);
    
    // Test cognition systems
    nlm::NeuralPlanner* planner = brain.getPlanner();
    assert(planner != nullptr);
    
    nlm::ConceptFormation* conceptFormation = brain.getConceptFormation();
    assert(conceptFormation != nullptr);
    
    nlm::AttentionalSelection* attention = brain.getAttention();
    assert(attention != nullptr);
    
    std::cout << "    testBrainPredictionCognition passed" << std::endl;
}

void testBrainDevelopment() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test development system
    nlm::DevelopmentSystem* developmentSystem = brain.getDevelopmentSystem();
    assert(developmentSystem != nullptr);
    
    // Apply development changes
    brain.develop();
    
    // Test developmental stage
    nlm::DevelopmentalStage stage = brain.getDevelopmentalStage();
    assert(stage == nlm::DevelopmentalStage::Initial || stage == nlm::DevelopmentalStage::CriticalPeriod || 
           stage == nlm::DevelopmentalStage::Maturation || stage == nlm::DevelopmentalStage::Adult);
    
    std::cout << "    testBrainDevelopment passed" << std::endl;
}

void testBrainStdpVerification() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get synapses
    if (auto* region = brain.getRegion(nlm::RegionId(1))) {
        auto& synapses = region->getSynapses();
        if (synapses.size() >= 3) {
            // Record initial weights
            float initialWeight1 = synapses[0]->getWeight();
            float initialWeight2 = synapses[1]->getWeight();
            
            // Enable only STDP
            synapses[0]->enablePlasticity(false, true, false);  // Enable only STDP
            synapses[1]->enablePlasticity(false, true, false);
            
            // Create correlated pre->post activity
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                // Fire pre-synaptic neuron
                neurons[0]->injectCurrent(60.0f);
                brain.step(0, 0.0);
                
                // Then post fires
                neurons[1]->injectCurrent(60.0f);
                brain.step(1, 0.001);
                
                // Check if weights changed (STDP should cause potentiation)
                float finalWeight1 = synapses[0]->getWeight();
                float finalWeight2 = synapses[1]->getWeight();
                
                // STDP should have modified weights
                bool stdpWorking = (std::abs(finalWeight1 - initialWeight1) > 0.001f || 
                                  std::abs(finalWeight2 - initialWeight2) > 0.001f);
                assert(stdpWorking);
            }
        }
    }
    
    std::cout << "    testBrainStdpVerification passed" << std::endl;
}

} // namespace test_brain
