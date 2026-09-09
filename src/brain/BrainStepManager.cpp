#include "BrainStepManager.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

struct BrainStepManager::Impl {
    Brain* brain;
    Config config;
};

BrainStepManager::BrainStepManager(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("BrainStepManager: invalid brain pointer");
        return;
    }
    
    brain_ = brain;
    pImpl = std::make_unique<Impl>();
    pImpl->brain = brain;
    totalSpikesThisStep_ = 0;
    totalSpikesTotal_ = 0;
    currentStep_ = 0;
    currentTime_ = 0.0;
}

BrainStepManager::~BrainStepManager() = default;

void BrainStepManager::executeStep(SimulationStep currentStep, Timestamp currentTime) {
    // Store current state
    currentStep_ = currentStep;
    currentTime_ = currentTime;
    totalSpikesThisStep_ = 0;
    
    // Execute the complete 15-step brain loop
    NLM_LOG_DEBUG("Executing 15-step brain loop");
    
    executeStepPhase1(currentStep, currentTime);
    executeStepPhase2(currentStep, currentTime);
    executeStepPhase3(currentStep, currentTime);
    executeStepPhase4(currentStep, currentTime);
    executeStepPhase5(currentStep, currentTime);
    executeStepPhase6(currentStep, currentTime);
    executeStepPhase7(currentStep, currentTime);
    executeStepPhase8(currentStep, currentTime);
    executeStepPhase9(currentStep, currentTime);
    executeStepPhase10(currentStep, currentTime);
    executeStepPhase11(currentStep, currentTime);
    executeStepPhase12(currentStep, currentTime);
    executeStepPhase13(currentStep, currentTime);
    executeStepPhase14(currentStep, currentTime);
    executeStepPhase15(currentStep, currentTime);
    
    // Update total spike count
    totalSpikesTotal_ += totalSpikesThisStep_;
    
    NLM_LOG_DEBUG("15-step brain loop completed (" + std::to_string(totalSpikesThisStep_) + " spikes this step)");
}

void BrainStepManager::executeStepPhase1(SimulationStep currentStep, Timestamp currentTime) {
    // Step 1: Process pending delayed spikes (deliver synaptic input)
    // This delegates to the SpikeSystem
    if (brain_->getSpikeSystem()) {
        brain_->getSpikeSystem()->processDelayedSpikes(currentStep, currentTime);
    }
}

void BrainStepManager::executeStepPhase2(SimulationStep currentStep, Timestamp currentTime) {
    // Step 2: Update all neurons (LIF dynamics)
    // This loops through all neurons and updates their membrane potentials
    if (brain_) {
        // For now, just log - actual implementation would need region population access
        NLM_LOG_DEBUG("Updating neuron membrane potentials (LIF dynamics)");
    }
}

void BrainStepManager::executeStepPhase3(SimulationStep currentStep, Timestamp currentTime) {
    // Step 3: Detect spikes and schedule spike events
    // This detects fired neurons and creates spike events
    if (brain_) {
        // For now, just log - actual implementation would need neuron state access
        NLM_LOG_DEBUG("Detecting spikes and scheduling spike events");
    }
}

void BrainStepManager::executeStepPhase4(SimulationStep currentStep, Timestamp currentTime) {
    // Step 4: Update working memory
    if (brain_ && brain_->getWorkingMemory()) {
        brain_->getWorkingMemory()->update(currentTime_);
    }
}

void BrainStepManager::executeStepPhase5(SimulationStep currentStep, Timestamp currentTime) {
    // Step 5: Apply neuromodulation effects
    if (brain_) {
        if (brain_->getNovelty()) {
            brain_->getNovelty()->update(currentTime_);
        }
        if (brain_->getCuriosity()) {
            brain_->getCuriosity()->update(currentTime_);
        }
        if (brain_->getDopamine()) {
            brain_->getDopamine()->update(currentTime_);
            
            // Apply dopamine effects on neural excitability
            float dopamineLevel = brain_->getDopamine()->getLevel();
            if (dopamineLevel > 0.0f) {
                float excitabilityMod = dopamineLevel * 0.5f;
                if (excitabilityMod > 0.0f) {
                    // TODO: Inject current into neurons
                }
            }
        }
    }
}

void BrainStepManager::executeStepPhase6(SimulationStep currentStep, Timestamp currentTime) {
    // Step 6: Apply plasticity rules (STDP and Hebbian)
    if (brain_) {
        // TODO: Implement plasticity application based on spike history
        NLM_LOG_DEBUG("Applying plasticity rules (STDP and Hebbian)");
    }
}

