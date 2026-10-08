#include "SimulationController.hpp"
#include "../core/Logger/Logger.hpp"
#include <stdexcept>

namespace nlm {

SimulationController::SimulationController(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
    : brain(brain)
    , config(config)
    , clock(std::make_unique<SimulationClock>(config->getOr<double>("simulation_timestep", 0.001)))
    , logger(std::make_unique<Logger>())
    , currentStep(0)
    , currentTime(0.0)
    , totalSpikesThisStep(0) {
    
    if (!brain) {
        throw std::invalid_argument("Brain pointer cannot be null");
    }
    if (!config) {
        throw std::invalid_argument("Config pointer cannot be null");
    }
    
    // Setup logger
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
}

void SimulationController::step(SimulationStep currentStep, Timestamp currentTime) {
    this->currentStep = currentStep;
    this->currentTime = currentTime;
    totalSpikesThisStep = 0;
    
    try {
        // Execute all simulation steps in order
        executeStep1_SpikeDelivery();
        executeStep2_NeuronUpdate();
        executeStep3_SpikeDetection();
        executeStep4_WorkingMemory();
        executeStep5_Neuromodulation();
        executeStep6_Plasticity();
        executeStep7_EpisodicMemory();
        executeStep8_PredictionSystem();
        executeStep9_Attention();
        executeStep10_ConceptFormation();
        executeStep11_StructuralPlasticity();
        executeStep12_MemoryReplay();
        executeStep13_Development();
        executeStep14_Consolidation();
        executeStep15_Checkpoint();
        
    } catch (const std::exception& e) {
        Logger::get()->error("Simulation step failed: " + std::string(e.what()));
        throw;
    }
}

void SimulationController::executeStep1_SpikeDelivery() {
    // Process pending delayed spike events (deliver synaptic input)
    if (brain->getSpikeSystem()) {
        brain->getSpikeSystem()->processDelayedSpikes(currentStep, currentTime);
    }
}

void SimulationController::executeStep2_NeuronUpdate() {
    // Update all neurons (LIF dynamics)
    for (const auto& region : brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, clock->getTimestep());
            }
        }
    }
}

void SimulationController::executeStep3_SpikeDetection() {
    // Detect spikes and schedule spike events
    for (const auto& region : brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < clock->getTimestep() * 2.0f);

                if (justFired) {
                    // Neuron fired - queue spike and record post-synaptic activity
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    if (brain->getSpikeSystem()) {
                        brain->getSpikeSystem()->queueSpike(event);
                    }

                    // Record post-synaptic spike for incoming synapses
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(currentTime);
                    }

                    totalSpikesThisStep++;
                }
            }
        }
    }
    
    if (brain->getSpikeSystem()) {
        brain->getSpikeSystem()->processSpikes(currentStep);
    }
}

void SimulationController::executeStep4_WorkingMemory() {
    // Update working memory
    if (brain->getWorkingMemory()) {
        brain->getWorkingMemory()->update(clock->getTimestep());
    }
}

void SimulationController::executeStep5_Neuromodulation() {
    // Update neuromodulation systems
    if (brain->getDopamine()) {
        brain->getDopamine()->update(clock->getTimestep());
    }
    if (brain->getCuriosity()) {
        brain->getCuriosity()->update(clock->getTimestep());
    }
    if (brain->getNovelty()) {
        brain->getNovelty()->update(clock->getTimestep());
    }
}

void SimulationController::executeStep6_Plasticity() {
    // Apply plasticity rules (STDP and Hebbian)
    float plasticityMod = 1.0f;
    if (brain->getDopamine()) {
        plasticityMod = brain->getDopamine()->getPlasticityFactor();
    }
    
    for (const auto& region : brain->getRegions()) {
        for (Synapse* syn : region->getSynapses()) {
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    if (brain->getSTDP()) {
                        brain->getSTDP()->update(syn, preSpikes, postSpikes, clock->getTimestep());
                    }
                }
            }
            
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    if (brain->getHebbian()) {
                        brain->getHebbian()->update(syn, preSpikes, postSpikes, clock->getTimestep());
                    }
                }
            }
            
            syn->step(currentTime);
        }
    }
}

void SimulationController::executeStep7_EpisodicMemory() {
    // Update episodic memory with current experience
    static size_t stepsSinceLastEpisode = 0;
    stepsSinceLastEpisode++;
    
    if (stepsSinceLastEpisode >= 10 && brain->getEpisodicMemory()) {
        stepsSinceLastEpisode = 0;
        
        EpisodicMemoryItem episode;
        episode.timestamp = currentStep;
        episode.reward = brain->getDopamine() ? brain->getDopamine()->getLevel() : 0.0f;
        
        // Store active neurons
        for (const auto& region : brain->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring() || 
                        std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                        episode.activeNeurons.push_back(neuron->getId());
                        episode.neuronActivations.push_back(
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                    }
                }
            }
        }
        
        brain->getEpisodicMemory()->storeEpisode(episode);
    }
}

void SimulationController::executeStep8_PredictionSystem() {
    // Update prediction system
    if (brain->getPredictionSystem()) {
        // Prediction system would be updated with sensory observations
    }
}

void SimulationController::executeStep9_Attention() {
    // Update attention system
    if (brain->getAttention()) {
        brain->getAttention()->update(clock->getTimestep());
    }
}

void SimulationController::executeStep10_ConceptFormation() {
    // Update concept formation
    if (brain->getConceptFormation()) {
        // Would process current neural activity patterns to form concepts
    }
}

void SimulationController::executeStep11_StructuralPlasticity() {
    // Apply structural plasticity periodically
    if (currentStep % 100 == 0 && brain->getStructuralPlasticity() && brain->getRandomGenerator()) {
        brain->getStructuralPlasticity()->update(brain.get(), *brain->getRandomGenerator());
    }
}

void SimulationController::executeStep12_MemoryReplay() {
    // Replay important memories periodically
    static size_t replayInterval = 100;
    if (currentStep % replayInterval == 0 && brain->getEpisodicMemory()) {
        auto episodesToReplay = brain->getEpisodicMemory()->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            brain->getEpisodicMemory()->replayEpisode(episode);
        }
    }
}

void SimulationController::executeStep13_Development() {
    // Apply development effects periodically
    if (currentStep % 1000 == 0 && brain->getDevelopmentSystem() && brain->getRandomGenerator()) {
        brain->getDevelopmentSystem()->update(brain.get(), *brain->getRandomGenerator(), clock->getTimestep() * 1000);
    }
}

void SimulationController::executeStep14_Consolidation() {
    // Periodic memory consolidation
    static size_t consolidationInterval = 1000;
    if (currentStep % consolidationInterval == 0 && brain->getEpisodicMemory()) {
        brain->getEpisodicMemory()->consolidate(0.3f);
    }
}

void SimulationController::executeStep15_Checkpoint() {
    // Checkpoint management
    if (brain->getSpikeSystem()) {
        brain->getSpikeSystem()->reset(); // Reset for next step
    }
}

} // namespace nlm