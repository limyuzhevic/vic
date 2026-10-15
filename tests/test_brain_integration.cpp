// Brain System Integration Test
// Tests the complete loop: WORLD → SENSORY → NEURAL → MOTOR → WORLD
// Integration test for core brain functionality with world interaction

#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "sensory/Vision.hpp"
#include "sensory/Audio.hpp"
#include "sensory/InternalSignals.hpp"
#include "motor/Action.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace test_brain_integration {

void testCompleteBrainWorldLoop() {
    std::cout << "    Testing complete brain-world loop..." << std::endl;
    
    // Create config for brain with enough neurons
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.15, nlm::ConfigSource::Runtime);
    
    // Create world
    nlm::SimpleWorld world;
    world.configure(100.0f, 100.0f, 32, 32);  // 100x100 world, 32x32 vision
    world.setAgentStart(50.0f, 50.0f);
    
    // Create brain
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Set brain developmental stage to adult
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
    
    // Run simulation loop with sensory input and motor output
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        double currentTime = world.getSimulationTime();
        
        // Get sensory percept from world
        const nlm::SensoryPercept& percept = world.getSensoryPercept();
        
        // Create vision input from percept
        nlm::Vision vision;
        vision.configure(percept.getVisionWidth(), percept.getVisionHeight());
        std::vector<float> visionData = percept.getVision();
        vision.setData(visionData);
        
        // Create internal signals from percept
        nlm::InternalSignals internalSignals;
        std::vector<float> internalData = percept.getInternal();
        for (float value : internalData) {
            internalSignals.addSignal(value);
        }
        
        // Apply sensory input to brain
        brain.receiveSensoryInput(vision);
        brain.receiveSensoryInput(internalSignals);
        
        // Apply neuromodulation (dopamine reward prediction)
        nlm::Dopamine dopamine;
        dopamine.signalReward(0.1f);  // Small reward for successful actions
        brain.applyNeuromodulation(dopamine);
        
        // Brain step
        brain.step(step, currentTime);
        
        // Get action from brain
        auto action = brain.produceAction();
        
        // Apply action to world if action exists
        if (action) {
            nlm::MotorCommand motorCmd;
            motorCmd.type = action->getType();
            std::vector<float> params = action->getParameters();
            
            // Apply motor command to world
            nlm::ActionResult result = world.applyMotorCommand(motorCmd, currentTime);
            
            // Log if action was successful
            if (result.success) {
                // Store reward in episodic memory
                nlm::EpisodicMemoryItem episode;
                episode.timestamp = step;
                episode.metadata = "successful_action";
                episode.value = result.reward;
                if (auto* episodicMem = brain.getEpisodicMemory()) {
                    episodicMem->storeEpisode(episode);
                }
            }
        }
        
        // Update world
        world.update(0.1);  // 0.1 timestep
        
        // Check for developmental changes
        brain.develop();
        
        // Verify system consistency
        assert(brain.getTotalNeuronCount() == 500);
        assert(brain.getRegionCount() == 3);
        assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Adult);
    }
    
    std::cout << "    testCompleteBrainWorldLoop passed" << std::endl;
}

