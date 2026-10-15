// Cross-Subsystem Coordination Test
// Tests coordination between multiple brain subsystems
// Validates complex interactions between development, memory, prediction, and neuromodulation

#include "brain/Brain.hpp"
#include "development/DevelopmentSystem.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "memory/AssociativeMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/STDP.hpp"
#include "world/SimpleWorld.hpp"
#include "sensory/Vision.hpp"
#include "sensory/InternalSignals.hpp"
#include "motor/Action.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <memory>
#include <thread>
#include <chrono>

namespace test_cross_subsystem_coordination {

void testMultiSubsystemCoordination() {
    std::cout << "    Testing multi-subsystem coordination..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // All subsystems that will coordinate
    nlm::DevelopmentSystem* devSystem = brain.getDevelopmentSystem();
    nlm::WorkingMemory* workingMem = brain.getWorkingMemory();
    nlm::EpisodicMemory* episodicMem = brain.getEpisodicMemory();
    nlm::AssociativeMemory* assocMem = brain.getAssociativeMemory();
    nlm::PredictionSystem* predictionSys = brain.getPredictionSystem();
    nlm::NeuralPlanner* planner = brain.getPlanner();
    nlm::ConceptFormation* conceptForm = brain.getConceptFormation();
    nlm::Dopamine* dopamine = brain.getDopamine();
    nlm::Curiosity* curiosity = brain.getCuriosity();
    nlm::PredictionError* predError = brain.getPredictionErrorSignal();
    nlm::Hebbian* hebbian = brain.getHebbian();
    nlm::STDP* stdp = brain.getSTDP();
    
    // Complex coordination loop
    for (nlm::SimulationStep step = 0; step < 45; ++step) {
        double currentTime = step * 0.1;
        
        // 1. Development system sets the stage
        if (devSystem) {
            devSystem->update(&brain, step);
            
            // Development influences plasticity
            if (hebbian) {
                float devPlasticity = devSystem->getPlasticityModifier();
                hebbian->setLearningRate(devPlasticity * 0.1f);
            }
            if (stdp) {
                float devPlasticity = devSystem->getPlasticityModifier();
                stdp->setLearningRate(devPlasticity * 0.1f);
            }
        }
        
        // 2. Memory systems store and retrieve experiences
        if (workingMem && episodicMem) {
            // Store working memory
            workingMem->store(step, 0.5f + 0.3f * std::sin(step * 0.2f));
            
            // Create episodic memory from complex experience
            nlm::EpisodicMemoryItem episode;
            episode.timestamp = step;
            episode.neurons = {static_cast<nlm::NeuronId>(100 + step), static_cast<nlm::NeuronId>(200 + step)};
            episode.values = {workingMem->retrieve(step), 0.7f};
            episode.metadata = "coordinated_learning_step_" + std::to_string(step);
            
            episodicMem->storeEpisode(episode);
        }
        
        // 3. Associative memory learns relationships
        if (assocMem && workingMem) {
            // Learn associations between current step and memory content
            float currentMemory = workingMem->retrieve(step);
            assocMem->associate(step % 50, step, currentMemory);
        }
        
        // 4. Prediction system makes predictions
        if (predictionSys) {
            predictionSys->update(brain.getTotalNeuronCount(), brain.getAverageFiringRate());
            
            // Use episodic memory to improve predictions
            if (episodicMem && predError) {
                auto recentEpisodes = episodicMem->getRecentEpisodes(3);
                float memoryInfluence = std::min(1.0f, recentEpisodes.size() * 0.2f);
                predictionSys->update(brain.getTotalNeuronCount(), brain.getAverageFiringRate(), memoryInfluence);
            }
        }
        
        // 5. Neuromodulation coordinates learning
        if (dopamine && curiosity && predError) {
            // Dopamine from reward
            dopamine->signalReward(0.3f + 0.2f * std::cos(step * 0.3f));
            
            // Curiosity drives exploration
            curiosity->setLevel(0.4f + 0.3f * std::sin(step * 0.1f));
            curiosity->update(0.1f);
            
            // Prediction error drives learning
            predError->setPredictionError(std::abs(std::sin(step * 0.15f)));
            
            // Apply neuromodulation to brain
            brain.applyNeuromodulation(*dopamine);
            brain.applyNeuromodulation(*curiosity);
        }
        
        // 6. Concept formation discovers patterns
        if (conceptForm) {
            conceptForm->update(brain.getTotalNeuronCount(), brain.getFiringNeuronCount());
            
            // Concepts influence prediction
            if (predictionSys) {
                auto concepts = conceptForm->getActiveConcepts();
                if (!concepts.empty()) {
                    float conceptInfluence = std::min(0.8f, concepts.size() * 0.1f);
                    predictionSys->update(brain.getTotalNeuronCount(), brain.getAverageFiringRate(), conceptInfluence);
                }
            }
        }
        
        // 7. Neural planner creates action plans
        if (planner) {
            planner->update(brain.getTotalNeuronCount(), brain.getAverageFiringRate());
            
            // Plans can be influenced by concepts
            if (conceptForm && predictionSys) {
                auto concepts = conceptForm->getActiveConcepts();
                float predictionError = predictionSys->getPredictionError();
                
                // Adjust planning based on concepts and prediction error
                planner->update(brain.getTotalNeuronCount(), brain.getAverageFiringRate(), 
                               predictionError, concepts.size() * 0.05f);
            }
            
            // Create actions based on plan
            auto plan = planner->getPlan();
            if (!plan.empty()) {
                auto action = brain.produceAction();
                if (action) {
                    // Modify action based on all subsystems
                    std::vector<float> params = action->getParameters();
                    
                    // Incorporate memory influence
                    if (workingMem) {
                        float memoryVal = workingMem->retrieve(step % 50);
                        for (auto& param : params) {
                            param *= (1.0f + memoryVal * 0.1f);
                        }
                    }
                    
                    // Incorporate concept influence
                    if (conceptForm) {
                        auto concepts = conceptForm->getActiveConcepts();
                        float conceptVal = std::min(1.0f, concepts.size() * 0.1f);
                        for (auto& param : params) {
                            param *= (1.0f + conceptVal);
                        }
                    }
                    
                    action->setParameters(params);
                }
            }
        }
        
        // 8. Plasticity updates synaptic weights
        if (hebbian) {
            hebbian->update(brain.getTotalNeuronCount(), brain.getAverageFiringRate());
        }
        if (stdp) {
            stdp->update(brain.getTotalNeuronCount(), brain.getAverageFiringRate());
        }
        
        // 9. Brain step integrates all subsystems
        brain.step(step, currentTime);
        brain.updatePlasticity();
        
        // 10. Verify system coordination
        assert(brain.getTotalNeuronCount() == 500);
        assert(brain.getRegionCount() == 4);
        
        // Check that all subsystems are active
        if (devSystem) {
            assert(devSystem->getDevelopmentalAge() > 0.0);
        }
        if (workingMem) {
            assert(workingMem->getCurrentSize() >= 0);
        }
        if (predictionSys) {
            assert(predictionSys->getPredictionError() >= 0.0f);
        }
    }
    
    std::cout << "    testMultiSubsystemCoordination passed" << std::endl;
}

void testRealWorldScenario() {
    std::cout << "    Testing real-world scenario..." << endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(450), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Runtime);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    nlm::SimpleWorld world;
    world.configure(100.0f, 100.0f, 32, 32);
    world.setAgentStart(50.0f, 50.0f);
    