void BrainStepManager::executeStepPhase7(SimulationStep currentStep, Timestamp currentTime) {
    // Step 7: Update episodic memory
    if (brain_ && brain_->getEpisodicMemory()) {
        brain_->getEpisodicMemory()->update(currentTime_);
    }
}

void BrainStepManager::executeStepPhase8(SimulationStep currentStep, Timestamp currentTime) {
    // Step 8: Update prediction system
    // TODO: Implement prediction system update with sensory input
    NLM_LOG_DEBUG("Updating prediction system");
}

void BrainStepManager::executeStepPhase9(SimulationStep currentStep, Timestamp currentTime) {
    // Step 9: Update attention system
    if (brain_ && brain_->getAttention()) {
        brain_->getAttention()->update(currentTime_);
        
        // Apply attention to working memory winners
        if (brain_->getWorkingMemory() && !brain_->getWorkingMemory()->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = brain_->getWorkingMemory()->getMemoryNeurons();
            brain_->getAttention()->processCompetition(competitors);
        }
    }
}

void BrainStepManager::executeStepPhase10(SimulationStep currentStep, Timestamp currentTime) {
    // Step 10: Update concept formation
    // TODO: Implement concept formation update
    NLM_LOG_DEBUG("Updating concept formation");
}

void BrainStepManager::executeStepPhase11(SimulationStep currentStep, Timestamp currentTime) {
    // Step 11: Apply structural plasticity periodically
    if (brain_ && currentStep_ % 100 == 0) {
        if (brain_->getStructuralPlasticity()) {
            if (brain_->getRandomGenerator()) {
                brain_->getStructuralPlasticity()->update(brain_, *brain_->getRandomGenerator());
            }
        }
    }
}

void BrainStepManager::executeStepPhase12(SimulationStep currentStep, Timestamp currentTime) {
    // Step 12: Replay important memories
    if (brain_ && brain_->getEpisodicMemory() && currentStep_ % pImpl->config.getOr<size_t>("replay_interval", 100) == 0) {
        // Get episodes for replay
        auto episodesToReplay = brain_->getEpisodicMemory()->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            brain_->getEpisodicMemory()->replayEpisode(episode);
        }
    }
}

void BrainStepManager::executeStepPhase13(SimulationStep currentStep, Timestamp currentTime) {
    // Step 13: Apply development effects
    if (brain_ && currentStep_ % 1000 == 0) {  // Update development every 1000 steps
        if (brain_->getDevelopmentSystem()) {
            if (brain_->getRandomGenerator()) {
                brain_->getDevelopmentSystem()->update(brain_, *brain_->getRandomGenerator(), currentTime_ * 1000);
                
                // Development affects plasticity rates
                if (brain_->getStructuralPlasticity()) {
                    DevelopmentalStage stage = brain_->getDevelopmentalStage();
                    float plasticityMod = 1.0f;
                    
                    switch (stage) {
                        case DevelopmentalStage::Initial:
                            plasticityMod = 1.0f;  // High plasticity
                            break;
                        case DevelopmentalStage::CriticalPeriod:
                            plasticityMod = 0.8f;
                            break;
                        case DevelopmentalStage::Maturation:
                            plasticityMod = 0.5f;
                            break;
                        case DevelopmentalStage::Adult:
                            plasticityMod = 0.2f;  // Stable
                            break;
                        case DevelopmentalStage::Aging:
                            plasticityMod = 0.1f;
                            break;
                    }
                    
                    brain_->getStructuralPlasticity()->setSynaptogenesisRate(0.0001f * plasticityMod);
                    brain_->getStructuralPlasticity()->setPruningRate(0.00001f * (2.0f - plasticityMod));
                }
            }
        }
    }
}

void BrainStepManager::executeStepPhase14(SimulationStep currentStep, Timestamp currentTime) {
    // Step 14: Periodic memory consolidation
    if (brain_ && brain_->getEpisodicMemory() && currentStep_ % pImpl->config.getOr<size_t>("consolidation_interval", 1000) == 0) {
        // Consolidate important memories, remove weak ones
        brain_->getEpisodicMemory()->consolidate(0.3f);
    }
}

void BrainStepManager::executeStepPhase15(SimulationStep currentStep, Timestamp currentTime) {
    // Step 15: Checkpoint management
    if (brain_) {
        if (brain_->getSpikeSystem()) {
            brain_->getSpikeSystem()->reset();
        }
    }
}

void BrainStepManager::configureFromConfig(const Config& config) {
    pImpl->config = config;
    NLM_LOG_INFO("BrainStepManager configured from config");
}

void BrainStepManager::reset() {
    totalSpikesThisStep_ = 0;
    currentStep_ = 0;
    currentTime_ = 0.0;
    NLM_LOG_INFO("BrainStepManager reset");
}

} // namespace nlm