void testMemoryNeuromodulationIntegration() {
    std::cout << "    Testing memory-neuromodulation integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Initialize neuromodulators
    nlm::Dopamine* dopamine = brain.getDopamine();
    nlm::Curiosity* curiosity = brain.getCuriosity();
    nlm::Novelty* novelty = brain.getNovelty();
    nlm::PredictionError* predError = brain.getPredictionErrorSignal();
    
    // Initialize memory systems
    nlm::NeuralWorkingMemory* workingMem = brain.getWorkingMemory();
    nlm::NeuralEpisodicMemory* episodicMem = brain.getEpisodicMemory();
    nlm::NeuralAssociativeMemory* assocMem = brain.getAssociativeMemory();
    
    // Create test neurons
    nlm::NeuronId neuron1 = 1, neuron2 = 2, neuron3 = 3;
    
    // Test integration: Store in working memory, update with neuromodulation
    if (workingMem) {
        workingMem->store(neuron1, 0.5f);
        assert(workingMem->contains(neuron1));
        assert(workingMem->retrieve(neuron1) == 0.5f);
    }
    
    // Apply reward-based neuromodulation
    if (dopamine) {
        dopamine->signalReward(1.0f);
        float plasticityFactor = dopamine->getPlasticityFactor();
        assert(plasticityFactor >= 0.0f);
    }
    
    // Apply curiosity-based learning
    if (curiosity) {
        curiosity->setLevel(0.7f);
        curiosity->update(0.1f);
    }
    
    // Apply novelty-based plasticity
    if (novelty) {
        novelty->setLevel(0.8f);
    }
    
    // Create episodic memory
    nlm::EpisodicMemoryItem episode;
    episode.timestamp = 10;
    episode.neurons = {neuron1, neuron2};
    episode.values = {0.5f, 0.7f};
    episode.metadata = "reward_learning";
    
    if (episodicMem) {
        episodicMem->storeEpisode(episode);
        assert(episodicMem->getEpisodeCount() == 1);
        
        auto recent = episodicMem->getRecentEpisodes(1);
        assert(recent.size() == 1);
        assert(recent[0].metadata == "reward_learning");
    }
    
    // Create associative memory
    if (assocMem) {
        assocMem->associate(neuron1, neuron2, 0.9f);
        std::vector<nlm::NeuronId> associations = assocMem->getAssociations(neuron1);
        assert(associations.size() == 1);
        assert(associations[0] == neuron2);
        assert(assocMem->getAssociationStrength(neuron1, neuron2) == 0.9f);
    }
    
    // Test integration: Use prediction system
    nlm::PredictionSystem* predictionSys = brain.getPredictionSystem();
    if (predictionSys) {
        predictionSys->update(brain.getTotalNeuronCount(), brain.getAverageFiringRate());
    }
    
    // Test integration: Use concept formation
    nlm::ConceptFormation* conceptForm = brain.getConceptFormation();
    if (conceptForm) {
        conceptForm->update(brain.getTotalNeuronCount(), brain.getFiringNeuronCount());
    }
    
    std::cout << "    testMemoryNeuromodulationIntegration passed" << std::endl;
}

void testPredictionCognitionIntegration() {
    std::cout << "    Testing prediction-cognition integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::NeuralPlanner* planner = brain.getPlanner();
    nlm::ConceptFormation* conceptForm = brain.getConceptFormation();
    nlm::PredictionSystem* predictionSys = brain.getPredictionSystem();
    
    // Run simulation with prediction and cognition
    for (nlm::SimulationStep step = 0; step < 30; ++step) {
        // Get current neural state
        size_t totalNeurons = brain.getTotalNeuronCount();
        float avgFiringRate = brain.getAverageFiringRate();
        
        // Update prediction system with current state
        if (predictionSys) {
            predictionSys->update(totalNeurons, avgFiringRate);
        }
        
        // Get prediction error
        nlm::PredictionError* predError = brain.getPredictionErrorSignal();
        if (predError) {
            float error = predError->getPredictionError();
            // Use error to drive learning
            if (error > 0.5f) {
                brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, error * 10.0f);
            }
        }
        
        // Update concept formation
        if (conceptForm) {
            conceptForm->update(totalNeurons, brain.getFiringNeuronCount());
        }
        
        // Use planner for action selection
        if (planner) {
            planner->update(brain.getTotalNeuronCount(), avgFiringRate);
            auto plannedActions = planner->getPlan();
            if (!plannedActions.empty()) {
                // Execute first planned action
                auto action = brain.produceAction();
                if (action) {
                    // Concepts influence action parameters
                    if (conceptForm) {
                        auto concepts = conceptForm->getActiveConcepts();
                        if (!concepts.empty()) {
                            // Modify action parameters based on concepts
                            std::vector<float> params = action->getParameters();
                            for (auto& param : params) {
                                param *= 1.1f;  // Scale parameter based on concepts
                            }
                            action->setParameters(params);
                        }
                    }
                }
            }
        }
        
        brain.step(step);
        
        // Verify integration
        assert(brain.getTotalNeuronCount() == 400);
        assert(brain.getRegionCount() == 2);
    }
    
    std::cout << "    testPredictionCognitionIntegration passed" << std::endl;
}