    // Create world objects (resources and hazards)
    for (int i = 0; i < 10; ++i) {
        nlm::WorldObject resource;
        resource.x = 20.0f + (i * 8.0f);
        resource.y = 30.0f + (i * 3.0f);
        resource.type = nlm::WorldObjectType::Resource;
        resource.value = 10.0f;
        resource.active = true;
        world.addObject(resource);
        
        nlm::WorldObject hazard;
        hazard.x = 70.0f + (i * 8.0f);
        hazard.y = 70.0f + (i * 3.0f);
        hazard.type = nlm::WorldObjectType::Hazard;
        hazard.value = -5.0f;
        hazard.active = true;
        world.addObject(hazard);
    }
    
    nlm::DevelopmentSystem* devSystem = brain.getDevelopmentSystem();
    nlm::WorkingMemory* workingMem = brain.getWorkingMemory();
    nlm::EpisodicMemory* episodicMem = brain.getEpisodicMemory();
    nlm::AssociativeMemory* assocMem = brain.getAssociativeMemory();
    nlm::PredictionSystem* predictionSys = brain.getPredictionSystem();
    nlm::NeuralPlanner* planner = brain.getPlanner();
    nlm::ConceptFormation* conceptForm = brain.getConceptFormation();
    nlm::Dopamine* dopamine = brain.getDopamine();
    nlm::Curiosity* curiosity = brain.getCuriosity();
    nlm::PredictionError* predError = brain.getPredictionErrorSignal();
    
    // Simulate agent learning in the world
    for (nlm::SimulationStep step = 0; step < 60; ++step) {
        double currentTime = world.getSimulationTime();
        
        // Get sensory input from world
        const nlm::SensoryPercept& percept = world.getSensoryPercept();
        
        // Create sensory inputs
        nlm::Vision vision;
        vision.configure(percept.getVisionWidth(), percept.getVisionHeight());
        std::vector<float> visionData = percept.getVision();
        vision.setData(visionData);
        
        nlm::InternalSignals internalSignals;
        std::vector<float> internalData = percept.getInternal();
        for (float value : internalData) {
            internalSignals.addSignal(value);
        }
        
        // Apply sensory input to brain
        brain.receiveSensoryInput(vision);
        brain.receiveSensoryInput(internalSignals);
        
        // Development system adapts based on environmental interaction
        if (devSystem) {
            devSystem->update(&brain, step);
            
            // Adjust learning rate based on environment
            float plasticity = devSystem->getPlasticityModifier();
            if (step < 20) {
                // Early development: high plasticity
                plasticity *= 2.0f;
            }
        }
        
        // Memory systems store environmental experiences
        if (workingMem && episodicMem) {
            // Store current perception
            workingMem->store(step, percept.getVision()[0]);
            
            // Create episodic memory of environmental state
            nlm::EpisodicMemoryItem episode;
            episode.timestamp = step;
            episode.neurons = {100, 200, 300};
            episode.values = {workingMem->retrieve(step), 0.5f, 0.8f};
            episode.metadata = "environmental_interaction_" + std::to_string(step);
            
            episodicMem->storeEpisode(episode);
        }
        
        // Prediction system predicts environmental outcomes
        if (predictionSys) {
            predictionSys->update(brain.getTotalNeuronCount(), brain.getAverageFiringRate());
            
            // Prediction error drives learning
            if (predError) {
                float error = predictionSys->getPredictionError();
                predError->setPredictionError(error);
            }
        }
        
        // Neuromodulation coordinates reward and exploration
        if (dopamine && curiosity) {
            // Reward based on finding resources and avoiding hazards
            int resourcesFound = 0;
            int hazardsAvoided = 0;
            
            // Check for nearby objects in vision
            std::vector<float> vision = percept.getVision();
            for (size_t i = 0; i < vision.size(); ++i) {
                if (vision[i] > 0.7f) {
                    // Likely a resource or hazard
                    if (i % 2 == 0) {
                        resourcesFound++;
                    } else {
                        hazardsAvoided++;
                    }
                }
            }
            
            float reward = (resourcesFound * 10.0f - hazardsAvoided * 5.0f) / 100.0f;
            dopamine->signalReward(std::max(0.0f, reward));
            
            // Curiosity drives exploration of unknown areas
            curiosity->setLevel(0.3f + 0.4f * std::sin(step * 0.2f));
            curiosity->update(0.1f);
            
            // Apply neuromodulation
            brain.applyNeuromodulation(*dopamine);
            brain.applyNeuromodulation(*curiosity);
        }
        
        // Concept formation learns environmental patterns
        if (conceptForm) {
            conceptForm->update(brain.getTotalNeuronCount(), brain.getFiringNeuronCount());
        }
        
        // Neural planner creates action plans
        if (planner) {
            planner->update(brain.getTotalNeuronCount(), brain.getAverageFiringRate());
            
            // Use prediction error to refine plans
            if (predError) {
                float error = predError->getPredictionError();
                planner->update(brain.getTotalNeuronCount(), brain.getAverageFiringRate(), error);
            }
            
            // Execute planned actions
            auto plan = planner->getPlan();
            if (!plan.empty()) {
                auto action = brain.produceAction();
                if (action) {
                    // Apply action to world
                    nlm::MotorCommand motorCmd;
                    motorCmd.type = action->getType();
                    std::vector<float> params = action->getParameters();
                    
                    // Modify action parameters based on learned concepts
                    if (conceptForm) {
                        auto concepts = conceptForm->getActiveConcepts();
                        for (size_t i = 0; i < params.size() && i < concepts.size(); ++i) {
                            params[i] *= (1.0f + concepts[i].size() * 0.05f);
                        }
                    }
                    
                    nlm::ActionResult result = world.applyMotorCommand(motorCmd, currentTime);
                    
                    // Store successful actions in episodic memory
                    if (result.success && episodicMem) {
                        nlm::EpisodicMemoryItem episode;
                        episode.timestamp = step;
                        episode.neurons = {400, 500};
                        episode.values = {result.reward, 0.9f};
                        episode.metadata = "successful_action";
                        episodicMem->storeEpisode(episode);
                    }
                }
            }
        }
        
        // Plasticity updates based on experience
        if (hebbian) {
            hebbian->update(brain.getTotalNeuronCount(), brain.getAverageFiringRate());
        }
        if (stdp) {
            stdp->update(brain.getTotalNeuronCount(), brain.getAverageFiringRate());
        }
        
        // Step brain and world
        brain.step(step, currentTime);
        brain.updatePlasticity();
        brain.develop();
        
        world.update(0.1);
        
        // Verify system state
        assert(brain.getTotalNeuronCount() == 450);
        assert(brain.getRegionCount() == 3);
        
        // Check learning progress
        if (workingMem) {
            assert(workingMem->getCurrentSize() >= 0);
        }
    }
    
    std::cout << "    testRealWorldScenario passed" << std::endl;
}

void runAll() {
    testMultiSubsystemCoordination();
    testRealWorldScenario();
}

} // namespace test_cross_subsystem_coordination