void testDevelopmentLearningIntegration() {
    std::cout << "    Testing development-learning integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(350), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::DevelopmentSystem* devSystem = brain.getDevelopmentSystem();
    
    // Test integration across developmental stages
    nlm::DevelopmentalStage currentStage = nlm::DevelopmentalStage::Juvenile;
    
    // Test progression through developmental stages
    for (nlm::DevelopmentalStage stage = nlm::DevelopmentalStage::Larval;
         stage <= nlm::DevelopmentalStage::Adult;
         static_cast<decltype(stage) &>(stage) = static_cast<nlm::DevelopmentalStage>(static_cast<int>(stage) + 1)) {
        
        brain.setDevelopmentalStage(stage);
        assert(brain.getDevelopmentalStage() == stage);
        
        // Update development system
        if (devSystem) {
            float plasticityModifier = devSystem->getPlasticityModifier();
            bool isCriticalPeriod = devSystem->isCriticalPeriod();
            
            // Adapt learning based on developmental stage
            float learningRate = isCriticalPeriod ? 0.5f : 0.1f;
            learningRate *= plasticityModifier;
            
            // Apply plasticity based on developmental stage
            if (stage == nlm::DevelopmentalStage::Juvenile) {
                // High plasticity during juvenile stage
                brain.injectCurrentToNeurons(nlm::NeuronType::Plastic, learningRate * 100.0f);
            } else if (stage == nlm::DevelopmentalStage::Adult) {
                // Lower plasticity in adult stage
                brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, learningRate * 50.0f);
            }
        }
        
        // Run brain steps at each stage
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
            brain.develop();  // Apply developmental changes
            
            assert(brain.getTotalNeuronCount() == 350);
            assert(brain.getRegionCount() == 2);
        }
    }
    
    // Test developmental plasticity effects
    nlm::DevelopmentSystem devSystem2;
    devSystem2.setStage(nlm::DevelopmentalStage::Juvenile);
    assert(devSystem2.getStage() == nlm::DevelopmentalStage::Juvenile);
    assert(devSystem2.getPlasticityModifier() > 1.0f);
    
    devSystem2.setStage(nlm::DevelopmentalStage::Adult);
    assert(devSystem2.getStage() == nlm::DevelopmentalStage::Adult);
    assert(devSystem2.getPlasticityModifier() < 1.0f);
    
    std::cout << "    testDevelopmentLearningIntegration passed" << std::endl;
}

void testCrossSubsystemCoordination() {
    std::cout << "    Testing cross-subsystem coordination..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(450), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::DevelopmentSystem* devSystem = brain.getDevelopmentSystem();
    nlm::PredictionSystem* predictionSys = brain.getPredictionSystem();
    nlm::Dopamine* dopamine = brain.getDopamine();
    
    // Coordinated operation of multiple subsystems
    for (nlm::SimulationStep step = 0; step < 40; ++step) {
        // Development system updates neural connectivity
        if (devSystem) {
            devSystem->update(&brain, step);
        }
        
        // Prediction system generates expectations
        if (predictionSys) {
            predictionSys->update(brain.getTotalNeuronCount(), brain.getAverageFiringRate());
        }
        
        // Neuromodulation coordinates learning and attention
        if (dopamine) {
            // Use prediction error to drive dopamine release
            if (predictionSys) {
                float predictionError = predictionSys->getPredictionError();
                dopamine->signalRewardPredictionError(predictionError);
            }
            dopamine->update(0.1f);
        }
        
        // Brain processes all inputs and updates state
        brain.step(step);
        brain.updatePlasticity();  // Apply plasticity changes
        brain.develop();           // Apply developmental changes
        
        // Verify coordinated behavior
        assert(brain.getTotalNeuronCount() == 450);
        assert(brain.getRegionCount() == 3);
        
        // Check that all systems are working together
        if (devSystem) {
            assert(devSystem->getDevelopmentalAge() > 0.0);
        }
    }
    
    // Test attentional coordination
    nlm::AttentionalSelection* attention = brain.getAttention();
    if (attention) {
        attention->setPriority(nlm::NeuronType::Sensory, 0.8f);
        attention->setPriority(nlm::NeuronType::Motor, 0.6f);
        attention->setPriority(nlm::NeuronType::Plastic, 0.4f);
    }
    
    std::cout << "    testCrossSubsystemCoordination passed" << std::endl;
}

void runAll() {
    testCompleteBrainWorldLoop();
    testMemoryNeuromodulationIntegration();
    testPredictionCognitionIntegration();
    testDevelopmentLearningIntegration();
    testCrossSubsystemCoordination();
}

} // namespace test_brain_integration